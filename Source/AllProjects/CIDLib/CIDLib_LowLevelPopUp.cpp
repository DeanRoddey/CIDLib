//
// FILE NAME: CIDLib_LowLevelPopUp.cpp
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
//  This file implements the low level emergency popup services.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  TPopUp methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TPopUp::PopUpMsg(const  tCIDLib::TCh* const pszFileName
                , const tCIDLib::TCard4     c4LineNum
                , const tCIDLib::TCh* const pszTitle1
                , const tCIDLib::TCh* const pszTitle2
                , const tCIDLib::TCh* const pszMsg
                , const tCIDLib::TCh* const pszMsg2)
{
    TKrnlPopup::Show
    (
        pszFileName
        , c4LineNum
        , pszTitle1
        , pszTitle2
        , 0
        , 0
        , 0
        , pszMsg
        , pszMsg2
    );
}

tCIDLib::TVoid
TPopUp::PopUpMsg(const  TString&        strFileName
                , const tCIDLib::TCard4 c4LineNum
                , const TString&        strTitle1
                , const TString&        strTitle2
                , const TString&        strMsg
                , const TString&        strMsg2)
{
    TKrnlPopup::Show
    (
        strFileName.pszBuffer()
        , c4LineNum
        , strTitle1.pszBuffer()
        , strTitle2.pszBuffer()
        , 0
        , 0
        , 0
        , strMsg.pszBuffer()
        , strMsg2.pszBuffer()
    );
}


tCIDLib::TVoid
TPopUp::PopUpErr(const  tCIDLib::TCh* const pszFileName
                , const tCIDLib::TCard4     c4LineNum
                , const tCIDLib::TCh* const pszTitle1
                , const tCIDLib::TCh* const pszTitle2
                , const tCIDLib::TErrCode   errcId
                , const tCIDLib::TErrCode   errcKrnlId
                , const tCIDLib::TOSErrCode errcHostId
                , const tCIDLib::TCh* const pszMsg
                , const tCIDLib::TCh* const pszMsg2)
{
    TKrnlPopup::Show
    (
        pszFileName
        , c4LineNum
        , pszTitle1
        , pszTitle2
        , errcId
        , errcKrnlId
        , errcHostId
        , pszMsg
        , pszMsg2
    );
}

tCIDLib::TVoid
TPopUp::PopUpErr(const  TString&            strFileName
                , const tCIDLib::TCard4     c4LineNum
                , const TString&            strTitle1
                , const TString&            strTitle2
                , const tCIDLib::TErrCode   errcId
                , const tCIDLib::TErrCode   errcKrnlId
                , const tCIDLib::TOSErrCode errcHostId
                , const TString&            strMsg
                , const TString&            strMsg2)
{
    TKrnlPopup::Show
    (
        strFileName.pszBuffer()
        , c4LineNum
        , strTitle1.pszBuffer()
        , strTitle2.pszBuffer()
        , errcId
        , errcKrnlId
        , errcHostId
        , strMsg.pszBuffer()
        , strMsg2.pszBuffer()
    );
}
