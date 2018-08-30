#ifndef OCTOON_UI_ASSET_WINDOW_H_
#define OCTOON_UI_ASSET_WINDOW_H_

#include <octoon/game_component.h>

namespace octoon
{
	namespace editor
	{
		class AssetWindow final : public GameComponent
		{
			OctoonDeclareSubClass(AssetWindow, GameComponent)
		public:
			AssetWindow() noexcept;
			~AssetWindow() noexcept;

			GameComponentPtr clone() const noexcept override;

		private:
			void onActivate() noexcept override;
			void onDeactivate() noexcept override;

			void onGui() noexcept override;

		private:
			bool isShowedAssetsWindow_;
		};
	}
}

#endif