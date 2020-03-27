#include <octoon/text_component.h>
#include <octoon/model/text_meshing.h>

namespace octoon
{
	OctoonImplementSubClass(TextComponent, GameComponent, "Text")

	TextComponent::TextComponent() noexcept
		: isSharedText_(false)
		, align_(font::TextAlign::Left)
	{
	}

	TextComponent::TextComponent(std::string_view mesh, bool sharedText) noexcept
		: TextComponent()
	{
		this->setText(mesh, sharedText);
	}

	TextComponent::~TextComponent() noexcept
	{
	}

	void
	TextComponent::setText(std::string_view mesh, bool sharedText) noexcept
	{
		if (u8str_ != mesh)
		{
			u8str_ = mesh;
			isSharedText_ = sharedText;
			this->uploadTextData();
		}
	}

	const std::string&
	TextComponent::getText() const noexcept
	{
		return u8str_;
	}

	void
	TextComponent::setTextMeshing(std::shared_ptr<font::TextMeshing>&& meshing) noexcept
	{
		if (meshing_ != meshing)
		{
			meshing_ = std::move(meshing);
			this->uploadTextData();
		}
	}

	void
	TextComponent::setTextMeshing(const std::shared_ptr<font::TextMeshing>& meshing) noexcept
	{
		if (meshing_ != meshing)
		{
			meshing_ = meshing;
			this->uploadTextData();
		}
	}

	const std::shared_ptr<font::TextMeshing>&
	TextComponent::getTextMeshing() const noexcept
	{
		return meshing_;
	}

	void
	TextComponent::setTextAlign(font::TextAlign align) noexcept
	{
		if (align_ != align)
		{
			align_ = align;
			this->uploadTextData();
		}
	}

	font::TextAlign
	TextComponent::getTextAlign() const noexcept
	{
		return align_;
	}

	bool
	TextComponent::isSharedText() const noexcept
	{
		return isSharedText_;
	}

	void
	TextComponent::onActivate() except
	{
		this->addMessageListener("octoon:mesh:get", std::bind(&TextComponent::onTextReplace, this));
		this->uploadTextData();
	}

	void
	TextComponent::onDeactivate() noexcept
	{
	}

	void
	TextComponent::uploadTextData() noexcept
	{
		std::string::size_type len = std::strlen(u8str_.c_str());

		std::wstring u16str;
		u16str.reserve(len);

		const unsigned char* p = (unsigned char*)(u8str_.c_str());
		if (len > 3 && p[0] == 0xEF && p[1] == 0xBB && p[2] == 0xBF) {
			p += 3;
			len -= 3;
		}

		bool is_ok = true;
		for (std::string::size_type i = 0; i < len; ++i)
		{
			uint32_t ch = p[i];
			if ((ch & 0x80) == 0)
			{
				u16str.push_back((char16_t)ch);
				continue;
			}

			switch (ch & 0xF0)
			{
			case 0xF0:
			{
				uint32_t c2 = p[++i];
				uint32_t c3 = p[++i];
				uint32_t c4 = p[++i];
				uint32_t codePoint = ((ch & 0x07U) << 18) | ((c2 & 0x3FU) << 12) | ((c3 & 0x3FU) << 6) | (c4 & 0x3FU);
				if (codePoint >= 0x10000)
				{
					codePoint -= 0x10000;
					u16str.push_back((char16_t)((codePoint >> 10) | 0xD800U));
					u16str.push_back((char16_t)((codePoint & 0x03FFU) | 0xDC00U));
				}
				else
				{
					u16str.push_back((char16_t)codePoint);
				}
			}
			break;
			case 0xE0:
			{
				uint32_t c2 = p[++i];
				uint32_t c3 = p[++i];
				uint32_t codePoint = ((ch & 0x0FU) << 12) | ((c2 & 0x3FU) << 6) | (c3 & 0x3FU);
				u16str.push_back((char16_t)codePoint);
			}
			break;
			case 0xD0:
			case 0xC0:
			{
				uint32_t c2 = p[++i];
				uint32_t codePoint = ((ch & 0x1FU) << 12) | ((c2 & 0x3FU) << 6);
				u16str.push_back((char16_t)codePoint);
			}
			break;
			default:
				is_ok = false;
				break;
			}
		}

		if (is_ok)
		{
			if (meshing_)
				mesh_ = std::make_shared<mesh::Mesh>(model::makeMesh(font::makeTextContours(u16str, *meshing_, 8, align_), 0.0f, false));
			else
				mesh_ = std::make_shared<mesh::Mesh>(model::makeMesh(font::makeTextContours(u16str, { "../../system/fonts/DroidSansFallback.ttf", 24 }, 8, align_), 0.0f, false));
		}
		else
		{
			mesh_ = nullptr;
		}

		this->onTextReplace();
	}

	GameComponentPtr
	TextComponent::clone() const noexcept
	{
		auto instance = std::make_shared<TextComponent>();
		instance->setName(instance->getName());
		instance->setText(u8str_);
		return instance;
	}

	void
	TextComponent::onTextReplace() noexcept
	{
		if (this->getGameObject())
			this->sendMessage("octoon:mesh:update", mesh_);
	}
}