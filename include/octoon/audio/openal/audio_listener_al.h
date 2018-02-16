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

#include <octoon/audio/openal/audio_types_al.h>
#include <octoon/audio/openal/audio_source_al.h>

namespace octoon
{
	namespace audio
	{
        class AudioListenerAL final : public AudioListener
        {
        public:
            AudioListenerAL() noexcept;
            ~AudioListenerAL() noexcept;

            virtual bool open() noexcept;
            virtual void close() noexcept;

            virtual void set_volume(float volume) noexcept;
            virtual float get_volume() const noexcept;

            virtual void set_translate(const math::float3& translate) noexcept;
            virtual void set_velocity(const math::float3& velocity) noexcept;
            virtual void set_orientation(const math::float3& forward, const math::float3& up) noexcept;

            virtual void get_translate(math::float3& translate) noexcept;
            virtual void get_velocity(math::float3& velocity) noexcept;
            virtual void get_orientation(math::float3& forward, math::float3& up) noexcept;
        };

	}
}

#endif // OCTOON_AUDIO_LISTENER_AL_H_


