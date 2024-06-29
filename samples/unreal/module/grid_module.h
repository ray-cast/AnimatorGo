#ifndef UNREAL_GRID_MODULE_H_
#define UNREAL_GRID_MODULE_H_

#include <unreal_model.h>

namespace unreal
{
	class GridModule final : public UnrealModule
	{
	public:
		GridModule() noexcept;
		virtual ~GridModule() noexcept;

		virtual void reset() noexcept override;

		virtual void load(nlohmann::json& reader) noexcept override;
		virtual void save(nlohmann::json& writer) noexcept override;

	private:
		GridModule(const GridModule&) = delete;
		GridModule& operator=(const GridModule&) = delete;
	};
}

#endif