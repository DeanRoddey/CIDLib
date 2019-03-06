//
// FILE NAME: CIDKernel_LowLevelPopUp_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 12/25/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file provides the Linux specific implementation for the TKrnlPopup
//  namespace.
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



// ---------------------------------------------------------------------------
//   CLASS: TKrnlPopup
//  PREFIX: kpu
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlPopup: Public, static methods
// ---------------------------------------------------------------------------
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
    tCIDLib::TSCh szBuf1[256 + 1];
    tCIDLib::TSCh szBuf2[256 + 1];

    TRawStr::pszConvert(pszTitle1, szBuf1, c4MaxBufChars(szBuf1));
    TRawStr::pszConvert(pszTitle2, szBuf2, c4MaxBufChars(szBuf2));
    ::fprintf(stderr, "CIDLib Kernel Runtime Error:\n%s\n%s\n", szBuf1, szBuf2);

    TRawStr::pszConvert(TKrnlSysInfo::pszProcessName(), szBuf1, c4MaxBufChars(szBuf1));
    ::fprintf(stderr, " Application: %s\n", szBuf1);

    const tCIDLib::TCh* pszTmp = TRawStr::pszFindLastChar(pszFileName, kCIDLib::chPathSeparator);
    if (!pszTmp)
        TRawStr::pszConvert(pszFileName, szBuf1, c4MaxBufChars(szBuf1));
    else
        TRawStr::pszConvert(pszTmp + 1, szBuf1, c4MaxBufChars(szBuf1));

    ::fprintf(stderr, " Module/Line: %s.%u\n", szBuf1, c4LineNum);
    ::fprintf(stderr, "CIDLib Error: %lu\n", errcId);
    ::fprintf(stderr, "Kernel Error: %lu\n", errcKrnlId);
    ::fprintf(stderr, "  Host Error: %lu\n", errcHostId);

    TRawStr::pszConvert(pszMsg, szBuf1, c4MaxBufChars(szBuf1));
    TRawStr::pszConvert(pszMsg2, szBuf2, c4MaxBufChars(szBuf2));
    ::fprintf(stderr, "%s\n%s\n", szBuf1, szBuf2);
}
