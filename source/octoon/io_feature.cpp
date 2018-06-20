#if defined(OCTOON_FEATURE_IO_ENABLE)
#include <octoon/io_feature.h>
#include <octoon/io/ioserver.h>
#include <octoon/io/farchive.h>

namespace octoon
{
	OctoonImplementSubClass(IOFeature, GameFeature, "IOFeature")

	IOFeature::IOFeature() noexcept
		: systemPath_("../../system")
		, diskPath_("")
	{
	}

	IOFeature::~IOFeature() noexcept
	{
	}

	void
	IOFeature::onActivate() except
	{
		io::IoServer::instance()->mount_archive("sys", std::make_shared<octoon::io::farchive>(systemPath_));
		io::IoServer::instance()->mount_archive("file", std::make_shared<octoon::io::farchive>(diskPath_));
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