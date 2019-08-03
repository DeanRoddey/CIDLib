//
// FILE NAME: TestCIDLib2_TestStreams.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/03/2016
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
//  Unlike most of the other files, this one has its own header because
//  there's a number of tests and it just keeps the main header from
//  getting too stuffed. This file does text stream oriented tests.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  CLASS: TTest_BaseTextStream
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_BaseTextStream : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        ~TTest_BaseTextStream();


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TTest_BaseTextStream
        (
            const   TString&                strName
            , const TString&                strDescr
        );

        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunBaseTests
        (
                    TTextInStream&          strmTestIn
            ,       TTextOutStream&         strmTestOut
            ,       TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_BaseTextStream,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_ChunkedTextStream
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_ChunkedTextStream : public TTest_BaseTextStream
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_ChunkedTextStream();

        ~TTest_ChunkedTextStream();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        )   final;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunOutTests
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_ChunkedTextStream,TTest_BaseTextStream)
};


