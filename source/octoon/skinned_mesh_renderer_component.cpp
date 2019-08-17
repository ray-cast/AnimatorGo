#include <octoon/skinned_mesh_renderer_component.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubClass(SkinnedMeshRendererComponent, RenderComponent, "SkinnedMeshRenderer")

	SkinnedMeshRendererComponent::SkinnedMeshRendererComponent() noexcept
		: needUpdate_(true)
	{
	}

	SkinnedMeshRendererComponent::SkinnedMeshRendererComponent(model::MaterialPtr&& material, GameObjects&& transforms) noexcept
		: needUpdate_(true)
	{
		this->setMaterial(std::move(material));
		this->setTransforms(std::move(transforms));
	}

	SkinnedMeshRendererComponent::SkinnedMeshRendererComponent(const model::MaterialPtr& material, const GameObjects& transforms) noexcept
		: needUpdate_(true)
	{
		this->setMaterial(material);
		this->setTransforms(transforms);
	}

	SkinnedMeshRendererComponent::~SkinnedMeshRendererComponent() noexcept
	{
	}

	void
	SkinnedMeshRendererComponent::setTransforms(const GameObjects& transforms) noexcept
	{
		transforms_ = transforms;
	}

	void
	SkinnedMeshRendererComponent::setTransforms(GameObjects&& transforms) noexcept
	{
		transforms_ = std::move(transforms);
	}

	const GameObjects&
	SkinnedMeshRendererComponent::getTransforms() const noexcept
	{
		return transforms_;
	}

	void
	SkinnedMeshRendererComponent::uploadMeshData(const model::Mesh& mesh) noexcept
	{
		std::vector<math::float4x4> joints(transforms_.size());

		auto& bindposes = mesh_->getBindposes();
		if (bindposes.size() != transforms_.size())
		{
			for (std::size_t i = 0; i < transforms_.size(); ++i)
				joints[i].makeIdentity();
		}
		else
		{
			for (std::size_t i = 0; i < transforms_.size(); ++i)
				joints[i] = math::transformMultiply(transforms_[i]->getComponent<TransformComponent>()->getTransform(), bindposes[i]);
		}

		auto& vertices = mesh_->getVertexArray();
		auto& normals = mesh_->getNormalArray();
		auto& weights = mesh_->getWeightArray();

#pragma omp parallel for num_threads(4)
		for (std::int32_t i = 0; i < (std::int32_t)vertices.size(); i++)
		{
			math::float3 v = math::float3::Zero;
			math::float3 n = math::float3::Zero;

			for (std::uint8_t j = 0; j < 4; j++)
			{
				auto w = weights[i].weights[j];
				if (w == 0.0f)
					break;
				v += (joints[weights[i].bones[j]] * vertices[i]) * w;
				n += ((math::float3x3)joints[weights[i].bones[j]] * normals[i]) * w;
			}

			skinnedMesh_->getVertexArray()[i] = v;
			skinnedMesh_->getNormalArray()[i] = n;
		}

		MeshRendererComponent::uploadMeshData(*skinnedMesh_);
	}

	GameComponentPtr
	SkinnedMeshRendererComponent::clone() const noexcept
	{
		auto instance = std::make_shared<SkinnedMeshRendererComponent>();
		instance->setName(this->getName());
		instance->setTransforms(this->getTransforms());
		instance->setMaterial(this->getMaterial() ? (this->isSharedMaterial() ? this->getMaterial() : this->getMaterial()->clone()) : nullptr, this->isSharedMaterial());

		return instance;
	}

	void
	SkinnedMeshRendererComponent::onActivate() noexcept
	{
		this->addComponentDispatch(GameDispatchType::FixedUpdate);
		this->addMessageListener("octoon:animation:update", std::bind(&SkinnedMeshRendererComponent::onAnimationUpdate, this, std::placeholders::_1));
		MeshRendererComponent::onActivate();
	}

	void
	SkinnedMeshRendererComponent::onDeactivate() noexcept
	{
		mesh_.reset();
		skinnedMesh_.reset();
		this->removeComponentDispatch(GameDispatchType::FixedUpdate);
		this->removeMessageListener("octoon:animation:update", std::bind(&SkinnedMeshRendererComponent::onAnimationUpdate, this, std::placeholders::_1));
		MeshRendererComponent::onDeactivate();
	}

	void
	SkinnedMeshRendererComponent::onFixedUpdate() noexcept
	{
		needUpdate_ = true;
	}

	void
	SkinnedMeshRendererComponent::onAnimationUpdate(const runtime::any& mesh) noexcept
	{
		needUpdate_ = true;
	}

	void
	SkinnedMeshRendererComponent::onMeshReplace(const runtime::any& data_) noexcept
	{
		auto mesh = runtime::any_cast<model::MeshPtr>(data_);
		if (mesh)
		{
			mesh_ = skinnedMesh_;
			skinnedMesh_ = mesh->clone();
		}

		needUpdate_ = true;
	}

	void
	SkinnedMeshRendererComponent::onPreRender(const video::Camera& camera) noexcept
	{
		if (needUpdate_ && skinnedMesh_)
		{
			this->uploadMeshData(*mesh_);
			needUpdate_ = false;
		}
	}
}