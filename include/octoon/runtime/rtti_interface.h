#ifndef OCTOON_RTTI_ITERFACE_H_
#define OCTOON_RTTI_ITERFACE_H_

#include <string>
#include <memory>
#include <vector>

#include <octoon/runtime/rtti.h>
#include <octoon/runtime/rtti_macros.h>

namespace octoon
{
	namespace runtime
	{
		typedef std::shared_ptr<class RttiInterface> RttiInterfacePtr;

		class OCTOON_EXPORT RttiInterface : public std::enable_shared_from_this<RttiInterface>
		{
			OctoonDeclareClass(RttiInterface)
		public:
			RttiInterface() = default;
			virtual ~RttiInterface() = default;

			bool isInstanceOf(const Rtti* rtti) const noexcept;
			bool isInstanceOf(const Rtti& rtti) const noexcept;
			bool isInstanceOf(const char* const className) const noexcept;
			bool isInstanceOf(const std::string& className) const noexcept;

			template<typename T>
			bool isInstanceOf() const noexcept
			{
				return this->isInstanceOf(T::getRtti());
			}

			bool isA(const Rtti* rtti) const noexcept;
			bool isA(const Rtti& rtti) const noexcept;
			bool isA(const char* const rttiName) const noexcept;
			bool isA(const std::string& rttiName) const noexcept;

			template<typename T>
			bool isA() const noexcept
			{
				return this->isA(T::getRtti());
			}

			template<typename T>
			T* upcast() noexcept
			{
				assert(this->isA<T>());
				return dynamic_cast<T*>(this);
			}

			template<typename T>
			T* downcast() noexcept
			{
				assert(this->isA<T>());
				return dynamic_cast<T*>(this);
			}

			template<typename T>
			T* cast() noexcept
			{
				return reinterpret_cast<T*>(this);
			}

			template<typename T>
			const T* upcast() const noexcept
			{
				assert(this->isA<T>());
				return dynamic_cast<const T*>(this);
			}

			template<typename T>
			const T* downcast() const noexcept
			{
				assert(this->isA<T>());
				return dynamic_cast<const T*>(this);
			}

			template<typename T>
			const T* cast() const noexcept
			{
				return reinterpret_cast<const T*>(this);
			}

			template<typename T>
			std::shared_ptr<T> upcast_pointer() noexcept
			{
				assert(this->isA<T>());
				return std::dynamic_pointer_cast<T>(this->shared_from_this());
			}

			template<typename T>
			std::shared_ptr<T> downcast_pointer() noexcept
			{
				assert(this->isA<T>());
				return std::dynamic_pointer_cast<T>(this->shared_from_this());
			}

			template<typename T>
			std::shared_ptr<T> cast_pointer() noexcept
			{
				assert(this->isA<T>());
				return std::dynamic_pointer_cast<T>(this->shared_from_this());
			}

			template<typename T>
			std::shared_ptr<const T> upcast_pointer() const noexcept
			{
				assert(this->isA<T>());
				return std::dynamic_pointer_cast<const T>(this->shared_from_this());
			}

			template<typename T>
			std::shared_ptr<const T> downcast_pointer() const noexcept
			{
				assert(this->isA<T>());
				return std::dynamic_pointer_cast<const T>(this->shared_from_this());
			}

			template<typename T>
			std::shared_ptr<const T> cast_pointer() const noexcept
			{
				assert(this->isA<T>());
				return std::dynamic_pointer_cast<const T>(this->shared_from_this());
			}
		};
	}
}

#endif