#include <octoon/runtime/rtti_factory.h>

namespace octoon
{
	namespace runtime
	{
		namespace rtti
		{
			bool
			Factory::open() noexcept
			{
				for (auto& it : rttis_)
				{
					if (it)
						rttiLists_[it->type_name()] = it;
				}
				return true;
			}

			bool
			Factory::add(Rtti* rtti) noexcept
			{
				rttis_.push_back(rtti);
				return true;
			}

			Rtti*
			Factory::getRTTI(const std::string& name) noexcept
			{
				return rttiLists_[name];
			}

			Rtti*
			Factory::getRTTI(const char* name) noexcept
			{
				return rttiLists_[name];
			}

			const Rtti*
			Factory::getRTTI(const std::string& name) const noexcept
			{
				auto it = rttiLists_.lower_bound(name);
				for (; it != rttiLists_.end(); ++it)
					if ((*it).first == name)
						return (*it).second;
				return nullptr;
			}

			const Rtti*
			Factory::getRTTI(const char* name) const noexcept
			{
				auto it = rttiLists_.lower_bound(name);
				for (; it != rttiLists_.end(); ++it)
					if ((*it).first == name)
						return (*it).second;
				return nullptr;
			}

			InterfacePtr
			Factory::createObject(const char* name, const Rtti& base) const except
			{
				assert(name);

				auto rtti = this->getRTTI(name);
				if (!rtti)
					return nullptr;

				if (rtti->isDerivedFrom(base))
					return rtti->create();

				return nullptr;
			}

			InterfacePtr
			Factory::createObject(const std::string& name, const Rtti& base) const except
			{
				assert(!name.empty());

				auto rtti = this->getRTTI(name);
				if (!rtti)
					return nullptr;

				if (rtti->isDerivedFrom(base))
					return rtti->create();

				return nullptr;
			}
		}
	}
}