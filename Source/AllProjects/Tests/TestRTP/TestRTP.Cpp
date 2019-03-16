//
// FILE NAME: TestRTP.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/27/2015
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
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
//  This is the main module for the program. It provides a simple test of the
//  CIDLib RTP/RTSP functionality.
//
//  We create a simple derivative of the RTSP server that provides the application
//  specific functionality. Primarily what we have to provide at the application
//  level is how to know what media to make available and the format of that data.
//  When the base class gets a request for media info, it asks us to provide that
//  info (in SDP format.) When it gets a request to start a stream up, it asks us
//  to create and set up a transmitter, which we give back to him to use.
//
//  For media info, we just use a simple
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes. This program is so simple that we don't even have a header of
//  our own.
// ---------------------------------------------------------------------------
#include    "CIDNet.hpp"

#include    "CIDDAE.hpp"

#if         !defined(CIDRTP_HPP)
#include    "CIDRTP.hpp"
#endif



// ---------------------------------------------------------------------------
//  Local types
// ---------------------------------------------------------------------------



// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
    ,   tCIDLib::TVoid*     pData
);


// ---------------------------------------------------------------------------
//  Local data
//
//  conIn
//      We are interactive, so we need an input stream.
//
//  conOut
//      Our output console for this program. Consoles are specializations
//      of the output text stream, TTextOutStream.
//
//  ippnPort
//      For the server modes, this is the port to listen on. Defaults to the
//      standard RTSP port, but can be overridden.
// ---------------------------------------------------------------------------
static TInConsole               conIn;
static TOutConsole              conOut;
static tCIDLib::TIPPortNum      ippnPort = kCIDRTP::ippnRTSPSrv;



// ---------------------------------------------------------------------------
//  Do the magic main module code
// ---------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"TestRTP", eMainThreadFunc))



// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------

static tCIDLib::EExitCodes eParseParms()
{
    TString strParm;
    TSysInfo::TCmdLineCursor cursParms = TSysInfo::cursCmdLineParms();
    for (; cursParms; ++cursParms)
    {
        strParm = *cursParms;
        if (strParm.bStartsWithI(L"/Port="))
        {
            strParm.Cut(0, 6);
            ippnPort = strParm.c4Val();
        }
         else
        {
            conOut << L"Unknown parameter: " << strParm << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::BadParameters;
        }
    }

    return tCIDLib::EExitCodes::Normal;
}


// ---------------------------------------------------------------------------
//   CLASS: TMyRTSPSrv
//  PREFIX: rtspsrv
//
//  This is our derivative of the RTSP server that we have to extend.
// ---------------------------------------------------------------------------
class TMyRTSPSrv : public TCIDRTSPSrv
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMyRTSPSrv();

        ~TMyRTSPSrv();


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4QueryMediaInfo
        (
            const   TURL&                   urlToGet
            ,       tCIDLib::TKVPCollect&   colToFill
            ,       TString&                strRepText
        );

        TCIDRTPIOSrc* prtpioMakeNew
        (
            const   TURL&                   urlToGet
            , const tCIDLib::TKVPList&      colSessInfo
        );


    private :
};



// ---------------------------------------------------------------------------
//  TMyRTSPSrv: Constructors and Destructor
// ---------------------------------------------------------------------------

//
//  For our debugging purposes, we tell it only to create one service thread, so
//  we don't have to deal with multiple threads hitting break points during
//  testing.
//
TMyRTSPSrv::TMyRTSPSrv() :

    TCIDRTSPSrv()
{
}

TMyRTSPSrv::~TMyRTSPSrv()
{
}


// ---------------------------------------------------------------------------
//  TMyRTSPSrv: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  We override this to provide the session description data for the passed
//  URL.
//
tCIDLib::TCard4
TMyRTSPSrv::c4QueryMediaInfo(const  TURL&                   urlToGet
                            ,       tCIDLib::TKVPCollect&   colToFill
                            ,       TString&                strRepText)
{
    colToFill.objAdd(TKeyValuePair(L"v", L"0"));
    colToFill.objAdd(TKeyValuePair(L"m", L"audio 0 RTP/AVP 0"));
    colToFill.objAdd(TKeyValuePair(L"c", L"IN IP4 192.168.0.2"));

    return kCIDNet::c4HTTPStatus_OK;
}


//
//  We override this to handle creating an RTP I/O source. We get the session
//  description info as a set of key/value pairs. If we can't create it, we
//  return null. In our case we just return a known source of our own creation,
//  that references a local file in the same directory. We provided dummy data
//  above that we know matches this file, so we don't need to look at the session
//  info to set up the source.
//
TCIDRTPIOSrc*
TMyRTSPSrv::prtpioMakeNew(  const   TURL&               urlToGet
                            , const tCIDLib::TKVPList&  colSessInfo)
{
//    return new TFileWaveIO
//    (
//        L"\\\\mary\\intmedia1\\Audio\\Carolyn McManus\\Carolyn McManus - Progressive Relaxaction - 01 - Track 1.wav"
//        , 10
//    );

    return new TFileWaveIO(L"C:\\Tmp\\All Clear.wav", 0);
}




// ---------------------------------------------------------------------------
//  Entry point
// ---------------------------------------------------------------------------

//
//  This is the the thread function for the main thread. It is started by
//  the CIDLib_MainModule() macro above.
//
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    // Parse the parameters if any
    tCIDLib::EExitCodes eRes = eParseParms();
    if (eRes != tCIDLib::EExitCodes::Normal)
        return eRes;

    try
    {
        //
        //  Create our server derivative and crank it up with the configured
        //  port.
        //
        TMyRTSPSrv rtspsvTest;

        if (!rtspsvTest.bStart(ippnPort))
        {
            conOut << L"Could not initialize RTSP server" << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::Normal;
        }

        //
        //  Now loop and accept input from the user on this thread until we
        //  get an exit command.
        //
        TString strCurLn;
        while (kCIDLib::True)
        {
            conIn.c4GetLine(strCurLn);
            strCurLn.StripWhitespace();
            if (strCurLn.bIsEmpty())
                continue;

            if (strCurLn.bCompareI(L"exit"))
                break;
        }

        // Stop the server
        rtspsvTest.Stop();
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
        conOut << L"A CIDLib runtime error occured during processing.\n\n"
                 << errToCatch << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but test for
    //  them here just in case.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        conOut << L"A kernel error occured during processing.\nError="
                 << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        conOut << L"A general exception occured during processing"
                 << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }
    return tCIDLib::EExitCodes::Normal;
}


