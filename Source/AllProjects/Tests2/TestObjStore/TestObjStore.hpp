//
// FILE NAME: TestObjStore.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/27/2007
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
//  This is the main header file of the CIDLib object store test app. This is
//  a standard CIDLib test framework test app.
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
#include    "CIDMath.hpp"
#include    "CIDObjStore.hpp"
#include    "TestFWLib.hpp"


// ---------------------------------------------------------------------------
//  CLASS: TTest_Basic1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_Basic1 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_Basic1();

        ~TTest_Basic1();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_Basic1,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_Basic2
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_Basic2 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_Basic2();

        ~TTest_Basic2();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_Basic2,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_Basic3
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_Basic3 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_Basic3();

        ~TTest_Basic3();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_Basic3,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_Basic4
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_Basic4 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_Basic4();

        ~TTest_Basic4();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_Basic4, TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_Basic5
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_Basic5 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_Basic5();

        ~TTest_Basic5();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_Basic5, TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TObjStTest
// PREFIX: tfwapp
//
//  This is our implementation of the test framework's test program framework.
//  We just create a derivative and override some methods.
// ---------------------------------------------------------------------------
class TObjStTestApp : public TTestFWApp
{
    public  :
        // ---------------------------------------------------------------------------
        //  Pubic, static data
        // ---------------------------------------------------------------------------
        static const TString strKey1;
        static const TString strKey2;
        static const TString strKey3;
        static const TString strKey4;
        static const TString strKey5;


        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TObjStTestApp();

        TObjStTestApp(const TObjStTestApp&) = delete;

        ~TObjStTestApp();


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
        RTTIDefs(TObjStTestApp,TTestFWApp)
};


