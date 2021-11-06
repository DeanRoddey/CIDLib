//
// FILE NAME: TestCIDMData_ChunkedFile.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/24/2007
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
//  The header for the chunked file tests.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  CLASS: TTest_ChunkedBasic
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_ChunkedBasic : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_ChunkedBasic();

        ~TTest_ChunkedBasic();


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
        tCIDLib::TBoolean bTestState
        (
                    TTextOutStream&         strmOut
            , const TChunkedFile&           chflTest
            , const TString&                strName
            , const tCIDLib::TCard4         c4Line
            , const tCIDLib::TCard4         c4SerialNum
            , const tCIDLib::TBoolean       bChanges
            , const tCIDLib::TCard4         c4MetaKeys
            , const tCIDLib::TCard4         c4Bytes
            , const tCIDLib::TEncodedTime   enctMinLast
        );


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_ChunkedBasic, TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_ChunkedExt
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_ChunkedExt : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_ChunkedExt();

        ~TTest_ChunkedExt();


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
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_ChunkedExt, TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_ChunkedMeta
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_ChunkedMeta : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_ChunkedMeta();

        ~TTest_ChunkedMeta();


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
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_ChunkedMeta, TTestFWTest)
};
