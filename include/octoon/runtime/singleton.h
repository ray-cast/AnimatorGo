#ifndef OCTOON_SINGLETON_H_
#define OCTOON_SINGLETON_H_

#include <octoon/runtime/platform.h>
#include <type_traits>

namespace octoon
{
	namespace runtime
	{
		template<typename T, typename = std::enable_if_t<std::is_class<T>::value>>
		class Singleton
		{
		public:
			static T* instance() noexcept
			{
				return &instance_;
			}

		protected:
			Singleton(void) = default;
			virtual ~Singleton(void) = default;

		private:
			Singleton(const Singleton&) = delete;
			Singleton& operator=(const Singleton&) = delete;

		private:
			static T instance_;
		};

		template<typename _Tx, typename _Ty> _Tx Singleton<_Tx, _Ty>::instance_;
	}
}

#define OctoonDeclareSingleton(type) \
public:\
    static type* instance();\
private:

#define OctoonImplementSingleton(type) \
type* type::instance() \
{\
    return runtime::Singleton<type>::instance();\
}

#endif