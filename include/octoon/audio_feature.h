#ifndef OCTOON_AUDIO_FEATURE_H_
#define OCTOON_AUDIO_FEATURE_H_

#include <octoon/game_feature.h>
#include <octoon/audio/audio_device.h>

namespace octoon
{
	class AudioFeature final : public GameFeature
	{
		OctoonDeclareSubClass(AudioFeature, GameFeature)
	public:
		AudioFeature() noexcept;
		virtual ~AudioFeature() noexcept;

		void setDistanceModel(bool enable) noexcept;
		bool getDistanceModel() const noexcept;

	private:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onFrameBegin() noexcept override;
		void onFrame() noexcept override;
		void onFrameEnd() noexcept override;

	private:
		std::shared_ptr<AudioDevice> device;		
	};
}

#endif