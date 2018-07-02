#include <octoon/text_component.h>
#include <octoon/model/text_meshing.h>

namespace octoon
{
	OctoonImplementSubClass(TextComponent, GameComponent, "Text")

	TextComponent::TextComponent() noexcept
		: isSharedText_(false)
	{
	}

	TextComponent::TextComponent(std::string&& mesh, bool sharedText) noexcept
	{
		this->setText(std::move(mesh), sharedText);
	}

	TextComponent::TextComponent(const std::string& mesh, bool sharedText) noexcept
	{
		this->setText(mesh, sharedText);
	}

	TextComponent::~TextComponent() noexcept
	{
	}

	void
	TextComponent::setText(std::string&& mesh, bool sharedText) noexcept
	{
		if (text_ != mesh)
		{
			text_ = std::move(mesh);
			isSharedText_ = sharedText;
			this->onTextReplace(text_);
		}
	}

	void
	TextComponent::setText(const std::string& mesh, bool sharedText) noexcept
	{
		if (text_ != mesh)
		{
			text_ = mesh;
			isSharedText_ = sharedText;
			this->onTextReplace(text_);
		}
	}

	const std::string&
	TextComponent::getText() const noexcept
	{
		return text_;
	}

	bool
	TextComponent::isSharedText() const noexcept
	{
		return isSharedText_;
	}

	void
	TextComponent::uploadTextData() noexcept
	{
		this->onTextReplace(text_);
	}

	GameComponentPtr
	TextComponent::clone() const noexcept
	{
		auto instance = std::make_shared<TextComponent>();
		instance->setName(instance->getName());
		instance->setText(text_);

		return instance;
	}

	void
	TextComponent::onTextReplace(const std::string& mesh) noexcept
	{
		//mesh_ = model::makeMesh(model::makeTextContours(u16str_, { fontPath, fontsize }, 8), 0.0f);

		if (this->getGameObject())
			this->sendMessage("octoon::mesh::update", mesh);
	}
}