#ifndef OCTOON_TEXT_MESHING_COMPONENT_H_
#define OCTOON_TEXT_MESHING_COMPONENT_H_

#include <octoon/mesh_filter_component.h>
#include <octoon/video/render_types.h>

namespace octoon
{
	class OCTOON_EXPORT TextMeshingComponent final : public MeshFilterComponent
	{
		OctoonDeclareSubClass(TextMeshingComponent, MeshFilterComponent)
	public:
		TextMeshingComponent() noexcept;
		virtual ~TextMeshingComponent() noexcept;

		void setFont(video::TextFilePtr&& font) noexcept;
		void setFont(const video::TextFilePtr& font) noexcept;
		const video::TextFilePtr& getFont() const noexcept;

		void setText(std::wstring&& font) noexcept;
		void setText(const std::wstring& font) noexcept;
		const std::wstring& getText() const noexcept;

		void setBezierSteps(std::uint16_t bezierSteps) noexcept;
		std::uint16_t getBezierSteps() const noexcept;

		virtual GameComponentPtr clone() const noexcept override;

	private:
		virtual void onActivate() noexcept(false);
		virtual void onDeactivate() noexcept;

	private:
		void buildContours(const std::wstring& string) noexcept(false);
		void addContours(const void* glyph, float offset, std::uint16_t bezierSteps) noexcept;

	private:
		TextMeshingComponent(const TextMeshingComponent&) = delete;
		TextMeshingComponent& operator=(const TextMeshingComponent&) = delete;

	private:
		std::wstring string_;
		std::uint16_t bezierSteps_;

		video::TextFilePtr font_;
	};
}

#endif