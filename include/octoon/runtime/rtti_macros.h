#ifndef OCTOON_RTTI_MACROS_H_
#define OCTOON_RTTI_MACROS_H_

#include <octoon/runtime/platform.h>

namespace octoon
{
	namespace runtime
	{
#define OctoonDeclareInterface(Base)\
public:\
	static OCTOON_PROJECT runtime::Rtti RTTI;\
	static OCTOON_PROJECT runtime::Rtti* getRtti() noexcept;\
    virtual OCTOON_PROJECT runtime::Rtti* rtti() const noexcept;\
	virtual const char* type_name() noexcept;\
private:

#define OctoonImplementInterface(Base, Name) \
    OCTOON_PROJECT runtime::Rtti Base::RTTI = OCTOON_PROJECT runtime::Rtti(Name, nullptr, nullptr);\
	OCTOON_PROJECT runtime::Rtti* Base::rtti() const noexcept { return &RTTI; }\
	static OCTOON_PROJECT runtime::Rtti* Base::getRtti() noexcept { return &RTTI; }\
	const char* Base::type_name() noexcept { return Name; };

#define OctoonDeclareSubInterface(Derived, Base)\
public:\
	static OCTOON_PROJECT runtime::Rtti RTTI;\
	static OCTOON_PROJECT runtime::Rtti* getRtti() noexcept;\
    virtual OCTOON_PROJECT runtime::Rtti* rtti() const noexcept;\
	virtual const char* type_name() noexcept;\
private:

#define OctoonImplementSubInterface(Derived, Base, Name) \
    OCTOON_PROJECT runtime::Rtti Derived::RTTI = OCTOON_PROJECT runtime::Rtti(Name, nullptr, Base::getRtti());\
	OCTOON_PROJECT runtime::Rtti* Derived::rtti() const noexcept { return &RTTI; }\
	OCTOON_PROJECT runtime::Rtti* Derived::getRtti() noexcept { return &RTTI; }\
	const char* Derived::type_name() noexcept { return Name; };

#define OctoonDeclareClass(Base) \
public:\
	static OCTOON_PROJECT runtime::Rtti RTTI;\
	static OCTOON_PROJECT runtime::Rtti* getRtti() noexcept;\
	static OCTOON_PROJECT runtime::RttiInterface* FactoryCreate(); \
    virtual OCTOON_PROJECT runtime::Rtti* rtti() const noexcept;\
	virtual const char* type_name() noexcept;\
private:

#define OctoonImplementClass(Base, Name) \
    OCTOON_PROJECT runtime::Rtti Base::RTTI = OCTOON_PROJECT runtime::Rtti(Name, Base::FactoryCreate, nullptr);\
	OCTOON_PROJECT runtime::Rtti* Base::rtti() const noexcept { return &RTTI; }\
	OCTOON_PROJECT runtime::Rtti* Base::getRtti() noexcept { return &RTTI; }\
	OCTOON_PROJECT runtime::RttiInterface* Base::FactoryCreate() { return new Base; }\
	const char* Base::type_name() noexcept { return Name; };

#define OctoonDeclareSubClass(Derived, Base) \
public:\
    static OCTOON_PROJECT runtime::Rtti RTTI;\
	static OCTOON_PROJECT runtime::Rtti* getRtti() noexcept;\
	static OCTOON_PROJECT runtime::RttiInterface* FactoryCreate(); \
    virtual OCTOON_PROJECT runtime::Rtti* rtti() const noexcept;\
	virtual const char* type_name() noexcept;\
private:

#define OctoonImplementSubClass(Derived, Base, Name) \
    OCTOON_PROJECT runtime::Rtti Derived::RTTI = OCTOON_PROJECT runtime::Rtti(Name, Derived::FactoryCreate, Base::getRtti());\
	OCTOON_PROJECT runtime::Rtti* Derived::rtti() const noexcept { return &RTTI; }\
	OCTOON_PROJECT runtime::RttiInterface* Derived::FactoryCreate() { return new Derived; }\
	OCTOON_PROJECT runtime::Rtti* Derived::getRtti() noexcept { return &RTTI; }\
	const char* Derived::type_name() noexcept { return Name; };
	}
}

#endif