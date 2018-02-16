#include <octoon/audio/openal/audio_listener_al.h>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx.h>

namespace octoon
{
    namespace audio
    {
        AudioListenerAL::AudioListenerAL() noexcept
        {
        }

        AudioListenerAL::~AudioListenerAL() noexcept
        {
            this->close();
        }

        bool
        AudioListenerAL::open() noexcept
        {
            return true;
        }

        void
        AudioListenerAL::close() noexcept
        {
        }

        void AudioListenerAL::set_volume(float volume) noexcept
        {
            alListenerf(AL_GAIN, volume);
        }

        float AudioListenerAL::get_volume() const noexcept
        {
            float volume = -1.0f;
            alGetListenerf(AL_GAIN, &volume);
            return volume;
        }

        void AudioListenerAL::set_translate(const math::float3& translate) noexcept
        {
            ALfloat pos[] = { translate.x, translate.y, translate.z };
            alListenerfv(AL_POSITION, pos);
        }

        void AudioListenerAL::set_velocity(const math::float3& velocity) noexcept
        {
            ALfloat vel[] = { velocity.x, velocity.y, velocity.z };
            alListenerfv(AL_VELOCITY, vel);
        }

        void AudioListenerAL::set_orientation(const math::float3& forward, const math::float3& up) noexcept
        {
            ALfloat dir[] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };
            alListenerfv(AL_ORIENTATION, dir);
        }

        void AudioListenerAL::get_translate(math::float3& translate) noexcept
        {
            ALfloat pos[3];
            alGetListenerfv(AL_POSITION, pos);
            translate.x = pos[0];
            translate.y = pos[1];
            translate.z = pos[2];
        }

        void AudioListenerAL::get_velocity(math::float3& velocity) noexcept
        {
            ALfloat pos[3];
            alGetListenerfv(AL_VELOCITY, pos);
            velocity.x = pos[0];
            velocity.y = pos[1];
            velocity.z = pos[2];
        }

        void AudioListenerAL::get_orientation(math::float3& forward, math::float3& up) noexcept
        {
            ALfloat dir[6];
            alGetListenerfv(AL_ORIENTATION, dir);
            forward.x = dir[0];
            forward.y = dir[1];
            forward.z = dir[2];
            up.x = dir[3];
            up.y = dir[4];
            up.z = dir[5];
        }
    }
}