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
#include <octoon/rotation_link_component.h>
#include <octoon/rotation_link_limit_component.h>
#include <octoon/cloth_component.h>

#include <octoon/io/fstream.h>
#include <octoon/pmx_loader.h>

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

	void createClothes(const model::Model& model, GameObjectPtr& meshes, const GameObjects& bones) noexcept(false)
	{
		for (auto& it : model.softbodies)
		{
			GameObjects collider;

			for (auto& body : bones)
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
				auto rigidbody = bones[it->anchorRigidbodies.front()];
				if (rigidbody)
				{
					if (rigidbody->getParent())
						cloth->setTarget(rigidbody->getParent()->downcast_pointer<GameObject>());
				}
			}
			
			meshes->addComponent(cloth);
		}
	}

	void createColliders(const model::Model& model, GameObjects& bones) noexcept(false)
	{
		for (auto& it : model.rigidbodies)
		{
			if (it->bone >= bones.size())
				continue;

			auto bone = bones[it->bone];
			auto baseTransformInverse = bone->getComponent<TransformComponent>()->getTransformInverse();
			auto localTransform = math::transformMultiply(baseTransformInverse, math::makeRotation(math::Quaternion(it->rotation), it->position));

			math::float3 translate;
			math::float3 scale;
			math::Quaternion rotation;
			localTransform.getTransform(translate, rotation, scale);

			if (it->shape == model::ShapeType::ShapeTypeSphere)
			{
				auto collider = bone->addComponent<SphereColliderComponent>(it->scale.x > 0.0f ? it->scale.x : math::EPSILON_E3);
				collider->setCenter(translate);
				collider->setQuaternion(rotation);
			}
			else if (it->shape == model::ShapeType::ShapeTypeSquare)
			{
				auto collider = bone->addComponent<BoxColliderComponent>(math::max(math::float3(0.001, 0.001, 0.001), it->scale * 2.0f));
				collider->setCenter(translate);
				collider->setQuaternion(rotation);
			}
			else if (it->shape == model::ShapeType::ShapeTypeCapsule)
			{
				auto collider = bone->addComponent<CapsuleColliderComponent>(it->scale.x > 0.0f ? it->scale.x : math::EPSILON_E3, it->scale.y);
				collider->setCenter(translate);
				collider->setQuaternion(rotation);
			}
		}
	}

	void createRigidbodies(const model::Model& model, GameObjects& bones) noexcept(false)
	{
		for (auto& it : model.rigidbodies)
		{
			if (it->bone >= bones.size())
				continue;

			auto bone = bones[it->bone];

			if (!bone->getComponent<RigidbodyComponent>())
			{
				bone->setLayer(it->group);

				auto component = bone->addComponent<RigidbodyComponent>();
				component->setName(it->name);
				component->setMass(it->mass);
				component->setGroupMask(it->groupMask);
				component->setRestitution(it->elasticity);
				component->setStaticFriction(it->friction);
				component->setDynamicFriction(it->friction);
				component->setLinearDamping(it->movementDecay);
				component->setAngularDamping(it->rotationDecay);

				if (it->physicsOperation == 0)
					component->setIsKinematic(it->physicsOperation == 0);
				else
					component->setSleepThreshold(0.0f);

				component->wakeUp();
			}
		}
	}

	void createJoints(const model::Model& model, GameObjects& bones) noexcept(false)
	{
		for (auto& it : model.joints)
		{
			if (it->bodyIndexA  >= model.rigidbodies.size()|| it->bodyIndexB >= model.rigidbodies.size())
				continue;

			auto boneA = model.rigidbodies[it->bodyIndexA]->bone;
			auto boneB = model.rigidbodies[it->bodyIndexB]->bone;

			if (boneA >= model.bones.size() || boneB >= model.bones.size())
				continue;

			auto& bodyA = bones[boneA];
			auto& bodyB = bones[boneB];

			if (bodyA != bodyB && bodyA && bodyB)
			{
				auto joint = bodyA->addComponent<ConfigurableJointComponent>();
				joint->setTargetPosition(it->position);
				joint->setTargetRotation(math::Quaternion(it->rotation));
				joint->setTarget(bodyB->getComponent<RigidbodyComponent>());
				joint->enablePreprocessing(false);

				if (it->movementLowerLimit.x == 0.0f && it->movementUpperLimit.x == 0.0f)
					joint->setXMotion(ConfigurableJointMotion::Locked);
				else if (it->movementLowerLimit.x > it->movementUpperLimit.x)
					joint->setXMotion(ConfigurableJointMotion::Free);
				else
				{
					joint->setHighXLimit(it->movementUpperLimit.x);
					joint->setLowXLimit(it->movementLowerLimit.x);
					joint->setXMotion(ConfigurableJointMotion::Limited);
				}

				if (it->movementLowerLimit.y == 0.0f && it->movementUpperLimit.y == 0.0f)
					joint->setYMotion(ConfigurableJointMotion::Locked);
				else if (it->movementLowerLimit.y > it->movementUpperLimit.y)
					joint->setYMotion(ConfigurableJointMotion::Free);
				else
				{
					joint->setHighYLimit(it->movementUpperLimit.y);
					joint->setLowYLimit(it->movementLowerLimit.y);
					joint->setYMotion(ConfigurableJointMotion::Limited);
				}

				if (it->movementLowerLimit.z == 0.0f && it->movementUpperLimit.z == 0.0f)
					joint->setZMotion(ConfigurableJointMotion::Locked);
				else if (it->movementLowerLimit.z > it->movementUpperLimit.z)
					joint->setZMotion(ConfigurableJointMotion::Free);
				else
				{
					joint->setHighZLimit(it->movementUpperLimit.z);
					joint->setLowZLimit(it->movementLowerLimit.z);
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

				if (std::abs(it->rotationLowerLimit.y) == std::abs(it->rotationUpperLimit.y) &&
					std::abs(it->rotationLowerLimit.z) == std::abs(it->rotationUpperLimit.z))
				{
					auto rotationLimitY = math::min(std::abs(it->rotationLowerLimit.y), math::radians(120.0f));
					auto rotationLimitZ = math::min(std::abs(it->rotationLowerLimit.z), math::radians(120.0f));
					joint->setSwingLimit(rotationLimitY, rotationLimitZ);
				}
				else
				{
					math::float2 lowerLimit = it->rotationLowerLimit.yz();
					math::float2 upperLimit = it->rotationUpperLimit.yz();

					if (joint->getAngularYMotion() == ConfigurableJointMotion::Locked)
						lowerLimit.x = upperLimit.x = it->rotationLowerLimit.y;
					else if (joint->getAngularYMotion() == ConfigurableJointMotion::Free)
						lowerLimit.x = upperLimit.x = 0;

					if (joint->getAngularZMotion() == ConfigurableJointMotion::Locked)
						lowerLimit.y = upperLimit.y = it->rotationLowerLimit.z;
					else if (joint->getAngularZMotion() == ConfigurableJointMotion::Free)
						lowerLimit.y = upperLimit.y = 0;

					joint->setPyramidSwingLimit(lowerLimit.x, upperLimit.x, lowerLimit.y, upperLimit.y);
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
					bones[child.boneIndex]->addComponent<RotationLimitComponent>(-child.angleRadian, child.angleRadian, math::float3::Zero, math::float3::Zero);

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

	void createMaterials(const model::Model& model, Materials& materials) noexcept(false)
	{
		materials.reserve(model.materials.size());

		for (auto& it : model.materials)
			materials.push_back(it);
	}

	void createMeshes(const model::Model& model, GameObjectPtr& meshes, const GameObjects& bones, std::string_view path) noexcept(false)
	{
		Materials materials;
		createMaterials(model, materials);

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
			smr->setGlobalIllumination(true);

			object->addComponent(smr);
		}

		meshes = object;
	}

	GameObjectPtr
	MeshLoader::load(std::string_view filepath, bool cache) noexcept(false)
	{
		model::Model model;

		PmxLoader load;
		load.doLoad(filepath, model);

		if (!model.meshes.empty())
		{
			GameObjectPtr actor;

			GameObjects bones;

			createBones(model, bones);
			createSolver(model, bones);
			createColliders(model, bones);
			createRigidbodies(model, bones);
			createJoints(model, bones);
			
			createMeshes(model, actor, bones, std::string(filepath));
			createMorph(model, actor);
			createClothes(model, actor, bones);

			return actor;
		}

		return nullptr;
	}
}