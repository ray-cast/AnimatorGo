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
	static OCTOON_PROJECT runtime::Rtti* get_rtti() noexcept;\
    virtual OCTOON_PROJECT runtime::Rtti* rtti() const noexcept;\
	virtual const char* type_name() noexcept;\
private:

#define OctoonImplementInterface(Base, Name) \
    OCTOON_PROJECT runtime::Rtti Base::RTTI = OCTOON_PROJECT runtime::Rtti(Name, nullptr, nullptr);\
	OCTOON_PROJECT runtime::Rtti* Base::rtti() const noexcept { return &RTTI; }\
	static OCTOON_PROJECT runtime::Rtti* Base::get_rtti() noexcept { return &RTTI; }\
	const char* Base::type_name() noexcept { return Name; };

#define OctoonDeclareSubInterface(Derived, Base)\
public:\
	static OCTOON_PROJECT runtime::Rtti RTTI;\
	static OCTOON_PROJECT runtime::Rtti* get_rtti() noexcept;\
    virtual OCTOON_PROJECT runtime::Rtti* rtti() const noexcept;\
	virtual const char* type_name() noexcept;\
private:

#define OctoonImplementSubInterface(Derived, Base, Name) \
    OCTOON_PROJECT runtime::Rtti Derived::RTTI = OCTOON_PROJECT runtime::Rtti(Name, nullptr, Base::get_rtti());\
	OCTOON_PROJECT runtime::Rtti* Derived::rtti() const noexcept { return &RTTI; }\
	OCTOON_PROJECT runtime::Rtti* Derived::get_rtti() noexcept { return &RTTI; }\
	const char* Derived::type_name() noexcept { return Name; };

#define OctoonDeclareClass(Base) \
public:\
	static OCTOON_PROJECT runtime::Rtti RTTI;\
	static OCTOON_PROJECT runtime::Rtti* get_rtti() noexcept;\
	static OCTOON_PROJECT runtime::RttiInterface* FactoryCreate(); \
    virtual OCTOON_PROJECT runtime::Rtti* rtti() const noexcept;\
	virtual const char* type_name() noexcept;\
private:

#define OctoonImplementClass(Base, Name) \
    OCTOON_PROJECT runtime::Rtti Base::RTTI = OCTOON_PROJECT runtime::Rtti(Name, Base::FactoryCreate, nullptr);\
	OCTOON_PROJECT runtime::Rtti* Base::rtti() const noexcept { return &RTTI; }\
	OCTOON_PROJECT runtime::Rtti* Base::get_rtti() noexcept { return &RTTI; }\
	OCTOON_PROJECT runtime::RttiInterface* Base::FactoryCreate() { return new Base; }\
	const char* Base::type_name() noexcept { return Name; };

#define OctoonDeclareSubClass(Derived, Base) \
public:\
    static OCTOON_PROJECT runtime::Rtti RTTI;\
	static OCTOON_PROJECT runtime::Rtti* get_rtti() noexcept;\
	static OCTOON_PROJECT runtime::RttiInterface* FactoryCreate(); \
    virtual OCTOON_PROJECT runtime::Rtti* rtti() const noexcept;\
	virtual const char* type_name() noexcept;\
private:

#define OctoonImplementSubClass(Derived, Base, Name) \
    OCTOON_PROJECT runtime::Rtti Derived::RTTI = OCTOON_PROJECT runtime::Rtti(Name, Derived::FactoryCreate, Base::get_rtti());\
	OCTOON_PROJECT runtime::Rtti* Derived::rtti() const noexcept { return &RTTI; }\
	OCTOON_PROJECT runtime::RttiInterface* Derived::FactoryCreate() { return new Derived; }\
	OCTOON_PROJECT runtime::Rtti* Derived::get_rtti() noexcept { return &RTTI; }\
	const char* Derived::type_name() noexcept { return Name; };
	}
}

#endif