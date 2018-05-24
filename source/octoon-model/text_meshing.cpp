#include <octoon/model/text_meshing.h>
#include <octoon/model/mesh.h>
#include <octoon/model/text_file.h>
#include <octoon/model/contour_group.h>
#include <octoon/model/path.h>
#include <octoon/model/path_group.h>
#include <octoon/runtime/except.h>

#include <ft2build.h>
#include <freetype/ftglyph.h>

namespace octoon
{
	namespace model
	{
		TextMeshing::TextMeshing() noexcept
			: font_(nullptr)
			, bezierSteps_(6)
			, pixelSize_(12)
		{
		}

		TextMeshing::TextMeshing(const char* path, std::uint16_t pixelsSize, std::uint16_t bezierSteps) noexcept
			: font_(std::make_shared<TextFile>(path))
			, bezierSteps_(bezierSteps)
			, pixelSize_(pixelsSize)
		{
		}

		TextMeshing::TextMeshing(const std::string& path, std::uint16_t pixelsSize, std::uint16_t bezierSteps) noexcept
			: font_(std::make_shared<TextFile>(path.c_str()))
			, bezierSteps_(bezierSteps)
			, pixelSize_(pixelsSize)
		{
		}

		TextMeshing::TextMeshing(TextFilePtr&& font, std::uint16_t pixelsSize, std::uint16_t bezierSteps) noexcept
			: font_(std::move(font))
			, bezierSteps_(bezierSteps)
			, pixelSize_(pixelsSize)
		{
		}

		TextMeshing::TextMeshing(const TextFilePtr& font, std::uint16_t pixelsSize, std::uint16_t bezierSteps) noexcept
			: font_(std::move(font))
			, bezierSteps_(bezierSteps)
			, pixelSize_(pixelsSize)
		{
		}

		TextMeshing::~TextMeshing() noexcept
		{
		}

		void
		TextMeshing::setFont(TextFilePtr&& font) noexcept
		{
			font_ = std::move(font);
		}

		void
		TextMeshing::setFont(const TextFilePtr& font) noexcept
		{
			font_ = font;
		}

		const TextFilePtr&
		TextMeshing::getFont() const noexcept
		{
			return font_;
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
		TextMeshing::setPixelsSize(std::uint16_t pixelsSize) noexcept
		{
			pixelSize_ = pixelsSize;
		}

		std::uint16_t
		TextMeshing::getPixelsSize() const noexcept
		{
			return pixelSize_;
		}

		TextMeshingPtr
		TextMeshing::clone() const noexcept
		{
			auto instance = std::make_shared<TextMeshing>();
			instance->setFont(this->getFont());
			instance->setPixelsSize(this->getPixelsSize());
			instance->setBezierSteps(this->getBezierSteps());

			return instance;
		}

		PathGroups makeTextPaths(const std::wstring& string, const TextMeshing& params) noexcept(false)
		{
			assert(params.getFont());
			assert(params.getPixelsSize() > 0);
			assert(params.getBezierSteps() > 1);

			auto addEdge = [](Path& contours, const FT_Vector* contour, const char* tags, std::size_t n) -> void
			{
				math::float3 prev;
				math::float3 cur(contour[(n - 1) % n].x / 64.0f, contour[(n - 1) % n].y / 64.0f, 0.0);
				math::float3 next(contour[0].x / 64.0f, contour[0].y / 64.0f, 0.0);

				for (std::size_t i = 0; i < n; i++)
				{
					prev = cur;
					cur = next;
					next = math::float3(contour[(i + 1) % n].x / 64.0f, contour[(i + 1) % n].y / 64.0f, 0.0f);

					switch (FT_CURVE_TAG(tags[i]))
					{
					case FT_Curve_Tag_On:
						contours.addEdge(cur);
						break;
					case FT_Curve_Tag_Cubic:
						contours.addEdge(prev, cur, next, math::float3(contour[(i + 2) % n].x / 64.0f, contour[(i + 2) % n].y / 64.0f, 0.0f));
						break;
					case FT_Curve_Tag_Conic:
					{
						math::float3 prev2 = prev, next2 = next;

						if (FT_CURVE_TAG(tags[(i + 1) % n]) == FT_Curve_Tag_Conic)
							next2 = (cur + next) * 0.5f;

						if (FT_CURVE_TAG(tags[(i - 1 + n) % n]) == FT_Curve_Tag_Conic)
							prev2 = (cur + prev) * 0.5f;

						contours.addEdge(prev2, cur, next2);
					}
					break;
					}
				}
			};

			auto addPath = [addEdge](const FT_GlyphSlot glyph, FT_Pos offset)
			{
				Paths paths(glyph->outline.n_contours);

				for (short i = 0; i < glyph->outline.n_contours; i++)
					paths[i] = std::make_unique<Path>();

				for (short startIndex = 0, i = 0; i < glyph->outline.n_contours; i++)
				{
					auto points = &glyph->outline.points[startIndex];
					auto tags = &glyph->outline.tags[startIndex];
					auto index = (glyph->outline.contours[i] - startIndex) + 1;

					startIndex = glyph->outline.contours[i] + 1;

					addEdge(*paths[i], points, tags, index);
				}

				paths += math::float3(offset, 0, 0);

				return std::move(paths);
			};

			if (::FT_Set_Pixel_Sizes((FT_Face)(params.getFont()->getFont()), params.getPixelsSize(), params.getPixelsSize()))
				throw runtime::runtime_error::create("FT_Set_Char_Size() failed (there is probably a problem with your font size", 3);

			FT_Face ftface = (FT_Face)params.getFont()->getFont();

			auto offset = ftface->glyph->advance.x;

			PathGroups groups;

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

				if (ch == ' ')
					offset += ftface->glyph->advance.x / 64;
				else
				{
					groups.push_back(std::make_shared<PathGroup>(addPath(ftface->glyph, offset)));

					offset += ftface->glyph->bitmap_left + ftface->glyph->bitmap.width;
				}
			}

			return groups;
		}

		ContourGroups makeTextContours(const PathGroups& paths, std::uint16_t bezierSteps) noexcept(false)
		{
			ContourGroups groups;

			for (auto& path : paths)
				groups.push_back(std::make_shared<ContourGroup>(*path, bezierSteps));

			return groups;
		}

		ContourGroups makeTextContours(const std::wstring& string, const TextMeshing& params) noexcept(false)
		{
			assert(params.getFont());
			assert(params.getPixelsSize() > 0);
			assert(params.getBezierSteps() > 1);

			auto addPoints = [](Contour& contours, const FT_Vector* contour, const char* tags, std::size_t n, std::uint16_t bezierSteps) -> void
			{
				math::float3 prev;
				math::float3 cur(contour[(n - 1) % n].x / 64.0f, contour[(n - 1) % n].y / 64.0f, 0.0);
				math::float3 next(contour[0].x / 64.0f, contour[0].y / 64.0f, 0.0);

				for (std::size_t i = 0; i < n; i++)
				{
					prev = cur;
					cur = next;
					next = math::float3(contour[(i + 1) % n].x / 64.0f, contour[(i + 1) % n].y / 64.0f, 0.0f);
					
					switch (FT_CURVE_TAG(tags[i]))
					{
					case FT_Curve_Tag_On:
						contours.addPoints(cur);
						break;
					case FT_Curve_Tag_Cubic:
						contours.addPoints(prev, cur, next, math::float3(contour[(i + 2) % n].x / 64.0f, contour[(i + 2) % n].y / 64.0f, 0.0f), bezierSteps);
						break;
					case FT_Curve_Tag_Conic:
					{
						math::float3 prev2 = prev, next2 = next;

						if (FT_CURVE_TAG(tags[(i + 1) % n]) == FT_Curve_Tag_Conic)
							next2 = (cur + next) * 0.5f;

						if (FT_CURVE_TAG(tags[(i - 1 + n) % n]) == FT_Curve_Tag_Conic)
							prev2 = (cur + prev) * 0.5f;

						contours.addPoints(prev2, cur, next2, bezierSteps);
					}
					break;
					}
				}
			};

			auto addContours = [addPoints](const FT_GlyphSlot glyph, FT_Pos offset, std::uint16_t bezierSteps)
			{
				Contours contours(glyph->outline.n_contours);

				for (short i = 0; i < glyph->outline.n_contours; i++)
					contours[i] = std::make_unique<Contour>();

				for (short startIndex = 0, i = 0; i < glyph->outline.n_contours; i++)
				{
					auto points = &glyph->outline.points[startIndex];
					auto tags = &glyph->outline.tags[startIndex];
					auto index = (glyph->outline.contours[i] - startIndex) + 1;

					startIndex = glyph->outline.contours[i] + 1;

					addPoints(*contours[i], points, tags, index, bezierSteps);
				}

				for (auto& contour : contours)
				{
					for (auto& point : contour->points())
						point.x += offset;
				}

				return std::make_shared<ContourGroup>(std::move(contours));
			};

			if (::FT_Set_Pixel_Sizes((FT_Face)(params.getFont()->getFont()), params.getPixelsSize(), params.getPixelsSize()))
				throw runtime::runtime_error::create("FT_Set_Char_Size() failed (there is probably a problem with your font size", 3);

			FT_Face ftface = (FT_Face)params.getFont()->getFont();

			auto offset = ftface->glyph->advance.x;

			ContourGroups groups;

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

				if (ch == ' ')
					offset += ftface->glyph->advance.x / 64;
				else
				{
					groups.push_back(addContours(ftface->glyph, offset, params.getBezierSteps()));

					offset += ftface->glyph->bitmap_left + ftface->glyph->bitmap.width;
				}
			}

			for (auto& group : groups)
				*group -= math::float3(offset * 0.5f, 0, 0);

			return groups;
		}

		Mesh makeText(const std::wstring& string, const TextMeshing& params, float thickness) noexcept(false)
		{
			Mesh mesh = makeMesh(makeTextContours(string, params), thickness, false);
			mesh.computeBoundingBox();

			return mesh;
		}

		Mesh makeTextWireframe(const std::wstring& string, const TextMeshing& params, float thickness) noexcept(false)
		{
			Mesh mesh = makeMeshWireframe(makeTextContours(string, params), thickness);
			mesh.computeBoundingBox();

			return mesh;
		}
	}
}