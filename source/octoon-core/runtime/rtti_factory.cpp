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
		RttiFactory::getRtti(std::string_view name) noexcept
		{
			auto it = rttiLists_.lower_bound(name);
			for (; it != rttiLists_.end(); ++it)
				if ((*it).first == name)
					return (*it).second;
			return nullptr;
		}

		const Rtti*
		RttiFactory::getRtti(std::string_view name) const noexcept
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
		RttiFactory::createInstance(std::string_view name, const Rtti& base) const except
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