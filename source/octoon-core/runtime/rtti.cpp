#include <octoon/runtime/rtti.h>
#include <octoon/runtime/rtti_factory.h>

namespace octoon
{
	Rtti::Rtti(std::string_view name, RttiConstruct creator, const Rtti* parent) noexcept
		: name_(name)
		, parent_(parent)
		, construct_(creator)
	{
		RttiFactory::instance()->add(this);
	}

	std::shared_ptr<Object>
	Rtti::create() const noexcept(false) //throw(std::bad_alloc)
	{
		assert(construct_);
		return std::shared_ptr<Object>(construct_());
	}

	const Rtti*
	Rtti::getParent() const noexcept
	{
		return parent_;
	}

	const std::string&
	Rtti::type_name() const noexcept
	{
		return name_;
	}

	bool
	Rtti::isDerivedFrom(const Rtti* other) const noexcept
	{
		assert(other);

		for (const Rtti* cur = this; cur != 0; cur = cur->getParent())
		{
			if (cur == other)
			{
				return true;
			}
		}

		return false;
	}

	bool
	Rtti::isDerivedFrom(const Rtti& other) const noexcept
	{
		return this->isDerivedFrom(&other);
	}

	bool
	Rtti::isDerivedFrom(std::string_view name) const noexcept
	{
		for (const Rtti* cur = this; cur != 0; cur = cur->getParent())
		{
			if (cur->name_ == name)
				return true;
		}

		return false;
	}
}