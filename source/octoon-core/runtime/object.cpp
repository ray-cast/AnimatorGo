#include <octoon/runtime/object.h>

namespace octoon
{
	OctoonImplementClass(Object, "Object")

	Object::Object() noexcept
		: localIdentifier_(0)
	{
	}

	Object::~Object() noexcept
	{
	}

	void
	Object::setLocalIdentifier(long id) noexcept
	{
		localIdentifier_ = id;
	}

	long
	Object::getLocalIdentifier() const noexcept
	{
		return localIdentifier_;
	}

	bool
	Object::isInstanceOf(const Rtti* rtti) const noexcept
	{
		return this->rtti() == rtti;
	}

	bool
	Object::isInstanceOf(const Rtti& rtti) const noexcept
	{
		return this->rtti() == &rtti;
	}

	bool
	Object::isInstanceOf(std::string_view className) const noexcept
	{
		return this->rtti()->type_name() == className;
	}

	bool
	Object::isA(const Rtti* rtti) const noexcept
	{
		return this->rtti()->isDerivedFrom(rtti);
	}

	bool
	Object::isA(const Rtti& rtti) const noexcept
	{
		return this->rtti()->isDerivedFrom(rtti);
	}

	bool
	Object::isA(std::string_view rttiName) const noexcept
	{
		return this->rtti()->isDerivedFrom(rttiName);
	}
}