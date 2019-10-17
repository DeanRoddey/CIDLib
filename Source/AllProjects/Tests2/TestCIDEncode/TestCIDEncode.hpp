//
// FILE NAME: TestCIDEncode.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/15/2008
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
//  This is the main header file of the CIDEncode tests. We just declare
//  all of the tests here.
//
//  Note that there are fundamental tests in the low level TestCIDlib program
//  that tests the simplest stuff (required by the Test Framework itself),
//  that test out basic in/out transcoding stuff. So we kind of pick up from
//  there.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// -----------------------------------------------------------------------------
//  Include underlying headers
// -----------------------------------------------------------------------------
#include    "CIDEncode.hpp"
#include    "TestFWLib.hpp"


// ---------------------------------------------------------------------------
//  CLASS: TTest_ByName
// PREFIX: tfwt
//
//  Tests creation of some converters by their name, making sure case doesn't matter
// ---------------------------------------------------------------------------
class TTest_ByName : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_ByName();

        TTest_ByName(const TTest_ByName&) = delete;

        ~TTest_ByName();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_ByName,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_Convert
// PREFIX: tfwt
//
//  Tests the error modes that tell the transcoders how to react if the src
//  has invalid data or the target cannot support the data to externalize.
// ---------------------------------------------------------------------------
class TTest_Convert : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_Convert();

        ~TTest_Convert();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bTestSrc
        (
                    TTextOutStream&         strmOut
            ,       TTextConverter&         tcvtTest
            , const tCIDLib::TCard1* const  pc1Data
            , const tCIDLib::TCard4         c4InCount
            , const TString&                strExpRes
        );

        tCIDLib::TBoolean bTestTar
        (
                    TTextOutStream&         strmOut
            ,       TTextConverter&         tcvtTest
            , const TString&                strData
            , const tCIDLib::TCard4         c4ExpBytes
            , const tCIDLib::TCard1* const  pc1ExpRes
        );


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_Convert,TTestFWTest)
};




// ---------------------------------------------------------------------------
//  CLASS: TTest_ErrModes
// PREFIX: tfwt
//
//  Tests the error modes that tell the transcoders how to react if the src
//  has invalid data or the target cannot support the data to externalize.
// ---------------------------------------------------------------------------
class TTest_ErrModes : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_ErrModes();

        ~TTest_ErrModes();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bTestBadSrc
        (
                    TTextOutStream&         strmOut
            ,       TTextConverter&         tcvtTest
            , const tCIDLib::TCard1* const  pc1Data
            , const tCIDLib::TCard4         c4Count
            , const tCIDLib::TCard4         c4ErrIndex
            , const TString&                strRepResult
            , const tCIDLib::TCh            chRepChar
            , const tCIDLib::TCard4         c4ErrBytes
        );

        tCIDLib::TBoolean bTestBadTar
        (
                    TTextOutStream&         strmOut
            ,       TTextConverter&         tcvtTest
            , const TString&                strData
            , const tCIDLib::TCard4         c4ErrIndex
            , const tCIDLib::TCard4         c4OutBytes
        );


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_ErrModes,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_RoundTrip1
// PREFIX: tfwt
//
//  Tests round tripping of text from source to Unicode back to source to see
//  if we get back the same stuff. This first one handles the the 'mode 1'
//  encoding derivatives, all of which derive from a common base class.
// ---------------------------------------------------------------------------
class TTest_RoundTrip1 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_RoundTrip1();

        ~TTest_RoundTrip1();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_RoundTrip1,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TEncodeTestApp
// PREFIX: tfwapp
//
//  This is our implementation of the test framework's test program framework.
//  We just create a derivative and override some methods.
// ---------------------------------------------------------------------------
class TEncodeTestApp : public TTestFWApp
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TEncodeTestApp();

        TEncodeTestApp(const TEncodeTestApp&) = delete;

        ~TEncodeTestApp();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bInitialize
        (
                    TString&                strErr
        )   override;

        tCIDLib::TVoid LoadTests() override;

        tCIDLib::TVoid PostTest
        (
            const   TTestFWTest&            tfwtFinished
        )   override;

        tCIDLib::TVoid PreTest
        (
            const   TTestFWTest&            tfwtStarting
        )   override;

        tCIDLib::TVoid Terminate() override;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TEncodeTestApp,TTestFWApp)
};



