#include "rabbit_profile.h"
#include <fstream>
#include <filesystem>
#include <octoon/runtime/json.h>

namespace rabbit
{
	RabbitProfile::RabbitProfile() noexcept
		: denoiseModule(std::make_shared<DenoiseModule>())
		, physicsModule(std::make_shared<PhysicsModule>())
		, h264Module(std::make_shared<H264Module>())
		, timeModule(std::make_shared<TimeModule>())
		, fileModule(std::make_shared<FileModule>())
		, entitiesModule(std::make_shared<EntitiesModule>())
		, offlineModule(std::make_shared<OfflineModule>())
		, canvasModule(std::make_shared<CanvasModule>())
		, markModule(std::make_shared<MarkModule>())
		, sunModule(std::make_shared<SunModule>())
		, environmentModule(std::make_shared<EnvironmentModule>())
		, clientModule(std::make_shared<ClientModule>())
		, materialModule(std::make_shared<MaterialModule>())
		, dragModule(std::make_shared<DragModule>())
		, gridModule(std::make_shared<GridModule>())
	{
	}

	RabbitProfile::RabbitProfile(std::string_view path) noexcept(false)
		: RabbitProfile()
	{
		std::ifstream stream(path);
		if (stream)
		{
			auto json = octoon::runtime::json::parse(stream);
			this->denoiseModule->load(json["denoise"]);
			this->physicsModule->load(json["physics"]);
			this->h264Module->load(json["h264"]);
			this->timeModule->load(json["time"]);
			this->fileModule->load(json["file"]);
			this->entitiesModule->load(json["entities"]);
			this->offlineModule->load(json["offline"]);
			this->canvasModule->load(json["canvas"]);
			this->markModule->load(json["mark"]);
			this->sunModule->load(json["sun"]);
			this->environmentModule->load(json["environment"]);
			this->clientModule->load(json["client"]);
			this->materialModule->load(json["material"]);
			this->dragModule->load(json["drag"]);
			this->gridModule->load(json["grid"]);
		}
	}

	RabbitProfile::~RabbitProfile() noexcept
	{
	}

	std::unique_ptr<RabbitProfile>
	RabbitProfile::load(std::string_view path) noexcept(false)
	{
		return std::make_unique<RabbitProfile>(path);
	}

	void
	RabbitProfile::save(std::string_view path, const RabbitProfile& profile) noexcept(false)
	{
		std::ofstream stream(path);
		if (stream)
		{
			octoon::runtime::json json;
			profile.denoiseModule->save(json["denoise"]);
			profile.physicsModule->save(json["physics"]);
			profile.h264Module->save(json["h264"]);
			profile.timeModule->save(json["time"]);
			profile.fileModule->save(json["file"]);
			profile.entitiesModule->save(json["entities"]);
			profile.offlineModule->save(json["offline"]);
			profile.canvasModule->save(json["canvas"]);
			profile.markModule->save(json["mark"]);
			profile.sunModule->save(json["sun"]);
			profile.environmentModule->save(json["environment"]);
			profile.clientModule->save(json["client"]);
			profile.materialModule->save(json["material"]);
			profile.dragModule->save(json["drag"]);
			profile.gridModule->save(json["grid"]);

			auto string = json.dump();
			stream.write(string.c_str(), string.size());
		}
		else
		{
			throw std::runtime_error(u8"无法创建文件: " + std::string(path));
		}
	}
}