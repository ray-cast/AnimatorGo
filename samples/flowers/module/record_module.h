#ifndef FLOWER_RECORD_MODULE_H_
#define FLOWER_RECORD_MODULE_H_

#include <vector>
#include <flower_model.h>
#include <octoon/math/vector3.h>

namespace flower
{
	class RecordModule final : public FlowerModule
	{
	public:
		RecordModule() noexcept;
		virtual ~RecordModule() noexcept;

		virtual void reset() noexcept override;
		virtual void resize(std::uint32_t width, std::uint32_t height) noexcept;

		virtual void load(octoon::runtime::json& reader) noexcept override;
		virtual void save(octoon::runtime::json& reader) noexcept override;

	private:
		RecordModule(const RecordModule&) = delete;
		RecordModule& operator=(const RecordModule&) = delete;

	public:
		bool hdr;
		bool srgb;
		bool active;

		std::uint32_t width;
		std::uint32_t height;
	};
}

#endif