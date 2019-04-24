//
// FILE NAME: Collect2.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/24/1997
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
//  This is the main module for the second of the collection oriented demo
//  programs. This one demonstrates the hash map collection. It creates a
//  simple class, TNameInfo, which is the element type for the test. This
//  class has a name string, the key field, and a counter. It is used to
//  track how many times a name is found in a list. A simple array of strings
//  is used as test data.
//
//  The logic of the program is to test to see if the next name is in the
//  set already. If not, its added. If so, then its counter is bumped up.
//  At the end, the information is dumped out.
//
//  The hash map requires a 'key ops' object to handle hashing and comparing
//  key objects. Since our key field is a TString, we can use a canned one
//  that is provided by CIDLib, TStringKeyOps. We also have to provide a
//  small function that will pull out the key from the data element, which we
//  just do with a lambda.
//
//  This guy also demonstrates column text output using stream format objects.
//
//  As with most of these simple demos, this one does not create a facility
//  object, it just starts the main thread on a local function.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes.
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ---------------------------------------------------------------------------
//  Do the magic main module code to point the main thread at a global function
//  below.
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"Collect2MainThread", eMainThreadFunc))


// ---------------------------------------------------------------------------
//  Local, static data
//
//  apszNames
//  c4NameCount
//      This is just a list of dummy names that serves as test data for the program
//      and the count of elements in it.
//
//  conOut
//      This is a console object which we use in this program for our standard
//      output. Its a specialized text output stream class. We don't support
//      redirection in this program, so we can just use a console directly.
// ---------------------------------------------------------------------------
static const tCIDLib::TCh* apszNames[] =
{
    L"Habib, Amat"
    , L"Roddey, Dean"
    , L"Smith, John"
    , L"Wiedersatz, Michelle"
    , L"Heninger, Andy"
    , L"Werts, Michael"
    , L"Smith, John"
    , L"Clinton, Bill"
    , L"Clinton, Hillary"
    , L"Habib, Amat"
    , L"Smith, John"
    , L"Hendrix, Jimi"
    , L"Hahn, Jessie"
    , L"Joplin, Janis"
    , L"Hendrix, Jimi"
    , L"Smith, John"
};
static const tCIDLib::TCard4 c4NameCount = tCIDLib::c4ArrayElems(apszNames);
static TOutConsole  conOut;


// ---------------------------------------------------------------------------
//   CLASS: TNameInfo
//  PREFIX: nmi
//
//  This is a small test class that is used as the element type for our hash map.
//  To keep it small for this sample, it just uses public members. The hash map
//  requires equality operators for its elements.
// ---------------------------------------------------------------------------
class TNameInfo
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructors
        // -------------------------------------------------------------------
        TNameInfo() = delete;
        TNameInfo(const TNameInfo&) = default;
        TNameInfo(const TString& strName) : m_strName(strName) { }
        ~TNameInfo() {}


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TBoolean operator==(const TNameInfo& nmiSrc) const
        {
            return((m_c4Counter == nmiSrc.m_c4Counter) &&  (m_strName == nmiSrc.m_strName));
        }

        tCIDLib::TBoolean operator!=(const TNameInfo& nmiSrc) const
        {
            return !operator==(nmiSrc);
        }


        // -------------------------------------------------------------------
        //  Data members
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4Counter = 1;
        TString             m_strName;
};


// ---------------------------------------------------------------------------
//  Create an alias for our hash map of TNameInfo object, with string
//  keys, and string key ops.
// ---------------------------------------------------------------------------
using TNameInfoList = TKeyedHashSet<TNameInfo,TString,TStringKeyOps>;


// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------

// This is the the thread function for the main thread.
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    //
    //  Create a hash map of name info objects., with a modulus of 11. We use a
    //  lambda to provide the required key extraction callback. We get a name info
    //  object and return a ref to the name. We tell the key ops guy to be non-
    //  case sensitive.
    //
    TNameInfoList colOfNames
    (
        11
        , new TStringKeyOps(kCIDLib::False)
        , [](const TNameInfo& nmiCur) -> const TString& { return nmiCur.m_strName; }
    );

    //
    //  Loop through our list of names and create a name info object
    //  for each one we find. We test each one to see if its there, and
    //  add it if not. If it is already there, we bump its counter.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4NameCount; c4Index++)
    {
        // Find the existing element or add it if not already there
        tCIDLib::TBoolean bAdded;
        TNameInfo& nmiCur = colOfNames.objFindOrAdd(TString(apszNames[c4Index]), bAdded);

        // If we didn't just add it, then increment its counter
        if (!bAdded)
            nmiCur.m_c4Counter++;
    }

    //
    //  Set up two stream format objects. One represents the name column
    //  and one represents the count column. We use these to control the
    //  output of each column.
    //
    TStreamFmt strmfCount(8, 0, tCIDLib::EHJustify::Right, kCIDLib::chSpace);
    TStreamFmt strmfName(48, 0, tCIDLib::EHJustify::Left, kCIDLib::chSpace);

    //
    //  Do a header line for the data we are going to dump out. Note
    //  that the TTextOutStream::Spaces() thingie outputs 4 spaces, which
    //  are not affected by the formatting. I.e. it will just output
    //  4 spaces, regardless of the set field width. We use this to
    //  space the columns apart. The same applies to the NewLn values
    //  which are not affected by formatting.
    //
    //  ESpaces() is built upon the more generic TTextOutStream::RepChars
    //  mechanism, which is a way to output a repeated sequence of
    //  characters.
    //
    const tCIDLib::TCard4 c4Spacing = 4;
    conOut  << kCIDLib::NewLn << strmfCount << L"Count"
            << TTextOutStream::Spaces(c4Spacing)
            << strmfName << L"Name" << kCIDLib::NewLn
            << strmfCount << L"------"
            << TTextOutStream::Spaces(c4Spacing)
            << strmfName << L"---------------------"
            << kCIDLib::EndLn;

    //
    //  Now iterate all the elements and dump their info out. Note that
    //  the Spaces() type is not constrained by the stream formatting stuff
    //  so it can be used to put spaces into columnar stuff conveniently.
    //
    TNameInfoList::TCursor cursNames(&colOfNames);
    for (; cursNames; ++cursNames)
    {
        conOut  << strmfCount << cursNames->m_c4Counter
                << TTextOutStream::Spaces(c4Spacing)
                << strmfName << cursNames->m_strName
                << kCIDLib::EndLn;
    }
    return tCIDLib::EExitCodes::Normal;
}
