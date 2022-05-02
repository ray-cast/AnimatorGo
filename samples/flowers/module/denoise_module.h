#ifndef FLOWER_DENOISE_MODULE_H_
#define FLOWER_DENOISE_MODULE_H_

#include <flower_model.h>

namespace flower
{
	class DenoiseModule final : public FlowerModule
	{
	public:
		DenoiseModule() noexcept;
		virtual ~DenoiseModule() noexcept;

		virtual void reset() noexcept override;

		virtual void load(octoon::runtime::json& reader) noexcept override;
		virtual void save(octoon::runtime::json& reader) noexcept override;

	private:
		DenoiseModule(const DenoiseModule&) = delete;
		DenoiseModule& operator=(const DenoiseModule&) = delete;

	public:
		bool active_;

		bool hdr;
		bool srgb;
	};
}

#endif