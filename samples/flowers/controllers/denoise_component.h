#ifndef FLOWER_DENOISE_COMPONENT_H_
#define FLOWER_DENOISE_COMPONENT_H_

#include "module/denoise_module.h"
#include "flower_component.h"

typedef struct OIDNFilterImpl* OIDNFilter;
typedef struct OIDNDeviceImpl* OIDNDevice;
typedef struct OIDNBufferImpl* OIDNBuffer;

namespace flower
{
	class DenoiseComponent final : public RabbitComponent<DenoiseModule>
	{
	public:
		DenoiseComponent() noexcept;
		virtual ~DenoiseComponent() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(DenoiseComponent);
		}

	private:
		void onEnable() noexcept override;
		void onDisable() noexcept override;

		void onPostProcess() noexcept override;

	private:
		DenoiseComponent(const DenoiseComponent&) = delete;
		DenoiseComponent& operator=(const DenoiseComponent&) = delete;

	private:
		OIDNDevice device_;
		OIDNFilter filter_;

		OIDNBuffer oidnColorBuffer_;
		OIDNBuffer oidnNormalBuffer_;
		OIDNBuffer oidnAlbedoBuffer_;
		OIDNBuffer oidnOutputBuffer_;
	};
}

#endif