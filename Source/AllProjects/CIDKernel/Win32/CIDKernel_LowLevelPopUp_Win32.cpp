//
// FILE NAME: CIDKernel_LowLevelPopUp_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/12/1998
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
//  This file provides the Win32 specific implementation for the TKrnlPopup
//  namespace. We just use a message box. It's a little tight for the amount
//  of info, but doing a full dialog would be messy.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"
#include    "CIDKernel_InternalHelpers_.hpp"


// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------

//
//  A simple function to write out a string of text to the low level log file
//  if we are in a service and cannot show the popop.
//
static tCIDLib::TVoid LogText(          HANDLE                  hFile
                                , const tCIDLib::TCh* const     pszText
                                ,       tCIDLib::TSCh* const    pszTmp
                                , const tCIDLib::TCard4         c4TmpSz
                                , const tCIDLib::TBoolean       bNewLine)
{
    if (!pszText)
        return;

    // Use the temp buffer to transcode to ASCII
    TRawStr::pszConvert(pszText, pszTmp, c4TmpSz);
    tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(pszTmp);

    // Add the new line if asked
    if (bNewLine)
    {
        *(pszTmp + c4Len) = 0xA;
        c4Len++;
    }

    // And write this to the file
    tCIDLib::TCard4 c4Written;
    ::WriteFile(hFile, pszTmp, c4Len, &c4Written, 0);
}

static tCIDLib::TVoid LogText(          HANDLE                  hFile
                                ,       tCIDLib::TSCh* const    pszText
                                , const tCIDLib::TBoolean       bNewLine)
{
    if (!pszText)
        return;

    tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(pszText);
    tCIDLib::TCard4 c4Written;
    ::WriteFile(hFile, pszText, c4Len, &c4Written, 0);

    if (bNewLine)
    {
        tCIDLib::TCard1 c1NL = 0xA;
        ::WriteFile(hFile, &c1NL, 1, &c4Written, 0);
    }
}


// ---------------------------------------------------------------------------
//  NAMESPACE: TKrnlPopup
// ---------------------------------------------------------------------------

// We have to be very conservative here and use the lowest level stuff
tCIDLib::TVoid TKrnlPopup::Show(const   tCIDLib::TCh* const pszFileName
                                , const tCIDLib::TCard4     c4LineNum
                                , const tCIDLib::TCh* const pszTitle1
                                , const tCIDLib::TCh* const pszTitle2
                                , const tCIDLib::TErrCode   errcId
                                , const tCIDLib::TErrCode   errcKrnlId
                                , const tCIDLib::TOSErrCode errcHostId
                                , const tCIDLib::TCh* const pszMsg
                                , const tCIDLib::TCh* const pszMsg2)
{
    constexpr tCIDLib::TCard4 c4BufLen = 4095;
    tCIDLib::TCh achBuffer[c4BufLen + 1];
    achBuffer[0] = kCIDLib::chNull;

    constexpr tCIDLib::TCard4 c4TmpBufLen = 63;
    tCIDLib::TCh achTmpBuf[c4TmpBufLen + 1];
    achTmpBuf[0] = kCIDLib::chNull;

    const tCIDLib::TCh* achNL(L"\r\n");

    TRawStr::CopyStr(achBuffer, pszFileName, c4BufLen);
    TRawStr::CatStr(achBuffer, L".", c4BufLen);
    TRawStr::bFormatVal(c4LineNum, achTmpBuf, c4TmpBufLen);
    TRawStr::CatStr(achBuffer, achTmpBuf, c4BufLen);
    TRawStr::CatStr(achBuffer, achNL, c4BufLen);

    if (pszMsg2)
        TRawStr::CopyCatStr(achBuffer, c4BufLen, pszMsg2, achNL);

    TRawStr::bFormatVal(errcId, achTmpBuf, c4TmpBufLen);
    TRawStr::CopyCatStr(achBuffer, c4BufLen, achTmpBuf, L"/");
    TRawStr::bFormatVal(errcKrnlId, achTmpBuf, c4TmpBufLen);
    TRawStr::CopyCatStr(achBuffer, c4BufLen, achTmpBuf, L"/");
    TRawStr::bFormatVal(errcHostId, achTmpBuf, c4TmpBufLen);
    TRawStr::CatStr(achBuffer, achNL, c4BufLen);


    UINT uType = MB_OK | MB_ICONERROR | MB_APPLMODAL;
    ::MessageBox(0, achBuffer, L"Unhandled Error", uType);
}

