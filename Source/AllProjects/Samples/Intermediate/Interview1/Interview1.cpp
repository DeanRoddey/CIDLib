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
//  pages represent the valid sequences of access by a customer.
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
//  We just need the base CIDLib facility for collections, strings, etc...
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ---------------------------------------------------------------------------
//  Do the magic main module code. For this simple program, we just point the main
//  thread to a global function below.
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
//  Unique id counters. This is all single threaded so no sync required. We only
//  use up to 64K URL ids but it's more convenient to use the same size counters.
//
//  IMPORTANTLY they cannot be zero! We use zero below to know if an id has been
//  set or not.
// ---------------------------------------------------------------------------
static tCIDLib::TCard4 c4NextCustId = 1;
static tCIDLib::TCard4 c4NextURLId = 1;


// ---------------------------------------------------------------------------
//  We'll need some hash sets of strings to create unique customer and URL ids.
//  We need a small class that includes the unique string and a unique id assigned
//  to each one. For this simple sample, just use public members.
//
//  We use a keyed hash set (the key is inside the elements in this type of set.)
//  The elements are our unique string structure, the key is the unique string,
//  the key ops object provides hashing for our key strings. We use a lambda to
//  provide the key extraction callback to let the hash set get the key out of the
//  elements.
//
//  We create one for tracking unique customers and one for unique URLs. A fairly
//  big hash modulus to reduce overhead by spreading the items out into smaller
//  buckets. An even bigger one might be reasonable and might be set based on
//  input set size in a real scenario.
// ---------------------------------------------------------------------------
struct TUniqueStr
{
    TUniqueStr() = delete;
    TUniqueStr(const TString& strVal, const tCIDLib::TCard4 c4Id) :

        m_strVal(strVal)
        , m_c4Id(c4Id)
    {
    }
    TString         m_strVal;
    tCIDLib::TCard4 m_c4Id;
};

using TUStringSet = TKeyedHashSet<TUniqueStr, TString, TStringKeyOps>;
static TUStringSet colUniqueCust
(
    509
    , new TStringKeyOps(kCIDLib::False)
    , [](const TUniqueStr& ustrCur) -> const TString& { return ustrCur.m_strVal; }
);
static TUStringSet colUniqueURL
(
    509
    , new TStringKeyOps(kCIDLib::False)
    , [](const TUniqueStr& ustrCur) -> const TString& { return ustrCur.m_strVal; }
);



// ---------------------------------------------------------------------------
//  This will be our own view of the data, in which the customers and URL have been
//  replaced with ids that represent unique ids for those things. We do this
//  while loading up the raw data, so we can work purely in terms of these unique
//  values for the rest of the way.
// ---------------------------------------------------------------------------
class TAccessItem
{
    public :
        TAccessItem() = delete;

        // Set us up from a passed raw access item
        TAccessItem(const TRawAccessItem& itemSrc) :

            m_c4TimeOfs(itemSrc.m_c4TimeOfs)
        {
            //
            //  Find (or add) the new strings and store their ids as our ids. We call
            //  a private helper to do the work since it's the same for both. But we
            //  only keep the low 16 bits of the URL id.
            //
            m_c4CustID = c4AddOrUpdate(colUniqueCust, itemSrc.m_strCust, c4NextCustId);
            m_c2URLID = tCIDLib::TCard2
            (
                c4AddOrUpdate(colUniqueURL, itemSrc.m_strURL, c4NextURLId)
            );
        }

        tCIDLib::TCard4     m_c4CustID;
        tCIDLib::TCard2     m_c2URLID;
        tCIDLib::TCard4     m_c4TimeOfs;

    private :
        //
        //  A helper since we do the same thing to look up both our customer and URL ids.
        //  Returns the id of the new string.
        //
        tCIDLib::TCard4 c4AddOrUpdate(          TUStringSet&        colList
                                        , const TString&            strNew
                                        ,       tCIDLib::TCard4&    c4Counter)
        {
            tCIDLib::TCard4 c4RetId;
            const TUniqueStr* pustrTest = colList.pobjFindByKey(strNew);
            if (pustrTest)
            {
                // Already have it so return that id
                c4RetId = pustrTest->m_c4Id;
            }
            else
            {
                // Take the next id and add this new string to the passed list
                c4RetId = c4Counter++;
                TUniqueStr ustrNew(strNew, c4RetId);
                colList.objAdd(ustrNew);
            }
            return c4RetId;
        }
};

//
//  The raw data will be converted as it's loaded. If the incoming data was time
//  ordered already we could skip a number of steps. But we need to sort it by time,
//  so we need to get into a list we can sort.
//
//  Note that the sort is not as heavy as it might seem since it is really just
//  sorting pointers, even though it is a 'by value' vector.
//
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
        //
        //  OK, let's loop through the source data and create our own view of the info
        //  with resolved customer/url ids. This is very simple since the access item
        //  objects resolve their own ids and store them. We just construct one from
        //  each raw test data item.
        //
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4RawItemCount; c4Index++)
        {
            colAccessItems.objAdd(TAccessItem(aitemsTestData[c4Index]));

            // If we max out unique URL ids, we give up
            if (c4NextURLId == kCIDLib::c2MaxCard)
            {
                strmOut << L"Maximum unique URLs reached, not all data will be processed"
                        << kCIDLib::EndLn;
                break;
            }
        }

        //
        //  Now sort by the time, we use a lambda for the comparator. As mentioned
        //  above, this is just sorting pointers, not the whole objects.
        //
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
        //  Now we can start to find all of the 3 page sequences. We can be somewhat
        //  clever here. Because we encode the sequences as 64 bit values, we don't
        //  need to keep anything but one 64 bit value per unique customer. These
        //  values will initially be zero.
        //
        //  For each new page hit we get for a particular customer, we go to the
        //  [custid - 1] element of that list and get that value out. We then just
        //  shift that value up by 16 bits and mask in the new value. That becomes
        //  the new sequence for that customer.
        //
        //  One gotcha is that we have to wait till we get the first three pages for
        //  a given customer before we start taking them. All we need to do is check
        //  the third byte. Once that's non-zero, we know we are OK. The URL ids
        //  cannot be zero.
        //
        //  To keep overhead low, we don't even attempt to figure out here if the
        //  sequences are unique. We just add each three page sequence to the list
        //  of 64 bit values. Else we'd have to go back every time and see if that
        //  sequence is already in the list, which would be a lot of overhead no
        //  matter how we do it.
        //
        //  We end up with a sequence list with all of the sequences, including
        //  dups, in no particular order.
        //
        TFundVector<tCIDLib::TCard8> fcolAllSeqsList;
        {
            // A list with one 64 bit value per unique customer
            TFundArray<tCIDLib::TCard8> fcolCustSeqList(c4NextCustId);
            {
                // We loop through the time sorted input data
                TAccessList::TCursor cursSorted(&colAccessItems);
                for (; cursSorted; cursSorted++)
                {
                    const TAccessItem& itemCur = *cursSorted;

                    //
                    //  Get a copy of this customer's lastest sequence and update it
                    //  make sure the top 16 get cleared after the shift! Then set it
                    //  back.
                    //
                    tCIDLib::TCard8 c8Seq = fcolCustSeqList[itemCur.m_c4CustID - 1];
                    c8Seq <<= 16;
                    c8Seq |= itemCur.m_c2URLID;
                    c8Seq &= 0xFFFFFFFFFFFF;
                    fcolCustSeqList[itemCur.m_c4CustID - 1] = c8Seq;

                    //  If we are now into valid sequences for this customer, keep it
                    if (c8Seq & 0xFFFF00000000)
                        fcolAllSeqsList.c4AddElement(c8Seq);
                }
            }
        }

        //
        //  Now we can sort all of the sequences by the sequence id, to get all dups
        //  adjacent. We are just sorting 64 bit values here, so it's not very heavy
        //  even though the list might be sort of large. We use a standard comparator
        //  here.
        //
        fcolAllSeqsList.Sort(tCIDLib::eComp<tCIDLib::TCard8>);

        //
        //  And finally we can just count adjacent dups to get the count for each
        //  sequence. We remember the best one as we go. At the end, we have the
        //  most accessed one. We don't try to deal with ties, we'll keep the first
        //  best sequence that we get.
        //
        tCIDLib::TCard8 c8LastBestSeq = 0;
        {
            tCIDLib::TCard4 c4LastBestCount = 0;
            const tCIDLib::TCard4 c4AllSeqCount = fcolAllSeqsList.c4ElemCount();
            tCIDLib::TCard4 c4Index = 0;
            while (c4Index < c4AllSeqCount)
            {
                const tCIDLib::TCard8 c8CurSeq = fcolAllSeqsList[c4Index++];
                tCIDLib::TCard4 c4RunCount = 1;
                while (c4Index < c4AllSeqCount)
                {
                    // If not the same, we are done for this one, else bump values
                    const tCIDLib::TCard8 c8NextSeq = fcolAllSeqsList[c4Index];
                    if (c8NextSeq != c8CurSeq)
                        break;

                    c4RunCount++;
                    c4Index++;
                }

                // If this count is better than the last, then take this one
                if (c4RunCount > c4LastBestCount)
                {
                    c4LastBestCount = c4RunCount;
                    c8LastBestSeq = c8CurSeq;
                }
            }
        }

        //
        //  And, now we have the most used sequence. If we want to get the actual
        //  URLs, we'll have to go back and find these URL ids in the unique URL list.
        //
        //  We pay a little at this point for our optimization above since we can
        //  do no better here than just bull through the whole unique URL list and
        //  look for the matching ids, since that relationship was lost.
        //
        TString strURL1;
        TString strURL2;
        TString strURL3;
        {
            // Break out the three page ids for convenience below
            const tCIDLib::TCard4 c4URL1 = (c8LastBestSeq & 0xFFFF00000000) >> 32;
            const tCIDLib::TCard4 c4URL2 = (c8LastBestSeq & 0xFFFF0000) >> 16;
            const tCIDLib::TCard4 c4URL3 = (c8LastBestSeq & 0xFFFF);

            //
            //  Stop once we find our three guys, no need to do the whole thing
            //  in most cases.
            //
            tCIDLib::TCard4 c4Found = 0;
            TUStringSet::TCursor cursURLs(&colUniqueURL);
            for (; cursURLs && (c4Found < 3); ++cursURLs)
            {
                const TUniqueStr& ustrCur = *cursURLs;
                if (ustrCur.m_c4Id == c4URL1)
                {
                    strURL1 = ustrCur.m_strVal;
                    c4Found++;
                }
                 else if (ustrCur.m_c4Id == c4URL2)
                {
                    strURL2 = ustrCur.m_strVal;
                    c4Found++;
                }
                 else if (ustrCur.m_c4Id == c4URL3)
                {
                    strURL3 = ustrCur.m_strVal;
                    c4Found++;
                }
            }
        }

        // And let's display them
        strmOut << L"\nThe top three page sequence is:\n"
                << L"  1. " << strURL1 << kCIDLib::NewLn
                << L"  2. " << strURL2 << kCIDLib::NewLn
                << L"  3. " << strURL3 << kCIDLib::NewLn
                << kCIDLib::EndLn;
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
