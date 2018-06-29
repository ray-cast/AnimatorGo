#ifndef OCTOON_TEXT_COMPONENT_H_
#define OCTOON_TEXT_COMPONENT_H_

#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT TextComponent : public GameComponent
	{
		OctoonDeclareSubClass(TextComponent, GameComponent)
	public:
		TextComponent() noexcept;
		TextComponent(std::string&& text, bool sharedText = false) noexcept;
		TextComponent(const std::string& text, bool sharedText = false) noexcept;
		virtual ~TextComponent() noexcept;

		void setText(std::string&& text, bool sharedText = false) noexcept;
		void setText(const std::string& text, bool sharedText = false) noexcept;
		const std::string& getText() const noexcept;

		bool isSharedText() const noexcept;

		void uploadTextData() noexcept;

		virtual GameComponentPtr clone() const noexcept override;

	private:
		virtual void onTextReplace(const std::string& text) noexcept;

	private:
		TextComponent(const TextComponent&) = delete;
		TextComponent& operator=(const TextComponent&) = delete;

	private:
		bool isSharedText_;
		std::string text_;
	};
}

#endif