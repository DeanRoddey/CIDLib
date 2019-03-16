//
// FILE NAME: CIDNet_XMLURLEntitySrc.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/20/1999
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
//  This file implements the TURLEntitySrc class.
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
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TURLEntitySrc,TXMLEntitySrc)



// ---------------------------------------------------------------------------
//  CLASS: TURLEntitySrc
// PREFIX: xsrc
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TURLEntitySrc: Constructors and Destructor
// ---------------------------------------------------------------------------

//
//  All of these guys set the system id on our parent from the fully
//  qualified URL that we built via our URL object. We tell it not to
//  expand, and not to include username/password info. The system id isn't
//  for the actual data access (m_purlSysId is), it's just to use as a system
//  id.
//
TURLEntitySrc::TURLEntitySrc(const TURL& urlSysId) :

    m_purlSysId(new TURL(urlSysId))
{
    TString strFullURL;
    m_purlSysId->QueryText(strFullURL, kCIDLib::False, kCIDLib::False);
    strSystemId(strFullURL);
}

TURLEntitySrc::TURLEntitySrc(const  TURL&       urlSysId
                            , const TString&    strPubId) :

    TXMLEntitySrc()
    , m_purlSysId(new TURL(urlSysId))
{
    strPublicId(strPubId);

    TString strFullURL;
    m_purlSysId->QueryText(strFullURL, kCIDLib::False, kCIDLib::False);
    strSystemId(strFullURL);
}

TURLEntitySrc::TURLEntitySrc(const TString& strSysId) :

    TXMLEntitySrc()
    , m_purlSysId(new TURL(strSysId, tCIDSock::EQualified::Full))
{
    TString strFullURL;
    m_purlSysId->QueryText(strFullURL, kCIDLib::False, kCIDLib::False);
    strSystemId(strFullURL);
}

TURLEntitySrc::TURLEntitySrc(const  TString&    strSysId
                            , const TString&    strPubId) :

    TXMLEntitySrc()
    , m_purlSysId(new TURL(strSysId, tCIDSock::EQualified::Full))
{
    strPublicId(strPubId);

    TString strFullURL;
    m_purlSysId->QueryText(strFullURL, kCIDLib::False, kCIDLib::False);
    strSystemId(strFullURL);
}

TURLEntitySrc::TURLEntitySrc(const  TString&    strParentId
                            , const TString&    strSysId
                            , const TString&    strPubId) :
    TXMLEntitySrc()
    , m_purlSysId(new TURL(strParentId, strSysId))
{
    strPublicId(strPubId);

    TString strFullURL;
    m_purlSysId->QueryText(strFullURL, kCIDLib::False, kCIDLib::False);
    strSystemId(strFullURL);
}


TURLEntitySrc::TURLEntitySrc(const TURLEntitySrc& xsrcToCopy) :

    TXMLEntitySrc(xsrcToCopy)
    , m_purlSysId(new TURL(*xsrcToCopy.m_purlSysId))
{
}

TURLEntitySrc::~TURLEntitySrc()
{
    delete m_purlSysId;
}


// ---------------------------------------------------------------------------
//  TURLEntitySrc: Public operators
// ---------------------------------------------------------------------------
TURLEntitySrc& TURLEntitySrc::operator=(const TURLEntitySrc& xsrcToAssign)
{
    if (this != &xsrcToAssign)
    {
        TParent::operator=(xsrcToAssign);
        *m_purlSysId = *xsrcToAssign.m_purlSysId;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TURLEntitySrc: Public, inherited methods
// ---------------------------------------------------------------------------
TBinInStream* TURLEntitySrc::pstrmMakeNew() const
{
    // If logging info msgs, then log the URL
    if (facCIDNet().bTestLog(tCIDLib::ESeverities::Info, tCIDNet::ELogFlags::XMLURLEntitySrc))
    {
        facCIDXML().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcURL_MakeURLEntity
            , TString(*m_purlSysId)
            , tCIDLib::ESeverities::Info
            , tCIDLib::EErrClasses::AppStatus
        );

    }

    //
    //  We only support local file and HTTP at this time, so check the URL
    //  to make sure its one of those.
    //
    TBinInStream* pstrmRet = nullptr;
    if (m_purlSysId->eProto() == tCIDSock::EProtos::File)
    {
        //
        //  See if the host is 'localhost' or empty. If not, then its not
        //  supported at this time.
        //
        if ((m_purlSysId->strHost() != L"localhost")
        &&  !m_purlSysId->strHost().bIsEmpty())
        {
            facCIDXML().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kXMLErrs::errcURL_Unsupported
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotSupported
            );
        }

        //
        //  If its a fully qualified Win32 style path, then we have to throw
        //  away the leading slash by moving the pointer up one.
        //
        const tCIDLib::TCh* pszTmp = m_purlSysId->strPath().pszBuffer();
        if (TRawStr::bIsAlpha(*(pszTmp + 1)))
        {
            if (*(pszTmp + 2) == kCIDLib::chColon)
            {
                if ((*(pszTmp + 3) == kCIDLib::chForwardSlash)
                ||  (*(pszTmp + 3) == kCIDLib::chBackSlash))
                {
                    pszTmp++;
                }
            }
        }

        pstrmRet = new TBinFileInStream
        (
            pszTmp
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::None
            , tCIDLib::EFileFlags::SequentialScan
        );
    }
     else if (m_purlSysId->eProto() == tCIDSock::EProtos::HTTP)
    {
        //
        //  We use a THTTPClient object to do our query and to get the
        //  response data.
        //
        tCIDLib::TCard4         c4ContLen = 0;
        tCIDLib::TKVPList       colInHdrLines;
        tCIDLib::TKVPList       colOutHdrLines;
        tCIDNet::EHTTPCodes     eCodeType;
        TString                 strContType;
        TString                 strRepLine;

        //
        //  Allocate a buffer for the body content since we are going to
        //  return a stream and we can just give it to him to adopt, and
        //  avoid having to copy it. We allocate it small but give it a
        //  large max, and just let the HTTP client size it up to fit
        //  the content. This is the most efficient way.
        //
        THeapBuf* pmbufContent = new THeapBuf(8, 0x800000, 16 * 1024);
        TJanitor<THeapBuf> janBuf(pmbufContent);

        //
        //  Create an HTTP client and do the request. Let it create a one shot socket,
        //  by passing in an initialized data source janitor.
        //
        TString strUltimateURL;
        THTTPClient httpcGet;
        TCIDDataSrcJan janEmpty;
        httpcGet.c4SendGetRedir
        (
            janEmpty
            , *m_purlSysId
            , TTime::enctNowPlusMSs(8000)
            , L"CIDLib XML URL Entity Parser/1.0"
            , L"text/xml"
            , eCodeType
            , strRepLine
            , colInHdrLines
            , strContType
            , *pmbufContent
            , c4ContLen
            , strUltimateURL
            , colOutHdrLines
        );

        // If the result isn't good, then throw
        if (eCodeType != tCIDNet::EHTTPCodes::Success)
        {
            facCIDXML().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kXMLErrs::errcURL_HTTPGetFailed
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , strRepLine
            );
        }

        // Looks ok so create the memory buffer
        pstrmRet = new TBinMBufInStream
        (
            janBuf.pobjOrphan(), c4ContLen, tCIDLib::EAdoptOpts::Adopt
        );
    }
     else
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcURL_Unsupported
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotSupported
        );
    }
    return pstrmRet;
}


