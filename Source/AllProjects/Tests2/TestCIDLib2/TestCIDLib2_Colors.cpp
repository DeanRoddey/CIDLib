//
// FILE NAME: TestCIDLib2_Colors.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/27/2007
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file contains tests related to the color oriented classes.
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
#include    "TestCIDLib2.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_Color1,TTestFWTest)
RTTIDecls(TTest_Color2,TTestFWTest)


// ---------------------------------------------------------------------------
//  CLASS: TTest_Color1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Color1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Color1::TTest_Color1() :

    TTestFWTest
    (
        L"Color Parsing", L"Tests the parsing of color values from text", 3
    )
{
}

TTest_Color1::~TTest_Color1()
{
}


// ---------------------------------------------------------------------------
//  TTest_Color1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Color1::eRunTest( TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    struct TTestData
    {
        public :
            TTestData(  const   tCIDLib::TCh* const pszData
                        , const tCIDLib::TCh        chSepChar
                        , const tCIDLib::TBoolean   bShoudFail
                        , const tCIDLib::ERadices   eRadix
                        , const TRGBClr&            rgbResult) :

                m_bShouldFail(bShoudFail)
                , m_chSepChar(chSepChar)
                , m_eRadix(eRadix)
                , m_pszData(pszData)
                , m_rgbResult(rgbResult)
            {
            }

            tCIDLib::TBoolean   m_bShouldFail;
            tCIDLib::TCh        m_chSepChar;
            tCIDLib::ERadices   m_eRadix;
            const tCIDLib::TCh* m_pszData;
            TRGBClr             m_rgbResult;
    };

    const TTestData aTestData[] =
    {
          TTestData(L"1,1,1,1", L',', kCIDLib::False, tCIDLib::ERadices::Auto, TRGBClr(1,1,1,1))
        , TTestData(L"255,255,255", L',', kCIDLib::False, tCIDLib::ERadices::Auto, TRGBClr(0xFF,0xFF,0xFF))
        , TTestData(L"0xFF,0xFF,0xFF", L',', kCIDLib::False, tCIDLib::ERadices::Auto, TRGBClr(0xFF,0xFF,0xFF))
        , TTestData(L"0xFF,0xFF,0xFF", L',', kCIDLib::False, tCIDLib::ERadices::Auto, TRGBClr(0xFF,0xFF,0xFF))
        , TTestData(L"FF/FF/FF", L'/', kCIDLib::False, tCIDLib::ERadices::Hex, TRGBClr(0xFF,0xFF,0xFF))

          // These should fail
        , TTestData(L"-1,-1,1,1", L',', kCIDLib::True, tCIDLib::ERadices::Auto, TRGBClr())
        , TTestData(L"0x100,0xFF,0xFF", L',', kCIDLib::True, tCIDLib::ERadices::Auto, TRGBClr())
    };
    const tCIDLib::TCard4 c4Count = tCIDLib::c4ArrayElems(aTestData);

    TRGBClr rgbTmp;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TTestData& Data = aTestData[c4Index];
        if (!rgbTmp.bParseFromText(Data.m_pszData, Data.m_eRadix, Data.m_chSepChar))
        {
            if (!Data.m_bShouldFail)
            {
                eRes = tTestFWLib::ETestRes::Failed;
                strmOut << TFWCurLn << L"Failed to parse test color "
                        << c4Index << kCIDLib::DNewLn;
            }
            continue;
        }

        if (Data.m_bShouldFail)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Test color " << c4Index
                    << L" should have failed to parse\n\n";
            continue;
        }

        if (rgbTmp != Data.m_rgbResult)
        {
            eRes = tTestFWLib::ETestRes::Failed;
            strmOut << TFWCurLn << L"Test color " << c4Index
                    << L" was not equal to expected value\n\n";
        }
    }
    return eRes;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_Color2
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_Color2: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_Color2::TTest_Color2() :

    TTestFWTest
    (
        L"Color Parsing", L"Tests color byte layout", 2
    )
{
}

TTest_Color2::~TTest_Color2()
{
}


// ---------------------------------------------------------------------------
//  TTest_Color2: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_Color2::eRunTest( TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Set up a test RGB color
    TRGBClr rgbTest(0x1, 0x2, 0x3);

    // Get it out as a coded color
    tCIDLib::TCard4 c4Clr = rgbTest.c4Color();

    //
    //  Look at that as a coded RGB color structure. This includes alpha, but that's ok,
    //  we don't use it here.
    //
    tCIDLib::TCodedRGB* prgbTest = reinterpret_cast<tCIDLib::TCodedRGB*>(&c4Clr);

    // And test that the colors match. If not, we have an endian problem
    if ((rgbTest.c1Red() != prgbTest->c1Red)
    ||  (rgbTest.c1Green() != prgbTest->c1Green)
    ||  (rgbTest.c1Blue() != prgbTest->c1Blue))
    {
        eRes = tTestFWLib::ETestRes::Failed;
        strmOut << TFWCurLn << L"Endian problem with TCodedRGB structure\n\n";
    }
    return eRes;
}
