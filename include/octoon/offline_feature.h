#ifndef OCTOON_OFFLINE_FEATURE_H_
#define OCTOON_OFFLINE_FEATURE_H_

#include <octoon/game_feature.h>
#include <octoon/offline_listener.h>
#include <octoon/hal/graphics_framebuffer.h>

#include <unordered_map>

namespace octoon
{
	class OCTOON_EXPORT OfflineFeature final : public GameFeature
	{
		OctoonDeclareSubClass(OfflineFeature, GameFeature)
	public:
		OfflineFeature() noexcept;
		OfflineFeature(std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) noexcept;
		~OfflineFeature() noexcept;

		void addOfflineListener(OfflineListener* listener) noexcept;
		void removeOfflineListener(OfflineListener* listener) noexcept;

		void setFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept;
		void getFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept;

		void setFramebufferDirty(bool dirty) noexcept;
		bool getFramebufferDirty() const noexcept;

		void* getScene() const noexcept;
		void* getContext() const noexcept;
		void* getMaterialSystem() const noexcept;

		hal::GraphicsFramebufferPtr getColorFramebuffer() const noexcept;

		void readColorFramebuffer(float dst[]) noexcept;
		void readNormalFramebuffer(float dst[]) noexcept;
		void readAlbedoFramebuffer(float dst[]) noexcept;

		void clearMemory() noexcept;

		std::pair<void*, void*> createMaterialTextures(const std::string& path) noexcept(false);

	private:
		void onActivate() noexcept(false) override;
		void onDeactivate() noexcept override;

		void onInputEvent(const runtime::any& data) noexcept;

		void onFrame() noexcept(false) override;

		void setupFramebuffers(std::uint32_t w, std::uint32_t h) noexcept(false);

	private:
		void setupProRender() noexcept(false);
		void cleanupProRender() noexcept;
		void cleanupFramebuffers() noexcept;

	private:
		bool dirty_;

		void* rprScene_;
		void* rprContext_;
		void* rprMaterialSystem_;

		void* colorFramebuffer_;
		void* normalFramebuffer_;
		void* albedoFramebuffer_;

		hal::GraphicsTexturePtr colorTexture_;
		hal::GraphicsTexturePtr normalTexture_;
		hal::GraphicsTexturePtr albedoTexture_;

		hal::GraphicsFramebufferPtr framebuffer_;

		std::uint32_t framebuffer_w_;
		std::uint32_t framebuffer_h_;

		std::vector<OfflineListener*> listener_;

		std::unordered_map<std::string, std::pair<void*, void*>> images_;
		std::unordered_map<std::string, std::pair<void*, void*>> imageNodes_;
	};
}

#endif