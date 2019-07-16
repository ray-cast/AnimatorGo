#ifndef OCTOON_TEXT_COMPONENT_H_
#define OCTOON_TEXT_COMPONENT_H_

#include <octoon/model/mesh.h>
#include <octoon/model/text_meshing.h>
#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT TextComponent : public GameComponent
	{
		OctoonDeclareSubClass(TextComponent, GameComponent)
	public:
		TextComponent() noexcept;
		TextComponent(std::string&& u8str, bool sharedText = false) noexcept;
		TextComponent(const std::string& u8str, bool sharedText = false) noexcept;
		virtual ~TextComponent() noexcept;

		void setText(std::string&& u8str, bool sharedText = false) noexcept;
		void setText(const std::string& u8str, bool sharedText = false) noexcept;
		const std::string& getText() const noexcept;

		void setTextMeshing(model::TextMeshingPtr&& meshing) noexcept;
		void setTextMeshing(const model::TextMeshingPtr& meshing) noexcept;
		const model::TextMeshingPtr& getTextMeshing() const noexcept;

		void setTextAlign(model::TextAlign align) noexcept;
		model::TextAlign getTextAlign() const noexcept;

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
		model::MeshPtr mesh_;
		model::TextAlign align_;
		model::TextMeshingPtr meshing_;
	};
}

#endif