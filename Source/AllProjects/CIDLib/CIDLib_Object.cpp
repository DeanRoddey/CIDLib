//
// FILE NAME: CIDLib_Object.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/20/1993
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
//  This file implements the TObject class, which is the fundamental
//  clas of the system, from which everything else is derived.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDLib_.hpp"



// ---------------------------------------------------------------------------
//  CLASS: TObject
// PREFIX: obj
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TObject: Public, static methods
// ---------------------------------------------------------------------------
const TClass& TObject::clsThis()
{
    static const TClass* pclsThis = 0;
    if (!pclsThis)
    {
        TBaseLock lockInit;
        if (!pclsThis)
        {
            TRawMem::pExchangePtr<const TClass>
            (
                &pclsThis, new TClass(L"TObject")
            );
        }
    }
    return *pclsThis;
}


// ---------------------------------------------------------------------------
//  TObject: Constructors and Destructor
// ---------------------------------------------------------------------------
TObject::~TObject()
{
}


// ---------------------------------------------------------------------------
//  TObject: Public, virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TObject::bIsDescendantOf(const TClass& clsTarget) const
{
    //
    //  This is the end of the line, so either its equal to our class or
    //  we are not a descendant of the passed clsas.
    //
    return (clsTarget == clsThis());
}

const TClass& TObject::clsIsA() const
{
    return clsThis();
}

const TClass& TObject::clsParent() const
{
    return TClass::Nul_TClass();
}


// ---------------------------------------------------------------------------
//  TObject: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TObject::bIsA(const TClass& clsTest) const
{
    return (clsTest == clsIsA());
}


// ---------------------------------------------------------------------------
//  TObject: Hidden constructors and operators
// ---------------------------------------------------------------------------
TObject::TObject()
{
}

TObject::TObject(const TObject&)
{
}


