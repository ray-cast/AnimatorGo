#ifndef OCTOON_OFFLINE_FEATURE_H_
#define OCTOON_OFFLINE_FEATURE_H_

#include <octoon/game_feature.h>
#include <octoon/hal/graphics_framebuffer.h>

namespace octoon
{
	class OCTOON_EXPORT OfflineFeature final : public GameFeature
	{
		OctoonDeclareSubClass(OfflineFeature, GameFeature)
	public:
		OfflineFeature() noexcept;
		OfflineFeature(std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) noexcept;
		~OfflineFeature() noexcept;

		void setFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept;
		void getFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept;

		void setFramebufferDirty(bool dirty) noexcept;
		bool getFramebufferDirty() const noexcept;

		void* getScene() const noexcept;
		void* getContext() const noexcept;
		void* getMaterialSystem() const noexcept;

		hal::GraphicsFramebufferPtr getFramebuffer() const noexcept;

	private:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onInputEvent(const runtime::any& data) noexcept;

		void onFrameBegin() noexcept override;
		void onFrame() noexcept override;
		void onFrameEnd() noexcept override;

		void onFramebufferChange(std::uint32_t w, std::uint32_t h) except;

	private:
		bool dirty_;

		void* rprScene_;
		void* rprContext_;
		void* rprMaterialSystem_;
		void* rprDenoise_;

		void* rprFramebuffer_;
		void* colorFramebuffer_;
		void* normalFramebuffer_;
		void* positionFramebuffer_;
		void* albedoFramebuffer_;
		void* objectIdFramebuffer_;

		hal::GraphicsFramebufferPtr framebuffer_;

		std::uint32_t framebuffer_w_;
		std::uint32_t framebuffer_h_;
	};
}

#endif