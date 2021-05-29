//
// FILE NAME: CIDKernel_IP_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/30/1998
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
//  This file implements the the TKrnlIP namespace for the Win32 platform. This
//  class provides all the usual IP related helper functionality separate
//  from socket communications stuff.
//
// CAVEATS/GOTCHAS:
//
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"
#include    "CIDKernel_InternalHelpers_.hpp"
#include    <Iphlpapi.h>
#include    <netfw.h>


// ---------------------------------------------------------------------------
//  Local types
//
//  TErrorMap
//      This is a simple record used in the table that maps host TCP/IP
//      errors to our errors. We cannot use the standard one here because
//      the errors are signed instead of unsigned.
// ---------------------------------------------------------------------------
struct TErrorMap
{
    tCIDLib::TSInt      iHostErr;
    tCIDLib::TErrCode   errcCIDErr;
};


namespace
{
    namespace CIDKernel_IP_WIN32
    {
        // -----------------------------------------------------------------------
        //  Local const data
        //
        //  aXlatErrors
        //      An array of mappings from socket errors to CIDLib kernel errors.
        //      We map a good bit of them to general errors, and some are mapped
        //      to error codes added just for TCP/IP support.
        //
        //  c4ErrCount
        //      The number of elements in the aXlatErrors array.
        //
        //  c2MinVersion
        //      The minimium version of the WinSock subsystem that we require.
        // -----------------------------------------------------------------------
        constexpr TErrorMap amapErrors[] =
        {
                { WSAEINTR          , kKrnlErrs::errcGen_Interrupted }
            ,   { WSAEBADF          , kKrnlErrs::errcData_InvalidHandle }
            ,   { WSAEACCES         , kKrnlErrs::errcAcc_InvalidAccess }
            ,   { WSAEFAULT         , kKrnlErrs::errcGen_GeneralFault }
            ,   { WSAEINVAL         , kKrnlErrs::errcData_InvalidValue }
            ,   { WSAEMFILE         , kKrnlErrs::errcFl_TooManyOpenFiles }
            ,   { WSAEWOULDBLOCK    , kKrnlErrs::errcGen_WouldBlock }
            ,   { WSAEINPROGRESS    , kKrnlErrs::errcGen_InProgress }
            ,   { WSAEALREADY       , kKrnlErrs::errcGen_Already }
            ,   { WSAENOTSOCK       , kKrnlErrs::errcNet_NotASocket }
            ,   { WSAEDESTADDRREQ   , kKrnlErrs::errcNet_DestAdrRequired }
            ,   { WSAEMSGSIZE       , kKrnlErrs::errcNet_MsgSize }
            ,   { WSAEPROTOTYPE     , kKrnlErrs::errcNet_ProtoType }
            ,   { WSAENOPROTOOPT    , kKrnlErrs::errcNet_NoProtoOpts }
            ,   { WSAEPROTONOSUPPORT, kKrnlErrs::errcGen_NotSupported }
            ,   { WSAESOCKTNOSUPPORT, kKrnlErrs::errcGen_NotSupported }
            ,   { WSAEOPNOTSUPP     , kKrnlErrs::errcGen_NotSupported }
            ,   { WSAEPFNOSUPPORT   , kKrnlErrs::errcGen_NotSupported }
            ,   { WSAEAFNOSUPPORT   , kKrnlErrs::errcGen_NotSupported }
            ,   { WSAEADDRINUSE     , kKrnlErrs::errcGen_Busy }
            ,   { WSAEADDRNOTAVAIL  , kKrnlErrs::errcGen_NotAvailable }
            ,   { WSAENETDOWN       , kKrnlErrs::errcNet_NetworkDown }
            ,   { WSAENETUNREACH    , kKrnlErrs::errcNet_NetworkUnreachable }
            ,   { WSAENETRESET      , kKrnlErrs::errcNet_NetworkReset }
            ,   { WSAECONNABORTED   , kKrnlErrs::errcNet_ConnAborted }
            ,   { WSAECONNRESET     , kKrnlErrs::errcNet_ConnReset }
            ,   { WSAENOBUFS        , kKrnlErrs::errcNet_NoBuffers }
            ,   { WSAEISCONN        , kKrnlErrs::errcGen_AlreadyConnected }
            ,   { WSAENOTCONN       , kKrnlErrs::errcGen_NotConnected }
            ,   { WSAESHUTDOWN      , kKrnlErrs::errcNet_Shutdown }
            ,   { WSAETOOMANYREFS   , kKrnlErrs::errcGen_TooMany }
            ,   { WSAETIMEDOUT      , kKrnlErrs::errcGen_Timeout }
            ,   { WSAECONNREFUSED   , kKrnlErrs::errcNet_ConnRefused }
            ,   { WSAELOOP          , kKrnlErrs::errcNet_Loop }
            ,   { WSAENAMETOOLONG   , kKrnlErrs::errcData_NameTooLong }
            ,   { WSAEHOSTDOWN      , kKrnlErrs::errcNet_HostDown }
            ,   { WSAEHOSTUNREACH   , kKrnlErrs::errcNet_HostUnreachable }
            ,   { WSAENOTEMPTY      , kKrnlErrs::errcGen_NotEmpty }
            ,   { WSAEPROCLIM       , kKrnlErrs::errcProc_ProcessLimit }
            ,   { WSAEUSERS         , kKrnlErrs::errcNet_Users }
            ,   { WSAEDQUOT         , kKrnlErrs::errcNet_Quota }
            ,   { WSAESTALE         , kKrnlErrs::errcNet_Stale }
            ,   { WSAEREMOTE        , kKrnlErrs::errcNet_Remote }
            ,   { WSAEDISCON        , kKrnlErrs::errcNet_Disconnect }
            ,   { WSANOTINITIALISED , kKrnlErrs::errcGen_NotInitialized }
        };
        constexpr tCIDLib::TCard4 c4ErrCount = tCIDLib::c4ArrayElems(amapErrors);
        constexpr tCIDLib::TCard2 c2MinVersion = 0x0202;


        // -----------------------------------------------------------------------
        //  Local data
        //
        //  c2Version
        //      This is the version of the host TCP/IP support that we are using.
        //      It is saved here during init and used by the TCP/IP version
        //      support method of TKrnlSysInfo that we implement here.
        // -----------------------------------------------------------------------
        tCIDLib::TCard2     c2Version = 0;


        // -----------------------------------------------------------------------
        //  We fault in info about whether IPV4 and 6 are installed. And a flag that
        //  let's the user allow us to auto-decide what interface to use when doing
        //  name to address translation (when the caller doesn't indicate a specific
        //  one.) This is set via the environment variable CID_AUTOIPINTF being
        //  set.
        // -----------------------------------------------------------------------
        TAtomicFlag         atomProtoLoaded;
        tCIDLib::TBoolean   bIPV4Avail    = kCIDLib::False;
        tCIDLib::TBoolean   bIPV6Avail    = kCIDLib::False;
        tCIDLib::TBoolean   bAutoIPIntf   = kCIDLib::False;


        // -----------------------------------------------------------------------
        //  We support an environment variable that lets the user do some things to
        //  help with name resolution issues that have cropped up in some versions of
        //  Windows. The values create a bitmap, with the lower two indicating case
        //  and the 3rd indicating whether to add a trailing period
        //
        //      0 = Default, do nothing
        //      1 = Force it all lower case     (=Lower)
        //      2 = Force it all upper case     (=Upper)
        //      5 = Lower plus trailing period  (=LowerPeriod)
        //      6 = Lower plus trailing period  (=UpperPeriod)
        // -----------------------------------------------------------------------
        tCIDLib::TCard4     c4DNSNameStyle = 0;
    }
}


// ---------------------------------------------------------------------------
//  Local helper methods
// ---------------------------------------------------------------------------

//
//  Convert a system adaptor info structure to one of our adaptor info
//  structures.
//
static tCIDLib::TVoid
CvtAdapterInfo(TKrnlIP::TAdaptorInfo& kadpFill, IP_ADAPTER_ADDRESSES& Adapter)
{
    tCIDLib::TIPPortNum ippnDummy;

    // And copy over the information
    TRawStr::pszConvert
    (
        Adapter.AdapterName, kadpFill.szName, tCIDLib::c4ArrayElems(kadpFill.szName)
    );

    TRawStr::CopyStr
    (
        kadpFill.szDescr, Adapter.Description, tCIDLib::c4ArrayElems(kadpFill.szDescr)
    );

    kadpFill.bDHCPEnabled = (Adapter.Flags & IP_ADAPTER_DHCP_ENABLED) != 0;
    kadpFill.bDedicated   = (Adapter.ConnectionType == NET_IF_CONNECTION_DEDICATED);
    kadpFill.bIPV4Enabled = (Adapter.Flags & IP_ADAPTER_IPV4_ENABLED) != 0;
    kadpFill.bIPV6Enabled = (Adapter.Flags & IP_ADAPTER_IPV6_ENABLED) != 0;
    kadpFill.bLoopback    = (Adapter.IfType == IF_TYPE_SOFTWARE_LOOPBACK);
    kadpFill.bReady       = (Adapter.OperStatus == IfOperStatusUp);
    kadpFill.bTunnel      = (Adapter.IfType == IF_TYPE_TUNNEL);

    // Setore the DNS address if available
    if (Adapter.FirstDnsServerAddress)
    {
        kadpFill.kipaDNS1.bFromSockAddr
        (
            Adapter.FirstDnsServerAddress->Address.lpSockaddr
            , Adapter.FirstDnsServerAddress->Address.iSockaddrLength
            , ippnDummy
        );
    }
     else
    {
        kadpFill.kipaDNS1.Reset();
    }

    // Store the hardware address if we can
    if (Adapter.PhysicalAddressLength> tCIDLib::c4ArrayElems(kadpFill.ac1HWAddr))
    {
        // It's something we cannot understand
        kadpFill.c4HWAddrSz = 0;
    }
     else
    {
        kadpFill.c4HWAddrSz = Adapter.PhysicalAddressLength;
        TRawMem::CopyMemBuf
        (
            kadpFill.ac1HWAddr, Adapter.PhysicalAddress, kadpFill.c4HWAddrSz
        );
    }

    // Pull out all the addresses, or up to the number we can accept
    IP_ADAPTER_UNICAST_ADDRESS* pCurAddr = Adapter.FirstUnicastAddress;
    kadpFill.c4AddrCnt = 0;
    while (pCurAddr && (kadpFill.c4AddrCnt < kCIDSock::c4MaxAddrs))
    {
        kadpFill.akipaAddrs[kadpFill.c4AddrCnt].bFromSockAddr
        (
            pCurAddr->Address.lpSockaddr
            , pCurAddr->Address.iSockaddrLength
            , ippnDummy
        );
        kadpFill.c4AddrCnt++;
        pCurAddr = pCurAddr->Next;
    }
}


//
//  Load up IP V4/6 availability flags. We have to enumerate the available
//  protocols. This is faulted in. The caller just checks the flag once. We
//  have to lock and check it again to be sure.
//
static tCIDLib::TVoid LoadIPVAvailInfo()
{
    if (!CIDKernel_IP_WIN32::atomProtoLoaded)
    {
        TBaseLock lockInit;
        if (!CIDKernel_IP_WIN32::atomProtoLoaded)
        {
            // Assume neither till proven otherwise
            CIDKernel_IP_WIN32::bIPV4Avail = kCIDLib::False;
            CIDKernel_IP_WIN32::bIPV6Avail = kCIDLib::False;

            TKrnlLList<TKrnlIP::TAdaptorInfo> kllstAdapters;
            if (TKrnlIP::bQueryAdaptorList(kllstAdapters))
            {
                //
                //  Query the adapters in the system. Loop through them and for any that
                //  we care about, check to see if they are enabled for either family.
                //
                //  We ignore tunneling interfaces and non-dedicated connections.
                //
                if (kllstAdapters.bResetCursor())
                {
                    TKrnlIP::TAdaptorInfo* pkaiCur = nullptr;
                    while (kllstAdapters.bNext(pkaiCur))
                    {
                        if (pkaiCur->bTunnel || !pkaiCur->bDedicated)
                            continue;

                        if (pkaiCur->bIPV4Enabled)
                            CIDKernel_IP_WIN32::bIPV4Avail = kCIDLib::True;

                        if (pkaiCur->bIPV6Enabled)
                            CIDKernel_IP_WIN32::bIPV6Avail = kCIDLib::True;
                    }
                }
            }
            else
            {
                // Not much we can do, assume V4
                CIDKernel_IP_WIN32::bIPV4Avail = kCIDLib::True;
            }

            // See if we should enable the auto-IP interface option
            if (TKrnlEnvironment::pszFind(L"CID_AUTOIPINTF"))
                CIDKernel_IP_WIN32::bAutoIPIntf = kCIDLib::True;

            CIDKernel_IP_WIN32::atomProtoLoaded.Set();
        }
    }
}


// ---------------------------------------------------------------------------
//  TCIDKrnlModule: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDKrnlModule::bInitTermIP(const tCIDLib::EInitTerm eState)
{
    // We only have pre-constructor init
    if (eState == tCIDLib::EInitTerm::Initialize)
    {
        tCIDLib::TCard2 c2Version = CIDKernel_IP_WIN32::c2MinVersion;
        WSADATA Data;

        tCIDLib::TCard4 c4Err = ::WSAStartup(c2Version, &Data);
        if (c4Err)
        {
            TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(c4Err), c4Err);
            return kCIDLib::False;
        }

        // Save the version that it really is
        CIDKernel_IP_WIN32::c2Version = Data.wVersion;

        // Look up the DNS name style variable, which is optional
        CIDKernel_IP_WIN32::c4DNSNameStyle = 0;
        tCIDLib::TCh szVal[64UL];
        if (::GetEnvironmentVariable(L"CID_DNSNAMESTYLE", szVal, 63))
        {
            if (TRawStr::eCompareStrI(szVal, L"Lower") == tCIDLib::ESortComps::Equal)
                CIDKernel_IP_WIN32::c4DNSNameStyle = 1;
            else if (TRawStr::eCompareStrI(szVal, L"Upper") == tCIDLib::ESortComps::Equal)
                CIDKernel_IP_WIN32::c4DNSNameStyle = 2;
            else if (TRawStr::eCompareStrI(szVal, L"LowerPeriod") == tCIDLib::ESortComps::Equal)
                CIDKernel_IP_WIN32::c4DNSNameStyle = 5;
            else if (TRawStr::eCompareStrI(szVal, L"UpperPeriod") == tCIDLib::ESortComps::Equal)
                CIDKernel_IP_WIN32::c4DNSNameStyle = 6;
        }
    }
     else
    {
        WSACleanup();
    }
    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  TKrnlTCPIP: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlIP::bAddToFirewall(const   tCIDLib::TCh* const pszAppPath
                        , const tCIDLib::TCh* const pszAppName
                        ,       tCIDLib::TCh* const pszErrText
                        , const tCIDLib::TCard4     c4MaxErrChars)
{
    INetFwMgr* pMgr = nullptr;
    INetFwPolicy* pPolicy = nullptr;
    INetFwProfile* pProfile = nullptr;
    INetFwAuthorizedApplication* pApp = nullptr;
    INetFwAuthorizedApplications* pApps = nullptr;

    HRESULT hRes = 0;
    tCIDLib::TBoolean bRes = kCIDLib::False;
    try
    {
        hRes = ::CoCreateInstance
        (
            __uuidof(NetFwMgr)
            , NULL
            , CLSCTX_INPROC_SERVER
            , __uuidof(INetFwMgr)
            , tCIDLib::pToVoidPP(&pMgr)
        );
        if (FAILED(hRes))
            throw kKrnlErrs::errcFW_CantInit;
        TCOMJanitor<INetFwMgr> janMgr(&pMgr);

        // Retrieve the local firewall policy.
        INetFwProfile* pProfile = nullptr;
        hRes = pMgr->get_LocalPolicy(&pPolicy);
        if (FAILED(hRes))
            throw kKrnlErrs::errcFW_CantGetPolicy;
        TCOMJanitor<INetFwPolicy> janPolicy(&pPolicy);

        // Retrieve the firewall profile currently in effect.
        hRes = pPolicy->get_CurrentProfile(&pProfile);
        if (FAILED(hRes))
            throw kKrnlErrs::errcFW_CantGetProfile;
        TCOMJanitor<INetFwProfile> janProfile(&pProfile);

        //
        //  First check to see if the application is already authorized.
        //  So get the list of authorized apps and look for one with the
        //  passed path.
        //
        hRes = pProfile->get_AuthorizedApplications(&pApps);
        if (FAILED(hRes))
            throw kKrnlErrs::errcFW_CantQueryApps;

        // Allocate a string for the app name.
        TBSTRJanitor janAppName(pszAppName);

        // Allocate a bstring for the app name and look for it in the list
        TBSTRJanitor janAppPath(pszAppPath);
        hRes = pApps->Item(janAppPath.bsData, &pApp);

        //
        //  If we found an item, then make sure it's got the right values,
        //  else we have to update it. If we don't find one, then add one.
        //
        if (SUCCEEDED(hRes))
        {
            TCOMJanitor<INetFwAuthorizedApplication> janApp(&pApp);

            VARIANT_BOOL Enabled;
            hRes = pApp->get_Enabled(&Enabled);
            if (FAILED(hRes))
                throw kKrnlErrs::errcFW_CantQueryAppState;

            // If not endabled, then enable it
            if (!Enabled)
            {
                Enabled = TRUE;
                hRes = pApp->put_Enabled(Enabled);
                if (FAILED(hRes))
                    throw kKrnlErrs::errcFW_CantEnableApp;
            }

            // Make sure the name is updated to what was passed
            hRes = pApp->put_Name(janAppName.bsData);
            if (FAILED(hRes))
                throw kKrnlErrs::errcFW_CantSetupApp;
        }
         else
        {
            // Create an instance of an authorized application.
            hRes = ::CoCreateInstance
            (
                __uuidof(NetFwAuthorizedApplication)
                , NULL
                , CLSCTX_INPROC_SERVER
                , __uuidof(INetFwAuthorizedApplication)
                , tCIDLib::pToVoidPP(&pApp)
            );
            if (FAILED(hRes))
                throw kKrnlErrs::errcFW_CantCreateApp;
            TCOMJanitor<INetFwAuthorizedApplication> janApp(&pApp);

            // And set them
            hRes = pApp->put_ProcessImageFileName(janAppPath.bsData);
            if (FAILED(hRes))
                throw kKrnlErrs::errcFW_CantSetupApp;

            hRes = pApp->put_Name(janAppPath.bsData);
            if (FAILED(hRes))
                throw kKrnlErrs::errcFW_CantSetupApp;

            // Add the application to the collection.
            hRes = pApps->Add(pApp);
            if (FAILED(hRes))
                throw kKrnlErrs::errcFW_CantAddApp;
        }

        // It all worked so set a success return
        bRes = kCIDLib::True;
    }

    catch(const tCIDLib::TErrCode errcThrown)
    {
        // Give back the COM error message
        ::FormatMessage
        (
            FORMAT_MESSAGE_FROM_SYSTEM
            , NULL
            , hRes
            , MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
            , pszErrText
            , c4MaxErrChars
            , NULL
        );

        //
        //  Store the error and fall through to clean up. Return flag
        //  is already defaulted to false.
        //
        TKrnlError::SetLastKrnlError(errcThrown);
    }
    return bRes;
}


//
//  Report whether IPV4 and V6 are available. We do this once upon first use
//  and keep that info around for later reporting. If a protocol is installed
//  or removed while the system is up, we won't see that.
//
tCIDLib::TBoolean TKrnlIP::bIPV4Avail()
{
    LoadIPVAvailInfo();
    return CIDKernel_IP_WIN32::bIPV4Avail;
}

tCIDLib::TBoolean TKrnlIP::bIPV6Avail()
{
    LoadIPVAvailInfo();
    return CIDKernel_IP_WIN32::bIPV6Avail;
}


// Load up the text for a TCP/IP level system error code
tCIDLib::TBoolean
TKrnlIP::bLoadTCPIPMsg(const tCIDLib::TOSErrCode errcId
                        ,       tCIDLib::TCh* const pszBuffer
                        , const tCIDLib::TCard4     c4MaxChars)
{
    //
    //  Just translate the TCP/IP message into one of our errors and load
    //  that message into the caller's buffer.
    //
    return kmodCIDKernel.bLoadCIDFacMsg
    (
        TKrnlIP::c4XlatError(errcId)
        , pszBuffer
        , c4MaxChars
    );
}


tCIDLib::TBoolean
TKrnlIP::bQueryAdaptorInfo( const   tCIDLib::TCh* const pszName
                            ,       TAdaptorInfo&       kadpToFill
                            ,       tCIDLib::TBoolean&  bFound)
{
    // Assume not found
    bFound = kCIDLib::False;

    // Call the ohter one to get the list
    TKrnlLList<TAdaptorInfo> llstAdaptors;
    if (!bQueryAdaptorList(llstAdaptors))
        return kCIDLib::False;

    // If empty, then return true, with found set to false
    if (!llstAdaptors.bResetCursor())
        return kCIDLib::True;

    // Loop through them and try to find the indicated name
    TAdaptorInfo* pCur = nullptr;
    while (llstAdaptors.bNext(pCur))
    {
        if (TRawStr::bCompareStr(pCur->szName, pszName))
        {
            kadpToFill = *pCur;
            bFound = kCIDLib::True;
            break;
        }
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlIP::bQueryAdaptorList(TKrnlLList<TAdaptorInfo>& llstToFill)
{
    //
    //  Allocate a buffer. They recommend 15K or larger to make it unlikely
    //  that a reallocation is needed. We go over twice that.
    //
    ULONG uBufSz = kCIDLib::c4Sz_32K;
    tCIDLib::TCard1* pc1Buf = new tCIDLib::TCard1[uBufSz];
    TArrayJanitor<tCIDLib::TCard1> janBuf(pc1Buf);
    IP_ADAPTER_ADDRESSES* pAdapters = (IP_ADAPTER_ADDRESSES*)pc1Buf;

    //
    //  And do the query. We don't need a lot of stuff so we use the flags
    //  to keep them from being returned. We currently only report unicast
    //  addresses here, plus gateway and DNS servers.
    //
    ULONG uRes = ::GetAdaptersAddresses
    (
        AF_UNSPEC
        , GAA_FLAG_SKIP_ANYCAST
          | GAA_FLAG_SKIP_MULTICAST
          | GAA_FLAG_SKIP_DNS_SERVER
        , 0
        , pAdapters
        , &uBufSz
    );

    if (uRes != ERROR_SUCCESS)
    {
        // Just aren't any adapters, so not an error, but bFound is false
        if (uRes == ERROR_NO_DATA)
            return kCIDLib::True;

        TKrnlError::SetLastHostError(uRes);
        return kCIDLib::False;
    }

    // Run through and store them all
    while (pAdapters)
    {
        TAdaptorInfo* pNew = llstToFill.pobjAddNew(new TAdaptorInfo);
        CvtAdapterInfo(*pNew, *pAdapters);
        pAdapters = pAdapters->Next;
    }
    return kCIDLib::True;
}



// Get the default local host address, non-loop back
tCIDLib::TBoolean
TKrnlIP::bQueryDefLocalAddr(        TKrnlIPAddr&            kipaToFill
                            , const tCIDSock::EAddrTypes    eType)
{

    // And pass that in to get the address info for this host
    ADDRINFOW* pInfo = nullptr;
    ADDRINFOW Filter = {0};
    Filter.ai_family = AF_UNSPEC;
    if (eType == tCIDSock::EAddrTypes::IPV4)
        Filter.ai_family = AF_INET;
    else if (eType == tCIDSock::EAddrTypes::IPV6)
        Filter.ai_family = AF_INET6;

    if (::GetAddrInfo(kCIDLib::pszEmptyZStr, nullptr, &Filter, &pInfo))
    {
        tCIDLib::TCard4 c4LastErr = ::WSAGetLastError();
        TKrnlError::SetLastKrnlError(c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }

    // Just grab the ip address of the first one
    tCIDLib::TIPPortNum ippnDummy;
    const tCIDLib::TBoolean bRet = kipaToFill.bFromSockAddr
    (
        (sockaddr*)pInfo->ai_addr, pInfo->ai_addrlen, ippnDummy
    );

    // Let the info go
    ::FreeAddrInfo(pInfo);

    return bRet;
}


//
//  Queries available IP addresses on this machine. We can optionally return
//  them in passive mode, for use by listeners. And we can optoinally include
//  loopbacks. Normally you would pass the local host name which will get non-LB
//  addresses, and then include loopbacks if you want. If you want only loopbacks,
//  pass "localhost", and the loopback flag doesn't matter.
//
//  If you pass a null or empty string, it'll replace that with the local host
//  name, so as to do the riht thing vis a vis the loopback flag.
//
tCIDLib::TBoolean
TKrnlIP::bQueryHostAddrs(   const   tCIDLib::TCh* const         pszHostName
                            ,       TKrnlLList<TKrnlIPAddr>&    llstToFill
                            , const tCIDSock::EAddrInfoFlags    eFlags
                            , const tCIDLib::TBoolean           bIncludeLoopback
                            , const tCIDLib::TBoolean           bAppend)
{
    // Get the host info for the converted name
    ADDRINFOW* pInfo = nullptr;
    ADDRINFOW Filter = {0};
    Filter.ai_flags = AI_CANONNAME;
    Filter.ai_family = PF_UNSPEC;

    // If asked, set the passive flag
    if (tCIDLib::bAllBitsOn(eFlags, tCIDSock::EAddrInfoFlags::Passive))
        Filter.ai_flags = AI_PASSIVE;

    // If they passed a null or empty string, get the local host name
    TKrnlString kstrHost;
    const tCIDLib::TCh* pszName = pszHostName;
    if (!pszHostName || !pszHostName)
    {
        if (!bQueryLocalName(kstrHost))
            return kCIDLib::False;
        pszName = kstrHost.pszValue();
    }

    if (!bAppend)
        llstToFill.RemoveAll();

    // First get non-loopbacks
    if (::GetAddrInfo(pszName, nullptr, &Filter, &pInfo))
    {
        // If that failed, then give up
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcNet_InvalidAddrString);
        return kCIDLib::False;
    }

    // Now start working through the items
    tCIDLib::TIPPortNum ippnDummy = 0;
    TKrnlIPAddr         kipaNew;
    while (pInfo)
    {
        sockaddr* pAddr = (sockaddr*)pInfo->ai_addr;

        // If it's one we care about, then take it
        if ((pAddr->sa_family == AF_INET) || (pAddr->sa_family == AF_INET6))
        {
            // If we set it sucessfully, then save it
            if (kipaNew.bFromSockAddr(pAddr, pInfo->ai_addrlen, ippnDummy))
                llstToFill.pobjAddNew(new TKrnlIPAddr(kipaNew));
        }
        pInfo = pInfo->ai_next;
    }

    // Let the info go
    ::FreeAddrInfo(pInfo);

    //
    //  If the asked for loopbacks and they didn't pass localhost or an empty
    //  string, then do a separate pass for those.
    //
    if (bIncludeLoopback && !TRawStr::bCompareStrI(pszName, L"localhost") && *pszName)
    {
        if (::GetAddrInfo(L"localhost", nullptr, &Filter, &pInfo))
        {
            // If that failed, then give up
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcNet_InvalidAddrString);
            return kCIDLib::False;
        }

        // Now start working through the items
        tCIDLib::TIPPortNum ippnDummy;
        while (pInfo)
        {
            sockaddr* pAddr = (sockaddr*)pInfo->ai_addr;

            // If it's one we care about, then take it
            if ((pAddr->sa_family == AF_INET) || (pAddr->sa_family == AF_INET6))
            {
                // If we set it sucessfully, then save it
                if (kipaNew.bFromSockAddr(pAddr, pInfo->ai_addrlen, ippnDummy))
                    llstToFill.pobjAddNew(new TKrnlIPAddr(kipaNew));
            }
            pInfo = pInfo->ai_next;
        }

        // Let the info go
        ::FreeAddrInfo(pInfo);
    }

    return kCIDLib::True;
}


//
//  Gets our local DNS name. They can, via an environment variable, force it to be
//  upper or lower cased, else it's taken as is.
//
tCIDLib::TBoolean TKrnlIP::bQueryLocalName(TKrnlString& kstrToFill)
{
    constexpr tCIDLib::TCard4 c4NameBufMax = kCIDLib::c4Sz_1K;
    tCIDLib::TCh szName[c4NameBufMax + 2];
    tCIDLib::TCard4 c4ActualSz = c4NameBufMax;
    if (!::GetComputerNameEx(ComputerNameDnsHostname, szName, &c4ActualSz))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcSysI_QueryCompName);
        return kCIDLib::False;
    }

    // The low two bits set the style, with zero meaning as is
    if ((CIDKernel_IP_WIN32::c4DNSNameStyle & 0x3) == 1)
        TRawStr::pszLowerCase(szName);
    else if ((CIDKernel_IP_WIN32::c4DNSNameStyle & 0x3) == 2)
        TRawStr::pszUpperCase(szName);

    // Third bit indicates whether we add a period
    if (CIDKernel_IP_WIN32::c4DNSNameStyle & 0x4)
        TRawStr::CatStr(szName, L".", c4NameBufMax);

    kstrToFill.Set(szName);
    return kCIDLib::True;
}


// Does a DNS lookup from address to host name.
tCIDLib::TBoolean
TKrnlIP::bQueryNameByAddr(  const   TKrnlIPAddr&    kipaToQuery
                            ,       TKrnlString&    kstrName)
{
    //
    //  Get the IP address into a socket address for the address type.
    //  This storage struct is guaranteed to be big enough to hold a
    //  valid address lf whatever family.
    //
    //  If the passed address isn't set up or for some reason he thinks that
    //  there's not enough room, he'll set the error so we just return false.
    //  He'll zero it out before setting the address and family info.
    //
    SOCKADDR_STORAGE AddrStore;
    tCIDLib::TIPPortNum ippnDummy = 0;
    tCIDLib::TCard4 c4Size = sizeof(AddrStore);
    if (!kipaToQuery.bToSockAddr(&AddrStore, c4Size, ippnDummy))
        return kCIDLib::False;

    // And get the host info for the indicated address
    tCIDLib::TCh szHost[NI_MAXHOST + 1];
    int Res = ::GetNameInfoW
    (
        (sockaddr*)&AddrStore, c4Size, szHost, NI_MAXHOST, 0, 0, NI_NAMEREQD
    );

    if (Res)
    {
        tCIDLib::TCard4 c4LastErr = WSAGetLastError();
        TKrnlError::SetLastKrnlError(c4XlatError(c4LastErr), c4LastErr);
        return kCIDLib::False;
    }

    // Copy the name out
    kstrName.Set(szHost);

    return kCIDLib::True;
}


// Returns the major/minor TCP/IP support versions
tCIDLib::TBoolean
TKrnlIP::bQueryTCPVersion(  tCIDLib::TCard4&    c4MajVersion
                            , tCIDLib::TCard4&  c4MinVersion)
{
    // Major version is in the low byte, minor in the high
    c4MinVersion = tCIDLib::TCard4(CIDKernel_IP_WIN32::c2Version >> 8);
    c4MajVersion = tCIDLib::TCard4(CIDKernel_IP_WIN32::c2Version & 0xFF);

    // In our implmentation, we have this cached away so it can't fail
    return kCIDLib::True;
}


// Converts the passed IP address into the texutal form
tCIDLib::TBoolean
TKrnlIP::bTextFromIPAddr(const  TKrnlIPAddr&    kipaToConvert
                        ,       TKrnlString&    kstrToFill)
{
    // Watch for an unset address special case.
    if (kipaToConvert.eType() == tCIDSock::EAddrTypes::Unspec)
    {
        kstrToFill.Set(L"[unspecaddr]");
        return kCIDLib::True;
    }

    // Get a socket address of the right type for the passed IP address
    SOCKADDR_STORAGE AddrStore;
    tCIDLib::TIPPortNum ippnDummy = 0;
    tCIDLib::TCard4 c4Size = sizeof(AddrStore);
    if (!kipaToConvert.bToSockAddr(&AddrStore, c4Size, ippnDummy))
        return kCIDLib::False;

    // Try the conversion
    constexpr tCIDLib::TCard4 c4Len = kCIDLib::c4Sz_1K;
    tCIDLib::TCh achBuf[c4Len + 1];

    tCIDLib::TCard4 c4InLen = c4Len;
    const INT iRes = ::WSAAddressToString
    (
        reinterpret_cast<SOCKADDR*>(&AddrStore), c4Size, 0, achBuf, &c4InLen
    );

    if (iRes)
    {
        TKrnlError::SetLastKrnlError
        (
            kKrnlErrs::errcNet_InvalidAddr, ::WSAGetLastError()
        );
        return kCIDLib::False;
    }

    // Get into the caller's buffer
    kstrToFill.Set(achBuf);

    return kCIDLib::True;
}


// Translates a TCP/IP error to one of our errors
tCIDLib::TCard4 TKrnlIP::c4XlatError(const tCIDLib::TSInt iToXlat)
{
    //
    //  Search the list for the passed error. Since it is a pretty short
    //  list we just do a linear search.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < CIDKernel_IP_WIN32::c4ErrCount; c4Index++)
    {
        if (CIDKernel_IP_WIN32::amapErrors[c4Index].iHostErr == iToXlat)
            return CIDKernel_IP_WIN32::amapErrors[c4Index].errcCIDErr;
    }

    // We did not find it, so return the generic 'TCPIP Error' indicator
    return kKrnlErrs::errcNet_NetError;
}


//
//  This guy will convert a text address to a numeric. If the passed string
//  is supposed to be a numeric address, then set bDoLookup to false and it
//  will fail if it cannot be directly converted. If it could be either, then
//  set bDoLookup to true, and it will first attempt the text conversion and
//  if that fails it will try to do a reverse lookup.
//
//  If the type is unspecified we'll try both types. If it's a specific type
//  we try that type and it has to work.
//
tCIDSock::EAddrCvtRes
TKrnlIP::eIPAFromText(  const   tCIDLib::TCh* const     pszIPAddrString
                        ,       TKrnlIPAddr&            kipaRet
                        , const tCIDLib::TBoolean       bDoLookup
                        , const tCIDSock::EAddrTypes    eType)
{
    SOCKADDR_STORAGE AddrStore = {0};
    sockaddr* pAddr = reinterpret_cast<sockaddr*>(&AddrStore);

    //
    //  We have to tell this stupid guy what type of address to parse, though
    //  we won't know any better than he does. So we just try 6 first. If
    //  that fails we try 4.
    //
    INT iAddrSz = sizeof(AddrStore);
    tCIDLib::TIPPortNum ippnDummy;
    INT iRes;

    //
    //  If not explicitly intended to be IPV4 and it's not empty or null,
    //  then try 6
    //
    if (pszIPAddrString && *pszIPAddrString && (eType != tCIDSock::EAddrTypes::IPV4))
    {
        pAddr->sa_family = AF_INET6;
        iRes = ::WSAStringToAddress
        (
            (LPTSTR)pszIPAddrString, AF_INET6, 0, (SOCKADDR*)&AddrStore, &iAddrSz
        );

        // If it worked, then set it and return it was a numeric address
        if (!iRes)
        {
            if (!kipaRet.bFromSockAddr(pAddr, sizeof(AddrStore), ippnDummy))
                return tCIDSock::EAddrCvtRes::Failed;
            return tCIDSock::EAddrCvtRes::WasNumeric;
        }
    }

    // If not explicitly intended to be IPV6 and not null or empty, try 4
    if (pszIPAddrString && *pszIPAddrString && (eType != tCIDSock::EAddrTypes::IPV6))
    {
        TRawMem::SetMemBuf(&AddrStore, sizeof(AddrStore), kCIDLib::c1MinCard);
        pAddr->sa_family = AF_INET;

        iAddrSz = sizeof(AddrStore);
        iRes = ::WSAStringToAddress
        (
            (LPTSTR)pszIPAddrString, AF_INET, 0, (SOCKADDR*)&AddrStore, &iAddrSz
        );

        // If it worked, then set it and return it was a numeric address
        if (!iRes)
        {
            if (!kipaRet.bFromSockAddr(pAddr, sizeof(AddrStore), ippnDummy))
                return tCIDSock::EAddrCvtRes::Failed;
            return tCIDSock::EAddrCvtRes::WasNumeric;
        }
    }

    // It failed, and if they don't want to try a lookup, then give up
    if (!bDoLookup)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcNet_InvalidAddrString);
        return tCIDSock::EAddrCvtRes::Failed;
    }

    //
    //  Copy the type. If it is unspecified, and we are allowed to, get the default
    //  address type for this machine, which is driven by the families we previously
    //  discovered enabled on those interfaces that we could possibly want to use.
    //  If we have both, it will remain unspec and we'll take what we get. Otherwise,
    //  we'll ask for an address we know we can handle.
    //
    tCIDSock::EAddrTypes eRealType = eType;
    if (CIDKernel_IP_WIN32::bAutoIPIntf)
    {
        if (eRealType == tCIDSock::EAddrTypes::Unspec)
            eRealType = eDefAddrType();
    }

    // And get the addresss info for this guy
    ADDRINFOW* pInfo = nullptr;
    ADDRINFOW Filter = {0};
    Filter.ai_family = AF_UNSPEC;
    if (eRealType == tCIDSock::EAddrTypes::IPV4)
        Filter.ai_family = AF_INET;
    else if (eRealType == tCIDSock::EAddrTypes::IPV6)
        Filter.ai_family = AF_INET6;
    if (::GetAddrInfo(pszIPAddrString, nullptr, &Filter, &pInfo))
    {
        // If that failed too, then give up
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcNet_InvalidAddrString);
        return tCIDSock::EAddrCvtRes::Failed;
    }

    // Take the first address. If we cannot set this, then we failed
    if (!kipaRet.bFromSockAddr((sockaddr*)pInfo->ai_addr, pInfo->ai_addrlen, ippnDummy))
    {
        // Let the info go and return failure
        ::FreeAddrInfoW(pInfo);
        return tCIDSock::EAddrCvtRes::Failed;
    }

    // Be sure to clean up the address info we got
    ::FreeAddrInfo(pInfo);

    // Indicate it worked via lookup
    return tCIDSock::EAddrCvtRes::ViaLookup;
}


//
//  Return the default all addresses listener thread address. We see if IP
//  V6 is installed and set up that if so. Else we set up IPV4.
//
tCIDLib::TVoid TKrnlIP::QueryDefAllListen(TKrnlIPAddr& kipaToFill)
{
    if (bIPV6Avail())
        kipaToFill.SetToSpecial(tCIDSock::ESpecAddrs::Any, tCIDSock::EAddrTypes::IPV6);
    else
        kipaToFill.SetToSpecial(tCIDSock::ESpecAddrs::Any, tCIDSock::EAddrTypes::IPV4);
}

