#ifndef FLOWER_UI_COMPONENT_H_
#define FLOWER_UI_COMPONENT_H_

#include <octoon/game_object.h>
#include <flower_component.h>
#include "module/record_module.h"

namespace flower
{
	class UIComponent final : public RabbitComponent<RecordModule>
	{
	public:
		UIComponent() noexcept;
		~UIComponent() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

		void addMessageListener(const std::string& event, std::function<void(const std::any&)> listener) noexcept;
		void removeMessageListener(const std::string& event, std::function<void(const std::any&)> listener) noexcept;

		virtual void init(const std::shared_ptr<RabbitContext>& context, const std::shared_ptr<RecordModule>& model) noexcept override;

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