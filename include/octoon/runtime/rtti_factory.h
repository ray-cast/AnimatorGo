#ifndef OCTOON_RTTI_FACTORY_H_
#define OCTOON_RTTI_FACTORY_H_

#include <map>

#include <octoon/runtime/rtti.h>
#include <octoon/runtime/rtti_interface.h>
#include <octoon/runtime/singleton.h>

namespace octoon
{
	namespace runtime
	{
		class OCTOON_EXPORT RttiFactory final : public Singleton<RttiFactory>
		{
		public:
			RttiFactory() = default;
			~RttiFactory() = default;

			bool open() noexcept;

			bool add(Rtti* rtti) noexcept;

			Rtti* get_rtti(const char* name) noexcept;
			Rtti* get_rtti(const std::string& name) noexcept;

			const Rtti* get_rtti(const char* name) const noexcept;
			const Rtti* get_rtti(const std::string& name) const noexcept;

			RttiInterfacePtr create_object(const char*, const Rtti& base) const except;
			RttiInterfacePtr create_object(const std::string& name, const Rtti& base) const except;

			template<typename T>
			std::shared_ptr<T> make_shared(const std::string& name)
			{
				return std::dynamic_pointer_cast<T>(this->create_object(name, T::RTTI));
			}

			template<typename T>
			std::shared_ptr<T> make_shared(const char* name)
			{
				return std::dynamic_pointer_cast<T>(this->create_object(name, T::RTTI));
			}

			template<typename T>
			std::unique_ptr<T> make_unique(const std::string& name)
			{
				return std::dynamic_pointer_cast<T>(this->create_object(name, T::RTTI));
			}

			template<typename T>
			std::unique_ptr<T> make_unique(const char* name)
			{
				return std::dynamic_pointer_cast<T>(this->create_object(name, T::RTTI));
			}

		private:
			std::vector<Rtti*> rttis_;
			std::map<std::string, Rtti*> rttiLists_;
		};

		namespace rtti
		{
			template<typename T>
			inline std::shared_ptr<T> make_shared(const std::string& name)
			{
				return RttiFactory::instance()->make_shared<T>(name);
			}

			template<typename T>
			inline std::shared_ptr<T> make_shared(const char* name)
			{
				return RttiFactory::instance()->make_shared<T>(name);
			}

			template<typename T>
			inline std::unique_ptr<T> make_unique(const std::string& name)
			{
				return RttiFactory::instance()->make_unique<T>(name);
			}

			template<typename T>
			inline std::unique_ptr<T> make_unique(const char* name)
			{
				return RttiFactory::instance()->make_unique<T>(name);
			}

			template<typename T, typename = std::enable_if_t<std::is_base_of_v<RttiInterface, T>>>
			inline std::shared_ptr<T> instantiate(const T* self)
			{
				assert(self);
				return std::dynamic_pointer_cast(self->rtti()->create());
			}

			template<typename T, typename = std::enable_if_t<std::is_base_of_v<RttiInterface, T>>>
			inline std::shared_ptr<T> instantiate(const T& self)
			{
				return instantiate(&self);
			}
		}
	}
}

#endif