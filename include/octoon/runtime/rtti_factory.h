#ifndef OCTOON_RTTI_FACTORY_H_
#define OCTOON_RTTI_FACTORY_H_

#include <map>

#include <octoon/runtime/rtti.h>
#include <octoon/runtime/object.h>
#include <octoon/runtime/singleton.h>

namespace octoon
{
	class OCTOON_EXPORT RttiFactory final
	{
		OctoonDeclareSingleton(RttiFactory)
	public:
		RttiFactory() = default;
		~RttiFactory() = default;

		void open() noexcept;
		void add(Rtti* rtti) noexcept;

		Rtti* getRtti(std::string_view name) noexcept;
		const Rtti* getRtti(std::string_view name) const noexcept;

		std::shared_ptr<Object> createInstance(const char*, const Rtti& base) const noexcept(false);
		std::shared_ptr<Object> createInstance(std::string_view name, const Rtti& base) const noexcept(false);

		template<typename T, typename = std::enable_if_t<std::is_base_of<Object, T>::value>>
		bool add()
		{
			return this->add(T::getRtti());
		}

		template<typename T>
		std::shared_ptr<T> make_shared(std::string_view name)
		{
			return std::dynamic_pointer_cast<T>(this->createInstance(name, T::RTTI));
		}

		template<typename T>
		std::unique_ptr<T> make_unique(std::string_view name)
		{
			return std::dynamic_pointer_cast<T>(this->createInstance(name, T::RTTI));
		}

	private:
		std::vector<Rtti*> caches_;
		std::map<std::string, Rtti*, std::less<>> types_;
	};

	template<typename T, typename = std::enable_if_t<std::is_base_of<Object, T>::value>>
	inline std::shared_ptr<T> instantiate(const T* self)
	{
		assert(self);
		return std::dynamic_pointer_cast<T>(self->rtti()->create());
	}

	template<typename T, typename = std::enable_if_t<std::is_base_of<Object, T>::value>>
	inline std::shared_ptr<T> instantiate(const T& self)
	{
		return instantiate(&self);
	}
}

#endif