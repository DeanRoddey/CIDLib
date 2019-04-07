//
// FILE NAME: CIDDBase_Statement_Win32.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/01/2019
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
//  This file implements the Win32 specific versions of a SQL statement. We do
//  both regular and direct statements here since it's not much per.
//
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
#include    "CIDDBase_Win32.hpp"



// ---------------------------------------------------------------------------
//  CLASS: TDBStatement
// PREFIX: dbstmt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBStatement: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TDBStatement::bDoPlatFetch()
{
    const SQLRETURN RetVal = ::SQLFetch(m_pStatement->hStatement);
    if (RetVal == SQL_NO_DATA)
        return kCIDLib::False;

    if ((RetVal != SQL_SUCCESS_WITH_INFO) && (RetVal != SQL_SUCCESS))
    {
        TString strInfo;
        TKrnlError kerrThrow = kerrcXlatDBError
        (
            SQL_HANDLE_STMT, m_pStatement->hStatement, RetVal, strInfo
        );

        facCIDDBase().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kDBErrs::errcStmt_Fetch
            , kerrThrow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
            , strInfo
        );
    }
    return kCIDLib::True;
}


tCIDLib::TVoid
TDBStatement::CreateStatement(const tCIDDBase::TConnHandle* pConnect)
{
    SQLHANDLE hStatement = SQL_NULL_HANDLE;
    const SQLRETURN RetVal = ::SQLAllocHandle
    (
        SQL_HANDLE_STMT, pConnect->hConn, &hStatement
    );

    if ((RetVal != SQL_SUCCESS_WITH_INFO) && (RetVal != SQL_SUCCESS))
    {
        TString strInfo;
        TKrnlError kerrThrow = kerrcXlatDBError
        (
          SQL_HANDLE_DBC, pConnect->hConn, RetVal, strInfo
        );

        facCIDDBase().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kDBErrs::errcStmt_Alloc
            , kerrThrow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
            , strInfo
        );
    }

    // It worked, so gen up a statement handle and store it
    m_pStatement = new tCIDDBase::TStmtHandle{0};
    m_pStatement->hStatement = hStatement;
}


tCIDLib::TVoid TDBStatement::DestroyStatement(const tCIDDBase::TConnHandle* pConnect)
{
    // Close the statement and clean up the statement handle if created
    if (m_pStatement)
    {
        if (m_pStatement->hStatement != SQL_NULL_HSTMT)
            ::SQLFreeHandle(SQL_HANDLE_STMT, m_pStatement->hStatement);

        delete m_pStatement;
        m_pStatement = nullptr;
    }
}


tCIDLib::TVoid TDBStatement::ClearCursor()
{
    // And clear any from the statement
    const SQLRETURN RetVal = ::SQLFreeStmt(m_pStatement->hStatement, SQL_CLOSE);

    if ((RetVal != SQL_SUCCESS_WITH_INFO) && (RetVal != SQL_SUCCESS))
    {
        TString strInfo;
        TKrnlError kerrThrow = kerrcXlatDBError
        (
            SQL_HANDLE_STMT, m_pStatement->hStatement, RetVal, strInfo
        );

        facCIDDBase().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kDBErrs::errcStmt_ClearCursor
            , kerrThrow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
            , strInfo
        );
    }
}


tCIDLib::TVoid TDBStatement::ClearHandleColBindings()
{
    // And clear any from the statement
    const SQLRETURN RetVal = ::SQLFreeStmt(m_pStatement->hStatement, SQL_UNBIND);
    if ((RetVal != SQL_SUCCESS_WITH_INFO) && (RetVal != SQL_SUCCESS))
    {
        TString strInfo;
        TKrnlError kerrThrow = kerrcXlatDBError
        (
            SQL_HANDLE_STMT, m_pStatement->hStatement, RetVal, strInfo
        );

        facCIDDBase().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kDBErrs::errcStmt_ClearColBindings
            , kerrThrow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
            , strInfo
        );
    }
}


tCIDLib::TVoid TDBStatement::ClearHandleParmBindings()
{
    // And clear any from the statement
    const SQLRETURN RetVal = ::SQLFreeStmt(m_pStatement->hStatement, SQL_RESET_PARAMS);
    if ((RetVal != SQL_SUCCESS_WITH_INFO) && (RetVal != SQL_SUCCESS))
    {
        TString strInfo;
        TKrnlError kerrThrow = kerrcXlatDBError
        (
            SQL_HANDLE_STMT, m_pStatement->hStatement, RetVal, strInfo
        );

        facCIDDBase().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kDBErrs::errcStmt_ClearParmBindings
            , kerrThrow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
            , strInfo
        );
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TDBDirStatement
// PREFIX: dbstmt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBDirStatement: Private, non-virtual methods
// ---------------------------------------------------------------------------

// We do the platform specific part of a direct SQL statement
tCIDLib::TVoid TDBDirStatement::DoPlatExecute(const TString& strStatement)
{
    //
    //  We have to cast off the const of the buffer, since this API doesn't
    //  support const'ness.
    //
    const SQLRETURN RetVal = ::SQLExecDirectW
    (
        pStatement()->hStatement
        , (SQLWCHAR*)strStatement.pszBuffer()
        , SQL_NTS
    );

    if ((RetVal != SQL_SUCCESS_WITH_INFO) && (RetVal != SQL_SUCCESS))
    {
        TString strInfo;
        TKrnlError kerrThrow = kerrcXlatDBError
        (
            SQL_HANDLE_STMT, pStatement()->hStatement, RetVal, strInfo
        );

        facCIDDBase().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kDBErrs::errcStmt_DirectFailed
            , kerrThrow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strName()
            , strInfo
        );
    }
}

