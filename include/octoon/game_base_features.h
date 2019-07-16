#ifndef OCTOON_GAME_BASE_FEATURES_H_
#define OCTOON_GAME_BASE_FEATURES_H_

#include <octoon/game_feature.h>

namespace octoon
{
	class GameBaseFeatures final : public GameFeature
	{
		OctoonDeclareSubClass(GameBaseFeatures, GameFeature)
	public:
		GameBaseFeatures() noexcept;
		~GameBaseFeatures() noexcept;

	private:
		void onFrameBegin() noexcept override;
		void onFrame() noexcept override;
		void onFrameEnd() noexcept override;
	};
}

#endif