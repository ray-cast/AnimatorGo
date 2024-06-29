#ifndef UNREAL_MARK_MODULE_H_
#define UNREAL_MARK_MODULE_H_

#include <unreal_model.h>

namespace unreal
{
	class MarkModule final : public UnrealModule
	{
	public:
		MarkModule() noexcept;
		virtual ~MarkModule() noexcept;

		virtual void reset() noexcept override;

		virtual void load(nlohmann::json& reader) noexcept override;
		virtual void save(nlohmann::json& writer) noexcept override;

	private:
		MarkModule(const MarkModule&) = delete;
		MarkModule& operator=(const MarkModule&) = delete;

	public:
		std::uint32_t x;
		std::uint32_t y;
		std::uint32_t width;
		std::uint32_t height;
		std::uint32_t channel;
		std::vector<std::uint8_t> pixels;
	};
}

#endif