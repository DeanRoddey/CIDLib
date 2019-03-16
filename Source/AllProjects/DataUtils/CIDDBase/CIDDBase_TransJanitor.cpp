//
// FILE NAME: CIDDBase_TransJanitor.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/23/2003
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
//  This file implements a scoped SQL Transaction class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDDBase_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TDBTransJan,TObject)


// ---------------------------------------------------------------------------
//  CLASS: TDBTransJan
// PREFIX: jan
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBTransJan: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBTransJan::TDBTransJan(       TDBConnection* const    pdbconToUse
                        , const TString&                strName) :

    m_bCommited(kCIDLib::False)
    , m_pdbconToUse(pdbconToUse)
    , m_strName(strName)
{
    // Start a transaction
    try
    {
        TDBDirStatement dbstmtBegin(*m_pdbconToUse, m_strName);
        dbstmtBegin.Execute(L"BEGIN TRANSACTION");
    }

    catch(const TError& errToCatch)
    {
        if (facCIDDBase().bShouldLog(errToCatch))
            TModule::LogEventObj(errToCatch);

        // Can't do much but complain to the logs
        if (facCIDDBase().bLogFailures())
        {
            facCIDDBase().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kDBErrs::errcStmt_StartTrans
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_strName
            );
        }
    }
}

TDBTransJan::~TDBTransJan()
{
    // If we didn't commit, then issue a rollback
    if (!m_bCommited)
    {
        try
        {
            TDBDirStatement dbstmtCommit(*m_pdbconToUse, m_strName);
            dbstmtCommit.Execute(L"ROLLBACK TRANSACTION");
        }

        catch(const TError& errToCatch)
        {
            if (facCIDDBase().bShouldLog(errToCatch))
                TModule::LogEventObj(errToCatch);

            // Can't do much but complain to the logs
            if (facCIDDBase().bLogFailures())
            {
                facCIDDBase().LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kDBErrs::errcStmt_RollbackFailed
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                    , m_strName
                );
            }
        }
    }
}


// ---------------------------------------------------------------------------
//  TDBTransJan: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TDBTransJan::Commit()
{
    // Issue a commit
    try
    {
        TDBDirStatement dbstmtCommit(*m_pdbconToUse, m_strName);
        dbstmtCommit.Execute(L"COMMIT TRANSACTION");

        // It worked so set our flag
        m_bCommited = kCIDLib::True;
    }

    catch(TError& errToCatch)
    {
        if (facCIDDBase().bShouldLog(errToCatch))
            TModule::LogEventObj(errToCatch);

        if (facCIDDBase().bLogFailures())
        {
            facCIDDBase().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kDBErrs::errcStmt_CommitFailed
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_strName
            );
        }

        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}

