//
// FILE NAME: TestAI_ParseTests.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/25/2016
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main header file of the CIDLib AI engine test app. This is
//  a standard CIDLib test framework test app.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  CLASS: TTest_BTParse1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_BTParse1 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_BTParse1();

        ~TTest_BTParse1();


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
        RTTIDefs(TTest_BTParse1,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_BTParse2
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_BTParse2 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_BTParse2();

        ~TTest_BTParse2();


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
        RTTIDefs(TTest_BTParse2,TTestFWTest)
};

