#include <octoon/runtime/rtti_singleton.h>
#include <octoon/runtime/rtti_factory.h>

namespace octoon
{
	namespace runtime
	{
		namespace singleton
		{
			std::map<const Rtti*, std::shared_ptr<RttiInterface>> singletons;

			RttiInterface* instance(const Rtti* rtti) noexcept
			{
				auto& instance = singletons[rtti];
				if (!instance)
					instance = rtti->create();
				return instance.get();
			}

			RttiInterface* instance(const Rtti& rtti) noexcept
			{
				return instance(&rtti);
			}
		}
	}
}