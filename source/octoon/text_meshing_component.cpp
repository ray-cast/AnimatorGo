#include <octoon/text_meshing_component.h>
#include <octoon/game_object.h>
#include <octoon/transform_component.h>
#include <octoon/mesh_renderer_component.h>
#include <octoon/video/mesh.h>
#include <octoon/video/text_file.h>
#include <octoon/video/text_contour_group.h>
#include <octoon/runtime/except.h>

#include <ft2build.h>
#include <freetype/ftglyph.h>

namespace octoon
{
	OctoonImplementSubClass(TextMeshingComponent, MeshFilterComponent, "TextMeshingComponent")

	TextMeshingComponent::TextMeshingComponent() noexcept
		: font_(nullptr)
		, bezierSteps_(6)
	{
	}

	TextMeshingComponent::~TextMeshingComponent() noexcept
	{
	}

	void
	TextMeshingComponent::setFont(video::TextFilePtr&& font) noexcept
	{
		font_ = std::move(font);
	}

	void
	TextMeshingComponent::setFont(const video::TextFilePtr& font) noexcept
	{
		font_ = font;
	}

	const video::TextFilePtr&
	TextMeshingComponent::getFont() const noexcept
	{
		return font_;
	}

	void
	TextMeshingComponent::setText(std::wstring&& text) noexcept
	{
		string_ = std::move(text);

		if (this->getActive() && this->getGameObject()->getActive())
			this->buildContours(string_);
	}

	void
	TextMeshingComponent::setText(const std::wstring& text) noexcept
	{
		string_ = text;

		if (this->getActive() && this->getGameObject()->getActive())
			this->buildContours(string_);
	}

	const std::wstring&
	TextMeshingComponent::getText() const noexcept
	{
		return string_;
	}

	void
	TextMeshingComponent::setBezierSteps(std::uint16_t bezierSteps) noexcept
	{
		bezierSteps_ = bezierSteps;
	}

	std::uint16_t
	TextMeshingComponent::getBezierSteps() const noexcept
	{
		return bezierSteps_;
	}

	void
	TextMeshingComponent::onActivate() noexcept(false)
	{
		if (!string_.empty())
			this->buildContours(string_);
	}

	void
	TextMeshingComponent::onDeactivate() noexcept
	{
		this->getGameObject()->cleanupChildren();
	}

	void
	TextMeshingComponent::buildContours(const std::wstring& string) noexcept(false)
	{
		if (!font_)
			throw runtime::null_reference::create("please call setFont() before process()");

		if (::FT_Set_Pixel_Sizes((FT_Face)(font_->getFont()), 12, 12))
			throw runtime::runtime_error::create("FT_Set_Char_Size() failed (there is probably a problem with your font size", 3);

		FT_Face ftface = (FT_Face)font_->getFont();

		float offset = 0;

		for (auto& ch : string)
		{
			FT_UInt index = FT_Get_Char_Index(ftface, ch);
			if (::FT_Load_Glyph(ftface, index, FT_LOAD_DEFAULT))
				throw runtime::runtime_error::create("FT_Load_Glyph failed.");

			FT_Glyph glyph;
			if (::FT_Get_Glyph(ftface->glyph, &glyph))
				throw runtime::runtime_error::create("FT_Get_Glyph failed.");

			if (glyph->format != FT_GLYPH_FORMAT_OUTLINE)
				throw runtime::runtime_error::create("Invalid Glyph Format.");

			this->addContours(ftface->glyph, offset, bezierSteps_);

			offset += ftface->glyph->bitmap_left + ftface->glyph->bitmap.width;
		}
	}

	void
	TextMeshingComponent::addContours(const void* glyph_, float offset, std::uint16_t bezierSteps) noexcept
	{
		auto addPoints = [](video::TextContour& contours, const FT_Vector* contour, const char* tags, std::size_t n, std::uint16_t bezierSteps) -> void
		{
			math::float3 prev;
			math::float3 cur(contour[(n - 1) % n].x / 64.0f, contour[(n - 1) % n].y / 64.0f, 0.0);
			math::float3 next(contour[0].x / 64.0f, contour[0].y / 64.0f, 0.0);

			float olddir, dir = std::atan2((next - cur).y, (next - cur).x);
			float angle = 0.0f;

			for (std::size_t i = 0; i < n; i++)
			{
				prev = cur;
				cur = next;
				next = math::float3(contour[(i + 1) % n].x / 64.0f, contour[(i + 1) % n].y / 64.0f, 0.0f);

				olddir = dir;
				dir = std::atan2((next - cur).y, (next - cur).x);

				float t = dir - olddir;
				if (t < -math::PI) t += 2 * math::PI;
				if (t > math::PI) t -= 2 * math::PI;
				angle += t;

				switch (FT_CURVE_TAG(tags[i]))
				{
				case FT_Curve_Tag_On:
				{
					contours.addPoints(cur);
				}
				break;
				case FT_Curve_Tag_Conic:
				{
					math::float3 prev2 = prev, next2 = next;

					if (FT_CURVE_TAG(tags[(i - 1 + n) % n]) == FT_Curve_Tag_Conic)
					{
						prev2 = (cur + prev) * 0.5f;
						contours.addPoints(prev2);
					}

					if (FT_CURVE_TAG(tags[(i + 1) % n]) == FT_Curve_Tag_Conic)
						next2 = (cur + next) * 0.5f;

					contours.addPoints(prev2, cur, next2, bezierSteps);
				}
				break;
				case FT_Curve_Tag_Cubic:
				{
					contours.addPoints(prev, cur, next, math::float3(contour[(i + 2) % n].x / 64.0f, contour[(i + 2) % n].y / 64.0f, 0.0f), bezierSteps);
				}
				break;
				default:
					break;
				}
			}

			contours.addPoints(contours.at(0));
			contours.isClockwise(angle < 0.0f);
		};

		auto glyph = (FT_GlyphSlot)glyph_;

		auto object = std::make_shared<GameObject>();
		object->addComponent<MeshFilterComponent>(std::make_shared<video::Mesh>());
		object->getComponent<TransformComponent>()->setTranslate(math::float3(offset + glyph->bitmap_left, 0.0, 0.0));

		auto renderer = this->getComponent<RenderComponent>();
		if (renderer)
		{
			auto copy = renderer->clone()->downcast_pointer<RenderComponent>();
			copy->setMaterial(renderer->getMaterial());
			object->addComponent(copy);
		}

		video::TextContours contours(glyph->outline.n_contours);

		for (std::size_t startIndex = 0, i = 0; i < glyph->outline.n_contours; i++)
		{
			auto contour = std::make_unique<video::TextContour>();

			addPoints(*contour, &glyph->outline.points[startIndex], &glyph->outline.tags[startIndex], (glyph->outline.contours[i] - startIndex) + 1, bezierSteps);

			startIndex = glyph->outline.contours[i] + 1;

			contours[i] = std::move(contour);
		}

		auto group = std::make_shared<video::TextContourGroup>();
		group->setContours(std::move(contours));
		group->buildMeshes(*object->getComponent<MeshFilterComponent>()->getMesh());

		this->getGameObject()->addChild(std::move(object));
	}

	GameComponentPtr
	TextMeshingComponent::clone() const noexcept
	{
		auto instance = std::make_shared<TextMeshingComponent>();
		instance->setFont(this->getFont());
		instance->setText(this->getText());
		instance->setBezierSteps(this->getBezierSteps());

		return instance;
	}
}