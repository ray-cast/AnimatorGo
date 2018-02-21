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
		virtual void on_frame_begin() noexcept override;
		virtual void on_frame() noexcept override;
		virtual void on_frame_end() noexcept override;
	};
}

#endif