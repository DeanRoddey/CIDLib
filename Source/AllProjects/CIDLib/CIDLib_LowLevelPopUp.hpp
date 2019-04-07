//
// FILE NAME: CIDLib_LowLevelPopUp.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/26/1996
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
//  This is the header file for the CIDLib_LowLevelPopup.Cpp file. This file
//  provides the mechanism to do emergency popups.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


namespace TPopUp
{
    CIDLIBEXP tCIDLib::TVoid PopUpMsg
    (
        const   tCIDLib::TCh* const     pszFileName
        , const tCIDLib::TCard4         c4LineNum
        , const tCIDLib::TCh* const     pszTitle1
        , const tCIDLib::TCh* const     pszTitle2
        , const tCIDLib::TCh* const     pszMsg
        , const tCIDLib::TCh* const     pszMsg2 = 0
    );

    CIDLIBEXP tCIDLib::TVoid PopUpMsg
    (
        const   TString&                strFileName
        , const tCIDLib::TCard4         c4LineNum
        , const TString&                strTitle1
        , const TString&                strTitle2
        , const TString&                strMsg
        , const TString&                strMsg2 = TString::Nul_TString()
    );

    CIDLIBEXP tCIDLib::TVoid PopUpErr
    (
        const   tCIDLib::TCh* const     pszFileName
        , const tCIDLib::TCard4         c4LineNum
        , const tCIDLib::TCh* const     pszTitle1
        , const tCIDLib::TCh* const     pszTitle2
        , const tCIDLib::TErrCode       errcId
        , const tCIDLib::TErrCode       errcKrnlId
        , const tCIDLib::TOSErrCode     errcHostId
        , const tCIDLib::TCh* const     pszMsg
        , const tCIDLib::TCh* const     pszMsg2
    );

    CIDLIBEXP tCIDLib::TVoid PopUpErr
    (
        const   TString&                strFileName
        , const tCIDLib::TCard4         c4LineNum
        , const TString&                strTitle1
        , const TString&                strTitle2
        , const tCIDLib::TErrCode       errcId
        , const tCIDLib::TErrCode       errcKrnlId
        , const tCIDLib::TOSErrCode     errcHostId
        , const TString&                strMsg
        , const TString&                strMsg2 = TString::Nul_TString()
    );
}
