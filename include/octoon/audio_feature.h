#ifndef OCTOON_AUDIO_FEATURE_H_
#define OCTOON_AUDIO_FEATURE_H_

#include <octoon/game_feature.h>

namespace octoon
{
	class OCTOON_EXPORT AudioFeature final : public GameFeature
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
		std::shared_ptr<class AudioDevice> device;
	};
}

#endif