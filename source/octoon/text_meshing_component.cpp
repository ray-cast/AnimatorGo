#include <text3d/text_meshing.h>
#include <text3d/entity_object.h>
#include <text3d/transform.h>
#include <text3d/mesh_renderer.h>
#include <text3d/renderer/mesh.h>
#include <text3d/renderer/text_file.h>
#include <text3d/renderer/text_contour_group.h>
#include <text3d/common/except.h>

#include <ft2build.h>
#include <freetype/ftglyph.h>

namespace text3d
{
	Text3dImplementSubClass(TextMeshing, MeshFilter, "TextMeshing")

	TextMeshing::TextMeshing() noexcept
		: font_(nullptr)
		, bezierSteps_(6)
	{
	}

	TextMeshing::~TextMeshing() noexcept
	{
	}

	void
	TextMeshing::setFont(render::TextFilePtr&& font) noexcept
	{
		font_ = std::move(font);
	}

	void
	TextMeshing::setFont(const render::TextFilePtr& font) noexcept
	{
		font_ = font;
	}

	const render::TextFilePtr&
	TextMeshing::getFont() const noexcept
	{
		return font_;
	}

	void
	TextMeshing::setText(std::wstring&& text) noexcept
	{
		string_ = std::move(text);

		if (this->getActive() && this->getEntityObject()->getActive())
			this->buildContours(string_);
	}

	void
	TextMeshing::setText(const std::wstring& text) noexcept
	{
		string_ = text;

		if (this->getActive() && this->getEntityObject()->getActive())
			this->buildContours(string_);
	}

	const std::wstring&
	TextMeshing::getText() const noexcept
	{
		return string_;
	}

	void
	TextMeshing::setBezierSteps(std::uint16_t bezierSteps) noexcept
	{
		bezierSteps_ = bezierSteps;
	}

	std::uint16_t
	TextMeshing::getBezierSteps() const noexcept
	{
		return bezierSteps_;
	}

	void
	TextMeshing::onActivate() noexcept(false)
	{
		if (!string_.empty())
			this->buildContours(string_);
	}

	void
	TextMeshing::onDeactivate() noexcept
	{
		this->getEntityObject()->cleanupChildren();
	}

	void
	TextMeshing::buildContours(const std::wstring& string) noexcept(false)
	{
		if (!font_)
			throw common::null_reference::create("please call setFont() before process()");

		if (::FT_Set_Pixel_Sizes((FT_Face)(font_->getFont()), 12, 12))
			throw common::runtime_error::create("FT_Set_Char_Size() failed (there is probably a problem with your font size", 3);

		FT_Face ftface = (FT_Face)font_->getFont();

		float offset = 0;

		for (auto& ch : string)
		{
			FT_UInt index = FT_Get_Char_Index(ftface, ch);
			if (::FT_Load_Glyph(ftface, index, FT_LOAD_DEFAULT))
				throw common::runtime_error::create("FT_Load_Glyph failed.");

			FT_Glyph glyph;
			if (::FT_Get_Glyph(ftface->glyph, &glyph))
				throw common::runtime_error::create("FT_Get_Glyph failed.");

			if (glyph->format != FT_GLYPH_FORMAT_OUTLINE)
				throw common::runtime_error::create("Invalid Glyph Format.");

			this->addContours(ftface->glyph, offset, bezierSteps_);

			offset += ftface->glyph->bitmap_left + ftface->glyph->bitmap.width;
		}
	}

	void
	TextMeshing::addContours(const void* glyph_, float offset, std::uint16_t bezierSteps) noexcept
	{
		auto addPoints = [](render::TextContour& contours, const FT_Vector* contour, const char* tags, std::size_t n, std::uint16_t bezierSteps) -> void
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

		auto object = std::make_shared<EntityObject>();
		object->addComponent<MeshFilter>(std::make_shared<render::Mesh>());
		object->getComponent<Transform>()->setTranslate(math::float3(offset + glyph->bitmap_left, 0.0, 0.0));

		auto renderer = this->getComponent<Renderer>();
		if (renderer)
		{
			auto copy = renderer->clone()->downcast_pointer<Renderer>();
			copy->setMaterial(renderer->getMaterial());
			object->addComponent(copy);
		}

		render::TextContours contours(glyph->outline.n_contours);

		for (std::size_t startIndex = 0, i = 0; i < glyph->outline.n_contours; i++)
		{
			auto contour = std::make_unique<render::TextContour>();

			addPoints(*contour, &glyph->outline.points[startIndex], &glyph->outline.tags[startIndex], (glyph->outline.contours[i] - startIndex) + 1, bezierSteps);

			startIndex = glyph->outline.contours[i] + 1;

			contours[i] = std::move(contour);
		}

		auto group = std::make_shared<render::TextContourGroup>();
		group->setContours(std::move(contours));
		group->buildMeshes(*object->getComponent<MeshFilter>()->getMesh());

		this->getEntityObject()->addChild(std::move(object));
	}

	EntityComponentPtr
	TextMeshing::clone() const noexcept
	{
		auto instance = std::make_shared<TextMeshing>();
		instance->setFont(this->getFont());
		instance->setText(this->getText());
		instance->setBezierSteps(this->getBezierSteps());

		return instance;
	}
}