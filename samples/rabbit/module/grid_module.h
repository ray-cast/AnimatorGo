#ifndef RABBIT_GRID_MODULE_H_
#define RABBIT_GRID_MODULE_H_

#include <rabbit_model.h>

namespace rabbit
{
	class GridModule final : public RabbitModule
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