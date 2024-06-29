#include <octoon/skinned_morph_component.h>
#include <octoon/asset_database.h>
#include <octoon/asset_importer.h>
#include <octoon/runtime/base64.h>

namespace octoon
{
	OctoonImplementSubClass(SkinnedMorphComponent, SkinnedComponent, "SkinnedMorph")

	SkinnedMorphComponent::SkinnedMorphComponent() noexcept
	{
	}

	SkinnedMorphComponent::SkinnedMorphComponent(math::float3s&& offsets, math::uint1s&& indices, float control) noexcept
	{
		offsets_ = std::move(offsets);
		indices_ = std::move(indices);
	}

	SkinnedMorphComponent::SkinnedMorphComponent(const math::float3s& offsets, const math::uint1s& indices, float control) noexcept
	{
		offsets_ = offsets;
		indices_ = indices;
	}

	SkinnedMorphComponent::~SkinnedMorphComponent() noexcept
	{
	}

	void
	SkinnedMorphComponent::setOffsets(math::float3s&& offsets) noexcept
	{
		offsets_ = std::move(offsets);
	}

	void
	SkinnedMorphComponent::setOffsets(const math::float3s& offsets) noexcept
	{
		offsets_ = offsets;
	}

	const math::float3s&
	SkinnedMorphComponent::getOffsets() const noexcept
	{
		return offsets_;
	}

	void
	SkinnedMorphComponent::setIndices(math::uint1s&& indices) noexcept
	{
		indices_ = std::move(indices);
	}

	void
	SkinnedMorphComponent::setIndices(const math::uint1s& indices) noexcept
	{
		indices_ = indices;
	}

	const math::uint1s&
	SkinnedMorphComponent::getIndices() const noexcept
	{
		return indices_;
	}

	void
	SkinnedMorphComponent::load(const nlohmann::json& json) noexcept(false)
	{
		GameComponent::load(json);

		if (json.contains("guid") && json.contains("localId"))
		{
			auto guid = json["guid"].get<std::string>();
			auto localId = json["localId"].get<std::int64_t>();

			auto component = AssetDatabase::instance()->loadAsset<SkinnedMorphComponent>(guid, localId);
			if (component)
			{
				this->setControl(component->getControl());
				this->setIndices(component->getIndices());
				this->setOffsets(component->getOffsets());
			}
		}
		else
		{
			if (json.contains("control"))
				this->setControl(json["control"].get<float>());

			if (json.contains("offsets"))
			{
				std::vector<char> buffer = base64_decode(json["offsets"].get<std::string>());
				this->offsets_.resize(buffer.size() / sizeof(math::float3));
				std::memcpy(this->offsets_.data(), buffer.data(), buffer.size());
			}

			if (json.contains("indices"))
			{
				std::vector<char> buffer = base64_decode(json["indices"].get<std::string>());
				this->indices_.resize(buffer.size() / sizeof(unsigned int));
				std::memcpy(this->offsets_.data(), buffer.data(), buffer.size());
			}
		}
	}

	void
	SkinnedMorphComponent::save(nlohmann::json& json) const noexcept(false)
	{
		GameComponent::save(json);

		std::string guid;
		std::int64_t localId;

		if (AssetDatabase::instance()->getGUIDAndLocalIdentifier(this->shared_from_this(), guid, localId))
		{
			json["guid"] = guid;
			json["localId"] = localId;
		}
		else
		{
			json["control"] = this->getControl();
			json["offsets"] = base64_encode((unsigned char*)offsets_.data(), offsets_.size() * sizeof(math::float3));
			json["indices"] = base64_encode((unsigned char*)indices_.data(), indices_.size() * sizeof(unsigned int));
		}
	}

	GameComponentPtr
	SkinnedMorphComponent::clone() const noexcept
	{
		auto instance = std::make_shared<SkinnedMorphComponent>();
		instance->setName(this->getName());
		instance->setControl(this->getControl());
		instance->setOffsets(this->getOffsets());
		instance->setIndices(this->getIndices());
		return instance;
	}

	void 
	SkinnedMorphComponent::onActivate() noexcept
	{
		if (!this->getName().empty())
			this->addMessageListener(this->getName(), std::bind(&SkinnedMorphComponent::onAnimationUpdate, this, std::placeholders::_1));
	}

	void
	SkinnedMorphComponent::onDeactivate() noexcept
	{
		if (!this->getName().empty())
			this->removeMessageListener(this->getName(), std::bind(&SkinnedMorphComponent::onAnimationUpdate, this, std::placeholders::_1));
	}

	void
	SkinnedMorphComponent::onAnimationUpdate(const std::any& value) noexcept
	{
		assert(value.type() == typeid(math::Variant));
		this->setControl(std::any_cast<math::Variant>(value).getFloat());
	}

	void
	SkinnedMorphComponent::onTargetReplace(std::string_view name) noexcept
	{
		if (!this->getName().empty())
			this->tryRemoveMessageListener(this->getName(), std::bind(&SkinnedMorphComponent::onAnimationUpdate, this, std::placeholders::_1));
		if (!name.empty())
			this->tryAddMessageListener(name, std::bind(&SkinnedMorphComponent::onAnimationUpdate, this, std::placeholders::_1));
	}
}