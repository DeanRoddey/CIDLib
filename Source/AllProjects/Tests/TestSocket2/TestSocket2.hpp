//
// FILE NAME: TestSocket2.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/10/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
//  This header brings in the other headers we need and defines our program's
//  facility class. This program basically does a simple simulation of a
//  particularly type of server program. In this case it's a socket server
//  of the type where there is not one thread serving each connection. Instead
//  it has a single thread waiting on a multi-select, and then hands off the
//  work into a queue that is serviced by a set of work threads (just one
//  in this simple example). It's the kind of thing that an ORB might do, though
//  a real ORB would have more than one worker thread.
//
//  So the server side has three threads. One is a listener thread, which looks
//  for connections and puts them into a list of connected sockets, which is
//  used for doing a multi-select and watch for data to arrive. The select is
//  done by a server thread which pulls the incoming data off the socket and
//  posting a little packet to a queue that is served by a response thread,
//  which will respond back to the caller.
//
//  This leaves out the thread pool that would normally handle the processing
//  of the work, since the work is trivial in this case.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes. This program is so simple that we don't even have a header of
//  our own. So just include CIDSock, which brings in all we need.
// ---------------------------------------------------------------------------
#include    "CIDSock.hpp"


// ---------------------------------------------------------------------------
//   CLASS: TWorkPacket
//  PREFIX: wpack
// ---------------------------------------------------------------------------
class TWorkPacket : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TWorkPacket(        TStreamSocket* const    psockReply
                    , const tCIDLib::TCard4         c4Data) :

            m_c4Data(c4Data)
            , m_psockReply(psockReply)
        {
        }

        ~TWorkPacket()
        {
            m_psockReply->c4Shutdown();
            delete m_psockReply;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4Data;
        TStreamSocket*  m_psockReply;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TWorkPacket,TObject)
};


// ---------------------------------------------------------------------------
//   CLASS: TFacTestSocket2
//  PREFIX: fac
// ---------------------------------------------------------------------------
class TFacTestSocket2 : public TFacility, public MSignalHandler
{
    public :
        // -------------------------------------------------------------------
        //  Local types
        // -------------------------------------------------------------------
        enum class EModes
        {
            Client
            , Server
        };


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacTestSocket2();

        ~TFacTestSocket2();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bHandleSignal
        (
            const   tCIDLib::ESignals       eSignal
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eMainThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        tCIDLib::EExitCodes eListenThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        tCIDLib::EExitCodes eWorkerThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TFacTestSocket2(const TFacTestSocket2&);
        tCIDLib::TVoid operator=(const TFacTestSocket2&);


        // -------------------------------------------------------------------
        //  Private class types
        // -------------------------------------------------------------------
        typedef TRefVector<TMSockSelItem>   TSelList;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bReplyToMsg
        (
                    TStreamSocket&          sockReply
        );

        tCIDLib::EExitCodes eInvokeClient();

        tCIDLib::EExitCodes eInvokeServer();

        tCIDLib::EExitCodes eParseParms();


        // -------------------------------------------------------------------
        //  Local data
        //
        //  m_c4Interval
        //      How long to wait on the client side between each message
        //
        //  m_c4PLTimes
        //      The number of times to send. The sizes must be from 1 to 64K.
        //
        //  m_c4ShutdownVal
        //      This is a value that is sent from the client to the server to
        //      tell it it is done. This lets the server distinguish between
        //      a client going away badly and one that's just finished. The
        //      server removes this socket from the list.
        //
        //  m_colSelect
        //      The listener thread maintains a collection of multi-select
        //      items. For each connection we get, we add a new item. When a
        //      client drops, we remove it.
        //
        //  m_colWork
        //      The server thread reads items that arrive on client connections
        //      and puts work packets onto this queue. The reply thread will
        //      pull them off and reply.
        //
        //  m_conOut
        //      Our output console for this program.
        //
        //  m_eMode
        //      Indicates the mode that we are playing in this instance. It
        //      is implied by whehter /Server is provided or not. If not, we
        //      are a server, else we are a client.
        //
        //  m_ippnPort
        //      For the server modes, this is the port to listen on. For the
        //      clients, this is the port to connect to. Defaults to 12000 if
        //      not provided.
        //
        //  m_crsOutput
        //      We have multiple threads on the server side, so we need to sync
        //      console output.
        //
        //  m_strServer
        //      The server to connect to. Means nothing for server side.
        //
        //  m_thrListener
        //      When in server mode we have a thread that listens for
        //      connections. When it gets one, it puts it in the socket list.
        // ---------------------------------------------------------------------------
        tCIDLib::TBoolean           m_bShutdown;
        tCIDLib::TCard4             m_c4Interval;
        tCIDLib::TCard4             m_c4PLTimes;
        tCIDLib::TCard4             m_c4ShutdownVal;
        TSelList                    m_colSelect;
        TRefQueue<TWorkPacket>      m_colWork;
        TOutConsole                 m_conOut;
        EModes                      m_eMode;
        tCIDLib::TIPPortNum         m_ippnPort;
        TCriticalSection            m_crsOutput;
        TString                     m_strServer;
        TThread                     m_thrListener;
        TThread                     m_thrWorker;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacTestSocket2,TFacility)
};

