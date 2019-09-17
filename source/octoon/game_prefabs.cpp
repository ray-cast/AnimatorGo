#include <octoon/game_prefabs.h>
#include <octoon/hal/graphics.h>
#include <octoon/image/image.h>
#include <octoon/io/vstream.h>

#include <octoon/video/render_system.h>
#include <octoon/video/ggx_material.h>
#include <octoon/video/basic_material.h>
#include <octoon/video/line_material.h>

#include <octoon/model/model.h>

#include <octoon/camera_component.h>
#include <octoon/ortho_camera_component.h>
#include <octoon/perspective_camera_component.h>
#include <octoon/film_camera_component.h>
#include <octoon/transform_component.h>
#include <octoon/first_person_camera_component.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/mesh_renderer_component.h>
#include <octoon/text_component.h>
#include <octoon/solver_component.h>
#include <octoon/animator_component.h>
#include <octoon/skinned_morph_component.h>
#include <octoon/skinned_mesh_renderer_component.h>
#include <octoon/skinned_joint_renderer_component.h>
#include <octoon/guizmo_component.h>
#include <octoon/rotation_limit_component.h>
#include <octoon/rigidbody_component.h>
#include <octoon/sphere_collider_component.h>
#include <octoon/box_collider_component.h>
#include <octoon/capsule_collider_component.h>
#include <octoon/configurable_joint_component.h>
#include <octoon/offline_mesh_renderer_component.h>
#include <octoon/offline_skinned_mesh_renderer_component.h>
#include <octoon/rotation_link_component.h>
#include <octoon/rotation_link_limit_component.h>
#include <octoon/cloth_component.h>

#include <octoon/runtime/except.h>
#include <octoon/runtime/string.h>

using namespace octoon::io;
using namespace octoon::image;
using namespace octoon::hal;
using namespace octoon::video;
using namespace octoon::model;

namespace octoon
{
	OctoonImplementSingleton(GamePrefabs)

	GamePrefabs::GamePrefabs() noexcept
	{
	}

	GamePrefabs::~GamePrefabs() noexcept
	{
	}

	GameObjectPtr
	GamePrefabs::createOrthoCamera() except
	{
		auto camera = GameObject::create("MainCamera");
		camera->addComponent<OrthoCameraComponent>(0.0f, 1.0f, 0.0f, 1.0f);
		camera->getComponent<OrthoCameraComponent>()->setCameraType(video::CameraType::Main);
		camera->getComponent<TransformComponent>()->setTranslate(math::float3(0.0f, 0.0f, -10.0f));
		camera->addComponent<FirstPersonCameraComponent>();

		return camera;
	}

	GameObjectPtr
	GamePrefabs::createPerspectiveCamera(float fov) except
	{
		auto object = GameObject::create("MainCamera");
		object->getComponent<TransformComponent>()->setTranslate(math::float3(0, 1, -10));
		object->addComponent<FirstPersonCameraComponent>();

		auto camera = object->addComponent<PerspectiveCameraComponent>(fov);
		camera->setCameraType(video::CameraType::Main);

		return object;
	}

	GameObjectPtr
	GamePrefabs::createFilmCamera(float canvasWidth, float zoom) except
	{
		auto object = GameObject::create("MainCamera");
		object->getComponent<TransformComponent>()->setTranslate(math::float3(0, 1, -10));
		object->addComponent<FirstPersonCameraComponent>();

		auto camera = object->addComponent<FilmCameraComponent>();
		camera->setCameraType(video::CameraType::Main);
		camera->setCanvasWidth(canvasWidth);
		camera->setZoom(zoom);

		return object;
	}

	GameObjectPtr
	GamePrefabs::createCircle(float radius, std::uint32_t segments, float thetaStart, float thetaLength) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makeCircle(radius, segments, thetaStart, thetaLength));
		object->addComponent<MeshRendererComponent>(std::make_shared<model::Material>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createPlane(float width, float height, std::uint32_t widthSegments, std::uint32_t heightSegments) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makePlane(width, height, widthSegments, heightSegments));
		object->addComponent<MeshRendererComponent>(std::make_shared<model::Material>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createPlane(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, std::uint8_t u, std::uint8_t v, float udir, float vdir) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makePlane(width, height, depth, widthSegments, heightSegments, depthSegments, u, v, udir, vdir));
		object->addComponent<MeshRendererComponent>(std::make_shared<model::Material>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createPlaneWireframe(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, std::uint8_t u, std::uint8_t v, float udir, float vdir, bool clear) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makePlaneWireframe(width, height, depth, widthSegments, heightSegments, depthSegments, u, v, udir, vdir, clear));
		object->addComponent<MeshRendererComponent>(std::make_shared<model::Material>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createFloor(float width, float height, std::uint32_t widthSegments, std::uint32_t heightSegments) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makeFloor(width, height, widthSegments, heightSegments));
		object->addComponent<MeshRendererComponent>(std::make_shared<model::Material>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createNoise(float width, float height, std::uint32_t widthSegments, std::uint32_t heightSegments) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makeNoise(width, height, widthSegments, heightSegments));
		object->addComponent<MeshRendererComponent>(std::make_shared<model::Material>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createCube(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makeCube(width, height, depth, widthSegments, heightSegments, depthSegments));
		object->addComponent<MeshRendererComponent>(std::make_shared<model::Material>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createCubeWireframe(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makeCubeWireframe(width, height, depth, widthSegments, heightSegments, depthSegments));
		object->addComponent<MeshRendererComponent>(std::make_shared<model::Material>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createRing(float innerRadius, float outerRadius, std::uint32_t thetaSegments, std::uint32_t phiSegments, float thetaStart, float thetaLength) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makeRing(innerRadius, outerRadius, thetaSegments, phiSegments, thetaStart, thetaLength));
		object->addComponent<MeshRendererComponent>(std::make_shared<model::Material>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createSphere(float radius, std::uint32_t widthSegments, std::uint32_t heightSegments, float phiStart, float phiLength, float thetaStart, float thetaLength) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makeSphere(radius, widthSegments, heightSegments, phiStart, phiLength, thetaStart, thetaLength));
		object->addComponent<MeshRendererComponent>(std::make_shared<model::Material>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createVolumes(float fovy, float znear, float zfar) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makeVolumes(fovy, znear, zfar));
		object->addComponent<MeshRendererComponent>(std::make_shared<model::Material>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createCone(float radius, float height, std::uint32_t segments, float thetaStart, float thetaLength) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makeCone(radius, height, segments, thetaStart, thetaLength));
		object->addComponent<MeshRendererComponent>(std::make_shared<model::Material>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createText(const wchar_t* text, std::uint16_t fontsize, const char* fontPath) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makeMesh(model::makeTextContours(text, { fontPath, fontsize })));
		object->addComponent<MeshRendererComponent>(std::make_shared<model::Material>());

		return object;
	}

	GameObjectPtr
	GamePrefabs::createText(const char* u8str, std::uint16_t fontsize, const char* fontPath) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<TextComponent>(u8str)->setTextMeshing(std::make_shared<TextMeshing>(fontPath, fontsize));
		object->addComponent<MeshRendererComponent>(std::make_shared<model::Material>());

		return object;
	}

	GameObjectPtr
	GamePrefabs::createModel(const std::string& path, bool cache) except
	{
		Model model(path);

		GameObjects bones;
		if (!this->createBones(model, bones))
			return false;

		if (!this->createSolver(model, bones))
			return false;

		GameObjects rigidbody;
		if (!this->createRigidbodies(model, bones, rigidbody))
			return false;

		GameObjects joints;
		if (!this->createJoints(model, rigidbody, joints))
			return false;

		GameObjectPtr actor;
		if (!this->createMeshes(model, actor, bones, path))
			return false;

		if (!this->createMorph(model, actor))
			return false;

		if (!this->createClothes(model, actor, bones, rigidbody))
			return false;

		return actor;
	}

	bool
	GamePrefabs::createBones(const Model& model, GameObjects& bones) noexcept
	{
		bones.reserve(model.get<Model::bone>().size());

		for (auto& it : model.get<Model::bone>())
			bones.emplace_back(GameObject::create(it->getName()));

		for (std::size_t i = 0; i < model.get<Model::bone>().size(); i++)
		{
			auto it = model.get<Model::bone>(i);

			auto parent = it->getParent();
			if (parent >= 0 && parent < bones.size())
				bones[i]->setParent(bones[parent]);

			auto transform = bones[i]->getComponent<TransformComponent>();
			transform->setTranslate(it->getPosition());
			transform->setQuaternion(it->getRotation());
			
			auto additiveParent = it->getAdditiveParent();
			if (additiveParent >= 0 && additiveParent < bones.size())
			{
				auto limit = bones[i]->addComponent<RotationLinkLimitComponent>();
				limit->setTranslate(transform->getTranslate());
				limit->setQuaternion(transform->getQuaternion());
				limit->setLocalTranslate(transform->getLocalTranslate());
				limit->setLocalQuaternion(transform->getLocalQuaternion());
				limit->setAdditiveMoveRatio(it->getAdditiveMoveRatio());
				limit->setAdditiveRotationRatio(it->getAdditiveRotationRatio());
				limit->setAdditiveUseLocal(it->getAdditiveUseLocal());

				auto parentController = bones[additiveParent]->getComponent<RotationLinkComponent>();
				if (parentController)
					parentController->addBone(bones[i]);
				else
				{
					auto additiveTransform = bones[additiveParent]->getComponent<TransformComponent>();
					auto rotationLink = bones[additiveParent]->addComponent<RotationLinkComponent>(bones[i]);
					rotationLink->setTranslate(additiveTransform->getTranslate());
					rotationLink->setQuaternion(additiveTransform->getQuaternion());
					rotationLink->setLocalTranslate(additiveTransform->getLocalTranslate());
					rotationLink->setLocalQuaternion(additiveTransform->getLocalQuaternion());
				}
			}
		}

		return true;
	}

	bool
	GamePrefabs::createClothes(const model::Model& model, GameObjectPtr& meshes, const GameObjects& bones, GameObjects& rigidbodies) noexcept
	{
		for (auto& it : model.get<Model::softbody>())
		{
			GameObjects collider;

			for (auto& body : rigidbodies)
			{
				auto rigidbody = body->getComponent<RigidbodyComponent>();
				if ((1 << rigidbody->getGroup()) & ~it->groupMask)
					continue;

				collider.push_back(body);
			}

			auto cloth = ClothComponent::create();
			cloth->setColliders(collider);
			cloth->setTotalMass(it->totalMass);
			cloth->setPinVertexIndices(it->pinVertexIndices);
			cloth->setSolverFrequency(300.0f);
			cloth->setEnableCCD(true);
			cloth->setMaterialId(it->materialIndex);

			if (!it->anchorRigidbodies.empty())
			{
				auto rigidbody = rigidbodies[it->anchorRigidbodies.front()];
				if (rigidbody)
				{
					if (rigidbody->getParent())
						cloth->setTarget(rigidbody->getParent()->downcast_pointer<GameObject>());
				}
			}
			
			meshes->addComponent(cloth);
		}

		return true;
	}

	bool
	GamePrefabs::createRigidbodies(const model::Model& model, GameObjects& bones, GameObjects& rigidbodys) noexcept
	{
		rigidbodys.reserve(model.get<Model::rigidbody>().size());

		for (auto& it : model.get<Model::rigidbody>())
		{
			auto gameObject = GameObject::create();
			gameObject->setName(it->name);
			gameObject->setParent(it->bone < bones.size() ? bones[it->bone] : nullptr);
			gameObject->setLayer(it->group);
			gameObject->getComponent<TransformComponent>()->setTransform(it->position, math::Quaternion(it->rotation));

			if (it->shape == ShapeType::ShapeTypeSphere)
				gameObject->addComponent<SphereColliderComponent>(it->scale.x > 0.0f ? it->scale.x : math::EPSILON_E3, 0.1f);
			else if (it->shape == ShapeType::ShapeTypeSquare)
				gameObject->addComponent<BoxColliderComponent>(math::max(math::float3(0.001, 0.001, 0.001), it->scale * 2.0f), 0.1f);
			else if (it->shape == ShapeType::ShapeTypeCapsule)
				gameObject->addComponent<CapsuleColliderComponent>(it->scale.x > 0.0f ? it->scale.x : math::EPSILON_E3, it->scale.y, 0.1f);
			else
				assert(false);

			auto component = std::make_shared<RigidbodyComponent>();
			component->setName(it->name);
			component->setMass(std::max(1.0f, it->mass));
			component->setGroupMask(it->groupMask);
			component->setRestitution(it->elasticity);
			component->setStaticFriction(it->friction);
			component->setDynamicFriction(it->friction / 1.5f);
			component->setLinearDamping(it->movementDecay);
			component->setAngularDamping(it->rotationDecay);
			component->setIsKinematic(it->physicsOperation == 0);
			component->setSleepThreshold(0.0f);
			component->setSolverIterationCounts(10, 10);
			//component->setEnableCCD(!component->getIsKinematic());

			gameObject->addComponent(component);

			rigidbodys.emplace_back(std::move(gameObject));
		}

		return true;
	}

	bool
	GamePrefabs::createJoints(const Model& model, const GameObjects& rigidbodys, GameObjects& joints) noexcept
	{
		joints.reserve(model.get<Model::joint>().size());

		for (auto& it : model.get<Model::joint>())
		{
			if (rigidbodys.size() <= it->bodyIndexA || rigidbodys.size() <= it->bodyIndexB)
				continue;

			auto bodyA = rigidbodys[it->bodyIndexA];
			auto bodyB = rigidbodys[it->bodyIndexB];

			if (bodyA != bodyB)
			{
				auto joint = bodyA->addComponent<ConfigurableJointComponent>();
				joint->setTarget(bodyB->getComponent<RigidbodyComponent>());
				joint->setTargetPosition(it->position);
				joint->setTargetRotation(math::Quaternion(it->rotation));

				if (it->movementLowerLimit.x == 0.0f && it->movementUpperLimit.x == 0.0f)
					joint->setXMotion(ConfigurableJointMotion::Locked);
				else if (it->movementLowerLimit.x > it->movementUpperLimit.x)
					joint->setXMotion(ConfigurableJointMotion::Free);
				else if (it->movementLowerLimit.x > -10.0f && it->movementUpperLimit.x < 10.0f)
				{
					joint->setLowXLimit(it->movementLowerLimit.x);
					joint->setHighXLimit(it->movementUpperLimit.x);
					joint->setXMotion(ConfigurableJointMotion::Limited);
				}

				if (it->movementLowerLimit.y == 0.0f && it->movementUpperLimit.y == 0.0f)
					joint->setYMotion(ConfigurableJointMotion::Locked);
				else if (it->movementLowerLimit.y > it->movementUpperLimit.y)
					joint->setXMotion(ConfigurableJointMotion::Free);
				else if (it->movementLowerLimit.y > -10.0f && it->movementUpperLimit.y < 10.0f)
				{
					joint->setLowYLimit(it->movementLowerLimit.y);
					joint->setHighYLimit(it->movementUpperLimit.y);
					joint->setYMotion(ConfigurableJointMotion::Limited);
				}

				if (it->movementLowerLimit.z == 0.0f && it->movementUpperLimit.z == 0.0f)
					joint->setZMotion(ConfigurableJointMotion::Locked);
				else if (it->movementLowerLimit.z > it->movementUpperLimit.z)
					joint->setXMotion(ConfigurableJointMotion::Free);
				else if (it->movementLowerLimit.z > -10.0f && it->movementUpperLimit.z < 10.0f)
				{
					joint->setLowZLimit(it->movementLowerLimit.z);
					joint->setHighZLimit(it->movementUpperLimit.z);
					joint->setZMotion(ConfigurableJointMotion::Limited);
				}

				if (it->movementLowerLimit.x != 0.0f || it->movementUpperLimit.x != 0.0f ||
					it->movementLowerLimit.y != 0.0f || it->movementUpperLimit.y != 0.0f ||
					it->movementLowerLimit.z != 0.0f || it->movementUpperLimit.z != 0.0f)
				{
					joint->setDistanceLimit(math::distance(it->movementLowerLimit, it->movementUpperLimit));
				}

				if (it->rotationLowerLimit.x == 0.0f && it->rotationUpperLimit.x == 0.0f)
					joint->setAngularXMotion(ConfigurableJointMotion::Locked);
				else
				{
					if (it->rotationLowerLimit.x > it->rotationUpperLimit.x)
						std::swap(it->rotationLowerLimit.x, it->rotationUpperLimit.x);

					joint->setTwistLimit(math::clamp(it->rotationLowerLimit.x, -6.24318f, 6.24318f) - 1e-5f, math::clamp(it->rotationUpperLimit.x, -6.24318f, 6.24318f) + 1e-5f);
					joint->setAngularXMotion(ConfigurableJointMotion::Limited);
				}

				if (it->rotationLowerLimit.y == 0.0f && it->rotationUpperLimit.y == 0.0f)
					joint->setAngularYMotion(ConfigurableJointMotion::Locked);
				else
					joint->setAngularYMotion(ConfigurableJointMotion::Limited);

				if (it->rotationLowerLimit.z == 0.0f && it->rotationUpperLimit.z == 0.0f)
					joint->setAngularZMotion(ConfigurableJointMotion::Locked);
				else
					joint->setAngularZMotion(ConfigurableJointMotion::Limited);

				if (it->rotationLowerLimit.y > it->rotationUpperLimit.y)
					std::swap(it->rotationLowerLimit.y, it->rotationUpperLimit.y);
				if (it->rotationLowerLimit.z > it->rotationUpperLimit.z)
					std::swap(it->rotationLowerLimit.z, it->rotationUpperLimit.z);

				auto rotationLimitY = std::max(std::abs(it->rotationLowerLimit.y), std::abs(it->rotationUpperLimit.y));
				auto rotationLimitZ = std::max(std::abs(it->rotationLowerLimit.z), std::abs(it->rotationUpperLimit.z));

				rotationLimitY = math::clamp(rotationLimitY, 1e-5f, 3.1415f);
				rotationLimitZ = math::clamp(rotationLimitZ, 1e-5f, 3.1415f);

				joint->setSwingLimit(rotationLimitY, rotationLimitZ);

				if (it->springMovementConstant.x != 0.0f)
					joint->setDriveMotionX(std::max(0.0f, it->springMovementConstant.x));
				if (it->springMovementConstant.y != 0.0f)
					joint->setDriveMotionX(std::max(0.0f, it->springMovementConstant.y));
				if (it->springMovementConstant.z != 0.0f)
					joint->setDriveMotionX(std::max(0.0f, it->springMovementConstant.z));

				if (it->springRotationConstant.x != 0.0f)
					joint->setDriveAngularX(std::max(0.0f, it->springRotationConstant.x));
				if (it->springRotationConstant.y != 0.0f || it->springRotationConstant.z != 0.0f)
					joint->setDriveAngularY(std::max(0.0f, (it->springRotationConstant.y + it->springRotationConstant.z) * 0.5f));

				joints.emplace_back(std::move(bodyA));
			}
		}

		return true;
	}

	bool 
	GamePrefabs::createSolver(const model::Model& model, GameObjects& bones) noexcept
	{
		for (auto& it : model.get<Model::ik>())
		{
			auto iksolver = std::make_shared<CCDSolverComponent>();
			iksolver->setTarget(bones[it->targetBoneIndex]);
			iksolver->setIterations(it->iterations);

			for (auto& child : it->child)
			{
				if (child.rotateLimited)
					bones[child.boneIndex]->addComponent<RotationLimitComponent>(-child.angleRadian, child.angleRadian, child.minimumRadian, child.maximumRadian);
				else
					bones[child.boneIndex]->addComponent<RotationLimitComponent>(-child.angleRadian, child.angleRadian, math::float3(-math::PI), math::float3(math::PI));

				iksolver->addBone(bones[child.boneIndex]);
			}

			bones[it->boneIndex]->addComponent(std::move(iksolver));
		}

		return true;
	}

	bool
	GamePrefabs::createMorph(const model::Model& model, GameObjectPtr& mesh) noexcept
	{
		for (auto& it : model.get<Model::morph>())
		{
			math::float3s offsets;
			math::uint1s indices;

			for (auto& v : it->vertices)
			{
				offsets.push_back(v.offset);
				indices.push_back(v.index);
			}

			auto animation = mesh->addComponent<SkinnedMorphComponent>();
			animation->setName(it->name);
			animation->setOffsets(offsets);
			animation->setIndices(indices);
		}

		return true;
	}

	bool
	GamePrefabs::createMeshes(const model::Model& model, GameObjectPtr& meshes, const GameObjects& bones, const std::string& path) noexcept
	{
		if (model.get<Model::mesh>().size() == 0)
			return false;

		model::Materials materials;
		if (!this->createMaterials(model, materials, "file:" + runtime::string::directory(path)))
			return false;

		auto mesh = model.get<Model::mesh>(0);
		auto object = GameObject::create(mesh->getName());
		object->addComponent<MeshFilterComponent>(mesh);

		if (bones.empty())
		{
			object->addComponent<MeshRendererComponent>(materials);
		}
		else
		{
			auto smr = SkinnedMeshRendererComponent::create();
			smr->setMaterials(materials);
			smr->setTransforms(bones);
			smr->setMorphBlendEnable(true);
			smr->setTextureBlendEnable(true);

			object->addComponent(smr);

			auto osmr = OfflineSkinnedMeshRendererComponent::create();
			osmr->setMaterials(materials);
			osmr->setTransforms(bones);
			osmr->setMorphBlendEnable(true);
			osmr->setTextureBlendEnable(true);
			object->addComponent(osmr);

			/*auto material = std::make_shared<model::Material>();
			material->set(MATKEY_COLOR_DIFFUSE, math::float3(0.4, 0.9, 0.4));

			auto sjr = std::make_shared<SkinnedJointRendererComponent>();
			sjr->setMaterial(material);
			sjr->setTransforms(bones);

			object->addComponent(sjr);*/
		}

		meshes = object;

		return true;
	}

	bool
	GamePrefabs::createMaterials(const model::Model& model, model::Materials& materials, const std::string& rootPath) noexcept
	{
		materials.reserve(model.get<Model::material>().size());

		for (auto& it : model.get<Model::material>())
		{
			it->set(MATKEY_PATH, rootPath);
			materials.push_back(it);
		}

		return true;
	}

	GameObjectPtr
	GamePrefabs::createSprite(const std::string& image, float w, float h) except
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makePlane(w, h));
		//object->addComponent<MeshRendererComponent>(std::make_shared<model::Material>(this->createTexture(image)));

		return object;
	}

	GameObjectPtr
	GamePrefabs::createSprite(const hal::GraphicsTexturePtr& texture) except
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makePlane((float)texture->getTextureDesc().getWidth(), (float)texture->getTextureDesc().getHeight()));
		//object->addComponent<MeshRendererComponent>(std::make_shared<model::Material>(texture));

		return object;
	}

	GameObjectPtr
	GamePrefabs::createSprite(const hal::GraphicsTexturePtr& texture, float w, float h) except
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makePlane(w, h));
		//object->addComponent<MeshRendererComponent>(std::make_shared<model::Material>(texture));

		return object;
	}

	GameObjectPtr
	GamePrefabs::createSpriteSquare(float w, float h) except
	{
		return this->createSprite("sys:sprite/square.png", w, h);
	}

	hal::GraphicsTexturePtr
	GamePrefabs::createTexture(const std::string& path, bool cache) noexcept
	{
		assert(!path.empty());

		try
		{
			auto it = textureCaches_.find(path);
			if (it != textureCaches_.end())
				return (*it).second;

			Image image;
			if (!image.load(path))
				throw runtime::runtime_error::create("Failed to open file :" + path);

			GraphicsFormat format = GraphicsFormat::Undefined;
			switch (image.format())
			{
			case Format::BC1RGBUNormBlock: format = GraphicsFormat::BC1RGBUNormBlock; break;
			case Format::BC1RGBAUNormBlock: format = GraphicsFormat::BC1RGBAUNormBlock; break;
			case Format::BC1RGBSRGBBlock: format = GraphicsFormat::BC1RGBSRGBBlock; break;
			case Format::BC1RGBASRGBBlock: format = GraphicsFormat::BC1RGBASRGBBlock; break;
			case Format::BC3UNormBlock: format = GraphicsFormat::BC3UNormBlock; break;
			case Format::BC3SRGBBlock: format = GraphicsFormat::BC3SRGBBlock; break;
			case Format::BC4UNormBlock: format = GraphicsFormat::BC4UNormBlock; break;
			case Format::BC4SNormBlock: format = GraphicsFormat::BC4SNormBlock; break;
			case Format::BC5UNormBlock: format = GraphicsFormat::BC5UNormBlock; break;
			case Format::BC5SNormBlock: format = GraphicsFormat::BC5SNormBlock; break;
			case Format::BC6HUFloatBlock: format = GraphicsFormat::BC6HUFloatBlock; break;
			case Format::BC6HSFloatBlock: format = GraphicsFormat::BC6HSFloatBlock; break;
			case Format::BC7UNormBlock: format = GraphicsFormat::BC7UNormBlock; break;
			case Format::BC7SRGBBlock: format = GraphicsFormat::BC7SRGBBlock; break;
			case Format::R8G8B8UNorm: format = GraphicsFormat::R8G8B8UNorm; break;
			case Format::R8G8B8SRGB: format = GraphicsFormat::R8G8B8UNorm; break;
			case Format::R8G8B8A8UNorm: format = GraphicsFormat::R8G8B8A8UNorm; break;
			case Format::R8G8B8A8SRGB: format = GraphicsFormat::R8G8B8A8UNorm; break;
			case Format::B8G8R8UNorm: format = GraphicsFormat::B8G8R8UNorm; break;
			case Format::B8G8R8SRGB: format = GraphicsFormat::B8G8R8UNorm; break;
			case Format::B8G8R8A8UNorm: format = GraphicsFormat::B8G8R8A8UNorm; break;
			case Format::B8G8R8A8SRGB: format = GraphicsFormat::B8G8R8A8UNorm; break;
			case Format::R8UNorm: format = GraphicsFormat::R8UNorm; break;
			case Format::R8SRGB: format = GraphicsFormat::R8UNorm; break;
			case Format::R8G8UNorm: format = GraphicsFormat::R8G8UNorm; break;
			case Format::R8G8SRGB: format = GraphicsFormat::R8G8UNorm; break;
			case Format::R16SFloat: format = GraphicsFormat::R16SFloat; break;
			case Format::R16G16SFloat: format = GraphicsFormat::R16G16SFloat; break;
			case Format::R16G16B16SFloat: format = GraphicsFormat::R16G16B16SFloat; break;
			case Format::R16G16B16A16SFloat: format = GraphicsFormat::R16G16B16A16SFloat; break;
			case Format::R32SFloat: format = GraphicsFormat::R32SFloat; break;
			case Format::R32G32SFloat: format = GraphicsFormat::R32G32SFloat; break;
			case Format::R32G32B32SFloat: format = GraphicsFormat::R32G32B32SFloat; break;
			case Format::R32G32B32A32SFloat: format = GraphicsFormat::R32G32B32A32SFloat; break;
			default:
				throw runtime::runtime_error::create("This image type is not supported by this function:" + path);
			}

			GraphicsTextureDesc textureDesc;
			textureDesc.setName(path);
			textureDesc.setSize(image.width(), image.height(), image.depth());
			textureDesc.setTexDim(GraphicsTextureDim::Texture2D);
			textureDesc.setTexFormat(format);
			textureDesc.setStream(image.data());
			textureDesc.setStreamSize(image.size());
			textureDesc.setMipBase(image.mipBase());
			textureDesc.setMipNums(image.mipLevel());
			textureDesc.setLayerBase(image.layerBase());
			textureDesc.setLayerNums(image.layerLevel());

			auto texture = RenderSystem::instance()->createTexture(textureDesc);
			if (!texture)
				return nullptr;

			if (cache)
				textureCaches_[path] = texture;

			return texture;
		}
		catch (...)
		{
			return nullptr;
		}
	}
}