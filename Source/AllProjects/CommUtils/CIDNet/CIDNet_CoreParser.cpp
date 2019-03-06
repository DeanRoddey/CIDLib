//
// FILE NAME: CIDNet_CoreParser.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/05/2013
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TTNetCoreParser class.
//
//  We have to do some somewhat unnatural parsing here. Normally we'd just
//  put a text input stream over the socket. But, we can't do that since the
//  header part is the only part we can really treat as text, and the body we
//  want to keep binary since it could contain anything. The stream socket
//  impl object  would try to read ahead and fail because it often won't be
//  able to transcode the contents correctly once it hits the body text.
//
//  So we do it the old fashioned way by just streaming in bytes from
//  the socket till we've read all the header lines. We provide our own
//  simple spooling buffer.
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
RTTIDecls(TNetCoreParser,TObject)




// ---------------------------------------------------------------------------
//   CLASS: TNetCoreParser
//  PREFIX: ncp
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TNetCoreParser: Public, static methods
// ---------------------------------------------------------------------------

//
//  Searchs a list of header line values to find a particular one, returning
//  the value if found. Return value indicates whether it was found or not.
//
tCIDLib::TBoolean
TNetCoreParser::bFindHdrLine(const  tCIDLib::TKVPCollect&   colHdrLines
                            , const TString&                strToFind
                            ,       TString&                strValue)
{
    TColCursor<TKeyValuePair>* pcursFind = colHdrLines.pcursNew();
    TJanitor<TColCursor<TKeyValuePair> > janCursor(pcursFind);

    if (!pcursFind->bReset())
        return kCIDLib::False;

    do
    {
        const TKeyValuePair& kvalCur = pcursFind->objRCur();
        if (kvalCur.strKey().bCompareI(strToFind))
        {
            strValue = kvalCur.strValue();
            break;
        }
    }   while (pcursFind->bNext());

    // If the cursor is still valid, we found it
    return pcursFind->bIsValid();
}


// Similar to Find above but just checks for existence
tCIDLib::TBoolean
TNetCoreParser::bHdrLineExists( const   tCIDLib::TKVPCollect&   colHdrLines
                                , const TString&                strToFind)
{
    TColCursor<TKeyValuePair>* pcursFind = colHdrLines.pcursNew();
    TJanitor<TColCursor<TKeyValuePair> > janCursor(pcursFind);

    if (!pcursFind->bReset())
        return kCIDLib::False;

    do
    {
        const TKeyValuePair& kvalCur = pcursFind->objRCur();
        if (kvalCur.strKey().bCompareI(strToFind))
            break;
    }   while (pcursFind->bNext());

    // If the cursor is still valid, we found it
    return pcursFind->bIsValid();
}

//
//  Takes the value of a Content-Type HTML header value and decides if it is
//  text of not (the return value indicates this), and if it is text, what
//  the text encoding is.
//
//  We can provide a default if the caller wants, and its not explicit in the
//  encoding header line. Else we will return false and an empty string.
//
tCIDLib::TBoolean
TNetCoreParser::bParseTextEncoding( const   TString&            strContType
                                    ,       TString&            strEncoding
                                    , const tCIDLib::TBoolean   bInsertDefault)
{
    strEncoding.Clear();

    //
    //  Find the semi-colon, and we'll copy out all the stuff from there on.
    //  If none, then no charset, so set it to default if asked, else empty.
    //
    tCIDLib::TCard4 c4Ofs;
    if (!strContType.bFirstOccurrence(kCIDLib::chSemiColon, c4Ofs))
    {
        if (bInsertDefault)
            strEncoding = kCIDNet::pszDefHTTPEncoding;
        return bInsertDefault;
    }

    // If the semi-colon is the last char, same thing
    if (c4Ofs == strContType.c4Length())
    {
        if (bInsertDefault)
            strEncoding = kCIDNet::pszDefHTTPEncoding;
        return bInsertDefault;
    }

    // Get the stuff past the semicolong and strip whitspace
    strContType.CopyOutSubStr(strEncoding, c4Ofs + 1);
    strEncoding.StripWhitespace();

    // If empty, then same thing
    if (strEncoding.bIsEmpty())
    {
        if (bInsertDefault)
            strEncoding = kCIDNet::pszDefHTTPEncoding;
        return bInsertDefault;
    }

    // If it doesn't start with charset=, then return false
    if (!strEncoding.bStartsWithI(L"charset="))
        return kCIDLib::False;

    // Cut out the charset= and the remainder should be the encoding
    strEncoding.Cut(0, 8);

    // If it has quotes, we have to remove those
    strEncoding.Strip(L"'\"");

    //
    //  Strip whitespace again. If empty, set it back to default if we were asked
    //  to provide a default.
    //
    strEncoding.StripWhitespace();
    if (strEncoding.bIsEmpty())
    {
        if (bInsertDefault)
            strEncoding = kCIDNet::pszDefHTTPEncoding;
        return bInsertDefault;
    }

    return kCIDLib::True;
}


//
//  The above bParseTextEncoding is kept around for backwards comptibility, but this
//  is a better way. It will first check for the encoding and use it if it was
//  provided. Else, we'll do some other probing to try to figure it out.
//
tCIDLib::TBoolean
TNetCoreParser::bFindTextEncoding(  const   tCIDLib::TKVPCollect&   colHdrLines
                                    , const TMemBuf&                mbufCont
                                    , const tCIDLib::TCard4         c4ContLen
                                    ,       TString&                strEncoding)
{
    // First, see if we have a content-type line
    TString strCType;
    if (bFindHdrLine(colHdrLines, THTTPClient::strHdr_ContType, strCType))
    {
        //
        //  Parse it but indicate no default to be provided. It will return true
        //  if an explicit encoding was provided.
        //
        if (bParseTextEncoding(strCType, strEncoding, kCIDLib::False))
            return kCIDLib::True;
    }

    //
    //  If the content length is at least 2 bytes, then check for a UTF-16 byte
    //  order mark, and use that if we find it.
    //
    if (c4ContLen >= 2)
    {
        if ((mbufCont[0] == 0xFF) && (mbufCont[1] == 0xFE))
        {
            strEncoding = L"UTF-16LE";
            return kCIDLib::True;
        }

        if ((mbufCont[1] == 0xFF) && (mbufCont[0] == 0xFE))
        {
            strEncoding = L"UTF-16BE";
            return kCIDLib::True;
        }
    }

    // If 3 bytes or more and a UTF-8 BOM, then go with that
    if (c4ContLen >= 3)
    {
        if ((mbufCont[0] == 0xEF)
        &&  (mbufCont[1] == 0xBB)
        &&  (mbufCont[2] == 0xBF))
        {
            strEncoding = L"UTF-8";
            return kCIDLib::True;
        }
    }

    return kCIDLib::False;
}


//
//  This is a standard first line parser. Most any syntax that derived classes will
//  use will have the same format as HTTP, i.e. three parameters. So we provide this
//  one for them to use, but they can do their own if needed.
//
//  The first one is just generic and looks for three distinct space separated parms.
//  THe other one is for parsing a reply from a server, when you know you are on the
//  client side. It'll do more work for you.
//
tCIDLib::TBoolean
TNetCoreParser::bParseFirstLn(  const   TString&    strFirstLn
                                ,       TString&    strParm1
                                ,       TString&    strParm2
                                ,       TString&    strParm3)
{
    TStringTokenizer stokReq(&strFirstLn, kCIDLib::szWhitespace);

    if (!stokReq.bGetNextToken(strParm1)
    ||  !stokReq.bGetNextToken(strParm2)
    ||  !stokReq.bGetRestOfLine(strParm3))
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TNetCoreParser::bParseSrvFirstLn(const  TString&                strFirstLn
                                ,       TString&                strRepType
                                ,       tCIDLib::TCard4&        c4Code
                                ,       tCIDNet::EHTTPCodes&    eCodeType
                                ,       TString&                strRepText
                                ,       tCIDLib::TErrCode&      errcFail)
{
    TStringTokenizer stokRep(&strFirstLn, kCIDLib::szWhitespace);

    // Get the reply type and error code
    TString strCode;
    if (!stokRep.bGetNextToken(strRepType)
    ||  !stokRep.bGetNextToken(strCode))
    {
        errcFail = kNetErrs::errcHTTP_BadStatus;
        return kCIDLib::False;
    }

    // Convert the status code. If not valid, we failed
    tCIDLib::TBoolean bOk;
    c4Code = TRawStr::c4AsBinary(strCode.pszBuffer(), bOk, tCIDLib::ERadices::Dec);
    if (!bOk)
    {
        errcFail = kNetErrs::errcHTTP_BadStatusCode;
        return kCIDLib::False;
    }

    // Calc the code type
    eCodeType = tCIDNet::EHTTPCodes::Unknown;
    if (c4Code < 600)
        eCodeType = tCIDNet::EHTTPCodes(c4Code / 100);

    // Put the rest in the reply text parameter
    stokRep.bGetRestOfLine(strRepText);

    return kCIDLib::True;
}




// ---------------------------------------------------------------------------
//  TNetCoreParser: Public Destructor
// ---------------------------------------------------------------------------
TNetCoreParser::~TNetCoreParser()
{
}


// ---------------------------------------------------------------------------
//  TNetCoreParser: Hidden Constructors
// ---------------------------------------------------------------------------
TNetCoreParser::TNetCoreParser(const TString& strHdrSep) :

    m_mbufTmp(8192, 0x800000, 64 * 1024)
    , m_strHdrSep(strHdrSep)
{
}


// ---------------------------------------------------------------------------
//  TNetCoreParser: Protected, virtual methods
// ---------------------------------------------------------------------------

//
//  The derived class can handle break apart header lines if needed, but for
//  the most part they'll use this version. We leave the key in the first
//  parm and move the value to the second.
//
tCIDLib::TBoolean
TNetCoreParser::bParseHdrLine(TString& strHdrLn, TString& strValue)
{
    // Return true if we split it on a colon
    if (!strHdrLn.bSplit(strValue, kCIDLib::chColon))
        return kCIDLib::False;

    // Strip lead/trail whitespace
    strHdrLn.StripWhitespace();
    strValue.StripWhitespace();

    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  TNetCoreParser: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDNet::ENetPReadRes
TNetCoreParser::eReadHdrLines(          TCIDDataSrc&            cdsSrc
                                , const tCIDLib::TEncodedTime   enctEnd
                                ,       tCIDLib::TKVPCollect&   colHdrLines
                                ,       TThread&                thrCaller)
{
    tCIDLib::TEncodedTime enctCur = TTime::enctNow();
    TKeyValuePair* pkvalLast = 0;
    TString strLine;
    TString strTmp;
    while ((enctCur < enctEnd) && !thrCaller.bCheckShutdownRequest())
    {
        const tCIDLib::ELineRes eRes = cdsSrc.eGetLine(strLine, enctEnd);

        // If an empty line, then that's the end of the headers
        if (eRes == tCIDLib::ELineRes::EmptyLine)
            break;

        if (eRes == tCIDLib::ELineRes::Timeout)
            return tCIDNet::ENetPReadRes::Timeout;
        else if (eRes == tCIDLib::ELineRes::Partial)
            return tCIDNet::ENetPReadRes::PartialMsg;

        //
        //  If it starts with white space, then it's a continuation of
        //  the previous header line, so append to its value. Else,
        //  enter a new one.
        //
        if (TRawStr::bIsSpace(strLine[0]))
        {
            // If we haven't seen one yet, then bad
            if (!pkvalLast)
                return tCIDNet::ENetPReadRes::BadLineCont;

            strLine.StripWhitespace();
            strTmp = pkvalLast->strValue();
            strTmp.Append(strLine);
            pkvalLast->strValue(strTmp);
        }
         else
        {
            //
            //  Let the derived class break it apart if they want, else our
            //  default handler does it, basd on the set divider character.
            //
            if (!bParseHdrLine(strLine, strTmp))
                return tCIDNet::ENetPReadRes::BadHdrLine;

            //
            //  Keep a pointer to the last added line, so that we can
            //  handle continuations.
            //
            pkvalLast = &colHdrLines.objAdd(TKeyValuePair(strLine, strTmp));
        }

        enctCur = TTime::enctNow();
    }
    return tCIDNet::ENetPReadRes::Success;
}


//
//  Get a message from the other side. This is used on both client and server sides.
//  It returns the first line, the header lines and any content received.
//
//  The caller can tell us that we shouldn't expect any content, even if there is a
//  content length indicate, such as when doing a HEAD in HTTP.
//
//  If it fails, we return a failure status, which the derived class can map to an
//  appropriate error msg and status.
//
tCIDNet::ENetPReadRes
TNetCoreParser::eGetMsg(        TCIDDataSrc&            cdsSrc
                        , const tCIDNet::EReadTypes     eReadType
                        , const tCIDLib::TEncodedTime   enctEnd
                        , const tCIDLib::TBoolean       bNoCont
                        ,       TString&                strFirstLine
                        ,       tCIDLib::TKVPCollect&   colHdrLines
                        ,       TMemBuf&                mbufCont
                        ,       tCIDLib::TCard4&        c4ContLen
                        ,       TString&                strContType
                        ,       TString&                strParm1
                        ,       TString&                strParm2
                        ,       TString&                strParm3)
{
    c4ContLen = 0;
    TString strLine;
    TString strTmp;

    // Clean out the header lines to get ready to start add new ones
    colHdrLines.RemoveAll();

    // Get the caller thread
    TThread& thrCaller = *TThread::pthrCaller();

    //
    //  Get the first line. We can't pass any judgement on this at this
    //  level. It is protocol specific, so the derived class will parse
    //  it out upon return.
    //
    tCIDLib::ELineRes eRes = cdsSrc.eGetLine(strFirstLine, enctEnd);
    if (eRes == tCIDLib::ELineRes::Timeout)
        return tCIDNet::ENetPReadRes::Timeout;
    else if (eRes == tCIDLib::ELineRes::Partial)
        return tCIDNet::ENetPReadRes::PartialMsg;
    else if (eRes != tCIDLib::ELineRes::GotLine)
        return tCIDNet::ENetPReadRes::BadFirstLine;

    //
    //  Read in the header lines until we run out of time or are asked to
    //  to shut down.
    //
    tCIDNet::ENetPReadRes eReadRes;
    eReadRes = eReadHdrLines(cdsSrc, enctEnd, colHdrLines, thrCaller);
    if (eReadRes != tCIDNet::ENetPReadRes::Success)
        return eReadRes;

    //
    //  Let the derived class do some basic validation and get us some info we
    //  need to enforce some things.
    //
    tCIDLib::TBoolean bClose;
    tCIDLib::TCard4 c4ToRead;
    tCIDLib::TBoolean bPersist;
    eReadRes = eBasicMsgVal
    (
        eReadType
        , colHdrLines
        , strFirstLine
        , bClose
        , bPersist
        , strContType
        , c4ToRead
        , strParm1
        , strParm2
        , strParm3
    );

    // If the basic validation failed, then we are done
    if (eReadRes != tCIDNet::ENetPReadRes::Success)
        return eReadRes;

    //
    //  The caller can tell us he knows that there will be no body, even if
    //  there is a content length. If there is a content length and it's
    //  zero then we obviously will not read any. If there was no content-length
    //  then c4ToRead will be maxcard and we'll try to read till the end (if that
    //  is valid, we'll check later below.)
    //
    const tCIDLib::TBoolean bReadBody = !bNoCont && (c4ToRead != 0);

    //
    //  Zero the content length and we'll run it up as we read in data, until
    //  we get the requested amount, or if to read is max card, we just read
    //  till the client drops the connection.
    //
    //  If we got zero content length or the caller told us no content should be
    //  seen, then we don't read anything.
    //
    tCIDLib::TEncodedTime enctCur = TTime::enctNow();
    c4ContLen = 0;
    if (bReadBody)
    {
        //
        //  If we got a specific data size to read, see if the buffer can
        //  handle it. If not, just throw now and don't waste time. If
        //  so, pre-allocate it up so we avoid resizing it potentially.
        //
        //  If we got no specific length, then just pay the price of letting
        //  it expand as we copy stuff in.
        //
        if (c4ToRead != kCIDLib::c4MaxCard)
        {
            if (mbufCont.c4MaxSize() < c4ToRead)
                return tCIDNet::ENetPReadRes::BufTooSmall;

            // Expand the buffer if needed
            if (mbufCont.c4Size() < c4ToRead)
                mbufCont.Reallocate(c4ToRead, kCIDLib::False);
        }

        //
        //  Now read body data until we get it or time out or are asked to exit. We
        //  have to deal here with chunked transfer. So we do two different loops.
        //  One is for a straightforward binary read of the data. The other handles
        //  chunked transfer, which is text oriented.
        //
        tCIDLib::TBoolean bChunked = kCIDLib::False;
        if (bFindHdrLine(colHdrLines, THTTPClient::strHdr_TransferEncoding, strTmp))
        {
            // The only encoding we currently handle is chunked so throw otherwise
            if (!strTmp.bCompareI(L"chunked"))
            {
                facCIDNet().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kNetErrs::errcNetP_UnsupTransEncoding
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::NotSupported
                    , strTmp
                );
            }
            bChunked = kCIDLib::True;
        }

        if (bChunked)
        {
            while ((enctCur < enctEnd) && !thrCaller.bCheckShutdownRequest())
            {
                // Pull in a line of ASCII text, CR/LF terminated
                strTmp.Clear();
                while (kCIDLib::True)
                {
                    const tCIDLib::TCard1 c1Cur = cdsSrc.c1ReadByte(enctEnd);
                    if (!c1Cur)
                    {
                        facCIDNet().ThrowErr
                        (
                            CID_FILE
                            , CID_LINE
                            , kNetErrs::errcNetP_BadChunkSize
                            , tCIDLib::ESeverities::Failed
                            , tCIDLib::EErrClasses::Format
                        );
                    }

                    // This is the end of line
                    if (c1Cur == 13)
                    {
                        if (cdsSrc.c1ReadByte(enctEnd) != 10)
                        {
                            facCIDNet().ThrowErr
                            (
                                CID_FILE
                                , CID_LINE
                                , kNetErrs::errcNetP_BadChunkSizeTerm
                                , tCIDLib::ESeverities::Failed
                                , tCIDLib::EErrClasses::Format
                            );
                        }
                        break;
                    }

                    // Otherwize take it as a content character
                    strTmp.Append(tCIDLib::TCh(c1Cur));
                }

                // Drop any extensions and keep just the size
                strTmp.bCapAtChar(kCIDLib::chSemiColon);

                // Convert it to a length
                tCIDLib::TCard4 c4ChunkLen;
                if (!strTmp.bToCard4(c4ChunkLen, tCIDLib::ERadices::Hex))
                {
                    facCIDNet().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kNetErrs::errcNetP_BadChunkSize
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Format
                    );
                }

                // If it's zero, then we are done with the body text
                if (!c4ChunkLen)
                    break;

                // Now let's read this many bytes into our content buffer
                tCIDLib::TCard4 c4SoFar = 0;
                enctCur = TTime::enctNow();
                while ((enctCur < enctEnd)
                &&     (c4SoFar < c4ChunkLen)
                &&     !thrCaller.bCheckShutdownRequest())
                {
                    //
                    //  Read some more data from the source. Up to 2K at a time,
                    //  or as much as we need to get if we are close. Note that,
                    //  if no known amount, then to read is max card, so this
                    //  calculation will always return 1K.
                    //
                    const tCIDLib::TCard4 c4NewBytes = cdsSrc.c4ReadBytes
                    (
                        m_mbufTmp
                        , tCIDLib::MinVal(2048UL, c4ChunkLen - c4SoFar), enctEnd
                    );

                    if (c4NewBytes)
                    {
                        mbufCont.CopyIn(m_mbufTmp, c4NewBytes, c4ContLen);
                        c4ContLen += c4NewBytes;
                        c4SoFar += c4NewBytes;
                    }
                     else
                    {
                        // We ran out of data
                        facCIDNet().ThrowErr
                        (
                            CID_FILE
                            , CID_LINE
                            , kNetErrs::errcNetP_ChunkEOS
                            , tCIDLib::ESeverities::Failed
                            , tCIDLib::EErrClasses::Format
                        );
                    }

                    // Update the time for the next round
                    enctCur = TTime::enctNow();
                }

                // The next two bytes should be CR/LF
                if ((cdsSrc.c1ReadByte(enctEnd) != 13)
                ||  (cdsSrc.c1ReadByte(enctEnd) != 10))
                {
                    facCIDNet().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kNetErrs::errcNetP_NoChunkTerm
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Format
                    );
                }
            }

            //
            //  We could have more header lines at the bottom. There will be a blank
            //  line either way, so it works just like regular header lines.
            //
            eReadRes = eReadHdrLines(cdsSrc, enctEnd, colHdrLines, thrCaller);
            if (eReadRes != tCIDNet::ENetPReadRes::Success)
                return eReadRes;
        }
         else if ((c4ToRead == kCIDLib::c4MaxCard) && bPersist && !bClose)
        {
            //
            //  In theory we are supposed to be able to reject this. It's a
            //  persistent connection, that has no content-length header, and
            //  it's not being closed. But, unfortunately, this will reject a
            //  lot of stuff that just isn't being done the way it should.
            //
            //  So, we just fall out with zero content length.
            //
            c4ContLen = 0;
        }
         else
        {
            //
            //  Looks reasonable. So ready until we either get the requested bytes,
            //  get an end of of input, or time out.
            //
            while ((enctCur < enctEnd)
            &&     (c4ContLen < c4ToRead)
            &&     !thrCaller.bCheckShutdownRequest())
            {
                //
                //  Read some more data from the source. Up to 2K at a time,
                //  or as much as we need to get if we are close. Note that,
                //  if no known amount, then to read is max card, so this
                //  calculation will always return 1K.
                //
                const tCIDLib::TCard4 c4NewBytes = cdsSrc.c4ReadBytes
                (
                    m_mbufTmp, tCIDLib::MinVal(2048UL, c4ToRead - c4ContLen), enctEnd
                );

                if (c4NewBytes)
                {
                    mbufCont.CopyIn(m_mbufTmp, c4NewBytes, c4ContLen);
                    c4ContLen += c4NewBytes;
                }
                 else
                {
                    //
                    //  We've hit the end one way or another, since we wait
                    //  for the remaining time each time around. If we don't
                    //  get anything, then we are done.
                    //
                    break;
                }

                // Update the time for the next round
                enctCur = TTime::enctNow();
            }

            //
            //  If we got a content length to expect, but didn't get that many
            //  bytes, then we didn't get the whole message.
            //
            if ((c4ToRead != kCIDLib::c4MaxCard) && (c4ContLen < c4ToRead))
                return tCIDNet::ENetPReadRes::PartialMsg;
        }
    }
     else
    {
        //
        //  If we didn't read any body despite having gotten a number of bytes
        //  to read, then it's because the caller told us not to read it, but
        //  we still want to return the content length if we got one. The
        //  caller should know that there won't be any actual data since he
        //  told us not to read it.
        //
        if (c4ToRead != kCIDLib::c4MaxCard)
            c4ContLen = c4ToRead;
    }

    //
    //  If we didn't get a content-length header, but we figured out the length,
    //  and the caller didn't request to ignore content, then let's force one in
    //  because we know now the length, and it makes everything downstream a lot
    //  simpler.
    //
    if (c4ContLen
    &&  !bFindHdrLine(colHdrLines, THTTPClient::strHdr_ContLen, strTmp)
    &&  !bNoCont)
    {
        strTmp.SetFormatted(c4ContLen);
        colHdrLines.objAdd(TKeyValuePair(THTTPClient::strHdr_ContLen, strTmp));
    }

    //
    //  See if we timed out. If we didn't either read all the requested
    //  bytes, or the server didn't complete and close an open ended
    //  msg body transmission within the expected time, we timed out.
    //
    if (enctCur >= enctEnd)
    {
        //
        //  If we got some data, then say it was a partial msg body, else
        //  just a general timeout.
        //
        if (c4ContLen)
            return tCIDNet::ENetPReadRes::PartialMsg;
        return tCIDNet::ENetPReadRes::Timeout;
    }
    return tCIDNet::ENetPReadRes::Success;
}



//
//  Send a message. We send header info and then if there's any content we send
//  that. The caller indicates if this is a one-shot, i.e. the caller created the
//  data source on the fly on behalf of his caller. If so, we insure that there
//  is a close connection header included.
//
tCIDLib::TVoid
TNetCoreParser::SendMsg(        TCIDDataSrc&            cdsTar
                        , const tCIDLib::TEncodedTime   enctEnd
                        , const TString&                strFirstLine
                        , const tCIDLib::TKVPCollect&   colHdrLines
                        , const tCIDLib::TKVPCollect&   colExtraHdrs
                        , const TMemBuf&                mbufCont
                        , const tCIDLib::TCard4&        c4ContLen)
{
    // Build up the data to send into our send buffer
    const tCIDLib::TCard4 c4MsgLen = c4BuildMsg
    (
        strFirstLine
        , colHdrLines
        , colExtraHdrs
        , mbufCont
        , c4ContLen
    );

#if CID_DEBUG_ON
// #define DUMP_OUT_MSGS
#if defined(DUMP_OUT_MSGS)
{
    TUTF8Converter tcvtTmp;
    TString strMsg;
    tcvtTmp.c4ConvertFrom(m_mbufTmp.pc1Data(), c4MsgLen, strMsg);
    TOutConsole conOut;
    conOut.eNewLineType(tCIDLib::ENewLineTypes::LF);
    conOut  << L"\n-----OUT MSG-----\n"
            << strMsg
            << L"\n-----OUT MSG-----\n\n";
}
#endif
#endif

    // Write to the source then flush it to force it all out
    cdsTar.WriteBytes(m_mbufTmp, c4MsgLen);
    cdsTar.FlushOut(enctEnd);
}


// ---------------------------------------------------------------------------
//  TNetCoreParser: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  A common helper that will build up the header and the caller's content
//  into our send buffer member and return the count of bytes. This is used
//  by the msg sending methods above.
//
tCIDLib::TCard4
TNetCoreParser::c4BuildMsg( const   TString&                strFirstLine
                            , const tCIDLib::TKVPCollect&   colHdrLines
                            , const tCIDLib::TKVPCollect&   colExtraHdrs
                            , const TMemBuf&                mbufCont
                            , const tCIDLib::TCard4&        c4ContLen)
{
    //
    //  Set up a memory buffer stream with an ASCII transcoder and format
    //  out all the header stuff first.
    //
    tCIDLib::TCard4 c4RetLen = 0;
    {
        TTextMBufOutStream strmOut
        (
            &m_mbufTmp, tCIDLib::EAdoptOpts::NoAdopt, new TUSASCIIConverter
        );
        strmOut.eNewLineType(tCIDLib::ENewLineTypes::CRLF);

        // Put out the first line
        strmOut << strFirstLine << kCIDLib::NewLn;

        // If we have incoming header lines, then send them
        FmtHdrLines(strmOut, colHdrLines);

        // If we have any extra ones, do those as well
        if (!colExtraHdrs.bIsEmpty())
            FmtHdrLines(strmOut, colExtraHdrs);

        // Cap off with an empty line and flush it out
        strmOut << kCIDLib::NewLn << kCIDLib::FlushIt;

        // And remember how long this part is
        c4RetLen = strmOut.c4CurSize();
    }

    //
    //  Now copy in the caller's body content if any and add that to the
    //  return count.
    //
    if (c4ContLen)
    {
        m_mbufTmp.CopyIn(mbufCont, c4ContLen, c4RetLen);
        c4RetLen += c4ContLen;
    }

    return c4RetLen;
}


//
//  Given an output stream that header info is being formatted into, this
//  will stream out all the passed header lines to it.
//
tCIDLib::TVoid
TNetCoreParser::FmtHdrLines(        TTextOutStream&         strmOut
                            , const tCIDLib::TKVPCollect&   colHdrs)
{
    TColCursor<TKeyValuePair>* pcursLines = colHdrs.pcursNew();
    TJanitor<TColCursor<TKeyValuePair> > janCursor(pcursLines);
    if (pcursLines->bReset())
    {
        do
        {
            const TKeyValuePair& kvalCur = pcursLines->objRCur();
            strmOut << kvalCur.strKey() << m_strHdrSep
                    << kvalCur.strValue() << kCIDLib::NewLn;
        }   while (pcursLines->bNext());
    }

}


