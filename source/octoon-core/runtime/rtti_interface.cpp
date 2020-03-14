#include <octoon/runtime/rtti_interface.h>

namespace octoon
{
	namespace runtime
	{
		OctoonImplementClass(RttiInterface, "RTTI")

		bool
		RttiInterface::isInstanceOf(const Rtti* rtti) const noexcept
		{
			return this->rtti() == rtti;
		}

		bool
		RttiInterface::isInstanceOf(const Rtti& rtti) const noexcept
		{
			return this->rtti() == &rtti;
		}

		bool
		RttiInterface::isInstanceOf(std::string_view className) const noexcept
		{
			return this->rtti()->type_name() == className;
		}

		bool
		RttiInterface::isA(const Rtti* rtti) const noexcept
		{
			return this->rtti()->isDerivedFrom(rtti);
		}

		bool
		RttiInterface::isA(const Rtti& rtti) const noexcept
		{
			return this->rtti()->isDerivedFrom(rtti);
		}

		bool
		RttiInterface::isA(std::string_view rttiName) const noexcept
		{
			return this->rtti()->isDerivedFrom(rttiName);
		}
	}
}