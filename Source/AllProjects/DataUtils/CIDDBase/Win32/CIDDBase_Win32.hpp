//
// FILE NAME: CIDDBase_Win32.hpp
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
//  This header is for us to share stuff amongst our Win32 specific cpp files. We
//  need to share some of the internal opaque per-platform structures in
//  particular.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Bring in the internal system headers we need
// ---------------------------------------------------------------------------
#include    <windows.h>
#include    <Sql.h>
#include    <Sqlext.h>
#include    <Sqltypes.h>
#include    <Sqlucode.h>

#include    <Odbcss.h>


#pragma CIDLIB_PACK(CIDLIBPACK)

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


// Define our version of the overall database environment handle structure
struct tCIDDBase::TDBEnvHandle
{
    SQLHANDLE   hEnv;
};


// Define our version of the public opque connection handle structure
struct tCIDDBase::TConnHandle
{
    SQLHANDLE   hConn;
};


// Define out verion of the per-platform statement handle structure
struct tCIDDBase::TStmtHandle
{
    SQLHANDLE   hStatement;
};

#pragma CIDLIB_POPPACK
