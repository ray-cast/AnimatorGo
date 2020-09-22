#ifndef RABBIT_PROFILE_H_
#define RABBIT_PROFILE_H_

#include <memory>
#include <string>

#include "module/denoise_module.h"
#include "module/h265_module.h"
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
#include "module/material_module.h"
#include "module/drag_module.h"
#include "module/grid_module.h"

namespace rabbit
{
	class RabbitProfile
	{
	public:
		RabbitProfile() noexcept;
		RabbitProfile(std::string_view path) noexcept(false);
		virtual ~RabbitProfile() noexcept;

		static std::unique_ptr<RabbitProfile> load(std::string_view path) noexcept(false);
		static void save(std::string_view path, const RabbitProfile& profile) noexcept(false);

	private:
		RabbitProfile(const RabbitProfile&) = delete;
		RabbitProfile& operator=(const RabbitProfile&) = delete;

	public:
		std::shared_ptr<CanvasModule> canvasModule;
		std::shared_ptr<FileModule> fileModule;
		std::shared_ptr<EntitiesModule> entitiesModule;
		std::shared_ptr<DenoiseModule> denoiseModule;
		std::shared_ptr<H265Module> h265Module;
		std::shared_ptr<PhysicsModule> physicsModule;
		std::shared_ptr<TimeModule> timeModule;
		std::shared_ptr<OfflineModule> offlineModule;
		std::shared_ptr<MarkModule> markModule;
		std::shared_ptr<ClientModule> clientModule;
		std::shared_ptr<SunModule> sunModule;
		std::shared_ptr<EnvironmentModule> environmentModule;
		std::shared_ptr<MaterialModule> materialModule;
		std::shared_ptr<DragModule> dragModule;
		std::shared_ptr<GridModule> gridModule;
	};
}

#endif