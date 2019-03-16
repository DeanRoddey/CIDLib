//
// FILE NAME: TestCIDDBase.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/19/2003
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
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
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDDBase.hpp"


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
    ,   tCIDLib::TVoid*     pData
);


// ---------------------------------------------------------------------------
//  Do the magic main module code
// ---------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"TestCIDDBase", eMainThreadFunc))


// ----------------------------------------------------------------------------
//  Local data
// ----------------------------------------------------------------------------
TTextOutStream&  strmOut = TSysInfo::strmOut();;



// ----------------------------------------------------------------------------
//  A tiny little class that allows me to easily format the current file and
//  line to the output stream.
// ----------------------------------------------------------------------------
class TCurLn
{
    public :
        TCurLn(   const   tCIDLib::TCh* const   pszFile
                  , const tCIDLib::TCard4       c4Line) :
            m_pszFile(0)
            , m_c4Line(c4Line)
        {
            m_pszFile = TRawStr::pszReplicate(pszFile);
        }

        ~TCurLn()
        {
            delete [] m_pszFile;
        }

        tCIDLib::TCh*   m_pszFile;
        tCIDLib::TCard4 m_c4Line;
};

TTextOutStream& operator<<(TTextOutStream& strmOut, const TCurLn& Cur)
{
    strmOut << L"(" << Cur.m_pszFile << L"." << Cur.m_c4Line << L" - ";
    return strmOut;
}

#define CUR_LN TCurLn(CID_FILE, CID_LINE)



// -----------------------------------------------------------------------------
//  Local functions
// -----------------------------------------------------------------------------
static tCIDLib::TVoid EnumSources()
{
    strmOut << L"\n\nAvailable SQL Sources\n"
              L"-------------------------------------\n";

    tCIDLib::TKVPList colToFill;
    facCIDDBase().QuerySources(colToFill, tCIDDBase::ESrcTypes::Both);

    // Get the count and if we have any, display them
    const tCIDLib::TCard4 c4Count = colToFill.c4ElemCount();

    if (c4Count)
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            const TKeyValuePair& kvalCur = colToFill[c4Index];

            strmOut  << kvalCur.strKey() << L"\n    - "
                    << kvalCur.strValue() << kCIDLib::NewLn;
        }
        strmOut << kCIDLib::NewLn;
    }
     else
    {
        strmOut << L"<None>\n\n";
    }

    strmOut.Flush();
}


tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    try
    {
        // Initialize the SQL environment
        facCIDDBase().Initialize();

        // Enumerate the available data sources and display them
        EnumSources();

        // Select a known data source and connect
        {
            TDBConnection dbconTest;
            dbconTest.Connect(L"CQSLCustSrv");

            // Create a direct statement
            TDBDirStatement dbstmtUpdate(dbconTest, L"Customer Query");

            //
            //  Add column bindings that we'll need, and execute a select
            //  statement.
            //
            dbstmtUpdate.Reset();
            dbstmtUpdate.AddColStrBinding(L"Name", 128);
            dbstmtUpdate.AddColDateBinding(L"Since", TTime::strMMDDYYYY());
            dbstmtUpdate.AddColBinding(L"Ranking", tCIDDBase::ECppTypes::Card2);
            dbstmtUpdate.AddColStrBinding(L"EMail", 128);
            dbstmtUpdate.AddColBinding(L"CustId", tCIDDBase::ECppTypes::Card8);

            dbstmtUpdate.Execute
            (
                L"SELECT Cust_Name,Cust_Since,Cust_Ranking,Cust_EMail,Cust_Num "
                L"FROM Customers"
            );

            tCIDLib::TCard4 c4Count = 0;
            tCIDLib::TCard8 c8MaxId = 0;
            while (dbstmtUpdate.bFetch())
            {
                const tCIDLib::TCard8 c8Id = dbstmtUpdate.c8ColVal(4);

                strmOut  << L"Name=" << dbstmtUpdate.strColVal(0)
                        << L", " << dbstmtUpdate.tmDateColVal(1)
                        << L", Rank=" << dbstmtUpdate.c2ColVal(2)
                        << L", Email=" << dbstmtUpdate.strColVal(3)
                        << L", Id=" << c8Id
                        << kCIDLib::NewLn;
                c4Count++;

                if (c8Id > c8MaxId)
                    c8MaxId = c8Id;
            }

            strmOut  << L"\nList complete. " << c4Count
                    << L" records returned. Max Id=" << c8MaxId
                    << kCIDLib::EndLn;
            strmOut.Flush();

            //
            //  It should be ok to disconnect before we clean up statements
            //  because the connection handle is actually referenced counted.
            //
            dbconTest.Disconnect();

            // Reset the statements and unselect the database
            dbstmtUpdate.Reset();
            dbstmtUpdate.Execute(L"USE MASTER");
        }

        // Clean up the SQL system
        facCIDDBase().Terminate();

        strmOut << L"Completed sucessfully" << kCIDLib::EndLn;
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
        strmOut  << L"A CIDLib runtime error occured during processing. Error:\n"
                << errToCatch << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but test for
    //  them here just in case.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        strmOut  << L"A kernel error occured during processing.\nError="
                << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        strmOut  << L"A general exception occured during processing"
                << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }
    return tCIDLib::EExitCodes::Normal;
}


