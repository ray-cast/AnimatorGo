#ifndef OCTOON_PRORENDER_FEATURE_H_
#define OCTOON_PRORENDER_FEATURE_H_

#include <memory>
#include <octoon/game_feature.h>

namespace octoon
{
	class ProrenderFeature final : public GameFeature
	{
		OctoonDeclareSubClass(ProrenderFeature, GameFeature)
	public:
		ProrenderFeature() noexcept;
		// ProrenderFeature(WindHandle window, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) noexcept;
		~ProrenderFeature() noexcept;

	private:
		virtual void on_activate() except;
		virtual void on_deactivate() noexcept;

        virtual void onReset() except;

		virtual void on_open_scene(const GameScenePtr& scene) except;
		virtual void on_close_scene(const GameScenePtr& scene) noexcept;
	private:
		
	};
}

#endif // OCTOON_PRORENDER_FEATURE_H_