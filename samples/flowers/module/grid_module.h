#ifndef FLOWER_GRID_MODULE_H_
#define FLOWER_GRID_MODULE_H_

#include <flower_model.h>

namespace flower
{
	class GridModule final : public FlowerModule
	{
	public:
		GridModule() noexcept;
		virtual ~GridModule() noexcept;

		virtual void reset() noexcept override;

		virtual void load(octoon::runtime::json& reader) noexcept override;
		virtual void save(octoon::runtime::json& reader) noexcept override;

	private:
		GridModule(const GridModule&) = delete;
		GridModule& operator=(const GridModule&) = delete;

	public:
		bool enable;
	};
}

#endif