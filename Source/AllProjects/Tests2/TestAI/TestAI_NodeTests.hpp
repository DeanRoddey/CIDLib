//
// FILE NAME: TestAI_NodeTests.hpp
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
//  This is the header for the file that implements the node oriented tests.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  CLASS: TTest_BTInverterTest1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_BTInverterTest1 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_BTInverterTest1();

        ~TTest_BTInverterTest1();


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
        RTTIDefs(TTest_BTInverterTest1,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_BTVarTest1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_BTVarTest1 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_BTVarTest1();

        ~TTest_BTVarTest1();


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
        RTTIDefs(TTest_BTVarTest1,TTestFWTest)
};
