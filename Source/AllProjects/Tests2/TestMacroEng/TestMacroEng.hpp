//
// FILE NAME: TestMacroEng.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/25/2007
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
//  This is the main header file of the CIDLib macro engine test app. This is
//  a standard CIDLib test framework test app. All of our tests as fairly
//  small so they are all in a single _Tests file.
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
#include    "CIDMacroEng.hpp"
#include    "TestFWLib.hpp"


// ---------------------------------------------------------------------------
//  CLASS: TTest_PathValidation
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_PathValidation : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_PathValidation();

        ~TTest_PathValidation();


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
        RTTIDefs(TTest_PathValidation,TTestFWTest)
};




// ---------------------------------------------------------------------------
//  CLASS: TTest_NumConstProbe
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_NumConstProbe : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_NumConstProbe();

        ~TTest_NumConstProbe();


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
        RTTIDefs(TTest_NumConstProbe,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_CMLRuntime
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_CMLRuntime : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_CMLRuntime();

        ~TTest_CMLRuntime();


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
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTestMacro
        (
                    TTextStringOutStream&   strmOut
            , const TString&                strClassPath
            , const TString&                strParms
        );


        // -------------------------------------------------------------------
        //  Private data members
        // -------------------------------------------------------------------
        TMEngFixedBaseClassMgr      m_mecmTest;
        TMEngFixedBaseFileResolver  m_mefrTest;
        TMEngStrmErrHandler         m_meehEngine;
        TMEngStrmPrsErrHandler      m_meehParser;
        TMacroEngParser             m_meprsTest;
        TTextStringOutStream        m_strmConsole;

        // This guy has to be last so it destructs last
        TCIDMacroEngine             m_meTest;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_CMLRuntime,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TMacroEngTestApp
// PREFIX: tfwapp
//
//  This is our implementation of the test framework's test program framework.
//  We just create a derivative and override some methods.
// ---------------------------------------------------------------------------
class TMacroEngTestApp : public TTestFWApp
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TMacroEngTestApp();

        TMacroEngTestApp(const TMacroEngTestApp&) = delete;

        ~TMacroEngTestApp();


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
        RTTIDefs(TMacroEngTestApp,TTestFWApp)
};


