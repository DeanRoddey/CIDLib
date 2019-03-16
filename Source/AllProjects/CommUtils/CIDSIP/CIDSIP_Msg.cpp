//
// FILE NAME: CIDSIP_Msg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/28/2013
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
//  This file implements the TSIPMsg class.
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
#include    "CIDSIP_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TSIPMsg,TObject)


// ---------------------------------------------------------------------------
//   CLASS: TSIPMsg
//  PREFIX: sipmsg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSIPMsg: Constructors and Destructor
// ---------------------------------------------------------------------------
TSIPMsg::TSIPMsg() :

    m_c4ContLen(0)
    , m_colHdrLines(29, new TStringKeyOps, &TKeyValuePair::strExtractKey)
{
}

TSIPMsg::TSIPMsg(const TSIPMsg& sipmsgSrc) :

    m_c4ContLen(sipmsgSrc.m_c4ContLen)
    , m_colHdrLines(sipmsgSrc.m_colHdrLines)
    , m_mbufContent(sipmsgSrc.m_mbufContent)
    , m_strFirstLine(sipmsgSrc.m_strFirstLine)
{
}

TSIPMsg::~TSIPMsg()
{
}


// ---------------------------------------------------------------------------
//  TSIPMsg: Public operators
// ---------------------------------------------------------------------------
const TSIPMsg& TSIPMsg::operator=(const TSIPMsg& sipmsgSrc)
{
    if (this != &sipmsgSrc)
    {
        m_c4ContLen     = sipmsgSrc.m_c4ContLen;
        m_colHdrLines   = sipmsgSrc.m_colHdrLines;
        m_mbufContent   = sipmsgSrc.m_mbufContent;
        m_strFirstLine  = sipmsgSrc.m_strFirstLine;
    }
    return *this;
}



// ---------------------------------------------------------------------------
//  TSIPMsg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TSIPMsg::AddHdrLine(const   TString&    strKey
                    , const TString&    strValue)
{
    // Add or update this key
    TKeyValuePair* pkvalTar = m_colHdrLines.pobjFindByKey(strKey);
    if (pkvalTar)
        pkvalTar->strValue(strValue);
    else
        m_colHdrLines.objAdd(TKeyValuePair(strKey, strValue));

    // If it's the content length line, store the length
    if (strKey.bCompareI(THTTPClient::strHdr_ContLen))
    {
        if (!strKey.bToCard4(m_c4ContLen, tCIDLib::ERadices::Dec))
        {
            facCIDSIP().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSIPErrs::errcSIP_BadContLenLine
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
            );
        }
    }
}


// See if a header line exists, if so give back the value
tCIDLib::TBoolean
TSIPMsg::bFindHdrLine(const TString& strKey, TString& strToFill) const
{
    const TKeyValuePair* pkvalTar = m_colHdrLines.pobjFindByKey(strKey);
    if (pkvalTar)
    {
        strToFill = pkvalTar->strValue();
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// Return the currently set content length
tCIDLib::TCard4 TSIPMsg::c4ContentLen() const
{
    return m_c4ContLen;
}


tCIDLib::TCard4 TSIPMsg::c4MaxForwards() const
{
    return m_c4MaxForwards;
}

tCIDLib::TCard4 TSIPMsg::c4MaxForwards(const tCIDLib::TCard4 c4ToSet)
{
    m_c4MaxForwards = c4ToSet;
    return m_c4MaxForwards;
}


// Provide access to the content buffer
const TMemBuf& TSIPMsg::mbufContent() const
{
    return m_mbufContent;
}


// Return a header line value, throw if not found
const TString& TSIPMsg::strFindHdrLine(const TString& strKey) const
{
    const TKeyValuePair* pkvalTar = m_colHdrLines.pobjFindByKey(strKey);
    if (!pkvalTar)
    {
        facCIDSIP().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSIPErrs::errcSIP_NoSuchHdrLn
            , strKey
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
        );
    }
    return pkvalTar->strValue();
}


//
//  Set the content. We store both the passed content byte count, and
//  we will add or update a content type header line as well.
//
tCIDLib::TVoid
TSIPMsg::SetContent(const   tCIDLib::TCard4 c4Bytes
                    , const TMemBuf&        mbufCont)
{
    m_c4ContLen = c4Bytes;
    if (c4Bytes)
        m_mbufContent.CopyIn(mbufCont, c4Bytes);

    //
    //  Deal with the header line. If it's already there, we update it, else
    //  we add one.
    //
    TString strValue;
    strValue.AppendFormatted(c4Bytes);
    AddHdrLine(THTTPClient::strHdr_ContLen, strValue);
}


// Get or set the first line
const TString& TSIPMsg::strFirstLine() const
{
    return m_strFirstLine;
}

const TString& TSIPMsg::strFirstLine(const TString& strToSet)
{
    return m_strFirstLine;
}

