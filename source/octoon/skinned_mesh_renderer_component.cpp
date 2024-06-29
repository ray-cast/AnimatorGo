#include <octoon/skinned_mesh_renderer_component.h>
#include <octoon/skinned_morph_component.h>
#include <octoon/skinned_texture_component.h>
#include <octoon/transform_component.h>
#include <octoon/asset_database.h>
#include <octoon/asset_importer.h>
#include <omp.h>

namespace octoon
{
	OctoonImplementSubClass(SkinnedMeshRendererComponent, MeshRendererComponent, "SkinnedMeshRenderer")

	SkinnedMeshRendererComponent::SkinnedMeshRendererComponent() noexcept
		: needUpdate_(true)
		, clothEnable_(true)
		, morphEnable_(true)
		, textureEnable_(true)
		, automaticUpdate_(true)
		, updateWhenOffscreenEnable_(true)
	{
	}

	SkinnedMeshRendererComponent::SkinnedMeshRendererComponent(Materials&& materials, GameObjects&& bones) noexcept
		: SkinnedMeshRendererComponent()
	{
		this->setBones(std::move(bones));
		this->setMaterials(std::move(materials));
	}

	SkinnedMeshRendererComponent::SkinnedMeshRendererComponent(MaterialPtr&& material, GameObjects&& bones) noexcept
		: SkinnedMeshRendererComponent()
	{
		this->setBones(std::move(bones));
		this->setMaterial(std::move(material));
	}

	SkinnedMeshRendererComponent::SkinnedMeshRendererComponent(const Materials& materials, const GameObjects& bones) noexcept
		: SkinnedMeshRendererComponent()
	{
		this->setBones(bones);
		this->setMaterials(materials);
	}

	SkinnedMeshRendererComponent::SkinnedMeshRendererComponent(const MaterialPtr& material, const GameObjects& bones) noexcept
		: SkinnedMeshRendererComponent()
	{
		this->setBones(bones);
		this->setMaterial(material);
	}

	SkinnedMeshRendererComponent::~SkinnedMeshRendererComponent() noexcept
	{
	}

	void
	SkinnedMeshRendererComponent::setBones(const GameObjects& bones) noexcept
	{
		bones_ = bones;
		quaternions_.resize(bones_.size());
	}

	void
	SkinnedMeshRendererComponent::setBones(GameObjects&& bones) noexcept
	{
		bones_ = std::move(bones);
		quaternions_.resize(bones_.size());
	}

	const GameObjects&
	SkinnedMeshRendererComponent::getBones() const noexcept
	{
		return bones_;
	}

	void
	SkinnedMeshRendererComponent::setUpdateWhenOffscreen(bool enable) noexcept
	{
		updateWhenOffscreenEnable_ = true;
	}

	bool
	SkinnedMeshRendererComponent::getUpdateWhenOffscreen() const noexcept
	{
		return updateWhenOffscreenEnable_;
	}

	void
	SkinnedMeshRendererComponent::setAutomaticUpdate(bool enable) noexcept
	{
		this->automaticUpdate_ = enable;
	}

	bool
	SkinnedMeshRendererComponent::getAutomaticUpdate() const noexcept
	{
		return automaticUpdate_;
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

	const MeshPtr&
	SkinnedMeshRendererComponent::getSkinnedMesh() noexcept
	{
		this->updateMeshData();
		return this->skinnedMesh_;
	}

	void
	SkinnedMeshRendererComponent::uploadMeshData(const MeshPtr& mesh) noexcept
	{
		mesh_ = mesh;
		needUpdate_ = true;
	}

	void
	SkinnedMeshRendererComponent::uploadMeshData() noexcept
	{
		needUpdate_ = true;
	}

	void
	SkinnedMeshRendererComponent::load(const nlohmann::json& json) noexcept(false)
	{
		MeshRendererComponent::load(json);

		if (json.contains("automaticUpdate"))
			this->setAutomaticUpdate(json["automaticUpdate"].get<bool>());
		if (json.contains("clothBlendEnable"))
			this->setClothBlendEnable(json["clothBlendEnable"].get<bool>());
		if (json.contains("morphBlendEnable"))
			this->setMorphBlendEnable(json["morphBlendEnable"].get<bool>());
		if (json.contains("textureBlendEnable"))
			this->setTextureBlendEnable(json["textureBlendEnable"].get<bool>());
		if (json.contains("updateWhenOffscreen"))
			this->setUpdateWhenOffscreen(json["updateWhenOffscreen"].get<bool>());

		if (json.contains("bones"))
		{
			GameObjects bones;

			for (auto& it : json["bones"])
			{
				auto guid = it["guid"].get<std::string>();
				auto localId = it["localId"].get<int>();

				bones.push_back(AssetDatabase::instance()->loadAsset<GameObject>(guid, localId));
			}

			this->setBones(std::move(bones));
		}
	}

	void
	SkinnedMeshRendererComponent::save(nlohmann::json& json) const noexcept(false)
	{
		MeshRendererComponent::save(json);

		json["automaticUpdate"] = this->getAutomaticUpdate();
		json["clothBlendEnable"] = this->getClothBlendEnable();
		json["morphBlendEnable"] = this->getMorphBlendEnable();
		json["textureBlendEnable"] = this->getTextureBlendEnable();
		json["updateWhenOffscreen"] = this->getUpdateWhenOffscreen();

		if (!this->getBones().empty())
		{
			auto& bonesJson = json["bones"];

			for (auto& it : bones_)
			{
				std::string guid;
				std::int64_t localId;

				if (AssetDatabase::instance()->getGUIDAndLocalIdentifier(it, guid, localId))
				{
					nlohmann::json bone;
					bone["guid"] = guid;
					bone["localId"] = localId;

					bonesJson.push_back(std::move(bone));
				}
			}
		}
	}

	GameComponentPtr
	SkinnedMeshRendererComponent::clone() const noexcept
	{
		auto instance = std::make_shared<SkinnedMeshRendererComponent>();
		instance->setName(this->getName());
		instance->setBones(this->getBones());
		instance->setMaterials(this->getMaterials());
		instance->setAutomaticUpdate(this->getAutomaticUpdate());
		instance->setClothBlendEnable(this->getClothBlendEnable());
		instance->setMorphBlendEnable(this->getMorphBlendEnable());
		instance->setTextureBlendEnable(this->getTextureBlendEnable());
		instance->setUpdateWhenOffscreen(this->getUpdateWhenOffscreen());

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
		if (!needUpdate_ && this->automaticUpdate_)
		{
			for (std::size_t i = 0; i < quaternions_.size(); i++)
			{
				auto transform = bones_[i]->getComponent<TransformComponent>();
				if (transform->getRotation() != quaternions_[i])
				{
					needUpdate_ = true;
					break;
				}
			}
		}
	}

	void
	SkinnedMeshRendererComponent::onAnimationUpdate(const std::any& mesh) noexcept
	{
		if (automaticUpdate_) needUpdate_ = true;
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
		this->uploadMeshData(std::any_cast<MeshPtr>(data));
	}

	void
	SkinnedMeshRendererComponent::onPreRender(const Camera& camera) noexcept
	{
		if (updateWhenOffscreenEnable_)
			this->updateMeshData();
	}

	void
	SkinnedMeshRendererComponent::updateJointData() noexcept
	{
		auto& bindposes = skinnedMesh_->getBindposes();
		auto boneSize = std::min(bindposes.size(), bones_.size());

		if (joints_.size() != bindposes.size())
			joints_.resize(bindposes.size());
		
		for (std::size_t i = 0; i < boneSize; ++i)
		{
			auto transform = bones_[i]->getComponent<TransformComponent>();
			quaternions_[i] = transform->getRotation();
			joints_[i] = math::transformMultiply(transform->getTransform(), bindposes[i]);
		}

		for (std::size_t i = boneSize; i < joints_.size(); ++i)
			joints_[i].makeIdentity();
	}

	void
	SkinnedMeshRendererComponent::updateBoneData() noexcept
	{
		auto& vertices = skinnedMesh_->getVertexArray();
		auto& normals = skinnedMesh_->getNormalArray();
		auto& weights = skinnedMesh_->getWeightArray();

		auto numVertices = skinnedMesh_->getNumVertices();
		auto numProcs = omp_get_num_procs() / 2;

#		pragma omp parallel for num_threads(numProcs) schedule(static)
		for (std::int32_t i = 0; i < numVertices; i++)
		{
			auto& blend = weights[i];

			auto w0 = blend.weights[0];
			auto w1 = blend.weights[1];
			auto w2 = blend.weights[2];
			auto w3 = blend.weights[3];

			math::float3 v = vertices[i];
			math::float3 n = normals[i];

			math::float3 sumVertex = math::float3::Zero;
			math::float3 sumNormal = math::float3::Zero;

			if (w0 != 0.0f) { auto& m = joints_[blend.bones[0]]; sumVertex += (m * v) * w0; sumNormal += ((math::float3x3)m * n) * w0; }
			if (w1 != 0.0f) { auto& m = joints_[blend.bones[1]]; sumVertex += (m * v) * w1; sumNormal += ((math::float3x3)m * n) * w1; }
			if (w2 != 0.0f) { auto& m = joints_[blend.bones[2]]; sumVertex += (m * v) * w2; sumNormal += ((math::float3x3)m * n) * w2; }
			if (w3 != 0.0f) { auto& m = joints_[blend.bones[3]]; sumVertex += (m * v) * w3; sumNormal += ((math::float3x3)m * n) * w3; }

			normals[i] = sumNormal;
			vertices[i] = sumVertex;
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

	void
	SkinnedMeshRendererComponent::updateMeshData() noexcept
	{
		if (needUpdate_)
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

			needUpdate_ = false;
		}
	}
}