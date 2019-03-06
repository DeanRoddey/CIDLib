//
// FILE NAME: CIDKernel_TCPIP_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 01/01/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the the TKrnlTCPIP class for the Linux platform. This
//  class provides the global view of the host TCP/IP host services, i.e.
//  stuff that is global to the workstation or process.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"


namespace
{
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


    // ---------------------------------------------------------------------------
    //  Local static data
    //
    //  __aXlatErrors
    //      An array of mappings from socket errors to CIDLib kernel errors.
    //      We map a good bit of them to general errors, and some are mapped to
    //      error codes added just for TCP/IP support.
    //
    //  __c4ErrCount
    //      The number of elements in the __aXlatErrors array.
    // ---------------------------------------------------------------------------
    const TErrorMap __amapErrors[] =
    {
            { NETDB_SUCCESS     , kKrnlErrs::errcNoError }
        ,   { HOST_NOT_FOUND    , kKrnlErrs::errcNet_HostDown }
        ,   { TRY_AGAIN         , kKrnlErrs::errcNet_HostUnreachable }
        ,   { NO_RECOVERY       , kKrnlErrs::errcGen_TermError }
        ,   { NO_DATA           , kKrnlErrs::errcGen_NoData }
    };
    const tCIDLib::TCard4 __c4ErrCount = tCIDLib::c4ArrayElems(__amapErrors);

    // ---------------------------------------------------------------------------
    //  Local functions
    // ---------------------------------------------------------------------------
    tCIDLib::TBoolean
    __bQueryHostIPAddrs(const   tCIDLib::TSCh* const    pszHostName
                        ,       tCIDLib::TIPAddr* const aipaToFill
                        , const tCIDLib::TCard4         c4MaxAddrs
                        ,       tCIDLib::TCard4&        c4AddrsFound)
    {
        // Get the host info for the passed worksation name
        struct hostent* pHostInfo = ::gethostbyname(pszHostName);

        if (!pHostInfo)
        {
            TKrnlError::SetLastKrnlError(TKrnlTCPIP::c4XlatError(h_errno), h_errno);
            return kCIDLib::False;
        }

        //
        //  Give back the caller as many elements as we can fit into his
        //  buffer.
        //
        tCIDLib::TCard4 c4Index = 0;
        while ((pHostInfo->h_addr_list[c4Index] != 0)
        &&     (c4Index < c4MaxAddrs))
        {
            aipaToFill[c4Index] = *(tCIDLib::TIPAddr*)(pHostInfo->h_addr_list[c4Index]);
            c4Index++;
        }

        //
        //  And continue to count up to see if there were any that could not
        //  fit into the caller's buffer.
        //
        while (pHostInfo->h_addr_list[c4Index] != 0)
            c4Index++;

        // And return the number that were available
        c4AddrsFound = c4Index;

        return kCIDLib::True;
    }
}


// ---------------------------------------------------------------------------
//  Local data
//
//  __c2Version
//      This is the version of the host TCP/IP support that we are using.
//      It is saved here during init and used by the TCP/IP version support
//      method of TKrnlSysInfo that we implement here.
// ---------------------------------------------------------------------------
static tCIDLib::TCard2  __c2Version = 0;



// ---------------------------------------------------------------------------
//  TCIDKrnlModule: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDKrnlModule::__bInitTermTCPIP(const tCIDLib::EInitTerm)
{
    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  TKrnlSysInfo functions
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlTCPIP::bQueryIPAddrs(          tCIDLib::TIPAddr* const aipaToFill
                            , const tCIDLib::TCard4         c4MaxAddrs
                            ,       tCIDLib::TCard4&        c4AddrsFound)
{
    struct utsname UtsName;
    if (::uname(&UtsName))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    return __bQueryHostIPAddrs(UtsName.nodename
                               , aipaToFill
                               , c4MaxAddrs
                               , c4AddrsFound);
}


tCIDLib::TBoolean
TKrnlTCPIP::bQueryIPHostName(           tCIDLib::TCh* const pszToFill
                                , const tCIDLib::TCard4     c4MaxChars)
{
    //
    //  Create a temporary short character name buffer that we will use for
    //  the call. Make it way long enough to handle anything that would
    //  every occur.
    //
    const tCIDLib::TCard4 c4MaxTmpChars = 4095;
    tCIDLib::TSCh szTmpName[c4MaxTmpChars+1];

    if (::gethostname(szTmpName, c4MaxTmpChars))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    // And get the host info for this worksation name
    struct hostent* pHostInfo = ::gethostbyname(szTmpName);

    if (!pHostInfo)
    {
        TKrnlError::SetLastKrnlError(TKrnlTCPIP::c4XlatError(h_errno), h_errno);
        return kCIDLib::False;
    }

    TRawStr::pszConvert(pHostInfo->h_name, pszToFill, c4MaxChars);

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlTCPIP::bQueryTCPVersion(     tCIDLib::TCard4&    c4MajVersion
                                , tCIDLib::TCard4&  c4MinVersion)
{
    // Major version is in the low byte, minor in the high
    c4MinVersion = tCIDLib::TCard4(__c2Version >> 8);
    c4MajVersion = tCIDLib::TCard4(__c2Version & 0xFF);

    // In our implmentation, we have this cached away so it can't fail
    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  TKrnlTCPIP: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlTCPIP::bIPAFromText(const  tCIDLib::TCh* const pszIPAddrString
                        ,       tCIDLib::TIPAddr&   ipaRet)
{
    struct in_addr InAddr;
    tCIDLib::TSCh* pszIPAddrCopy = TRawStr::pszConvert(pszIPAddrString);
    TArrayJanitor<tCIDLib::TSCh> janIPAddr(pszIPAddrCopy);

    if (::inet_aton(pszIPAddrCopy, &InAddr))
    {
        ipaRet = InAddr.s_addr;
    }
    else
    {
        // Get the host info for the passed worksation name
        struct hostent* pHostInfo = ::gethostbyname(pszIPAddrCopy);

        // If that failed too, then throw
        if (!pHostInfo)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcNet_InvalidAddrString);
            return kCIDLib::False;
        }

        // Otherwise, return the 0th address
        ipaRet = *(tCIDLib::TIPAddr*)(pHostInfo->h_addr_list[0]);
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlTCPIP::bQueryHostIPAddrs(  const   tCIDLib::TCh* const     pszHostName
                                ,       tCIDLib::TIPAddr* const aipaToFill
                                , const tCIDLib::TCard4         c4MaxAddrs
                                ,       tCIDLib::TCard4&        c4AddrsFound)
{
    tCIDLib::TSCh* pszHostCopy = TRawStr::pszConvert(pszHostName);
    TArrayJanitor<tCIDLib::TSCh> janHost(pszHostCopy);

    return __bQueryHostIPAddrs(pszHostCopy
                               , aipaToFill
                               , c4MaxAddrs
                               , c4AddrsFound);
}


tCIDLib::TBoolean
TKrnlTCPIP::bQueryHostInfo( const   tCIDLib::TCh* const     pszHostName
                            ,       tCIDLib::TIPAddr* const aipaToFill
                            ,       tCIDLib::TCard4&        c4MaxAddrs
                            ,       tCIDLib::TCh**          ppszAliases
                            ,       tCIDLib::TCard4&        c4MaxAliases)
{
    tCIDLib::TSCh* pszHostCopy = TRawStr::pszConvert(pszHostName);
    TArrayJanitor<tCIDLib::TSCh> janHost(pszHostCopy);

    // Get the host info for the passed worksation name
    struct hostent* pHostInfo = ::gethostbyname(pszHostCopy);

    if (!pHostInfo)
    {
        TKrnlError::SetLastKrnlError(c4XlatError(h_errno), h_errno);
        return kCIDLib::False;
    }

    //
    //  Give back the caller as many addresses as we can fit into his
    //  buffer.
    //
    tCIDLib::TCard4 c4Index = 0;
    while ((pHostInfo->h_addr_list[c4Index] != 0) && (c4Index < c4MaxAddrs))
    {
        aipaToFill[c4Index] = *(tCIDLib::TIPAddr*)(pHostInfo->h_addr_list[c4Index]);
        c4Index++;
    }
    c4MaxAddrs = c4Index;

    //
    //  And do the same for the aliases. We have to allocate new buffers that
    //  hold the Unicode versions of the names. The caller must free these.
    //
    c4Index = 0;
    while ((pHostInfo->h_aliases[c4Index] != 0) && (c4Index < c4MaxAliases))
    {
        ppszAliases[c4Index] = TRawStr::pszConvert(pHostInfo->h_aliases[c4Index]);
        c4Index++;
    }

    c4MaxAliases = c4Index;

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlTCPIP::bQueryHostNameByAddr(const  tCIDLib::TIPAddr    ipaToQuery
                                ,       tCIDLib::TCh* const pszToFill
                                , const tCIDLib::TCard4     c4MaxChars)
{
    // And get the host info for the indicated address
    struct hostent* pHostInfo = ::gethostbyaddr(reinterpret_cast<tCIDLib::TSCh*>(ipaToQuery),
                                                sizeof(ipaToQuery),
                                                AF_INET);

    if (!pHostInfo)
    {
        TKrnlError::SetLastKrnlError(c4XlatError(h_errno), h_errno);
        return kCIDLib::False;
    }

    TRawStr::pszConvert(pHostInfo->h_name, pszToFill, c4MaxChars);

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlTCPIP::bTextFromIPAddr(const   tCIDLib::TIPAddr    ipaToConvert
                            ,       tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars)
{
    // Try the conversion
    tCIDLib::TSCh* pszTmp = inet_ntoa(*(in_addr*)&ipaToConvert);

    // See if it failed and throw and exception if so
    if (!pszTmp)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcNet_InvalidAddr);
        return kCIDLib::False;
    }

    // Get into the caller's buffer
    TRawStr::pszConvert(pszTmp, pszToFill, c4MaxChars);

    return kCIDLib::True;
}


tCIDLib::TCard4 TKrnlTCPIP::c4XlatError(const tCIDLib::TSInt iToXlat)
{
    //
    //  Search the list for the passed error. Since it is a pretty short
    //  list we just do a linear search.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < __c4ErrCount; c4Index++)
    {
        if (__amapErrors[c4Index].iHostErr == iToXlat)
            return __amapErrors[c4Index].errcCIDErr;
    }

    // We did not find it, so return the generic 'TCPIP Error' indicator
    return kKrnlErrs::errcNet_NetError;
}


tCIDLib::TBoolean
TKrnlTCPIP::bLoadTCPIPMsg(const tCIDLib::TOSErrCode   errcId
                          ,       tCIDLib::TCh* const pszBuffer
                          , const tCIDLib::TCard4     c4MaxChars)
{
    //
    //  Just translate the TCP/IP message into one of our errors and load
    //  that message into the caller's buffer.
    //
    return kmodCIDKernel.bLoadCIDMsg
    (
        TKrnlTCPIP::c4XlatError(errcId)
        , pszBuffer
        , c4MaxChars
    );
}
