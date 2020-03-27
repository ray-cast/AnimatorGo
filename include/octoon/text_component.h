#ifndef OCTOON_TEXT_COMPONENT_H_
#define OCTOON_TEXT_COMPONENT_H_

#include <octoon/mesh/mesh.h>
#include <octoon/model/text_meshing.h>
#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT TextComponent : public GameComponent
	{
		OctoonDeclareSubClass(TextComponent, GameComponent)
	public:
		TextComponent() noexcept;
		TextComponent(std::string_view u8str, bool sharedText = false) noexcept;
		virtual ~TextComponent() noexcept;

		void setText(std::string_view u8str, bool sharedText = false) noexcept;
		const std::string& getText() const noexcept;

		void setTextMeshing(std::shared_ptr<font::TextMeshing>&& meshing) noexcept;
		void setTextMeshing(const std::shared_ptr<font::TextMeshing>& meshing) noexcept;
		const std::shared_ptr<font::TextMeshing>& getTextMeshing() const noexcept;

		void setTextAlign(font::TextAlign align) noexcept;
		font::TextAlign getTextAlign() const noexcept;

		bool isSharedText() const noexcept;

		void uploadTextData() noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onTextReplace() noexcept;

	private:
		TextComponent(const TextComponent&) = delete;
		TextComponent& operator=(const TextComponent&) = delete;

	private:
		bool isSharedText_;
		std::string u8str_;
		mesh::MeshPtr mesh_;
		font::TextAlign align_;
		std::shared_ptr<font::TextMeshing> meshing_;
	};
}

#endif