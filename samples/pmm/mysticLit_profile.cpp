#include "mysticLit_profile.h"
#include <fstream>
#include <filesystem>
#include <octoon/runtime/json.h>

namespace MysticLit
{
	MysticLitProfile::MysticLitProfile() noexcept
		: denoiseModule(std::make_shared<DenoiseModule>())
		, physicsModule(std::make_shared<PhysicsModule>())
		, h264Module(std::make_shared<H264Module>())
		, timeModule(std::make_shared<TimeModule>())
		, fileModule(std::make_shared<FileModule>())
		, entitiesModule(std::make_shared<EntitiesModule>())
		, offlineModule(std::make_shared<OfflineModule>())
		, canvasModule(std::make_shared<CanvasModule>())
		, markModule(std::make_shared<MarkModule>())
	{
	}

	MysticLitProfile::MysticLitProfile(const std::string& path) noexcept
		: MysticLitProfile()
	{
		try
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
			}
		}
		catch (...)
		{
		}
	}

	MysticLitProfile::~MysticLitProfile() noexcept
	{
	}

	std::unique_ptr<MysticLitProfile>
	MysticLitProfile::load(const std::string& path) noexcept
	{
		return std::make_unique<MysticLitProfile>(path);
	}

	bool
	MysticLitProfile::save(const std::string& path, const MysticLitProfile& profile) noexcept
	{
		try
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

				auto string = json.dump();
				stream.write(string.c_str(), string.size());

				return true;
			}
		}
		catch (...)
		{
		}

		return false;
	}
}