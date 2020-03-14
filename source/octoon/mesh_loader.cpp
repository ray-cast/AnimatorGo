#include <octoon/mesh_loader.h>
#include <octoon/model/model.h>
#include <octoon/runtime/string.h>

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
#include <octoon/offline_mesh_renderer_component.h>
#include <octoon/offline_skinned_mesh_renderer_component.h>
#include <octoon/rotation_link_component.h>
#include <octoon/rotation_link_limit_component.h>
#include <octoon/cloth_component.h>

#include <octoon/io/fstream.h>
#include <octoon/model/pmx_loader.h>

namespace octoon
{
	void createBones(const model::Model& model, GameObjects& bones) noexcept(false)
	{
		bones.reserve(model.bones.size());

		for (auto& it : model.bones)
			bones.emplace_back(GameObject::create(it->getName()));

		for (std::size_t i = 0; i < model.bones.size(); i++)
		{
			auto it = model.bones[i];

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
	}

	void createClothes(const model::Model& model, GameObjectPtr& meshes, const GameObjects& bones, GameObjects& rigidbodies) noexcept(false)
	{
		for (auto& it : model.softbodies)
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
	}

	void createRigidbodies(const model::Model& model, GameObjects& bones, GameObjects& rigidbodys) noexcept(false)
	{
		rigidbodys.reserve(model.rigidbodies.size());

		for (auto& it : model.rigidbodies)
		{
			auto gameObject = GameObject::create();
			gameObject->setName(it->name);
			gameObject->setParent(it->bone < bones.size() ? bones[it->bone] : nullptr);
			gameObject->setLayer(it->group);
			gameObject->getComponent<TransformComponent>()->setTransform(it->position, math::Quaternion(it->rotation));

			if (it->shape == model::ShapeType::ShapeTypeSphere)
				gameObject->addComponent<SphereColliderComponent>(it->scale.x > 0.0f ? it->scale.x : math::EPSILON_E3, 0.1f);
			else if (it->shape == model::ShapeType::ShapeTypeSquare)
				gameObject->addComponent<BoxColliderComponent>(math::max(math::float3(0.001, 0.001, 0.001), it->scale * 2.0f), 0.1f);
			else if (it->shape == model::ShapeType::ShapeTypeCapsule)
				gameObject->addComponent<CapsuleColliderComponent>(it->scale.x > 0.0f ? it->scale.x : math::EPSILON_E3, it->scale.y, 0.1f);
			else
				assert(false);

			auto component = std::make_shared<RigidbodyComponent>();
			component->setName(it->name);
			component->setMass(std::clamp(it->mass, 0.1f, 10.0f));
			component->setGroupMask(it->groupMask);
			component->setRestitution(it->elasticity);
			component->setStaticFriction(it->friction * 1.5f);
			component->setDynamicFriction(it->friction);
			component->setLinearDamping(it->movementDecay);
			component->setAngularDamping(it->rotationDecay);
			component->setIsKinematic(it->physicsOperation == 0);
			component->setSleepThreshold(0.0f);
			component->setSolverIterationCounts(12, 3);
			//component->setEnableCCD(!component->getIsKinematic());

			gameObject->addComponent(component);

			rigidbodys.emplace_back(std::move(gameObject));
		}
	}

	void createJoints(const model::Model& model, const GameObjects& rigidbodys, GameObjects& joints) noexcept(false)
	{
		joints.reserve(model.joints.size());

		for (auto& it : model.joints)
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
				else
				{
					joint->setLowXLimit(std::max(it->movementLowerLimit.x, -10.0f));
					joint->setHighXLimit(std::min(it->movementUpperLimit.x, 10.0f));
					joint->setXMotion(ConfigurableJointMotion::Limited);
				}

				if (it->movementLowerLimit.y == 0.0f && it->movementUpperLimit.y == 0.0f)
					joint->setYMotion(ConfigurableJointMotion::Locked);
				else if (it->movementLowerLimit.y > it->movementUpperLimit.y)
					joint->setYMotion(ConfigurableJointMotion::Free);
				else
				{
					joint->setLowYLimit(std::max(it->movementLowerLimit.y, -10.0f));
					joint->setHighYLimit(std::min(it->movementUpperLimit.y, 10.0f));
					joint->setYMotion(ConfigurableJointMotion::Limited);
				}

				if (it->movementLowerLimit.z == 0.0f && it->movementUpperLimit.z == 0.0f)
					joint->setZMotion(ConfigurableJointMotion::Locked);
				else if (it->movementLowerLimit.z > it->movementUpperLimit.z)
					joint->setZMotion(ConfigurableJointMotion::Free);
				else
				{
					joint->setLowZLimit(std::max(it->movementLowerLimit.z, -10.0f));
					joint->setHighZLimit(std::min(it->movementUpperLimit.z, 10.0f));
					joint->setZMotion(ConfigurableJointMotion::Limited);
				}

				if (it->rotationLowerLimit.x == 0.0f && it->rotationUpperLimit.x == 0.0f)
					joint->setAngularXMotion(ConfigurableJointMotion::Locked);
				else if (it->rotationLowerLimit.x > it->rotationUpperLimit.x)
					joint->setAngularXMotion(ConfigurableJointMotion::Free);
				else
					joint->setAngularXMotion(ConfigurableJointMotion::Limited);

				if (it->rotationLowerLimit.y == 0.0f && it->rotationUpperLimit.y == 0.0f)
					joint->setAngularYMotion(ConfigurableJointMotion::Locked);
				else if (it->rotationLowerLimit.y > it->rotationUpperLimit.y)
					joint->setAngularYMotion(ConfigurableJointMotion::Free);
				else
					joint->setAngularYMotion(ConfigurableJointMotion::Limited);

				if (it->rotationLowerLimit.z == 0.0f && it->rotationUpperLimit.z == 0.0f)
					joint->setAngularZMotion(ConfigurableJointMotion::Locked);
				else if (it->rotationLowerLimit.z > it->rotationUpperLimit.z)
					joint->setAngularZMotion(ConfigurableJointMotion::Free);
				else
					joint->setAngularZMotion(ConfigurableJointMotion::Limited);

				if (it->rotationLowerLimit.x < it->rotationUpperLimit.x)
				{
					auto lower = math::clamp(it->rotationLowerLimit.x, -math::radians(120.0f), math::radians(120.0f)) - 1e-5f;
					auto upper = math::clamp(it->rotationUpperLimit.x, -math::radians(120.0f), math::radians(120.0f)) + 1e-5f;
					joint->setTwistLimit(lower, upper);
				}

				if (it->rotationLowerLimit.y < it->rotationUpperLimit.y || it->rotationLowerLimit.z < it->rotationUpperLimit.z)
				{
					auto rotationLimitY = math::clamp(std::max(std::abs(it->rotationLowerLimit.y), std::abs(it->rotationUpperLimit.y)), math::EPSILON_E5, math::radians(120.0f));
					auto rotationLimitZ = math::clamp(std::max(std::abs(it->rotationLowerLimit.z), std::abs(it->rotationUpperLimit.z)), math::EPSILON_E5, math::radians(120.0f));
					joint->setSwingLimit(rotationLimitY, rotationLimitZ);
				}

				if (it->springMovementConstant.x != 0.0f)
					joint->setDriveMotionX(std::max(0.0f, it->springMovementConstant.x));
				if (it->springMovementConstant.y != 0.0f)
					joint->setDriveMotionY(std::max(0.0f, it->springMovementConstant.y));
				if (it->springMovementConstant.z != 0.0f)
					joint->setDriveMotionZ(std::max(0.0f, it->springMovementConstant.z));

				if (it->springRotationConstant.x != 0.0f)
					joint->setDriveAngularX(std::max(0.0f, it->springRotationConstant.x));
				if (it->springRotationConstant.y != 0.0f || it->springRotationConstant.z != 0.0f)
					joint->setDriveAngularZ(std::max(0.0f, (it->springRotationConstant.y + it->springRotationConstant.z) * 0.5f));

				joints.emplace_back(std::move(bodyA));
			}
		}
	}

	void createSolver(const model::Model& model, GameObjects& bones) noexcept(false)
	{
		for (auto& it : model.iks)
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
	}

	void createMorph(const model::Model& model, GameObjectPtr& mesh) noexcept(false)
	{
		for (auto& it : model.morphs)
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
	}

	void createMaterials(const model::Model& model, material::Materials& materials, std::string_view rootPath) noexcept(false)
	{
		materials.reserve(model.materials.size());

		for (auto& it : model.materials)
		{
			it->set(MATKEY_PATH, std::string(rootPath));
			materials.push_back(it);
		}
	}

	void createMeshes(const model::Model& model, GameObjectPtr& meshes, const GameObjects& bones, std::string_view path) noexcept(false)
	{
		material::Materials materials;
		createMaterials(model, materials, "file:" + runtime::string::directory(std::string(path)));

		auto mesh = model.meshes[0];
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

			/*auto material = std::make_shared<material::Material>();
			material->set(MATKEY_COLOR_DIFFUSE, math::float3(0.4, 0.9, 0.4));

			auto sjr = std::make_shared<SkinnedJointRendererComponent>();
			sjr->setMaterial(material);
			sjr->setTransforms(bones);

			object->addComponent(sjr);*/
		}

		meshes = object;
	}

	GameObjectPtr
	MeshLoader::load(std::string_view filepath, bool cache) noexcept(false)
	{
		model::Model model;

		io::ifstream stream;
		stream.open(std::string(filepath));

		model::PmxLoader load;
		load.doLoad(stream, model);

		if (!model.meshes.empty())
		{
			GameObjectPtr actor;

			GameObjects bones;
			GameObjects rigidbody;
			GameObjects joints;

			createBones(model, bones);
			createSolver(model, bones);
			createRigidbodies(model, bones, rigidbody);
			createJoints(model, rigidbody, joints);
			
			createMeshes(model, actor, bones, std::string(filepath));
			createMorph(model, actor);
			createClothes(model, actor, bones, rigidbody);

			return actor;
		}

		return nullptr;
	}
}