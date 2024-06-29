#include <octoon/pmx_importer.h>
#include <octoon/pmx.h>

#include <octoon/runtime/string.h>
#include <octoon/runtime/guid.h>

#include <octoon/material/mesh_standard_material.h>

#include <octoon/transform_component.h>
#include <octoon/solver_component.h>
#include <octoon/animator_component.h>
#include <octoon/skinned_morph_component.h>
#include <octoon/skinned_mesh_renderer_component.h>
#include <octoon/skinned_joint_renderer_component.h>
#include <octoon/rotation_limit_component.h>
#include <octoon/rigidbody_component.h>
#include <octoon/sphere_collider_component.h>
#include <octoon/box_collider_component.h>
#include <octoon/capsule_collider_component.h>
#include <octoon/configurable_joint_component.h>
#include <octoon/rotation_link_component.h>
#include <octoon/rotation_link_limit_component.h>
#include <octoon/cloth_component.h>
#include <octoon/asset_importer.h>
#include <octoon/asset_database.h>

#include <set>
#include <codecvt>
#include <fstream>

namespace octoon
{
	OctoonImplementSubClass(PMXImporter, AssetImporter, "PMXImporter")

	PMXImporter::PMXImporter() noexcept
	{
	}

	PMXImporter::~PMXImporter()
	{
	}

	void
	PMXImporter::createBones(AssetImporterContext& context, const PMX& pmx, GameObjects& bones) noexcept(false)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> cv;

		bones.reserve(pmx.bones.size());

		for (auto& it : pmx.bones)
			bones.emplace_back(std::make_shared<GameObject>((std::string_view)cv.to_bytes(it.name.name)));

		for (std::size_t i = 0; i < pmx.bones.size(); i++)
		{
			auto& it = pmx.bones[i];

			auto parent = it.Parent;
			if (parent >= 0 && parent < bones.size())
				bones[i]->setParent(bones[parent]);

			auto transform = bones[i]->getComponent<TransformComponent>();
			transform->setTranslate(math::float3(it.position.x, it.position.y, it.position.z));

			auto additiveParent = it.ProvidedParentBoneIndex;
			if (additiveParent >= 0 && additiveParent < bones.size())
			{
				auto limit = bones[i]->addComponent<RotationLinkLimitComponent>();
				limit->setTranslate(transform->getTranslate());
				limit->setRotation(transform->getRotation());
				limit->setLocalTranslate(transform->getLocalTranslate());
				limit->setLocalQuaternion(transform->getLocalQuaternion());
				limit->setAdditiveUseLocal(!(it.Flag & PMX_BONE_ADD_LOCAL));

				if (it.Flag & PMX_BONE_ADD_MOVE)
					limit->setAdditiveMoveRatio(it.ProvidedRatio);
				if (it.Flag & PMX_BONE_ADD_ROTATION)
					limit->setAdditiveRotationRatio(it.ProvidedRatio);

				auto parentController = bones[additiveParent]->getComponent<RotationLinkComponent>();
				if (parentController)
					parentController->addBone(bones[i]);
				else
				{
					auto additiveTransform = bones[additiveParent]->getComponent<TransformComponent>();
					auto rotationLink = bones[additiveParent]->addComponent<RotationLinkComponent>(bones[i]);
					rotationLink->setTranslate(additiveTransform->getTranslate());
					rotationLink->setRotation(additiveTransform->getRotation());
					rotationLink->setLocalTranslate(additiveTransform->getLocalTranslate());
					rotationLink->setLocalQuaternion(additiveTransform->getLocalQuaternion());
				}
			}

			if (it.Flag & PMX_BONE_IK)
			{
				auto solver = std::make_shared<CCDSolverComponent>();
				solver->setTarget(bones[it.IKTargetBoneIndex]);
				solver->setIterations(it.IKLoopCount);
				solver->setAutomaticUpdate(false);

				for (auto& child : it.IKList)
				{
					auto rotationLimit = bones[child.BoneIndex]->addComponent<RotationLimitComponent>();
					rotationLimit->setMininumAngle(-it.IKLimitedRadian);
					rotationLimit->setMaximumAngle(it.IKLimitedRadian);
					rotationLimit->setMinimumAxis(math::float3(child.minimumRadian.x, child.minimumRadian.y, child.minimumRadian.z));
					rotationLimit->setMaximumAxis(math::float3(child.maximumRadian.x, child.maximumRadian.y, child.maximumRadian.z));
					rotationLimit->setAxisLimitEnable(child.rotateLimited);

					solver->addBone(bones[child.BoneIndex]);
				}

				bones[i]->addComponent(std::move(solver));
			}
		}
	}

	void
	PMXImporter::createClothes(AssetImporterContext& context, const PMX& pmx, GameObjectPtr& meshes, const GameObjects& bones) noexcept(false)
	{
		for (auto& it : pmx.softbodies)
		{
			GameObjects collider;

			for (auto& body : bones)
			{
				auto rigidbody = body->getComponent<RigidbodyComponent>();
				if ((1 << rigidbody->getGroup()) & ~it.groupMask)
					continue;

				collider.push_back(body);
			}

			math::uint1s pinVertexIndices;

			for (std::size_t i = 0; i < it.numIndices; i++)
			{
				std::uint32_t index = 0;
				if (pmx.header.sizeOfIndices == 1)
					index = *((std::uint8_t*)it.pinVertexIndices.data() + i);
				else if (pmx.header.sizeOfIndices == 2)
					index = *((std::uint16_t*)it.pinVertexIndices.data() + i);
				else if (pmx.header.sizeOfIndices == 4)
					index = *((std::uint32_t*)it.pinVertexIndices.data() + i);

				pinVertexIndices.push_back(index);
			}

			auto cloth = std::make_shared<ClothComponent>();
			cloth->setColliders(collider);
			cloth->setTotalMass(it.totalMass);
			cloth->setPinVertexIndices(pinVertexIndices);
			cloth->setSolverFrequency(300.0f);
			cloth->setEnableCCD(true);
			cloth->setMaterialId(it.materialIndex);

			if (!it.anchorRigidbodies.empty())
			{
				auto rigidbody = bones[it.anchorRigidbodies[0].rigidBodyIndex];
				if (rigidbody)
				{
					if (rigidbody->getParent())
						cloth->setTarget(rigidbody->getParent()->downcast_pointer<GameObject>());
				}
			}
			
			meshes->addComponent(cloth);
		}
	}

	void
	PMXImporter::createColliders(AssetImporterContext& context, const PMX& pmx, GameObjects& bones) noexcept(false)
	{
		for (auto& it : pmx.rigidbodies)
		{
			if (it.bone >= bones.size())
				continue;

			auto bone = bones[it.bone];
			auto baseTransformInverse = bone->getComponent<TransformComponent>()->getTransformInverse();
			auto localTransform = math::transformMultiply(baseTransformInverse, math::makeRotation(math::Quaternion(math::float3(it.rotate.x, it.rotate.y, it.rotate.z)), math::float3(it.position.x, it.position.y, it.position.z)));

			math::float3 translate;
			math::float3 scale;
			math::Quaternion rotation;
			localTransform.getTransform(translate, rotation, scale);

			if (it.shape == PmxShapeType::ShapeTypeSphere)
			{
				auto collider = bone->addComponent<SphereColliderComponent>(it.scale.x > 0.0f ? it.scale.x : math::EPSILON_E3);
				collider->setCenter(translate);
				collider->setRotation(rotation);
			}
			else if (it.shape == PmxShapeType::ShapeTypeSquare)
			{
				auto collider = bone->addComponent<BoxColliderComponent>(math::max(math::float3(0.001, 0.001, 0.001), math::float3(it.scale.x, it.scale.y, it.scale.z) * 2.0f));
				collider->setCenter(translate);
				collider->setRotation(rotation);
			}
			else if (it.shape == PmxShapeType::ShapeTypeCapsule)
			{
				auto collider = bone->addComponent<CapsuleColliderComponent>(it.scale.x > 0.0f ? it.scale.x : math::EPSILON_E3, it.scale.y);
				collider->setCenter(translate);
				collider->setRotation(rotation);
			}
		}
	}

	void
	PMXImporter::createRigidbodies(AssetImporterContext& context, const PMX& pmx, GameObjects& bones) noexcept(false)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> cv;

		for (auto& it : pmx.rigidbodies)
		{
			if (it.bone >= bones.size())
				continue;

			auto bone = bones[it.bone];

			if (!bone->getComponent<RigidbodyComponent>())
			{
				auto component = bone->addComponent<RigidbodyComponent>();
				component->setName(cv.to_bytes(it.name.name));
				component->setMass(it.mass);
				component->setGroup(it.group);
				component->setGroupMask(it.groupMask);
				component->setRestitution(it.elasticity);
				component->setStaticFriction(it.friction);
				component->setDynamicFriction(it.friction);
				component->setLinearDamping(it.movementDecay);
				component->setAngularDamping(it.rotationDecay);

				if (it.physicsOperation == 0)
					component->setIsKinematic(it.physicsOperation == 0);
				else
					component->setSleepThreshold(0.0f);

				component->wakeUp();
			}
		}
	}

	void
	PMXImporter::createJoints(AssetImporterContext& context, const PMX& pmx, GameObjects& bones) noexcept(false)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> cv;

		for (auto& it : pmx.joints)
		{
			if (it.relatedRigidBodyIndexA >= pmx.rigidbodies.size()|| it.relatedRigidBodyIndexB >= pmx.rigidbodies.size())
				continue;

			auto boneA = pmx.rigidbodies[it.relatedRigidBodyIndexA].bone;
			auto boneB = pmx.rigidbodies[it.relatedRigidBodyIndexB].bone;

			if (boneA >= pmx.bones.size() || boneB >= pmx.bones.size())
				continue;

			auto& bodyA = bones[boneA];
			auto& bodyB = bones[boneB];

			if (bodyA != bodyB && bodyA && bodyB)
			{
				auto joint = bodyA->addComponent<ConfigurableJointComponent>();
				joint->setName(cv.to_bytes(it.name.name));
				joint->setTargetPosition(math::float3(it.position.x, it.position.y, it.position.z));
				joint->setTargetRotation(math::Quaternion(math::float3(it.rotation.x, it.rotation.y, it.rotation.z)));
				joint->setTarget(bodyB->getComponent<RigidbodyComponent>());
				joint->enablePreprocessing(false);

				if (it.movementLowerLimit.x == 0.0f && it.movementUpperLimit.x == 0.0f)
					joint->setXMotion(ConfigurableJointMotion::Locked);
				else if (it.movementLowerLimit.x > it.movementUpperLimit.x)
					joint->setXMotion(ConfigurableJointMotion::Free);
				else
				{
					joint->setHighXLimit(it.movementUpperLimit.x);
					joint->setLowXLimit(it.movementLowerLimit.x);
					joint->setXMotion(ConfigurableJointMotion::Limited);
				}

				if (it.movementLowerLimit.y == 0.0f && it.movementUpperLimit.y == 0.0f)
					joint->setYMotion(ConfigurableJointMotion::Locked);
				else if (it.movementLowerLimit.y > it.movementUpperLimit.y)
					joint->setYMotion(ConfigurableJointMotion::Free);
				else
				{
					joint->setHighYLimit(it.movementUpperLimit.y);
					joint->setLowYLimit(it.movementLowerLimit.y);
					joint->setYMotion(ConfigurableJointMotion::Limited);
				}

				if (it.movementLowerLimit.z == 0.0f && it.movementUpperLimit.z == 0.0f)
					joint->setZMotion(ConfigurableJointMotion::Locked);
				else if (it.movementLowerLimit.z > it.movementUpperLimit.z)
					joint->setZMotion(ConfigurableJointMotion::Free);
				else
				{
					joint->setHighZLimit(it.movementUpperLimit.z);
					joint->setLowZLimit(it.movementLowerLimit.z);
					joint->setZMotion(ConfigurableJointMotion::Limited);
				}

				if (it.rotationLowerLimit.x == 0.0f && it.rotationUpperLimit.x == 0.0f)
					joint->setAngularXMotion(ConfigurableJointMotion::Locked);
				else if (it.rotationLowerLimit.x > it.rotationUpperLimit.x)
					joint->setAngularXMotion(ConfigurableJointMotion::Free);
				else
					joint->setAngularXMotion(ConfigurableJointMotion::Limited);

				if (it.rotationLowerLimit.y == 0.0f && it.rotationUpperLimit.y == 0.0f)
					joint->setAngularYMotion(ConfigurableJointMotion::Locked);
				else if (it.rotationLowerLimit.y > it.rotationUpperLimit.y)
					joint->setAngularYMotion(ConfigurableJointMotion::Free);
				else
					joint->setAngularYMotion(ConfigurableJointMotion::Limited);

				if (it.rotationLowerLimit.z == 0.0f && it.rotationUpperLimit.z == 0.0f)
					joint->setAngularZMotion(ConfigurableJointMotion::Locked);
				else if (it.rotationLowerLimit.z > it.rotationUpperLimit.z)
					joint->setAngularZMotion(ConfigurableJointMotion::Free);
				else
					joint->setAngularZMotion(ConfigurableJointMotion::Limited);

				joint->setAngularLimit(it.rotationLowerLimit.x, it.rotationUpperLimit.x, it.rotationLowerLimit.y, it.rotationUpperLimit.y, it.rotationLowerLimit.z, it.rotationUpperLimit.z);
				
				joint->setDriveMotionX(it.springMovementConstant.x);
				joint->setDriveMotionY(it.springMovementConstant.y);
				joint->setDriveMotionZ(it.springMovementConstant.z);

				joint->setDriveAngularX(it.springRotationConstant.x);
				joint->setDriveAngularY(it.springRotationConstant.y);
				joint->setDriveAngularZ(it.springRotationConstant.z);
			}
		}
	}

	void
	PMXImporter::createMorph(AssetImporterContext& context, const PMX& pmx, GameObjectPtr& mesh) noexcept(false)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> cv;

		for (auto& it : pmx.morphs)
		{
			switch (it.morphType)
			{
			case PmxMorphType::PMX_MorphTypeVertex:
			{
				math::float3s offsets;
				math::uint1s indices;

				for (auto& v : it.vertices)
				{
					offsets.push_back(math::float3(v.offset.x, v.offset.y, v.offset.z));
					indices.push_back(v.index);
				}

				auto morph = mesh->addComponent<SkinnedMorphComponent>();
				morph->setName(cv.to_bytes(it.name.name));
				morph->setOffsets(std::move(offsets));
				morph->setIndices(std::move(indices));
			}
			break;
			}
		}
	}

	void
	PMXImporter::createMaterials(AssetImporterContext& context, const PMX& pmx, GameObjectPtr& object, Materials& materials) noexcept(false)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> cv;
		std::unordered_map<std::wstring, std::shared_ptr<Texture>> textureMap;

		materials.reserve(pmx.materials.size());

		for (auto& it : pmx.textures)
		{
			try
			{
				if (!textureMap.contains(it.name))
				{
					auto assetPath = context.getAssetPath().parent_path().append(it.name);
					if (!std::filesystem::exists(AssetDatabase::instance()->getAbsolutePath(assetPath)))
						continue;

					auto texture = AssetDatabase::instance()->loadAssetAtPath<Texture>(assetPath);
					if (texture)
					{
						texture->apply();
						context.addObjectToAsset(texture->getName(), texture);
						textureMap[it.name] = std::move(texture);
					}
				}
			}
			catch (...)
			{
			}
		}

		for (auto& it : pmx.materials)
		{
			auto material = std::make_shared<MeshStandardMaterial>();
			material->setName(it.name.length > 0 ? cv.to_bytes(it.name.name) : "Untitled");
			material->setColor(math::srgb2linear(math::float3(it.Diffuse.x, it.Diffuse.y, it.Diffuse.z)));
			material->setOpacity(it.Opacity);

			std::uint32_t limits = 0;
			if (pmx.header.sizeOfTexture == 1)
				limits = std::numeric_limits<PmxUInt8>::max();
			else if (pmx.header.sizeOfTexture == 2)
				limits = std::numeric_limits<PmxUInt16>::max();
			else if (pmx.header.sizeOfTexture == 4)
				limits = std::numeric_limits<PmxUInt32>::max();

			if (it.TextureIndex < limits)
			{
				auto name = pmx.textures[it.TextureIndex].name;
				if (textureMap.find(name) != textureMap.end())
				{
					auto texture = textureMap.at(name);
					material->setColorMap(texture);
				}
			}

			bool hasAlphaTexture = it.TextureIndex < limits ? std::wstring_view(pmx.textures[it.TextureIndex].name).find(L".png") != std::string::npos : false;

			auto colorMap = material->getColorMap();
			if (colorMap)
			{
				auto textureFormat = material->getColorMap()->format();
				if (textureFormat == Format::B8G8R8A8SRGB ||
					textureFormat == Format::B8G8R8A8UNorm ||
					textureFormat == Format::R8G8B8A8SRGB ||
					textureFormat == Format::R8G8B8A8UNorm)
				{
					hasAlphaTexture = true;
				}
			}

			if (it.Opacity < 1.0 || hasAlphaTexture) {
				material->setBlendEnable(true);
				material->setBlendSrc(BlendMode::SrcAlpha);
				material->setBlendDest(BlendMode::OneMinusSrcAlpha);
			}

			context.addObjectToAsset(material->getName(), material);

			materials.emplace_back(std::move(material));
		}
	}

	void
	PMXImporter::createMeshes(AssetImporterContext& context, const PMX& pmx, GameObjectPtr& object, const GameObjects& bones) noexcept(false)
	{
		math::float4x4s bindposes(pmx.bones.size());
		for (std::size_t i = 0; i < pmx.bones.size(); i++)
			bindposes[i].makeTranslate(-math::float3(pmx.bones[i].position.x, pmx.bones[i].position.y, pmx.bones[i].position.z));

		math::float3s vertices_;
		math::float3s normals_;
		math::float2s texcoords_;
		std::vector<VertexWeight> weights;
		std::vector<std::shared_ptr<Mesh>> meshes;

		vertices_.resize(pmx.numVertices);
		normals_.resize(pmx.numVertices);
		texcoords_.resize(pmx.numVertices);

		if (pmx.numBones)
			weights.resize(pmx.numVertices);

		for (std::size_t i = 0; i < pmx.numVertices; i++)
		{
			auto& v = pmx.vertices[i];

			vertices_[i].set(v.position.x, v.position.y, v.position.z);
			normals_[i].set(v.normal.x, v.normal.y, v.normal.z);
			texcoords_[i].set(v.coord.x, v.coord.y);

			if (pmx.numBones)
			{
				VertexWeight weight;
				weight.weight1 = v.weight.weight1;
				weight.weight2 = v.weight.weight2;
				weight.weight3 = v.weight.weight3;
				weight.weight4 = v.weight.weight4;
				weight.bone1 = v.weight.bone1 < pmx.numBones ? v.weight.bone1 : 0;
				weight.bone2 = v.weight.bone2 < pmx.numBones ? v.weight.bone2 : 0;
				weight.bone3 = v.weight.bone3 < pmx.numBones ? v.weight.bone3 : 0;
				weight.bone4 = v.weight.bone4 < pmx.numBones ? v.weight.bone4 : 0;

				weights[i] = weight;
			}
		}

		auto mesh = std::make_shared<Mesh>();
		mesh->setName("Mesh");
		mesh->setBindposes(std::move(bindposes));
		mesh->setVertexArray(std::move(vertices_));
		mesh->setNormalArray(std::move(normals_));
		mesh->setTexcoordArray(std::move(texcoords_));
		mesh->setWeightArray(std::move(weights));

		PmxUInt32 startIndices = 0;

		for (std::size_t i = 0; i < pmx.materials.size(); i++)
		{
			math::uint1s indices_(pmx.materials[i].FaceCount);

			for (std::size_t j = startIndices; j < startIndices + pmx.materials[i].FaceCount; j++)
			{
				std::uint32_t index = 0;
				if (pmx.header.sizeOfIndices == 1)
					index = *((std::uint8_t*)pmx.indices.data() + j);
				else if (pmx.header.sizeOfIndices == 2)
					index = *((std::uint16_t*)pmx.indices.data() + j);
				else if (pmx.header.sizeOfIndices == 4)
					index = *((std::uint32_t*)pmx.indices.data() + j);

				indices_[j - startIndices] = index;
			}

			mesh->setIndicesArray(std::move(indices_), i);

			startIndices += pmx.materials[i].FaceCount;
		}

		mesh->computeBoundingBox();

		context.addObjectToAsset(mesh->getName(), mesh);

		object->addComponent<MeshFilterComponent>(std::move(mesh));

		if (bones.empty())
		{
			auto meshRender = object->addComponent<MeshRendererComponent>();
			meshRender->setGlobalIllumination(true);

			Materials materials;
			createMaterials(context, pmx, object, materials);
			meshRender->setMaterials(std::move(materials));
		}
		else
		{
			auto smr = std::make_shared<SkinnedMeshRendererComponent>();
			smr->setBones(bones);
			smr->setMorphBlendEnable(true);
			smr->setTextureBlendEnable(true);
			smr->setGlobalIllumination(true);

			Materials materials;
			createMaterials(context, pmx, object, materials);
			smr->setMaterials(std::move(materials));

			object->addComponent(smr);
		}
	}

	void
	PMXImporter::onImportAsset(AssetImporterContext& context) noexcept(false)
	{
		auto filepath = AssetDatabase::instance()->getAbsolutePath(context.getAssetPath());

		PMX pmx;
		if (!PMX::load(filepath, pmx))
			return;
		
		if (pmx.numMaterials > 0)
		{
			GameObjects bones;
			createBones(context, pmx, bones);
			createColliders(context, pmx, bones);
			createRigidbodies(context, pmx, bones);
			createJoints(context, pmx, bones);

			GameObjectPtr actor = std::make_shared<GameObject>();
			actor->addComponent<AnimatorComponent>(bones);

			if (!pmx.description.japanModelName.empty())
			{
				std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> cv;
				actor->setName(cv.to_bytes(pmx.description.japanModelName.data()));
			}
			else
			{
				actor->setName((char*)std::filesystem::path(filepath).filename().c_str());
			}

			createMeshes(context, pmx, actor, bones);
			createMorph(context, pmx, actor);
			createClothes(context, pmx, actor, bones);

			for (auto& it : bones)
				context.addObjectToAsset(it->getName(), it);

			context.addObjectToAsset("MainAsset", actor);
			context.setMainObject(actor);
		}
	}

	bool
	PMXImporter::save(const GameObject& gameObject, PMX& pmx, const std::filesystem::path& path) noexcept(false)
	{
		if (!gameObject.getName().empty())
		{
			pmx.header.magic[0] = 'P';
			pmx.header.magic[1] = 'M';
			pmx.header.magic[2] = 'X';
			pmx.header.offset = 0x20;
			pmx.header.version = 2.0f;
			pmx.header.dataSize = 0x08;
			pmx.header.encode = 0x0;
			pmx.header.addUVCount = 0;
			pmx.header.sizeOfIndices = 4;
			pmx.header.sizeOfTexture = 1;
			pmx.header.sizeOfMaterial = 1;
			pmx.header.sizeOfBone = 2;
			pmx.header.sizeOfMorph = 1;
			pmx.header.sizeOfBody = 1;

			pmx.numVertices = 0;
			pmx.numIndices = 0;
			pmx.numTextures = 0;
			pmx.numMaterials = 0;
			pmx.numBones = 0;
			pmx.numMorphs = 0;
			pmx.numDisplayFrames = 0;
			pmx.numRigidbodys = 0;
			pmx.numJoints = 0;
			pmx.numSoftbodies = 0;

			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> cv;
			auto utf16 = cv.from_bytes(gameObject.getName());
			pmx.description.japanModelLength = utf16.length() * 2;
			pmx.description.japanModelName.resize(utf16.length());

			std::memcpy(pmx.description.japanModelName.data(), utf16.data(), pmx.description.japanModelLength);

			auto mf = gameObject.getComponent<MeshFilterComponent>();
			if (mf && mf->getMesh())
			{
				auto mesh = mf->getMesh();
				auto& weight = mesh->getWeightArray();

				pmx.numVertices = mesh->getNumVertices();
				pmx.numIndices = mesh->getNumIndices();

				pmx.vertices.resize(pmx.numVertices);
				pmx.indices.resize(pmx.numIndices * sizeof(std::uint32_t));

				for (std::size_t i = 0; i < pmx.numVertices; i++)
				{
					auto& pmxVertex = pmx.vertices[i];
					pmxVertex.position = mesh->getVertexArray()[i];
					pmxVertex.normal = mesh->getNormalArray()[i];
					pmxVertex.coord = mesh->getTexcoordArray()[i];
					pmxVertex.edge = 1.0f;
					pmxVertex.type = PmxVertexSkinningType::PMX_BDEF1;
					pmxVertex.weight.bone1 = weight[i].bone1;
					pmxVertex.weight.bone2 = weight[i].bone2;
					pmxVertex.weight.bone3 = weight[i].bone3;
					pmxVertex.weight.bone4 = weight[i].bone4;
					pmxVertex.weight.weight1 = weight[i].weight1;
					pmxVertex.weight.weight2 = weight[i].weight2;
					pmxVertex.weight.weight3 = weight[i].weight3;
					pmxVertex.weight.weight4 = weight[i].weight4;

					if (pmxVertex.weight.weight1 != 0 && pmxVertex.weight.weight2 != 0 && pmxVertex.weight.weight3 != 0 && pmxVertex.weight.weight4 != 0)
						pmxVertex.type = PmxVertexSkinningType::PMX_BDEF4;
					else if (pmxVertex.weight.weight1 != 0 && pmxVertex.weight.weight2 != 0)
						pmxVertex.type = PmxVertexSkinningType::PMX_BDEF2;
					else if (pmxVertex.weight.weight1 != 0)
						pmxVertex.type = PmxVertexSkinningType::PMX_BDEF1;
				}

				for (std::size_t i = 0, offset = 0; i < mesh->getNumSubsets(); i++)
				{
					auto& indices = mesh->getIndicesArray(i);
					std::memcpy(pmx.indices.data() + offset, indices.data(), indices.size() * sizeof(std::uint32_t));
					offset += indices.size() * sizeof(std::uint32_t);
				}

				auto smr = gameObject.getComponent<SkinnedMeshRendererComponent>();
				if (smr)
				{
					std::map<std::shared_ptr<Texture>, std::size_t> textureMap;

					pmx.numMaterials = smr->getMaterials().size();
					pmx.materials.resize(pmx.numMaterials);

					for (std::size_t i = 0; i < pmx.numMaterials; i++)
					{
						auto standard = smr->getMaterial(i)->downcast_pointer<MeshStandardMaterial>();
						auto texture = standard->getColorMap();

						if (texture && textureMap.find(texture) == textureMap.end())
							textureMap.insert(std::make_pair(texture, textureMap.size()));

						auto& pmxMaterial = pmx.materials[i];
						pmxMaterial.name = PmxName(standard->getName());
						pmxMaterial.nameEng = PmxName("");
						pmxMaterial.Diffuse = math::linear2srgb(standard->getColor());
						pmxMaterial.Ambient = math::float3(0.5f, 0.5f, 0.5f);
						pmxMaterial.Opacity = standard->getOpacity();
						pmxMaterial.TextureIndex = texture ? textureMap[texture] : -1;
						pmxMaterial.ToonTexture = -1;
						pmxMaterial.SphereTextureIndex = -1;
						pmxMaterial.FaceCount = mesh->getIndicesArray(i).size();
					}

					pmx.numTextures = textureMap.size();
					pmx.textures.resize(textureMap.size());

					for (auto& it : textureMap)
					{
						auto texture = it.first;
						auto name = texture->getName();
						auto filename = make_guid() + name.substr(name.find_last_of("."));
						auto outputPath = std::filesystem::path(path).append(filename).string();
						auto extension = outputPath.substr(outputPath.find_last_of(".") + 1);
						texture->save(outputPath, extension);
						pmx.textures[it.second] = PmxName(filename);
					}

					auto transforms = smr->getBones();
					if (!transforms.empty())
					{
						pmx.numBones = smr->getBones().size();
						pmx.bones.resize(pmx.numBones);

						std::map<GameObject*, std::size_t> boneMap;

						for (std::size_t i = 0; i < pmx.numBones; i++)
							boneMap[smr->getBones()[i].get()] = i;

						for (std::size_t i = 0; i < pmx.numBones; i++)
						{
							auto transform = smr->getBones()[i];

							auto& pmxBone = pmx.bones[i];
							pmxBone.name = transform->getName();
							pmxBone.Visable = true;
							pmxBone.position = -mesh->getBindposes()[i].getTranslate();
							pmxBone.Parent = transform->getParent() ? boneMap[transform->getParent().get()] : -1;
							pmxBone.ProvidedParentBoneIndex = -1;
							pmxBone.Flag |= PMX_BONE_ROTATION | PMX_BONE_OPERATOR | PMX_BONE_DISPLAY;

							auto linkLimit = transform->getComponent<RotationLinkLimitComponent>();
							if (linkLimit)
							{
								pmxBone.ProvidedRatio = linkLimit->getAdditiveRotationRatio();

								if (!linkLimit->getAdditiveUseLocal())
									pmxBone.Flag |= PMX_BONE_ADD_LOCAL;
								if (linkLimit->getAdditiveMoveRatio() != 0.0f)
									pmxBone.Flag |= PMX_BONE_ADD_MOVE;
								if (linkLimit->getAdditiveRotationRatio() != 0.0f)
									pmxBone.Flag |= PMX_BONE_ADD_ROTATION;
							}
							else
							{
								pmxBone.Flag |= PMX_BONE_MOVE;
							}

							auto slover = transform->getComponent<CCDSolverComponent>();
							if (slover)
							{
								pmxBone.Flag |= PMX_BONE_IK;
								pmxBone.Level = 1;
								pmxBone.IKLoopCount = slover->getIterations();
								pmxBone.IKTargetBoneIndex = boneMap[slover->getTarget().get()];
								pmxBone.IKLinkCount = slover->getBones().size();
								pmxBone.IKList.resize(pmxBone.IKLinkCount);

								for (std::size_t n = 0; n < pmxBone.IKLinkCount; n++)
								{
									auto bone = slover->getBone(n);
									auto rotationLimit = bone->getComponent<RotationLimitComponent>();

									auto& ik = pmxBone.IKList[n];
									ik.BoneIndex = boneMap[bone.get()];
									
									if (rotationLimit)
									{
										pmxBone.IKLimitedRadian = rotationLimit->getMaximumAngle();

										ik.rotateLimited = rotationLimit->getAxisLimitEnable();
										ik.minimumRadian = rotationLimit->getMinimumAxis();
										ik.maximumRadian = rotationLimit->getMaximumAxis();
									}
								}
							}
						}

						for (std::size_t i = 0; i < pmx.numBones; i++)
						{
							auto transform = smr->getBones()[i];
							auto links = transform->getComponent<RotationLinkComponent>();
							if (links)
							{
								for (auto& bone : links->getBones())
									pmx.bones[boneMap[bone.get()]].ProvidedParentBoneIndex = i;
							}
						}

						std::vector<std::size_t> rigidbodyToBone;
						std::vector<RigidbodyComponent*> rigidbodies;

						for (std::size_t i = 0; i < pmx.numBones; i++)
						{
							auto bone = smr->getBones()[i];
							auto rigibdody = bone->getComponent<RigidbodyComponent>();
							if (rigibdody)
							{
								rigidbodies.push_back(rigibdody.get());
								rigidbodyToBone.push_back(i);
							}
						}

						pmx.numRigidbodys = rigidbodies.size();
						pmx.rigidbodies.resize(pmx.numRigidbodys);

						for (std::size_t i = 0; i < pmx.numRigidbodys; i++)
						{
							auto it = rigidbodies[i]->downcast<RigidbodyComponent>();

							auto& pmxRigidbody = pmx.rigidbodies[i];
							pmxRigidbody.bone = rigidbodyToBone[i];
							pmxRigidbody.name = it->getName();
							pmxRigidbody.nameEng = PmxName("");
							pmxRigidbody.mass = it->getMass();
							pmxRigidbody.group = it->getGroup();
							pmxRigidbody.groupMask  = it->getGroupMask();
							pmxRigidbody.elasticity = it->getRestitution();
							pmxRigidbody.friction = it->getStaticFriction();
							pmxRigidbody.movementDecay = it->getLinearDamping();
							pmxRigidbody.rotationDecay = it->getAngularDamping();
							pmxRigidbody.physicsOperation = it->getIsKinematic() ? 0 : 1;

							auto collider = it->getComponent<ColliderComponent>();
							if (collider)
							{
								auto baseTransform = it->getComponent<TransformComponent>()->getTransform();
								auto localTransform = math::transformMultiply(baseTransform, math::makeRotation(collider->getRotation(), collider->getCenter()));

								math::float3 translate;
								math::float3 scale;
								math::Quaternion rotation;
								localTransform.getTransform(translate, rotation, scale);

								pmxRigidbody.position = translate;
								pmxRigidbody.rotate = math::eulerAngles(rotation);
							}

							if (auto boxCollider = it->getComponent<BoxColliderComponent>())
							{
								pmxRigidbody.shape = PmxShapeType::ShapeTypeSquare;
								pmxRigidbody.scale = boxCollider->getSize();
							}								
							else if (auto capsuleCollider = it->getComponent<CapsuleColliderComponent>())
							{
								pmxRigidbody.shape = PmxShapeType::ShapeTypeCapsule;
								pmxRigidbody.scale.x = capsuleCollider->getRadius();
								pmxRigidbody.scale.y = capsuleCollider->getHeight();
								pmxRigidbody.scale.z = 0.0f;
							}
							else if (auto sphereCollider = it->getComponent<SphereColliderComponent>())
							{
								pmxRigidbody.shape = PmxShapeType::ShapeTypeSphere;
								pmxRigidbody.scale.x = sphereCollider->getRadius();
								pmxRigidbody.scale.y = 0.0f;
								pmxRigidbody.scale.z = 0.0f;
							}
						}

						std::map<RigidbodyComponent*, std::size_t> rigidbodyMap;

						for (std::size_t i = 0; i < pmx.numRigidbodys; i++)
						{
							auto rigidbody = rigidbodies[i]->downcast<RigidbodyComponent>();
							if (rigidbody)
								rigidbodyMap[rigidbody] = rigidbodyMap.size();
						}

						GameComponents joints;
						for (std::size_t i = 0; i < pmx.numRigidbodys; i++)
							rigidbodies[i]->getComponents<ConfigurableJointComponent>(joints);

						pmx.numJoints = joints.size();
						pmx.joints.resize(pmx.numJoints);

						for (std::size_t i = 0; i < pmx.numJoints; i++)
						{
							float minX, maxX, minY, maxY, minZ, maxZ;

							auto joint = joints[i]->downcast<ConfigurableJointComponent>();
							joint->getAngularLimit(minX, maxX, minY, maxY, minZ, maxZ);

							auto& pmxJoint = pmx.joints[i];
							pmxJoint.name = joint->getName();
							pmxJoint.nameEng = PmxName("");
							pmxJoint.type = 0;
							pmxJoint.position = joint->getTargetPosition();
							pmxJoint.rotation = math::eulerAngles(joint->getTargetRotation());
							pmxJoint.relatedRigidBodyIndexA = rigidbodyMap[joint->getComponent<RigidbodyComponent>().get()];
							pmxJoint.relatedRigidBodyIndexB = joint->getTarget() ? rigidbodyMap[joint->getTarget().get()] : -1;
							pmxJoint.movementLowerLimit.x = joint->getLowXLimit();
							pmxJoint.movementLowerLimit.y = joint->getLowYLimit();
							pmxJoint.movementLowerLimit.z = joint->getLowZLimit();
							pmxJoint.movementUpperLimit.x = joint->getHighXLimit();
							pmxJoint.movementUpperLimit.y = joint->getHighYLimit();
							pmxJoint.movementUpperLimit.z = joint->getHighZLimit();
							pmxJoint.rotationLowerLimit.x = minX;
							pmxJoint.rotationLowerLimit.y = minY;
							pmxJoint.rotationLowerLimit.z = minZ;
							pmxJoint.rotationUpperLimit.x = maxX;
							pmxJoint.rotationUpperLimit.y = maxY;
							pmxJoint.rotationUpperLimit.z = maxZ;
							pmxJoint.springMovementConstant.x = joint->getDriveMotionX();
							pmxJoint.springMovementConstant.y = joint->getDriveMotionY();
							pmxJoint.springMovementConstant.z = joint->getDriveMotionZ();
							pmxJoint.springRotationConstant.x = joint->getDriveAngularX();
							pmxJoint.springRotationConstant.y = joint->getDriveAngularY();
							pmxJoint.springRotationConstant.z = joint->getDriveAngularZ();
						}
					}
				}
			}

			GameComponents morphComponents;
			gameObject.getComponents<SkinnedMorphComponent>(morphComponents);

			if (!morphComponents.empty())
			{
				pmx.numMorphs = morphComponents.size();
				pmx.morphs.resize(pmx.numMorphs);

				for (std::size_t i = 0; i < pmx.numMorphs; i++)
				{
					auto morphComponent = morphComponents[i]->downcast<SkinnedMorphComponent>();
					auto& offsets = morphComponent->getOffsets();
					auto& indices = morphComponent->getIndices();

					auto& morph = pmx.morphs[i];
					morph.name = morphComponent->getName();
					morph.control = morphComponent->getControl();
					morph.morphType = PmxMorphType::PMX_MorphTypeVertex;
					morph.morphCount = indices.size();
					morph.vertices.resize(indices.size());

					for (std::size_t n = 0; n < indices.size(); n++)
					{
						morph.vertices[n].index = indices[n];
						morph.vertices[n].offset = offsets[n];
					}
				}
			}
		}

		return true;
	}

	bool
	PMXImporter::save(const GameObject& gameObject, const std::filesystem::path& path) noexcept(false)
	{
		auto stream = std::ofstream(path, std::ios_base::binary);
		if (stream)
		{
			auto pmx = std::make_unique<PMX>();
			auto root = path.parent_path();

			save(gameObject, *pmx, root);

			PMX::save(stream, *pmx);

			return true;
		}

		return false;
	}
}