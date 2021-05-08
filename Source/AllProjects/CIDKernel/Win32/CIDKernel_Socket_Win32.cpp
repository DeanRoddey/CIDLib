//
// FILE NAME: CIDKernel_Socket_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/07/1998
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
//  This file provides the Win32 specific implementation of the TKrnlSocket
//  class, which provides the  host abstraction for socket services.
//
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

#pragma     warning(push)
#include    <CodeAnalysis\Warnings.h>
#pragma     warning(disable : ALL_CODE_ANALYSIS_WARNINGS 26812 26814)
#include    <winhttp.h>
#pragma     warning(pop)



namespace CIDKernel_Socket_Win32
{
    // -----------------------------------------------------------------------
    //  Socket constants
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TCard4   hInvalid = tCIDLib::TCard4(-1);


    // -----------------------------------------------------------------------
    //  A mapping array to map from our socket protocol enum to the actual
    //  system value for that protocol.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4       ac4ProtoMaps[] =
    {
        0       // IP
        , 1     // ICMP
        , 2     // IGMP
        , 6     // TCP
        , 12    // PUP
        , 17    // UDP
        , 22    // IDP
        , 255   // RawIP
        , 58    // ICMP6
    };
    constexpr tCIDLib::TCard4   c4ProtoCnt = tCIDLib::c4ArrayElems(ac4ProtoMaps);
}



// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------
static tCIDLib::TCard4 c4XlatProto(const tCIDSock::ESockProtos eToXlat)
{
    if (tCIDLib::TCard4(eToXlat) >= CIDKernel_Socket_Win32::c4ProtoCnt)
    {
        kmodCIDKernel.MsgPopUp
        (
            L"The socket protocol could not be mapped"
            , CID_FILE
            , CID_LINE
        );
    }
    return CIDKernel_Socket_Win32::ac4ProtoMaps[tCIDLib::c4EnumOrd(eToXlat)];
}


static unsigned short usConvertFamily(TKrnlIPAddr& kipaSrc)
{

}


// ---------------------------------------------------------------------------
//   CLASS: TSocketHandle
//  PREFIX: hsock
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSocketHandle: Constructors and Destructor
// ---------------------------------------------------------------------------
TSocketHandle::TSocketHandle() :

    m_phsockiThis(nullptr)
{
    m_phsockiThis = new TSocketHandleImpl;
    m_phsockiThis->hSock = CIDKernel_Socket_Win32::hInvalid;
}

TSocketHandle::TSocketHandle(const TSocketHandle& hsockToCopy) :

    m_phsockiThis(nullptr)
{
    m_phsockiThis = new TSocketHandleImpl;
    m_phsockiThis->hSock = hsockToCopy.m_phsockiThis->hSock;
}

TSocketHandle::~TSocketHandle()
{
    delete m_phsockiThis;
    m_phsockiThis = nullptr;
}


// ---------------------------------------------------------------------------
//  TSocketHandle: Public operators
// ---------------------------------------------------------------------------
TSocketHandle& TSocketHandle::operator=(const TSocketHandle& hsockToAssign)
{
    if (this == &hsockToAssign)
        return *this;

    m_phsockiThis->hSock = hsockToAssign.m_phsockiThis->hSock;

    return *this;
}


tCIDLib::TBoolean
TSocketHandle::operator==(const TSocketHandle& hsockToCompare) const
{
    return (m_phsockiThis->hSock == hsockToCompare.m_phsockiThis->hSock);
}

tCIDLib::TBoolean
TSocketHandle::operator!=(const TSocketHandle& hsockToCompare) const
{
    return !operator==(hsockToCompare);
}


// ---------------------------------------------------------------------------
//  TSocketHandle: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TSocketHandle::bIsValid() const
{
    return (m_phsockiThis->hSock != CIDKernel_Socket_Win32::hInvalid);
}

tCIDLib::TVoid TSocketHandle::Clear()
{
    m_phsockiThis->hSock = CIDKernel_Socket_Win32::hInvalid;
}

tCIDLib::TVoid
TSocketHandle::FormatToStr(       tCIDLib::TCh* const pszToFill
                        , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal
    (
        tCIDLib::TCard4(m_phsockiThis->hSock)
        , pszToFill
        , c4MaxChars
    );
}

const TSocketHandleImpl& TSocketHandle::hsockiThis() const
{
    return *m_phsockiThis;
}


// ---------------------------------------------------------------------------
//   CLASS: TKrnlSocket
//  PREFIX: ksock
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlSocket: Public, static methods
// ---------------------------------------------------------------------------


//
//  This is a bit of a hack to support the few places where an SSL based HTTP
//  query needs to be done. At some point it would be nice to implement our
//  own SSL support and add it to CIDNet. But for now this is all we have, based
//  on WinHTTP.
//
//  The caller must clean up the buffer we allocate for him.
//
tCIDLib::TBoolean
TKrnlSocket::bDoSSLPageOp(  const   tCIDNet::ESSLOps                eOp
                            , const tCIDLib::TCh* const             pszTarSrv
                            , const tCIDLib::TIPPortNum             ippnTar
                            , const tCIDLib::TCh* const             pszURL
                            , const tCIDLib::TCh* const             pszAgent
                            , const tCIDLib::TCh* const             pszAccept
                            ,       tCIDLib::TCard1*&               pc1OutBuf
                            ,       tCIDLib::TCard4&                c4ContLen
                            ,       TKrnlString&                    kstrContType
                            ,       tCIDLib::TCard4&                c4ResCode
                            , const tCIDLib::TCh* const             pszInHdrs
                            ,       TKrnlLList<TKrnlKVPair>* const  pkllstOutHdrs)
{
    BOOL                bRes;
    tCIDLib::TBoolean   bValid;
    HINTERNET           hOpen;
    HINTERNET           hConnect;
    HINTERNET           hRequest;
    IStream*            pStream = nullptr;
    HRESULT             hRes;

    // Try to open the WinHTTP API
    hOpen = ::WinHttpOpen
    (
        pszAgent
        , WINHTTP_ACCESS_TYPE_NO_PROXY
        , WINHTTP_NO_PROXY_NAME
        , WINHTTP_NO_PROXY_BYPASS
        , 0
    );

    if (!hOpen)
    {
        TKrnlError::SetLastError(kKrnlErrs::errcSSL_InitFailed);
        return kCIDLib::False;
    }

    // Try to connect to the target server
    hConnect = ::WinHttpConnect(hOpen, pszTarSrv, (INTERNET_PORT)ippnTar, 0);
    if (!hConnect)
    {
        TKrnlError::SetLastError(kKrnlErrs::errcSSL_ConnectFailed);

        // Clean up and give up
        ::WinHttpCloseHandle(hOpen);
        return kCIDLib::False;
    }

    //
    //  SEt any inncoming accept type. If it's null, then that'll work as
    //  well since we'll appear to just have no entries in this list.
    //
    const tCIDLib::TCh* apszTypes[2];
    apszTypes[0] = pszAccept;
    apszTypes[1] = 0;

    // Set up the correct operation
    const tCIDLib::TCh* pszOp = 0;
    if (eOp == tCIDNet::ESSLOps::GET)
        pszOp = L"GET";
    else if (eOp == tCIDNet::ESSLOps::POST)
        pszOp = L"POST";
    else if (eOp == tCIDNet::ESSLOps::PUT)
        pszOp = L"PUT";
    else
    {
        TKrnlError::SetLastError(kKrnlErrs::errcSSL_UnknownOp);

        // Clean up and give up
        ::WinHttpCloseHandle(hOpen);
        return kCIDLib::False;
    }

    // Use flag WINHTTP_FLAG_SECURE to initiate SSL
    hRequest = ::WinHttpOpenRequest
    (
        hConnect
        , pszOp
        , pszURL
        , L"HTTP/1.0"
        , WINHTTP_NO_REFERER
        , apszTypes
        , WINHTTP_FLAG_SECURE
    );

    if (!hRequest)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcSSL_OpenReqFailed, GetLastError());

        // Clean up and give up
        ::WinHttpCloseHandle(hConnect);
        ::WinHttpCloseHandle(hOpen);
        return kCIDLib::False;
    }

    // For now we don't handle keep alive
    DWORD dwOpt = WINHTTP_DISABLE_KEEP_ALIVE;
    bRes = WinHttpSetOption
    (
        hRequest
        , WINHTTP_OPTION_DISABLE_FEATURE
        , &dwOpt
        , sizeof(dwOpt)
    );

    if (!hRequest)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcSSL_SetOptFailed, GetLastError());

        // Clean up and give up
        ::WinHttpCloseHandle(hRequest);
        ::WinHttpCloseHandle(hConnect);
        ::WinHttpCloseHandle(hOpen);
        return kCIDLib::False;
    }

    // And now do the actual send
    bRes = ::WinHttpSendRequest
    (
        hRequest
        , pszInHdrs ? pszInHdrs : WINHTTP_NO_ADDITIONAL_HEADERS
        , -1
        , c4ContLen ? pc1OutBuf : nullptr
        , c4ContLen
        , c4ContLen
        , 0
    );

    if (!bRes)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcSSL_SendReqFailed, GetLastError());

        // Clean up and give up
        ::WinHttpCloseHandle(hRequest);
        ::WinHttpCloseHandle(hConnect);
        ::WinHttpCloseHandle(hOpen);
        return kCIDLib::False;
    }

    bRes = ::WinHttpReceiveResponse(hRequest, 0);
    if (!bRes)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcSSL_NoResponse, GetLastError());

        // Clean up and give up
        ::WinHttpCloseHandle(hRequest);
        ::WinHttpCloseHandle(hConnect);
        ::WinHttpCloseHandle(hOpen);
        return kCIDLib::False;
    }

    // Get the content type header info
    tCIDLib::TZStr256 szTmpStr;
    DWORD cch = 255 * kCIDLib::c4CharBytes;
    DWORD dwHeaderIndex = WINHTTP_NO_HEADER_INDEX;
    bRes = ::WinHttpQueryHeaders
    (
        hRequest
        , WINHTTP_QUERY_CONTENT_TYPE
        , NULL
        , (void*)szTmpStr
        , &cch
        , &dwHeaderIndex
    );
    if (bRes)
        kstrContType = szTmpStr;
    else
        kstrContType.Clear();

    // Get the result code
    dwHeaderIndex = WINHTTP_NO_HEADER_INDEX;
    bRes = ::WinHttpQueryHeaders
    (
        hRequest
        , WINHTTP_QUERY_STATUS_CODE
        , NULL
        , (void*)szTmpStr
        , &cch
        , &dwHeaderIndex
    );
    if (bRes)
    {
        c4ResCode = TRawStr::c4AsBinary(szTmpStr, bValid, tCIDLib::ERadices::Dec);
        if (!bValid)
            c4ResCode = 0;
    }
     else
    {
        c4ResCode = 0;
    }

    //
    //  If they provided an output headers pointer, get those out and return
    //  them. We get all the headers as a raw buffer and we'll then break them
    //  out into separate strings for return.
    //
    if (pkllstOutHdrs)
    {
        pkllstOutHdrs->RemoveAll();
        dwHeaderIndex = WINHTTP_NO_HEADER_INDEX;
        bRes = ::WinHttpQueryHeaders
        (
            hRequest
            , WINHTTP_QUERY_RAW_HEADERS
            , NULL
            , 0
            , &cch
            , &dwHeaderIndex
        );

        if (!bRes && (::GetLastError() == ERROR_INSUFFICIENT_BUFFER))
        {
            tCIDLib::TCard1* pc1OutHrs = new tCIDLib::TCard1[cch];
            TArrayJanitor<tCIDLib::TCard1> janOutHdrs(pc1OutHrs);

            dwHeaderIndex = WINHTTP_NO_HEADER_INDEX;
            bRes = ::WinHttpQueryHeaders
            (
                hRequest
                , WINHTTP_QUERY_RAW_HEADERS
                , NULL
                , pc1OutHrs
                , &cch
                , &dwHeaderIndex
            );

            if (bRes)
            {
                tCIDLib::TCh* pszKey = (tCIDLib::TCh*)pc1OutHrs;
                const tCIDLib::TCh* pszEnd = (tCIDLib::TCh*)(pc1OutHrs + cch);
                tCIDLib::TCard4 c4Len;
                tCIDLib::TCh* pszVal = nullptr;
                while (pszKey < pszEnd)
                {
                    // Remember the full length before we mangle it
                    c4Len = TRawStr::c4StrLen(pszKey);

                    // Run the value pointer up to the end of the key
                    pszVal = pszKey;
                    while (*pszVal && (*pszVal != L':') && !TRawStr::bIsSpace(*pszVal))
                        pszVal++;

                     // If any value content, or divider/space, then deal with that
                    if (*pszVal)
                    {
                        //
                        //  Cap off the key part, then move forward and keep
                        //  going, to make sure we skip any whitspace and divider
                        //
                        *pszVal = 0;
                        pszVal++;
                        while (*pszVal && ((*pszVal == L':') || TRawStr::bIsSpace(*pszVal)))
                            pszVal++;
                    }
                    pkllstOutHdrs->pobjAddNew(new TKrnlKVPair(pszKey, pszVal));

                    // Most past the null terminator of this one
                    pszKey += c4Len + 1;
                }
            }
        }
    }

    // Figure how how big the contents is if that info is available
    tCIDLib::TCh szContentLength[32];
    cch = 64;
    BOOL haveContentLength = ::WinHttpQueryHeaders
    (
        hRequest
        , WINHTTP_QUERY_CONTENT_LENGTH
        , NULL
        , szContentLength
        , &cch
        , &dwHeaderIndex
    );

    //
    //  If there was a content length, then set it, else we have to read until
    //  no more.
    //
    DWORD dwContentLength = 0;
    if (haveContentLength)
    {
        dwContentLength = TRawStr::c4AsBinary(szContentLength, bValid);
        if (!bValid)
        {
            TKrnlError::SetLastError(kKrnlErrs::errcSSL_InvalidContLenStr);

            // Clean up and give up
            ::WinHttpCloseHandle(hRequest);
            ::WinHttpCloseHandle(hConnect);
            ::WinHttpCloseHandle(hOpen);
            return kCIDLib::False;
        }
    }

    // read the response into memory stream
    hRes = ::CreateStreamOnHGlobal(0, true, &pStream);
    if (hRes)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcSSL_SendReqFailed, hRes);

        // Clean up and give up
        ::WinHttpCloseHandle(hRequest);
        ::WinHttpCloseHandle(hConnect);
        ::WinHttpCloseHandle(hOpen);
        return kCIDLib::False;
    }

    // allocate buffer for streaming received data
    tCIDLib::TCard1 *pc1Buf = new tCIDLib::TCard1[4096];
    if (!pc1Buf)
    {
        TKrnlError::SetLastError(kKrnlErrs::errcSSL_CreateBuf);

        // Clean up and give up
        pStream->Release();
        ::WinHttpCloseHandle(hRequest);
        ::WinHttpCloseHandle(hConnect);
        ::WinHttpCloseHandle(hOpen);
        return kCIDLib::False;
    }
    TArrayJanitor<tCIDLib::TCard1> janBuf(pc1Buf);

    // Loop until we get all the data
    DWORD dwReceivedTotal = 0;
    while (::WinHttpQueryDataAvailable(hRequest, &cch) && cch)
    {
        // Do up to 4K a round
        if (cch > 4096)
            cch = 4096;

        // Keep track of how many so far
        dwReceivedTotal += cch;

        // Get bytes and add them to the stream
        ::WinHttpReadData(hRequest, pc1Buf, cch, &cch);
        hRes = pStream->Write(pc1Buf, cch, NULL);
        if (hRes)
        {
            TKrnlError::SetLastError(kKrnlErrs::errcSSL_CreateBuf);

            // Clean up and give up
            pStream->Release();
            ::WinHttpCloseHandle(hRequest);
            ::WinHttpCloseHandle(hConnect);
            ::WinHttpCloseHandle(hOpen);
            return kCIDLib::False;
        }
    }

    // We can clean up the HTTP stuf now. The data is in the stream
    ::WinHttpCloseHandle(hRequest);
    ::WinHttpCloseHandle(hConnect);
    ::WinHttpCloseHandle(hOpen);

    // Null terminate the stream
    tCIDLib::TCard1* pc1Null = nullptr;
    pStream->Write(&pc1Null, 1, NULL);

    HGLOBAL hgl;
    hRes = ::GetHGlobalFromStream(pStream, &hgl);
    if (hRes)
    {
        pStream->Release();
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcSSL_GetStreamData, GetLastError());
        return kCIDLib::False;
    }

    // Allocate a buffer for the caller and copy the data over
    c4ContLen = dwReceivedTotal;
    const tCIDLib::TVoid* pData = ::GlobalLock(hgl);
    pc1OutBuf = new tCIDLib::TCard1[c4ContLen];
    TRawMem::CopyMemBuf(pc1OutBuf, (const tCIDLib::TCard1*)(pData), c4ContLen);

    // And now let the data and stream go
    ::GlobalUnlock(hgl);
    pStream->Release();

    return kCIDLib::True;
}



//
//  Do a read select operation on muliple ports and return true if any of them
//  are read, and return the flags for each of them. We also return how many of
//  them are ready. We'll wait up to the indicated time for at least one to
//  become ready, but we return as soon as anything is ready.
//
tCIDLib::TBoolean TKrnlSocket::
bMultiReadSel(          TKrnlSocket*            apsockList[kCIDSock::c4MaxSelect]
                ,       tCIDSock::EMSelFlags    aeFlags[kCIDSock::c4MaxSelect]
                ,       tCIDLib::TCard4&        c4Count
                , const tCIDLib::TEncodedTime   enctWait)
{
    // If debugging, make sure we don't pass too large a list
    #if CID_DEBUG_ON
    if (c4Count > kCIDSock::c4MaxSelect)
    {
        TKrnlError::SetLastError(kKrnlErrs::errcNet_SelListTooBig);
        return kCIDLib::False;
    }
    #endif

    // Set up the select structures for our socket
    fd_set FDRead;
    FD_ZERO(&FDRead);
    tCIDLib::TInt4 i4Cur;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        i4Cur = apsockList[c4Index]->m_hsockThis.m_phsockiThis->hSock;
        FD_SET(i4Cur, &FDRead);
    }

    timeval WTime;
    timeval* pWTime = nullptr;
    if (enctWait != kCIDLib::enctMaxWait)
    {
        WTime.tv_sec = tCIDLib::TCard4(enctWait / kCIDLib::enctOneSecond);
        WTime.tv_usec = tCIDLib::TCard4
        (
            (enctWait - (WTime.tv_sec * kCIDLib::enctOneSecond)) / 10
        );
        pWTime = &WTime;
    }

    // And now do the select statement
    const tCIDLib::TSInt iCount = ::select(0, &FDRead, 0, 0, pWTime);
    if (iCount == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }

    //
    //  Return the flags for the changes we got and no flags for the ones
    //  that didn't change.
    //
    tCIDLib::TCard4 c4ChangedSocks = 0;
    if (iCount > 0)
    {
        // And set our return flags to indicate the states
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            i4Cur = apsockList[c4Index]->m_hsockThis.m_phsockiThis->hSock;
            if (FD_ISSET(i4Cur, &FDRead))
            {
                aeFlags[c4Index] = tCIDSock::EMSelFlags::Read;
                c4ChangedSocks++;
            }
             else
            {
                aeFlags[c4Index] = tCIDSock::EMSelFlags::None;
            }
        }
    }
     else
    {
        // None were ready so just set them all to no change
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            aeFlags[c4Index] = tCIDSock::EMSelFlags::None;
    }

    // Give him back the number of sockets whose state changed
    c4Count = c4ChangedSocks;

    //
    //  NOTE: Getting zero isn't an error. We let the caller look at the
    //        count and see if any actual changes occured.
    //
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSocket::bMultiSel(         TKrnlSocket*            apsockList[kCIDSock::c4MaxSelect]
                        ,       tCIDSock::EMSelFlags    aeFlags[kCIDSock::c4MaxSelect]
                        ,       tCIDLib::TCard4&        c4Count
                        , const tCIDLib::TEncodedTime   enctWait)
{
    // If debugging, make sure we don't pass too large a list
    #if CID_DEBUG_ON
    if (c4Count > kCIDSock::c4MaxSelect)
    {
        TKrnlError::SetLastError(kKrnlErrs::errcNet_SelListTooBig);
        return kCIDLib::False;
    }
    #endif

    // Set up the select structures for our socket
    fd_set FDRead;
    fd_set FDWrite;
    fd_set FDExcept;

    FD_ZERO(&FDRead);
    FD_ZERO(&FDWrite);
    FD_ZERO(&FDExcept);

    // Load up the initial sets
    tCIDLib::TInt4 i4Cur;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        i4Cur = apsockList[c4Index]->m_hsockThis.m_phsockiThis->hSock;
        FD_SET(i4Cur, &FDRead);
        FD_SET(i4Cur, &FDWrite);
        FD_SET(i4Cur, &FDExcept);
    }

    timeval WTime;
    timeval* pWTime = nullptr;
    if (enctWait != kCIDLib::enctMaxWait)
    {
        WTime.tv_sec = tCIDLib::TCard4(enctWait / kCIDLib::enctOneSecond);
        WTime.tv_usec = tCIDLib::TCard4
        (
            (enctWait - (WTime.tv_sec * kCIDLib::enctOneSecond)) / 10
        );
        pWTime = &WTime;
    }

    // And now do the select statement
    const tCIDLib::TSInt iCount = ::select(0, &FDRead, &FDWrite, &FDExcept, pWTime);
    if (iCount == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }

    //
    //  Return the flags for the changes we got and no flags for the ones
    //  that didn't change.
    //
    tCIDLib::TCard4 c4ChangedSocks = 0;
    if (iCount > 0)
    {
        // And set our return flags to indicate the states
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            i4Cur = apsockList[c4Index]->m_hsockThis.m_phsockiThis->hSock;

            tCIDSock::EMSelFlags eCur = tCIDSock::EMSelFlags::None;
            if (FD_ISSET(i4Cur, &FDRead))
                eCur = tCIDSock::EMSelFlags::Read;

            if (FD_ISSET(i4Cur, &FDWrite))
                eCur = tCIDLib::eOREnumBits(eCur, tCIDSock::EMSelFlags::Write);

            if (FD_ISSET(i4Cur, &FDExcept))
                eCur = tCIDLib::eOREnumBits(eCur, tCIDSock::EMSelFlags::Except);

            if (eCur != tCIDSock::EMSelFlags::None)
                c4ChangedSocks++;

            aeFlags[c4Index] = eCur;
        }
    }
     else
    {
        // None were ready so just set them all to no change
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            aeFlags[c4Index] = tCIDSock::EMSelFlags::None;
    }

    // Give him back the number of sockets whose state changed
    c4Count = c4ChangedSocks;

    //
    //  NOTE: Getting zero isn't an error. We let the caller look at the
    //        count and see if any actual changes occured.
    //
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TKrnlSocket: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlSocket::TKrnlSocket()
{
}

TKrnlSocket::TKrnlSocket(const TSocketHandle& hsockToAdopt) :

    m_hsockThis(hsockToAdopt)
{
}


TKrnlSocket::~TKrnlSocket()
{
    //
    //  This should only happen in emergencies, i.e. the higher levels
    //  usually close down cleanly. But, just in case, we just do a
    //  no questions asked close.
    //
    if (m_hsockThis.m_phsockiThis->hSock != CIDKernel_Socket_Win32::hInvalid)
    {
        if (::closesocket(m_hsockThis.m_phsockiThis->hSock) == SOCKET_ERROR)
        {
            #if CID_DEBUG_ON
            const tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
            tCIDLib::TErrCode errcCIDLib = TKrnlIP::c4XlatError(c4LastErr);

            kmodCIDKernel.KrnlErrorPopUp
            (
                TKrnlError(errcCIDLib, c4LastErr)
                , CID_FILE
                , CID_LINE
            );
            #endif
        }
        m_hsockThis.m_phsockiThis->hSock = CIDKernel_Socket_Win32::hInvalid;
    }
}


// ---------------------------------------------------------------------------
//  TKrnlSocket: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlSocket::bAccept( TKrnlIPAddr&            kipaClient
                                        , tCIDLib::TIPPortNum&  ippnClient
                                        , TSocketHandle&        hsockToFill)
{
    SOCKADDR_STORAGE AddrStore = {0};
    tCIDLib::TSInt  iLen = sizeof(AddrStore);

    const tCIDLib::TInt4 i4Sock = ::accept
    (
        m_hsockThis.m_phsockiThis->hSock
        , reinterpret_cast<sockaddr*>(&AddrStore)
        , &iLen
    );

    if (i4Sock == INVALID_SOCKET)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }

    // Ask the ip address object to update from the info we got
    if (!kipaClient.bFromSockAddr(&AddrStore, tCIDLib::TCard4(iLen), ippnClient))
        return kCIDLib::False;

    // Fill in the passed kernel socket object with the accepted socket info
    hsockToFill.m_phsockiThis->hSock = i4Sock;

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSocket::bAssociateReadEvent(TKrnlEvent& kevToSignal)
{
    if (::WSAEventSelect(m_hsockThis.m_phsockiThis->hSock
                        , *tCIDLib::pReCastPtr<HANDLE>(kevToSignal.pHandle())
                        , FD_READ | FD_CLOSE))
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  This is called for listener sockets, which need to be able to be bound to
//  specific addresses, or to the ANY address, on a specific port.
//
tCIDLib::TBoolean
TKrnlSocket::bBindListen(const  TKrnlIPAddr&        kipaListen
                        , const tCIDLib::TIPPortNum ippnLocal)
{
    // It has to be created
    if (m_hsockThis.m_phsockiThis->hSock == CIDKernel_Socket_Win32::hInvalid)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    // Get the raw address info and put the port number in
    SOCKADDR_STORAGE AddrListen = {0};
    tCIDLib::TCard4 c4Len = sizeof(AddrListen);
    if (!kipaListen.bToSockAddr(&AddrListen, c4Len, ippnLocal))
        return kCIDLib::False;

    // And now bind to it
    if (::bind(m_hsockThis.m_phsockiThis->hSock
               , reinterpret_cast<sockaddr*>(&AddrListen)
               , c4Len) == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  This is called to bind to an appropriate local address given a particular target
//  address.
//
tCIDLib::TBoolean
TKrnlSocket::bBindForRemote(const   TKrnlIPAddr&        kipaTarAddr
                            , const tCIDLib::TIPPortNum ippnLocal)
{
    // It has to be created
    if (m_hsockThis.m_phsockiThis->hSock == CIDKernel_Socket_Win32::hInvalid)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    // Get the raw target address info and put the port number in
    SOCKADDR_STORAGE AddrTar = {0};
    tCIDLib::TCard4 c4RemLen = sizeof(AddrTar);
    if (!kipaTarAddr.bToSockAddr(&AddrTar, c4RemLen, ippnLocal))
        return kCIDLib::False;

    // Figure out what local IP address we should use for the target address.
    SOCKADDR_STORAGE AddrLocal = {0};
    tCIDLib::TCard4 c4LocalLen = sizeof(AddrLocal);

    if (::WSAIoctl( m_hsockThis.m_phsockiThis->hSock
                    , SIO_ROUTING_INTERFACE_QUERY
                    , &AddrTar
                    , c4RemLen
                    , (SOCKADDR*)&AddrLocal
                    , sizeof(AddrLocal)
                    , &c4LocalLen
                    , 0
                    , 0) == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcNet_GetLocalAddr, c4LastErr);
        return kCIDLib::False;
    }

    if (::bind(m_hsockThis.m_phsockiThis->hSock
               , reinterpret_cast<sockaddr*>(&AddrLocal)
               , c4LocalLen) == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// Clears the read event flag
tCIDLib::TBoolean TKrnlSocket::bClearReadEvent(TKrnlEvent& kevToClear)
{
    if (::WSAEventSelect(m_hsockThis.m_phsockiThis->hSock
                        , *tCIDLib::pReCastPtr<HANDLE>(kevToClear.pHandle())
                        , 0))
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSocket::bClose()
{
    if (m_hsockThis.m_phsockiThis->hSock != CIDKernel_Socket_Win32::hInvalid)
    {
        if (::closesocket(m_hsockThis.m_phsockiThis->hSock) == SOCKET_ERROR)
        {
            tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
            TKrnlError::SetLastKrnlError
            (
                TKrnlIP::c4XlatError(c4LastErr)
                , c4LastErr
            );
            return kCIDLib::False;
        }
        m_hsockThis.m_phsockiThis->hSock = CIDKernel_Socket_Win32::hInvalid;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSocket::bConnect(  const   TKrnlIPAddr&        kipaConnectTo
                        , const tCIDLib::TIPPortNum ippnTarget
                        , const tCIDLib::TCard8     enctWait)
{
    // Create a socket address for the target
    SOCKADDR_STORAGE AddrStore = {0};
    tCIDLib::TCard4 c4Len = sizeof(AddrStore);
    kipaConnectTo.bToSockAddr(&AddrStore, c4Len, ippnTarget);

    //
    //  Try the connect. Because we are non-blocking, they may have already
    //  tried, and given up, and it completed while we were away. So we can
    //  get an already connected error, in which case we just return because
    //  we are ready.
    //
    tCIDLib::TInt4 i4Res = ::connect
    (
        m_hsockThis.m_phsockiThis->hSock
        , reinterpret_cast<sockaddr*>(&AddrStore)
        , c4Len
    );

    if (i4Res == SOCKET_ERROR)
    {
        //
        //  If we get an already
        const tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();

        // If it says already connected, then we are ok
        if (c4LastErr == WSAEISCONN)
            return kCIDLib::True;

        //
        //  If it's the would block or already errors, then we are waiting
        //  on the connect and can fall through, else we failed.
        //
        if ((c4LastErr != WSAEWOULDBLOCK) && (c4LastErr != WSAEALREADY))
        {
            TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
            return kCIDLib::False;
        }
    }

    //
    //  We need to wait for the connect to complete for up to the length
    //  of time the caller indicated. Note that, if we time out, the connect
    //  is still active and they can call back in and wait again. It's still
    //  going on until it works or they close the socket.
    //
    fd_set FDWrite;
    FDWrite.fd_count = 1;
    FDWrite.fd_array[0] = m_hsockThis.m_phsockiThis->hSock;

    // Set up the timeout
    timeval WTime;
    timeval* pWTime = 0;
    if (enctWait != kCIDLib::enctMaxWait)
    {
        WTime.tv_sec = tCIDLib::TCard4(enctWait / kCIDLib::enctOneSecond);
        WTime.tv_usec = tCIDLib::TCard4
        (
            (enctWait - (WTime.tv_sec * kCIDLib::enctOneSecond)) / 10
        );
        pWTime = &WTime;
    }

    // And wait for the connect to complete
    const tCIDLib::TSInt iCount = ::select(0, 0, &FDWrite, 0, pWTime);
    if (iCount == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }

    // If it's zero, then we timed out, so return a timeout error
    if (!iCount)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_Timeout);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  Create a socket of the indicated type and protocol. It will be unbound and
//  unconnected, however they must provide us with the IP address so that we
//  know which address family to create it for.
//
tCIDLib::TBoolean
TKrnlSocket::bCreate(   const   tCIDSock::ESocketTypes  eType
                        , const tCIDSock::ESockProtos   eProtocol
                        , const tCIDSock::EAddrTypes    eAddrType)
{
    // Make sure its not already created
    if (m_hsockThis.m_phsockiThis->hSock != CIDKernel_Socket_Win32::hInvalid)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyExists);
        return kCIDLib::False;
    }

    // Convert the passed socket type to the internal value
    tCIDLib::TCard4 c4Type;
    if (eType == tCIDSock::ESocketTypes::Stream)
        c4Type = SOCK_STREAM;
    else if (eType == tCIDSock::ESocketTypes::Datagram)
        c4Type = SOCK_DGRAM;
    else if (eType == tCIDSock::ESocketTypes::Raw)
        c4Type = SOCK_RAW;
    else
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcNet_WrongSOType);
        return kCIDLib::False;
    }

    // Translate the protocol
    const tCIDLib::TCard4 c4Proto = c4XlatProto(eProtocol);

    int iFamily = AF_UNSPEC;
    if (eAddrType == tCIDSock::EAddrTypes::IPV4)
        iFamily = AF_INET;
    else if (eAddrType == tCIDSock::EAddrTypes::IPV6)
        iFamily = AF_INET6;
    else
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcNet_BadAddrType);
        return kCIDLib::False;
    }

    // And try to create the socket
    const tCIDLib::TInt4 i4Tmp = ::socket(iFamily, c4Type, c4Proto);
    if (i4Tmp == INVALID_SOCKET)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }

    // It worked, so store the socket id
    m_hsockThis.m_phsockiThis->hSock = i4Tmp;

    //
    //  If it is a stream socket, we will put it in the socket into
    //  non-blocking mode.
    //
    if (eType == tCIDSock::ESocketTypes::Stream)
    {
        unsigned long uFlag = 1;
        if (::ioctlsocket(i4Tmp, FIONBIO, &uFlag) == SOCKET_ERROR)
        {
            tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
            TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSocket::bDataReady(tCIDLib::TCard4& c4ToFill) const
{
    // Query the amount of data readable via the next read operation
    if (::ioctlsocket(m_hsockThis.m_phsockiThis->hSock, FIONREAD, &c4ToFill) == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSocket::bEnumEvents(TKrnlEvent& kevReset, tCIDSock::ESockEvs& eToFill)
{
    WSANETWORKEVENTS Events;
    if (::WSAEnumNetworkEvents( m_hsockThis.m_phsockiThis->hSock
                                , *tCIDLib::pReCastPtr<HANDLE>(kevReset.pHandle())
                                , &Events))
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }

    // Get the flags out into our format
    eToFill = tCIDSock::ESockEvs::None;

    if (Events.lNetworkEvents & FD_ACCEPT)
        eToFill = tCIDLib::eOREnumBits(eToFill, tCIDSock::ESockEvs::Accept);

    if (Events.lNetworkEvents & FD_CLOSE)
        eToFill = tCIDLib::eOREnumBits(eToFill, tCIDSock::ESockEvs::Close);

    if (Events.lNetworkEvents & FD_CONNECT)
        eToFill = tCIDLib::eOREnumBits(eToFill, tCIDSock::ESockEvs::Connect);

    if (Events.lNetworkEvents & FD_READ)
        eToFill = tCIDLib::eOREnumBits(eToFill, tCIDSock::ESockEvs::Read);

    if (Events.lNetworkEvents & FD_WRITE)
        eToFill = tCIDLib::eOREnumBits(eToFill, tCIDSock::ESockEvs::Write);

    return kCIDLib::True;
}



// Get socket options of various data types
tCIDLib::TBoolean
TKrnlSocket::bGetSockOpt(const  TKrnlSocket::EBSockOpts eOpt
                        ,       tCIDLib::TBoolean&      bStatus) const
{
    // Translate the option into a level and option flag
    tCIDLib::TSInt  iLevel;
    tCIDLib::TSInt  iOpt;

    // Some of these we flip the meaning of
    tCIDLib::TBoolean bNegate = kCIDLib::False;
    switch(eOpt)
    {
        case EBSockOpts::AllowBroadcast :
            iLevel = SOL_SOCKET;
            iOpt = SO_BROADCAST;
            break;

        case EBSockOpts::DontRoute :
            iLevel = SOL_SOCKET;
            iOpt = SO_DONTROUTE;
            break;

        case EBSockOpts::KeepAlive :
            iLevel = SOL_SOCKET;
            iOpt = SO_KEEPALIVE;
            break;

        case EBSockOpts::Nagle :
            iLevel = IPPROTO_TCP;
            iOpt = TCP_NODELAY;
            bNegate = kCIDLib::True;
            break;

        default :
            TKrnlError::SetLastError(kKrnlErrs::errcNet_BadSockOpt);
            return kCIDLib::False;
            break;
    };

    BOOL bFlag;
    int  iLen;
    if (::getsockopt
    (
        m_hsockThis.m_phsockiThis->hSock
        , iLevel
        , iOpt
        , reinterpret_cast<char*>(&bFlag)
        , &iLen) == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }

    if (iLen != sizeof(BOOL))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcNet_GetOptSize);
        return kCIDLib::False;
    }

    // Get the result back into the caller's parm. Negate if indicated
    bStatus = bFlag ? kCIDLib::True : kCIDLib::False;
    if (bNegate)
        bStatus = !bStatus;
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSocket::bGetSockOpt(const  TKrnlSocket::EISockOpts eOpt
                        ,       tCIDLib::TInt4&         i4Value) const
{
    // Translate the option into a level and option flag
    tCIDLib::TSInt iLevel = 0;
    tCIDLib::TSInt iOpt = 0;

    switch(eOpt)
    {
        case EISockOpts::LastError :
            iLevel = SOL_SOCKET;
            iLevel = SO_ERROR;
            break;

        case EISockOpts::MaxMsgSize :
            iLevel = SOL_SOCKET;
            iOpt = SO_MAX_MSG_SIZE;
            break;

        case EISockOpts::ReceiveBuf :
            iLevel = SOL_SOCKET;
            iOpt = SO_RCVBUF;
            break;

        case EISockOpts::SendBuf :
            iLevel = SOL_SOCKET;
            iOpt = SO_SNDBUF;
            break;

        default :
            TKrnlError::SetLastError(kKrnlErrs::errcNet_BadSockOpt);
            return kCIDLib::False;
            break;
    };

    int iFlag;
    int iLen;
    if (::getsockopt
    (
        m_hsockThis.m_phsockiThis->hSock
        , iLevel
        , iOpt
        , reinterpret_cast<char*>(&iFlag)
        , &iLen) == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }

    if (iLen != sizeof(int))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcNet_GetOptSize);
        return kCIDLib::False;
    }

    i4Value = iFlag;
    return kCIDLib::True;
}


// Tell the caller if this socket is open or not
tCIDLib::TBoolean TKrnlSocket::bIsOpen(tCIDLib::TBoolean& bToFill) const
{
    // If the handle is set, it's open
    bToFill = (m_hsockThis.m_phsockiThis->hSock != CIDKernel_Socket_Win32::hInvalid);
    return kCIDLib::True;
}


// Add a multicast group address to this socket
tCIDLib::TBoolean
TKrnlSocket::bJoinMulticastGroup(const  TKrnlIPAddr& kipaGroup
                                , const TKrnlIPAddr& kipaIntf)
{
    // Use the correct structure type
    ip_mreq     v4Req = {0};
    ipv6_mreq   v6Req = {0};

    // Convert the group address, port doesn't matter
    tCIDLib::TCard4 c4Len;
    const char* pData = 0;
    tCIDLib::TCard4 c4Size = 0;
    tCIDLib::TCard4 c4Opt = 0;
    if (kipaGroup.eType() == tCIDSock::EAddrTypes::IPV4)
    {
        c4Len = sizeof(v4Req.imr_multiaddr);
        if (!kipaGroup.bToInAddr(&v4Req.imr_multiaddr, c4Len))
            return kCIDLib::False;

        c4Len = sizeof(v4Req.imr_interface);
        if (!kipaIntf.bToInAddr(&v4Req.imr_interface, c4Len))
            return kCIDLib::False;

        c4Size = sizeof(v4Req);
        pData = reinterpret_cast<const char*>(&v4Req);
        c4Opt = IP_ADD_MEMBERSHIP;
    }
     else
    {
        c4Len = sizeof(v6Req.ipv6mr_multiaddr);
        if (!kipaGroup.bToInAddr(&v6Req.ipv6mr_multiaddr, c4Len))
            return kCIDLib::False;

        c4Len = sizeof(v6Req.ipv6mr_interface);
        if (!kipaIntf.bToInAddr(&v6Req.ipv6mr_interface, c4Len))
            return kCIDLib::False;

        c4Size = sizeof(v6Req);
        pData = reinterpret_cast<const char*>(&v6Req);
        c4Opt = IPV6_JOIN_GROUP;
    }

    // And do the socket option
    int iSock = m_hsockThis.m_phsockiThis->hSock;
    if (::setsockopt(iSock, IPPROTO_IP, c4Opt, pData, c4Size) == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// Setting linger has it's own special methods
tCIDLib::TBoolean TKrnlSocket::bLinger(tCIDLib::TBoolean& bNewState) const
{
    linger LingerInfo;
    socklen_t LingerLen = sizeof(linger);

    if (::getsockopt
    (
        m_hsockThis.m_phsockiThis->hSock
        , SOL_SOCKET
        , SO_LINGER
        , reinterpret_cast<char*>(&LingerInfo)
        , &LingerLen) == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }

    bNewState = LingerInfo.l_onoff ? kCIDLib::True : kCIDLib::False;
    return kCIDLib::True;
}

tCIDLib::TBoolean
TKrnlSocket::bLinger(   const   tCIDLib::TBoolean   bNewState
                        , const tCIDLib::TCard4     c4Time)
{
    linger LingerInfo;
    LingerInfo.l_onoff = bNewState ? short(1) : short(0);
    LingerInfo.l_linger = tCIDLib::TCard2(c4Time);

    if (::setsockopt
    (
        m_hsockThis.m_phsockiThis->hSock
        , SOL_SOCKET
        , SO_LINGER
        , reinterpret_cast<const char*>(&LingerInfo)
        , sizeof(linger)) == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// Listen on a listening socket
tCIDLib::TBoolean TKrnlSocket::bListen(const tCIDLib::TCard4 c4Backlog)
{
    const tCIDLib::TCard4 c4ActualBL = c4Backlog ? c4Backlog : SOMAXCONN;
    if (::listen(m_hsockThis.m_phsockiThis->hSock, c4ActualBL) == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// Get the local end point this guy is bound to, if any
tCIDLib::TBoolean
TKrnlSocket::bLocalEndPoint(tCIDLib::TIPPortNum&    ippnToFill
                            , TKrnlIPAddr&          kipaToFill) const
{
    // Create a socket address for the target
    SOCKADDR_STORAGE AddrStore = {0};
    reinterpret_cast<sockaddr*>(&AddrStore)->sa_family = AF_UNSPEC;
    int iLen = sizeof(AddrStore);

    if (::getsockname
    (
        m_hsockThis.m_phsockiThis->hSock
        , reinterpret_cast<sockaddr*>(&AddrStore)
        , &iLen) == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }

    return kipaToFill.bFromSockAddr
    (
        (sockaddr*)&AddrStore, tCIDLib::TCard4(iLen), ippnToFill
    );
}


// Qeries the usual status flags for this socket
tCIDLib::TBoolean
TKrnlSocket::bQuerySelStatus(       tCIDLib::TBoolean&      bIsReadable
                            ,       tCIDLib::TBoolean&      bIsWriteable
                            ,       tCIDLib::TBoolean&      bHasExcepts
                            , const tCIDLib::TEncodedTime   enctWait) const
{
    // Set up the select structures for our socket
    fd_set FDRead;
    FDRead.fd_count = 1;
    FDRead.fd_array[0] = m_hsockThis.m_phsockiThis->hSock;

    fd_set FDWrite;
    FDWrite.fd_count = 1;
    FDWrite.fd_array[0] = m_hsockThis.m_phsockiThis->hSock;

    fd_set FDExcept;
    FDExcept.fd_count = 1;
    FDExcept.fd_array[0] = m_hsockThis.m_phsockiThis->hSock;

    // Set up the timeout
    timeval WTime;
    timeval* pWTime = 0;
    if (enctWait != kCIDLib::enctMaxWait)
    {
        WTime.tv_sec = tCIDLib::TCard4(enctWait / kCIDLib::enctOneSecond);
        WTime.tv_usec = tCIDLib::TCard4
        (
            (enctWait - (WTime.tv_sec * kCIDLib::enctOneSecond)) / 10
        );
        pWTime = &WTime;
    }

    // And now do the select statement
    const tCIDLib::TSInt iCount = ::select(0, &FDRead, &FDWrite, &FDExcept, pWTime);
    if (iCount == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }

    //
    //  See if our socket survived in any of them. We know we only put in
    //  one socket, so we just check the 0th element.
    //
    bIsReadable = FD_ISSET(m_hsockThis.m_phsockiThis->hSock, &FDRead) != 0;
    bIsWriteable = FD_ISSET(m_hsockThis.m_phsockiThis->hSock, &FDWrite) != 0;
    bHasExcepts = FD_ISSET(m_hsockThis.m_phsockiThis->hSock, &FDExcept) != 0;

    return kCIDLib::True;
}


// Do a stream socket read, or optionally a peek
tCIDLib::TBoolean
TKrnlSocket::bReceive(          tCIDLib::TVoid* const   pToFill
                        , const tCIDLib::TCard4         c4MaxToRecv
                        ,       tCIDLib::TCard4&        c4Received
                        , const tCIDLib::TBoolean       bPeek)
{
    // Set up the flags
    const tCIDLib::TCard4 c4Flags = bPeek ? MSG_PEEK : 0;

    const tCIDLib::TSInt iRet = ::recv
    (
        m_hsockThis.m_phsockiThis->hSock
        , reinterpret_cast<char*>(pToFill)
        , c4MaxToRecv
        , c4Flags
    );

    if (iRet == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();

        //
        //  If it is a 'would block', then this guy is non-blocking so we
        //  need to just return zero read.
        //
        if (c4LastErr == WSAEWOULDBLOCK)
        {
            c4Received = 0;
            return kCIDLib::True;
        }

        c4Received = 0;
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }

    //
    //  If zero bytes, then the other size has closed down so we return an
    //  error indicating this.
    //
    if (!iRet)
    {
        c4Received = 0;
        TKrnlError::SetLastError(kKrnlErrs::errcNet_ConnReset);
        return kCIDLib::False;
    }

    c4Received = static_cast<tCIDLib::TCard4>(iRet);
    return kCIDLib::True;
}


// Do a datagram read
tCIDLib::TBoolean
TKrnlSocket::bReceiveFrom(          tCIDLib::TVoid* const   pToFill
                            , const tCIDLib::TCard4         c4BytesToRecv
                            ,       tCIDLib::TCard4&        c4Received
                            ,       TKrnlIPAddr&            kipaFrom
                            ,       tCIDLib::TIPPortNum&    ippnFrom
                            , const tCIDLib::TBoolean       bPeek)
{
    // Create a socket address for the target
    SOCKADDR_STORAGE AddrStore = {0};
    reinterpret_cast<sockaddr*>(&AddrStore)->sa_family = AF_UNSPEC;
    int iLen = sizeof(AddrStore);

    // Set up the flags
    const tCIDLib::TCard4 c4Flags = bPeek ? MSG_PEEK : 0;

    tCIDLib::TSInt iRet = ::recvfrom
    (
        m_hsockThis.m_phsockiThis->hSock
        , reinterpret_cast<char*>(pToFill)
        , c4BytesToRecv
        , c4Flags
        , reinterpret_cast<sockaddr*>(&AddrStore)
        , &iLen
    );

    if (iRet == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }

    c4Received = static_cast<tCIDLib::TCard4>(iRet);

    // Get the address info out for return
    return kipaFrom.bFromSockAddr
    (
        (sockaddr*)&AddrStore, tCIDLib::TCard4(iLen), ippnFrom
    );
}


// Returns the remote end point a stream socket is connected to
tCIDLib::TBoolean
TKrnlSocket::bRemoteEndPoint(   tCIDLib::TIPPortNum&    ippnToFill
                                , TKrnlIPAddr&          kipaToFill) const
{
    SOCKADDR_STORAGE AddrStore = {0};
    reinterpret_cast<sockaddr*>(&AddrStore)->sa_family = AF_UNSPEC;
    int iLen = sizeof(AddrStore);

    if (::getpeername
    (
        m_hsockThis.m_phsockiThis->hSock
        , reinterpret_cast<sockaddr*>(&AddrStore)
        , &iLen) == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }

    return kipaToFill.bFromSockAddr
    (
        (sockaddr*)&AddrStore, tCIDLib::TCard4(iLen), ippnToFill
    );
}


// A stream socket send
tCIDLib::TBoolean TKrnlSocket::bSend(const   tCIDLib::TVoid* const  pToSend
                                    , const tCIDLib::TCard4         c4BytesToSend)
{
    // We assume all bytes were sent or an error
    tCIDLib::TSInt iRet = ::send
    (
        m_hsockThis.m_phsockiThis->hSock
        , reinterpret_cast<const char*>(pToSend)
        , c4BytesToSend
        , 0
    );

    if (iRet == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// Send a datagram
tCIDLib::TBoolean
TKrnlSocket::bSendTo(   const   tCIDLib::TVoid* const   pToSend
                        , const tCIDLib::TCard4         c4BytesToSend
                        ,       tCIDLib::TCard4&        c4Sent
                        , const TKrnlIPAddr&            kipaTarget
                        , const tCIDLib::TIPPortNum     ippnTarget)
{
    // Create a socket address for the target
    SOCKADDR_STORAGE AddrStore = {0};
    tCIDLib::TCard4 c4Len = sizeof(AddrStore);
    kipaTarget.bToSockAddr(&AddrStore, c4Len, ippnTarget);

    tCIDLib::TSInt iRet = ::sendto
    (
        m_hsockThis.m_phsockiThis->hSock
        , reinterpret_cast<const char*>(pToSend)
        , c4BytesToSend
        , 0
        , reinterpret_cast<sockaddr*>(&AddrStore)
        , (int)c4Len
    );

    if (iRet == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }
    c4Sent = tCIDLib::TCard4(iRet);
    return kCIDLib::True;
}


// Set the indicated socket option
tCIDLib::TBoolean
TKrnlSocket::bSetSockOpt(       TKrnlSocket::EBSockOpts eOpt
                        , const tCIDLib::TBoolean       bNewValue)
{
    // Translate the option into a level and option flag
    tCIDLib::TCard4 c4Level;
    tCIDLib::TCard4 c4Opt;
    BOOL bFlag = bNewValue;

    switch(eOpt)
    {
        case EBSockOpts::AllowBroadcast :
            c4Level = SOL_SOCKET;
            c4Opt = SO_BROADCAST;
            break;

        case EBSockOpts::DontRoute :
            c4Level = SOL_SOCKET;
            c4Opt = SO_DONTROUTE;
            break;

        case EBSockOpts::KeepAlive :
            c4Level = SOL_SOCKET;
            c4Opt = SO_KEEPALIVE;
            break;

        case EBSockOpts::Nagle :
            c4Level = IPPROTO_TCP;
            c4Opt = TCP_NODELAY;

            // We flip the logic of this one
            bFlag = !bNewValue;
            break;

        case EBSockOpts::ReuseAddr :
            c4Level = SOL_SOCKET;
            c4Opt = SO_REUSEADDR;
            break;

        default :
            TKrnlError::SetLastError(kKrnlErrs::errcNet_BadSockOpt);
            return kCIDLib::False;
            break;
    };

    if (::setsockopt
    (
        m_hsockThis.m_phsockiThis->hSock
        , c4Level
        , c4Opt
        , reinterpret_cast<const char*>(&bFlag)
        , sizeof(BOOL)) == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSocket::bSetSockOpt(       TKrnlSocket::EISockOpts eOpt
                        , const tCIDLib::TInt4          i4NewValue)
{
    // Translate the option into a level and option flag
    tCIDLib::TCard4 c4Level;
    tCIDLib::TCard4 c4Opt;
    int iFlag = i4NewValue;

    switch(eOpt)
    {
        case EISockOpts::ReceiveBuf :
            c4Level = SOL_SOCKET;
            c4Opt = SO_RCVBUF;
            break;

        case EISockOpts::SendBuf :
            c4Level = SOL_SOCKET;
            c4Opt = SO_SNDBUF;
            break;

        case EISockOpts::TTL :
            c4Level = IPPROTO_IP;
            c4Opt = IP_TTL;
            break;

        case EISockOpts::TTLV6 :
            c4Level = IPPROTO_IPV6;
            c4Opt = IPV6_UNICAST_HOPS;
            break;

        default :
            TKrnlError::SetLastError(kKrnlErrs::errcNet_BadSockOpt);
            return kCIDLib::False;
            break;
    };

    if (::setsockopt
    (
        m_hsockThis.m_phsockiThis->hSock
        , c4Level
        , c4Opt
        , reinterpret_cast<const char*>(&iFlag)
        , sizeof(int)) == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// Start the graceful shutdown process
tCIDLib::TBoolean TKrnlSocket::bShutdown(const tCIDSock::ESockShutdown eMode)
{
    // Create the 'how' flag
    tCIDLib::TCard4 c4How = 0;
    if (eMode == tCIDSock::ESockShutdown::Both)
        c4How = SD_BOTH;
    else if (eMode == tCIDSock::ESockShutdown::Send)
        c4How = SD_SEND;
    else
        c4How = SD_RECEIVE;

    if (::shutdown(m_hsockThis.m_phsockiThis->hSock, c4How) == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// Wait for up to the indicated period of time for a connect on a listening socket
tCIDLib::TBoolean
TKrnlSocket::bWaitForConnectReady(          tCIDLib::TBoolean&      bGotConnect
                                    , const tCIDLib::TEncodedTime   enctWait)
{
    // Set up the select structures for our socket
    fd_set FDConn;
    FDConn.fd_count = 1;
    FDConn.fd_array[0] = m_hsockThis.m_phsockiThis->hSock;

    timeval WTime;
    timeval* pWTime = nullptr;
    if (enctWait != kCIDLib::enctMaxWait)
    {
        WTime.tv_sec = tCIDLib::TCard4(enctWait / kCIDLib::enctOneSecond);
        WTime.tv_usec = tCIDLib::TCard4
        (
            (enctWait - (WTime.tv_sec * kCIDLib::enctOneSecond)) / 10
        );
        pWTime = &WTime;
    }

    // And now do the select statement
    const tCIDLib::TSInt iCount = ::select(0, &FDConn, 0, 0, pWTime);
    if (iCount == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }

    //
    //  See if our socket survived in any of them. We know we only put in
    //  one socket, so we just check the 0th element.
    //
    if (iCount == 1)
        bGotConnect = FD_ISSET(m_hsockThis.m_phsockiThis->hSock, &FDConn) != 0;
    else
        bGotConnect = kCIDLib::False;
    return kCIDLib::True;
}



// Wait for up to the indicated period of time to see if data is ready
tCIDLib::TBoolean
TKrnlSocket::bWaitForDataReady(         tCIDLib::TBoolean&      bGotData
                                , const tCIDLib::TEncodedTime   enctWait) const
{
    // Set up the select structures for our socket
    fd_set FDRead;
    FDRead.fd_count = 1;
    FDRead.fd_array[0] = m_hsockThis.m_phsockiThis->hSock;

    timeval WTime;
    timeval* pWTime = nullptr;
    if (enctWait != kCIDLib::enctMaxWait)
    {
        WTime.tv_sec = tCIDLib::TCard4(enctWait / kCIDLib::enctOneSecond);
        WTime.tv_usec = tCIDLib::TCard4
        (
            (enctWait - (WTime.tv_sec * kCIDLib::enctOneSecond)) / 10
        );
        pWTime = &WTime;
    }

    // And now do the select statement
    const tCIDLib::TSInt iCount = ::select(0, &FDRead, 0, 0, pWTime);
    if (iCount == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }

    if (iCount == 1)
        bGotData = FD_ISSET(m_hsockThis.m_phsockiThis->hSock, &FDRead) != 0;
    else
        bGotData = kCIDLib::False;
    return kCIDLib::True;
}


// Wait for up to the indicated period of time to see if it's ok to send
tCIDLib::TBoolean
TKrnlSocket::bWaitForSendReady(         tCIDLib::TBoolean&      bReady
                                , const tCIDLib::TEncodedTime   enctWait) const
{
    // Assume not ready until proven otherwise
    bReady = kCIDLib::False;

    //
    //  Set up the select structures for our socket. In order to wake
    //  up immediately if the socket get's dropped on the other side while
    //  we wait, we set up both a read and a write. The read gets the
    //  socket going down, and the write gets the ready to send.
    //
    fd_set FDWrite;
    FDWrite.fd_count = 1;
    FDWrite.fd_array[0] = m_hsockThis.m_phsockiThis->hSock;

    fd_set FDRead;
    FDRead.fd_count = 1;
    FDRead.fd_array[0] = m_hsockThis.m_phsockiThis->hSock;

    timeval WTime;
    timeval* pWTime = nullptr;
    if (enctWait != kCIDLib::enctMaxWait)
    {
        WTime.tv_sec = tCIDLib::TCard4(enctWait / kCIDLib::enctOneSecond);
        WTime.tv_usec = tCIDLib::TCard4
        (
            (enctWait - (WTime.tv_sec * kCIDLib::enctOneSecond)) / 10
        );
        pWTime = &WTime;
    }

    // And now do the select statement
    const tCIDLib::TSInt iCount = ::select(0, &FDRead, &FDWrite, 0, pWTime);
    if (iCount == SOCKET_ERROR)
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }

    //
    //  See if our socket survived in any of them. We know we only put in
    //  one socket, so we just check the 0th element.
    //
    if (iCount == 1)
    {
        //
        //  If we get the read indicator, then the socket has been dropped.
        //  So, what we do is lie and say it's ready. That'll make the caller
        //  try to send, which will get the appropriate error.
        //
        if (FD_ISSET(m_hsockThis.m_phsockiThis->hSock, &FDRead))
        {
            bReady = kCIDLib::True;
            return kCIDLib::True;
        }

        // We didn't lose the socket, so return the indicator
        bReady = FD_ISSET(m_hsockThis.m_phsockiThis->hSock, &FDWrite) != 0;
    }
    return kCIDLib::True;
}

