//
// FILE NAME: TestCIDLib2_BinStreams.hpp
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
//  getting too stuffed. Since streams are very much a polymorphic hierarchy
//  we have a base test class from which the various derived classes descend
//  The base class provides common tests for them.
//
//  In some cases the stuff testsed in the base class could just be done once
//  because that stuff is done purely in the base in/out stream classes and so
//  common to them all. But it's not worth bothering, and there's always some
//  possibility that it could show up an issue in some way that wouldn't
//  otherwise come to light.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  CLASS: TTest_BaseBinStream
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_BaseBinStream : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        ~TTest_BaseBinStream();


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TTest_BaseBinStream
        (
            const   TString&                strName
            , const TString&                strDescr
        );


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunBaseTests
        (
                    TBinInStream&           strmInTest
            ,       TBinOutStream&          strmOutTest
            ,       TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunPushbackTests
        (
                    TBinInStream&           strmInTest
            ,       TBinOutStream&          strmOutTest
            ,       TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_BaseBinStream,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_ChunkedBinStream
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_ChunkedBinStream : public TTest_BaseBinStream
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_ChunkedBinStream();

        ~TTest_ChunkedBinStream();


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
        tTestFWLib::ETestRes eRunOutTests
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_ChunkedBinStream,TTest_BaseBinStream)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_MBufBinStream
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_MBufBinStream : public TTest_BaseBinStream
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_MBufBinStream();

        ~TTest_MBufBinStream();


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
        tTestFWLib::ETestRes eRunOutTests
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        );


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_MBufBinStream,TTest_BaseBinStream)
};
