#ifndef OCTOON_RTTI_SINGLETON_H_
#define OCTOON_RTTI_SINGLETON_H_

#include <octoon/runtime/object.h>
#include <type_traits>

namespace octoon
{
	template<typename T, typename = std::enable_if_t<std::is_class<T>::value>>
	class RttiSingleton : public Object
	{
	public:
		static T* instance() noexcept
		{
			return instance_;
		}

	protected:
		RttiSingleton(void) = default;
		virtual ~RttiSingleton(void) = default;

	private:
		RttiSingleton(const RttiSingleton&) = delete;
		RttiSingleton& operator=(const RttiSingleton&) = delete;

	private:
		static T* instance_;
	};

	namespace singleton
	{
		OCTOON_EXPORT Object* instance(const Rtti* rtti) noexcept;
		OCTOON_EXPORT Object* instance(const Rtti& rtti) noexcept;
	}

	template<typename _Tx, typename _Ty> _Tx* RttiSingleton<_Tx, _Ty>::instance_ = singleton::instance(_Tx::RTTI)->template downcast<_Tx>();
}

#endif