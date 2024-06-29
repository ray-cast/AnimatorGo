#include <octoon/runtime/rtti_singleton.h>
#include <octoon/runtime/rtti_factory.h>

namespace octoon
{
	namespace singleton
	{
		std::map<const Rtti*, std::shared_ptr<Object>> singletons;

		Object* instance(const Rtti* rtti) noexcept
		{
			auto& instance = singletons[rtti];
			if (!instance)
				instance = rtti->create();
			return instance.get();
		}

		Object* instance(const Rtti& rtti) noexcept
		{
			return instance(&rtti);
		}
	}
}