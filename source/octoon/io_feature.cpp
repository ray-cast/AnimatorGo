#if defined(OCTOON_FEATURE_IO_ENABLE)
#include <octoon/io_feature.h>
#include <octoon/io/ioserver.h>
#include <octoon/io/fpackage.h>

namespace octoon
{
	OctoonImplementSubClass(IOFeature, GameFeature, "IOFeature")

	IOFeature::IOFeature() noexcept
		: systemPath_("../../system/")
		, diskPath_("")
	{
	}

	IOFeature::~IOFeature() noexcept
	{
	}

	void
	IOFeature::onActivate() except
	{
		io::IoServer::instance()->mount_package("sys", std::make_shared<octoon::io::fpackage>(systemPath_));
		io::IoServer::instance()->mount_package("file", std::make_shared<octoon::io::fpackage>(diskPath_));
	}

	void
	IOFeature::onDeactivate() noexcept
	{
		io::IoServer::instance()->unmount_archive("sys");
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