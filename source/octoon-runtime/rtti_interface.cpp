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
		RttiInterface::isInstanceOf(const std::string& className) const noexcept
		{
			return this->rtti()->type_name() == className;
		}

		bool
		RttiInterface::isInstanceOf(const char* const className) const noexcept
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
		RttiInterface::isA(const char* const rttiName) const noexcept
		{
			return this->rtti()->isDerivedFrom(rttiName);
		}

		bool
		RttiInterface::isA(const std::string& rttiName) const noexcept
		{
			return this->rtti()->isDerivedFrom(rttiName);
		}
	}
}