//
// FILE NAME: TestCIDLib2.hpp
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
//  This is the main header file of the CIDLib facility test app. This is
//  a standard CIDLib test framework test app.
//
//  Note that the test framework itself uses the CIDLib facility. So there
//  is a separate, standalone test that tests the most basic bits of CIDLib
//  that the test framework uses. Here we are just testing the other parts
//  that can be tested via the test framework itself.
//
//  That's why these files are named TestCIDLib2... because the low level
//  test for CIDLib has already taken the name we'd otherwise have used.
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
#include    "CIDLib.hpp"
#include    "TestFWLib.hpp"


// ---------------------------------------------------------------------------
//  Include our own internally used headers
// ---------------------------------------------------------------------------
#include    "TestCIDLib2_BinStreams.hpp"
#include    "TestCIDLib2_TextStreams.hpp"
#include    "TestCIDLib2_IDLGen.hpp"



// ---------------------------------------------------------------------------
//  CLASS: TTest_CircularBuf
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_CircularBuf : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_CircularBuf();

        ~TTest_CircularBuf();


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
        RTTIDefs(TTest_CircularBuf,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_CoordCtor
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_CoordCtor : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_CoordCtor();

        ~TTest_CoordCtor();


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
        RTTIDefs(TTest_CoordCtor,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_CoordAcc
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_CoordAcc : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_CoordAcc();

        ~TTest_CoordAcc();


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
        RTTIDefs(TTest_CoordAcc,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_CoordOps
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_CoordOps : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_CoordOps();

        ~TTest_CoordOps();


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
        RTTIDefs(TTest_CoordOps,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_Area1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_Area1 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_Area1();

        ~TTest_Area1();


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
        RTTIDefs(TTest_Area1,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_Area2
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_Area2 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_Area2();

        ~TTest_Area2();


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
        RTTIDefs(TTest_Area2,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_Area3
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_Area3 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_Area3();

        ~TTest_Area3();


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
        RTTIDefs(TTest_Area3,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_Base641
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_Base641 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_Base641();

        ~TTest_Base641();


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
        tCIDLib::TBoolean bDoExpTest
        (
                    TTextOutStream&         strmOut
            , const tCIDLib::TCh* const     pszTestName
            , const tCIDLib::TCard1* const  pc1Input
            , const tCIDLib::TCard4         c4InCount
            , const tCIDLib::TCard1* const  pc1Expected
            , const tCIDLib::TCard4         c4ExpCount
            , const tCIDLib::TCard4         c4LineWidth
        );

        tCIDLib::TBoolean bDoExpTest
        (
                    TTextOutStream&         strmOut
            , const tCIDLib::TCh* const     pszTestName
            , const tCIDLib::TSCh* const    pchInput
            , const tCIDLib::TCard4         c4InCount
            , const tCIDLib::TSCh* const    pchExpected
            , const tCIDLib::TCard4         c4ExpCount
            , const tCIDLib::TCard4         c4LineWidth
        );


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_Base641,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_BitSeq1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_BitSeq1 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_BitSeq1();

        ~TTest_BitSeq1();


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
        RTTIDefs(TTest_BitSeq1,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_BitSeq2
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_BitSeq2 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_BitSeq2();

        ~TTest_BitSeq2();


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
        RTTIDefs(TTest_BitSeq2,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_BitSeq3
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_BitSeq3 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_BitSeq3();

        ~TTest_BitSeq3();


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
        RTTIDefs(TTest_BitSeq3,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_Bitset1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_Bitset1 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_Bitset1();

        ~TTest_Bitset1();


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
        RTTIDefs(TTest_Bitset1,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_ColAlgo1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_ColAlgo1 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_ColAlgo1();

        ~TTest_ColAlgo1();


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
        RTTIDefs(TTest_ColAlgo1,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_ColCursors
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_ColCursors : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_ColCursors();

        ~TTest_ColCursors();


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
        RTTIDefs(TTest_ColCursors,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_Color1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_Color1 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_Color1();

        ~TTest_Color1();


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
        RTTIDefs(TTest_Color1,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_Color2
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_Color2 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_Color2();

        ~TTest_Color2();


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
        RTTIDefs(TTest_Color2,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_CntPtr1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_CntPtr1 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_CntPtr1();

        ~TTest_CntPtr1();


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
        //  Private, virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid TestConstDef
        (
            const   TCntPtr<TString>&       cptrTest
            ,       tCIDLib::TCard4&        c4ErrCnt
        );


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_CntPtr1,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_CntPtr2
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_CntPtr2 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_CntPtr2();

        ~TTest_CntPtr2();


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
        tCIDLib::EExitCodes eTestThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colThreads
        //      A list of threads we start up to do the testing. They are all started
        //      on eTestThread.
        //
        //  m_cptrTest
        //      A counted pointer that the threads will make and destroy copies of
        //      ultimately leaving the ref count at 0.
        // -------------------------------------------------------------------
        TRefVector<TThread>     m_colThreads;
        TCntPtr<TCardinal>      m_cptrTest;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_CntPtr2,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_DequeMoveSem
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_DequeMoveSem : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_DequeMoveSem();

        ~TTest_DequeMoveSem();


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
        RTTIDefs(TTest_DequeMoveSem,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_DequePlace
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_DequePlace : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_DequePlace();

        ~TTest_DequePlace();


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
        RTTIDefs(TTest_DequePlace,TTestFWTest)
};




// ---------------------------------------------------------------------------
//  CLASS: TTest_Enum1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_Enum1 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_Enum1();

        ~TTest_Enum1();


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
        RTTIDefs(TTest_Enum1,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_EnumForEach
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_EnumForEach : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_EnumForEach();

        ~TTest_EnumForEach();


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
        RTTIDefs(TTest_EnumForEach,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_FixedSizePool
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_FixedSizePool : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_FixedSizePool();

        ~TTest_FixedSizePool();


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
        RTTIDefs(TTest_FixedSizePool,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_FundVectorLambda
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_FundVectorLambda : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_FundVectorLambda();

        ~TTest_FundVectorLambda();


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
        RTTIDefs(TTest_FundVectorLambda, TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_GenCache1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_GenCache1 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_GenCache1();

        ~TTest_GenCache1();


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
        RTTIDefs(TTest_GenCache1,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_GenCache2
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_GenCache2 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_GenCache2();

        ~TTest_GenCache2();


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
        RTTIDefs(TTest_GenCache2,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_GenCache3
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_GenCache3 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_GenCache3();

        ~TTest_GenCache3();


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
        RTTIDefs(TTest_GenCache3,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_DirChange1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_DirChange1 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_DirChange1();

        ~TTest_DirChange1();


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
        RTTIDefs(TTest_DirChange1,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_FlagJanitor
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_FlagJanitor : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_FlagJanitor();

        ~TTest_FlagJanitor();


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
        RTTIDefs(TTest_FlagJanitor,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_HashSetPlace
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_HashSetPlace : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_HashSetPlace();

        ~TTest_HashSetPlace();


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
        RTTIDefs(TTest_HashSetPlace,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_LambdaJan
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_LambdaJan : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_LambdaJan();

        ~TTest_LambdaJan();


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
        RTTIDefs(TTest_LambdaJan,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_LogLimiter
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_LogLimiter : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_LogLimiter();

        ~TTest_LogLimiter();


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
        RTTIDefs(TTest_LogLimiter,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_MemBufMove
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_MemBufMove : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_MemBufMove();

        ~TTest_MemBufMove();


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
        RTTIDefs(TTest_MemBufMove,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_MemBufExp
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_MemBufExp : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_MemBufExp();

        ~TTest_MemBufExp();


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
        RTTIDefs(TTest_MemBufExp,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_Numeric1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_Numeric1 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_Numeric1();

        ~TTest_Numeric1();


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
        RTTIDefs(TTest_Numeric1,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_Numeric2
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_Numeric2 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_Numeric2();

        ~TTest_Numeric2();


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
        RTTIDefs(TTest_Numeric2,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_ObjArrayLambda
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_ObjArrayLambda : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_ObjArrayLambda();

        ~TTest_ObjArrayLambda();


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
        RTTIDefs(TTest_ObjArrayLambda,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_ObjArrayMoveSem
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_ObjArrayMoveSem : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_ObjArrayMoveSem();

        ~TTest_ObjArrayMoveSem();


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
        RTTIDefs(TTest_ObjArrayMoveSem,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_UniquePtr
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_UniquePtr : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_UniquePtr();

        ~TTest_UniquePtr();


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
        RTTIDefs(TTest_UniquePtr,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_PubSub
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_PubSub1 : public TTestFWTest, private MPubSubscription
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_PubSub1();

        ~TTest_PubSub1();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        )   override;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ProcessPubMsg
        (
            const   TPubSubMsg&             psmsgIn
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strLastMsg
        //  m_strLastTopic
        //      We just drop the incoming message and topic id here for the main method
        //      to check.
        // -------------------------------------------------------------------
        TString     m_strLastMsg;
        TString     m_strLastTopic;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_PubSub1,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_PubSubVector
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_PubSubVector : public TTestFWTest, private MPubSubscription
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_PubSubVector();

        ~TTest_PubSubVector();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tTestFWLib::ETestRes eRunTest
        (
                    TTextStringOutStream&   strmOutput
            ,       tCIDLib::TBoolean&      bWarning
        )   override;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ProcessPubMsg
        (
            const   TPubSubMsg&             psmsgIn
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4MsgCount
        //      We bump this for each msg received, to help us make sure we are getting
        //      what we expect.
        //
        //  m_colpsiLast
        //  m_strLastTopic
        //      We just drop the incoming message and topic id here for the main method
        //      to check.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4MsgCount;
        TColPubSubInfo  m_colpsiLast;
        TString         m_strLastTopic;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bTestPubMsg
        (
                    TTextStringOutStream&   strmOutput
            , const TColPubSubInfo::EEvents eEvent
            , const tCIDLib::TCard4         c4Index1
            , const tCIDLib::TCard4         c4Index2
            ,       tCIDLib::TCard4&        c4Count
        );


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_PubSubVector,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_RawBits
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_RawBits : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_RawBits();

        ~TTest_RawBits();


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
        RTTIDefs(TTest_RawBits, TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_RefVectorBasic
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_RefVectorBasic : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_RefVectorBasic();

        ~TTest_RefVectorBasic();


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
        RTTIDefs(TTest_RefVectorBasic,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_RefVectorLambda
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_RefVectorLambda : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_RefVectorLambda();

        ~TTest_RefVectorLambda();


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
        RTTIDefs(TTest_RefVectorLambda,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_RefVectorMoveItem
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_RefVectorMoveItem : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_RefVectorMoveItem();

        ~TTest_RefVectorMoveItem();


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
        RTTIDefs(TTest_RefVectorMoveItem,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_SafeCnt1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_SafeCnt1 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_SafeCnt1();

        ~TTest_SafeCnt1();


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
        RTTIDefs(TTest_SafeCnt1,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_SafeCnt2
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_SafeCnt2 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_SafeCnt2();

        ~TTest_SafeCnt2();


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
        tCIDLib::EExitCodes eTestThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colThreads
        //      A list of threads we start up to do the testing. They are all started
        //      on eTestThread.
        //
        //  m_scntCard
        //  m_scntInt
        //      Counters for the threads to inc/dec. Ultimately they should end up back
        //      at zero.
        // -------------------------------------------------------------------
        TRefVector<TThread>     m_colThreads;
        TSafeCard4Counter       m_scntCard;
        TSafeInt4Counter        m_scntInt;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_SafeCnt2,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_SimplePoolPtr
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_SimplePoolPtr : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_SimplePoolPtr();

        ~TTest_SimplePoolPtr();


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
        RTTIDefs(TTest_SimplePoolPtr,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_String1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_String1 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_String1();

        ~TTest_String1();


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
        RTTIDefs(TTest_String1,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_String2
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_String2 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_String2();

        ~TTest_String2();


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
        RTTIDefs(TTest_String2,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_String3
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_String3 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_String3();

        ~TTest_String3();


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
        RTTIDefs(TTest_String3,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_StringMove
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_StringMove : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_StringMove();

        ~TTest_StringMove();


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
        RTTIDefs(TTest_StringMove,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_StringCat
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_StringCat : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_StringCat();

        ~TTest_StringCat();


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
        RTTIDefs(TTest_StringCat,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_Time1
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_Time1 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_Time1();

        ~TTest_Time1();


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
        RTTIDefs(TTest_Time1,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_Time2
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_Time2 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_Time2();

        ~TTest_Time2();


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
        RTTIDefs(TTest_Time2,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_Time3
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_Time3 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_Time3();

        ~TTest_Time3();


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
        tCIDLib::TVoid TestParse
        (
                    TTextStringOutStream&   strmOut
            ,       tTestFWLib::ETestRes&   eRes
            , const TTFWCurLn&              twclAt
            , const tCIDLib::TCh* const     pszSrc
            , const TTime::TCompList&       fcolComps
            , const tCIDLib::TCh            chDateSep
            , const tCIDLib::TCh            chTimeSep
            , const tCIDLib::TCh            chTZSep
            , const TTime&                  tmExpected
            , const tCIDLib::TBoolean       bShouldPass
        );


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTest_Time3,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_Time4
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_Time4 : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_Time4();

        ~TTest_Time4();


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
        RTTIDefs(TTest_Time4,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_Types
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_Types  : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_Types();

        ~TTest_Types();


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
        RTTIDefs(TTest_Types,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_SimplePool
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_SimplePool : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_SimplePool();

        ~TTest_SimplePool();


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
        RTTIDefs(TTest_SimplePool,TTestFWTest)
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_VectorBasic
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_VectorBasic : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_VectorBasic();

        ~TTest_VectorBasic();


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
        RTTIDefs(TTest_VectorBasic,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_VectorLambda
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_VectorLambda : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_VectorLambda();

        ~TTest_VectorLambda();


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
        RTTIDefs(TTest_VectorLambda,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_VectorMoveSem
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_VectorMoveSem : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_VectorMoveSem();

        ~TTest_VectorMoveSem();


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
        RTTIDefs(TTest_VectorMoveSem,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TTest_VectorPlace
// PREFIX: tfwt
// ---------------------------------------------------------------------------
class TTest_VectorPlace : public TTestFWTest
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TTest_VectorPlace();

        ~TTest_VectorPlace();


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
        RTTIDefs(TTest_VectorPlace,TTestFWTest)
};


// ---------------------------------------------------------------------------
//  CLASS: TCIDLibTest
// PREFIX: tfwapp
//
//  This is our implementation of the test framework's test program framework.
//  We just create a derivative and override some methods.
// ---------------------------------------------------------------------------
class TCIDLibTestApp : public TTestFWApp
{
    public  :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TCIDLibTestApp();

        TCIDLibTestApp(const TCIDLibTestApp&) = delete;

        ~TCIDLibTestApp();


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
        RTTIDefs(TCIDLibTestApp,TTestFWApp)
};


