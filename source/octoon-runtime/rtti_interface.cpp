#include <octoon/runtime/rtti_interface.h>

namespace octoon
{
	namespace runtime
	{
		OctoonImplementClass(RttiInterface, "RTTI")

		bool
		RttiInterface::is_instance_of(const Rtti* rtti) const noexcept
		{
			return this->rtti() == rtti;
		}

		bool
		RttiInterface::is_instance_of(const Rtti& rtti) const noexcept
		{
			return this->rtti() == &rtti;
		}

		bool
		RttiInterface::is_instance_of(const std::string& className) const noexcept
		{
			return this->rtti()->type_name() == className;
		}

		bool
		RttiInterface::is_instance_of(const char* const className) const noexcept
		{
			return this->rtti()->type_name() == className;
		}

		bool
		RttiInterface::is_a(const Rtti* rtti) const noexcept
		{
			return this->rtti()->is_derived_from(rtti);
		}

		bool
		RttiInterface::is_a(const Rtti& rtti) const noexcept
		{
			return this->rtti()->is_derived_from(rtti);
		}

		bool
		RttiInterface::is_a(const char* const rttiName) const noexcept
		{
			return this->rtti()->is_derived_from(rttiName);
		}

		bool
		RttiInterface::is_a(const std::string& rttiName) const noexcept
		{
			return this->rtti()->is_derived_from(rttiName);
		}
	}
}