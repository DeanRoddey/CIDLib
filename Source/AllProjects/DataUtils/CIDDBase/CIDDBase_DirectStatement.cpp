//
// FILE NAME: CIDDBase_DirectStatement.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/21/2003
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
//  This file implements the directy query statement class.
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
RTTIDecls(TDBDirStatement,TDBStatement)


// ---------------------------------------------------------------------------
//  CLASS: TDBStatement
// PREFIX: dbstmt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDBStatement: Constructors and Destructor
// ---------------------------------------------------------------------------
TDBDirStatement::TDBDirStatement( const   TDBConnection&    dbconTarget
                                        , const TString&    strName) :
    TDBStatement(dbconTarget, strName)
{
}

TDBDirStatement::~TDBDirStatement()
{
}


// ---------------------------------------------------------------------------
//  TDBStatement: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TDBDirStatement::Execute(const TString& strStatement)
{
    // Ask the parent class to do the parameter bindings
    DoParmBindings();

    //
    //  We have to cast off the const of the buffer, since this API doesn't
    //  support const'ness.
    //
    const SQLRETURN RetVal = ::SQLExecDirect
    (
        hStatement()
        , (SQLWCHAR*)strStatement.pszBuffer()
        , SQL_NTS
    );

    if ((RetVal != SQL_SUCCESS_WITH_INFO) && (RetVal != SQL_SUCCESS))
    {
        TString strInfo;
        TKrnlError kerrThrow = kerrcXlatDBError
        (
            SQL_HANDLE_STMT
            , hStatement()
            , RetVal
            , strInfo
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

    // It worked, so ask him to do the column bindings
    DoColBindings();
}


