//
// FILE NAME: CIDNet_HTTPClient.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/29/2000
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
//  This file implements the THTTPClient class.
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
#include    "CIDNet_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(THTTPClient,TNetCoreParser)
RTTIDecls(TAsyncHTTPQ, TObject)



// ---------------------------------------------------------------------------
//   CLASS: THTTPClient
//  PREFIX: httpc
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  HTTPClient: Public, static data
// ---------------------------------------------------------------------------
const TString   THTTPClient::strCC_MaxAge(L"max-age");
const TString   THTTPClient::strCC_MaxStale(L"max-stale");
const TString   THTTPClient::strCC_SMaxAge(L"s-maxage");
const TString   THTTPClient::strCC_MustReval(L"must-revalidate");
const TString   THTTPClient::strCC_NoCache(L"no-cache");
const TString   THTTPClient::strCC_NoStore(L"no-store");

const TString   THTTPClient::strHdr_Accept(L"Accept");
const TString   THTTPClient::strHdr_CacheControl(L"Cache-Control");
const TString   THTTPClient::strHdr_Connection(L"Connection");
const TString   THTTPClient::strHdr_ContDisposition(L"Content-Disposition");
const TString   THTTPClient::strHdr_ContLen(L"Content-Length");
const TString   THTTPClient::strHdr_ContTransferEncoding(L"Content-Transfer-Encoding");
const TString   THTTPClient::strHdr_ContType(L"Content-Type");
const TString   THTTPClient::strHdr_ETag(L"ETag");
const TString   THTTPClient::strHdr_Expires(L"Expires");
const TString   THTTPClient::strHdr_Host(L"Host");
const TString   THTTPClient::strHdr_IfModifiedSince(L"If-Modified-Since");
const TString   THTTPClient::strHdr_LastModified(L"Last-Modified");
const TString   THTTPClient::strHdr_Location(L"Location");
const TString   THTTPClient::strHdr_Origin(L"Origin");
const TString   THTTPClient::strHdr_SecWSAccept(L"Sec-WebSocket-Accept");
const TString   THTTPClient::strHdr_SecWSKey(L"Sec-WebSocket-Key");
const TString   THTTPClient::strHdr_TransferEncoding(L"Transfer-Encoding");
const TString   THTTPClient::strHdr_TE(L"TE");
const TString   THTTPClient::strHdr_Upgrade(L"Upgrade");
const TString   THTTPClient::strHdr_WWWAuthenticate(L"WWW-Authenticate");


// ---------------------------------------------------------------------------
//  THTTPClient: Public, static methods
// ---------------------------------------------------------------------------

//
//  Does the basic grunt work of authentication, since it's generally
//  always done the same. The basic processes involved are also exposed
//  separately below if the caller wants to handle it.
//
//  They provide us with the output header lines from the previous operation
//  that returned an unauthorized error, which we can search for info we
//  need. And they provide us with a list of out headers that we can add
//  the authorization pairs to.
//
tCIDLib::TBoolean
THTTPClient::bBuildAuthStr( const   tCIDLib::TKVPCollect&   colInHdrLines
                            , const TString&                strUserName
                            , const TString&                strPassword
                            , const TURL&                   urlSrc
                            ,       tCIDLib::TCard4&        c4ReqCnt
                            ,       TString&                strAuthInfo)

{
    // If the URL doesn'thave user name and password info, we fail
    if (strUserName.bIsEmpty() || strPassword.bIsEmpty())
        return kCIDLib::False;

    // We have to have gotten an authenticate header line
    if (!bFindHdrLine(  colInHdrLines
                        , THTTPClient::strHdr_WWWAuthenticate
                        , strAuthInfo))
    {
        return kCIDLib::False;
    }

    //
    //  Parse out the authenticiation request info and get all the
    //  various bits we need.
    //
    tCIDNet::EHTTPAuthTypes eType;
    TString                 strReqLine;
    TString                 strRealm;
    TString                 strNonce;
    TString                 strOpaque;
    TString                 strDomain;
    TString                 strAlgorithm;
    ParseAuthReq
    (
        strAuthInfo, eType, strRealm, strNonce, strOpaque, strDomain, strAlgorithm
    );

    if (eType == tCIDNet::EHTTPAuthTypes::Basic)
    {
        //
        //  This will create the hash for us, but we hve to prepend Basic
        //  before it.
        //
        TString strAuthStr;
        MakeBasicAuthStr(strUserName, strPassword, strAuthStr);
        strAuthInfo = L"Basic ";
        strAuthInfo.Append(strAuthStr);
    }
     else if (eType == tCIDNet::EHTTPAuthTypes::Digest)
    {
        const TString strCNonce(L"a8438fca4");

        // And let's calculate the two return values
        TString strHA1 = strUserName;
        strHA1.Append(L':');
        strHA1.Append(strRealm);
        strHA1.Append(L':');
        strHA1.Append(strPassword);

        TString strHA2 = L"GET:";
        strHA2.Append(urlSrc.strPath());

        TMessageDigest5 mdigAuth;
        TMD5Hash        mhashAuth1;
        TMD5Hash        mhashAuth2;

        // Hash the two values
        mdigAuth.StartNew();
        mdigAuth.DigestStr(strHA1);
        mdigAuth.Complete(mhashAuth1);

        mdigAuth.StartNew();
        mdigAuth.DigestStr(strHA2);
        mdigAuth.Complete(mhashAuth2);

        // Format them back out to hex strings and lower case them
        mhashAuth1.RawFormat(strHA1);
        mhashAuth2.RawFormat(strHA2);
        strHA1.ToLower();
        strHA2.ToLower();

        //
        //  Format out the request number out as an 8 digit, leading
        //  zero filled hex number.
        //
        TString strReqCnt;
        {
            TString strRC;
            strRC.AppendFormatted(c4ReqCnt, tCIDLib::ERadices::Hex);
            strReqCnt.FormatToFld(strRC, 8, tCIDLib::EHJustify::Right, L'0');

            // Bump the number for next time
            c4ReqCnt++;
        }

        // Build up the whole string and hash that
        TString strResponse(strHA1);
        strResponse.Append(L':');
        strResponse.Append(strNonce);
        strResponse.Append(L':');
        strResponse.Append(strReqCnt);
        strResponse.Append(L':');
        strResponse.Append(strCNonce);
        strResponse.Append(L":auth:");
        strResponse.Append(strHA2);

        // Hash that and format the result back out to hex and lower case
        TMD5Hash mhashResponse;
        mdigAuth.StartNew();
        mdigAuth.DigestStr(strResponse);
        mdigAuth.Complete(mhashResponse);
        mhashResponse.RawFormat(strResponse);
        strResponse.ToLower();

        //
        //  And let's add the various pairs required. This will be easier
        //  with an output stream.
        //
        TTextStringOutStream strmOut(&strAuthInfo);
        strmOut << L"Digest username=\""
                << strUserName << L"\", realm=\""
                << strRealm << L"\", nonce=\"" << strNonce
                << L"\", uri=\"" << urlSrc.strPath()
                << L"\", qop=\"auth\", algorithm=MD5, nc="
                << strReqCnt
                << L", cnonce=\""
                << strCNonce
                << L"\", response=\""
                << strResponse << L"\", opaque=\""
                << strOpaque << L"\""
                << kCIDLib::FlushIt;
    }
     else
    {
        // Not one we understand
        return kCIDLib::False;
    }
    return kCIDLib::True;
}



//
//  Builds up a standard error reply message, replacing a few tokens with
//  the info provided. The output is formatted itno the passed text stream.
//
tCIDLib::TVoid
THTTPClient::BuildErrReply( const  tCIDLib::TCard4  c4ErrCode
                            , const TString&        strErrText
                            ,       TTextOutStream& strmTar)
{
    static const tCIDLib::TCh* const pszStdErr1
    (
        L"<html><head><title>CIDNet Error Report</title></head><body>"
        L"<p><b><font size='3'><blockquote>The page cannot be displayed</font></b></p>"
        L"<p><font size='2'>The page you requested could not be accessed for some reason. It "
        L"may be a temporary error on the other end, or you may have asked for an invalid "
        L"page. The error message below may help you diagnose the problem.</blockquote></font></p>"
        L"<hr color='#00FFFF' width='80%' size='1'><p><font size='2'>Error Information:"
        L"</font></p><blockquote><p><font size='2'>Error Code: "
    );
    static const tCIDLib::TCh* const pszStdErr2
    (
        L"</font></p></blockquote><p>&nbsp;</p></body></html>"
    );

    // Format out the first part, the two bits of info, then the second part
    strmTar << pszStdErr1
            << c4ErrCode << L"<br>Error Message: ";
    EscapeBodyText(strErrText, strmTar);
    strmTar << pszStdErr2 << kCIDLib::FlushIt;
}

tCIDLib::TCard4
THTTPClient::c4BuildErrReply(const  tCIDLib::TCard4  c4ErrCode
                            , const TString&        strErrText
                            ,       TString&        strEncoding
                            ,       TMemBuf&        mbufTar)
{
    TTextMBufOutStream strmOut
    (
        &mbufTar, tCIDLib::EAdoptOpts::NoAdopt, new TUTF8Converter
    );
    strEncoding = L"text/html; charset=utf-8";
    BuildErrReply(c4ErrCode, strErrText, strmOut);
    strmOut.Flush();
    return strmOut.c4CurSize();
}


//
//  This is a bit of a hack for now to allow us to make SSL HTTP requests. We
//  use a wrapper in the kernel layer that on Windows wraps the WinHTTP API and
//  let's us make simple requests. We have one that takes in/out header lines
//  and one that doesn't. It's worth the replication to avoid the overhead
//  since there can be a fair amount of data involved, and it has to be copied
//  and buffers allocated, etc...
//
tCIDLib::TCard4
THTTPClient::c4DoSSLPageOp( const   tCIDNet::ESSLOps    eOp
                            , const TURL&               urlToGet
                            , const TString&            strAgent
                            , const TString&            strAccept
                            ,       TString&            strContType
                            ,       tCIDLib::TCard4&    c4ResCode
                            ,       TMemBuf&            mbufData
                            , const tCIDLib::TCard4     c4PutBytes)
{
    //
    //  Break out the URL parts in their encoded forms that we need to
    //  send them.
    //
    TIPEndPoint ipepTar;
    TString     strTmp;
    TString     strHost;
    TString     strUser;
    TString     strPath;
    TString     strSuffix;

    try
    {
        urlToGet.QueryEncodedForm(strTmp, strUser, strHost, strPath, strSuffix);
        ipepTar = urlToGet.ipepHost(tCIDSock::EAddrTypes::IPV4);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    // Build up the full path and query
    TString strQuery(strPath.c4Length() + strSuffix.c4Length() + 4);
    strQuery = strPath;
    strQuery.Append(strSuffix);

    //
    //  And let's make the request. Pass in any incoming content type and
    //  any data to send. If the put bytes is zero, the ingoing data pointer
    //  will be ignored.
    //
    tCIDLib::TCard4         c4ContLen = c4PutBytes;
    tCIDLib::TCard1*        pc1OutData = mbufData.pc1Data();
    TKrnlString             kstrContType(strContType.pszBuffer());

    tCIDLib::TBoolean bRes = TKrnlSocket::bDoSSLPageOp
    (
        eOp
        , strHost.pszBuffer()
        , ipepTar.ippnThis()
        , strQuery.pszBuffer()
        , strAgent.pszBuffer()
        , strAccept.pszBuffer()
        , pc1OutData
        , c4ContLen
        , kstrContType
        , c4ResCode
        , 0
        , 0
    );

    if (!bRes)
    {
        facCIDNet().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcHTTP_SSLGetFailed
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
        );
    }

    // Make sure the output data gets cleaned up
    TArrayJanitor<tCIDLib::TCard1> janBuf(pc1OutData);

    // Copy the data into the buffer and give back the content type
    strContType = kstrContType.pszValue();
    mbufData.CopyIn(pc1OutData, c4ContLen);

    // Return the content length
    return c4ContLen;
}


tCIDLib::TCard4
THTTPClient::c4DoSSLPageOp( const   tCIDNet::ESSLOps        eOp
                            , const TURL&                   urlToGet
                            , const TString&                strAgent
                            , const TString&                strAccept
                            ,       TString&                strContType
                            ,       tCIDLib::TCard4&        c4ResCode
                            ,       TMemBuf&                mbufData
                            , const tCIDLib::TKVPCollect&   colInHdrs
                            ,       tCIDLib::TKVPCollect&   colOutHdrs
                            , const tCIDLib::TCard4         c4PutBytes)
{
    //
    //  Break out the URL parts in their encoded forms that we need to
    //  send them.
    //
    TIPEndPoint ipepTar;
    TString     strTmp;
    TString     strHost;
    TString     strUser;
    TString     strPath;
    TString     strSuffix;

    try
    {
        urlToGet.QueryEncodedForm(strTmp, strUser, strHost, strPath, strSuffix);
        ipepTar = urlToGet.ipepHost(tCIDSock::EAddrTypes::IPV4);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    // Build up the full path and query
    TString strQuery(strPath.c4Length() + strSuffix.c4Length() + 4);
    strQuery = strPath;
    strQuery.Append(strSuffix);

    //
    //  If any ingoing headers, format them out. Remember if we saw a
    //  content type header or not. If not, and we have outgoing bytes,
    //  and the content type isn't empty, we'll add one for them.
    //
    tCIDLib::TBoolean bHaveCType = kCIDLib::False;
    TString strInHdrs;
    if (!colInHdrs.bIsEmpty())
    {
        TColCursor<TKeyValuePair>* pcursIndHdrs = colInHdrs.pcursNew();
        TJanitor<TColCursor<TKeyValuePair> > janCurs(pcursIndHdrs);
        pcursIndHdrs->bReset();
        do
        {
            const TKeyValuePair& kvalCur = pcursIndHdrs->objRCur();

            strInHdrs.Append(kvalCur.strKey());
            strInHdrs.Append(L": ");
            strInHdrs.Append(kvalCur.strValue());
            strInHdrs.Append(L"\n");

            if (kvalCur.strKey().bCompareI(THTTPClient::strHdr_ContType))
                bHaveCType = kCIDLib::True;

        }   while(pcursIndHdrs->bNext());
    }

    // If no explicit content type, add one if appropriate
    if (!bHaveCType && c4PutBytes && !strContType.bIsEmpty())
    {
        if (!strInHdrs.bIsEmpty())
            strInHdrs.Append(L"\n");
        strInHdrs.Append(THTTPClient::strHdr_ContType);
        strInHdrs.Append(L": ");
        strInHdrs.Append(strContType);
    }

    //
    //  And let's make the request. Pass in any incoming content type and
    //  any data to send. If the put bytes is zero, the ingoing data pointer
    //  will be ignored.
    //
    tCIDLib::TCard4         c4ContLen = c4PutBytes;
    TKrnlLList<TKrnlKVPair> kllstOutHdrs;
    tCIDLib::TCard1*        pc1OutData = mbufData.pc1Data();
    TKrnlString             kstrContType(strContType.pszBuffer());

    tCIDLib::TBoolean bRes = TKrnlSocket::bDoSSLPageOp
    (
        eOp
        , strHost.pszBuffer()
        , ipepTar.ippnThis()
        , strQuery.pszBuffer()
        , strAgent.pszBuffer()
        , strAccept.pszBuffer()
        , pc1OutData
        , c4ContLen
        , kstrContType
        , c4ResCode
        , strInHdrs.bIsEmpty() ? 0 : strInHdrs.pszBuffer()
        , &kllstOutHdrs
    );

    if (!bRes)
    {
        facCIDNet().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcHTTP_SSLGetFailed
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
        );
    }

    // Make sure the output data gets cleaned up
    TArrayJanitor<tCIDLib::TCard1> janBuf(pc1OutData);

    // Copy the data into the buffer and give back the content type
    strContType = kstrContType.pszValue();
    mbufData.CopyIn(pc1OutData, c4ContLen);

    // If any headers come back, give them back
    colOutHdrs.RemoveAll();
    if (!kllstOutHdrs.bIsEmpty())
    {
        kllstOutHdrs.bResetCursor();
        TKrnlKVPair* pkvpCur;
        while (kllstOutHdrs.bNext(pkvpCur))
        {
            colOutHdrs.objAdd
            (
                TKeyValuePair(pkvpCur->pszKey(), pkvpCur->pszValue())
            );
        }
    }

    // Return the content length
    return c4ContLen;
}


//
//  Sends the passed text content to the output stream, escaping characters
//  that need to be escape in the body of an HTML page (i.e. not in any of
//  the markup, just regular text.
//
tCIDLib::TVoid
THTTPClient::EscapeBodyText(const TString& strText, TTextOutStream& strmOut)
{
    const tCIDLib::TCh* pszCur = strText.pszBuffer();
    const tCIDLib::TCh* pszEnd = pszCur + strText.c4Length();
    while (pszCur < pszEnd)
    {
        const tCIDLib::TCh chCur = *pszCur++;

        switch(chCur)
        {
            case kCIDLib::chLessThan :
                strmOut << L"&lt;";
                break;

            case kCIDLib::chGreaterThan :
                strmOut << L"&gt;";
                break;

            case kCIDLib::chAmpersand :
                strmOut << L"&amp;";
                break;

            default :
                // Nothing special, just copy to stream
                strmOut << chCur;
                break;
        };
    }
}

tCIDLib::TVoid
THTTPClient::EscapeBodyText(const   TString&            strText
                            ,       TString&            strOut
                            , const tCIDLib::TBoolean   bAppend)
{
    if (!bAppend)
        strOut.Clear();

    const tCIDLib::TCh* pszCur = strText.pszBuffer();
    const tCIDLib::TCh* pszEnd = pszCur + strText.c4Length();
    while (pszCur < pszEnd)
    {
        const tCIDLib::TCh chCur = *pszCur++;

        switch(chCur)
        {
            case kCIDLib::chLessThan :
                strOut.Append(L"&lt;");
                break;

            case kCIDLib::chGreaterThan :
                strOut.Append(L"&gt;");
                break;

            case kCIDLib::chAmpersand :
                strOut.Append(L"&amp;");
                break;

            default :
                // Nothing special, just copy
                strOut.Append(chCur);
                break;
        };
    }
}


//
//  Sends the passed text content to the output stream, expanding encoded
//  characters that are encoded via numeric char refs or the standard three
//  char refs.
//
tCIDLib::TVoid
THTTPClient::ExpandBodyText(const TString& strText, TTextOutStream& strmOut)
{
    // Call the other version, with a temp string, then stream that
    TString strTmp;
    ExpandBodyText(strText, strTmp, kCIDLib::False);
    strmOut << strTmp;
}

tCIDLib::TVoid
THTTPClient::ExpandBodyText(const   TString&            strText
                            ,       TString&            strOut
                            , const tCIDLib::TBoolean   bAppend)
{
    if (!bAppend)
        strOut.Clear();

    // We do a little state machine
    enum class EStates
    {
        WaitRef
        , WaitType
        , WaitLT
        , WaitGT
        , WaitAmp2
        , WaitAmp3
        , WaitQuot2
        , WaitQuot3
        , WaitQuot4
        , WaitRadix

        // These have to be in this order
        , WaitNum1
        , WaitNum2
        , WaitNum3
        , WaitNum4
        , WaitSemi
    };

    const tCIDLib::TCh* const pszStart = strText.pszBuffer();
    const tCIDLib::TCh* pszCur = pszStart;

    EStates eState = EStates::WaitRef;
    tCIDLib::TCard2 c2NumRef = 0;
    tCIDLib::ERadices eRadix = tCIDLib::ERadices::Dec;
    while (*pszCur)
    {
        //
        //  We got ahead and bump the source pointer. This means that the
        //  calculation done in ThrowBadCharRef will naturally result in
        //  a 1 based offset.
        //
        const tCIDLib::TCh chCur = *pszCur++;
        switch(eState)
        {
            case EStates::WaitRef :
            {
                if (chCur == kCIDLib::chAmpersand)
                {
                    c2NumRef = 0;
                    eRadix = tCIDLib::ERadices::Dec;
                    eState = EStates::WaitType;
                }
                 else
                {
                    strOut.Append(chCur);
                }
                break;
            }

            case EStates::WaitType :
            {
                if (chCur == kCIDLib::chPoundSign)
                    eState = EStates::WaitRadix;
                else if (chCur == kCIDLib::chLatin_a)
                    eState = EStates::WaitAmp2;
                else if (chCur == kCIDLib::chLatin_g)
                    eState = EStates::WaitGT;
                else if (chCur == kCIDLib::chLatin_l)
                    eState = EStates::WaitLT;
                else if (chCur == kCIDLib::chLatin_q)
                    eState = EStates::WaitQuot2;
                else
                    ThrowBadCharRef(pszCur - pszStart);
                break;
            }

            case EStates::WaitGT :
            {
                if (chCur == kCIDLib::chLatin_t)
                {
                    eState = EStates::WaitSemi;
                    strOut.Append(kCIDLib::chGreaterThan);
                }
                 else
                {
                    ThrowBadCharRef(pszCur - pszStart);
                }
                break;
            }

            case EStates::WaitLT :
            {
                if (chCur == kCIDLib::chLatin_t)
                {
                    eState = EStates::WaitSemi;
                    strOut.Append(kCIDLib::chLessThan);
                }
                 else
                {
                    ThrowBadCharRef(pszCur - pszStart);
                }
                break;
            }

            case EStates::WaitAmp2 :
            {
                if (chCur == kCIDLib::chLatin_m)
                    eState = EStates::WaitAmp3;
                else
                    ThrowBadCharRef(pszCur - pszStart);
                break;
            }

            case EStates::WaitAmp3 :
            {
                if (chCur == kCIDLib::chLatin_p)
                {
                    eState = EStates::WaitSemi;
                    strOut.Append(kCIDLib::chAmpersand);
                }
                 else
                {
                    ThrowBadCharRef(pszCur - pszStart);
                }
                break;
            }

            case EStates::WaitQuot2 :
            {
                if (chCur == kCIDLib::chLatin_u)
                    eState = EStates::WaitQuot3;
                else
                    ThrowBadCharRef(pszCur - pszStart);
                break;
            }

            case EStates::WaitQuot3 :
            {
                if (chCur == kCIDLib::chLatin_o)
                    eState = EStates::WaitQuot4;
                else
                    ThrowBadCharRef(pszCur - pszStart);
                break;
            }

            case EStates::WaitQuot4 :
            {
                if (chCur == kCIDLib::chLatin_t)
                {
                    eState = EStates::WaitSemi;
                    strOut.Append(kCIDLib::chQuotation);
                }
                 else
                {
                    ThrowBadCharRef(pszCur - pszStart);
                }
                break;
            }

            case EStates::WaitRadix :
            {
                if ((chCur == kCIDLib::chLatin_x) || (chCur == kCIDLib::chLatin_X))
                {
                    eRadix = tCIDLib::ERadices::Hex;
                    eState = EStates::WaitNum1;
                }
                 else if (TRawStr::bIsDigit(chCur))
                {
                    eState = EStates::WaitNum2;
                    c2NumRef = tCIDLib::TCard2(chCur - 0x30);
                }
                 else
                {
                    ThrowBadCharRef(pszCur - pszStart);
                }
                break;
            }

            case EStates::WaitNum1 :
            {
                if (TRawStr::bIsHexDigit(chCur))
                {
                    if (eRadix == tCIDLib::ERadices::Dec)
                        c2NumRef = tCIDLib::TCard2(chCur - 0x30);
                    else
                        c2NumRef = tCIDLib::TCard2(TRawStr::c4ToHex(chCur));
                }
                 else
                {
                    ThrowBadCharRef(pszCur - pszStart);
                }
                break;
            }

            case EStates::WaitNum2 :
            case EStates::WaitNum3 :
            case EStates::WaitNum4 :
            {
                if (chCur == kCIDLib::chSemiColon)
                {
                    // Put out the numeric char and go back to start
                    strOut.Append(tCIDLib::TCh(c2NumRef));
                    eState = EStates::WaitRef;
                }
                 else if (TRawStr::bIsHexDigit(chCur))
                {
                    if (eRadix == tCIDLib::ERadices::Dec)
                    {
                        c2NumRef *= 10;
                        c2NumRef += tCIDLib::TCard2(chCur - 0x30);
                    }
                     else
                    {
                        c2NumRef <<= 4;
                        c2NumRef |= tCIDLib::TCard2(TRawStr::c4ToHex(chCur));
                    }

                    // Moves us to up to the next number or to semicolon
                    eState = EStates(tCIDLib::c4EnumOrd(eState) + 1);

                    // If we got to semi-colon, go ahead and put it out
                    if (eState == EStates::WaitSemi)
                        strOut.Append(tCIDLib::TCh(c2NumRef));
                }
                 else
                {
                    ThrowBadCharRef(pszCur - pszStart);
                }
                break;
            }

            case EStates::WaitSemi :
            {
                if (chCur != kCIDLib::chSemiColon)
                    ThrowBadCharRef(pszCur - pszStart);
                eState = EStates::WaitRef;
                break;
            }
        };
    }
}


//
//  This method will build a basic authorization string, to help client
//  side apps handle authorization.
//
tCIDLib::TVoid
THTTPClient::MakeBasicAuthStr(  const   TString&    strUserName
                                , const TString&    strPassword
                                ,       TString&    strOut)
{

    // Concatenate the user name, colon, and password
    TString strHashSrc(strUserName);
    strHashSrc.Append(kCIDLib::chColon);
    strHashSrc.Append(strPassword);

    // Create text streams over the source and the output string
    TTextStringInStream strmSrc(tCIDLib::ForceMove(strHashSrc));
    TTextStringOutStream strmTar(&strOut);
    strmTar.Reset();

    //
    //  Create a base64 object, and be sure to set the line width really
    //  big so it won't try to wrap the line. This will be put on a header
    //  line which can't wrap. And then encode from the source to the
    //  target.
    //
    TBase64 b64Hash;
    b64Hash.c4LineWidth(512);
    b64Hash.Encode(strmSrc, strmTar);
    strmTar.Flush();
}


//
//  Takes a WWW-Authenticate header line value and parses out the info. There's
//  a fair bit of grunt work involved, so best to have it in one place.
//
//  The format is a list of comma separated key=value pairs, where the values
//  can be quoted, so that they can have spaces in them.
//
tCIDLib::TVoid
THTTPClient::ParseAuthReq(  const   TString&                    strReqLine
                            ,       tCIDNet::EHTTPAuthTypes&    eType
                            ,       TString&                    strRealm
                            ,       TString&                    strNonce
                            ,       TString&                    strOpaque
                            ,       TString&                    strDomain
                            ,       TString&                    strAlgorithm)
{
    //
    //  This one is complicated enough that we use a little state machine
    //  to pull the values out. But, first we have to get the first token
    //  which is the authentication type, then the key=value pairs start.
    //
    const tCIDLib::TCard4   c4End = strReqLine.c4Length();
    tCIDLib::TCard4         c4Index = 0;

    TString strType;
    while (c4Index < c4End)
    {
        const tCIDLib::TCh chCur = strReqLine[c4Index++];
        if (TRawStr::bIsSpace(chCur))
            break;
        strType.Append(chCur);
    }

    // Make sure it's an understood authentication type
    strType.StripWhitespace();
    if (strType.bCompareI(L"Basic"))
        eType = tCIDNet::EHTTPAuthTypes::Basic;
    else if (strType.bCompareI(L"Digest"))
        eType = tCIDNet::EHTTPAuthTypes::Digest;
    else
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcHTTP_UnknownAuthType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Authority
            , strType
        );
    }

    // OK, now let's parse the rest of it as key=value pairs
    enum class EStates
    {
        Name
        , Quote
        , Value
        , Comma
    };
    tCIDLib::TBoolean       bInQuote = kCIDLib::False;
    EStates                 eCurState = EStates::Name;
    TString                 strName;
    TString                 strValue;

    //
    //  Note that we don't increase the index until after we've processed
    //  each character, otherwise we can fall out a character too early
    //  in some cases.
    //
    while (c4Index <= c4End)
    {
        //
        //  Look for the next non-space unless we are in the quoted part, else
        //  just take the next character.
        //
        tCIDLib::TCh chCur;
        while (c4Index < c4End)
        {
            chCur = strReqLine[c4Index];
            if (bInQuote || !TRawStr::bIsSpace(chCur))
                break;
            c4Index++;
        }

        if (c4Index == c4End)
        {
            //
            //  If in value mode, fake a comma, to terminate an current value,
            //  else just break out and we are done. We'll fall out anyway
            //  on the next round since the main loop check above will fail.
            //
            //  Otherwise we'd have to duplicate the code for storing a
            //  value. This way we can force the same code to be used.
            //
            if (eCurState == EStates::Value)
                chCur = kCIDLib::chComma;
            else
                break;
        }

        switch(eCurState)
        {
            case EStates::Name :
                if (chCur == kCIDLib::chEquals)
                    eCurState = EStates::Quote;
                else
                    strName.Append(chCur);
                break;

            case EStates::Quote :
                //
                //  If we start with a quote, then remember that, else just
                //  store the first char of the value.
                //
                strValue.Clear();
                if (chCur == kCIDLib::chQuotation)
                    bInQuote = kCIDLib::True;
                else
                    strValue.Append(chCur);
                eCurState = EStates::Value;
                break;

            case EStates::Value :
                //
                //  We are either storing another character into the value,
                //  or the value is done and we need to store it.
                //
                if ((chCur == kCIDLib::chComma) && !bInQuote)
                {
                    //
                    //  If not in a quote, then a comma ends the value. Else
                    //  it's just part of the value and wil be handled below.
                    //
                    eCurState = EStates::Name;
                }
                 else if (chCur == kCIDLib::chQuotation)
                {
                    if (!bInQuote)
                    {
                        // This is not valid, an unbalanced quote
                        facCIDNet().ThrowErr
                        (
                            CID_FILE
                            , CID_LINE
                            , kNetErrs::errcHTTP_UnexpectedQuote
                            , tCIDLib::ESeverities::Failed
                            , tCIDLib::EErrClasses::Authority
                        );
                    }

                    // Have to see a comma now before another value
                    eCurState = EStates::Comma;
                }
                 else if (TRawStr::bIsSpace(chCur))
                {
                    //
                    //  If not in quotes, then any space ends a value, and
                    //  we need to look for the comma. Else it's just part of
                    //  the value.
                    //
                    if (bInQuote)
                        strValue.Append(chCur);
                    else
                        eCurState = EStates::Comma;
                }
                 else
                {
                    // Nothing special, just append to the value
                    strValue.Append(chCur);
                }

                //
                //  If not still in value state, then we got a value and
                //  need to store it.
                //
                if (eCurState != EStates::Value)
                {
                    if (strName.bCompareI(L"algorithm"))
                        strAlgorithm = strValue;
                    else if (strName.bCompareI(L"domain"))
                        strDomain = strValue;
                    else if (strName.bCompareI(L"nonce"))
                        strNonce = strValue;
                    else if (strName.bCompareI(L"opaque"))
                        strOpaque = strValue;
                    else if (strName.bCompareI(L"realm"))
                        strRealm = strValue;

                    // Now reset the name and value and quote flag
                    bInQuote = kCIDLib::False;
                    strName.Clear();
                    strValue.Clear();
                }
                break;

            case EStates::Comma :
                // We have to see a comma before another non-whitespace
                if (chCur == kCIDLib::chComma)
                {
                    eCurState = EStates::Name;
                }
                 else if (!TRawStr::bIsSpace(chCur))
                {
                    facCIDNet().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kNetErrs::errcHTTP_ExpectedComma
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Authority
                    );
                }
                break;
        };

        // Move forward
        c4Index++;
    }

    //
    //  If we get here, we have to be in either name or comma state. In
    //  theory we shouldn't have seen any name characters at this point,
    //  but we'll ignore that.
    //
    if ((eCurState != EStates::Name) && (eCurState != EStates::Comma))
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcHTTP_UnexpectedEOL
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Authority
        );
    }
}


//
//  Takes the value of a Cache-Control header line and parses out the
//  individual values, putting them into the passed list of key/value
//  pairs.
//
tCIDLib::TVoid
THTTPClient::ParseCacheControl( const   TString&                strSrc
                                ,       tCIDLib::TKVPCollect&   colToFill)
{
    colToFill.RemoveAll();

    // We want to break it into comma separated tokens
    TStringTokenizer stokSrc(&strSrc, L",");
    TString strKey;
    TString strValue;
    while (kCIDLib::True)
    {
        if (!stokSrc.bGetNextToken(strKey))
            break;

        // We got one. Split it on the equals sign if there is one
        if (!strKey.bSplit(strValue, kCIDLib::chEquals, kCIDLib::False))
            strValue.Clear();

        colToFill.objAdd(TKeyValuePair(strKey, strValue));
    }
}



// Sends a standard error reply
tCIDLib::TVoid
THTTPClient::SendErrReply(          TCIDDataSrc&        cdsTar
                            , const TString&            strServer
                            , const tCIDLib::TCard4     c4ErrCode
                            , const TString&            strRepText
                            , const TString&            strBodyText
                            , const tCIDLib::TBoolean   bCloseFlag)
{
    try
    {
        // Set up memory buffer based text stream. Use one of our I/O streams
        TTextMBufOutStream strmTar(4192, 16 * 1024, new TUTF8Converter);
        strmTar.eNewLineType(tCIDLib::ENewLineTypes::CRLF);

        // We'll need a date field, which must be UTC format in a particular format.
        TTime tmReply(tCIDLib::ESpecialTimes::CurrentUTC);
        tmReply.strDefaultFormat(TTime::strCTime());

        // Output the header stuff
        strmTar << L"HTTP/1.0 " << c4ErrCode << L' ' << strRepText << kCIDLib::NewLn
                << THTTPClient::strHdr_ContType
                << L": text/html; charset=utf-8\n"
                << L"Server: " << strServer << kCIDLib::NewLn
                << L"Date: " << tmReply << kCIDLib::NewLn;

        // Put out a close indicator if asked
        if (bCloseFlag)
            strmTar << L"Connect: Close\n";

        // And put out the separator empty line
        strmTar << kCIDLib::NewLn;

        // Build the standard error reply body into the buffer after the header
        BuildErrReply(c4ErrCode, strBodyText, strmTar);

        // And now we can send the accumulated data
        strmTar.Flush();
        cdsTar.WriteBytes(strmTar.mbufData(), strmTar.c4CurSize());
        cdsTar.FlushOut(TTime::enctNowPlusSecs(5));
    }

    catch(TError& errToCatch)
    {
        //
        //  If we failed because the socket is not connected, then don't log
        //  error, because it doesn't matter that we couldn't send the error
        //  reply.
        //
        if (!errToCatch.bCheckEvent(L"CIDSock", kSockErrs::errcSock_NotConnected)
        &&  facCIDNet().bTestLog(errToCatch, tCIDNet::ELogFlags::HTTPClient))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
}



// ---------------------------------------------------------------------------
//  HTTPClient: Constructors and Destructor
// ---------------------------------------------------------------------------
THTTPClient::THTTPClient() :

    TNetCoreParser(L": ")
    , m_bAutoAuth(kCIDLib::False)
    , m_c4ReqNum(1)
    , m_c4WaitConnSecs(5)
    , m_eAddrType(tCIDSock::EAddrTypes::IPV4)
    , m_ippnProxy(0)
    , m_strRepServer(L"CIDLib HTTP Services")
{
}

THTTPClient::~THTTPClient()
{
}


// ---------------------------------------------------------------------------
//  THTTPClient: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  The base parser class calls us here when he's gotten through the header of
//  of an incoming message. We have to do basic validation of the info and get
//  some other info out for him so he can make some decisions.
//
//  For validation:
//
//  1. We make sure the first line can be parsed
//  2. That it contains a valid request type for us if server side
//  3. That the protocol type is valid if server side
//
//  And we give back this info
//
//  1. If the connection is to be closed, we set bClose. If the protocol is 1.0,
//     we assume close and set it even if there is no connection close header. We
//     don't deal with 1.0 keepalive stuff.
//  2. If the other side is doing a persistent connection (HTTP 1.1 in our case)
//     then we set bPersist.
//  3. We find the content type and give it back
//  4. We return the content length, or return max card if it's not provided.
//  5. We give back the broken out three parameters that are on the first line of
//     of a message in either direction.
//
tCIDNet::ENetPReadRes
THTTPClient::eBasicMsgVal(  const   tCIDNet::EReadTypes     eReadType
                            , const tCIDLib::TKVPCollect&   colHdrLines
                            , const TString&                strFirstLn
                            ,       tCIDLib::TBoolean&      bClose
                            ,       tCIDLib::TBoolean&      bPersist
                            ,       TString&                strContType
                            ,       tCIDLib::TCard4&        c4ContLen
                            ,       TString&                strParm1
                            ,       TString&                strParm2
                            ,       TString&                strParm3)
{
    //
    //  Parse the first line. Return error if it fails. This is a generic parse
    //  that just looks for three parameters.
    //
    if (!bParseFirstLn(strFirstLn, strParm1, strParm2, strParm3))
        return tCIDNet::ENetPReadRes::BadHdrLine;

    // Do some client and server side specific checks.
    if (eReadType == tCIDNet::EReadTypes::Server)
    {
        // Make sure it's a valid request type for us
        if (!bValidateReqType(strParm1))
            return tCIDNet::ENetPReadRes::InvalidMsg;

        // Make sure it's a valid protocol for us
        if (!bValidateProto(strParm3))
            return tCIDNet::ENetPReadRes::InvalidMsg;
    }

    // Indicate whether or not this is a persistent type connection (1.1)
    bPersist = strParm3.eCompareI(L"HTTP/1.1") == tCIDLib::ESortComps::Equal;

    //
    //  Check for a content length, returning max card if one is not set
    //  explicitly.
    //
    c4ContLen = kCIDLib::c4MaxCard;
    TString strVal;
    if (bFindHdrLine(colHdrLines, THTTPClient::strHdr_ContLen, strVal))
        c4ContLen = strVal.c4Val(tCIDLib::ERadices::Dec);

    // Check for a content type. If not found, return nothing
    if (!bFindHdrLine(colHdrLines, THTTPClient::strHdr_ContType, strContType))
        strContType.Clear();

    // Check for a connection close
    bClose = kCIDLib::False;
    TString strTmp;
    if (bFindHdrLine(colHdrLines, THTTPClient::strHdr_Connection, strTmp))
        bClose = strTmp.eCompareI(L"close") == tCIDLib::ESortComps::Equal;

    return tCIDNet::ENetPReadRes::Success;
}




// ---------------------------------------------------------------------------
//  THTTPClient: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Get or set the auto-authorization flag
tCIDLib::TBoolean THTTPClient::bAutoAuth() const
{
    return m_bAutoAuth;
}

tCIDLib::TBoolean THTTPClient::bAutoAuth(const tCIDLib::TBoolean bToSet)
{
    m_bAutoAuth = bToSet;
    return m_bAutoAuth;
}



//
//  Handles getting a response back from the server in response to a GET or
//  POST. Returns all the information, but if the reply is malformed it will
//  throw.
//
tCIDLib::TCard4
THTTPClient::c4GetSrvReply(         TCIDDataSrc&            cdsSrc
                            , const tCIDLib::TEncodedTime   enctEnd
                            , const tCIDLib::TBoolean       bNoCont
                            ,       TString&                strRepType
                            ,       tCIDNet::EHTTPCodes&    eCodeType
                            ,       TString&                strRepText
                            ,       tCIDLib::TKVPCollect&   colOutHdrLines
                            ,       TString&                strContType
                            ,       TMemBuf&                mbufCont
                            ,       tCIDLib::TCard4&        c4ContLen)
{
    tCIDLib::TCard4 c4Status = 0;

    try
    {
        //
        //  Call our base class to do the basic message read. It returns us some
        //  useful info. In our case we pass the caller's output strings in the order
        //  expected for a server response first line (protocol, then status code,
        //  then reply text.)
        //
        TString strProto;
        TString strCode;
        TString strFirstLn;
        const tCIDNet::ENetPReadRes eRes = eGetMsg
        (
            cdsSrc
            , tCIDNet::EReadTypes::Client
            , enctEnd
            , bNoCont
            , strFirstLn
            , colOutHdrLines
            , mbufCont
            , c4ContLen
            , strContType
            , strProto
            , strCode
            , strRepText
        );

        // If not success, then throw
        if (eRes != tCIDNet::ENetPReadRes::Success)
        {
            //
            //  Get the loggable text for the returned result and pass that
            //  as the alt text. Use some of the above strings as temps to
            //  get the output strings. The second one is the error msg text.
            //
            c4Status = c4ReadResMsg(eRes, strFirstLn, strRepText);

            facCIDNet().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kNetErrs::errcHTTP_ReqFailed
                , strRepText
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }

        // Convert the return code string to binary
        if (!strCode.bToCard4(c4Status, tCIDLib::ERadices::Dec))
        {
            facCIDNet().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kNetErrs::errcHTTP_BadStatusCode
                , strCode
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
            );
        }

        eCodeType = tCIDNet::EHTTPCodes::Unknown;
        if (c4Status < 600)
            eCodeType = tCIDNet::EHTTPCodes(c4Status / 100);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    return c4Status;
}


tCIDLib::TCard4 THTTPClient::
c4SendGet(          TCIDDataSrc* const      pcdsSrc
            , const TURL&                   urlToGet
            , const tCIDLib::TEncodedTime   enctEnd
            , const TString&                strAgent
            , const TString&                strAccept
            ,       tCIDNet::EHTTPCodes&    eCodeType
            ,       TString&                strRepText
            ,       tCIDLib::TKVPCollect&   colOutHdrs
            ,       TString&                strContType
            ,       TMemBuf&                mbufCont
            ,       tCIDLib::TCard4&        c4ContLen
            , const tCIDLib::TBoolean       bOutwardBody
            , const tCIDLib::TKVPCollect&   colInHdrLines)
{
    tCIDLib::TCard4 c4Res = 0;
    try
    {
        // Do the send side of the GET operation
        c4Res = c4DoOp
        (
            pcdsSrc
            , urlToGet
            , enctEnd
            , kCIDNet::pszHTTP_GET
            , strAgent
            , strAccept
            , eCodeType
            , strRepText
            , colOutHdrs
            , strContType
            , mbufCont
            , c4ContLen
            , bOutwardBody
            , colInHdrLines
            , 0
        );


        //
        //  If it's an auth request, and our auto-authorization is enabled,
        //  then try to handle that and reissue it.
        //
        if ((c4Res == kCIDNet::c4HTTPStatus_Unauthorized) && m_bAutoAuth)
        {
            //
            //  If we have no auth info, or we can't understand the
            //  authorization challenge info we got, then just return
            //  the unauthorized error code.
            //
            TString strAuthInfo;
            TString strPass;
            TString strUser;
            if (!bHaveAuthInfo(urlToGet, strUser, strPass)
            ||  !bBuildAuthStr(colOutHdrs, strUser, strPass, urlToGet, m_c4ReqNum, strAuthInfo))
            {
                return c4Res;
            }

            // Set up an extra pair to pass in
            TKeyValuePair kvalAuth(L"Authorization", strAuthInfo);

            // And re-issue it
            c4Res = c4DoOp
            (
                pcdsSrc
                , urlToGet
                , enctEnd
                , kCIDNet::pszHTTP_GET
                , strAgent
                , strAccept
                , eCodeType
                , strRepText
                , colOutHdrs
                , strContType
                , mbufCont
                , c4ContLen
                , bOutwardBody
                , colInHdrLines
                , &kvalAuth
            );
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return c4Res;
}


//
//  This is a helper that just works in terms of c4SendGet above, but deals with
//  redirections by re-issuing the get for redirected URLs, returning the ultimate
//  URL that we ended up getting. The caller provides a counter pointer to a data
//  source, which will we will replce if redirected. They'll get back the final
//  one.
//
//  We don't support an outbound body here because it introduces complications for
//  a generic redirection mechanism.
//
tCIDLib::TCard4
THTTPClient::
c4SendGetRedir(         TCIDDataSrcJan&         janSrc
                , const TURL&                   urlToGet
                , const tCIDLib::TEncodedTime   enctEnd
                , const TString&                strAgent
                , const TString&                strAccept
                ,       tCIDNet::EHTTPCodes&    eCodeType
                ,       TString&                strRepText
                ,       tCIDLib::TKVPCollect&   colOutHdrLines
                ,       TString&                strContType
                ,       TMemBuf&                mbufCont
                ,       tCIDLib::TCard4&        c4ContLen
                ,       TString&                strUltimateURL
                , const tCIDLib::TKVPCollect&   colInHdrLines)
{
    tCIDLib::TCard4 c4Res = 0;
    try
    {
        //
        //  For sanity's sake, we'll only do 4 layers. If it goes beyond
        //  that, it's probably some sort of scam.
        //
        tCIDLib::TCard4 c4LpCount = 0;
        TURL urlActual = urlToGet;
        TURL urlNew;
        TString strHdrVal;
        while (c4LpCount < 4)
        {
            //
            //  We call the public get method, so that we'll get auto-auth if it
            //  happens on any of the URLs.
            //
            c4ContLen = 0;
            c4Res = c4SendGet
            (
                janSrc.pcdsData()
                , urlActual
                , enctEnd
                , strAgent
                , strAccept
                , eCodeType
                , strRepText
                , colOutHdrLines
                , strContType
                , mbufCont
                , c4ContLen
                , kCIDLib::False
                , colInHdrLines
            );

            if ((c4Res != kCIDNet::c4HTTPStatus_MovedPerm)
            &&  (c4Res != kCIDNet::c4HTTPStatus_MovedTemp)
            &&  (c4Res != kCIDNet::c4HTTPStatus_TempRedir)
            &&  (c4Res != kCIDNet::c4HTTPStatus_PermRedir))
            {
                break;
            }

            //
            //  It's redirected, so find the Location line. If not found,
            //  then break out with the redirected status.
            //
            TString strLoc;
            if (!bFindHdrLine(colOutHdrLines, strHdr_Location, strLoc))
                break;

            //
            //  Set up the new URL. Just in case, handle the situation
            //  where it might be relative to the original, or fully
            //  qualified. We have to do it to a temp the copy that back
            //  to the actual one.
            //
            urlNew.Reset(urlActual, strLoc);
            urlActual = urlNew;

            //
            //  If we don't have an incoming source, or we had one but the server
            //  indicated close, then let's create a new one for the new URL. Else
            //  just ask the current one to reset its buffers.
            //
            if (!janSrc.pcdsData()
            ||  (bFindHdrLine(colOutHdrLines, strHdr_Connection, strHdrVal)
            &&   strHdrVal.bCompareI(L"Close")))
            {
                //
                //  Create a new data source based on the new URL, and set it on
                //  the janitor, replacing any original value.
                //
                TCIDDataSrc* pcdsNew = nullptr;

                TClientStreamSocket* psockLocal = new TClientStreamSocket
                (
                    tCIDSock::ESockProtos::TCP, urlActual.ipepHost()
                );

                // Set it up on a data source
                try
                {
                    if (urlToGet.eProto() == tCIDSock::EProtos::HTTP)
                    {
                        pcdsNew = new TCIDSockStreamDataSrc(psockLocal, tCIDLib::EAdoptOpts::Adopt);
                    }
                     else if (urlToGet.eProto() == tCIDSock::EProtos::HTTPS)
                    {
                        tCIDLib::TStrList colALPN;
                        pcdsNew = new TCIDSChanClDataSrc
                        (
                            L"HTTPClient"
                            , psockLocal
                            , tCIDLib::EAdoptOpts::Adopt
                            , TString::strEmpty()
                            , colALPN
                            , tCIDSChan::EConnOpts::None
                            , urlToGet.strHost()
                        );
                    }
                     else
                    {
                        facCIDNet().ThrowErr
                        (
                            CID_FILE
                            , CID_LINE
                            , kNetErrs::errcHTTP_UnsupportedProto
                            , tCIDLib::ESeverities::Failed
                            , tCIDLib::EErrClasses::NotSupported
                        );
                    }
                }

                catch(...)
                {
                    delete psockLocal;
                    throw;
                }

                janSrc.SetNew(pcdsNew, tCIDLib::EAdoptOpts::Adopt, kCIDLib::True, enctEnd);
            }
             else
            {
                janSrc.cdsData().ResetBufs();
            }

            // Bump the loop count and try to get this new redirection
            c4LpCount++;
        }

        // Return the URL we ultimate did, if we succeeded
        if (c4Res == kCIDNet::c4HTTPStatus_OK)
        {
            //
            //  Query the text, with no expansion, exclusing any
            //  user/password info.
            //
            urlActual.QueryText
            (
                strUltimateURL, kCIDLib::False, kCIDLib::False
            );
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return c4Res;
}


// To do a HEAD request, which never returns any body
tCIDLib::TCard4 THTTPClient::
c4SendHead(         TCIDDataSrc* const      pcdsSrc
            , const TURL&                   urlToGet
            , const tCIDLib::TEncodedTime   enctEnd
            , const TString&                strAgent
            , const TString&                strAccept
            ,       tCIDNet::EHTTPCodes&    eCodeType
            ,       TString&                strRepText
            ,       tCIDLib::TKVPCollect&   colOutHdrLines
            ,       TString&                strContType
            ,       tCIDLib::TCard4&        c4ContLen
            , const tCIDLib::TKVPCollect&   colInHdrLines)
{
    tCIDLib::TCard4 c4Res = 0;
    try
    {
        //
        //  There's no outgoing content for a HEAD, and none coming
        //  bakc out. So pass in false for the outgoing body and 0
        //  outgoing cont length. Create a small dummy buffer since
        //  we have to pass one.
        //
        c4ContLen = 0;
        THeapBuf mbufDummy(8, 8);

        c4Res = c4DoOp
        (
            pcdsSrc
            , urlToGet
            , enctEnd
            , kCIDNet::pszHTTP_HEAD
            , strAgent
            , strAccept
            , eCodeType
            , strRepText
            , colOutHdrLines
            , strContType
            , mbufDummy
            , c4ContLen
            , kCIDLib::False
            , colInHdrLines
            , 0
        );
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return c4Res;
}


//
//  This method does the common POST scheme, where are you posting a set of
//  URL encoded key=value pairs, i.e. the kind you'd do from a form. The
//  colBody parm provides the content for the body. You can optionally pass
//  in a list of incoming header lines as well, to put into the message
//  header. Everything else is the usual stuff, and you just get back the
//  response body and header lines and type and so forth.
//
tCIDLib::TCard4 THTTPClient::
c4SendPost(         TCIDDataSrc* const      pcdsSrc
            , const TURL&                   urlToGet
            , const tCIDLib::TEncodedTime   enctEnd
            , const TString&                strAgent
            , const TString&                strAccept
            , const tCIDLib::TKVPCollect&   colBody
            ,       tCIDNet::EHTTPCodes&    eCodeType
            ,       TString&                strRepText
            ,       tCIDLib::TKVPCollect&   colOutHdrLines
            ,       TString&                strContType
            ,       TMemBuf&                mbufCont
            ,       tCIDLib::TCard4&        c4ContLen
            , const tCIDLib::TKVPCollect&   colInHdrLines)
{
    tCIDLib::TCard4 c4Res = 0;
    try
    {
        // Build up the outgoing URL encoded data
        TString strBody(1024UL);
        TColCursor<TKeyValuePair>* pcursBody = colBody.pcursNew();
        TJanitor<TColCursor<TKeyValuePair> > janCurs(pcursBody);
        if (pcursBody->bReset())
        {
            do
            {
                const TKeyValuePair& kvalCur = pcursBody->objRCur();

                if (!strBody.bIsEmpty())
                    strBody.Append(kCIDLib::chAmpersand);

                strBody.Append(kvalCur.strKey());
                strBody.Append(kCIDLib::chEquals);

                // Have to encode the value using query rules
                TURL::EncodeTo
                (
                    kvalCur.strValue()
                    , strBody
                    , TURL::EExpTypes::Query
                    , tCIDLib::EAppendOver::Append
                );
            }   while (pcursBody->bNext());
        }

        //
        //  Transcode the body text into the caller's buffer which we use
        //  as a temp buffer. We'll use a UTF-8 transcoded, and that's what
        //  we put into the charset= value in the header. This should support
        //  any kind of content. We get the transcoded length, which is what
        //  we pass in as the outgoing content length.
        //
        c4ContLen = 0;
        {
            TUTF8Converter tcvtBody;
            tcvtBody.c4ConvertTo(strBody, mbufCont, c4ContLen);
        }

        //
        //  And now call the common op method. Put our type temporarily
        //  into the caller's content type string so we can pass it in
        //  for the outgoing.
        //
        strContType = L"application/x-www-form-urlencoded; charset=utf-8";
        c4Res = c4DoOp
        (
            pcdsSrc
            , urlToGet
            , enctEnd
            , kCIDNet::pszHTTP_POST
            , strAgent
            , strAccept
            , eCodeType
            , strRepText
            , colOutHdrLines
            , strContType
            , mbufCont
            , c4ContLen
            , kCIDLib::True
            , colInHdrLines
            , 0
        );
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return c4Res;
}



//
//  This variation is for when you want to do a post with body content, or
//  just with a header. If bOutwardBody is true, then we expect the content
//  type, length, and memory buffer to contain data for the body to send out,
//  in addition to being used to hand back the reply data. Else, they are just
//  for reply data.
//
//  We can just turn this one into a simple call to the common op method
//
tCIDLib::TCard4 THTTPClient::
c4SendPost(         TCIDDataSrc* const      pcdsSrc
            , const TURL&                   urlToGet
            , const tCIDLib::TEncodedTime   enctEnd
            , const TString&                strAgent
            , const TString&                strAccept
            ,       tCIDNet::EHTTPCodes&    eCodeType
            ,       TString&                strRepText
            ,       tCIDLib::TKVPCollect&   colOutHdrLines
            ,       TString&                strContType
            ,       TMemBuf&                mbufCont
            ,       tCIDLib::TCard4&        c4ContLen
            , const tCIDLib::TBoolean       bOutwardBody
            , const tCIDLib::TKVPCollect&   colInHdrLines)
{
    tCIDLib::TCard4 c4Res = 0;
    try
    {
        c4Res = c4DoOp
        (
            pcdsSrc
            , urlToGet
            , enctEnd
            , kCIDNet::pszHTTP_POST
            , strAgent
            , strAccept
            , eCodeType
            , strRepText
            , colOutHdrLines
            , strContType
            , mbufCont
            , c4ContLen
            , bOutwardBody
            , colInHdrLines
            , 0
        );
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return c4Res;
}


tCIDLib::TCard4 THTTPClient::
c4SendPut(          TCIDDataSrc* const      pcdsSrc
            , const TURL&                   urlToGet
            , const tCIDLib::TEncodedTime   enctEnd
            , const TString&                strAgent
            , const TString&                strAccept
            ,       tCIDNet::EHTTPCodes&    eCodeType
            ,       TString&                strRepText
            ,       tCIDLib::TKVPCollect&   colOutHdrs
            ,       TString&                strContType
            ,       TMemBuf&                mbufCont
            ,       tCIDLib::TCard4&        c4ContLen
            , const tCIDLib::TBoolean       bOutwardBody
            , const tCIDLib::TKVPCollect&   colInHdrLines)
{
    tCIDLib::TCard4 c4Res = 0;
    try
    {
        // Do the basic PUT operation
        c4Res = c4DoOp
        (
            pcdsSrc
            , urlToGet
            , enctEnd
            , kCIDNet::pszHTTP_PUT
            , strAgent
            , strAccept
            , eCodeType
            , strRepText
            , colOutHdrs
            , strContType
            , mbufCont
            , c4ContLen
            , bOutwardBody
            , colInHdrLines
            , 0
        );

        //
        //  If it's an auth request, and our auto-authorization is enabled,
        //  then try to handle that and reissue it.
        //
        if ((c4Res == kCIDNet::c4HTTPStatus_Unauthorized) && m_bAutoAuth)
        {
            //
            //  If we have no auth info, or we can't understand the
            //  authorization challenge info we got, then just return
            //  the unauthorized error code.
            //
            TString strAuthInfo;
            TString strPass;
            TString strUser;
            if (!bHaveAuthInfo(urlToGet, strUser, strPass)
            ||  !bBuildAuthStr(colOutHdrs, strUser, strPass, urlToGet, m_c4ReqNum, strAuthInfo))
            {
                return c4Res;
            }

            // Set up an extra pair to pass in
            TKeyValuePair kvalAuth(L"Authorization", strAuthInfo);

            // And re-issue it
            c4Res = c4DoOp
            (
                pcdsSrc
                , urlToGet
                , enctEnd
                , kCIDNet::pszHTTP_PUT
                , strAgent
                , strAccept
                , eCodeType
                , strRepText
                , colOutHdrs
                , strContType
                , mbufCont
                , c4ContLen
                , bOutwardBody
                , colInHdrLines
                , &kvalAuth
            );
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return c4Res;
}



tCIDLib::TVoid THTTPClient::ClearProxy()
{
    m_ippnProxy = 0;
    m_strProxy.Clear();
}


tCIDLib::TVoid THTTPClient::ClearAuthInfo()
{
    m_strPassword.Clear();
    m_strUserName.Clear();
}


// Get or set the desired IP address family type
tCIDSock::EAddrTypes THTTPClient::eAddrType() const
{
    return m_eAddrType;
}

tCIDSock::EAddrTypes THTTPClient::eAddrType(const tCIDSock::EAddrTypes eToSet)
{
    m_eAddrType = eToSet;
    return m_eAddrType;
}



//
//  Read in a request from a client, and return the resulting info. The return
//  value indicates success or the basic reason for the failure. If a failure at
//  the point befoe we even can get any input, then the request text is updated
//  with an error msg to give some idea why.
//
tCIDNet::ENetPReadRes
THTTPClient::eGetClientMsg(         TCIDDataSrc&            cdsSrc
                            , const tCIDLib::TEncodedTime   enctEnd
                            ,       TString&                strReqType
                            ,       tCIDLib::TKVPCollect&   colHdrLines
                            ,       TString&                strContType
                            ,       TString&                strURL
                            ,       tCIDLib::TCard4&        c4ProtoVer
                            ,       TMemBuf&                mbufCont
                            ,       tCIDLib::TCard4&        c4ContLen
                            , const tCIDLib::TBoolean       bSendErrReply)
{
    // Init to no content until we prove otherwise
    c4ContLen = 0;

    try
    {
        //
        //  Call our base class to do the basic message read. In this case, we pass
        //  in the caller's output strings for a msg from a client, where the three
        //  first line values are request type, resource URL, and protocol.
        //
        TString strFirstLine;
        TString strProto;
        tCIDNet::ENetPReadRes eRes = eGetMsg
        (
            cdsSrc
            , tCIDNet::EReadTypes::Server
            , enctEnd
            , kCIDLib::False
            , strFirstLine
            , colHdrLines
            , mbufCont
            , c4ContLen
            , strContType
            , strReqType
            , strURL
            , strProto
        );

        //
        //  If we failed at the basic read level, then return an error to
        //  the caller. Based on the return value we will choose an HTTP
        //  error code to send back. An error message will be in the first
        //  line string for us to use.
        //
        if (eRes != tCIDNet::ENetPReadRes::Success)
        {
            if (bSendErrReply)
            {
                // Get the status code and reply text for this read result
                TString strRepText;
                TString strErrMsg;
                const tCIDLib::TCard4 c4Code = c4ReadResMsg(eRes, strRepText, strErrMsg);

                SendErrReply
                (
                    cdsSrc
                    , m_strRepServer
                    , c4Code
                    , strRepText
                    , strErrMsg
                );
            }
            return eRes;
        }

        // REturn the protocol as a numeric version
        if (strProto.bCompareI(L"HTTP/1.0"))
            c4ProtoVer = 0x0100;
        else if (strProto.bCompareI(L"HTTP/1.1"))
            c4ProtoVer = 0x0101;
        else
            return tCIDNet::ENetPReadRes::BadProtoVer;
    }

    catch(TError& errToCatch)
    {
        // Send a reply to the client if asked to
        if (bSendErrReply)
        {
            SendErrReply
            (
                cdsSrc
                , m_strRepServer
                , kCIDNet::c4HTTPStatus_SrvError
                , L"Srv Exception"
                , errToCatch.strErrText()
            );
        }

        if (facCIDNet().bTestLog(errToCatch, tCIDNet::ELogFlags::HTTPClient))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
        throw;
    }

    // It worked so return success
    return tCIDNet::ENetPReadRes::Success;
}



// Get the proxy port if any
tCIDLib::TIPPortNum THTTPClient::ippnProxy() const
{
    return m_ippnProxy;
}


const TString& THTTPClient::strProxy() const
{
    return m_strProxy;
}


tCIDLib::TVoid THTTPClient::Reset()
{
    // Currently just need to clear any proxy
    ClearProxy();
}


// Get/set the name used as the server name in any generated error replies
const TString& THTTPClient::strRepServer() const
{
    return m_strRepServer;
}

TString& THTTPClient::strRepServer(const TString& strToSet)
{
    m_strRepServer = strToSet;
    return m_strRepServer;
}


// Set authorization info
tCIDLib::TVoid
THTTPClient::SetAuthInfo(const TString& strUserName, const TString& strPassword)
{
    m_strPassword = strPassword;
    m_strUserName = strUserName;
}


// Set our proxy server. It's clearable using ClearProxy().
tCIDLib::TVoid
THTTPClient::SetProxy(  const   TString&                strToSet
                        , const tCIDLib::TIPPortNum     ippnProxy)
{
    m_ippnProxy = ippnProxy;
    m_strProxy = strToSet;
}



// ---------------------------------------------------------------------------
//  THTTPClient: Protected, virtual methods
// ---------------------------------------------------------------------------

// If not overridden we check for the standard HTTP request types that we support
tCIDLib::TBoolean THTTPClient::bValidateReqType(const TString& strToCheck)
{
    return
    (
        strToCheck.bCompareI(kCIDNet::pszHTTP_GET)
        || strToCheck.bCompareI(kCIDNet::pszHTTP_HEAD)
        || strToCheck.bCompareI(kCIDNet::pszHTTP_POST)
    );
}


// If not overridden we check for the standard HTTP 1.0 and 1.1 protocols
tCIDLib::TBoolean THTTPClient::bValidateProto(const TString& strToCheck)
{
    return
    (
        strToCheck.bCompareI(L"HTTP/1.0")
        || strToCheck.bCompareI(L"HTTP/1.1")
    );
}




// ---------------------------------------------------------------------------
//  THTTPClient: Private, static methods
// ---------------------------------------------------------------------------

//
//  Called from the public, static body text expander method above, to avoid
//  replicating this code over and over.
//
tCIDLib::TVoid THTTPClient::ThrowBadCharRef(const tCIDLib::TCard4 c4Ofs)
{
    facCIDNet().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kNetErrs::errcHTTP_BadCharRef
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Format
        , TCardinal(c4Ofs)
    );
}


// ---------------------------------------------------------------------------
//  THTTPClient: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Returns true if we have authentication info, and returns the username
//  and password if so.
//
tCIDLib::TBoolean
THTTPClient::bHaveAuthInfo( const   TURL&       urlSrc
                            ,       TString&    strUserName
                            ,       TString&    strPassword)
{
    // Start with URL based first
    strPassword = urlSrc.strPassword();
    strUserName = urlSrc.strUser();

    // If we don't have those, then see if we have values set
    if (strPassword.bIsEmpty() && strUserName.bIsEmpty())
    {
        strPassword = m_strPassword;
        strUserName = m_strUserName;
    }

    // Return true if both are not empty
    return !strPassword.bIsEmpty() && !strUserName.bIsEmpty();
}


//
//  All of the client side stuff above is in terms of this basic transaction
//  method. We send outgoing and get a response.
//
tCIDLib::TCard4
THTTPClient::c4DoOp(        TCIDDataSrc* const      pcdsSrc
                    , const TURL&                   urlToGet
                    , const tCIDLib::TEncodedTime   enctEnd
                    , const TString&                strOpType
                    , const TString&                strAgent
                    , const TString&                strAccept
                    ,       tCIDNet::EHTTPCodes&    eCodeType
                    ,       TString&                strRepText
                    ,       tCIDLib::TKVPCollect&   colOutHdrLines
                    ,       TString&                strContType
                    ,       TMemBuf&                mbufCont
                    ,       tCIDLib::TCard4&        c4ContLen
                    , const tCIDLib::TBoolean       bOutwardBody
                    , const tCIDLib::TKVPCollect&   colInHdrLines
                    , const TKeyValuePair* const    pkvalExLine)
{
    tCIDLib::TCard4 c4Ret = 0;
    try
    {
        // See if we got a data source or need to create a temporary one
        TCIDDataSrc*            pcdsLocal = 0;
        TCIDDataSrc*            pcdsToUse = pcdsSrc;
        TClientStreamSocket*    psockLocal = 0;
        TIPEndPoint             ipepActual;
        if (!pcdsToUse)
        {
            //
            //  If we have a proxy, then we use it's end point instead, else use
            //  the end point indicated in the URL.
            //
            if (m_strProxy.bIsEmpty())
                ipepActual = urlToGet.ipepHost(m_eAddrType);
            else
                ipepActual = TIPEndPoint(m_strProxy, m_ippnProxy, m_eAddrType);

            // Try to connect
            psockLocal = new TClientStreamSocket(tCIDSock::ESockProtos::TCP, ipepActual);

            // Set it up on a data source
            try
            {
                if (urlToGet.eProto() == tCIDSock::EProtos::HTTP)
                {
                    pcdsLocal = new TCIDSockStreamDataSrc(psockLocal, tCIDLib::EAdoptOpts::Adopt);
                }
                 else if (urlToGet.eProto() == tCIDSock::EProtos::HTTPS)
                {
                    tCIDLib::TStrList colALPN;
                    pcdsLocal = new TCIDSChanClDataSrc
                    (
                        L"HTTP Client"
                        , psockLocal
                        , tCIDLib::EAdoptOpts::Adopt
                        , TString::strEmpty()
                        , colALPN
                        , tCIDSChan::EConnOpts::None
                        , urlToGet.strHost()
                    );
                }
                 else
                {
                    facCIDNet().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kNetErrs::errcHTTP_UnsupportedProto
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::NotSupported
                    );
                }
            }

            catch(...)
            {
                delete psockLocal;
                throw;
            }
        }

        //
        //  If we created one, put a janitor on it, and try to connect. If we did not
        //  then the pointer is null and the janitor will do nothing.
        //
        TCIDDataSrcJan janLocal
        (
            pcdsLocal, tCIDLib::EAdoptOpts::Adopt, kCIDLib::True, enctEnd
        );

        if (pcdsLocal)
            pcdsToUse = pcdsLocal;

        //
        //  Break out the URL parts in their encoded forms that we need to
        //  send them.
        //
        TString strTmp;
        TString strHost;
        TString strUser;
        TString strPath;
        TString strSuffix;
        urlToGet.QueryEncodedForm(strTmp, strUser, strHost, strPath, strSuffix);

        //
        //  If we have a proxy, we need to send the entire URL. else just the
        //  path and suffix part.
        //
        if (m_strProxy.bIsEmpty())
        {
            // If the path is empty, then set it to /
            if (strPath.bIsEmpty())
                strPath = L"/";

            strPath.Append(strSuffix);
        }
         else
        {
            //
            //  Instead of trying to put all the bits back together, just
            //  query the full encoded URL again. We still needed the break
            //  out above so that we can get the host. And we also want to
            //  get rid of any username/password info in teh URL we actually
            //  send, so we tell it not to format that out. We also tell it
            //  not to expand, but give us the encoded form.
            //
            urlToGet.QueryText(strPath, kCIDLib::False, kCIDLib::False);
        }

        //
        //  Build up the first line. If the caller passed a data source of his own,
        //  we assume 1.1 and persistence. Else we do 1.0 since we created the source
        //  here locally and will destroy it on exit.
        //
        TString strFirstLn(strOpType);
        strFirstLn.Append(kCIDLib::chSpace);
        strFirstLn.Append(strPath);
		strFirstLn.Append(L" HTTP/1.1");

        //
        //  Set up the special header lines we take directly, if they are not
        //  already present.
        //
        tCIDLib::TKVPList colExHdrs;

        if (!TNetCoreParser::bHdrLineExists(colInHdrLines, L"Host") && !strHost.bIsEmpty())
            colExHdrs.objPlace(L"Host", strHost);

        if (!TNetCoreParser::bHdrLineExists(colInHdrLines, L"User-Agent") && !strAgent.bIsEmpty())
            colExHdrs.objPlace(L"User-Agent", strAgent);

        if (!TNetCoreParser::bHdrLineExists(colInHdrLines, L"Accept") && !strAccept.bIsEmpty())
            colExHdrs.objPlace(L"Accept", strAccept);

        //
        //  If 1.1, and there's no explict TE line, then we indicate we'll take
        //  chunked transfers plus trailers.
        //
        if (pcdsSrc && !TNetCoreParser::bHdrLineExists(colInHdrLines, strHdr_TE))
            colExHdrs.objPlace(strHdr_TE, L"trailers, chunked");

        //
        //  If outward body content have to add the content type. Either way we set
        //  the content length, insuring it's zero if the caller indicates no outward
        //  body.
        //
        if (bOutwardBody)
        {
            if (!TNetCoreParser::bHdrLineExists(colInHdrLines, THTTPClient::strHdr_ContType)
            &&  !strContType.bIsEmpty())
            {
                colExHdrs.objPlace(THTTPClient::strHdr_ContType, strContType);
            }
        }
         else
        {
            c4ContLen = 0;
        }

        if (!TNetCoreParser::bHdrLineExists(colInHdrLines, THTTPClient::strHdr_ContLen))
            colExHdrs.objPlace(strHdr_ContLen, c4ContLen);

        // They can pass us one extra one
        if (pkvalExLine)
            colExHdrs.objAdd(*pkvalExLine);

        //
        //  And send it. If there's any content that will be sent. If they told us
        //  no outward body we forced content length to zero above so we won't send
        //  any.
        //
        SendMsg
        (
            *pcdsToUse, enctEnd, strFirstLn, colInHdrLines, colExHdrs, mbufCont, c4ContLen
        );

        //
        //  If a HEAD tell it not to read any body text, even if there's
        //  content length reported.
        //
        tCIDLib::TBoolean bNoCont = kCIDLib::False;
        if (strOpType.bCompareI(kCIDNet::pszHTTP_HEAD))
            bNoCont = kCIDLib::True;

        // Parse out all the reply data and return the result code
        c4ContLen = 0;
        TString strRepType;
        c4Ret = c4GetSrvReply
        (
            *pcdsToUse
            , enctEnd
            , bNoCont
            , strRepType
            , eCodeType
            , strRepText
            , colOutHdrLines
            , strContType
            , mbufCont
            , c4ContLen
        );
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return c4Ret;
}


//
//  Given a generic msg read result enum, we map that to a status code and
//  reply text to be sent back to the client, and a loggable msg (which may also
//  be put into the body of an error reply.
//
//
tCIDLib::TCard4
THTTPClient::c4ReadResMsg(  const   tCIDNet::ENetPReadRes   eRes
                            ,       TString&                strResText
                            ,       TString&                strErrMsg)
{
    struct TReadResMap
    {
        tCIDNet::ENetPReadRes   eResToMap;
        const tCIDLib::TCh*     pszResText;
        tCIDLib::TCard4         c4Status;
        tCIDLib::TMsgId         midMsgText;
    };

    static const TReadResMap aMap[]
    {
        {
            tCIDNet::ENetPReadRes::Success
            , L"OK"
            , kCIDNet::c4HTTPStatus_OK
            , kNetErrs::errcNetP_Success
        }

      , {
            tCIDNet::ENetPReadRes::BadFirstLine
            , L"Bad Request"
            , kCIDNet::c4HTTPStatus_BadRequest
            , kNetErrs::errcNetP_BadFirstLine
        }

      , {
            tCIDNet::ENetPReadRes::BadHdrLine
            , L"Bad Request"
            , kCIDNet::c4HTTPStatus_BadRequest
            , kNetErrs::errcNetP_BadHdrLine
        }

      , {
            tCIDNet::ENetPReadRes::BadLineCont
            , L"Bad Request"
            , kCIDNet::c4HTTPStatus_BadRequest
            , kNetErrs::errcNetP_BadHdrContinue
        }

      , {
            tCIDNet::ENetPReadRes::BadProtoVer
            , L"Version not supported"
            , kCIDNet::c4HTTPStatus_VerNotSup
            , kNetErrs::errcNetP_BadProtoVer
        }

      , {
            tCIDNet::ENetPReadRes::BufTooSmall
            , L"Server error"
            , kCIDNet::c4HTTPStatus_SrvError
            , kNetErrs::errcNetP_BufTooSmall
        }

      , {
            tCIDNet::ENetPReadRes::ContLen
            , L"Bad request"
            , kCIDNet::c4HTTPStatus_BadRequest
            , kNetErrs::errcNetP_ContLen
        }

      , {
            tCIDNet::ENetPReadRes::InvalidMsg
            , L"Bad Request"
            , kCIDNet::c4HTTPStatus_BadRequest
            , kNetErrs::errcNetP_InvalidMsg
        }

      , {
            tCIDNet::ENetPReadRes::PartialMsg
            , L"Bad Request"
            , kCIDNet::c4HTTPStatus_BadRequest
            , kNetErrs::errcNetP_PartialMsg
        }

      , {
            tCIDNet::ENetPReadRes::Timeout
            , L"Request timed out"
            , kCIDNet::c4HTTPStatus_BadRequest
            , kNetErrs::errcNetP_Timeout
        }
    };
    const tCIDLib::TCard4 c4MapSz = tCIDLib::c4ArrayElems(aMap);

    if (tCIDLib::TCard4(eRes) >= c4MapSz)
    {
        strResText = L"Unknown error";
        strErrMsg = L"An unknown msg read result occurred";
        return kCIDNet::c4HTTPStatus_SrvError;
    }

    strResText = aMap[tCIDLib::c4EnumOrd(eRes)].pszResText;
    strErrMsg = facCIDNet().strMsg(aMap[tCIDLib::c4EnumOrd(eRes)].midMsgText);
    return aMap[tCIDLib::c4EnumOrd(eRes)].c4Status;
}



// ---------------------------------------------------------------------------
//   CLASS: TAsyncHTTPQ
//  PREFIX: ahttpq
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TAsyncHTTPQ: Constructors and Destructor
// ---------------------------------------------------------------------------
TAsyncHTTPQ::TAsyncHTTPQ() :

    m_eState(tCIDLib::EAsyncWaitRes::Complete)
    , m_thrQ
      (
        facCIDLib().strNextThreadName(L"AsyncHTTP")
        , TMemberFunc<TAsyncHTTPQ>(this, &TAsyncHTTPQ::eQueryThread)
      )
    , m_c4InContLen(0)
    , m_c4OutContLen(0)
    , m_c4ResCode(0)
    , m_c4WaitFor(0)
    , m_eCodeType(tCIDNet::EHTTPCodes::Unknown)
    , m_OpType(EOpTypes::None)
{
}

TAsyncHTTPQ::~TAsyncHTTPQ()
{
    //
    //  Make sure we stop any outstanding operation. Hopefully they will have
    //  already done some, but just in case.
    //
    CancelOp();
}


// ---------------------------------------------------------------------------
//  TAsyncHTTPQ: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  If not waiting for an operation, then this can be called to get the results
//  of the operation last completed.
//
tCIDLib::TCard4
TAsyncHTTPQ::c4GetOutput(tCIDNet::EHTTPCodes&   eCodeType
                        , TString&              strRepText
                        , tCIDLib::TKVPCollect& colOutHdrLines
                        , TString&              strContType
                        , TMemBuf&              mbufCont
                        , tCIDLib::TCard4&      c4ContLen
                        , TString&              strUltimateURL)
{
    // We cannot be in waiting state
    CheckNotBusy();

    // Looks ok so give back the info
    eCodeType = m_eCodeType;
    strRepText = m_strRepText;
    strContType = m_strContType;
    strUltimateURL = m_strUltimateURL;

    // If any outgoing content, then copy it back
    c4ContLen = m_c4OutContLen;
    if (c4ContLen)
        mbufCont.CopyIn(m_mbufOutCont, c4ContLen);

    CopyOutHdrLines(colOutHdrLines);

    return m_c4ResCode;
}


tCIDLib::TVoid TAsyncHTTPQ::CancelOp()
{
    if (m_thrQ.bIsRunning())
    {
        try
        {
            m_thrQ.ReqShutdownSync(6000);
            m_thrQ.eWaitForDeath(2000);
        }

        catch(TError& errToCatch)
        {
            if (facCIDNet().bTestLog(errToCatch, tCIDNet::ELogFlags::HTTPClient))
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }
        }
    }

    // Reset us back to complete state
    m_eState = tCIDLib::EAsyncWaitRes::Complete;
}


// Let clients know our current state
tCIDLib::EAsyncWaitRes TAsyncHTTPQ::eState() const
{
    return m_eState;
}


// If the state is Error, then this is the error we got
const TError& TAsyncHTTPQ::errLast() const
{
    return m_errLast;
}


const TString& TAsyncHTTPQ::strTitle() const
{
    return m_strTitle;
}


//
//  If not currenting doing an operation, this will start up a GET.
//
tCIDLib::TVoid
TAsyncHTTPQ::StartGETRedir( const   TString&                strTitle
                            , const TURL&                   urlToGet
                            , const tCIDLib::TCard4         c4WaitFor
                            , const TString&                strAgent
                            , const TString&                strAccept
                            , const tCIDLib::TKVPCollect&   colInHdrLines)
{
    // We cannot be in waiting state
    CheckNotBusy();

    // OK, let's set up the info
    m_c4InContLen = 0;
    m_strTitle = strTitle;
    m_OpType = EOpTypes::GETRedir;
    m_urlToGet = urlToGet;
    m_c4WaitFor = c4WaitFor;
    m_strAgent = strAgent;
    m_strAccept = strAccept;
    CopyInHdrLines(colInHdrLines, m_colInHdrLines);

    // Reset the output stuff for safety
    ResetOutput();

    // And start up the thread
    m_thrQ.Start();
}



//
//  If not currenting doing an operation, this will start up a POST
//
tCIDLib::TVoid
TAsyncHTTPQ::StartPOST( const   TString&                strTitle
                        , const TURL&                   urlToGet
                        , const tCIDLib::TCard4         c4WaitFor
                        , const TString&                strAgent
                        , const TString&                strAccept
                        , const tCIDLib::TKVPCollect&   colBody
                        , const tCIDLib::TKVPCollect&   colInHdrLines)
{
    // We cannot be in waiting state
    CheckNotBusy();

    // OK, let's set up the info
    m_strTitle = strTitle;
    m_OpType = EOpTypes::POST;
    m_urlToGet = urlToGet;
    m_c4WaitFor = c4WaitFor;
    m_strAgent = strAgent;
    m_strAccept = strAccept;

    CopyInHdrLines(colInHdrLines, m_colInHdrLines);
    CopyInHdrLines(colBody, m_colPostBody);

    // Reset the output stuff for safety
    ResetOutput();

    // And start up the thread
    m_thrQ.Start();
}


// ---------------------------------------------------------------------------
//  TAsyncHTTPQ: Private, non-virtual methods
// ---------------------------------------------------------------------------

// Make sure we are in one of the end states, not waiting for an exchange
tCIDLib::TVoid TAsyncHTTPQ::CheckNotBusy()
{
    if (m_eState == tCIDLib::EAsyncWaitRes::Waiting)
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcAHTTP_Busy
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Busy
            , m_strTitle
        );
    }
}


// Avoid duplicating this code for each operation startup method
tCIDLib::TVoid
TAsyncHTTPQ::CopyInHdrLines(const   tCIDLib::TKVPCollect&   colSrc
                            ,       tCIDLib::TKVPList&      colTar)
{
    TColCursor<TKeyValuePair>& cursHdrLns = *colSrc.pcursNew();
    TJanitor<TColCursor<TKeyValuePair>> janCursor(&cursHdrLns);
    m_colInHdrLines.RemoveAll();
    for (; cursHdrLns; ++cursHdrLns)
        colTar.objAdd(*cursHdrLns);
}

tCIDLib::TVoid
TAsyncHTTPQ::CopyOutHdrLines(tCIDLib::TKVPCollect& colTar)
{
    colTar.RemoveAll();
    const tCIDLib::TCard4 c4Count = m_colOutHdrLines.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        colTar.objAdd(m_colOutHdrLines[c4Index]);
}


//
//  This is the processing thread. It is started up for each new operationg and
//  runs till it completes or times out or gets an error.
//
tCIDLib::EExitCodes
TAsyncHTTPQ::eQueryThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // Deal with any member updates while the caller is blocked
    m_eState = tCIDLib::EAsyncWaitRes::Waiting;

    //
    //  And now let the caller go. He will be waiting for us to udpate the
    //  status.
    //
    thrThis.Sync();

    try
    {
        if (m_OpType == EOpTypes::GETRedir)
        {
            //
            //  Issue the operation. We have to create a persistent data source to
            //  deal with redirection. There's a helper to do this.
            //
            TCIDDataSrcJan janSrc
            (
                facCIDNet().pcdsMakeSocketSrc(m_strAgent, m_urlToGet)
                , tCIDLib::EAdoptOpts::Adopt
                , kCIDLib::True
            );

            m_c4ResCode = m_httpcThis.c4SendGetRedir
            (
                janSrc
                , m_urlToGet
                , TTime::enctNowPlusMSs(m_c4WaitFor)
                , m_strAgent
                , m_strAccept
                , m_eCodeType
                , m_strRepText
                , m_colOutHdrLines
                , m_strContType
                , m_mbufOutCont
                , m_c4OutContLen
                , m_strUltimateURL
                , m_colInHdrLines
            );
        }
         else if (m_OpType == EOpTypes::POST)
        {
            m_c4ResCode = m_httpcThis.c4SendPost
            (
                nullptr
                , m_urlToGet
                , TTime::enctNowPlusMSs(m_c4WaitFor)
                , m_strAgent
                , m_strAccept
                , m_colPostBody
                , m_eCodeType
                , m_strRepText
                , m_colOutHdrLines
                , m_strContType
                , m_mbufOutCont
                , m_c4OutContLen
                , m_colInHdrLines
            );
        }
    }

    catch(TError& errToCatch)
    {
        m_errLast = errToCatch;
        m_eState = tCIDLib::EAsyncWaitRes::Error;
        return tCIDLib::EExitCodes::FatalError;
    }

    catch(...)
    {
        m_errLast = TError
        (
            facCIDNet().strName()
            , CID_FILE
            , CID_LINE
            , kNetErrs::errcAHTTP_UnknownExcept
            , facCIDNet().strMsg(kNetErrs::errcAHTTP_UnknownExcept)
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
        );

        m_eState = tCIDLib::EAsyncWaitRes::Error;
        return tCIDLib::EExitCodes::FatalError;
    }

    m_eState = tCIDLib::EAsyncWaitRes::Complete;
    return tCIDLib::EExitCodes::Normal;
}


//
//  Resets all of the output members to safe defaults.
//
tCIDLib::TVoid TAsyncHTTPQ::ResetOutput()
{
    m_c4ResCode = 0;
    m_c4OutContLen = 0;
    m_colOutHdrLines.RemoveAll();
    m_eCodeType = tCIDNet::EHTTPCodes::Unknown;
    m_strContType.Clear();
    m_strRepText.Clear();
    m_strUltimateURL.Clear();
}


