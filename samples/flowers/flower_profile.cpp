#include "flower_profile.h"
#include <fstream>
#include <filesystem>
#include <octoon/runtime/json.h>

namespace flower
{
	FlowerProfile::FlowerProfile() noexcept
		: physicsModule(std::make_shared<PhysicsModule>())
		, h265Module(std::make_shared<H265Module>())
		, playerModule(std::make_shared<PlayerModule>())
		, fileModule(std::make_shared<FileModule>())
		, entitiesModule(std::make_shared<EntitiesModule>())
		, offlineModule(std::make_shared<OfflineModule>())
		, canvasModule(std::make_shared<RecordModule>())
		, markModule(std::make_shared<MarkModule>())
		, sunModule(std::make_shared<SunModule>())
		, environmentModule(std::make_shared<EnvironmentModule>())
		, clientModule(std::make_shared<ClientModule>())
		, materialModule(std::make_shared<MaterialModule>())
		, dragModule(std::make_shared<DragModule>())
		, gridModule(std::make_shared<GridModule>())
	{
	}

	FlowerProfile::FlowerProfile(std::string_view path) noexcept(false)
		: FlowerProfile()
	{
		std::ifstream stream(path);
		if (stream)
		{
			auto json = octoon::runtime::json::parse(stream);
			this->physicsModule->load(json["physics"]);
			this->h265Module->load(json["h265"]);
			this->playerModule->load(json["time"]);
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

	FlowerProfile::~FlowerProfile() noexcept
	{
	}

	std::unique_ptr<FlowerProfile>
	FlowerProfile::load(std::string_view path) noexcept(false)
	{
		return std::make_unique<FlowerProfile>(path);
	}

	void
	FlowerProfile::save(std::string_view path, const FlowerProfile& profile) noexcept(false)
	{
		std::ofstream stream(path);
		if (stream)
		{
			octoon::runtime::json json;
			profile.physicsModule->save(json["physics"]);
			profile.h265Module->save(json["h265"]);
			profile.playerModule->save(json["time"]);
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