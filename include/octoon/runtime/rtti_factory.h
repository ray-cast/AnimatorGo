#ifndef OCTOON_RTTI_FACTORY_H_
#define OCTOON_RTTI_FACTORY_H_

#include <map>

#include <octoon/runtime/rtti.h>
#include <octoon/runtime/singleton.h>

namespace octoon
{
	namespace runtime
	{
		namespace rtti
		{
			class DLL_EXPORT Factory final : public Singleton<Factory>
			{
			public:
				Factory() = default;
				~Factory() = default;

				bool open() noexcept;

				bool add(Rtti* rtti) noexcept;

				Rtti* getRTTI(const char* name) noexcept;
				Rtti* getRTTI(const std::string& name) noexcept;

				const Rtti* getRTTI(const char* name) const noexcept;
				const Rtti* getRTTI(const std::string& name) const noexcept;

				InterfacePtr createObject(const char*, const Rtti& base) const except;
				InterfacePtr createObject(const std::string& name, const Rtti& base) const except;

				template<typename T>
				std::shared_ptr<T> make_shared(const std::string& name)
				{
					return std::dynamic_pointer_cast<T>(this->createObject(name, T::RTTI));
				}

				template<typename T>
				std::shared_ptr<T> make_shared(const char* name)
				{
					return std::dynamic_pointer_cast<T>(this->createObject(name, T::RTTI));
				}

				template<typename T>
				std::unique_ptr<T> make_unique(const std::string& name)
				{
					return std::dynamic_pointer_cast<T>(this->createObject(name, T::RTTI));
				}

				template<typename T>
				std::unique_ptr<T> make_unique(const char* name)
				{
					return std::dynamic_pointer_cast<T>(this->createObject(name, T::RTTI));
				}

			private:
				std::vector<Rtti*> rttis_;
				std::map<std::string, Rtti*> rttiLists_;
			};

			template<typename T>
			std::shared_ptr<T> make_shared(const std::string& name)
			{
				return Factory::instance()->make_shared<T>(name);
			}

			template<typename T>
			std::shared_ptr<T> make_shared(const char* name)
			{
				return Factory::instance()->make_shared<T>(name);
			}

			template<typename T>
			std::unique_ptr<T> make_unique(const std::string& name)
			{
				return Factory::instance()->make_unique<T>(name);
			}

			template<typename T>
			std::unique_ptr<T> make_unique(const char* name)
			{
				return Factory::instance()->make_unique<T>(name);
			}
		}
	}
}

#endif