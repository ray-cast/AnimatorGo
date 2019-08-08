#include <octoon/cloth_component.h>
#include <octoon/transform_component.h>
#include <octoon/game_scene.h>
#include <octoon/timer_feature.h>
#include <octoon/rotation_limit_component.h>
#include <octoon/rotation_link_component.h>
#include <octoon/rotation_link_limit_component.h>
#include <octoon/cloth_feature.h>
#include <octoon/mesh_filter_component.h>

#include <NvCloth/Cloth.h>
#include <NvCloth/Factory.h>
#include <NvCloth/Solver.h>
#include <NvClothExt/ClothFabricCooker.h>

#include <PxPhysicsAPI.h>

namespace octoon
{
	OctoonImplementSubClass(ClothComponent, GameComponent, "Cloth")

	ClothComponent::ClothComponent() noexcept
		: cloth_(nullptr)
		, needUpdate_(false)
	{
	}

	ClothComponent::ClothComponent(const GameObjects& bone) noexcept
		: ClothComponent()
	{
		this->setBones(bone);
	}

	ClothComponent::~ClothComponent() noexcept
	{
	}

	void
	ClothComponent::addBone(GameObjectPtr&& joint) noexcept
	{
		bones_.emplace_back(std::move(joint));
	}

	void
	ClothComponent::addBone(const GameObjectPtr& joint) noexcept
	{
		bones_.push_back(joint);
	}

	void
	ClothComponent::setBones(GameObjects&& bones) noexcept
	{
		bones_ = std::move(bones);
	}

	void
	ClothComponent::setBones(const GameObjects& bones) noexcept
	{
		bones_ = bones;
	}

	const GameObjects&
	ClothComponent::getBones() const noexcept
	{
		return bones_;
	}

	GameComponentPtr
	ClothComponent::clone() const noexcept
	{
		auto instance = std::make_shared<ClothComponent>();
		instance->setName(this->getName());
		instance->setBones(this->getBones());

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

			std::vector<float> mass(mesh.getVertexArray().size());
			for (std::size_t i = 0; i < mass.size(); i++)
				mass[i] = 100.0f;

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
				positions.push_back(physx::PxVec4(it.x, it.y, it.z, 100.0f));

			auto transform = this->getComponent<TransformComponent>();
			auto translate = transform->getTranslate();
			auto rotation = transform->getQuaternion();

			cloth_ = clothFeature->getContext()->createCloth(nv::cloth::Range<physx::PxVec4>(positions.data(), positions.data() + positions.size()), *fabric);
			cloth_->setUserData(this);
			cloth_->setGravity(gravity);
			cloth_->setTranslation(physx::PxVec3(translate.x, translate.y, translate.z));
			cloth_->setRotation(physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w));
			cloth_->clearInertia();

			nv::cloth::Range<physx::PxVec4> motionConstraints = cloth_->getMotionConstraints();
			for (int i = 0; i < (int)motionConstraints.size(); i++)
			{
				if (std::sqrt(std::max(0.0f, 12.5f - positions[i].y) * 0.2f) > 0.0f)
					motionConstraints[i] = physx::PxVec4(positions[i].getXYZ(), 1.0f);
				else
					motionConstraints[i] = physx::PxVec4(positions[i].getXYZ(), 0.0f);
			}

			nv::cloth::PhaseConfig* phases = new nv::cloth::PhaseConfig[fabric->getNumPhases()];
			for (int i = 0; i < fabric->getNumPhases(); i++)
			{
				switch (phaseTypeInfo[i])
				{
				case nv::cloth::ClothFabricPhaseType::eINVALID:
					break;
				case nv::cloth::ClothFabricPhaseType::eVERTICAL:
					break;
				case nv::cloth::ClothFabricPhaseType::eHORIZONTAL:
					break;
				case nv::cloth::ClothFabricPhaseType::eBENDING:
					break;
				case nv::cloth::ClothFabricPhaseType::eSHEARING:
					break;
				}

				phases[i].mPhaseIndex = i;
				phases[i].mStiffness = 0.0f;
				phases[i].mStiffnessMultiplier = 0.0f;
				phases[i].mCompressionLimit = 1.0f;
				phases[i].mStretchLimit = 1.0f;
			}

			cloth_->setPhaseConfig(nv::cloth::Range<nv::cloth::PhaseConfig>(phases, phases + fabric->getNumPhases()));

			fabric->decRefCount();

			clothFeature->getScene()->addCloth(cloth_);
		}
	}
}