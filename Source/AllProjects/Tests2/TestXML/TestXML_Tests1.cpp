//
// FILE NAME: TestXML_Tests.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/05/2008
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
//  This file contains the first set of tests, just for the most fundamental
//  stuff.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Include underlying headers
// ---------------------------------------------------------------------------
#include    "TestXML.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_Attr,TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_Attr
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Parse: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Attr::TTest_Attr() :

    TTestFWTest
    (
        L"XML Attributes", L"Tests the XML attribute classes", 3
    )
    , m_strBaseName(L"Test")
    , m_strPrefix(L"TestPref")
    , m_strURI(L"uri:TestAttrURI")
    , m_strValue(L"TheTestValue")
{
}

TTest_Attr::~TTest_Attr()
{
}


// ---------------------------------------------------------------------------
//  TTest_Attr: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Attr::eRunTest(TTextStringOutStream&  strmOut
                    , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Test the default ctor
    {
        TXMLAttr xattrTest;
        if (xattrTest.bExplicit() || (xattrTest.eType() != tCIDXML::EAttrTypes::Count))
        {
            strmOut << L"Default ctor created incorrect default values\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }
    }

    // Test another of the ctors
    {
        TXMLAttr xattrTest2
        (
            m_strBaseName.pszBuffer()
            , m_strPrefix
            , m_strURI
            , m_strValue
            , tCIDXML::EAttrTypes::CData
            , kCIDLib::True
        );

        if (!bCheckContents(xattrTest2))
        {
            strmOut << L"Ctor 2 created incorrect values\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }
    }

    // Test another of the ctors
    {
        TXMLAttr xattrTest3
        (
            m_strBaseName
            , m_strPrefix
            , m_strURI
            , m_strValue
            , tCIDXML::EAttrTypes::CData
            , kCIDLib::True
        );

        if (!bCheckContents(xattrTest3))
        {
            strmOut << L"Ctor 3 created incorrect values\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }
    }

    // Test the two Set methods
    {
        TXMLAttr xattrTest;
        xattrTest.Set
        (
            m_strBaseName.pszBuffer()
            , m_strPrefix
            , m_strURI
            , m_strValue
            , tCIDXML::EAttrTypes::CData
            , kCIDLib::True
        );

        if (!bCheckContents(xattrTest))
        {
            strmOut << L"Set 1 created incorrect values\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }
    }

    {
        TXMLAttr xattrTest;
        xattrTest.Set
        (
            m_strBaseName
            , m_strPrefix
            , m_strURI
            , m_strValue
            , tCIDXML::EAttrTypes::CData
            , kCIDLib::True
        );

        if (!bCheckContents(xattrTest))
        {
            strmOut << L"Set 2 created incorrect values\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }
    }

    // Test round trip through copy/assign
    {
        TXMLAttr xattrTest
        (
            L"MyName"
            , L"MyPref"
            , L"uri:MyURIX"
            , L"MyValue"
            , tCIDXML::EAttrTypes::Entity
            , kCIDLib::False
        );

        TXMLAttr xattrTest2(xattrTest);
        TXMLAttr xattrTest3;
        xattrTest3 = xattrTest2;

        if (xattrTest != xattrTest3)
        {
            strmOut << L"Copy/Assign round trip failed\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }
    }
    return eRes;
}


// ---------------------------------------------------------------------------
//  TTest_Attr: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TTest_Attr::bCheckContents(const TXMLAttr& xattrTest)
{
    if (!xattrTest.bExplicit()
    ||  (xattrTest.eType() != tCIDXML::EAttrTypes::CData)
    ||  (xattrTest.strBaseName() != L"Test")
    ||  (xattrTest.strPrefix() != m_strPrefix)
    ||  (xattrTest.strURI() != m_strURI)
    ||  (xattrTest.strValue() != m_strValue)
    ||  (xattrTest.strQName() != L"TestPref:Test"))
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}

