//
// FILE NAME: CIDKernel_LowLevelPopUp.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/26/1996
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
//  This is the header for the CIDKernel_LowLevelPopUp.Cpp file. This
//  file implements the TKrnlPopUp namespace, which is a safe and simple
//  way to report errors that might occur at very low levels in the system
//  (and therefore potentially have little working infrastructure to call
//  upon.)
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  NAMESPACE: TKrnlPopup
// ---------------------------------------------------------------------------
namespace TKrnlPopup
{
    KRNLEXPORT tCIDLib::TVoid Show
    (
        const   tCIDLib::TCh* const     pszFileName
        , const tCIDLib::TCard4         c4LineNum
        , const tCIDLib::TCh* const     pszTitle1
        , const tCIDLib::TCh* const     pszTitle2
        , const tCIDLib::TErrCode       errcId
        , const tCIDLib::TErrCode       errcKrnlId
        , const tCIDLib::TOSErrCode     errcHostId
        , const tCIDLib::TCh* const     pszMsg
        , const tCIDLib::TCh* const     pszMsg2 = 0
    );
}

#pragma CIDLIB_POPPACK
