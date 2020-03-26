#include <octoon/offline_skinned_mesh_renderer_component.h>
#include <octoon/offline_feature.h>
#include <octoon/skinned_morph_component.h>
#include <octoon/skinned_texture_component.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubClass(OfflineSkinnedMeshRendererComponent, OfflineMeshRendererComponent, "OfflineSkinnedMeshRendererComponent")

	OfflineSkinnedMeshRendererComponent::OfflineSkinnedMeshRendererComponent() noexcept
		: needUpdate_(true)
		, clothEnable_(true)
		, morphEnable_(false)
		, textureEnable_(false)
	{
	}

	OfflineSkinnedMeshRendererComponent::OfflineSkinnedMeshRendererComponent(material::Materials&& materials, GameObjects&& transforms) noexcept
		: OfflineSkinnedMeshRendererComponent()
	{
		this->setMaterials(std::move(materials));
		this->setTransforms(std::move(transforms));
	}

	OfflineSkinnedMeshRendererComponent::OfflineSkinnedMeshRendererComponent(material::MaterialPtr&& material, GameObjects&& transforms) noexcept
		: OfflineSkinnedMeshRendererComponent()
	{
		this->setMaterial(std::move(material));
		this->setTransforms(std::move(transforms));
	}

	OfflineSkinnedMeshRendererComponent::OfflineSkinnedMeshRendererComponent(const material::Materials& materials, const GameObjects& transforms) noexcept
		: OfflineSkinnedMeshRendererComponent()
	{
		this->setMaterials(materials);
		this->setTransforms(transforms);
	}

	OfflineSkinnedMeshRendererComponent::OfflineSkinnedMeshRendererComponent(const material::MaterialPtr& material, const GameObjects& transforms) noexcept
		: OfflineSkinnedMeshRendererComponent()
	{
		this->setMaterial(material);
		this->setTransforms(transforms);
	}

	OfflineSkinnedMeshRendererComponent::~OfflineSkinnedMeshRendererComponent() noexcept
	{
	}

	void
	OfflineSkinnedMeshRendererComponent::setTransforms(const GameObjects& transforms) noexcept
	{
		transforms_ = transforms;
	}

	void
	OfflineSkinnedMeshRendererComponent::setTransforms(GameObjects&& transforms) noexcept
	{
		transforms_ = std::move(transforms);
	}

	const GameObjects&
	OfflineSkinnedMeshRendererComponent::getTransforms() const noexcept
	{
		return transforms_;
	}

	void
	OfflineSkinnedMeshRendererComponent::setClothBlendEnable(bool enable) noexcept
	{
		clothEnable_ = enable;
	}

	bool
	OfflineSkinnedMeshRendererComponent::getClothBlendEnable() const noexcept
	{
		return clothEnable_;
	}

	void
	OfflineSkinnedMeshRendererComponent::setMorphBlendEnable(bool enable) noexcept
	{
		morphEnable_ = enable;
	}

	bool
	OfflineSkinnedMeshRendererComponent::getMorphBlendEnable() const noexcept
	{
		return morphEnable_;
	}

	void
	OfflineSkinnedMeshRendererComponent::setTextureBlendEnable(bool enable) noexcept
	{
		textureEnable_ = enable;
	}

	bool 
	OfflineSkinnedMeshRendererComponent::getTextureBlendEnable() const noexcept
	{
		return textureEnable_;
	}

	void
	OfflineSkinnedMeshRendererComponent::uploadMeshData(const mesh::MeshPtr& mesh) noexcept
	{
		mesh_ = mesh;
		needUpdate_ = false;
		this->updateMeshData();
	}

	void
	OfflineSkinnedMeshRendererComponent::updateMeshData() noexcept
	{
		if (mesh_)
		{
			if (this->skinnedMesh_)
			{
				this->skinnedMesh_->setVertexArray(mesh_->getVertexArray());
				this->skinnedMesh_->setNormalArray(mesh_->getNormalArray());
			}
			else
			{
				skinnedMesh_ = mesh_->clone();
			}

			this->updateJointData();
			this->updateClothBlendData();
			this->updateMorphBlendData();
			this->updateTextureBlendData();
			this->updateBoneData();

			OfflineMeshRendererComponent::uploadMeshData(skinnedMesh_);
		}
		else
		{
			OfflineMeshRendererComponent::uploadMeshData(nullptr);
		}
	}

	GameComponentPtr
	OfflineSkinnedMeshRendererComponent::clone() const noexcept
	{
		auto instance = std::make_shared<OfflineSkinnedMeshRendererComponent>();
		instance->setName(this->getName());
		instance->setTransforms(this->getTransforms());
		instance->setMaterial(this->getMaterial() ? (this->isSharedMaterial() ? this->getMaterial() : this->getMaterial()->clone()) : nullptr, this->isSharedMaterial());

		return instance;
	}

	void
	OfflineSkinnedMeshRendererComponent::onActivate() noexcept
	{
		this->addMessageListener("octoon:animation:update", std::bind(&OfflineSkinnedMeshRendererComponent::onAnimationUpdate, this, std::placeholders::_1));
		OfflineMeshRendererComponent::onActivate();
	}

	void
	OfflineSkinnedMeshRendererComponent::onDeactivate() noexcept
	{
		this->removeMessageListener("octoon:animation:update", std::bind(&OfflineSkinnedMeshRendererComponent::onAnimationUpdate, this, std::placeholders::_1));
		OfflineMeshRendererComponent::onDeactivate();
	}

	void
	OfflineSkinnedMeshRendererComponent::onAnimationUpdate(const std::any& data) noexcept
	{
		auto offlineFeature = this->getFeature<OfflineFeature>();
		if (offlineFeature)
			offlineFeature->setFramebufferDirty(true);

		this->needUpdate_ = true;
	}

	void
	OfflineSkinnedMeshRendererComponent::onMeshReplace(const std::any& data) noexcept
	{
		this->uploadMeshData(std::any_cast<mesh::MeshPtr>(data));

		auto offlineFeature = this->getFeature<OfflineFeature>();
		if (offlineFeature)
			offlineFeature->setFramebufferDirty(true);
	}

	void
	OfflineSkinnedMeshRendererComponent::onAttachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isInstanceOf<SkinnedMorphComponent>())
			morphComponents_.push_back(component.get()->downcast<SkinnedMorphComponent>());
		else if (component->isInstanceOf<SkinnedTextureComponent>())
			textureComponents_.push_back(component.get()->downcast<SkinnedTextureComponent>());
		else if (component->isInstanceOf<ClothComponent>())
			clothComponents_.push_back(component.get()->downcast<ClothComponent>());
	}

	void
	OfflineSkinnedMeshRendererComponent::onDetachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isInstanceOf<SkinnedMorphComponent>())
		{
			auto it = std::find(morphComponents_.begin(), morphComponents_.end(), component.get());
			if (it != morphComponents_.end())
				morphComponents_.erase(it);
		}
		else if (component->isInstanceOf<SkinnedTextureComponent>())
		{
			auto it = std::find(textureComponents_.begin(), textureComponents_.end(), component.get());
			if (it != textureComponents_.end())
				textureComponents_.erase(it);
		}
		else if (component->isInstanceOf<ClothComponent>())
		{
			auto it = std::find(clothComponents_.begin(), clothComponents_.end(), component.get());
			if (it != clothComponents_.end())
				clothComponents_.erase(it);
		}
	}

	void
	OfflineSkinnedMeshRendererComponent::onPreRender() noexcept
	{
		OfflineMeshRendererComponent::onPreRender();

		if (mesh_ && needUpdate_)
		{
			this->updateMeshData();
			needUpdate_ = false;
		}
	}

	void
	OfflineSkinnedMeshRendererComponent::updateJointData() noexcept
	{
		joints_.resize(skinnedMesh_->getBindposes().size());

		auto& bindposes = skinnedMesh_->getBindposes();
		if (bindposes.size() != transforms_.size())
		{
			for (std::size_t i = 0; i < transforms_.size(); ++i)
				joints_[i].makeIdentity();
		}
		else
		{
			for (std::size_t i = 0; i < transforms_.size(); ++i)
				joints_[i] = math::transformMultiply(transforms_[i]->getComponent<TransformComponent>()->getTransform(), bindposes[i]);
		}
	}

	void
	OfflineSkinnedMeshRendererComponent::updateBoneData() noexcept
	{
		auto& vertices = skinnedMesh_->getVertexArray();
		auto& normals = skinnedMesh_->getNormalArray();
		auto& weights = skinnedMesh_->getWeightArray();

		auto numVertices = skinnedMesh_->getNumVertices();

#		pragma omp parallel for num_threads(4)
		for (std::int32_t i = 0; i < numVertices; i++)
		{
			auto& blend = weights[i];

			math::float3 v = math::float3::Zero;
			math::float3 n = math::float3::Zero;

			for (std::uint8_t j = 0; j < 4; j++)
			{
				auto w = blend.weights[j];
				if (w == 0.0f)
					break;
				v += (joints_[blend.bones[j]] * vertices[i]) * w;
				n += ((math::float3x3)joints_[blend.bones[j]] * normals[i]) * w;
			}

			vertices[i] = v;
			normals[i] = n;
		}
	}

	void
	OfflineSkinnedMeshRendererComponent::updateClothBlendData() noexcept
	{
		if (clothEnable_)
		{
			auto& dstVertices = skinnedMesh_->getVertexArray();

			for (auto& it : clothComponents_)
			{
				auto& indices = it->getIndices();
				auto& partices = it->getPartices();

				std::size_t numIndices = indices.size();
				for (std::size_t i = 0; i < numIndices; i++)
					dstVertices[indices[i]] = partices[i].xyz();
			}
		}
	}

	void
	OfflineSkinnedMeshRendererComponent::updateMorphBlendData() noexcept
	{
		if (morphEnable_)
		{
			auto& dstVertices = skinnedMesh_->getVertexArray();

			for (auto& it : morphComponents_)
			{
				auto control = it->getControl();
				if (control > 0.0f)
				{
					auto& indices = it->getIndices();
					auto& offsets = it->getOffsets();

					auto numIndices = indices.size();

					for (std::size_t i = 0; i < numIndices; i++)
						dstVertices[indices[i]] += offsets[i] * control;
				}
			}
		}
	}

	void
	OfflineSkinnedMeshRendererComponent::updateTextureBlendData() noexcept
	{
		if (textureEnable_)
		{
			auto& dstTextures = skinnedMesh_->getTexcoordArray();

			for (auto& it : textureComponents_)
			{
				auto control = it->getControl();
				if (control > 0.0f)
				{
					auto& indices = it->getIndices();
					auto& offsets = it->getOffsets();

					auto numIndices = indices.size();

					for (std::size_t i = 0; i < numIndices; i++)
						dstTextures[indices[i]] += offsets[i] * control;
				}
			}
		}
	}
}