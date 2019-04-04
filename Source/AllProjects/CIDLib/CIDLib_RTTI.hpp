//
// FILE NAME: CIDLib_RTTI.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/15/1996
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file implements some standard templates and macros that are
//  used by the custom RTTI support.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  These are the standard macros that go into classes to endow them with
//  RTTI info.
// ---------------------------------------------------------------------------
#define RTTIDefs(Class,ParentClass) \
private : \
using TParent = ParentClass; \
public  :   \
static const TClass& clsThis(); \
tCIDLib::TBoolean bIsDescendantOf(const TClass& clsTarget) const \
{ \
    if (clsTarget == clsThis()) \
        return kCIDLib::True; \
     else \
        return ParentClass::bIsDescendantOf(clsTarget); \
} \
\
const TClass& clsIsA() const \
{ \
    return clsThis(); \
} \
\
const TClass& clsParent() const \
{ \
    return ParentClass::clsThis(); \
}



#define TemplateRTTIDefs(Class,ParentClass) \
private : \
using TParent = ParentClass; \
public  : \
static const TClass& clsThis() \
{ \
    static const TClass* pclsThis = 0; \
    if (!pclsThis) \
    { \
        TBaseLock lockInit; \
        pclsThis = new TClass(CIDLib_MakeLStr2(Class)); \
    } \
    return *pclsThis; \
} \
\
tCIDLib::TBoolean bIsDescendantOf(const TClass& clsTarget) const \
{ \
    if (clsTarget == clsThis()) \
        return kCIDLib::True; \
     else \
        return ParentClass::bIsDescendantOf(clsTarget); \
} \
\
const TClass& clsIsA() const \
{ \
    return clsThis(); \
} \
\
const TClass& clsParent() const \
{ \
    return ParentClass::clsThis(); \
}



// ---------------------------------------------------------------------------
//  A simple macro for creating a type factory and registering it with the
//  type registry, using a simple global object.
// ---------------------------------------------------------------------------
#define RTTIDecls(Class,ParentClass) \
const TClass& Class::clsThis() \
{ \
    static const TClass* pclsThis = 0; \
    if (!pclsThis) \
    { \
        TBaseLock lockInit; \
        pclsThis = new TClass(CIDLib_MakeLStr2(Class)); \
    } \
    return *pclsThis; \
}

#define AdvRTTIDecls(Class,ParentClass) \
[[nodiscard]] TObject* pMakeNew_##Class() {return new Class;} \
static const TTypeFactoryKicker Class##Kicker(CIDLib_MakeLStr2(Class), pMakeNew_##Class); \
const TClass& Class::clsThis() \
{ \
    static const TClass* pclsThis = 0; \
    if (!pclsThis) \
    { \
        TBaseLock lockInit; \
        pclsThis = new TClass(CIDLib_MakeLStr2(Class)); \
    } \
    return *pclsThis; \
}


// ---------------------------------------------------------------------------
//  If you alias one class for another, but still want to be able to create
//  the original class dynamically via the aliased name, this one provides
//  that. The AdvRTTIDecls() macro for the original must be done first, and
//  be visible to this macro.
// ---------------------------------------------------------------------------
#define AdvRTTIAlias(Class,Alias) \
static const TTypeFactoryKicker Alias##Kicker(CIDLib_MakeLStr2(Alias), pMakeNew_##Class);



// ---------------------------------------------------------------------------
//  If you want to hide your default constructor use this macro. Dynamic
//  typing requires that the default constructor be available to the factory
//  function, so this macro will make it a friend of your class. That way
//  you can hide it but still be polymorphically streamable and dynamically
//  creatable.
// ---------------------------------------------------------------------------
#define BefriendFactory(Class) \
friend TObject* pMakeNew_##Class();
