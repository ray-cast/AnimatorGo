#ifndef OCTOON_AUDIO_DEVICE_AL_H_
#define OCTOON_AUDIO_DEVICE_AL_H_

#include <memory>
#include <cstdint>
#include <cstddef>

#include <octoon/runtime/platform.h>
#include <octoon/math/vector3.h>
#include <octoon/math/mathfwd.h>

#include <octoon/audio/common/audio_device.h>
#include <octoon/audio/common/audio_reader.h>
#include <octoon/audio/common/audio_source.h>
#include <octoon/audio/common/audio_source_listener.h>
#include <octoon/audio/openal/audio_source_al.h>

struct ALCdevice;
struct ALCcontext;

namespace octoon
{
	class OCTOON_EXPORT AudioDeviceAL final : public AudioDevice
	{
	public:
		AudioDeviceAL() noexcept;
		virtual ~AudioDeviceAL() noexcept override;

		virtual bool open() noexcept override;
		virtual void close() noexcept override;

		virtual bool is_open() const noexcept override;

		virtual void set_distance_model(bool enable) noexcept override;
		virtual bool get_distance_model() const noexcept override;

		virtual std::shared_ptr<AudioSource> create_audio_source() override;
		virtual std::shared_ptr<AudioListener> get_audio_listener() noexcept override;

	private:
		bool _distance_model;

		ALCdevice* _device;
		ALCcontext* _context;
		std::shared_ptr<AudioListener> _audio_listener;
	};
}

#endif


