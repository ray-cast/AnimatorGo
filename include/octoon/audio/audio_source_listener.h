#ifndef OCTOON_AUDIO_SOURCE_LISTENER_H_
#define OCTOON_AUDIO_SOURCE_LISTENER_H_

#include <memory>

#include <cstdint>
#include <cstddef>

#include <octoon/runtime/platform.h>

#include <octoon/audio/audio_buffer.h>
#include <octoon/audio/audio_types.h>
#include <octoon/audio/audio_reader.h>

namespace octoon
{
    class OCTOON_EXPORT AudioSourceListener
    {
    public:
        AudioSourceListener() noexcept;
        virtual ~AudioSourceListener() noexcept;

        virtual void onPlayEnd() noexcept = 0;
    };
}

#endif