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

		virtual bool open() noexcept override;
		virtual void close() noexcept override;

		virtual void set_volume(float volume) noexcept override;
		virtual float get_volume() const noexcept override;

		virtual void set_translate(const math::float3& translate) noexcept override;
		virtual void set_velocity(const math::float3& velocity) noexcept override;
		virtual void set_orientation(const math::float3& forward, const math::float3& up) noexcept override;

		virtual void get_translate(math::float3& translate) noexcept override;
		virtual void get_velocity(math::float3& velocity) noexcept override;
		virtual void get_orientation(math::float3& forward, math::float3& up) noexcept override;
	};
}

#endif