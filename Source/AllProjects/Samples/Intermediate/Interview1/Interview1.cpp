//
// FILE NAME: Interview1.cpp
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
//  This is the main module for a program that implements a question I was asked
//  in an interview. Doing it on the fly meant that version was pretty messy and I
//  made a lot of syntax errors even though I was using my own stuff since I had
//  to do it by eye, so I figured I'd do a good quality version of it and add it
//  as a sample.
//
//  It mostly demonstrates collection stuff, since it's just the kind of problem
//  that involves various lists of things.
//
//  The problem is:
//
//      Given a list of accessed web pages, the customers who accessed them,
//      and the time each access was made, find the most accessed sequence of
//      three pages in a row by any particular customer.
//
//  So a list like:
//
//      c1, pagea, t
//      c2, pageb, t
//      c1, pagee, t
//      c2, pagec, t
//      c3, pagea, t
//
//  And so on is the incoming data we get. We have to use the time stamp to know
//  the access order of the pages, and we have to use the customer to know which
//  pages represent the pages accessed by a specific customer.
//
//
//  Here I'm making an assumption that there will not be more than 64K unique URLs. If
//  so, the rest will be ignored. So we are probably assuming that this is for a
//  company to assess access to its own resources, not the entire internet. In return
//  we can be a lot more efficient.
//
//  This lets us encode the sequences in 64 bit values that hold the three unique page
//  ids that make up a sequence. They are stored (high to low bytes) like this:
//
//      00000000  firstpage  secpage  thirdpage
//
//  We use this to very good effect below.
//
//
//  If there is more than one most used sequence, we only keep the first one we
//  find and report that. It would be pretty simple to extend it to report all
//  ties for first place.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  We need the CIDMData facility for unique strings. It'll bring in the rest
//  we need.
// ---------------------------------------------------------------------------
#include    "CIDMData.hpp"


// ---------------------------------------------------------------------------
//  For this simple program, start main thread on a global
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"Interview1MainThread", eMainThreadFunc))


// ---------------------------------------------------------------------------
//  Here is our test data. Time stamps are just numbers, possibly representing
//  some offset from the beginning of the data collection period. For this simple
//  program, it's just a basic array of structures.
//
//  Given this set of data, the answer should be:
//
//    1. https://bogusworld.net/home.html
//    2. http://bogusworld.net/interviews.html?v=43
//    3. http://bogusworld.net/interviews.html?v=53
// ---------------------------------------------------------------------------
struct TRawAccessItem
{
    const TString           m_strCust;
    const TString           m_strURL;
    const tCIDLib::TCard4   m_c4TimeOfs;
};
static const TRawAccessItem aitemsTestData[] =
{
    { L"Carly Gauss"    , L"https://bogusworld.net/home.html"           ,   20 }
  , { L"Lenny Oiler"    , L"http://bigeyedfish.net/index.html"          ,    5 }
  , { L"Lenny Oiler"    , L"http://bigeyedfish.net/signup.php"          ,   45 }
  , { L"Stevie Hawkley" , L"http://makebigbucks.com/login.php?id=none"  ,   60 }
  , { L"Stevie Hawkley" , L"http://makebigbucks.com/login.php?id=644"   ,   68 }
  , { L"Stevie Hawkley" , L"http://makebigbucks.com/ripoff.php"         ,   48 }
  , { L"Lenny Oiler"    , L"http://bigeyedfish.net/Walleye.html"        ,   45 }
  , { L"Carly Gauss"    , L"http://bogusworld.net/interviews.html?v=43" ,   20 }
  , { L"Carly Gauss"    , L"http://bogusworld.net/interviews.html?v=53" ,   24 }
  , { L"Stevie Hawkley" , L"http://makebigbucks.com/ripoff.php"         ,   58 }
  , { L"Lenny Oiler"    , L"http://bogusworld.net/signup.php"           ,   20 }
  , { L"Lenny Oiler"    , L"http://bigeyedfish.net/Nemoids.html"        ,   92 }
};
static const tCIDLib::TCard4 c4RawItemCount = tCIDLib::c4ArrayElems(aitemsTestData);



// ---------------------------------------------------------------------------
//  We need to convert URLs and customers to unique ids. Max of 64K URLs. These
//  handle the unique string grunt work for us.
// ---------------------------------------------------------------------------
static TUniqueString ustrCustomers(kCIDLib::c4MaxCard, 509);
static TUniqueString ustrURLs(kCIDLib::c2MaxCard, 509);



// ---------------------------------------------------------------------------
//  This will be our own view of the data, in which the customers and URLs
//  have been replaced with ids that represent unique ids for those things.
//  We will convert each each raw test item into a list of these massaged
//  items.
// ---------------------------------------------------------------------------
class TAccessItem
{
    public :
        // Set us up from a passed raw access item
        TAccessItem(const TRawAccessItem& itemSrc) :

            m_c4TimeOfs(itemSrc.m_c4TimeOfs)
        {
            // Store our customer/url unique ids
            m_c4CustID = ustrCustomers.c4AddString(itemSrc.m_strCust);
            m_c2URLID = tCIDLib::TCard2(ustrURLs.c4AddString(itemSrc.m_strURL));
        }

        tCIDLib::TCard4     m_c4CustID;
        tCIDLib::TCard2     m_c2URLID;
        tCIDLib::TCard4     m_c4TimeOfs;
};

using TAccessList = TVector<TAccessItem>;
static TAccessList colAccessItems;


// ---------------------------------------------------------------------------
//  This is the the thread function for the main thread. It is started by
//  CIDLib_MainModule() above.
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    TTextOutStream& strmOut = TSysInfo::strmOut();
    try
    {
        // Construct one massaged item from each raw item
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4RawItemCount; c4Index++)
        {
            colAccessItems.objAdd(TAccessItem(aitemsTestData[c4Index]));
            if (ustrURLs.bIsFull())
            {
                strmOut << L"Maximum unique URLs reached, not all data will be processed"
                        << kCIDLib::EndLn;
                break;
            }
        }

        // Now sort by the time. This is just sorting pointers, not objects
        colAccessItems.Sort
        (
            [](const TAccessItem& item1, const TAccessItem& item2) -> tCIDLib::ESortComps
            {
                if (item1.m_c4TimeOfs < item2.m_c4TimeOfs)
                    return tCIDLib::ESortComps::FirstLess;
                else if (item1.m_c4TimeOfs > item2.m_c4TimeOfs)
                    return tCIDLib::ESortComps::FirstGreater;
                return tCIDLib::ESortComps::Equal;
            }
        );

        //
        //  For each customer, we maintain a 64 bit seq value. As we see a new URL for
        //  each customer, we will shift the id of that URL into the customer's
        //  current seq value. The sequence values are encoded like this:
        //
        //      0000 firsturlid securlid thirdurlid
        //
        //  This is a low cost way to find all of the sequences, with dups. We don't
        //  keep any until at least three have been seen for that customer.
        //
        using TSeqList = TFundVector<tCIDLib::TCard8>;
        TSeqList fcolAllSeqsList;
        {
            // A list with one 64 bit value per unique customer
            TFundArray<tCIDLib::TCard8> fcolCustSeqList(ustrCustomers.c4StringCount());
            colAccessItems.ForEach
            (
                [&] (const TAccessItem& itemCur)
                {
                    // Update this customer's current sequence value with a new URL
                    tCIDLib::TCard8 c8Seq = fcolCustSeqList[itemCur.m_c4CustID - 1];
                    c8Seq = ((c8Seq << 16) | itemCur.m_c2URLID) & 0xFFFFFFFFFFFF;
                    fcolCustSeqList[itemCur.m_c4CustID - 1] = c8Seq;

                    // If this customer has valid sequences now, store this one
                    if (c8Seq & 0xFFFF00000000)
                        fcolAllSeqsList.c4AddElement(c8Seq);
                    return kCIDLib::True;
                }
            );
        }

        // Now sort the 64 bit sequence values to get dups adjacent
        fcolAllSeqsList.Sort(tCIDLib::eComp<tCIDLib::TCard8>);

        // Find the sequence that has the most sequential dups
        const tCIDLib::TCard8 c8BestSeq
        (
            tCIDColAlgo::tFindMaxFundSeqDup<TSeqList>(fcolAllSeqsList)
        );

        // Possible no customer ever had 3 URLs
        if (!c8BestSeq)
        {
            strmOut << L"There was no best last sequence" << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::Normal;
        }

        // And let's display the winners
        strmOut << L"\nThe top three page sequence is:"
                << L"\n  1. " << ustrURLs[(c8BestSeq & 0xFFFF00000000) >> 32]
                << L"\n  2. " << ustrURLs[(c8BestSeq & 0xFFFF0000) >> 16]
                << L"\n  3. " << ustrURLs[(c8BestSeq & 0xFFFF)]
                << kCIDLib::NewEndLn;
    }

    catch(const TError& errToCatch)
    {
        strmOut << L"An exception occurred:\n";
        TStreamIndentJan janIndent(&strmOut, 8);
        strmOut << errToCatch << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    return tCIDLib::EExitCodes::Normal;
}
