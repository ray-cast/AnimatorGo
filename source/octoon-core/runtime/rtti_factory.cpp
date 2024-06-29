#include <octoon/runtime/rtti_factory.h>

namespace octoon
{
	OctoonImplementSingleton(RttiFactory)

	void
	RttiFactory::open() noexcept
	{
		for (auto& it : caches_)
			types_[it->type_name()] = it;
		caches_.clear();
	}

	void
	RttiFactory::add(Rtti* rtti) noexcept
	{
		assert(rtti);
		caches_.push_back(rtti);
	}

	Rtti*
	RttiFactory::getRtti(std::string_view name) noexcept
	{
		auto it = types_.lower_bound(name);
		for (; it != types_.end(); ++it)
			if ((*it).first == name)
				return (*it).second;
		return nullptr;
	}

	const Rtti*
	RttiFactory::getRtti(std::string_view name) const noexcept
	{
		auto it = types_.lower_bound(name);
		for (; it != types_.end(); ++it)
			if ((*it).first == name)
				return (*it).second;
		return nullptr;
	}

	std::shared_ptr<Object>
	RttiFactory::createInstance(const char* name, const Rtti& base) const noexcept(false)
	{
		assert(name);

		auto rtti = this->getRtti(name);
		if (!rtti)
			return nullptr;

		if (rtti->isDerivedFrom(base))
			return rtti->create();

		return nullptr;
	}

	std::shared_ptr<Object>
	RttiFactory::createInstance(std::string_view name, const Rtti& base) const noexcept(false)
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