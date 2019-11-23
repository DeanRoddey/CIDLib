//
// FILE NAME: CIDKernel_TCPIP_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 01/01/1999
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


namespace CIDKernel_IP_Linux
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
    //  CIDKernel_IP_Linux::c4ErrCount
    //      The number of elements in the __aXlatErrors array.
    // ---------------------------------------------------------------------------
    const TErrorMap amapErrors[] =
    {
            { NETDB_SUCCESS     , kKrnlErrs::errcNoError }
        ,   { HOST_NOT_FOUND    , kKrnlErrs::errcNet_HostDown }
        ,   { TRY_AGAIN         , kKrnlErrs::errcNet_HostUnreachable }
        ,   { NO_RECOVERY       , kKrnlErrs::errcGen_TermError }
        ,   { NO_DATA           , kKrnlErrs::errcGen_NoData }
    };
    const tCIDLib::TCard4 c4ErrCount = tCIDLib::c4ArrayElems(CIDKernel_IP_Linux::amapErrors);



    // ---------------------------------------------------------------------------
    //  Local functions
    // ---------------------------------------------------------------------------
    static tCIDLib::TBoolean
    bQueryHostIPAddrs(  const   tCIDLib::TSCh* const    pszHostName
                        ,       tCIDLib::TIPAddr* const aipaToFill
                        , const tCIDLib::TCard4         c4MaxAddrs
                        ,       tCIDLib::TCard4&        c4AddrsFound)
    {
        // Get the host info for the passed worksation name
        struct hostent* pHostInfo = ::gethostbyname(pszHostName);

        if (!pHostInfo)
        {
            TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(h_errno), h_errno);
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

    //
    //  This is the version of the host TCP/IP support that we are using.
    //  It is saved here during init and used by the TCP/IP version support
    //  method of TKrnlSysInfo that we implement here.    
    //
    tCIDLib::TCard2  c2Version = 0;    
}



// ---------------------------------------------------------------------------
//  TCIDKrnlModule: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TCIDKrnlModule::bInitTermIP(const tCIDLib::EInitTerm)
{
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TKrnlTCPIP: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TKrnlIP::bAddToFirewall(const   tCIDLib::TCh* const     pszAppPath
                        , const tCIDLib::TCh* const     pszAppName
                        ,       tCIDLib::TCh* const     pszErrText
                        , const tCIDLib::TCard4         c4MaxErrChars)
{

    // Does this have any meaning on Linux?
    return kCIDLib::False;
}                        



// <TBD> These need to be implemented
tCIDLib::TBoolean TKrnlIP::bIPV4Avail()
{
    return kCIDLib::False;
}

tCIDLib::TBoolean TKrnlIP::bIPV6Avail()
{
    return kCIDLib::False;
}


tCIDLib::TBoolean
TKrnlIP::bLoadTCPIPMsg( const   tCIDLib::TOSErrCode errcId
                        ,       tCIDLib::TCh* const pszBuffer
                        , const tCIDLib::TCard4     c4MaxChars)
{
    //
    //  Just translate the TCP/IP message into one of our errors and load
    //  that message into the caller's buffer.
    //
    return kmodCIDKernel.bLoadCIDFacMsg(TKrnlIP::c4XlatError(errcId), pszBuffer, c4MaxChars);
}


// <TBD> This needs to be implemented
tCIDLib::TBoolean
TKrnlIP::bQueryAdaptorInfo( const   tCIDLib::TCh* const     pszName
                            ,       TAdaptorInfo&           kadpToFill
                            ,       tCIDLib::TBoolean&      bFound)
{
    return kCIDLib::False;
}                            


// <TBD> This needs to be implemented
tCIDLib::TBoolean TKrnlIP::bQueryAdaptorList(TKrnlLList<TAdaptorInfo>& kllstToFill)
{
    return kCIDLib::False;
}


// <TBD> This needs to be implemented
tCIDLib::TBoolean
TKrnlIP::bQueryDefLocalAddr(TKrnlIPAddr& kipaToFill, const tCIDSock::EAddrTypes eType)
{
    return kCIDLib::False;
}



tCIDLib::TBoolean
TKrnlIP::bQueryHostAddrs(   const   tCIDLib::TCh* const         pszHostName
                            ,       TKrnlLList<TKrnlIPAddr>&    kllstAddrs
                            , const tCIDSock::EAddrInfoFlags    eFlags
                            , const tCIDLib::TBoolean           bIncludeLoopback
                            , const tCIDLib::TBoolean           bAppend)
{
    struct utsname UtsName;
    if (::uname(&UtsName))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    // <TBD> This has been completed changed and has to be redone
    /*
    tCIDLib::TSCh* pszHostCopy = TRawStr::pszConvert(pszHostName);
    TArrayJanitor<tCIDLib::TSCh> janHost(pszHostCopy);

    return CIDKernel_IP_Linux::bQueryHostIPAddrs(pszHostCopy
                               , aipaToFill
                               , c4MaxAddrs
                               , c4AddrsFound);
    */

    return kCIDLib::False;
}


tCIDLib::TBoolean
TKrnlIP::bQueryLocalName(TKrnlString& kstrToFill)
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
        TKrnlError::SetLastKrnlError(TKrnlIP::c4XlatError(h_errno), h_errno);
        return kCIDLib::False;
    }

    kstrToFill = szTmpName;
    return kCIDLib::True;
}


// <TBD> This needs to be fixed up
tCIDLib::TBoolean
TKrnlIP::bQueryNameByAddr(const TKrnlIPAddr& kipaToQuery, TKrnlString& kstrToFill)
{
    /*
    // And get the host info for the indicated address
    struct hostent* pHostInfo = ::gethostbyaddr(reinterpret_cast<tCIDLib::TSCh*>(kipaToQuery.),
                                                sizeof(ipaToQuery),
                                                AF_INET);

    if (!pHostInfo)
    {
        TKrnlError::SetLastKrnlError(c4XlatError(h_errno), h_errno);
        return kCIDLib::False;
    }

    kstrToFill = pHostInfo->h_name;
    return kCIDLib::True;
    */
   return kCIDLib::False;
}


tCIDLib::TBoolean
TKrnlIP::bQueryTCPVersion(     tCIDLib::TCard4&    c4MajVersion
                                , tCIDLib::TCard4&  c4MinVersion)
{
    // Major version is in the low byte, minor in the high
    c4MinVersion = tCIDLib::TCard4(CIDKernel_IP_Linux::c2Version >> 8);
    c4MajVersion = tCIDLib::TCard4(CIDKernel_IP_Linux::c2Version & 0xFF);

    // In our implmentation, we have this cached away so it can't fail
    return kCIDLib::True;
}


// <TBD> This needs to be reworked
tCIDLib::TBoolean
TKrnlIP::bTextFromIPAddr(const TKrnlIPAddr& kipaCvt, TKrnlString& kstrToFill)
{
    /*
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
    */

   return kCIDLib::False;
}


tCIDLib::TCard4 TKrnlIP::c4XlatError(const tCIDLib::TSInt iToXlat)
{
    //
    //  Search the list for the passed error. Since it is a pretty short
    //  list we just do a linear search.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < CIDKernel_IP_Linux::c4ErrCount; c4Index++)
    {
        if (CIDKernel_IP_Linux::amapErrors[c4Index].iHostErr == iToXlat)
            return CIDKernel_IP_Linux::amapErrors[c4Index].errcCIDErr;
    }

    // We did not find it, so return the generic 'TCPIP Error' indicator
    return kKrnlErrs::errcNet_NetError;
}



tCIDSock::EAddrCvtRes
TKrnlIP::eIPAFromText(  const   tCIDLib::TCh* const     pszIPAddrString
                        ,       TKrnlIPAddr&            kipaToFill
                        , const tCIDLib::TBoolean       bDoLookup
                        , const tCIDSock::EAddrTypes    eReqAddr)
{
    // <TBD> This has changed completely since the original
    /*
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
    */

    return tCIDSock::EAddrCvtRes::Failed;
}


// <TBD> This needs to be implemented
tCIDLib::TVoid TKrnlIP::QueryDefAllListen(TKrnlIPAddr& kipaToFill)
{
}
