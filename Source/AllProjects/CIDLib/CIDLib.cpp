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
//  For this file we want to pre instantiate some collections, but not for any
//  other files. Only this file defines this toke, which prevents the use of the
//  extern declaration.
// ---------------------------------------------------------------------------
#define CIDLIB_PREINST  1


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
    //  For the TEArray template class, to keep this out of line. We do this very
    //  primitively, creating the event ourself and throwing it directly, to avoid
    //  possible issues, since these simple arrays might be used in some lower
    //  level code.
    //
    tCIDLib::TVoid ThrowEArrayIndexErr( const   tCIDLib::TCard4     c4At
                                        , const tCIDLib::TCard4     c4Size)
    {
        TString strMsg(L"Bad index (", 32UL);
        strMsg.AppendFormatted(c4At);
        strMsg.Append(L") used for TEArray, size is ");
        strMsg.AppendFormatted(c4Size);
        TLogEvent errToThrow
        (
            L"CIDLib"
            , L"CIDLib.cpp"
            , CID_LINE
            , strMsg
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
        );
        throw errToThrow;
    }
}

tCIDLib::TVoid tCIDLib::ThrowAssert(const TString& strErr)
{
    throw TLogEvent
    (
        L"CIDLib"
        , CID_FILE
        , CID_LINE
        , kCIDErrs::errcDbg_AssertFailed
        , strErr
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Assert
    );
}

tCIDLib::TVoid
tCIDLib::ThrowAssert(const TString& strErr, const MFormattable& mfbtlToken1)
{
    TString strMsg(strErr);
    strMsg.eReplaceToken(mfbtlToken1, kCIDLib::chDigit1);
    throw TLogEvent
    (
        L"CIDLib"
        , CID_FILE
        , CID_LINE
        , kCIDErrs::errcDbg_AssertFailed
        , strMsg
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
template class TBasicTreeCol<TArea>;
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
template class TQueue<TString>;
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
    tCIDLib::ESortComps::Equal;
}

static tCIDLib::TVoid DummyFunc()
{
    TArrayJanitor<tCIDLib::TCard4> janBuf(10);

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
    colOne.ForEach([] (const TString& strCur) -> tCIDLib::TBoolean { return kCIDLib::True; });

    TStringPool splStrings(L"Test", 8);
    THeapBufPool splHeapBufs(L"Test", 8);

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


    TRefSortedBag<TString> colRefBagOStrings
    (
        tCIDLib::EAdoptOpts::Adopt
        , [](const TString& str1, const TString& str2) { return str1.eCompareI(str2); }
    );


    tCIDLib::TKVHashSet col1(7, new TStringKeyOps(kCIDLib::False), TKeyValuePair::strExtractKey);
    tCIDLib::TKVHashSet col2(7, new TStringKeyOps(kCIDLib::False), TKeyValuePair::strExtractKey);
    if (tCIDLib::bCompareElems(col1, col2, TKeyValuePair::bComp))
    {
    }

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
    objaEnumed.ForEach
    (
        [&c4Count] (const TArea& areaCur)
        {
            c4Count++;
            return kCIDLib::True;
        }
    );


    // Force some of the enum based collection stuff to be generated
    TVector<TArea, tCIDLib::EErrClasses> colArea;
    colArea.ForEachNC([] (TArea& areaCur) { return kCIDLib::True; });

    TRefVector<TArea> colRefArea(tCIDLib::EAdoptOpts::Adopt);
    TRefVector<TArea> colRefArea2(tCIDLib::EAdoptOpts::Adopt);
    colRefArea.ForEachNC([] (TArea& areaCur) { return kCIDLib::True; });
    if (tCIDLib::bCompareElems(colRefArea, colRefArea2, TString::bComp)
    ||  !tCIDLib::bCompareElems(colRefArea, colRefArea2, TString::bComp))
    {
    }

    if ((colRefArea == colRefArea2) || (colRefArea != colRefArea2))
    {
    }

    TQueue<TString> colStrQ;
    colStrQ.ForEachNC([] (const TString& areaCur) { return kCIDLib::True; });

    TRefQueue<TString> colStrRQ(tCIDLib::EAdoptOpts::Adopt);
    colStrRQ.ForEachNC([] (const TString& areaCur) { return kCIDLib::True; });

    TRefKeyedHashSet<TKeyValuePair, TString, TStringKeyOps>
    colRKHS
    (
        tCIDLib::EAdoptOpts::NoAdopt
        , 23
        , new TStringKeyOps(kCIDLib::False)
        , &TKeyValuePair::strExtractKey
    );
    colRKHS.ForEachNC([](TKeyValuePair&) { return kCIDLib::False; } );

    TKeyedHashSet<TKeyValuePair, TString, TStringKeyOps>
    colKHS
    (
        23, new TStringKeyOps(kCIDLib::False), &TKeyValuePair::strExtractKey
    );
    colKHS.ForEachNC([](TKeyValuePair&) { return kCIDLib::False; } );


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
    fcolArray.ForEach
    (
        [](const tCIDLib::TBoolean bCur) { return kCIDLib::True; }
    );

    TFundVector<tCIDLib::TBoolean> fcolVector(8);
    fcolVector.ForEach
    (
        [](const tCIDLib::TBoolean bCur) { return kCIDLib::True; }
    );


    TTextStringOutStream strmTest(1024UL);
    TMutex mtxSync;
    TSafeTStrmJan janStrm(&strmTest, &mtxSync);
    *janStrm << L"This is a test" << kCIDLib::EndLn;


    TVector<TArea> colTestCurs(2);
    colTestCurs.objAdd(TArea(1, 1, 1, 1));
    colTestCurs.objAdd(TArea(2, 2, 2, 2));
    TVector<TArea>::TCursor cursTest(&colTestCurs);
    cursTest++;
    --cursTest;

    for (cursTest.bReset(); cursTest; ++cursTest)
    {
    }

    TVector<TArea>::TNCCursor cursNCTest(&colTestCurs);
    cursNCTest++;
    --cursNCTest;

    if ((cursTest->i4Y() == 0) && (*cursTest == TArea(0, 0, 0, 0)))
    {
    }

    TVector<TArea>::TCursor cursFind = tCIDColAlgo::cursFind(colTestCurs, TArea(1,1,1,1));
    if (cursFind.bIsValid())
    {
    }

    TNamedValMap<TString> colNVM1;
    TNamedValMap<TString> colNVM2;
    if ((colNVM1 == colNVM2) || (colNVM1 != colNVM2))
    {
    }
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
    static TFacCIDLib* pfacCIDLib = nullptr;
    if (!pfacCIDLib)
    {
        TBaseLock lockInit;
        if (!pfacCIDLib)
            pfacCIDLib = new TFacCIDLib;
    }
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

