#ifndef OCTOON_AUDIO_LISTENER_AL_H_
#define OCTOON_AUDIO_LISTENER_AL_H_

#include <memory>
#include <cstdint>
#include <cstddef>

#include <octoon/runtime/platform.h>
#include <octoon/math/vector3.h>
#include <octoon/math/mathfwd.h>

#include <octoon/audio/common/audio_device.h>
#include <octoon/audio/common/audio_reader.h>
#include <octoon/audio/common/audio_source.h>
#include <octoon/audio/common/audio_listener.h>
#include <octoon/audio/common/audio_source_listener.h>
#include <octoon/audio/openal/audio_source_al.h>

namespace octoon
{
	class AudioListenerAL final : public AudioListener
	{
	public:
		AudioListenerAL() noexcept;
		~AudioListenerAL() noexcept;

		virtual void setVolume(float volume) noexcept override;
		virtual float getVolume() const noexcept override;

		virtual void setTranslate(const math::float3& translate) noexcept override;
		virtual void setVelocity(const math::float3& velocity) noexcept override;
		virtual void setOrientation(const math::float3& forward, const math::float3& up) noexcept override;

		virtual void getTranslate(math::float3& translate) noexcept override;
		virtual void getVelocity(math::float3& velocity) noexcept override;
		virtual void getOrientation(math::float3& forward, math::float3& up) noexcept override;
	};
}

#endif