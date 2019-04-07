//
// FILE NAME: TestMData_AttrData.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/07/2018
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
//  These are tests of the attribute data classes.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Include our public header and our own specific header
// ---------------------------------------------------------------------------
#include    "TestCIDMData.hpp"
#include    "TestCIDMData_AttrData.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_AttrDataBasic, TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_AttrDataBasic
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_AttrDataBasic: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_AttrDataBasic::TTest_AttrDataBasic() :

    TTestFWTest
    (
        L"Attr Data 1", L"Basic tests of the attribute data class", 4
    )
{
}

TTest_AttrDataBasic::~TTest_AttrDataBasic()
{
}


// ---------------------------------------------------------------------------
//  TTest_AttrDataBasic: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_AttrDataBasic::eRunTest(TTextStringOutStream& strmOut, tCIDLib::TBoolean& bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;
    tCIDLib::TBoolean bRes;

    // Test a default constructed one for initial state
    {
        TAttrData adatDef;
        bRes = bTestState3
        (
            strmOut
            , L"def ctor"
            , CID_LINE
            , adatDef
            , TString::strEmpty()
            , TString::strEmpty()
            , TString::strEmpty()
            , tCIDMData::EAttrTypes::String
            , tCIDMData::EAttrEdTypes::None
            , TString::strEmpty()
            , 0
            , TString::strEmpty()
        );
        if (!bRes)
            eRes = tTestFWLib::ETestRes::Failed;
    }

    // Test setting and reading back each value type
    {
        TAttrData adatType;

        //
        //  Do boolean type
        //
        adatType.Set
        (
            L"Bool Test"
            , L"/Test/Boolean"
            , TString::strEmpty()
            , tCIDMData::EAttrTypes::Bool
        );
        bRes = bTestState
        (
            strmOut
            , L"Bool Test"
            , CID_LINE
            , adatType
            , TString::strEmpty()
            , tCIDMData::EAttrTypes::Bool
            , L"False"
        );
        if (!bRes)
            eRes = tTestFWLib::ETestRes::Failed;

        adatType.SetBool(kCIDLib::True);
        if (adatType.bVal() != kCIDLib::True)
        {
            strmOut << TFWCurLn << L"Did not get back set true value" << kCIDLib::NewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }


        //
        //  Do Card type
        //
        adatType.Set
        (
            L"Card Test"
            , L"/Test/Card"
            , TString::strEmpty()
            , tCIDMData::EAttrTypes::Card
        );
        bRes = bTestState
        (
            strmOut
            , L"Card Test"
            , CID_LINE
            , adatType
            , TString::strEmpty()
            , tCIDMData::EAttrTypes::Card
            , L"0"
        );
        if (!bRes)
            eRes = tTestFWLib::ETestRes::Failed;

        adatType.SetCard(11);
        if (adatType.c4Val() != 11)
        {
            strmOut << TFWCurLn << L"Did not get back set 11 value" << kCIDLib::NewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }
    }

    return eRes;
}


// ---------------------------------------------------------------------------
//  TTest_AttrDataBasic: Private, non-virtual methods
// ---------------------------------------------------------------------------

// Avoid some redundancy by providing a test for basic state of an attribute
tCIDLib::TBoolean
TTest_AttrDataBasic::bTestState(        TTextOutStream&         strmOut
                                , const TString&                strTestName
                                , const tCIDLib::TCard4         c4Line
                                , const TAttrData&              adatTest
                                , const TString&                strLimits
                                , const tCIDMData::EAttrTypes   eType
                                , const TString&                strFmtValue)
{
    tCIDLib::TBoolean bRet = kCIDLib::True;

    if (eType != adatTest.eType())
    {
        strmOut << TTFWCurLn(CID_FILE, c4Line)
                << strTestName << L" should have type "
                << tCIDMData::strXlatEAttrTypes(eType)
                << L" but has "
                << tCIDMData::strXlatEAttrTypes(adatTest.eType())
                << kCIDLib::NewLn;
        bRet = kCIDLib::False;
    }

    if (strLimits != adatTest.strLimits())
    {
        strmOut << TTFWCurLn(CID_FILE, c4Line)
                << strTestName << L" should have limit '"
                << strLimits
                << L"' but has '"
                << adatTest.strLimits()
                << L"'\n";
        bRet = kCIDLib::False;
    }

    // Format the value to test
    TString strTestVal;
    adatTest.FormatToText(strTestVal);
    if (strTestVal != strFmtValue)
    {
        strmOut << TTFWCurLn(CID_FILE, c4Line)
                << strTestName << L" should have value '"
                << strFmtValue
                << L"' but has '"
                << strTestVal
                << L"'\n";
        bRet = kCIDLib::False;
    }

    return bRet;
}


tCIDLib::TBoolean
TTest_AttrDataBasic::bTestState2(       TTextOutStream&         strmOut
                                , const TString&                strTestName
                                , const tCIDLib::TCard4         c4Line
                                , const TAttrData&              adatTest
                                , const TString&                strAttrId
                                , const TString&                strSpecType
                                , const TString&                strLimits
                                , const tCIDMData::EAttrTypes   eType
                                , const tCIDMData::EAttrEdTypes eEditType
                                , const TString&                strFmtValue)
{
    // Call the other version first
    tCIDLib::TBoolean bRet = bTestState
    (
        strmOut
        , strTestName
        , c4Line
        , adatTest
        , strLimits
        , eType
        , strFmtValue
    );

    if (strAttrId != adatTest.strId())
    {
        strmOut << TTFWCurLn(CID_FILE, c4Line)
                << strTestName << L" should have id '"
                << strAttrId
                << L"' but has '"
                << adatTest.strId()
                << L"'\n";
        bRet = kCIDLib::False;
    }

    if (strSpecType != adatTest.strSpecType())
    {
        strmOut << TTFWCurLn(CID_FILE, c4Line)
                << strTestName << L" should have special type '"
                << strSpecType
                << L"' but has '"
                << adatTest.strSpecType()
                << L"'\n";
        bRet = kCIDLib::False;
    }

    if (eEditType != adatTest.eEditType())
    {
        strmOut << TTFWCurLn(CID_FILE, c4Line)
                << strTestName << L" should have edit type "
                << tCIDMData::strXlatEAttrEdTypes(eEditType)
                << L" but has "
                << tCIDMData::strXlatEAttrEdTypes(adatTest.eEditType())
                << kCIDLib::NewLn;
        bRet = kCIDLib::False;
    }

    return bRet;
}


tCIDLib::TBoolean
TTest_AttrDataBasic::bTestState3(       TTextOutStream&         strmOut
                                , const TString&                strTestName
                                , const tCIDLib::TCard4         c4Line
                                , const TAttrData&              adatTest
                                , const TString&                strAttrId
                                , const TString&                strSpecType
                                , const TString&                strLimits
                                , const tCIDMData::EAttrTypes   eType
                                , const tCIDMData::EAttrEdTypes eEditType
                                , const TString&                strFmtValue
                                , const tCIDLib::TCard8         c8UserData
                                , const TString&                strUserData)
{
    // Call the other version first
    tCIDLib::TBoolean bRet = bTestState2
    (
        strmOut
        , strTestName
        , c4Line
        , adatTest
        , strAttrId
        , strSpecType
        , strLimits
        , eType
        , eEditType
        , strFmtValue
    );

    // And do our extra bits
    if (c8UserData != adatTest.c8User())
    {
        strmOut << TTFWCurLn(CID_FILE, c4Line)
                << strTestName << L" should have TCard8 user data "
                << c8UserData
                << L" but has "
                << adatTest.c8User()
                << kCIDLib::NewLn;
        bRet = kCIDLib::False;
    }

    if (strUserData != adatTest.strUserData())
    {
        strmOut << TTFWCurLn(CID_FILE, c4Line)
                << strTestName << L" should have string user data '"
                << strUserData
                << L"' but has '"
                << adatTest.strUserData()
                << L"'\n";
        bRet = kCIDLib::False;
    }

    return bRet;
}
