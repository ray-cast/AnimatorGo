#include <octoon/skinned_mesh_renderer_component.h>
#include <octoon/skinned_morph_component.h>
#include <octoon/skinned_texture_component.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubClass(SkinnedMeshRendererComponent, MeshRendererComponent, "SkinnedMeshRenderer")

	SkinnedMeshRendererComponent::SkinnedMeshRendererComponent() noexcept
		: needUpdate_(true)
		, clothEnable_(true)
		, morphEnable_(true)
		, textureEnable_(true)
	{
	}

	SkinnedMeshRendererComponent::SkinnedMeshRendererComponent(material::Materials&& materials, GameObjects&& transforms) noexcept
		: SkinnedMeshRendererComponent()
	{
		this->setMaterials(std::move(materials));
		this->setTransforms(std::move(transforms));
	}

	SkinnedMeshRendererComponent::SkinnedMeshRendererComponent(material::MaterialPtr&& material, GameObjects&& transforms) noexcept
		: SkinnedMeshRendererComponent()
	{
		this->setMaterial(std::move(material));
		this->setTransforms(std::move(transforms));
	}

	SkinnedMeshRendererComponent::SkinnedMeshRendererComponent(const material::Materials& materials, const GameObjects& transforms) noexcept
		: SkinnedMeshRendererComponent()
	{
		this->setMaterials(materials);
		this->setTransforms(transforms);
	}

	SkinnedMeshRendererComponent::SkinnedMeshRendererComponent(const material::MaterialPtr& material, const GameObjects& transforms) noexcept
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
		quaternions_.resize(transforms.size());
	}

	void
	SkinnedMeshRendererComponent::setTransforms(GameObjects&& transforms) noexcept
	{
		transforms_ = std::move(transforms);
		quaternions_.resize(transforms.size());
	}

	const GameObjects&
	SkinnedMeshRendererComponent::getTransforms() const noexcept
	{
		return transforms_;
	}

	void
	SkinnedMeshRendererComponent::setClothBlendEnable(bool enable) noexcept
	{
		clothEnable_ = enable;
	}

	bool
	SkinnedMeshRendererComponent::getClothBlendEnable() const noexcept
	{
		return clothEnable_;
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
	SkinnedMeshRendererComponent::setTextureBlendEnable(bool enable) noexcept
	{
		textureEnable_ = enable;
	}

	bool
	SkinnedMeshRendererComponent::getTextureBlendEnable() const noexcept
	{
		return textureEnable_;
	}

	const mesh::MeshPtr&
	SkinnedMeshRendererComponent::getSkinnedMesh() const noexcept
	{
		return this->skinnedMesh_;
	}

	void
	SkinnedMeshRendererComponent::uploadMeshData(const mesh::MeshPtr& mesh) noexcept
	{
		mesh_ = mesh;
		needUpdate_ = false;
		this->updateMeshData();
	}

	void
	SkinnedMeshRendererComponent::updateMeshData() noexcept
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

			skinnedMesh_->computeBoundingBox();
			skinnedMesh_->setDirty(true);

			MeshRendererComponent::uploadMeshData(skinnedMesh_);
		}
		else
		{
			MeshRendererComponent::uploadMeshData(nullptr);
		}
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
		for (std::size_t i = 0; i < quaternions_.size(); i++ )
		{
			auto& quat = quaternions_[i];
			auto transform = transforms_[i]->getComponent<TransformComponent>();

			if (transform->getQuaternion() != quat)
			{
				needUpdate_ = true;
				break;
			}
		}
	}

	void
	SkinnedMeshRendererComponent::onAnimationUpdate(const std::any& mesh) noexcept
	{
		needUpdate_ = true;
	}

	void
	SkinnedMeshRendererComponent::onAttachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isInstanceOf<SkinnedMorphComponent>())
			morphComponents_.push_back(component.get()->downcast<SkinnedMorphComponent>());
		else if (component->isInstanceOf<SkinnedTextureComponent>())
			textureComponents_.push_back(component.get()->downcast<SkinnedTextureComponent>());
		else if (component->isInstanceOf<ClothComponent>())
			clothComponents_.push_back(component.get()->downcast<ClothComponent>());		
	}

	void
	SkinnedMeshRendererComponent::onDetachComponent(const GameComponentPtr& component) noexcept
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
	SkinnedMeshRendererComponent::onMeshReplace(const std::any& data) noexcept
	{
		this->uploadMeshData(std::any_cast<mesh::MeshPtr>(data));
	}

	void
	SkinnedMeshRendererComponent::onPreRender(const camera::Camera& camera) noexcept
	{
		if (needUpdate_)
		{
			this->updateMeshData();
			needUpdate_ = false;
		}
	}

	void
	SkinnedMeshRendererComponent::updateJointData() noexcept
	{
		auto& bindposes = skinnedMesh_->getBindposes();
		auto boneSize = std::min(bindposes.size(), transforms_.size());

		joints_.resize(bindposes.size());
		
		for (std::size_t i = 0; i < boneSize; ++i)
		{
			auto transform = transforms_[i]->getComponent<TransformComponent>();
			quaternions_[i] = transform->getQuaternion();
			joints_[i] = math::transformMultiply(transform->getTransform(), bindposes[i]);
		}

		for (std::size_t i = boneSize; i < joints_.size(); ++i)
		{
			quaternions_[i].identity();
			joints_[i].makeIdentity();
		}

		/*if (!jointData_)
		{
			hal::GraphicsDataDesc jointDesc;
			jointDesc.setStreamSize(sizeof(math::float4x4) * static_cast<std::uint16_t>(joints_.size()));
			jointDesc.setUsage(hal::GraphicsUsageFlagBits::WriteBit);
			jointDesc.setType(hal::GraphicsDataType::UniformBuffer);

			jointData_ = video::Renderer::instance()->createGraphicsData(jointDesc);
		}*/
	}

	void
	SkinnedMeshRendererComponent::updateBoneData() noexcept
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
	SkinnedMeshRendererComponent::updateClothBlendData() noexcept
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
	SkinnedMeshRendererComponent::updateMorphBlendData() noexcept
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
	SkinnedMeshRendererComponent::updateTextureBlendData() noexcept
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