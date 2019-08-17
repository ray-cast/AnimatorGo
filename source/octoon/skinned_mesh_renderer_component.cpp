#include <octoon/skinned_mesh_renderer_component.h>
#include <octoon/skinned_morph_component.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubClass(SkinnedMeshRendererComponent, RenderComponent, "SkinnedMeshRenderer")

	SkinnedMeshRendererComponent::SkinnedMeshRendererComponent() noexcept
		: needUpdate_(true)
		, morphEnable_(false)
	{
	}

	SkinnedMeshRendererComponent::SkinnedMeshRendererComponent(model::Materials&& materials, GameObjects&& transforms) noexcept
		: SkinnedMeshRendererComponent()
	{
		this->setMaterials(std::move(materials));
		this->setTransforms(std::move(transforms));
	}

	SkinnedMeshRendererComponent::SkinnedMeshRendererComponent(model::MaterialPtr&& material, GameObjects&& transforms) noexcept
		: SkinnedMeshRendererComponent()
	{
		this->setMaterial(std::move(material));
		this->setTransforms(std::move(transforms));
	}

	SkinnedMeshRendererComponent::SkinnedMeshRendererComponent(const model::Materials& materials, const GameObjects& transforms) noexcept
		: SkinnedMeshRendererComponent()
	{
		this->setMaterials(materials);
		this->setTransforms(transforms);
	}

	SkinnedMeshRendererComponent::SkinnedMeshRendererComponent(const model::MaterialPtr& material, const GameObjects& transforms) noexcept
		: SkinnedMeshRendererComponent()
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
	SkinnedMeshRendererComponent::setMorphBlendEnable(bool enable) noexcept
	{
		morphEnable_ = enable;
	}

	bool
	SkinnedMeshRendererComponent::getMorphBlendEnable() const noexcept
	{
		return morphEnable_;
	}

	void
	SkinnedMeshRendererComponent::uploadMeshData(const model::Mesh& mesh) noexcept
	{
		std::vector<math::float4x4> joints(transforms_.size());

		auto& bindposes = mesh.getBindposes();
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

		auto& vertices = mesh.getVertexArray();
		auto& normals = mesh.getNormalArray();
		auto& weights = mesh.getWeightArray();

		auto& dstVertices = skinnedMesh_->getVertexArray();
		auto& dstNormals = skinnedMesh_->getNormalArray();

		auto numVertices = mesh.getNumVertices();

#if !defined(_DEBUG)
#		pragma omp parallel for num_threads(4)
#endif
		for (std::size_t i = 0; i < numVertices; i++)
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

			dstVertices[i] = v;
			dstNormals[i] = n;
		}

		if (morphEnable_)
		{
			for (auto& it : skinnedComponents_)
			{
				auto control = it->getControl();
				if (control > 0.0f)
				{
					auto morph = it->downcast<SkinnedMorphComponent>();
					auto& indices = morph->getIndices();
					auto& offsets = morph->getOffsets();
					
					auto numIndices = indices.size();

					for (std::size_t i = 0; i < numIndices; i++)
					{
						auto offset = offsets[i];
						auto index = indices[i];

						dstVertices[index] += offset * control;
					}
				}
			}
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
	SkinnedMeshRendererComponent::onAttachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isInstanceOf<SkinnedMorphComponent>())
			skinnedComponents_.push_back(component.get()->downcast<SkinnedComponent>());
	}

	void
	SkinnedMeshRendererComponent::onDetachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isInstanceOf<SkinnedMorphComponent>())
		{
			auto it = std::find(skinnedComponents_.begin(), skinnedComponents_.end(), component.get());
			if (it != skinnedComponents_.end())
				skinnedComponents_.erase(it);
		}
	}

	void
	SkinnedMeshRendererComponent::onMeshReplace(const runtime::any& data_) noexcept
	{
		auto mesh = runtime::any_cast<model::MeshPtr>(data_);
		if (mesh)
		{
			mesh_ = mesh;
			skinnedMesh_ = mesh->clone();
		}

		this->uploadMeshData(*mesh_);
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