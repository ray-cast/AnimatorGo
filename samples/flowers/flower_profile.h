#ifndef FLOWER_PROFILE_H_
#define FLOWER_PROFILE_H_

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

namespace flower
{
	class FlowerProfile
	{
	public:
		FlowerProfile() noexcept;
		FlowerProfile(std::string_view path) noexcept(false);
		virtual ~FlowerProfile() noexcept;

		static std::unique_ptr<FlowerProfile> load(std::string_view path) noexcept(false);
		static void save(std::string_view path, const FlowerProfile& profile) noexcept(false);

	private:
		FlowerProfile(const FlowerProfile&) = delete;
		FlowerProfile& operator=(const FlowerProfile&) = delete;

	public:
		std::shared_ptr<CanvasModule> canvasModule;
		std::shared_ptr<FileModule> fileModule;
		std::shared_ptr<EntitiesModule> entitiesModule;
		std::shared_ptr<DenoiseModule> denoiseModule;
		std::shared_ptr<H265Module> h265Module;
		std::shared_ptr<PhysicsModule> physicsModule;
		std::shared_ptr<PlayerModule> playerModule;
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