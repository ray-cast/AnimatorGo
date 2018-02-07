#ifndef OCTOON_RTTI_MACROS_H_
#define OCTOON_RTTI_MACROS_H_

#include <octoon/runtime/platform.h>

namespace octoon
{
	namespace runtime
	{
		namespace rtti
		{
#define OctoonDeclareInterface(Base)\
public:\
	static PROJECT runtime::rtti::Rtti RTTI;\
	static PROJECT runtime::rtti::Rtti* getRtti() noexcept;\
    virtual PROJECT runtime::rtti::Rtti* rtti() const noexcept;\
	virtual const char* typePROJECT() noexcept;\
private:

#define OctoonImplementInterface(Base, Name) \
    PROJECT runtime::rtti::Rtti Base::RTTI = PROJECT runtime::rtti::Rtti(Name, nullptr, nullptr);\
	PROJECT runtime::rtti::Rtti* Base::rtti() const noexcept { return &RTTI; }\
	static PROJECT runtime::rtti::Rtti* Base::getRtti() noexcept { return &RTTI; }\
	const char* Base::typePROJECT() noexcept { return Name; };

#define OctoonDeclareSubInterface(Derived, Base)\
public:\
	static PROJECT runtime::rtti::Rtti RTTI;\
	static PROJECT runtime::rtti::Rtti* getRtti() noexcept;\
    virtual PROJECT runtime::rtti::Rtti* rtti() const noexcept;\
	virtual const char* typePROJECT() noexcept;\
private:

#define OctoonImplementSubInterface(Derived, Base, Name) \
    PROJECT runtime::rtti::Rtti Derived::RTTI = PROJECT runtime::rtti::Rtti(Name, nullptr, Base::getRtti());\
	PROJECT runtime::rtti::Rtti* Derived::rtti() const noexcept { return &RTTI; }\
	PROJECT runtime::rtti::Rtti* Derived::getRtti() noexcept { return &RTTI; }\
	const char* Derived::typePROJECT() noexcept { return Name; };

#define OctoonDeclareClass(Base) \
public:\
	static PROJECT runtime::rtti::Rtti RTTI;\
	static PROJECT runtime::rtti::Rtti* getRtti() noexcept;\
	static PROJECT runtime::rtti::Interface* FactoryCreate(); \
    virtual PROJECT runtime::rtti::Rtti* rtti() const noexcept;\
	virtual const char* typePROJECT() noexcept;\
private:

#define OctoonImplementClass(Base, Name) \
    PROJECT runtime::rtti::Rtti Base::RTTI = PROJECT runtime::rtti::Rtti(Name, Base::FactoryCreate, nullptr);\
	PROJECT runtime::rtti::Rtti* Base::rtti() const noexcept { return &RTTI; }\
	PROJECT runtime::rtti::Rtti* Base::getRtti() noexcept { return &RTTI; }\
	PROJECT runtime::rtti::Interface* Base::FactoryCreate() { return new Base; }\
	const char* Base::typePROJECT() noexcept { return Name; };

#define OctoonDeclareSubClass(Derived, Base) \
public:\
    static PROJECT runtime::rtti::Rtti RTTI;\
	static PROJECT runtime::rtti::Rtti* getRtti() noexcept;\
	static PROJECT runtime::rtti::Interface* FactoryCreate(); \
    virtual PROJECT runtime::rtti::Rtti* rtti() const noexcept;\
	virtual const char* typePROJECT() noexcept;\
private:

#define OctoonImplementSubClass(Derived, Base, Name) \
    PROJECT runtime::rtti::Rtti Derived::RTTI = PROJECT runtime::rtti::Rtti(Name, Derived::FactoryCreate, Base::getRtti());\
	PROJECT runtime::rtti::Rtti* Derived::rtti() const noexcept { return &RTTI; }\
	PROJECT runtime::rtti::Interface* Derived::FactoryCreate() { return new Derived; }\
	PROJECT runtime::rtti::Rtti* Derived::getRtti() noexcept { return &RTTI; }\
	const char* Derived::typePROJECT() noexcept { return Name; };
		}
	}
}

#endif