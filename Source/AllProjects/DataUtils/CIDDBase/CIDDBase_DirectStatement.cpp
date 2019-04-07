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
//  CLASS: TDBDirStatement
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

TDBDirStatement::TDBDirStatement( const   tCIDDBase::THConn&    hConnection
                                        , const TString&        strName) :
    TDBStatement(hConnection, strName)
{
}

TDBDirStatement::~TDBDirStatement()
{
}


// ---------------------------------------------------------------------------
//  TDBStatement: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  We ask the parent to set up any parameter bindings that have been set on us
//  then we ask the per-platform code to do the actual direct execution. Then we
//  ask the parent class to bind the column bindings to the returned results if
//  any.
//
tCIDLib::TVoid TDBDirStatement::Execute(const TString& strStatement)
{
    DoParmBindings();
    DoPlatExecute(strStatement);
    DoColBindings();
}
