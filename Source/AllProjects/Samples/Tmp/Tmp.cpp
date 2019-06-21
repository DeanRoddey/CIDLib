// A junk projects for doing temporary tests or code for articles and such
#include "CIDLib.hpp"

tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"Collect1MainThread", eMainThreadFunc))
TOutConsole  conOut;


//
// For compactness below. A base collection class of strings, a
// vector of strings, and a hash set of strings (the latter two
// and all other collections derive from the base class). And a
// unique pointer to a base cursor class over strings.
//
using TBaseList = TCollection<TString>;
using TVec = TVector<TString>;
using THSet = THashSet<TString, TStringKeyOps>;
using TPolyCursPtr = TUniquePtr<TColCursor<TString>>;

//
// Polymorphic load of any collection of strings via base TCollection
// interface.
//
static tCIDLib::TVoid
LoadCollection(TBaseList& colToload, const tCIDLib::TCard4 c4Count)
{
    TString strTmp(L"Value #");
    const tCIDLib::TCard4 c4BaseLen = strTmp.c4Length();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        strTmp.CapAt(c4BaseLen);
        strTmp.AppendFormatted(c4Index + 1);
        colToload.objAdd(strTmp);
    }
}


//
//  Non polymorphic find (vector specific), using a returned cursor
//  by value as a failure indicator, and pointing to the found value
//  if successful.
//
static TVec::TCursor cursFind(TVec& colSrc, const TString& strToFind)
{
    TVec::TCursor cursFind(&colSrc);
    for (; cursFind; ++cursFind)
    {
        if (*cursFind == strToFind)
            break;
    }
    return cursFind;
}


//
//  A polymorphic find. In this case we have to return a wrapped cursor
//  because it has to be allocated.
//
static TPolyCursPtr
uptrPolyFind(TBaseList& colSrc, const TString& strToFind)
{
    // Get a cursor from the collection and put it into a unique pointer
    TPolyCursPtr uptrCursor(colSrc.pcursNew());

    // Get a ref for convenient syntax, then same as loop above
    TColCursor<TString>& cursFind = *uptrCursor;
    for (; cursFind; ++cursFind)
    {
        if (*cursFind == strToFind)
            break;
    }
    return uptrCursor;
}


tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    //
    //  Create a vector of strings and a hash set of strings. Use
    //  polymorphic loader above to load 4 strings each.
    //
    TVec colStrList;
    THSet colStrSet(19, TStringKeyOps());
    LoadCollection(colStrList, 4);
    LoadCollection(colStrSet, 4);

    //
    // Do the non-polymorphic find, which just returns a cursor directly.
    // Note the cursor validity semantics, which makes them useful for
    // this type of thing.
    //
    {
        TVec::TCursor cursFound = cursFind(colStrList, L"Value #3");
        if (cursFound)
            conOut << L"Found the value: " << *cursFound << kCIDLib::NewLn;

        // As a side note, remove it via the cursor and it remains valid
        colStrList.RemoveAt(cursFound);
        if (cursFound)
            conOut << L"Cursor now points to: " << *cursFound << L"\n";

        // Modify the collection another way and it knows it's not valid
        colStrList.RemoveAt(0);
        if (!cursFound)
            conOut << L"Cursor is no longer valid\n";

        conOut << kCIDLib::EndLn;
    }

    // Polymorphic search of both vector and hash set
    {
        // We get back unique pointers to the polymorphic cursors
        TPolyCursPtr uptrVec = uptrPolyFind(colStrList, L"Value #2");
        TPolyCursPtr uptrSet = uptrPolyFind(colStrSet, L"Value #2");

        // Deref the smart pointers to get to the cursors
        if (*uptrVec)
            conOut << L"Found item in vector\n";
        if (*uptrSet)
            conOut << L"Found item in hash set\n";

        conOut << kCIDLib::EndLn;
    }

    return tCIDLib::EExitCodes::Normal;
}