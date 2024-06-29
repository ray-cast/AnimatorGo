#if defined(OCTOON_FEATURE_IO_ENABLE)
#include <octoon/io_feature.h>
#include <octoon/asset_database.h>

namespace octoon
{
	OctoonImplementSubClass(IOFeature, GameFeature, "IOFeature")

	IOFeature::IOFeature() noexcept
		: diskPath_("")
	{
	}

	IOFeature::~IOFeature() noexcept
	{
	}

	void
	IOFeature::onActivate() except
	{
		AssetDatabase::instance()->mountPackage(u8"", diskPath_);
	}

	void
	IOFeature::onDeactivate() noexcept
	{
		AssetDatabase::instance()->unmountPackage(u8"");
	}

	void
	IOFeature::onFrameBegin() noexcept
	{
	}

	void
	IOFeature::onFrame() noexcept
	{
	}

	void
	IOFeature::onFrameEnd() noexcept
	{
	}
}
#endif