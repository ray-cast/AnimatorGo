#include "mysticLit_profile.h"

namespace MysticLit
{
	MysticLitProfile::MysticLitProfile() noexcept
		: denoiseModule_(std::make_shared<DenoiseModule>())
		, physicsModule_(std::make_shared<PhysicsModule>())
		, h264Module_(std::make_shared<H264Module>())
		, timeModule_(std::make_shared<TimeModule>())
		, fileModule_(std::make_shared<FileModule>())
		, entitiesModule_(std::make_shared<EntitiesModule>())
		, offlineModule_(std::make_shared<OfflineModule>())
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