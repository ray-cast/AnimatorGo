#ifndef OCTOON_GAME_BASE_FEATURES_H_
#define OCTOON_GAME_BASE_FEATURES_H_

#include <octoon/game_feature.h>

namespace octoon
{
	class GameBaseFeatures : public GameFeature
	{
		OctoonDeclareSubClass(GameBaseFeatures, GameFeature)
	public:
		GameBaseFeatures() noexcept;
		~GameBaseFeatures() noexcept;

	private:

		virtual void onFrameBegin() noexcept;
		virtual void onFrame() noexcept;
		virtual void onFrameEnd() noexcept;
	};
}

#endif