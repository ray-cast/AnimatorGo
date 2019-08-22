#ifndef MYSTICLIT_MAIN_CONTROLLER_H_
#define MYSTICLIT_MAIN_CONTROLLER_H_

#include <octoon/game_object.h>
#include <mysticLit_component.h>
#include <module/canvas_module.h>

namespace MysticLit
{
	class UIComponent final : public MysticLitComponent<CanvasModule>
	{
	public:
		UIComponent() noexcept;
		~UIComponent() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

		void addMessageListener(const std::string& event, std::function<void(const octoon::runtime::any&)> listener) noexcept;
		void removeMessageListener(const std::string& event, std::function<void(const octoon::runtime::any&)> listener) noexcept;

		virtual void init(const std::shared_ptr<MysticLitContext>& context, const std::shared_ptr<CanvasModule>& model) noexcept override;

	private:
		void onEnable() noexcept override;
		void onDisable() noexcept override;

	private:
		octoon::GameObjectPtr main_;
	};
}

#endif