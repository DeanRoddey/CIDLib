//
// FILE NAME: CIDNet_SMTPClient.hpp
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
//  This is the header for the CIDNet_SMTPClient.cpp file. This file
//  implements the TSMTPClient class. This class allows you to do basic client
//  side SMTP operations, i.e. send mail messages.
//
//  We have a class to represent attachements, one to represent messages to
//  be sent, and another to represent the SMTP client.
//
// CAVEATS/GOTCHAS:
//
//  1)  Because of the unfortunate decision that SMTP secure connections are not just
//      based on target port, we have to do some communciations at the raw level even
//      if we ultimately end up being secure. So, we need more than one version of
//      some methods. We have to be able to do EHLO and get a reply in the clear no
//      matter what. So we use the raw socket to do that.
//
//      Then we set up a data source (secure or not) and do the rest.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//   CLASS: TEmailAttachment
//  PREFIX: eatt
// ---------------------------------------------------------------------------
class CIDNETEXP TEmailAttachment
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TEmailAttachment();

        TEmailAttachment
        (
            const   tCIDLib::TCard4         c4BufSz
            , const TMemBuf&                mbufData
            , const TString&                strMIMEType
            , const TString&                strFileName
        );

        TEmailAttachment
        (
            const   TEmailAttachment&       eattSrc
        );

        ~TEmailAttachment();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TEmailAttachment& operator=
        (
            const   TEmailAttachment&       eattSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4BufSz() const;

        const TMemBuf& mbufData() const;

        const TString& strFileName() const;

        const TString& strMIMEType() const;

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCard4         c4BufSz
            , const TMemBuf&                mbufData
            , const TString&                strMIMEType
            , const TString&                strFileName
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4BufSz
        //      The number of bytes in the attachement buffer.
        //
        //  m_mbufData
        //      The attachement data.
        //
        //  m_strFileName
        //      The suggested file name for the attachement, if any
        //
        //  m_strMIMEType
        //      The type to indicate in the transmitted message for this
        //      attachement.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4BufSz;
        THeapBuf        m_mbufData;
        TString         m_strFileName;
        TString         m_strMIMEType;
};



// ---------------------------------------------------------------------------
//   CLASS: TEmailMsg
//  PREFIX: emsg
// ---------------------------------------------------------------------------
class CIDNETEXP TEmailMsg : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public class types and constants
        // -------------------------------------------------------------------
        using TCursor = TBag<TString>::TCursor;
        static const tCIDLib::TCard4    c4MaxAttachements = 64;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TEmailMsg();

        TEmailMsg
        (
            const   TEmailMsg&              emsgToCopy
        );

        ~TEmailMsg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TEmailMsg& operator=
        (
            const   TEmailMsg&              emsgToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddAttachment
        (
            const   tCIDLib::TCard4         c4BufSz
            , const TMemBuf&                mbufData
            , const TString&                strMIMEType
            , const TString&                strFileName
        );

        tCIDLib::TVoid AddFileAttachment
        (
            const   TString&                strMIMEType
            , const TString&                strFileName
        );

        tCIDLib::TVoid AddCCAddr
        (
            const   TString&                strToAdd
        );

        tCIDLib::TVoid AddToAddr
        (
            const   TString&                strToAdd
        );

        tCIDLib::TCard4 c4AttachmentCnt() const;

        TCursor cursCCList() const;

        TCursor cursToList() const;

        const TEmailAttachment& eattAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        tCIDLib::TVoid RemoteAttachmentAt
        (
            const   tCIDLib::TCard4         c4At
        );

        const TString& strFrom() const;

        const TString& strFrom
        (
            const   TString&                strToSet
        );

        const TString& strMsg() const;

        const TString& strMsg
        (
            const   TString&                strToSet
        );

        const TString& strTopic() const;

        const TString& strTopic
        (
            const   TString&                strToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private data types
        // -------------------------------------------------------------------
        using TAttachList = TVector<TEmailAttachment>;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pcolAttachments
        //      A list of attachements, faulted in upon use. It has a max which is
        //      set as a public constant.
        //
        //  m_pcolCCList
        //  m_pcolToList
        //      A collection of strings that hold the list of primary target
        //      addresses, and the list of 'carbon copy' targets. These are
        //      both lazily faulted in as required.
        //
        //  m_strFrom
        //      The address that the message is to show as from.
        //
        //  m_strMsg
        //      This is the message to send. It must be convertable to whatever
        //      out-going encoding is set on the SMTP client object.
        //
        //  m_strTopic
        //      The topic of the message.
        // -------------------------------------------------------------------
        mutable TAttachList*        m_pcolAttachments;
        mutable tCIDLib::TStrBag*   m_pcolCCList;
        mutable tCIDLib::TStrBag*   m_pcolToList;
        TString                     m_strFrom;
        TString                     m_strMsg;
        TString                     m_strTopic;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TEmailMsg,TObject)
};



// ---------------------------------------------------------------------------
//   CLASS: TSMTPClient
//  PREFIX: smtp
// ---------------------------------------------------------------------------
class CIDNETEXP TSMTPClient : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSMTPClient();

        TSMTPClient(const TSMTPClient&) = delete;

        ~TSMTPClient();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSMTPClient& operator=(const TSMTPClient&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddMsgToQueue
        (
                    TEmailMsg* const        pemsgToAdopt
        );

        tCIDLib::TCard4 c4ColumnWidth() const;

        tCIDLib::TCard4 c4ColumnWidth
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TCard4 c4MsgsInQueue() const;

        tCIDLib::TCard4 c4ReadTimeout() const;

        tCIDLib::TCard4 c4ReadTimeout
        (
            const   tCIDLib::TCard4         c4Millis
        );

        tCIDLib::TVoid ClearQueue();

        tCIDNet::EMailAuthTypes eAuthType() const;

        tCIDNet::EMailAuthTypes eAuthType
        (
            const   tCIDNet::EMailAuthTypes eToSet
        );

        tCIDLib::TIPPortNum ippnToUse() const;

        const TString& strFromDomain() const;

        const TString& strPassword() const;

        const TString& strUserName() const;

        tCIDLib::TVoid SendMsgs
        (
            const   tCIDLib::TCard4         c4MaxPerMsgMSs
            , const tCIDLib::TBoolean       bEvenIfEmpty = kCIDLib::False
        );

        tCIDLib::TVoid SetConnInfo
        (
            const   TString&                strFromDomain
            , const TString&                strTarSrv
            , const tCIDLib::TIPPortNum     ippnTar
            , const tCIDLib::TBoolean       bSecure
            , const TString&                strUser
            , const TString&                strPassword
        );

        tCIDLib::TVoid SetContentType
        (
            const   TString&                strType
            , const TString&                strEncoding
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Authenticate
        (
                    TCIDDataSrc&            cdsSrc
            , const tCIDLib::TEncodedTime   enctEnd
        );

        tCIDLib::TCard4 c4CallMultiResponse
        (
                    TCIDDataSrc&            cdsSrc
            , const TString&                strCall
            , const tCIDLib::TEncodedTime   enctEnd
            , const tCIDLib::TCh* const     pszFunction
            ,       tCIDLib::TStrCollect&   colToFill
        );

        tCIDLib::TCard4 c4CallResponse
        (
                    TCIDDataSrc&            cdsSrc
            , const TString&                strCall
            ,       TString&                strReply
            , const tCIDLib::TEncodedTime   enctEnd
        );

        tCIDLib::TCard4 c4GetResponse
        (
                    TCIDDataSrc&            cdsSrc
            ,       TString&                strReply
            , const tCIDLib::TEncodedTime   enctEnd
            , const tCIDLib::TCh* const     pszFunction
        );

        tCIDLib::TCard4 c4InitConn
        (
                    TStreamSocket&          sockSrc
            , const tCIDLib::TEncodedTime   enctEnd
        );

        tCIDLib::TCard4 c4ReadRawLine
        (
                    TStreamSocket&          sockSrc
            ,       TString&                strToFill
            , const tCIDLib::TEncodedTime   enctEnd
            , const tCIDLib::TCh* const     pszFunction
            ,       tCIDLib::TBoolean&      bLast
        );

        tCIDLib::TCard4 c4SplitLine
        (
                    TString&                strOrgLine
            ,       TString&                strToFill
            ,       tCIDLib::TBoolean&      bLastLine
        );

        tCIDLib::TVoid ColumnateMsg
        (
                    TCIDDataSrc&            cdsSrc
            , const TString&                strMsgText
            , const TString&                strTextEncoding
            , const tCIDLib::TEncodedTime   enctEnd
        );

        tCIDLib::TVoid Hello
        (
                    TStreamSocket&          sockSrc
            , const tCIDLib::TEncodedTime   enctEnd
        );

        tCIDLib::TVoid Hello
        (
                    TCIDDataSrc&            cdsSrc
            , const tCIDLib::TEncodedTime   enctEnd
        );

        tCIDLib::TVoid OutputAttachments
        (
                    TCIDDataSrc&            cdsSrc
            , const TEmailMsg&              emsgSrc
            , const tCIDLib::TEncodedTime   enctEnd
        );

        tCIDLib::TVoid QueryServerCaps();

        tCIDLib::TVoid Quit
        (
                    TCIDDataSrc&            cdsSrc
            , const tCIDLib::TEncodedTime   enctEnd
        );

        tCIDLib::TVoid SendAccum
        (
                    TCIDDataSrc&            cdsSrc
            , const tCIDLib::TEncodedTime   enctEnd
        );

        tCIDLib::TVoid SendAMsg
        (
                    TCIDDataSrc&            cdsSrc
            , const TEmailMsg&              emsgCur
            , const tCIDLib::TEncodedTime   enctEnd
        );

        tCIDLib::TVoid SendRawLine
        (
                    TStreamSocket&          sockSrc
            , const TString&                strToSend
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bSecure
        //      Indicates if we should use a secure connection or not. If secure, the
        //      client code should have provided the correct port for such.
        //
        //  m_bSupportsXX
        //      These are flags that are set during the connection to the server. They
        //      are really only valid during the message sending process.
        //
        //  m_c4ColWidth
        //      This is the width to which the message lines will be formatted on the
        //      way out. It defaults to 68, which is a reasonable width that should
        //      probably be left alone.
        //
        //  m_c4MaxMsgSize
        //      This is the maximum message size. Its defaulted to a very conservative
        //      value, but can be set if the server supports the message size extension.
        //
        //  m_eAuthType
        //      The authorization type to use when sending messages.
        //
        //  m_ippnToUse
        //      This is the port number to use to connect to the server. We default it
        //      to 25, and the caller must set it up otherwise if that's not appropriate.
        //
        //  m_pcolQueue
        //      This is a pointer to a collection into which added messages are placed
        //      until the SendMsgs() method is called.
        //
        //  m_strAttachEncoding
        //      The attachements are all Base64 encoded, so we use ASCII as the text
        //      encoding to spit out the Base64 content (when we columnate it.) So we
        //      just pre-set this string for that purpose.
        //
        //  m_strContEncoding
        //  m_strContType
        //      The msg body content type to indicate in the header. If not set, it is
        //      not indicated so that the server assumes the default. And the associated
        //      text encoding that we should use to generate text in that format.
        //
        //  m_strFromDomain
        //      This is the domain that the messages are to be indicated as coming from.
        //
        //  m_strPassword
        //      If authentication is required, then this is the password to send.
        //
        //  m_strServer
        //      This is the server that is to be connected to. The client code provides
        //      this.
        //
        //  m_strTmpRead
        //      A temp string used to read in msgs and break them out into status and
        //      text reply.
        //
        //  m_strmFmt
        //      In some cases we have to format first to a string, because we then need
        //      to format that resulting data with appropriate line length. This is far
        //      easier to do if we format out the text first and then break that text
        //      into appropriate lengths which we then transcode and output.
        //
        //  m_strmOut
        //      This is used to format out most of the data to send to the other side.
        //      This guy is set up for USASCII, which is fine for the commands we send
        //      to the server and any of the 'meta' text. But it can't be used for the
        //      message body, which can have a separate encoding if we are in a multi-
        //      part e-mail. The body part can be separate encoded. Attachments are
        //      effectively ASCII (base64) but would require duplicating the colunating
        //      stuff in order to use this stream to accumulate the columned lines. So
        //      it's not used for the attachements either. See ColumnateMsg().
        //
        //  m_strUserName
        //      If authentication is required, then this is the user name to send.
        //
        //  m_tmCurrent
        //      There are a couple places during the creating of a message where the
        //      current time is used. So its set here before each message is sent, and
        //      any code that needs it just looks at this guy.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bSecure;
        tCIDLib::TBoolean       m_bSupports8BitMIME;
        tCIDLib::TBoolean       m_bSupportsMsgSize;
        tCIDLib::TBoolean       m_bSupportsPipelining;
        tCIDLib::TBoolean       m_bSupportsTLS;
        tCIDLib::TCard4         m_c4ColWidth;
        tCIDLib::TCard4         m_c4MaxMsgSize;
        tCIDNet::EMailAuthTypes m_eAuthType;
        tCIDLib::TIPPortNum     m_ippnToUse;
        TRefQueue<TEmailMsg>*   m_pcolQueue;
        const TString           m_strAttachEncoding;
        TString                 m_strContEncoding;
        TString                 m_strContType;
        TString                 m_strFromDomain;
        TString                 m_strPassword;
        TString                 m_strServer;
        TString                 m_strTmpRead;
        TString                 m_strUserName;
        TTextStringOutStream    m_strmFmt;
        TTextMBufOutStream      m_strmOut;
        TTime                   m_tmCurrent;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSMTPClient,TObject)
};

#pragma CIDLIB_POPPACK


