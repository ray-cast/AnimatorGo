#ifndef OCTOON_DENOISE_COMPONENT_H_
#define OCTOON_DENOISE_COMPONENT_H_

#include <octoon/game_component.h>
#include <octoon/io/iostream.h>

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

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

	private:
		void onEnable() noexcept override;
		void onDisable() noexcept override;

	private:
		DenoiseComponent(const DenoiseComponent&) = delete;
		DenoiseComponent& operator=(const DenoiseComponent&) = delete;

	private:
		bool active_;

		OIDNFilter filter_;
		OIDNDevice device_;

		OIDNBuffer oidnColorBuffer_;
		OIDNBuffer oidnNormalBuffer_;
		OIDNBuffer oidnAlbedoBuffer_;
		OIDNBuffer oidnOutputBuffer_;
	};
}

#endif