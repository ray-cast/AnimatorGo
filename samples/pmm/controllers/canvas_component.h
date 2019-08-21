#ifndef MYSTIC_CANVAS_CONTROLLER_H_
#define MYSTIC_CANVAS_CONTROLLER_H_

#include <mysticLit_component.h>
#include <module/canvas_module.h>

namespace MysticLit
{
	class CanvasComponent final : public MysticLitComponent<CanvasModule>
	{
	public:
		CanvasComponent() noexcept;
		~CanvasComponent() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

	private:
		virtual void onEnable() noexcept override;
		virtual void onDisable() noexcept override;

		virtual void onPostProcess() noexcept override;

	private:
		bool active_;
	};
}

#endif