#include <octoon/runtime/rtti_factory.h>

namespace octoon
{
	namespace runtime
	{
		bool
		RttiFactory::open() noexcept
		{
			for (auto& it : rttis_)
			{
				if (it)
					rttiLists_[it->type_name()] = it;
			}
			return true;
		}

		bool
		RttiFactory::add(Rtti* rtti) noexcept
		{
			rttis_.push_back(rtti);
			return true;
		}

		Rtti*
		RttiFactory::getRtti(const std::string& name) noexcept
		{
			return rttiLists_[name];
		}

		Rtti*
		RttiFactory::getRtti(const char* name) noexcept
		{
			return rttiLists_[name];
		}

		const Rtti*
		RttiFactory::getRtti(const std::string& name) const noexcept
		{
			auto it = rttiLists_.lower_bound(name);
			for (; it != rttiLists_.end(); ++it)
				if ((*it).first == name)
					return (*it).second;
			return nullptr;
		}

		const Rtti*
		RttiFactory::getRtti(const char* name) const noexcept
		{
			auto it = rttiLists_.lower_bound(name);
			for (; it != rttiLists_.end(); ++it)
				if ((*it).first == name)
					return (*it).second;
			return nullptr;
		}

		RttiInterfacePtr
		RttiFactory::createInstance(const char* name, const Rtti& base) const except
		{
			assert(name);

			auto rtti = this->getRtti(name);
			if (!rtti)
				return nullptr;

			if (rtti->isDerivedFrom(base))
				return rtti->create();

			return nullptr;
		}

		RttiInterfacePtr
		RttiFactory::createInstance(const std::string& name, const Rtti& base) const except
		{
			assert(!name.empty());

			auto rtti = this->getRtti(name);
			if (!rtti)
				return nullptr;

			if (rtti->isDerivedFrom(base))
				return rtti->create();

			return nullptr;
		}
	}
}