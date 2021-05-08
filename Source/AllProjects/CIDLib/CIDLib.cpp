//
// FILE NAME: CIDLib.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/23/1996
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
//  This is the main file of the facility. It provides an init function that
//  is called by the magic main module macro that users must include in their
//  main program file.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDLib_.hpp"



// ---------------------------------------------------------------------------
//  Do some RTTI macros for classes that are all inline (i.e. they don't
//  have a Cpp file to do it in.)
// ---------------------------------------------------------------------------
RTTIDecls(TCriticalSection,TObject)
RTTIDecls(TCritSecLocker,TObject)
RTTIDecls(TSafeCard4Counter,TObject)
RTTIDecls(TSafeInt4Counter,TObject)


// ---------------------------------------------------------------------------
//  For those collections types that we pre-instantiate we have to force their
//  implementations here. This will save much overhead throughout the system.
// ---------------------------------------------------------------------------
template class CIDLIBEXP TBasicTreeCol<TKeyValuePair>;

template class CIDLIBEXP TFundArray<tCIDLib::TBoolean>;
template class CIDLIBEXP TFundArray<tCIDLib::TCard4>;
template class CIDLIBEXP TFundArray<tCIDLib::TFloat8>;
template class CIDLIBEXP TFundArray<tCIDLib::TInt4>;

template class CIDLIBEXP TFundVector<tCIDLib::TBoolean>;
template class CIDLIBEXP TFundVector<tCIDLib::TCard1>;
template class CIDLIBEXP TFundVector<tCIDLib::TCard4>;
template class CIDLIBEXP TFundVector<tCIDLib::TCard8>;
template class CIDLIBEXP TFundVector<tCIDLib::TFloat4>;
template class CIDLIBEXP TFundVector<tCIDLib::TFloat8>;

template class CIDLIBEXP TFundStack<tCIDLib::TBoolean>;
template class CIDLIBEXP TFundStack<tCIDLib::TCard1>;
template class CIDLIBEXP TFundStack<tCIDLib::TCard4>;

template class CIDLIBEXP TObjArray<TString>;

template class CIDLIBEXP TColCursor<TString>;
template class CIDLIBEXP TColCursor<TKeyValuePair>;

template class CIDLIBEXP TCollection<TString>;
template class CIDLIBEXP TCollection<TKeyValuePair>;
template class CIDLIBEXP TCollection<TKeyValFPair>;

template class CIDLIBEXP TQueue<TString>;
template class CIDLIBEXP TQueue<TKeyValuePair>;

template class CIDLIBEXP TKeyedHashSet<TKeyValuePair,TString,TStringKeyOps>;
template class CIDLIBEXP TKeyedHashSet<TCIDGenCacheItem, TString, TStringKeyOps>;

template class CIDLIBEXP THashSet<TString,TStringKeyOps>;

template class CIDLIBEXP TQueue<TLogEvent>;

template class CIDLIBEXP TVector<TString>;
template class CIDLIBEXP TVector<TStringId>;
template class CIDLIBEXP TVector<TKeyValuePair>;
template class CIDLIBEXP TVector<TKeyValFPair>;
template class CIDLIBEXP TVector<TKeyValues>;
template class CIDLIBEXP TVector<TKeyNumPair>;
template class CIDLIBEXP TVector<TDirChangeInfo>;

template class CIDLIBEXP TRefVector<TThread>;


// Some low level helper methods
namespace tCIDLib
{
    //
    //  For the TEArray template class and some raw array indexing templates, to
    //  keep this out of line. We do this very primitively, creating the event
    //  ourself and throwing it directly, to avoid possible issues, since these
    //  simple and raw arrays might be used in some lower level code.
    //
    tCIDLib::TVoid ThrowArrayIndexErr( const   tCIDLib::TCard4     c4At
                                        , const tCIDLib::TCard4     c4Size)
    {
        TString strMsg(L"Bad index (", 32UL);
        strMsg.AppendFormatted(c4At);
        strMsg.Append(L") used for array, size is ");
        strMsg.AppendFormatted(c4Size);
        throw TLogEvent
        (
            L"CIDLib"
            , L"CIDLib.cpp"
            , CID_LINE
            , strMsg
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
        );
    }
}

tCIDLib::TVoid
tCIDLib::ThrowAssert(const  tCIDLib::TCh* const pszErr
                    , const tCIDLib::TCh* const pszFile
                    , const tCIDLib::TCard4     c4Line)
{
    ThrowAssert(TString(pszErr), pszFile, c4Line);
}

tCIDLib::TVoid
tCIDLib::ThrowAssert(const  TString&            strErr
                    , const tCIDLib::TCh* const pszFile
                    , const tCIDLib::TCard4     c4Line)
{
    throw TLogEvent
    (
        L"CIDLib"
        , pszFile
        , c4Line
        , kCIDErrs::errcDbg_AssertFailed
        , strErr
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Assert
    );
}

tCIDLib::TVoid
tCIDLib::ThrowAssert(const  tCIDLib::TCh* const pszErr
                    , const tCIDLib::TCh* const pszFile
                    , const tCIDLib::TCard4     c4Line
                    , const MFormattable&       mfbtlToken1)
{
    ThrowAssert(TString(pszErr), pszFile, c4Line, mfbtlToken1);
}

tCIDLib::TVoid
tCIDLib::ThrowAssert(const  TString&            strErr
                    , const tCIDLib::TCh* const pszFile
                    , const tCIDLib::TCard4     c4Line
                    , const MFormattable&       mfbtlToken1)
{
    TString strMsg(strErr);
    strMsg.eReplaceToken(mfbtlToken1, kCIDLib::chDigit1);
    throw TLogEvent
    (
        L"CIDLib"
        , pszFile
        , c4Line
        , kCIDErrs::errcDbg_AssertFailed
        , strMsg
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Assert
    );
}


tCIDLib::TVoid
tCIDLib::ThrowPreCond(  const   tCIDLib::TCh* const pszCond
                        , const tCIDLib::TCh* const pszFile
                        , const tCIDLib::TCard4     c4Line)
{
    ThrowPreCond(TString(pszCond), pszFile, c4Line);
}

tCIDLib::TVoid
tCIDLib::ThrowPreCond(  const   TString&            strCond
                        , const tCIDLib::TCh* const pszFile
                        , const tCIDLib::TCard4     c4Line)
{
    throw TLogEvent
    (
        L"CIDLib"
        , pszFile
        , c4Line
        , kCIDErrs::errcDbg_PreCondFailed
        , facCIDLib().strMsg(kCIDErrs::errcDbg_PreCondFailed)
        , strCond
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Assert
    );
}

tCIDLib::TVoid
tCIDLib::ThrowPostCond( const   tCIDLib::TCh* const pszCond
                        , const tCIDLib::TCh* const pszFile
                        , const tCIDLib::TCard4     c4Line)
{
    ThrowPostCond(TString(pszCond), pszFile, c4Line);
}

tCIDLib::TVoid
tCIDLib::ThrowPostCond( const   TString&            strCond
                        , const tCIDLib::TCh* const pszFile
                        , const tCIDLib::TCard4     c4Line)
{
    throw TLogEvent
    (
        L"CIDLib"
        , pszFile
        , c4Line
        , kCIDErrs::errcDbg_PostCondFailed
        , facCIDLib().strMsg(kCIDErrs::errcDbg_PreCondFailed)
        , strCond
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Assert
    );
}


// ---------------------------------------------------------------------------
//  If debugging, then force instantiations of some template classes that are not
//  already being forced above for pre-instantiation.
// ---------------------------------------------------------------------------
#if CID_DEBUG_ON
template class TArrayJanitor<TArea>;
template class TBag<TArea>;
template class TBasicDLinkedCol<TArea>;
template class TBasicDLinkedRefCol<TArea>;
template class TBasicTreeCol<TArea>;
template class TEnumBitset<tCIDLib::EAdoptOpts, tCIDLib::EAdoptOpts::Count>;
template class TCircularBuf<TString>;
template class TCIDCacheItem<TArea>;
template class TCntPtr<TArea>;
template class TDeque<TArea>;
template class TFundDeque<tCIDLib::TCard4>;
template class THashMap<TFindBuf, TPathStr, TStringKeyOps>;
template class THashSet<TFindBuf, TStringKeyOps>;
template class TKeyedHashSet<TFindBuf, TPathStr, TStringKeyOps>;
template class TJanitor<TArea>;
template class TKeyObjPair<TString,TArea>;
template class TMngPtr<TArea>;
template class TNamedValMap<TArea>;
template class TObjArray<TArea>;
template class TPolyStreamer<TArea>;
template class TRefBag<TArea>;
template class TRefDeque<TArea>;
template class TRefKeyedHashSet<TFindBuf, TPathStr, TStringKeyOps>;
template class TRefQueue<TArea>;
template class TRefSortedBag<TString>;
template class TRefStack<TArea>;
template class TRefStackJan<TArea>;
template class TRefVector<TArea>;
template class TSortedBag<TString>;
template class TStack<TArea>;
template class TStackJan<TArea>;


static tCIDLib::ESortComps
eCompTest(const tCIDLib::TCard4& c41, const tCIDLib::TCard4& c42)
{
    if (c41 < c42)
        return tCIDLib::ESortComps::FirstLess;
    else if (c41 > c42)
        return tCIDLib::ESortComps::FirstGreater;
    return tCIDLib::ESortComps::Equal;
}

static tCIDLib::TVoid DummyFunc()
{
    tCIDLib::TBasicPair<TArea, TString> pairTmp;
    pairTmp.m_tF.Set(0, 1, 2, 3);

    TArrayJanitor<tCIDLib::TCard4> janBuf(10);

//    TMemberPtr<TArea> mbptrTest;

    TVector<TString> colOne;
    TVector<TString> colTwo;
    if (tCIDLib::bCompareElems(colOne, colTwo)
    ||  !tCIDLib::bCompareElems(colOne, colTwo))
    {
    }

    if ((colOne == colTwo) || (colOne != colTwo))
    {
    }
    colOne.AddXCopies(TString::strEmpty(), 4);
    colOne.bForEach
    (
        [] (const TString& strCur) -> tCIDLib::TBoolean { return kCIDLib::True; }
    );

    colOne.objPlace(L"Test in place");

    TStringPool splStrings(L"Test", 8);
    THeapBufPool splHeapBufs(L"Test", 8);

    colOne.objAdd(TPathStr(L"Testing"));

    tCIDLib::TCard4 ac4Array[10];
    TArrayOps::TSort<tCIDLib::TCard4>
    (
        ac4Array, 10, tCIDLib::eComp<tCIDLib::TCard4>
    );

    TArrayOps::TSort<tCIDLib::TCard4>
    (
        ac4Array, 10, tCIDLib::TRevCompWrap(eCompTest)
    );

    tCIDLib::TCard4* apc4CArray[10];
    TArrayOps::TSort<tCIDLib::TCard4*>
    (
        apc4CArray, 10, tCIDLib::eCompPtr<tCIDLib::TCard4>
    );

    tCIDLib::TCard4* apc4Array[10];
    TArrayOps::TSortSubFile<tCIDLib::TCard4*>
    (
        apc4Array
        , 2
        , 6
        , tCIDLib::eCompPtr<tCIDLib::TCard4>
    );

    TSortedBag<TString> colBagOStrings
    (
        [](const TString& str1, const TString& str2) { return str1.eCompareI(str2); }
    );
    colBagOStrings.objAdd(L"Added");
    colBagOStrings.objPlace(L"Placed");

    TRefSortedBag<TString> colRefBagOStrings
    (
        tCIDLib::EAdoptOpts::Adopt
        , [](const TString& str1, const TString& str2) { return str1.eCompareI(str2); }
    );


    TBag<TKeyValuePair> colBag;
    colBag.objPlace(L"Key", L"Value");


    tCIDLib::TKVHashSet col1(7, TStringKeyOps(kCIDLib::False), TKeyValuePair::strExtractKey);
    tCIDLib::TKVHashSet col2(7, TStringKeyOps(kCIDLib::False), TKeyValuePair::strExtractKey);
    if (tCIDLib::bCompareElems(col1, col2, TKeyValuePair::bComp))
    {
    }

    col1.objPlace(L"Key", L"Value");

    TEArray<tCIDLib::TCard4, tCIDLib::EDirs, tCIDLib::EDirs::Count> eaTest(0UL);
    eaTest[tCIDLib::EDirs::Left] = 1;
    tCIDLib::ForceMove(eaTest);


    // Object arrays aren't collections, so we need separate stuff for them
    TObjArray<TArea, tCIDLib::EErrClasses> objaEnumed(tCIDLib::EErrClasses::Count);
    TBinMBufOutStream strmOut(2048UL);
    strmOut << objaEnumed;
    TBinMBufInStream strmIn(strmOut);
    strmIn >> objaEnumed;

    tCIDLib::TCard4 c4Count = 0;
    objaEnumed.bForEach
    (
        [&c4Count] (const TArea& areaCur)
        {
            c4Count++;
            return kCIDLib::True;
        }
    );


    // Force some of the enum based collection stuff to be generated
    TVector<TArea, tCIDLib::EErrClasses> colArea;
    colArea.bForEachNC([] (TArea& areaCur) { return kCIDLib::True; });

    TRefVector<TArea> colRefArea(tCIDLib::EAdoptOpts::Adopt);
    TRefVector<TArea> colRefArea2(tCIDLib::EAdoptOpts::Adopt);
    colRefArea.bForEachNC([] (TArea& areaCur) { return kCIDLib::True; });
    if (tCIDLib::bCompareElems(colRefArea, colRefArea2, TString::bComp)
    ||  !tCIDLib::bCompareElems(colRefArea, colRefArea2, TString::bComp))
    {
    }

    if ((colRefArea == colRefArea2) || (colRefArea != colRefArea2))
    {
    }

    TQueue<TString> colStrQ;
    colStrQ.bForEachNC([] (const TString& areaCur) { return kCIDLib::True; });

    colStrQ.objPlace(L"Test placement");

    TRefQueue<TString> colStrRQ(tCIDLib::EAdoptOpts::Adopt);
    colStrRQ.bForEachNC([] (const TString& areaCur) { return kCIDLib::True; });

    TRefKeyedHashSet<TKeyValuePair, TString, TStringKeyOps>
    colRKHS
    (
        tCIDLib::EAdoptOpts::NoAdopt
        , 23
        , TStringKeyOps(kCIDLib::False)
        , &TKeyValuePair::strExtractKey
    );
    colRKHS.bForEachNC([](TKeyValuePair&) { return kCIDLib::False; } );

    TKeyedHashSet<TKeyValuePair, TString, TStringKeyOps>
    colKHS
    (
        23, TStringKeyOps(kCIDLib::False), &TKeyValuePair::strExtractKey
    );
    colKHS.bForEachNC([](TKeyValuePair&) { return kCIDLib::False; } );

    colKHS.objPlace(L"Key", L"Value");


    // Unique pointer template expansion to make sure it's good
    TUniquePtr<TString> uptrTest1(new TString(L"This is a test"));
    TUniquePtr<TString> uptrTest2(tCIDLib::ForceMove(uptrTest1));
    if (uptrTest1 || !uptrTest2->bCompare(L"This is a test"))
    {
    }

    // And managed pointers
    TString strMPtrTest(L"Managed string");
    TMngPtr<TString> mptr1(&strMPtrTest);
    TMngPtr<TString> mptr2(mptr1);
    if ((*mptr1 != *mptr2) || (*mptr1.pobjData() != *mptr2.pobjData()))
    {
    }


    // The typed memory buffer class
    TTypedMBuf<tCIDLib::TMsgIndexEntry> mbufTyped;
    mbufTyped.objData().midThis = 0;
    mbufTyped->c4TextOffset = 1;


    // Fundamental collections
    TFundArray<tCIDLib::TBoolean> fcolArray(8);
    fcolArray.bForEach
    (
        [](const tCIDLib::TBoolean bCur) { return kCIDLib::True; }
    );

    TFundVector<tCIDLib::TBoolean> fcolVector(8);
    fcolVector.bForEach
    (
        [](const tCIDLib::TBoolean bCur) { return kCIDLib::True; }
    );

    TFundVector<tCIDLib::TCard8> fcolCard8s;
    const tCIDLib::TCard8 c8MaxDup
        = tCIDColAlgo::tFindMaxFundSeqDup<TFundVector<tCIDLib::TCard8>>(fcolCard8s);

    TTextStringOutStream strmTest(1024UL);
    strmTest.Format(L"Value %(1) is really %(2)", TString(L"Test"), TCardinal(2));


    TVector<TArea> colTestCurs(2);
    colTestCurs.objAdd(TArea(1, 1, 1UL, 1UL));
    colTestCurs.objAdd(TArea(2, 2, 2UL, 2UL));
    TVector<TArea>::TCursor cursTest(&colTestCurs);
    cursTest++;
    --cursTest;

    for (cursTest.bReset(); cursTest; ++cursTest)
    {
    }

    TVector<TArea>::TNCCursor cursNCTest(&colTestCurs);
    cursNCTest++;
    --cursNCTest;

    if ((cursTest->i4Y() == 0) && (*cursTest == TArea(0, 0, 0UL, 0UL)))
    {
    }

    TVector<TArea>::TCursor cursFind = tCIDColAlgo::cursFind(colTestCurs, TArea(1,1,1UL,1UL));
    if (cursFind.bIsValid())
    {
    }

    TVector<TArea>::TNCCursor cursFindNC = tCIDColAlgo::cursFindNC(colTestCurs, TArea(1,1,1UL,1UL));
    if (cursFindNC.bIsValid())
    {
    }

    TNamedValMap<TString> colNVM1;
    TNamedValMap<TString> colNVM2;
    if ((colNVM1 == colNVM2) || (colNVM1 != colNVM2))
    {
    }

    TVector<const TString> colConstStr;
    const TString& strNC = colConstStr[0];

    TUniquePtr<TString> uptrTest(new TString(L"Testing"));

    TObjLocker<TArea> olockrTest(10, 12, 14UL, 16UL);
    TObjLock<TArea> olockTest = olockrTest.olockGet(5000);
    if (*olockTest != TArea(10, 12, 14UL, 16UL))
    {
    }
    TObjLock<TArea> olockTest2 = olockrTest.olockTryGet(1000);
    if (olockTest2)
    {
    }

    TBasicTreeCol<TString> colTree;
    StreamOutBasicTree<TString>(colTree, strmOut);

    TString::strConcat(strNC, kCIDLib::chOpenBrace, L" Some text");

    constexpr tCIDLib::TFloat4 f4Test = 0;
    if (tCIDLib::IsTFloatX<decltype(f4Test)>::bState)
    {
    }

    tCIDLib::TCard4 c4Val1 = 16;
    tCIDLib::TCard4 c4Val2 = 32;
    alignas(kCIDLib::c4CacheAlign) tCIDLib::TCard4* pTest1 = &c4Val1;
    alignas(kCIDLib::c4CacheAlign) tCIDLib::TCard4* pTest2 = &c4Val2;
    if (TAtomic::pFencedGet(&pTest1) != &c4Val1)
    {
    }

    CIDLib_Suppress(6386)
    TAtomicPtr<TString> atomStr;
    if (!atomStr.pValue())
    {
        atomStr.SetValue(new TString());
    }
    if (atomStr->bIsEmpty())
    {
    }


    if (bCIDPreCond(c4Val1 > c4Val2) || !bCIDPostCond(c4Val1 < c4Val2))
    {
    }

    if (!bCIDAssert(c4Val1 > c4Val2, L"It was wrong, dude"))
    {
    }

    if (!bCIDAssertX(c4Val1 > c4Val2, L"It was wrong, %(1)", TString(L"Dude")))
    {
    }

    TSingleton<TString> singleTest;

    tCIDLib::TCard4 ac4Test[4] = { 0, 1, 2, 3 };
    tCIDLib::c4ArrayElems(ac4Test);
    if (tCIDLib::c4ArrayAt(ac4Test, 2) != 3)
    {
        tCIDLib::TCard4* pc4End = tCIDLib::pArrayEndPtr(ac4Test);
    }

    const tCIDLib::TCard4 ac4TestC[4] = { 0, 1, 2, 3 };
    tCIDLib::c4ArrayElems(ac4TestC);
    if (tCIDLib::c4ArrayAt(ac4TestC, 2) != 3)
    {
        const tCIDLib::TCard4* pc4End = tCIDLib::pArrayEndPtr(ac4TestC);
    }

    tCIDLib::ForEachE<tCIDLib::ELogFlags>([](const tCIDLib::ELogFlags eFlag)
    {
        if (eFlag == tCIDLib::ELogFlags::DataSrc)
        {

        }
    });

    // Test ignoring return values
    tCIDLib::IgnoreRet(strNC.i4Val());
}
#endif


// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------

//
//  The lazy evaluation method for the facility object. This is the only
//  access to the facility object, insuring that it gets allocated when
//  its required.
//
TFacCIDLib& facCIDLib()
{
    static TFacCIDLib* pfacCIDLib = new TFacCIDLib();
    return *pfacCIDLib;
}


//
//  This is called from the magic main thread macro in CIDLib.Hpp. Its called
//  after the (w)main() function has been entered and before the main thread
//  blocks on the user's primary thread. It gives us an opportunity to do any
//  non-platform init before the thread is started up.
//
//  For now we don't need it, but it could be useful in the future.
//
tCIDLib::TVoid CIDLib_Init()
{
}

