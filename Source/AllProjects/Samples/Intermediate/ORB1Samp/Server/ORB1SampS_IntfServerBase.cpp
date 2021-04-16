// ----------------------------------------------------------------------------
//  FILE: ORB1SampS_IntfServerBase.cpp
//  DATE: Fri, Feb 12 21:10:40 2021 -0500
//    ID: E1CCC9CBCFA1FDAF-29A8F7CB04763265
//
//  This file was generated by the Charmed Quark CIDIDL compiler. Do not make
//  changes by hand, because they will be lost if the file is regenerated.
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "ORB1SampS.hpp"

// ----------------------------------------------------------------------------
// Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TORB1SampServerBase,TOrbServerBase)


// ----------------------------------------------------------------------------
// Public, static data
// ----------------------------------------------------------------------------
const TString TORB1SampServerBase::strInterfaceId(L"E1CCC9CBCFA1FDAF-29A8F7CB04763265");
const TString TORB1SampServerBase::strBinding(L"/ORB1Samp/IntfImpl");


// ----------------------------------------------------------------------------
// TORB1SampServerBase: Destructor
// ----------------------------------------------------------------------------
TORB1SampServerBase::~TORB1SampServerBase()
{
}

// ----------------------------------------------------------------------------
// TORB1SampServerBase: Protected Constructors
// ----------------------------------------------------------------------------
TORB1SampServerBase::TORB1SampServerBase() :
    TOrbServerBase(strInterfaceId, L"TORB1SampClientProxy" )
{
}

TORB1SampServerBase::TORB1SampServerBase(const TOrbObjId& ooidThis) :
    TOrbServerBase(ooidThis)
{
}

// ----------------------------------------------------------------------------
// TORB1SampServerBase: Public, non-virtual methods
// ----------------------------------------------------------------------------
tCIDLib::TVoid TORB1SampServerBase::Dispatch(const TString& strMethodName, TOrbCmd& orbcToDispatch)
{
    if (strMethodName == L"bQueryByName")
    {
        TString strToFind;
        orbcToDispatch.strmIn() >> strToFind;
        TORB1SampRec recFound;
        tCIDLib::TBoolean retVal = bQueryByName
        (
            strToFind
          , recFound
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
        orbcToDispatch.strmOut() << recFound;
    }
     else if (strMethodName == L"bQueryAllStatus")
    {
        tORB1SampSh::EStatus eToFind;
        orbcToDispatch.strmIn() >> eToFind;
        TVector<TORB1SampRec> colFound;
        tCIDLib::TBoolean retVal = bQueryAllStatus
        (
            eToFind
          , colFound
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
        orbcToDispatch.strmOut() << colFound;
    }
     else
    {
         TParent::Dispatch(strMethodName, orbcToDispatch);
    }
}

