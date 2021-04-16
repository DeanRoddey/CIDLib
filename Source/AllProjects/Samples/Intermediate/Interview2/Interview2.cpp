//
// FILE NAME: Interview2.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/16/2019
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
//  This is the main module for a program that implements a coding interview
//  type scenario. Which is the classic one of reading in a text file, getting
//  the n most frequently used words, and printing them out sorted along with their
//  frequency.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  We just need the core CIDLib stuff
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ---------------------------------------------------------------------------
//  For this simple program, start main thread on a global
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"Interview2MainThread", eMainThreadFunc))


// ---------------------------------------------------------------------------
//  This is the the thread function for the main thread. It is started by
//  CIDLib_MainModule() above.
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    //
    //  Get references to the in/out streams. Input will be redirected
    //  in this case, to give us text file input.
    //
    TTextOutStream& strmOut = TSysInfo::strmOut();
    TTextInStream& strmIn = TSysInfo::strmIn();

    // Create a simple class to hold our string/count values and a hash set of that
    using TWordItem = TKeyObjPair<TString, tCIDLib::TCard4>;
    using TWordList = TKeyedHashSet<TWordItem, TString, TStringKeyOps>;
    TWordList colWords(109, TStringKeyOps(kCIDLib::False), TWordItem::objExtractKey);

    //
    //  Read white space separated tokens from the stream. For each one add
    //  it to our our list or bump its counter if already present.
    //
    {
        const TString strWS(kCIDLib::szWhitespace);
        TString strCurToken;
        TWordList::TNCCursor cursFind;
        while (!strmIn.bEndOfStream())
        {
            if (strmIn.c4GetToken(strCurToken, strWS))
            {
                cursFind = colWords.cursFindByKey(strCurToken);
                if (cursFind)
                    cursFind->objValue()++;
                else
                    colWords.objAdd(TWordItem(strCurToken, 1));
            }
        }
    }

    // Get a vector view of the word items
    TRefVector<TWordItem> colSorted(tCIDLib::EAdoptOpts::NoAdopt);
    colWords.bForEachNC
    (
        [&colSorted](TWordItem& itemCur)
        {
            colSorted.Add(&itemCur); return kCIDLib::True;
        }
    );

    // Sort the view by count reversed
    colSorted.Sort
    (
        [](const TWordItem& item1, const TWordItem& item2)
        {
            return tCIDLib::eRevComp(item1.objValue(), item2.objValue());
        }
    );

    // And display up to 10 of them
    tCIDLib::TCard4 c4Count = 0;
    TRefVector<TWordItem>::TCursor cursWords(&colSorted);
    for (; cursWords && (c4Count < 10); ++cursWords, ++c4Count)
    {
        strmOut << cursWords->objKey() << L"=" << cursWords->objValue()
                << kCIDLib::NewLn;
    }
    strmOut << kCIDLib::EndLn;

    return tCIDLib::EExitCodes::Normal;
}
