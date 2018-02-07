#ifndef OCTOON_RTTI_H_
#define OCTOON_RTTI_H_

#include <string>
#include <memory>
#include <vector>

#include <octoon/runtime/rtti_macros.h>

namespace octoon
{
	namespace runtime
	{
		namespace rtti
		{
			typedef std::shared_ptr<class Interface> InterfacePtr;

			class DLL_EXPORT Rtti
			{
			public:
				typedef Interface*(*RttiConstruct)();
			public:
				Rtti(const std::string& name, RttiConstruct creator, const Rtti* parent) noexcept;
				~Rtti() = default;

				InterfacePtr create() const throw(std::bad_alloc);

				const Rtti* getParent() const noexcept;

				const std::string& type_name() const noexcept;

				bool isDerivedFrom(const Rtti* other) const noexcept;
				bool isDerivedFrom(const Rtti& other) const noexcept;
				bool isDerivedFrom(const char* const name) const noexcept;
				bool isDerivedFrom(const std::string& name) const noexcept;

			private:
				std::string name_;
				const Rtti* parent_;
				RttiConstruct construct_;
			};

			class DLL_EXPORT Interface : public std::enable_shared_from_this<Interface>
			{
				OctoonDeclareClass(Interface)
			public:
				Interface() = default;
				virtual ~Interface() = default;

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
}

#endif