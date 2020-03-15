#ifndef RABBIT_UI_CONTROLLER_H_
#define RABBIT_UI_CONTROLLER_H_

#include <octoon/game_object.h>
#include <rabbit_component.h>
#include <module/canvas_module.h>

namespace rabbit
{
	class UIComponent final : public RabbitComponent<CanvasModule>
	{
	public:
		UIComponent() noexcept;
		~UIComponent() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

		void addMessageListener(const std::string& event, std::function<void(const std::any&)> listener) noexcept;
		void removeMessageListener(const std::string& event, std::function<void(const std::any&)> listener) noexcept;

		virtual void init(const std::shared_ptr<RabbitContext>& context, const std::shared_ptr<CanvasModule>& model) noexcept override;

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(UIComponent);
		}

	private:
		void onEnable() noexcept override;
		void onDisable() noexcept override;

	private:
		octoon::GameObjectPtr main_;
	};
}

#endif