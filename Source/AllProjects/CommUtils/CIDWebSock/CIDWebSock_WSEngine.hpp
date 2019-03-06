//
// FILE NAME: CIDWebSock_WSEngine.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/24/2017
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDWebSock_WSEngine.cpp file. This file implements the
//  class that server side web socket applications would derive from to create a handler
//  for a single Websockets session. See the main facility header for high level info about
//  how it all works.
//
//  The application's derivative will override methods that get called when we get messages
//  from clients or other websockets events occur. So the derived class has to be event
//  driven and state based. That's just the nature of WebSockets. We will maintain the
//  WebSockets level state info, but the derived class will typically have to maintain some
//  application level state of his own.
//
//  We provide an idle callback so that the derived class can do some periodic processing
//  even if nothing is currently happening in terms of incoming msgs from the client. This
//  callback must be quick and to the point, so that we can go right back to listening for
//  incoming messages. It should catch any errors, otherwise we will terminate the connection
//  and this object will be dead. A new one will have to be created.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TSChannel;


// ---------------------------------------------------------------------------
//   CLASS: TCIDWebSockThread
//  PREFIX: thr
// ---------------------------------------------------------------------------
class CIDWEBSOCKEXP TCIDWebSockThread : public TThread
{
    public :
        // --------------------------------------------------------------------
        //  Public, static methods
        // --------------------------------------------------------------------
        static tCIDLib::TBoolean bIsControlType
        (
            const   tCIDLib::TCard1         c1Type
        );



        // --------------------------------------------------------------------
        // Destructor
        // --------------------------------------------------------------------
        TCIDWebSockThread() = delete;

        TCIDWebSockThread(const TCIDWebSockThread&) = delete;

        ~TCIDWebSockThread();


        // --------------------------------------------------------------------
        //  Pubilc operators
        // --------------------------------------------------------------------
        TCIDWebSockThread& operator=(const TCIDWebSockThread&) = delete;


        // --------------------------------------------------------------------
        //  Public, non-virtual methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid EnableMsgLogging
        (
            const   tCIDLib::TBoolean       bState
            , const TString&                strPath
        );

        tCIDLib::TVoid SendTextMsg
        (
            const   TString&                strText
        );

        tCIDLib::TVoid StartShutdown
        (
            const   tCIDLib::TCard2         c2Err
        );


    protected :
        // --------------------------------------------------------------------
        //  Hidden constructors
        // --------------------------------------------------------------------
        TCIDWebSockThread
        (
            const   TString&                strName
            ,       TCIDDataSrc* const      pcdsToUse
            , const tCIDLib::TBoolean       bAdoptSrc
        );


        // --------------------------------------------------------------------
        //  Protected, inherited methods
        // --------------------------------------------------------------------
        tCIDLib::EExitCodes eProcess() override;


        // --------------------------------------------------------------------
        //  Protected, virtual methods
        // --------------------------------------------------------------------
        virtual tCIDLib::TBoolean bWSInitialize() = 0;

        virtual tCIDLib::TVoid WSCheckShutdownReq() const = 0;

        virtual tCIDLib::TVoid WSConnected() = 0;

        virtual tCIDLib::TVoid WSDisconnected() = 0;

        virtual tCIDLib::TVoid WSIdle() = 0;

        virtual tCIDLib::TVoid WSProcessMsg
        (
            const   TString&                strMsg
        ) = 0;

        virtual tCIDLib::TVoid WSTerminate();


        // --------------------------------------------------------------------
        //  Protected, non-virtual methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid SendBinMsg
        (
            const   tCIDLib::TCard1         c1Type
            , const tCIDLib::TCard2         c2Payload
        );

        tCIDLib::TVoid SendMsg
        (
            const   tCIDLib::TCard1         c1Type
            , const THeapBuf&               mbufData
            , const tCIDLib::TCard4         c4DataCnt
        );

        tCIDLib::TVoid SendMsg
        (
            const   tCIDLib::TCard1         c1Type
            , const tCIDLib::TVoid* const   pData
            , const tCIDLib::TCard4         c4DataCnt
        );


    private :
        // --------------------------------------------------------------------
        //  Protected, non-virtual methods
        // --------------------------------------------------------------------
        tCIDLib::TBoolean bGetFragment
        (
                    tCIDLib::TCard1&        c1Type
            ,       tCIDLib::TBoolean&      bFinal
            ,       TMemBuf&                mbufToFill
            ,       tCIDLib::TCard4&        c4MsgBytes
        );

        tCIDLib::TVoid Cleanup();

        tCIDLib::EExitCodes eServiceClient();

        tCIDLib::TVoid HandleMsg
        (
            const   tCIDLib::TCard1         c1Type
            , const THeapBuf&               mbufData
            , const tCIDLib::TCard4         c4DataCnt
        );

        tCIDLib::TVoid SendClose
        (
            const   tCIDLib::TCard2         c2Code
        );

        tCIDLib::TVoid SendPing();




        // --------------------------------------------------------------------
        //  Private data members
        //
        //  m_bAdoptSrc
        //      Indicates if we are to adopt the provided data source or not. If so, then
        //      we clean it up when we terminate. We will always clean it up in terms of
        //      the socket session, but may also clean up the actual object if we own it.
        //
        //  m_bSimMode
        //      Some programs may want directly call the service client method for testing
        //      purposes, i.e. avoid the extra thread that they don't control. This is
        //      defaulted to true. If eProcess gets called, it will set it to false since
        //      we know we were started on a separate thread. If not in sim mode then we
        //      can do things like check for standard thread shutdown requests.
        //
        //  m_bWaitPong
        //  m_c4PingVal
        //      Remember if we are waiting for a pong response to a ping we have sent.
        //      We send a 32 bit value in each ping, which we increment each time, so
        //      this is the value we should get back in the pong. The byte order doesn't
        //      matter since the client doesn't interpret it, it just echoes it back.
        //
        //  m_colFields
        //      The derived class can ask us to monitor fields. We set up these fields
        //      on the poll engine. We get back a poll info object for each field
        //      which we use to check for changes.
        //
        //  m_eState
        //      Our current state, which the main loop uses to know what to do. It
        //      starts out in Connecting state and moves towards eventually one of the
        //      close states.
        //
        //  m_enctEndTimer
        //      When we start a close from our side, we will wait for up to a certain
        //      amount of time for the other side to respond. Else we'll give up.
        //
        //  m_enctLastInMsg
        //  m_enctLastOutMsg
        //      We track the last time we sent a message (on behalf of the derived class
        //      primarily) and the last time we got one from the client. If we haven't
        //      sent a message in the last 30 seconds, we'll send a ping so that the
        //      client doesn't give up on us.
        //
        //  m_mbufRead
        //      The eGetFragment method reads in raw fragments. If it's masked, then
        //      it's read first into m_mbufRead, then unmasked into the caller's buffer
        //      Else it's read directly into the caller's buffer.
        //
        //  m_mbufReadFrag
        //      This is used for reading in fragments. It's passed to eGetFragment()
        //      get any incoming fragement content. This is used by the main processing
        //      loop.
        //
        //  m_mbufReadMsg
        //      We need a buffer to build up fragments into to create the final msg. This
        //      is used by the main processing loop, which just adds fragements into it
        //      until the final one.
        //
        //  m_mbufWriteMsg
        //      For text messages, we need an intermediate buffer to convert outgoing
        //      text to UTF-8 encoding, and to build up the outgoing data whether binary
        //      or text.
        //
        //  m_pcdsServer
        //      We are given the data source. We may or may not own it, depending on
        //      m_bAdoptSrc.
        //
        //  m_pstrmLog
        //      The derived class can enable a log of messages exchanged. If enabled
        //      this is set, else it's null.
        //
        //  m_strTextDispatch
        //      A temp string to use for transcoding text messages to a string to pass
        //      on to the derived class.
        //
        //  m_tcvtText
        //      Text messages must be encoded to UTF8, so we keep one of these around
        //      for encoding outgoing and decoding text messages.
        //
        //  m_tmLog
        //      A time object we use in logging. We pre-set it up for the desired time
        //      format, and just set it to the current time any time we want to output
        //      it to the log.
        //
        //  m_urlReq
        //      The original request URL from the first HTTP line. We keep it around so
        //      that we can make it available to the derived class if needed.
        // --------------------------------------------------------------------
        tCIDLib::TBoolean       m_bAdoptSrc;
        tCIDLib::TBoolean       m_bSimMode;
        tCIDLib::TBoolean       m_bWaitPong;
        tCIDLib::TCard4         m_c4PingVal;
        tCIDWebSock::EStates    m_eState;
        tCIDLib::TEncodedTime   m_enctEndTimer;
        tCIDLib::TEncodedTime   m_enctLastInMsg;
        tCIDLib::TEncodedTime   m_enctLastOutMsg;
        THeapBuf                m_mbufRead;
        THeapBuf                m_mbufReadFrag;
        THeapBuf                m_mbufReadMsg;
        THeapBuf                m_mbufWriteMsg;
        TCIDDataSrc*            m_pcdsServer;
        TTextFileOutStream*     m_pstrmLog;
        TString                 m_strTextDispatch;
        TUTF8Converter          m_tcvtText;
        TTime                   m_tmLog;
        TURL                    m_urlReq;
};


#pragma CIDLIB_POPPACK
