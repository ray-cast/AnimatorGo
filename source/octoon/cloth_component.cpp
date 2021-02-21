#include <octoon/cloth_component.h>
#include <octoon/transform_component.h>
#include <octoon/timer_feature.h>
#include <octoon/rotation_limit_component.h>
#include <octoon/rotation_link_component.h>
#include <octoon/rotation_link_limit_component.h>
#include <octoon/cloth_feature.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/collider_component.h>
#include <octoon/box_collider_component.h>
#include <octoon/sphere_collider_component.h>
#include <octoon/capsule_collider_component.h>
#include <octoon/mesh_renderer_component.h>

#include <NvCloth/Cloth.h>
#include <NvCloth/Factory.h>
#include <NvCloth/Solver.h>
#include <NvClothExt/ClothFabricCooker.h>
#include <foundation/PxVec2.h>
#include <foundation/PxVec4.h>
#include <foundation/PxQuat.h>

namespace octoon
{
	OctoonImplementSubClass(ClothComponent, GameComponent, "Cloth")

	ClothComponent::ClothComponent() noexcept
		: cloth_(nullptr)
		, needUpdate_(false)
		, totalMass_(100.0f)
		, friction_(0.5f)
		, solverFrequency_(120.0f)
		, enableContinuousCollision_(false)
		, materialId_(0)
		, gravity_(math::float3(0.0f, -98.0f, 0.0f))
	{
	}

	ClothComponent::ClothComponent(const GameObjects& colliders) noexcept
		: ClothComponent()
	{
		this->setColliders(colliders);
	}

	ClothComponent::~ClothComponent() noexcept
	{
	}

	void
	ClothComponent::setTotalMass(float totalMass) noexcept
	{
		totalMass_ = totalMass;
	}

	void
	ClothComponent::setFriction(float friction) noexcept
	{
		if (cloth_)
			cloth_->setFriction(friction);
		friction_ = friction;
	}

	void
	ClothComponent::setSolverFrequency(float solverFrequency) noexcept
	{
		if (cloth_)
			cloth_->setSolverFrequency(solverFrequency);
		solverFrequency_ = solverFrequency;
	}

	float
	ClothComponent::getTotalMass() const noexcept
	{
		return this->totalMass_;
	}

	float
	ClothComponent::getFriction() const noexcept
	{
		return this->friction_;
	}

	float
	ClothComponent::getSolverFrequency() const noexcept
	{
		return this->solverFrequency_;
	}

	void
	ClothComponent::setTarget(const GameObjectPtr& target) noexcept
	{
		target_ = target;
	}

	GameObjectPtr
	ClothComponent::getTarget() const noexcept
	{
		return target_;
	}

	void
	ClothComponent::setMaterialId(std::uint32_t n) noexcept
	{
		materialId_ = n;
	}

	void
	ClothComponent::setEnableCCD(bool enableContinuousCollision) noexcept
	{
		if (cloth_)
			cloth_->enableContinuousCollision(enableContinuousCollision);
		enableContinuousCollision_ = enableContinuousCollision;
	}

	bool
	ClothComponent::getEnableCCD() const noexcept
	{
		return enableContinuousCollision_;
	}

	void
	ClothComponent::addCollider(GameObjectPtr&& joint) noexcept
	{
		collides_.emplace_back(std::move(joint));
	}

	void
	ClothComponent::addCollider(const GameObjectPtr& joint) noexcept
	{
		collides_.push_back(joint);
	}

	void
	ClothComponent::setColliders(GameObjects&& bones) noexcept
	{
		collides_ = std::move(bones);
	}

	void
	ClothComponent::setColliders(const GameObjects& bones) noexcept
	{
		collides_ = bones;
	}

	const GameObjects&
	ClothComponent::getColliders() const noexcept
	{
		return collides_;
	}

	void
	ClothComponent::setGravity(const math::float3& gravity) noexcept
	{
		if (gravity_ != gravity)
		{
			if (cloth_)
			{
				cloth_->setGravity(physx::PxVec3(gravity.x, gravity.y, gravity.z));
			}

			gravity_ = gravity;
		}
	}
	
	const math::float3&
	ClothComponent::getGravity() const noexcept
	{
		return gravity_;
	}

	void
	ClothComponent::setPartices(const math::float4s& partices) noexcept
	{
		partices_ = partices;
	}
	
	const math::float4s&
	ClothComponent::getPartices() const noexcept
	{
		return partices_;
	}

	void
	ClothComponent::setIndices(const math::uint1s& indices) noexcept
	{
		particesIndices_ = indices;
	}

	const math::uint1s&
	ClothComponent::getIndices() const noexcept
	{
		return particesIndices_;
	}

	void
	ClothComponent::setPinVertexIndices(const math::uint1s& indices) noexcept
	{
		pinVertexIndices_ = indices;
	}

	const math::uint1s&
	ClothComponent::getPinVertexIndices() const noexcept
	{
		return pinVertexIndices_;
	}

	GameComponentPtr
	ClothComponent::clone() const noexcept
	{
		auto instance = std::make_shared<ClothComponent>();
		instance->setName(this->getName());
		instance->setColliders(this->getColliders());

		return instance;
	}

	void
	ClothComponent::onActivate() noexcept
	{
		this->addComponentDispatch(GameDispatchType::FixedUpdate);
		this->addComponentDispatch(GameDispatchType::LateUpdate);
		this->addComponentDispatch(GameDispatchType::MoveAfter);

		auto meshFilter = this->getComponent<MeshFilterComponent>();
		if (meshFilter)
			this->uploadMeshData(*meshFilter->getMesh());
	}

	void
	ClothComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::FixedUpdate);
		this->removeComponentDispatch(GameDispatchType::LateUpdate);
		this->removeComponentDispatch(GameDispatchType::MoveAfter);

		if (cloth_)
		{
			auto clothFeature = this->getFeature<ClothFeature>();
			if (clothFeature)
				clothFeature->getScene()->removeCloth(cloth_);

			delete cloth_;
			cloth_ = nullptr;
		}
	}

	void
	ClothComponent::onFixedUpdate() noexcept
	{
		if (cloth_ && !cloth_->isAsleep())
			needUpdate_ = true;
	}

	void
	ClothComponent::onLateUpdate() noexcept
	{
		if (target_)
		{
			auto transform = target_->getComponent<TransformComponent>();
			auto translate = transform->getTranslate();
			auto rotation = transform->getQuaternion();

			cloth_->teleportToLocation(physx::PxVec3(translate.x, translate.y, translate.z), physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w));
		}

		if (needUpdate_)
		{
			this->updateCollideData();
			this->updateParticesData();
			needUpdate_ = false;
		}
	}

	void
	ClothComponent::updateCollideData() noexcept
	{
		std::vector<physx::PxVec4> spheres;
		std::vector<physx::PxU32> capsules;

		for (auto& it : this->collides_)
		{
			auto collide = it->getComponent<ColliderComponent>();
			if (collide->isInstanceOf<SphereColliderComponent>())
			{
				auto radius = collide->downcast<SphereColliderComponent>()->getRadius();
				auto translate = collide->getComponent<TransformComponent>()->getTranslate();
				spheres.push_back(physx::PxVec4(translate.x, translate.y, translate.z, radius));
			}
			else if (collide->isInstanceOf<CapsuleColliderComponent>())
			{
				auto capsule = collide->downcast<CapsuleColliderComponent>();
				auto transform = collide->getComponent<TransformComponent>();
				auto translate = transform->getTranslate();
				auto quaternion = transform->getQuaternion();
				auto height = capsule->getHeight();
				auto radius = capsule->getRadius();
				auto halfHeight = height * 0.5f;

				auto s1 = math::float3(0, -halfHeight, 0);
				auto s2 = math::float3(0, halfHeight, 0);

				s1 = math::rotate(quaternion, s1) + translate;
				s2 = math::rotate(quaternion, s2) + translate;

				spheres.push_back(physx::PxVec4(s1.x, s1.y, s1.z, radius));
				spheres.push_back(physx::PxVec4(s2.x, s2.y, s2.z, radius));

				capsules.push_back(static_cast<std::uint32_t>(spheres.size() - 2));
				capsules.push_back(static_cast<std::uint32_t>(spheres.size() - 1));
			}
		}

		/*for (auto& it : spheres)
		{
			auto& rotation = cloth_->getRotation();
			auto& translate = cloth_->getTranslation();
			auto xyz = rotation.rotate(it.getXYZ() - translate);
			it.x = xyz.x;
			it.y = xyz.y;
			it.z = xyz.z;
		}*/

		nv::cloth::Range<const physx::PxVec4> sphereRange(spheres.data(), spheres.data() + spheres.size());
		cloth_->setSpheres(sphereRange, 0, cloth_->getNumSpheres());

		nv::cloth::Range<const physx::PxU32> capsuleRange(capsules.data(), capsules.data() + capsules.size());
		cloth_->setCapsules(capsuleRange, 0, cloth_->getNumCapsules());
	}

	void
	ClothComponent::updateParticesData() noexcept
	{
		nv::cloth::MappedRange<physx::PxVec4> particles = cloth_->getCurrentParticles();
		std::memcpy(partices_.data(), &particles.front(), particles.size() * sizeof(math::float4));
	}

	void
	ClothComponent::uploadMeshData(const Mesh& mesh) noexcept
	{
		auto clothFeature = this->tryGetFeature<ClothFeature>();
		if (!clothFeature)
			return;

		if (cloth_)
		{
			clothFeature->getScene()->removeCloth(cloth_);
			delete cloth_;
		}

		auto& vertices = mesh.getVertexArray();
		auto& triangles = mesh.getIndicesArray(materialId_);

		std::vector<std::uint32_t> indices;
		std::map<std::uint32_t, std::uint32_t> indicesMap;

		for (auto& it : triangles)
		{
			if (indicesMap.find(it) == indicesMap.end())
				indicesMap[it] = static_cast<std::uint32_t>(indicesMap.size());
			indices.push_back(indicesMap[it]);
		}

		partices_.resize(indicesMap.size());
		particesIndices_.resize(indicesMap.size());

		float invMass = 1.0f / (totalMass_ / indices.size());
		for (auto& index : indicesMap)
		{
			partices_[index.second] = math::float4(vertices[index.first], invMass);
			particesIndices_[index.second] = index.first;
		}
		
		nv::cloth::ClothMeshDesc meshDesc;
		meshDesc.points.data = partices_.data();
		meshDesc.points.stride = sizeof(math::float4);
		meshDesc.points.count = static_cast<physx::PxU32>(partices_.size());
		meshDesc.triangles.data = indices.data();
		meshDesc.triangles.stride = sizeof(std::uint32_t) * 3;
		meshDesc.triangles.count = static_cast<physx::PxU32>(indices.size() / 3);
		meshDesc.invMasses.data = &partices_.front().w;
		meshDesc.invMasses.stride = sizeof(math::float4);
		meshDesc.invMasses.count = static_cast<physx::PxU32>(partices_.size());

		physx::PxVec3 gravity(gravity_.x, gravity_.y, gravity_.z);
		nv::cloth::Vector<int32_t>::Type phaseTypeInfo;
		nv::cloth::Fabric* fabric = NvClothCookFabricFromMesh(clothFeature->getContext(), meshDesc, gravity, &phaseTypeInfo, true);

		try
		{
			if (fabric)
			{
				cloth_ = clothFeature->getContext()->createCloth(nv::cloth::Range<physx::PxVec4>((physx::PxVec4*)partices_.data(), (physx::PxVec4*)partices_.data() + partices_.size()), *fabric);
				cloth_->setUserData(this);
				cloth_->setGravity(gravity);
				cloth_->setFriction(this->getFriction());
				cloth_->setSolverFrequency(this->getSolverFrequency());
				cloth_->setDamping(physx::PxVec3(0.1f, 0.1f, 0.1f));
				cloth_->setSelfCollisionDistance(0.07f);
				cloth_->setTetherConstraintScale(1.0f);
				cloth_->setTetherConstraintStiffness(1.0f);
				cloth_->enableContinuousCollision(this->getEnableCCD());

				auto transform = this->getComponent<TransformComponent>();
				auto translate = transform->getTranslate();
				auto rotation = transform->getQuaternion();
				cloth_->teleportToLocation(physx::PxVec3(translate.x, translate.y, translate.z), physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w));

				nv::cloth::Range<physx::PxVec4> motionConstraints = cloth_->getMotionConstraints();
				for (int i = 0; i < (int)motionConstraints.size(); i++)
					motionConstraints[i] = physx::PxVec4(partices_[i].x, partices_[i].y, partices_[i].z, 10.0f);

				for (auto& index : this->pinVertexIndices_)
				{
					auto it = indicesMap.find(index);
					if (it != indicesMap.end())
						motionConstraints[(*it).second].w = 1e-5f;
				}

				std::vector<nv::cloth::PhaseConfig> phases(fabric->getNumPhases());
				for (std::uint16_t i = 0; i < fabric->getNumPhases(); i++)
				{
					phases[i].mPhaseIndex = i;
					phases[i].mStiffness = 1.0f;
					phases[i].mStiffnessMultiplier = 1.0f;
					phases[i].mCompressionLimit = 1.0f;
					phases[i].mStretchLimit = 1.0f;
				}

				cloth_->setPhaseConfig(nv::cloth::Range<nv::cloth::PhaseConfig>(phases.data(), phases.data() + fabric->getNumPhases()));
				clothFeature->getScene()->addCloth(cloth_);
			}

			fabric->decRefCount();
		}
		catch (...)
		{
			fabric->decRefCount();
		}
	}
}