#ifndef RABBIT_PROFILE_H_
#define RABBIT_PROFILE_H_

#include <memory>
#include <string>

#include "module/denoise_module.h"
#include "module/h264_module.h"
#include "module/physics_module.h"
#include "module/time_module.h"
#include "module/file_module.h"
#include "module/entities_module.h"
#include "module/offline_module.h"
#include "module/canvas_module.h"
#include "module/mark_module.h"
#include "module/sun_module.h"
#include "module/environment_module.h"
#include "module/client_module.h"

namespace rabbit
{
	class RabbitProfile
	{
	public:
		RabbitProfile() noexcept;
		RabbitProfile(const std::string& path) noexcept(false);
		virtual ~RabbitProfile() noexcept;

		static std::unique_ptr<RabbitProfile> load(const std::string& path) noexcept(false);
		static void save(const std::string& path, const RabbitProfile& profile) noexcept(false);

	private:
		RabbitProfile(const RabbitProfile&) = delete;
		RabbitProfile& operator=(const RabbitProfile&) = delete;

	public:
		std::shared_ptr<CanvasModule> canvasModule;
		std::shared_ptr<FileModule> fileModule;
		std::shared_ptr<EntitiesModule> entitiesModule;
		std::shared_ptr<DenoiseModule> denoiseModule;
		std::shared_ptr<H264Module> h264Module;
		std::shared_ptr<PhysicsModule> physicsModule;
		std::shared_ptr<TimeModule> timeModule;
		std::shared_ptr<OfflineModule> offlineModule;
		std::shared_ptr<MarkModule> markModule;
		std::shared_ptr<ClientModule> clientModule;
		std::shared_ptr<SunModule> sunModule;
		std::shared_ptr<EnvironmentModule> environmentModule;
	};
}

#endif