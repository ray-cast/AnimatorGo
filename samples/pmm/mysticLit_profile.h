#ifndef MYSTICLIT_PROFILE_H_
#define MYSTICLIT_PROFILE_H_

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

namespace MysticLit
{
	class MysticLitProfile
	{
	public:
		MysticLitProfile() noexcept;
		MysticLitProfile(const std::string& path) noexcept;
		virtual ~MysticLitProfile() noexcept;

		static std::unique_ptr<MysticLitProfile> load(const std::string& path) noexcept;
		static bool save(const std::string& path, const MysticLitProfile& profile) noexcept;

	private:
		MysticLitProfile(const MysticLitProfile&) = delete;
		MysticLitProfile& operator=(const MysticLitProfile&) = delete;

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
	};
}

#endif