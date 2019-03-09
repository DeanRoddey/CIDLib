//
// FILE NAME: TestCIDLib_Pointers.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/06/1993
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
//  This module is part of the TestCIDLib.Exe program and is called from the
//  program's main() function. The functions in this module test the
//  pointer wrapper classes.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// -----------------------------------------------------------------------------
//  Facility specific includes
// -----------------------------------------------------------------------------
#include    "TestCIDLib.hpp"



// -----------------------------------------------------------------------------
//  CLASS: TTester
//
//  This class is used to test the counted pointer template. It lets us know
//  for sure that it got deleted properly by setting a flag.
// -----------------------------------------------------------------------------
class TTester : public TObject
{
    public  :
        TTester(tCIDLib::TBoolean& bGotDeleted) :

            m_bGotDeleted(bGotDeleted)
        {
        }

        ~TTester()
        {
            m_bGotDeleted = kCIDLib::True;
        }

        tCIDLib::TBoolean&  m_bGotDeleted;
        tCIDLib::TInt4      m_i4Field1;
        tCIDLib::TInt4      m_i4Field2;

        RTTIDefs(TTester,TObject)
};
RTTIDecls(TTester,TObject)


// -----------------------------------------------------------------------------
//  Local functions
// -----------------------------------------------------------------------------
static tCIDLib::TVoid TestBasicCountedPointer(TTextOutStream& strmOut)
{
    // A boolean for the tester to set
    tCIDLib::TBoolean   bGotDeleted = kCIDLib::False;

    // Create a counted pointer for a TTester object
    {
        TCntPtr<TTester> cptrTest(new TTester(bGotDeleted));

        // Make sure that we can access the point members via the operators
        cptrTest->m_i4Field1 = 1;
        cptrTest->m_i4Field2 = 2;
    }

    // Make sure it got deleted
    if (!bGotDeleted)
        strmOut << CUR_LN << L"Test pointer did not get deleted" << kCIDLib::EndLn;

    //
    //  Now do a similar test, but allocate and delete a lot of copies of
    //  the counted pointer object, to make sure that the counting is
    //  working correctly.
    //
    {
        const tCIDLib::TCard4 c4MaxCopies = 63;

        bGotDeleted = kCIDLib::False;
        TCntPtr<TTester> cptrTest(new TTester(bGotDeleted));
        TCntPtr<TTester> acptrCopies[c4MaxCopies];

        tCIDLib::TCard4 c4Index;
        for (c4Index = 0; c4Index < c4MaxCopies; c4Index++)
            acptrCopies[c4Index] = cptrTest;

        // Should not have been deleted yet
        if (bGotDeleted)
            strmOut << CUR_LN << L"Test pointer was already deleted" << kCIDLib::EndLn;

        // The count should be 64 now
        if (cptrTest.c4RefCount() != 64)
        {
            strmOut << CUR_LN << L"Ref count was " << cptrTest.c4RefCount()
                    << L" instead of 64" << kCIDLib::EndLn;
        }

        // Now make the copies drop their references
        for (c4Index = 0; c4Index < c4MaxCopies; c4Index++)
            acptrCopies[c4Index].DropRef();

        // Still should not be deleted
        if (bGotDeleted)
            strmOut << CUR_LN << L"Test pointer was already deleted" << kCIDLib::EndLn;

        // Count should now be 1 again
        if (cptrTest.c4RefCount() != 1)
        {
            strmOut << CUR_LN << L"Ref count was " << cptrTest.c4RefCount()
                    << L" instead of 1" << kCIDLib::EndLn;
        }
    }

    // Make sure it got deleted now that last one went out of scope
    if (!bGotDeleted)
        strmOut << CUR_LN << L"Test pointer did not get deleted" << kCIDLib::EndLn;
}


// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// -----------------------------------------------------------------------------

//
//  This method tests out the pointer wrapper classes.
//
tCIDLib::TVoid TFacTestCIDLib::TestPointerClasses()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        pszCurTest = L"basic counted pointer tests";
        TestBasicCountedPointer(strmOut());
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        strmOut()   << L"Exception occured during the " << pszCurTest
                    << L" test" << kCIDLib::EndLn;
        throw;
    }
}
