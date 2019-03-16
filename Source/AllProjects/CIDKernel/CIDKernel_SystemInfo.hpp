//
// FILE NAME: CIDKernel_SystemInfo.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/06/1996
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the header for the CIDKernel_SystemInfo.Cpp file. This file
//  implements the TKrnlSysInfo namespace, which encapsulates the retrieval
//  of some general purpose system information.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


namespace TKrnlSysInfo
{
    // A simple structure used by the QuerySysId() method below
    const tCIDLib::TCard4 c4SysIdCnt = 6;
    struct TSysId
    {
        tCIDLib::TCard4 ac4Vals[c4SysIdCnt];
    };


    KRNLEXPORT tCIDLib::TBoolean bCmdLineArg
    (
        const   tCIDLib::TCard4         c4Index
        , const tCIDLib::TCh*&          pszToFill
    );

    KRNLEXPORT tCIDLib::TBoolean bIsHostAdmin();

    KRNLEXPORT tCIDLib::TBoolean bQueryAvailPhysicalMem
    (
                tCIDLib::TCard8&        c8ToFill
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryMachineID
    (
                tCIDLib::TCh* const     pchBuffer
        , const tCIDLib::TCard4         c4Bytes
    );

    KRNLEXPORT tCIDLib::TBoolean bQuerySpecialPath
    (
                tCIDLib::TCh* const     pszBuffer
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::ESpecialPaths  ePath
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryUserName
    (
                tCIDLib::TCh* const     pszBuffer
        , const tCIDLib::TCard4         c4MaxChars
    );

    KRNLEXPORT tCIDLib::TBoolean bRemoveCmdLineArg
    (
        const   tCIDLib::TCard4         c4Index
    );

    KRNLEXPORT tCIDLib::TCard4 c4CheckSysChange
    (
        const   TSysId&                 sidFirst
        , const TSysId&                 sidSec
    );

    KRNLEXPORT tCIDLib::TCard4 c4CmdLineArgCount();

    KRNLEXPORT tCIDLib::TCard4 c4CPUCount();

    KRNLEXPORT tCIDLib::TCard4 c4SSELevel();

    KRNLEXPORT tCIDLib::TCard8 c8TotalPhysicalMem();

    KRNLEXPORT tCIDLib::ECPUTypes eCPUType();

    KRNLEXPORT const tCIDLib::TCh* pszNodeName();

    KRNLEXPORT const tCIDLib::TCh* pszProcessName();

    KRNLEXPORT tCIDLib::TVoid QueryOSInfo
    (
                tCIDLib::TCard4&        c4OSMajVersion
        ,       tCIDLib::TCard4&        c4OSMinVersion
        ,       tCIDLib::TCard4&        c4OSRev
        ,       tCIDLib::TCard4&        c4OSBuildNum
    );

    KRNLEXPORT tCIDLib::TVoid QuerySysId
    (
                TSysId&                 sidToFill
    );

    KRNLEXPORT tCIDLib::TVoid QueryVirtualScrRectl
    (
                tCIDLib::THostRectl&    rectToFill
    );

    // For internal use
    KRNLEXPORT tCIDLib::TVoid OverrideCmdLine
    (
        const   tCIDLib::TCard4         c4Count
        ,       tCIDLib::TCh**          apszParms
    );
}

