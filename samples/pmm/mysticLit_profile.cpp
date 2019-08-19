#include "mysticLit_profile.h"

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
	{
	}

	MysticLitProfile::~MysticLitProfile() noexcept
	{
	}

	MysticLitProfile
	MysticLitProfile::load(const std::string& path) noexcept
	{
		return MysticLitProfile();
	}

	bool
	MysticLitProfile::save(const std::string& path, const MysticLitProfile& profile) noexcept
	{
		return false;
	}
}