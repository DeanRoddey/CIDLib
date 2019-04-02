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
//  This file implements a scoped transaction janitor class.
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
TDBTransJan::TDBTransJan(TDBConnection& dbconnTar, const TString& strName) :

    m_bCommited(kCIDLib::False)
    , m_hConnection(dbconnTar.hConnection())
    , m_strName(strName)
{
    StartTrans();
}

TDBTransJan::TDBTransJan(tCIDDBase::THConn& hConnection, const TString& strName) :

    m_bCommited(kCIDLib::False)
    , m_hConnection(hConnection)
    , m_strName(strName)
{
    StartTrans();
}

TDBTransJan::~TDBTransJan()
{
    // If we didn't commit, then issue a rollback
    if (!m_bCommited)
    {
        try
        {
            TDBDirStatement dbstmtCommit(m_hConnection, m_strName);
            dbstmtCommit.Execute(L"ROLLBACK TRANSACTION");
        }

        catch(TError& errToCatch)
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

            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
}


// ---------------------------------------------------------------------------
//  TDBTransJan: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TDBTransJan::Commit()
{
    try
    {
        TDBDirStatement dbstmtCommit(m_hConnection, m_strName);
        dbstmtCommit.Execute(L"COMMIT TRANSACTION");
        m_bCommited = kCIDLib::True;
    }

    catch(TError& errToCatch)
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

        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}


// ---------------------------------------------------------------------------
//  TDBTransJan: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  WE have to do this from multiple ctors, so break it out. If anything goes wrong
//  it throw.
//
tCIDLib::TVoid TDBTransJan::StartTrans()
{
    // Start a transaction
    try
    {
        TDBDirStatement dbstmtBegin(m_hConnection, m_strName);
        dbstmtBegin.Execute(L"BEGIN TRANSACTION");
    }

    catch(TError& errToCatch)
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

        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}
