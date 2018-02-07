#ifndef OCTOON_SINGLETON_H_
#define OCTOON_SINGLETON_H_

#include <octoon/runtime/platform.h>

namespace octoon
{
	namespace runtime
	{
		template<typename T, typename = std::enable_if_t<std::is_class_v<T>>>
		class Singleton
		{
		public:
			static T* instance() noexcept
			{
				return &_instance;
			}

		protected:
			Singleton(void) = default;
			virtual ~Singleton(void) = default;

		private:
			Singleton(const Singleton&) = delete;
			Singleton& operator=(const Singleton&) = delete;

		private:
			static T _instance;
		};

		template<typename _Tx, typename _Ty> _Tx Singleton<_Tx, _Ty>::_instance;
	}
}

#endif