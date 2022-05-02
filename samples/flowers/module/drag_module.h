#ifndef FLOWER_DRAG_MODULE_H_
#define FLOWER_DRAG_MODULE_H_

#include <flower_model.h>
#include <optional>
#include <octoon/raycaster.h>

namespace flower
{
	class DragModule final : public FlowerModule
	{
	public:
		DragModule() noexcept;
		virtual ~DragModule() noexcept;

		virtual void reset() noexcept override;

		virtual void load(octoon::runtime::json& reader) noexcept override;
		virtual void save(octoon::runtime::json& reader) noexcept override;

	private:
		DragModule(const DragModule&) = delete;
		DragModule& operator=(const DragModule&) = delete;

	public:
		std::optional<octoon::RaycastHit> selectedItem_;
		std::optional<octoon::RaycastHit> selectedItemHover_;
	};
}

#endif