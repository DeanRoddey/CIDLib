//
// FILE NAME: CIDNet_SMTPClient.cpp
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
//  This file implements the TSMTPClient class.
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
RTTIDecls(TEmailMsg,TObject)
RTTIDecls(TSMTPClient,TObject)


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDNet_SMTPClient
{
    // The separator we use for attachments
    const TString   strAttachSep(L"5_3333_yaf44_7454_miofe");
}



// ---------------------------------------------------------------------------
//   CLASS: TEmailAttachment
//  PREFIX: eatt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TEmailAttachment: Constructors and Destructor
// ---------------------------------------------------------------------------
TEmailAttachment::TEmailAttachment() :

    m_c4BufSz(0)
{
}

TEmailAttachment::TEmailAttachment( const   tCIDLib::TCard4 c4BufSz
                                    , const TMemBuf&        mbufData
                                    , const TString&        strMIMEType
                                    , const TString&        strFileName) :
    m_c4BufSz(c4BufSz)
    , m_mbufData(c4BufSz, c4BufSz)
    , m_strMIMEType(strMIMEType)
{
    m_mbufData.CopyIn(mbufData, c4BufSz);

    // We only keep the file name part of the name, no path
    TPathStr pathTmp(strFileName);
    pathTmp.bQueryNameExt(m_strFileName);
}

TEmailAttachment::TEmailAttachment( const   tCIDLib::TCard4 c4BufSz
                                    ,       THeapBuf&&      mbufData
                                    , const TString&        strMIMEType
                                    , const TString&        strFileName) :
    m_c4BufSz(c4BufSz)
    , m_mbufData(tCIDLib::ForceMove(mbufData))
    , m_strMIMEType(strMIMEType)
{
    // We only keep the file name part of the name, no path
    TPathStr pathTmp(strFileName);
    pathTmp.bQueryNameExt(m_strFileName);
}

TEmailAttachment::TEmailAttachment(const TEmailAttachment& eattSrc) :

    m_c4BufSz(eattSrc.m_c4BufSz)
    , m_strFileName(eattSrc.m_strFileName)
    , m_strMIMEType(eattSrc.m_strMIMEType)
{
    m_mbufData.CopyIn(eattSrc.m_mbufData, eattSrc.m_c4BufSz);
}

TEmailAttachment::~TEmailAttachment()
{
}


// ---------------------------------------------------------------------------
//  TEmailAttachment: Public operators
// ---------------------------------------------------------------------------
TEmailAttachment&
TEmailAttachment::operator=(const TEmailAttachment& eattSrc)
{
    if (&eattSrc != this)
    {
        m_c4BufSz = eattSrc.m_c4BufSz;
        m_mbufData.CopyIn(eattSrc.m_mbufData, eattSrc.m_c4BufSz);
        m_strFileName = eattSrc.m_strFileName;
        m_strMIMEType = eattSrc.m_strMIMEType;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TEmailAttachment: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TEmailAttachment::c4BufSz() const
{
    return m_c4BufSz;
}


const TMemBuf& TEmailAttachment::mbufData() const
{
    return m_mbufData;
}


const TString& TEmailAttachment::strFileName() const
{
    return m_strFileName;
}


const TString& TEmailAttachment::strMIMEType() const
{
    return m_strMIMEType;
}


tCIDLib::TVoid
TEmailAttachment::Set(  const   tCIDLib::TCard4 c4BufSz
                        , const TMemBuf&        mbufData
                        , const TString&        strMIMEType
                        , const TString&        strFileName)
{
    m_c4BufSz = c4BufSz;
    m_mbufData.CopyIn(mbufData, c4BufSz);
    m_strMIMEType = strMIMEType;

    // We only keep the file name part of the name, no path
    TPathStr pathTmp(strFileName);
    pathTmp.bQueryNameExt(m_strFileName);
}



// ---------------------------------------------------------------------------
//   CLASS: TEmailMsg
//  PREFIX: emsg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TEmailMsg: Constructors and Destructor
// ---------------------------------------------------------------------------
TEmailMsg::TEmailMsg() :

    m_pcolAttachments(nullptr)
    , m_pcolCCList(nullptr)
    , m_pcolToList(nullptr)
{
}

// A simple one for the most common scenario
TEmailMsg::TEmailMsg(const  TString&    strFrom
                    , const TString&    strTo
                    , const TString&    strTopic
                    , const TString&    strMsg) :

    m_pcolAttachments(nullptr)
    , m_pcolCCList(nullptr)
    , m_pcolToList(new tCIDLib::TStrBag())
    , m_strFrom(strFrom)
    , m_strMsg(strMsg)
    , m_strTopic(strTopic)
{
    m_pcolToList->objAdd(strTo);
}

TEmailMsg::TEmailMsg(const TEmailMsg& emsgSrc) :

    m_pcolAttachments(nullptr)
    , m_pcolCCList(nullptr)
    , m_pcolToList(nullptr)
    , m_strFrom(emsgSrc.m_strFrom)
    , m_strMsg(emsgSrc.m_strMsg)
    , m_strTopic(emsgSrc.m_strTopic)
{
    if (emsgSrc.m_pcolAttachments)
        m_pcolAttachments = new TAttachList(*emsgSrc.m_pcolAttachments);

    if (emsgSrc.m_pcolCCList)
        m_pcolCCList = new tCIDLib::TStrBag(*emsgSrc.m_pcolCCList);

    if (emsgSrc.m_pcolToList)
        m_pcolToList = new tCIDLib::TStrBag(*emsgSrc.m_pcolToList);
}

//
//  The lists can all be null if they never get used, so we can just swap out
//  a set of null lists to the source. To avoid extra overhead we don't just
//  call the move operator, we do the work here separately so we can directly
//  move the string members as well.
//
TEmailMsg::TEmailMsg(TEmailMsg&& emsgSrc) :

    m_pcolAttachments(nullptr)
    , m_pcolCCList(nullptr)
    , m_pcolToList(nullptr)
    , m_strFrom(tCIDLib::ForceMove(emsgSrc.m_strFrom))
    , m_strMsg(tCIDLib::ForceMove(emsgSrc.m_strMsg))
    , m_strTopic(tCIDLib::ForceMove(emsgSrc.m_strTopic))
{
    tCIDLib::Swap(m_pcolAttachments, emsgSrc.m_pcolAttachments);
    tCIDLib::Swap(m_pcolCCList, emsgSrc.m_pcolCCList);
    tCIDLib::Swap(m_pcolToList, emsgSrc.m_pcolToList);
}


TEmailMsg::~TEmailMsg()
{
    try
    {
        delete m_pcolAttachments;
        delete m_pcolCCList;
        delete m_pcolToList;
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }
}


// ---------------------------------------------------------------------------
//  TEmailMsg: Public, non-virtual methods
// ---------------------------------------------------------------------------
TEmailMsg& TEmailMsg::operator=(const TEmailMsg& emsgSrc)
{
    if (this != &emsgSrc)
    {
        // Copy over the easy fields first
        m_strFrom   = emsgSrc.m_strFrom;
        m_strMsg    = emsgSrc.m_strMsg;
        m_strTopic  = emsgSrc.m_strTopic;

        //
        //  Handle the lists. First, if we have one, flush ours. Then copy the
        //  source one if its present.
        //
        if (m_pcolAttachments)
            m_pcolAttachments->RemoveAll();
        if (emsgSrc.m_pcolAttachments)
        {
            if (!m_pcolAttachments)
                m_pcolAttachments = new TAttachList(4);
            *m_pcolAttachments = *emsgSrc.m_pcolAttachments;
        }

        if (m_pcolCCList)
            m_pcolCCList->RemoveAll();
        if (emsgSrc.m_pcolCCList)
        {
            if (!m_pcolCCList)
                m_pcolCCList = new tCIDLib::TStrBag;
            *m_pcolCCList = *emsgSrc.m_pcolCCList;
        }

        if (m_pcolToList)
            m_pcolToList->RemoveAll();
        if (emsgSrc.m_pcolToList)
        {
            if (!m_pcolToList)
                m_pcolToList = new tCIDLib::TStrBag;
            *m_pcolToList = *emsgSrc.m_pcolToList;
        }
    }
    return *this;
}

TEmailMsg& TEmailMsg::operator=(TEmailMsg&& emsgSrc)
{
    if (this != &emsgSrc)
    {
        m_strFrom   = tCIDLib::ForceMove(emsgSrc.m_strFrom);
        m_strMsg    = tCIDLib::ForceMove(emsgSrc.m_strMsg);
        m_strTopic  = tCIDLib::ForceMove(emsgSrc.m_strTopic);

        tCIDLib::Swap(m_pcolAttachments, emsgSrc.m_pcolAttachments);
        tCIDLib::Swap(m_pcolCCList, emsgSrc.m_pcolCCList);
        tCIDLib::Swap(m_pcolToList, emsgSrc.m_pcolToList);
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TEmailMsg: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Add an attachement to this message
tCIDLib::TVoid
TEmailMsg::AddAttachment(const  tCIDLib::TCard4 c4BufSz
                        , const TMemBuf&        mbufData
                        , const TString&        strMIMEType
                        , const TString&        strFileName)
{
    // Fault in the list if needed
    if (!m_pcolAttachments)
        m_pcolAttachments = new TAttachList(4);

    m_pcolAttachments->objPlace(c4BufSz, mbufData, strMIMEType, strFileName);
}

tCIDLib::TVoid
TEmailMsg::AddAttachment(const  tCIDLib::TCard4 c4BufSz
                        ,       THeapBuf&&      mbufData
                        , const TString&        strMIMEType
                        , const TString&        strFileName)
{
    // Fault in the list if needed
    if (!m_pcolAttachments)
        m_pcolAttachments = new TAttachList(4);

    m_pcolAttachments->objPlace
    (
        c4BufSz, tCIDLib::ForceMove(mbufData), strMIMEType, strFileName
    );
}


//
//  A convenient to add a file based attachment. We load up the file and
//  set up the buffer and buffer size.
//
tCIDLib::TVoid
TEmailMsg::AddFileAttachment(const  TString&    strMIMEType
                            , const TString&    strFileName)
{
    // Fault in the list if needed
    if (!m_pcolAttachments)
        m_pcolAttachments = new TAttachList(4);

    // Open the file
    TBinaryFile flSrc(strFileName);
    flSrc.Open
    (
        tCIDLib::EAccessModes::Multi_Read
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
    );

    // Get the size and read it into a buffer
    const tCIDLib::TCard4 c4Size = tCIDLib::TCard4(flSrc.c8CurSize());
    THeapBuf mbufFile(c4Size, c4Size);
    flSrc.c4ReadBuffer(mbufFile, c4Size, tCIDLib::EAllData::FailIfNotAll);

    m_pcolAttachments->objAdd
    (
        TEmailAttachment(c4Size, mbufFile, strMIMEType, strFileName)
    );
}



// Add an address to the CC list
tCIDLib::TVoid TEmailMsg::AddCCAddr(const TString& strToAdd)
{
    // Fault in the collection if not already
    if (!m_pcolCCList)
        m_pcolCCList = new TBag<TString>;
    m_pcolCCList->objAdd(strToAdd);
}


// And ad address to the To list
tCIDLib::TVoid TEmailMsg::AddToAddr(const TString& strToAdd)
{
    // Fault in the collection if not already
    if (!m_pcolToList)
        m_pcolToList = new TBag<TString>;
    m_pcolToList->objAdd(strToAdd);
}


// The number of attachements
tCIDLib::TCard4 TEmailMsg::c4AttachmentCnt() const
{
    if (!m_pcolAttachments)
        return 0;
    return m_pcolAttachments->c4ElemCount();
}


// Get a cursor on the CC address list
TEmailMsg::TCursor TEmailMsg::cursCCList() const
{
    // Fault in the collection if not already
    if (!m_pcolCCList)
        m_pcolCCList = new TBag<TString>;
    return TCursor(m_pcolCCList);
}


// Get a cursor on the To address list
TEmailMsg::TCursor TEmailMsg::cursToList() const
{
    // Fault in the collection if not already
    if (!m_pcolToList)
        m_pcolToList = new TBag<TString>;
    return TCursor(m_pcolToList);
}


// Access an attachement at the indicated index
const TEmailAttachment& TEmailMsg::eattAt(const tCIDLib::TCard4 c4At) const
{
    CIDAssert(m_pcolAttachments != nullptr, L"No email attachements available");
    return m_pcolAttachments->objAt(c4At);
}


// Remove the attachement at a specific index
tCIDLib::TVoid TEmailMsg::RemoteAttachmentAt(const tCIDLib::TCard4 c4At)
{
    // If the list hasn't been faulted in, then obviously this is not legal
    if (!m_pcolAttachments)
        return;
    m_pcolAttachments->RemoveAt(c4At);
}


// Get/set the from address
const TString& TEmailMsg::strFrom() const
{
    return m_strFrom;
}

const TString& TEmailMsg::strFrom(const TString& strToSet)
{
    m_strFrom = strToSet;
    return m_strFrom;
}


// Get/set the message body text
const TString& TEmailMsg::strMsg() const
{
    return m_strMsg;
}

const TString& TEmailMsg::strMsg(const TString& strToSet)
{
    m_strMsg = strToSet;
    return m_strMsg;
}


// Get/set the e-mail topic
const TString& TEmailMsg::strTopic() const
{
    return m_strTopic;
}

const TString& TEmailMsg::strTopic(const TString& strToSet)
{
    m_strTopic = strToSet;
    return m_strTopic;
}




// ---------------------------------------------------------------------------
//   CLASS: TSMTPClient
//  PREFIX: smtp
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSMTPClient: Constructors and Destructor
// ---------------------------------------------------------------------------
TSMTPClient::TSMTPClient() :

    m_bSecure(kCIDLib::False)
    , m_bSupports8BitMIME(kCIDLib::False)
    , m_bSupportsMsgSize(kCIDLib::False)
    , m_bSupportsPipelining(kCIDLib::False)
    , m_bSupportsTLS(kCIDLib::False)
    , m_c4ColWidth(0)
    , m_c4MaxMsgSize(1024)
    , m_eAuthType(tCIDNet::EMailAuthTypes::None)
    , m_ippnToUse(25)
    , m_pcolQueue(nullptr)
    , m_strAttachEncoding(L"US-ASCII")
    , m_strFromDomain(facCIDSock().strIPHostName())
    , m_strmFmt(32 * 1024)
    , m_strmOut(2048UL, 0x400000, new TUSASCIIConverter)
{
    // Set the time format to the ctime format
    m_tmCurrent.strDefaultFormat(TTime::strCTime());

    // Set the new line format on our internal formatting stream
    m_strmOut.eNewLineType(tCIDLib::ENewLineTypes::CRLF);
}

TSMTPClient::~TSMTPClient()
{
    // Clean up the message queue. Unsent messages are thrown away
    try
    {
        delete m_pcolQueue;
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }
}


// ---------------------------------------------------------------------------
//  TSMTPClient: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Add a message to the send queue
tCIDLib::TVoid TSMTPClient::AddMsgToQueue(const TEmailMsg& emsgToAdd)
{
    // If the queue isn't created yet, then create it
    if (!m_pcolQueue)
        m_pcolQueue = new TQueue<TEmailMsg>();

    // And add the new message
    m_pcolQueue->objAdd(emsgToAdd);
}

tCIDLib::TVoid TSMTPClient::AddMsgToQueue(TEmailMsg&& emsgToAdd)
{
    // If the queue isn't created yet, then create it
    if (!m_pcolQueue)
        m_pcolQueue = new TQueue<TEmailMsg>();

    // And add the new message
    m_pcolQueue->objAdd(tCIDLib::ForceMove(emsgToAdd));
}

// For the most common scenario, we can just do an emplace of the parameters
tCIDLib::TVoid
TSMTPClient::AddMsgToQueue(const    TString&    strFrom
                            , const TString&    strTo
                            , const TString&    strTopic
                            , const TString&    strMsg)
{
    // If the queue isn't created yet, then create it
    if (!m_pcolQueue)
        m_pcolQueue = new TQueue<TEmailMsg>();

    m_pcolQueue->objPlace(strFrom, strTo, strTo, strMsg);
}

// Get/set the column width that we are to use to format outgoing body text
tCIDLib::TCard4 TSMTPClient::c4ColumnWidth() const
{
    return m_c4ColWidth;
}

tCIDLib::TCard4 TSMTPClient::c4ColumnWidth(const tCIDLib::TCard4 c4NewWidth)
{
    //
    //  Don't let the width be set to something crazy. It has to be either
    //  zero for no width, or from 32 to 4195.
    //
    if (m_c4ColWidth && ((m_c4ColWidth < 32) || (m_c4ColWidth > 4196)))
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcSMTP_BadColWidth
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    m_c4ColWidth = c4NewWidth;
    return m_c4ColWidth;
}


// Get the number of messages currently in the queue
tCIDLib::TCard4 TSMTPClient::c4MsgsInQueue() const
{
    if (!m_pcolQueue)
        return 0;
    return m_pcolQueue->c4ElemCount();
}


// Remove all messages from the queue if we've created it yet
tCIDLib::TVoid TSMTPClient::ClearQueue()
{
    if (m_pcolQueue)
        m_pcolQueue->RemoveAll();
}


// Get/set the authorization type
tCIDNet::EMailAuthTypes TSMTPClient::eAuthType() const
{
    return m_eAuthType;
}

tCIDNet::EMailAuthTypes
TSMTPClient::eAuthType(const tCIDNet::EMailAuthTypes eToSet)
{
    m_eAuthType = eToSet;
    return m_eAuthType;
}



// Get the curently set IP port to contact to the server on
tCIDLib::TIPPortNum TSMTPClient::ippnToUse() const
{
    return m_ippnToUse;
}


// Get/set the currently set domain
const TString& TSMTPClient::strFromDomain() const
{
    return m_strFromDomain;
}

// Get the currently set password
const TString& TSMTPClient::strPassword() const
{
    return m_strPassword;
}

// Get the currently set user name
const TString& TSMTPClient::strUserName() const
{
    return m_strUserName;
}


//
//  Sends all the queued messages. Note that we can be told to ignore the fact
//  that we have no messages, so that we can be used as a login credential
//  authentication mechanism. We are also told if this shoudl be done over TLS
//  or not. If so, they should have set us on the right port for that.
//
tCIDLib::TVoid
TSMTPClient::SendMsgs(  const   tCIDLib::TCard4     c4MaxMSPer
                        , const tCIDLib::TBoolean   bEvenIfEmpty)
{
    //
    //  If no queue exists or its empty, then we are done, unless they are
    //  wanting us to just authenticate.
    //
    if ((!m_pcolQueue || m_pcolQueue->bIsEmpty()) && !bEvenIfEmpty)
        return;

    // If no msg text encoding has been set yet, go with ASCII
    if (m_strContEncoding.bIsEmpty())
        m_strContEncoding = L"US-ASCII";


    //
    //  Create our input and output socket based text streams. The constructor
    //  provided the server address. We get our own address and create end
    //  points for both ends. We allow our end to auto select the port to bind
    //  to.
    //
    TIPEndPoint ipepServer(m_strServer, m_ippnToUse, tCIDSock::EAddrTypes::Unspec);

    //
    //  We create a client stream socket to act as the source. Its a TCP style
    //  protocol.
    //
    TClientStreamSocket* psockClient = new TClientStreamSocket
    (
        tCIDSock::ESockProtos::TCP, ipepServer
    );

    try
    {
        // Set up the per-msg end time for this round
        #if CID_DEBUG_ON
        const tCIDLib::TEncodedTime enctEnd = TTime::enctNowPlusMSs(1000000);
        #else
        const tCIDLib::TEncodedTime enctEnd = TTime::enctNowPlusMSs(c4MaxMSPer);
        #endif

        //
        //  Upon connect we get one set of header lines, so we need to eat
        //  those first. We pass an empty string since we don't need to send
        //  anything this time. We ignore these lines.
        //
        if (c4InitConn(*psockClient, enctEnd) >= 400)
        {
            facCIDNet().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kNetErrs::errcSMTP_NoConnectLines
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NoReply
            );
        }

        //
        //  Do the greeting sequence. This will figure out any extensions that
        //  we care about and set the appropriate members. If we cannot connect,
        //  then an exception will propogate out and we'll clean up and move
        //  on.
        //
        Hello(*psockClient, enctEnd);

        //
        //  OK, at this point, if we were told to do a secure connection, see if the
        //  server supports it. If not, we have to give up. Else, we now will set up
        //  the appropriate data source.
        //
        TCIDDataSrc* pcdsSrv = nullptr;
        if (m_bSecure)
        {
            if (!m_bSupportsTLS)
            {
                facCIDNet().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kNetErrs::errcSMTP_SrvNotSecure
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::NotSupported
                );
            }

            // Indicate our intent to do encrypted session
            SendRawLine(*psockClient, L"STARTTLS");

            tCIDLib::TBoolean bLast;
            TString strMsg;
            const tCIDLib::TCard4 c4Res = c4ReadRawLine
            (
                *psockClient, strMsg, enctEnd, L"starttls", bLast
            );

            if (c4Res != 220)
            {
                // It was rejected
                facCIDNet().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kNetErrs::errcSMTP_StartTLSRejected
                    , strMsg
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::RejectedConn
                    , TCardinal(c4Res)
                );
            }

            tCIDLib::TStrList colALPN;
            pcdsSrv = new TCIDSChanClDataSrc
            (
                L"SMTP Client"
                , psockClient
                , tCIDLib::EAdoptOpts::Adopt
                , TString::strEmpty()
                , colALPN
                , tCIDSChan::EConnOpts::None
                , m_strServer
            );
        }
         else
        {
            pcdsSrv = new TCIDSockStreamDataSrc(psockClient, tCIDLib::EAdoptOpts::Adopt);
        }

        //
        //  At this point, clear the socket pointer so that the exceptio handler
        //  below knows that it doesn't have to clean up the socket. It's now owned
        //  by the data source. And put the data source into a janitor, which will
        //  adopt it (and hence the socket.)
        //
        psockClient = nullptr;
        TCIDDataSrcJan janSrc
        (
            pcdsSrv, tCIDLib::EAdoptOpts::Adopt, kCIDLib::True, enctEnd, 5000
        );

        // Start us over now with a secure hello if secure
        if (m_bSecure)
            Hello(*pcdsSrv, enctEnd);

        // Do authentication if required
        Authenticate(*pcdsSrv, enctEnd);

        //
        //  Ok, we can now call the method that will iterate the messages in
        //  the queue and send each one, removing them as they are sent.
        //  If there's a queue set, then iterate and send.
        //
        if (m_pcolQueue)
        {
            TQueue<TEmailMsg>::TNCCursor cursMsgs(m_pcolQueue);
            while (!m_pcolQueue->bIsEmpty())
            {
                //
                //  Send the current one, an dthe remove it, which will move the cursor
                //  forward to the next one.
                //
                SendAMsg(*pcdsSrv, *cursMsgs, enctEnd);
                m_pcolQueue->RemoveAt(cursMsgs);
            }
        }

        // Send a goodbye message
        Quit(*pcdsSrv, enctEnd);

        // The data source going out of scope will terminate the session
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);

        //
        //  If we never got the socket stored away, then we need to clean it up
        //  before we rethrow.
        //
        if (psockClient)
        {
            try
            {
                psockClient->Close();
            }

            catch(TError& errToCatch)
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }
        }

        throw;
    }
}


//
//  Set up all of the connection information. This has to be done before you call
//  SendMsgs().
//
tCIDLib::TVoid
TSMTPClient::SetConnInfo(const  TString&            strFromDomain
                        , const TString&            strTarSrv
                        , const tCIDLib::TIPPortNum ippnTar
                        , const tCIDLib::TBoolean   bSecure
                        , const TString&            strUser
                        , const TString&            strPassword)
{
    m_bSecure       = bSecure;
    m_ippnToUse     = ippnTar;
    m_strPassword   = strPassword;
    m_strServer     = strTarSrv;
    m_strUserName   = strUser;

    //
    //  If the from domain isn't empty take it. Else we keep the local host name
    //  that was defaulted in by the ctor.
    //
    if (!strFromDomain.bIsEmpty())
        m_strFromDomain = strFromDomain;
}


//
//  Set up the content encoding stuff. This only applies to the body text, and mostly
//  is to allow for HTML type content to be done in the default 8859-1.
//
tCIDLib::TVoid
TSMTPClient::SetContentType(const   TString&    strType
                            , const TString&    strEncoding)
{
    m_strContType = strType;
    m_strContEncoding = strEncoding;
}


// ---------------------------------------------------------------------------
//  TSMTPClient: Private, non-virtual methods
// ---------------------------------------------------------------------------

// Does the standard authentication exchange with the server
tCIDLib::TVoid
TSMTPClient::Authenticate(          TCIDDataSrc&            cdsSrc
                            , const tCIDLib::TEncodedTime   enctEnd)
{
    // If no authentication type is set, then just return
    if (m_eAuthType == tCIDNet::EMailAuthTypes::None)
        return;

    // If the user name and password isn't set,then that's a problem
    if (m_strUserName.bIsEmpty() || m_strPassword.bIsEmpty())
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcSMTP_NoAuthData
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    //
    //  Send the authentication login. If it doesn't require authentication,
    //  it'll tell us and we'll just return.
    //
    tCIDLib::TCard4 c4Result;
    TString strMsg(L"auth login");
    tCIDLib::TStrList colReplies;

    c4Result = c4CallMultiResponse(cdsSrc, strMsg, enctEnd, L"auth", colReplies);
    if (c4Result == 520)
    {
        // There's no authentication required
        return;
    }

    if ((c4Result != 334) || (colReplies[0] != L"VXNlcm5hbWU6"))
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcSMTP_UnknownAuthReply
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , TCardinal(c4Result)
            , colReplies[0]
        );
    }

    //
    //  Ok, looks like we are good to go. Send the Base64 encoded user
    //  name.
    //
    TBase64 b64Auth;
    {
        m_strmFmt.Reset();
        TTextStringInStream strmSrc(&m_strUserName);
        b64Auth.Encode(strmSrc, m_strmFmt);
        c4Result = c4CallMultiResponse
        (
            cdsSrc, m_strmFmt.strData(), enctEnd, L"user", colReplies
        );
    }

    // Should be password challenge
    if ((c4Result != 334) || (colReplies[0] != L"UGFzc3dvcmQ6"))
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcSMTP_UnknownAuthReply
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , TCardinal(c4Result)
            , colReplies[0]
        );
    }

    // Now send the password
    {
        m_strmFmt.Reset();
        TTextStringInStream strmSrc(&m_strPassword);
        b64Auth.Encode(strmSrc, m_strmFmt);
        c4Result = c4CallMultiResponse
        (
            cdsSrc, m_strmFmt.strData(), enctEnd, L"password", colReplies
        );
    }

    if (c4Result != 235)
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcSMTP_AuthFailed
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Authority
            , colReplies[0]
        );
    }
}


//
//  This method handles the standard exchange where we send something and
//  get back a set of lines. Sometimes we don't send anything, since he
//  also sends stuff upon connect, in which case the incoming string will
//  be empty.
//
tCIDLib::TCard4
TSMTPClient::c4CallMultiResponse(       TCIDDataSrc&            cdsSrc
                                , const TString&                strCall
                                , const tCIDLib::TEncodedTime   enctEnd
                                , const tCIDLib::TCh* const     pszFunction
                                ,       tCIDLib::TStrCollect&   colToFill)
{
    colToFill.RemoveAll();

    //
    //  Send the call if any, with an end line to flush it and provide
    //  the CR/LF needed
    //
    if (!strCall.bIsEmpty())
    {
        m_strmOut << strCall << kCIDLib::EndLn;
        SendAccum(cdsSrc, enctEnd);
    }

    //
    //  Now get all the responses. For each one, get the first three chars
    //  and convert it to a response. Keep the one with the highest first
    //  digit as the return. We read until we get a line without a dash after
    //  the status code.
    //
    tCIDLib::TCard4 c4Ret = 0;
    try
    {
        tCIDLib::TBoolean   bLastLine = kCIDLib::False;
        TString             strReply;
        TString             strText;
        do
        {
            // Get the next terminated input line
            if (cdsSrc.eGetLine(strReply, enctEnd) != tCIDLib::ELineRes::GotLine)
            {
                facCIDNet().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kNetErrs::errcSMTP_Timeout
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Timeout
                    , TString(pszFunction)
                );
            }

            // Split into status and line
            const tCIDLib::TCard4 c4Status = c4SplitLine(strReply, strText, bLastLine);

            // Keep the highest status returned
            if (c4Status > c4Ret)
                c4Ret = c4Status;

            // Store this one away
            colToFill.objAdd(strText);

        }   while (!bLastLine);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return c4Ret;
}


// Sends the passed call and gets a single line reply
tCIDLib::TCard4
TSMTPClient::c4CallResponse(        TCIDDataSrc&            cdsSrc
                            , const TString&                strCall
                            ,       TString&                strReply
                            , const tCIDLib::TEncodedTime   enctEnd)
{
    // If there's an outgoing call to make, send it with an end line.
    if (!strCall.bIsEmpty())
    {
        m_strmOut << strCall << kCIDLib::EndLn;
        SendAccum(cdsSrc, enctEnd);
    }

    // And wait for a single line response
    return c4GetResponse(cdsSrc, strReply, enctEnd, strCall.pszBuffer());
}


//
//  This handles the initial connection and eating the initial lines output by the
//  server, which we don't need to process. This is done at the raw socket level because
//  it has to be done whether we end up secure or non-secure. And we won't know what
//  type of data source to set up until we get through the EHLO phase.
//
//  We return the highest status number return we got
//
tCIDLib::TCard4
TSMTPClient::c4InitConn(        TStreamSocket&          sockSrc
                        , const tCIDLib::TEncodedTime   enctEnd)
{
    // Read in lines until we get an empty one
    tCIDLib::TBoolean       bLast;
    tCIDLib::TCard4         c4Ret = 0;
    tCIDLib::TEncodedTime   enctCur = 0;
    TString strCurLine;
    do
    {
        // Get a line
        const tCIDLib::TCard4 c4Status = c4ReadRawLine
        (
            sockSrc, strCurLine, enctEnd, L"connect", bLast
        );

        // Remember the highest status we got
        if (c4Status > c4Ret)
            c4Ret = c4Status;

        // If this is the last line in this response, we are done
        if (bLast)
            break;

        // Get the current time and break out if we time out
        enctCur = TTime::enctNow();
    }   while(enctCur < enctEnd);


    // If we timed out, throw
    if (enctCur >= enctEnd)
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcSMTP_Timeout
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Timeout
            , TString(L"connect")
        );
    }

    return c4Ret;
}


// Get's a single line reply
tCIDLib::TCard4
TSMTPClient::c4GetResponse(         TCIDDataSrc&            cdsSrc
                            ,       TString&                strReply
                            , const tCIDLib::TEncodedTime   enctEnd
                            , const tCIDLib::TCh* const     pszFunction)
{
    // Get the next terminated input line
    if (cdsSrc.eGetLine(m_strTmpRead, enctEnd) != tCIDLib::ELineRes::GotLine)
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcSMTP_Timeout
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Timeout
            , TString(pszFunction)
        );
    }

    // Break it into code and text and return the results
    tCIDLib::TBoolean bLast;
    const tCIDLib::TCard4 c4Ret = c4SplitLine(m_strTmpRead, strReply, bLast);

    // This should be the last line, since a single line response was expected
    if (!bLast)
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcSMTP_NotSingleLine
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Protocol
            , TString(pszFunction)
        );
    }
    return c4Ret;
}


//
//  Until we get through the helo/ehlo phase, we have to read lines at the raw
//  socket level. We break out the leading code number and return that and put the
//  remaining text into the caller's output parm.
//
//  We cannot read ahead here since we may have to switch to an encrypted scheme
//  or even if not to put the socket into a data source. So we want any data we have
//  not dealt with to still be in the socket. So we just do byte by byte reads. It
//  will only be used for a few lines at the start of the process.
//
tCIDLib::TCard4
TSMTPClient::c4ReadRawLine(         TStreamSocket&          sockSrc
                            ,       TString&                strToFill
                            , const tCIDLib::TEncodedTime   enctEnd
                            , const tCIDLib::TCh* const     pszFunction
                            ,       tCIDLib::TBoolean&      bLast)
{
    tCIDLib::TEncodedTime enctCur = 0;

    tCIDLib::TCard4 c4Status = 0;
    m_strTmpRead.Clear();
    strToFill.bIsEmpty();
    do
    {
        tCIDLib::TCard1 c1Cur;
        sockSrc.c4ReceiveRawTO(&c1Cur, enctEnd, 1);

        // Eat a CR, end on LF, else append
        if (c1Cur == 0x0A)
        {
            // Break out the two parts
            c4Status = c4SplitLine(m_strTmpRead, strToFill, bLast);
            break;
        }

        if (c1Cur != 0x0D)
            m_strTmpRead.Append(tCIDLib::TCh(c1Cur));

        enctCur = TTime::enctNow();
    }   while (enctCur < enctEnd);

    // If we timed out, then throw
    if (enctCur >= enctEnd)
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcSMTP_Timeout
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Timeout
            , TString(pszFunction)
        );
    }

    // It cannot be legally less than 100 or greater than 599
    if ((c4Status < 100) || (c4Status > 599))
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcSMTP_BadReplyCode
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c4Status)
        );
    }
    return c4Status;
}


//
//  Given a reply line from the server, break it into the code and the reply, throwing
//  if there are any issues. We return whether it's the last line in the response.
//
tCIDLib::TCard4
TSMTPClient::c4SplitLine(TString&               strOrgLine
                        , TString&              strToFill
                        , tCIDLib::TBoolean&    bLast)
{
    // Has to be at least 4 chars long
    if (strOrgLine.c4Length() < 4)
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcSMTP_BadlyFormedLine
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Protocol
        );
    }

    // Get the fourth char, which is the divider
    const tCIDLib::TCh chDiv = strOrgLine[3];

    // Split and make sure the first part is a valid number
    tCIDLib::TCard4 c4Ret;
    if (!strOrgLine.bSplit(strToFill, chDiv)
    ||  !strOrgLine.bToCard4(c4Ret, tCIDLib::ERadices::Dec))
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcSMTP_BadlyFormedLine
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Protocol
        );
    }

    // If the divider was a space, it's the last line
    bLast = (chDiv == kCIDLib::chSpace);

    return c4Ret;
}


//
//  Formats out the passed message, honoring the requested maximum column width.
//  The lines are written to the output stream for latter sending.
//
//  NOTE: We have an issue here in that the text encoding of the body text can be
//  different from that of the 'meta' text and the Base64 encoded attachments. So
//  we can't use the m_strmOut stream to output this stuff. It is set up for ASCII
//  text to do all of the other stuff.
//
//  So we get an encoding to use and set up a text converter for each one, and we
//  use a local memory buffer to transcode the text into and then write that to the
//  data source.
//
tCIDLib::TVoid
TSMTPClient::ColumnateMsg(          TCIDDataSrc&            cdsSrc
                            , const TString&                strMsgText
                            , const TString&                strTextEncoding
                            , const tCIDLib::TEncodedTime   enctEnd)
{
    //
    //  If we have no column width, just dump it as is and we are done. Else
    //  we have to break it into lines.
    //
    TJanitor<TTextConverter> janEncoder(facCIDEncode().ptcvtMake(strTextEncoding));

    tCIDLib::TCard4 c4DBytes = 0;
    if (m_c4ColWidth)
    {
        THeapBuf mbufData(m_c4ColWidth * 6);

        //
        //  Use a local buffer of the required column width, plus an extra
        //  char for the null, and an char for the line feed.
        //
        tCIDLib::TCh* pszTmpBuf = new tCIDLib::TCh[m_c4ColWidth + 4];
        TArrayJanitor<tCIDLib::TCh> janBuf(pszTmpBuf);

        //
        //  Start looping through the string and building up lines. We handle
        //  breaking lines as required.
        //
        const tCIDLib::TCard4 c4MsgLen = strMsgText.c4Length();
        tCIDLib::TCard4 c4SrcIndex = 0;
        tCIDLib::TCard4 c4OutIndex = 0;
        while (c4SrcIndex < c4MsgLen)
        {
            // Lets fill up a new line to send
            while ((c4SrcIndex < c4MsgLen) && (c4OutIndex < m_c4ColWidth))
            {
                // Get out the next character from the source string
                tCIDLib::TCh chCur = strMsgText[c4SrcIndex++];

                //
                //  If we have to stuff this line, lets put in a special up
                //  front period.
                //
                if (!c4OutIndex)
                {
                    if (chCur == kCIDLib::chPeriod)
                        pszTmpBuf[c4OutIndex++] = kCIDLib::chPeriod;
                }

                // If its a newline, then break out
                if (chCur == kCIDLib::chLF)
                    break;

                // If not an ASCII character, then replace with a space
                if (chCur > 128)
                    chCur = kCIDLib::chSpace;

                // Not a new line, so store it
                pszTmpBuf[c4OutIndex++] = chCur;
            }

            // Put on a new line
            pszTmpBuf[c4OutIndex++] = kCIDLib::chLF;

            // Terminate this line, transcode it, and write it to the output
            pszTmpBuf[c4OutIndex] = 0;

            janEncoder->c4ConvertTo(pszTmpBuf, c4OutIndex, mbufData, c4DBytes);
            cdsSrc.WriteBytes(mbufData, c4DBytes);

            // Reset the line index
            c4OutIndex = 0;
        }

        // And we need a blank line after the end
        c4DBytes = 0;
        mbufData.PutCard1(0xD, c4DBytes++);
        mbufData.PutCard1(0xA, c4DBytes++);
        cdsSrc.WriteBytes(mbufData, 2);
    }
     else
    {
        THeapBuf mbufData(strMsgText.c4Length() * 2);
        janEncoder->c4ConvertTo(strMsgText, mbufData, c4DBytes);

        // And we need a blank line after it
        mbufData.PutCard1(0xD, c4DBytes++);
        mbufData.PutCard1(0xA, c4DBytes++);

        cdsSrc.WriteBytes(mbufData, c4DBytes);
    }

    // Now send this stuff out
    SendAccum(cdsSrc, enctEnd);
}


//
//  Handles the helo/ehlo exchange. First we try ehlo, and if that fails we try
//  helo. Failure results in an exception. This has to be done at the raw socket
//  level. Until we get back from this, we don't know if we will be doing a secure
//  connection potentially (or throwing if the server doesn't support that.)
//
//  Because of the stupid way SMTP works wrt to encryption, we have to have two
//  versions of this, because we have to issue hello again after we get to the
//  encrypted stage. Though, at that point, we know ehlo is going to work so we
//  only have to deal with that scenario.
//
tCIDLib::TVoid
TSMTPClient::Hello(TStreamSocket& sockSrc, const tCIDLib::TEncodedTime enctEnd)
{
    // Reset all of the capabilities flags first so they are false if not set
    m_bSupportsMsgSize = kCIDLib::False;
    m_bSupports8BitMIME = kCIDLib::False;
    m_bSupportsPipelining = kCIDLib::False;
    m_bSupportsTLS = kCIDLib::False;

    tCIDLib::TBoolean   bLast;
    tCIDLib::TCard4     c4Result;
    TString             strMsg;

    // Send the hello command
    strMsg = L"ehlo ";
    strMsg.Append(m_strFromDomain);
    SendRawLine(sockSrc, strMsg);

    //
    //  Get the first response, which will either be failure or success. If success
    //  we look for subsequent respones. Else we either try standard helo, or if we
    //  are supposed to be secure, we give up.
    //
    c4Result = c4ReadRawLine(sockSrc, strMsg, enctEnd, L"ehlo", bLast);

    if (c4Result < 400)
    {
        //
        //  The extended greeting was successful. So lets look through the remaining
        //  lines, if any, and store the capabilities.
        //
        while (!bLast)
        {
            c4Result = c4ReadRawLine(sockSrc, strMsg, enctEnd, L"ehlo", bLast);

            if (strMsg.bCompareI(L"size"))
            {
                m_bSupportsMsgSize = kCIDLib::True;
                if (strMsg.c4Length() > 4)
                {
                    strMsg.Cut(0, 4);
                    strMsg.bToCard4(m_c4MaxMsgSize, tCIDLib::ERadices::Dec);
                }
            }
             else if (strMsg.bCompareI(L"8bitmime"))
            {
                m_bSupports8BitMIME = kCIDLib::True;
            }
             else if (strMsg.bCompareI(L"pipelining"))
            {
                m_bSupportsPipelining = kCIDLib::True;
            }
             else if (strMsg.bCompareI(L"starttls"))
            {
                m_bSupportsTLS = kCIDLib::True;
            }
        }
    }
     else if (c4Result < 500)
    {
        //
        //  The error is temporary, so throw a temporary protocol exception
        //  for this one.
        //
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcSMTP_TempProtoError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TempSituation
            , strMsg
        );
    }
     else
    {
        //
        //  The extended one failed, so we can back off and try to do the more basic
        //  greeting.
        //
        strMsg = L"helo ";
        strMsg.Append(m_strFromDomain);
        SendRawLine(sockSrc, strMsg);

        c4Result = c4ReadRawLine(sockSrc, strMsg, enctEnd, L"helo", bLast);

        // If we didn't get 200 level reply this time, then give up
        if (c4Result > 299)
        {
            facCIDNet().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kNetErrs::errcSMTP_ProtoError
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , strMsg
            );
        }
    }
}


tCIDLib::TVoid
TSMTPClient::Hello(TCIDDataSrc& cdsSrc, const tCIDLib::TEncodedTime enctEnd)
{
    // Reset all of the capabilities flags first so they are false if not set
    m_bSupportsMsgSize = kCIDLib::False;
    m_bSupports8BitMIME = kCIDLib::False;
    m_bSupportsPipelining = kCIDLib::False;
    m_bSupportsTLS = kCIDLib::False;

    tCIDLib::TCard4     c4Result;
    TString             strMsg;
    tCIDLib::TStrList   colLines;

    strMsg = L"ehlo ";
    strMsg.Append(m_strFromDomain);

    c4Result = c4CallMultiResponse(cdsSrc, strMsg, enctEnd, L"tlselho", colLines);
    if (c4Result < 400)
    {
        const tCIDLib::TCard4 c4Count = colLines.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            strMsg = colLines[c4Index];

            if (strMsg.bCompareI(L"size"))
            {
                m_bSupportsMsgSize = kCIDLib::True;
                if (strMsg.c4Length() > 4)
                {
                    strMsg.Cut(0, 4);
                    strMsg.bToCard4(m_c4MaxMsgSize, tCIDLib::ERadices::Dec);
                }
            }
             else if (strMsg.bCompareI(L"8bitmime"))
            {
                m_bSupports8BitMIME = kCIDLib::True;
            }
             else if (strMsg.bCompareI(L"pipelining"))
            {
                m_bSupportsPipelining = kCIDLib::True;
            }
             else if (strMsg.bCompareI(L"starttls"))
            {
                m_bSupportsTLS = kCIDLib::True;
            }
        }
    }
     else
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcSMTP_TLSHelloFailed
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::RejectedConn
            , TCardinal(c4Result)
        );
    }
}



//
//  This is called to format the body output if there are attachements. We go through
//  each attachement, outputting the separator lines that go between chunks, and then
//  we base 64 encode the attachement and call the regular outputer to output the
//  content in the correct columnated format.
//
//  We flush the output stream before each one because each one is sent separately (they
//  could be large and we wan't want to try to buffer them all up.
//
//  This means that the caller should send anything it has buffered before it calls
//  this.
//
tCIDLib::TVoid
TSMTPClient::OutputAttachments(         TCIDDataSrc&            cdsSrc
                                , const TEmailMsg&              emsgSrc
                                , const tCIDLib::TEncodedTime   enctEnd)
{
    TBase64                 b64Encode;
    const tCIDLib::TCard4   c4Count = emsgSrc.c4AttachmentCnt();

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TEmailAttachment& eattCur = emsgSrc.eattAt(c4Index);

        // Create an input stream over this message
        TBinMBufInStream strmSrc(&eattCur.mbufData(), eattCur.c4BufSz());

        //
        //  Output a separator and the per-attachment header lines, then
        //  a blank line.
        //
        m_strmOut   << L"--" << CIDNet_SMTPClient::strAttachSep
                    << kCIDLib::NewLn
                    << L"Content-Type: " << eattCur.strMIMEType()
                    << kCIDLib::NewLn
                    << L"Content-Transfer-Encoding: base64"
                    << kCIDLib::NewLn
                    << L"Content-Disposition: attachment; filename = "
                    << eattCur.strFileName()
                    << kCIDLib::DNewLn;
        SendAccum(cdsSrc, enctEnd);

        // Encode the attachment to our temp formatting stream
        m_strmFmt.Reset();
        b64Encode.Encode(strmSrc, m_strmFmt);
        m_strmFmt.Flush();

        // Now columnate that and send the output plus the trailing new line
        ColumnateMsg(cdsSrc, m_strmFmt.strData(), m_strAttachEncoding, enctEnd);
    }

    // Now, we need a final terminator dashes at the end and send it out
    m_strmOut   << L"--" << CIDNet_SMTPClient::strAttachSep
                << L"--" << kCIDLib::DNewLn;
    SendAccum(cdsSrc, enctEnd);
}


//
//  Does the QUIT exchange with the server
//
tCIDLib::TVoid
TSMTPClient::Quit(TCIDDataSrc& cdsSrc, const tCIDLib::TEncodedTime enctEnd)
{
    TString strMsg(L"quit");
    TString strReply;
    #if CID_DEBUG_ON
    const tCIDLib::TCard4 c4Status =
    #endif

    c4CallResponse(cdsSrc, strMsg, strReply, enctEnd);

    // If we are in debug mode, report this failure
    #if CID_DEBUG_ON
    if (c4Status > 299)
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcSMTP_ProtoError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , strReply
        );
    }
    #endif
}


//
//  This will send any data accumulated into the output stream and then reset it for
//  subsequent use.
//
tCIDLib::TVoid
TSMTPClient::SendAccum(TCIDDataSrc& cdsSrc, const tCIDLib::TEncodedTime enctEnd)
{
    // Make sure any output is flushed
    m_strmOut.Flush();

    // And write it to the data source and flush that in turn
    cdsSrc.WriteBytes(m_strmOut.mbufData(), m_strmOut.c4CurSize());
    cdsSrc.FlushOut(enctEnd);

    // Reset the output stream for the next user
    m_strmOut.Reset();
}



//
//  Handles the logic of sending a single message out of the list of messages that
//  might have been queued.
//
tCIDLib::TVoid
TSMTPClient::SendAMsg(          TCIDDataSrc&            cdsSrc
                        , const TEmailMsg&              emsgCur
                        , const tCIDLib::TEncodedTime   enctEnd)
{
    tCIDLib::TCard4     c4Status;
    TString             strBodyTo;
    TString             strCall;
    TString             strReply;

    try
    {
        // Do the mail from exchange
        strCall = L"mail from: <";
        strCall.Append(emsgCur.strFrom());
        strCall.Append(L">");

        c4Status = c4CallResponse(cdsSrc, strCall, strReply, enctEnd);
        if (c4Status != 250)
        {
            facCIDNet().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kNetErrs::errcSMTP_MsgError
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TString(L"Mail From")
                , strReply
            );
        }

        //
        //  Do the recipient exchange for all of the to addresses in the
        //  list. While we are doing this, save up a string of them for
        //  later use in the TO of the message body.
        //
        TEmailMsg::TCursor cursTo = emsgCur.cursToList();
        for (; cursTo; ++cursTo)
        {
            strBodyTo.Append(*cursTo);
            strBodyTo.Append(kCIDLib::chComma);

            strCall = L"rcpt to: <";
            strCall.Append(*cursTo);
            strCall.Append(L">");
            c4Status = c4CallResponse(cdsSrc, strCall, strReply, enctEnd);
            if (c4Status != 250)
            {
                facCIDNet().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kNetErrs::errcSMTP_MsgError
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                    , TString(L"Rcpt To")
                    , strReply
                );
            }
        }


        // And now start the data exchange
        strCall = L"data";
        c4Status = c4CallResponse(cdsSrc, strCall, strReply, enctEnd);
        if (c4Status != 354)
        {
            facCIDNet().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kNetErrs::errcSMTP_MsgDataError
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , strReply
            );
        }

        // Get a time stamp in the correct format
        m_tmCurrent.SetToNow();

        // Send the header lines
        m_strmOut   << L"From: " << emsgCur.strFrom() << kCIDLib::NewLn
                    << L"To: " << strBodyTo << kCIDLib::NewLn
                    << L"Date: " << m_tmCurrent << kCIDLib::NewLn
                    << L"Subject: " << emsgCur.strTopic() << kCIDLib::NewLn
                    << L"Message-Id: " << TUniqueId::strMakeId() << kCIDLib::NewLn
                    << L"X-Mailer: " << L"CIDLib Network Services" << kCIDLib::NewLn;


        //
        //  If we have attachements, the we do the multi-part type for the overall
        //  message, and indicate the attachement separator. Else, put out the basic
        //  message type we were given, if any, as a header.
        //
        if (emsgCur.c4AttachmentCnt())
        {
            m_strmOut   << L"Content-Type: multipart/mixed; boundary = "
                        << CIDNet_SMTPClient::strAttachSep << kCIDLib::NewLn
                        << L"MIME-Version: 1.0" << kCIDLib::NewLn;
        }
         else if (!m_strContType.bIsEmpty())
        {
            m_strmOut   << L"Content-Type: " << m_strContType << kCIDLib::NewLn;
        }

        // And cap off the header lines with an empty line and send this stuff
        m_strmOut << kCIDLib::NewLn;
        SendAccum(cdsSrc, enctEnd);

        //
        //  If there's any  message text, output that. If we have attachements
        //  we need to put it into a multi-part separator.
        //
        if (!emsgCur.strMsg().bIsEmpty())
        {
            if (emsgCur.c4AttachmentCnt())
            {
                m_strmOut   << L"--" << CIDNet_SMTPClient::strAttachSep
                            << kCIDLib::DNewLn;

                // Do the per-part content type if we were given a msg body encoding
                if (!m_strContType.bIsEmpty())
                {
                    m_strmOut   << L"Content-Type: " << m_strContType
                                << kCIDLib::DNewLn;
                }

                // Send this before we columnate, since it will reset the stream!
                SendAccum(cdsSrc, enctEnd);
            }

            // Columnate it and send the result with trailing new line
            ColumnateMsg(cdsSrc, emsgCur.strMsg(), m_strContEncoding, enctEnd);

            //
            //  We don't have to do a closing separator because we know that
            //  attachments are still to come and a final one will be put out at
            //  the end of those.
            //
        }


        // If there are attachements, then send them.
        if (emsgCur.c4AttachmentCnt())
            OutputAttachments(cdsSrc, emsgCur, enctEnd);

        // Send the terminating line, which is a raw period
        m_strmOut << L"." << kCIDLib::EndLn;
        SendAccum(cdsSrc, enctEnd);

        // Get the response back
        c4Status = c4GetResponse(cdsSrc, strReply, enctEnd, L"msg cleanup");
        if (c4Status != 250)
        {
            facCIDNet().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kNetErrs::errcSMTP_MsgTermError
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , strReply
            );
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        //
        //  Do a reset, but catch any errors, we want the original to throw, not a
        //  reset failure, which may well happen if something went wrong above.
        //
        try
        {
            strCall = L"rset";
            c4Status = c4CallResponse(cdsSrc, strCall, strReply, enctEnd);
        }

        catch(...)
        {
        }
        throw;
    }
}


//
//  Send a line at the raw socket level, before we get our data source set up.
//
tCIDLib::TVoid
TSMTPClient::SendRawLine(TStreamSocket& sockSrc, const TString& strToSend)
{
    m_strmOut.Reset();
    m_strmOut << strToSend << kCIDLib::EndLn;
    sockSrc.Send(m_strmOut.mbufData(), m_strmOut.c4CurSize());

    // Make sure its reset for the next user
    m_strmOut.Reset();
}

