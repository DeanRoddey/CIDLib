//
// FILE NAME: CIDDBase_.hpp
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
//  This is the main internal header for the facility. It includes the main
//  public header, any internal headers that are needed, intrafacility function
//  prototypes, intrafacility constants, and macros.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Include our own public header and any internal headers we need
// ---------------------------------------------------------------------------
#include    "CIDDBase.hpp"


// ---------------------------------------------------------------------------
//  Bring in any internal headers
// ---------------------------------------------------------------------------
#include    "CIDDBase_MessageIds.hpp"


// ---------------------------------------------------------------------------
//  Bring in the internal system headers we need
// ---------------------------------------------------------------------------
#include    <windows.h>
#include    <Sql.h>
#include    <Sqlext.h>
#include    <Sqltypes.h>
#include    <Sqlucode.h>

#include    <Odbcss.h>


// ---------------------------------------------------------------------------
//  Some internal globals that use SQL specific types
// ---------------------------------------------------------------------------
extern TKrnlError kerrcXlatDBError
(
    const   SQLSMALLINT siType
    ,       SQLHANDLE   hToCheck
    , const SQLRETURN   resToXlat
    ,       TString&    strInfo
);


