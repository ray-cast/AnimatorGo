#include <octoon/runtime/rtti.h>
#include <octoon/runtime/rtti_factory.h>

namespace octoon
{
	namespace runtime
	{
		namespace rtti
		{
			OctoonImplementClass(Interface, "RTTI")

			Rtti::Rtti(const std::string& name, RttiConstruct creator, const Rtti* parent) noexcept
				: name_(name)
				, parent_(parent)
				, construct_(creator)
			{
				Factory::instance()->add(this);
			}

			InterfacePtr
			Rtti::create() const throw(std::bad_alloc)
			{
				assert(construct_);
				return std::shared_ptr<Interface>(construct_());
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
			Rtti::isDerivedFrom(const std::string& name) const noexcept
			{
				for (const Rtti* cur = this; cur != 0; cur = cur->getParent())
				{
					if (cur->name_ == name)
					{
						return true;
					}
				}

				return false;
			}

			bool
			Rtti::isDerivedFrom(const char* const name) const noexcept
			{
				for (const Rtti* cur = this; cur != 0; cur = cur->getParent())
				{
					if (cur->name_ == name)
					{
						return true;
					}
				}

				return false;
			}

			bool
			Interface::isInstanceOf(const Rtti* rtti) const noexcept
			{
				return this->rtti() == rtti;
			}

			bool
			Interface::isInstanceOf(const Rtti& rtti) const noexcept
			{
				return this->rtti() == &rtti;
			}

			bool
			Interface::isInstanceOf(const std::string& className) const noexcept
			{
				return this->rtti()->type_name() == className;
			}

			bool
			Interface::isInstanceOf(const char* const className) const noexcept
			{
				return this->rtti()->type_name() == className;
			}

			bool
			Interface::isA(const Rtti* rtti) const noexcept
			{
				return this->rtti()->isDerivedFrom(rtti);
			}

			bool
			Interface::isA(const Rtti& rtti) const noexcept
			{
				return this->rtti()->isDerivedFrom(rtti);
			}

			bool
			Interface::isA(const char* const rttiName) const noexcept
			{
				return this->rtti()->isDerivedFrom(rttiName);
			}

			bool
			Interface::isA(const std::string& rttiName) const noexcept
			{
				return this->rtti()->isDerivedFrom(rttiName);
			}
		}
	}
}