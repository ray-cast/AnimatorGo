#ifndef MYSTICLIT_DENOISE_COMPONENT_H_
#define MYSTICLIT_DENOISE_COMPONENT_H_

#include "module/denoise_module.h"
#include "mysticLit_component.h"

typedef struct OIDNFilterImpl* OIDNFilter;
typedef struct OIDNDeviceImpl* OIDNDevice;
typedef struct OIDNBufferImpl* OIDNBuffer;

namespace MysticLit
{
	class DenoiseComponent final : public MysticLitComponent<DenoiseModule>
	{
	public:
		DenoiseComponent() noexcept;
		virtual ~DenoiseComponent() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

	private:
		void onEnable() noexcept override;
		void onDisable() noexcept override;

		void onPostProcess() noexcept override;

	private:
		DenoiseComponent(const DenoiseComponent&) = delete;
		DenoiseComponent& operator=(const DenoiseComponent&) = delete;

	private:
		OIDNFilter filter_;
		OIDNDevice device_;

		OIDNBuffer oidnColorBuffer_;
		OIDNBuffer oidnNormalBuffer_;
		OIDNBuffer oidnAlbedoBuffer_;
		OIDNBuffer oidnOutputBuffer_;
	};
}

#endif