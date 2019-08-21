#ifndef MYSTICLIT_DENOISE_MODULE_H_
#define MYSTICLIT_DENOISE_MODULE_H_

#include <mysticLit_model.h>

namespace MysticLit
{
	class DenoiseModule : public MysticLitModule
	{
	public:
		DenoiseModule() noexcept;
		virtual ~DenoiseModule() noexcept;

		virtual void reset() noexcept override;

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