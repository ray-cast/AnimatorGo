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
        virtual ~AudioListener() noexcept;

        virtual void setVolume(float volume) noexcept = 0;
        virtual float getVolume() const noexcept = 0;

        virtual void setTranslate(const math::float3& translate) noexcept = 0;
        virtual void setVelocity(const math::float3& velocity) noexcept = 0;
        virtual void setOrientation(const math::float3& forward, const math::float3& up) noexcept = 0;

        virtual void getTranslate(math::float3& translate) noexcept = 0;
        virtual void getVelocity(math::float3& velocity) noexcept = 0;
        virtual void getOrientation(math::float3& forward, math::float3& up) noexcept = 0;
    };
}

#endif