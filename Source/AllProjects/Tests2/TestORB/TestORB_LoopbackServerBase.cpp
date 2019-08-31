// ----------------------------------------------------------------------------
//  FILE: TestORB_LoopbackServerBase.cpp
//  DATE: Sat, Aug 31 17:05:56 2019 -0400
//    ID: 1799C1BE4E47BA7E-19344A2250A462D2
//
//  This file was generated by the Charmed Quark CIDIDL compiler. Do not make
//  changes by hand, because they will be lost if the file is regenerated.
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "TestORB.hpp"

// ----------------------------------------------------------------------------
// Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TTestOrbIntfServerBase,TOrbServerBase)


// ----------------------------------------------------------------------------
// Public, static data
// ----------------------------------------------------------------------------
const TString TTestOrbIntfServerBase::strInterfaceId(L"1799C1BE4E47BA7E-19344A2250A462D2");
// ----------------------------------------------------------------------------
// TTestOrbIntfServerBase: Destructor
// ----------------------------------------------------------------------------
TTestOrbIntfServerBase::~TTestOrbIntfServerBase()
{
}

// ----------------------------------------------------------------------------
// TTestOrbIntfServerBase: Protected Constructors
// ----------------------------------------------------------------------------
TTestOrbIntfServerBase::TTestOrbIntfServerBase() :
    TOrbServerBase(strInterfaceId, L"TTestOrbIntfClientProxy" )
{
}

TTestOrbIntfServerBase::TTestOrbIntfServerBase(const TOrbObjId& ooidThis) :
    TOrbServerBase(ooidThis)
{
}

// ----------------------------------------------------------------------------
// TTestOrbIntfServerBase: Public, non-virtual methods
// ----------------------------------------------------------------------------
tCIDLib::TVoid TTestOrbIntfServerBase::Dispatch(const TString& strMethodName, TOrbCmd& orbcToDispatch)
{
    if (strMethodName == L"bBagParamTest")
    {
        TBag<TArea> colIn;
        orbcToDispatch.strmIn() >> colIn;
        TBag<TPoint> colOut;
        TString strFailReason;
        tCIDLib::TBoolean retVal = bBagParamTest
        (
            colIn
          , colOut
          , strFailReason
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
        orbcToDispatch.strmOut() << colOut;
        orbcToDispatch.strmOut() << strFailReason;
    }
     else if (strMethodName == L"bParamTest")
    {
        tCIDLib::TCard4 c4In;
        orbcToDispatch.strmIn() >> c4In;
        tCIDLib::TCard4 c4Out;
        tCIDLib::TFloat8 f8In;
        orbcToDispatch.strmIn() >> f8In;
        tCIDLib::TFloat8 f8Out;
        TString strIn;
        orbcToDispatch.strmIn() >> strIn;
        TString strOut;
        TString strInOut;
        orbcToDispatch.strmIn() >> strInOut;
        tCIDLib::TBoolean bIn;
        orbcToDispatch.strmIn() >> bIn;
        tCIDLib::TBoolean bOut;
        TString strFailReason;
        tCIDLib::TBoolean retVal = bParamTest
        (
            c4In
          , c4Out
          , f8In
          , f8Out
          , strIn
          , strOut
          , strInOut
          , bIn
          , bOut
          , strFailReason
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
        orbcToDispatch.strmOut() << c4Out;
        orbcToDispatch.strmOut() << f8Out;
        orbcToDispatch.strmOut() << strOut;
        orbcToDispatch.strmOut() << strInOut;
        orbcToDispatch.strmOut() << bOut;
        orbcToDispatch.strmOut() << strFailReason;
    }
     else if (strMethodName == L"bPollData")
    {
        tCIDLib::TCard4 c4SerialNum;
        orbcToDispatch.strmIn() >> c4SerialNum;
        TArea areaRet;
        tCIDLib::TBoolean retVal = bPollData
        (
            c4SerialNum
          , areaRet
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
        if (retVal)
        {
            orbcToDispatch.strmOut() << c4SerialNum;
            orbcToDispatch.strmOut() << areaRet;
        }
    }
     else if (strMethodName == L"bVectorParamTest")
    {
        TVector<TString> colInOut;
        orbcToDispatch.strmIn() >> colInOut;
        TString strFailReason;
        tCIDLib::TBoolean retVal = bVectorParamTest
        (
            colInOut
          , strFailReason
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
        orbcToDispatch.strmOut() << colInOut;
        orbcToDispatch.strmOut() << strFailReason;
    }
     else if (strMethodName == L"c4BumpIt")
    {
        tCIDLib::TCard4 c4Input;
        orbcToDispatch.strmIn() >> c4Input;
        tCIDLib::TCard4 retVal = c4BumpIt
        (
            c4Input
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
    }
     else if (strMethodName == L"BulkTest")
    {
        tCIDLib::TCard4 c4BufSz_mbufOut;
        orbcToDispatch.strmIn() >> c4BufSz_mbufOut;
        THeapBuf mbufOut(c4BufSz_mbufOut + 1);
        orbcToDispatch.strmIn().c4ReadBuffer(mbufOut, c4BufSz_mbufOut);
        BulkTest
        (
            c4BufSz_mbufOut
          , mbufOut
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << c4BufSz_mbufOut;
        orbcToDispatch.strmOut().c4WriteBuffer(mbufOut, c4BufSz_mbufOut);
    }
     else if (strMethodName == L"ExceptTest")
    {
        ExceptTest
        (
        );
        orbcToDispatch.strmOut().Reset();
    }
     else if (strMethodName == L"ExceptTest1")
    {
        tCIDLib::TCard4 c4Dummy;
        orbcToDispatch.strmIn() >> c4Dummy;
        ExceptTest
        (
            c4Dummy
        );
        orbcToDispatch.strmOut().Reset();
    }
     else if (strMethodName == L"SetData")
    {
        TArea areaToSet;
        orbcToDispatch.strmIn() >> areaToSet;
        SetData
        (
            areaToSet
        );
        orbcToDispatch.strmOut().Reset();
    }
     else
    {
         TParent::Dispatch(strMethodName, orbcToDispatch);
    }
}

