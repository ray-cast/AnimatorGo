#ifndef OCTOON_AUDIO_LISTENER_H_
#define OCTOON_AUDIO_LISTENER_H_

#include <octoon/runtime/platform.h>
#include <octoon/math/vector3.h>
#include <octoon/math/mathfwd.h>

namespace octoon
{
    class OCTOON_EXPORT AudioListener
    {
    public:
        AudioListener() noexcept;
        ~AudioListener() noexcept;

        virtual bool open() noexcept = 0;
        virtual void close() noexcept = 0;

        virtual void set_volume(float volume) noexcept = 0;
        virtual float get_volume() const noexcept = 0;

        virtual void set_translate(const math::float3& translate) noexcept = 0;
        virtual void set_velocity(const math::float3& velocity) noexcept = 0;
        virtual void set_orientation(const math::float3& forward, const math::float3& up) noexcept = 0;

        virtual void get_translate(math::float3& translate) noexcept = 0;
        virtual void get_velocity(math::float3& velocity) noexcept = 0;
        virtual void get_orientation(math::float3& forward, math::float3& up) noexcept = 0;
    };
}

#endif