#include <octoon/cloth_component.h>
#include <octoon/transform_component.h>
#include <octoon/game_scene.h>
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

	float
	ClothComponent::getTotalMass() const noexcept
	{
		return this->totalMass_;
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
			delete cloth_;
			cloth_ = nullptr;
		}
	}

	void
	ClothComponent::onMoveAfter() noexcept
	{
		if (cloth_)
		{
			auto transform = this->getComponent<TransformComponent>();
			auto translate = transform->getTranslate();
			auto rotation = transform->getQuaternion();

			cloth_->setTranslation(physx::PxVec3(translate.x, translate.y, translate.z));
			cloth_->setRotation(physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w));
		}
	}

	void
	ClothComponent::onFixedUpdate() noexcept
	{
		needUpdate_ = true;
	}

	void
	ClothComponent::onLateUpdate() noexcept
	{
		if (!cloth_) return;

		if (needUpdate_)
		{
			if (!cloth_->isAsleep())
			{
				std::vector<physx::PxVec4> spheres;

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
						auto translate = collide->getComponent<TransformComponent>()->getTranslate();
						spheres.push_back(physx::PxVec4(translate.x, translate.y, translate.z, capsule->getRadius()));
					}
				}

				for (auto& it : spheres)
				{
					auto& rotation = cloth_->getRotation();
					auto& translate = cloth_->getTranslation();
					auto xyz = rotation.rotate(it.getXYZ() - translate);
					it.x = xyz.x;
					it.y = xyz.y;
					it.z = xyz.z;
				}

				nv::cloth::Range<const physx::PxVec4> sphereRange(spheres.data(), spheres.data() + spheres.size());
				cloth_->setSpheres(sphereRange, 0, cloth_->getNumSpheres());

				auto meshFilter = this->getComponent<MeshFilterComponent>();
				if (meshFilter)
				{
					auto& vertices = meshFilter->getMesh()->getVertexArray();

					nv::cloth::MappedRange<physx::PxVec4> particles = cloth_->getCurrentParticles();
					for (int i = 0; i < particles.size(); i++)
					{
						vertices[i].x = particles[i].x;
						vertices[i].y = particles[i].y;
						vertices[i].z = particles[i].z;
					}

					meshFilter->getMesh()->computeVertexNormals();
					meshFilter->uploadMeshData();
				}
			}

			needUpdate_ = false;
		}
	}

	void
	ClothComponent::uploadMeshData(const model::Mesh& mesh) noexcept
	{
		auto clothFeature = this->getGameScene()->getFeature<ClothFeature>();
		if (clothFeature)
		{
			if (cloth_)
				delete cloth_;

			float invMass = totalMass_ / mesh.getVertexArray().size();

			std::vector<float> mass(mesh.getVertexArray().size(), invMass);

			nv::cloth::ClothMeshDesc meshDesc;
			meshDesc.points.data = mesh.getVertexArray().data();
			meshDesc.points.stride = sizeof(math::float3);
			meshDesc.points.count = mesh.getVertexArray().size();
			meshDesc.triangles.data = mesh.getIndicesArray().data();
			meshDesc.triangles.stride = sizeof(std::uint32_t) * 3;
			meshDesc.triangles.count = mesh.getIndicesArray().size() / 3;
			meshDesc.invMasses.data = mass.data();
			meshDesc.invMasses.stride = sizeof(float);
			meshDesc.invMasses.count = mass.size();

			physx::PxVec3 gravity(0.0f, -9.8f, 0.0f);
			nv::cloth::Vector<int32_t>::Type phaseTypeInfo;
			nv::cloth::Fabric* fabric = NvClothCookFabricFromMesh(clothFeature->getContext(), meshDesc, gravity, &phaseTypeInfo, false);

			std::vector<physx::PxVec4> positions;
			for (auto& it : mesh.getVertexArray())
				positions.push_back(physx::PxVec4(it.x, it.y, it.z, invMass));

			auto transform = this->getComponent<TransformComponent>();
			auto translate = transform->getTranslate();
			auto rotation = transform->getQuaternion();

			cloth_ = clothFeature->getContext()->createCloth(nv::cloth::Range<physx::PxVec4>(positions.data(), positions.data() + positions.size()), *fabric);
			cloth_->setUserData(this);
			cloth_->setGravity(gravity);
			cloth_->setDragCoefficient(0.1f);
			cloth_->setTranslation(physx::PxVec3(translate.x, translate.y, translate.z));
			cloth_->setRotation(physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w));
			cloth_->clearInertia();

			nv::cloth::Range<physx::PxVec4> motionConstraints = cloth_->getMotionConstraints();
			for (int i = 0; i < (int)motionConstraints.size(); i++)
				motionConstraints[i] = physx::PxVec4(positions[i].getXYZ(), 10.0f);

			std::vector<nv::cloth::PhaseConfig> phases(fabric->getNumPhases());
			for (int i = 0; i < fabric->getNumPhases(); i++)
			{
				switch (phaseTypeInfo[i])
				{
				case nv::cloth::ClothFabricPhaseType::eINVALID:
					phases[i].mPhaseIndex = i;
					phases[i].mStiffness = 0.5f;
					phases[i].mStiffnessMultiplier = 1.0f;
					phases[i].mCompressionLimit = 1.0f;
					phases[i].mStretchLimit = 1.0f;
					break;
				case nv::cloth::ClothFabricPhaseType::eVERTICAL:
					phases[i].mPhaseIndex = i;
					phases[i].mStiffness = 0.5f;
					phases[i].mStiffnessMultiplier = 1.0f;
					phases[i].mCompressionLimit = 1.0f;
					phases[i].mStretchLimit = 1.0f;
					break;
				case nv::cloth::ClothFabricPhaseType::eHORIZONTAL:
					phases[i].mPhaseIndex = i;
					phases[i].mStiffness = 0.5f;
					phases[i].mStiffnessMultiplier = 1.0f;
					phases[i].mCompressionLimit = 1.0f;
					phases[i].mStretchLimit = 1.0f;
					break;
				case nv::cloth::ClothFabricPhaseType::eBENDING:
					phases[i].mPhaseIndex = i;
					phases[i].mStiffness = 0.5f;
					phases[i].mStiffnessMultiplier = 1.0f;
					phases[i].mCompressionLimit = 1.0f;
					phases[i].mStretchLimit = 1.0f;
					break;
				case nv::cloth::ClothFabricPhaseType::eSHEARING:
					phases[i].mPhaseIndex = i;
					phases[i].mStiffness = 0.75f;
					phases[i].mStiffnessMultiplier = 1.0f;
					phases[i].mCompressionLimit = 1.0f;
					phases[i].mStretchLimit = 1.0f;
					break;
				}
			}

			cloth_->setPhaseConfig(nv::cloth::Range<nv::cloth::PhaseConfig>(phases.data(), phases.data() + fabric->getNumPhases()));
			clothFeature->getScene()->addCloth(cloth_);

			fabric->decRefCount();
		}
	}
}