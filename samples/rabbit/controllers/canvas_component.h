#ifndef RABBIT_CANVAS_COMPONENT_H_
#define RABBIT_CANVAS_COMPONENT_H_

#include <rabbit_component.h>
#include <module/canvas_module.h>

namespace rabbit
{
	class CanvasComponent final : public RabbitComponent<CanvasModule>
	{
	public:
		CanvasComponent() noexcept;
		~CanvasComponent() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

		void save(std::string_view filepath) noexcept;

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(CanvasComponent);
		}

	private:
		virtual void onEnable() noexcept override;
		virtual void onDisable() noexcept override;

		virtual void onPostProcess() noexcept override;

	private:
		bool active_;
	};
}

#endif