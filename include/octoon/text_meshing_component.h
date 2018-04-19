#ifndef TEXT3D_TEXT_MESHING_H_
#define TEXT3D_TEXT_MESHING_H_

#include <text3d/mesh_filter.h>
#include <text3d/renderer/render_types.h>

namespace text3d
{
	class TEXT3D_EXPORT TextMeshing final : public MeshFilter
	{
		Text3dDeclareSubClass(TextMeshing, MeshFilter)
	public:
		TextMeshing() noexcept;
		virtual ~TextMeshing() noexcept;

		void setFont(render::TextFilePtr&& font) noexcept;
		void setFont(const render::TextFilePtr& font) noexcept;
		const render::TextFilePtr& getFont() const noexcept;

		void setText(std::wstring&& font) noexcept;
		void setText(const std::wstring& font) noexcept;
		const std::wstring& getText() const noexcept;

		void setBezierSteps(std::uint16_t bezierSteps) noexcept;
		std::uint16_t getBezierSteps() const noexcept;

		virtual EntityComponentPtr clone() const noexcept override;

	private:
		virtual void onActivate() noexcept(false);
		virtual void onDeactivate() noexcept;

	private:
		void buildContours(const std::wstring& string) noexcept(false);
		void addContours(const void* glyph, float offset, std::uint16_t bezierSteps) noexcept;

	private:
		TextMeshing(const TextMeshing&) = delete;
		TextMeshing& operator=(const TextMeshing&) = delete;

	private:
		std::wstring string_;
		std::uint16_t bezierSteps_;

		render::TextFilePtr font_;
	};
}

#endif