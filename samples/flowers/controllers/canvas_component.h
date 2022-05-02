#ifndef FLOWER_CANVAS_COMPONENT_H_
#define FLOWER_CANVAS_COMPONENT_H_

#include <flower_component.h>
#include <module/canvas_module.h>

namespace flower
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

		virtual void onResize(const octoon::input::InputEvent& event) noexcept override;

	private:
		bool active_;
	};
}

#endif