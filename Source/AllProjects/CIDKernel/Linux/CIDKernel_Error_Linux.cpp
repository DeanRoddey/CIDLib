//
// FILE NAME: CIDKernel_Error_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 10/20/1998
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
//  This file provides the Linux specific implementation for the TKrnlError
//  class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"

namespace
{
    // ---------------------------------------------------------------------------
    //  Local, const data
    //
    //  __aerrmList
    //      This is the list of error mapping structures that are used to map
    //      host errors into our errors.
    //
    //      !!These MUST be in ascending order for the first column, because a
    //      binary search is used to find the CIDKernel error that maps to the
    //      first column.
    // ---------------------------------------------------------------------------
    const tCIDKernel_::TErrMap __amapList[] =
    {
            { EPERM                         , kKrnlErrs::errcGen_NotAvailable }
        ,   { ENOENT                        , kKrnlErrs::errcFl_FileNotFound }
        ,   { ESRCH                         , kKrnlErrs::errcMod_ProcNotFound }
        ,   { EINTR                         , kKrnlErrs::errcGen_Interrupted }
        ,   { EIO                           , kKrnlErrs::errcGen_GenFailure }
        ,   { ENXIO                         , kKrnlErrs::errcDev_NoSuchDevice }
        ,   { E2BIG                         , kKrnlErrs::errcData_InvalidArguments }
        ,   { ENOEXEC                       , kKrnlErrs::errcData_InvalidFormat }
        ,   { EBADF                         , kKrnlErrs::errcData_InvalidHandle }
        ,   { ECHILD                        , kKrnlErrs::errcGen_NoData }
        ,   { EAGAIN                        , kKrnlErrs::errcGen_WouldBlock }
        ,   { ENOMEM                        , kKrnlErrs::errcMem_OutOfMemory }
        ,   { EACCES                        , kKrnlErrs::errcAcc_NoAccess }
        ,   { EFAULT                        , kKrnlErrs::errcMem_InvalidAddress }
        ,   { ENOTBLK                       , kKrnlErrs::errcGen_InvalidDeviceId }
        ,   { EBUSY                         , kKrnlErrs::errcGen_Busy }
        ,   { EEXIST                        , kKrnlErrs::errcGen_AlreadyExists }
        ,   { EXDEV                         , kKrnlErrs::errcDev_NotSameDevice }
        ,   { ENODEV                        , kKrnlErrs::errcDev_NoSuchDevice }
        ,   { ENOTDIR                       , kKrnlErrs::errcGen_GeneralFault }
        ,   { EISDIR                        , kKrnlErrs::errcGen_GeneralFault }
        ,   { EINVAL                        , kKrnlErrs::errcData_InvalidArguments }
        ,   { ENFILE                        , kKrnlErrs::errcData_BufferOverflow }
        ,   { EMFILE                        , kKrnlErrs::errcFl_TooManyOpenFiles }
        ,   { ENOTTY                        , kKrnlErrs::errcDev_InvalidUnit }
        ,   { ETXTBSY                       , kKrnlErrs::errcAcc_SharingViolation }
        ,   { EFBIG                         , kKrnlErrs::errcData_InsufficientBuffer }
        ,   { ENOSPC                        , kKrnlErrs::errcGen_Full }
        ,   { ESPIPE                        , kKrnlErrs::errcFl_Seek }
        ,   { EROFS                         , kKrnlErrs::errcGen_WriteFault }
        ,   { EMLINK                        , kKrnlErrs::errcGen_TooMany }
        ,   { EPIPE                         , kKrnlErrs::errcGen_Interrupted }
        ,   { EDOM                          , kKrnlErrs::errcData_InvalidValue }
        ,   { ERANGE                        , kKrnlErrs::errcData_InvalidData }
        ,   { EDEADLK                       , kKrnlErrs::errcSync_PossibleDeadlock }
        ,   { ENAMETOOLONG                  , kKrnlErrs::errcData_NameTooLong }
        ,   { ENOLCK                        , kKrnlErrs::errcAcc_LockViolation }
        ,   { ENOSYS                        , kKrnlErrs::errcGen_NotSupported }
        ,   { ENOTEMPTY                     , kKrnlErrs::errcFl_DirNotEmpty }
        ,   { ELOOP                         , kKrnlErrs::errcGen_TooMany }
        ,   { ENOMSG                        , kKrnlErrs::errcGen_NoData }
        ,   { EIDRM                         , kKrnlErrs::errcData_InvalidHandle }
        ,   { ECHRNG                        , kKrnlErrs::errcGen_IndexError }
        ,   { EL2NSYNC                      , kKrnlErrs::errcGen_GenFailure }
        ,   { EL3HLT                        , kKrnlErrs::errcGen_GeneralFault }
        ,   { EL3RST                        , kKrnlErrs::errcGen_GeneralFault }
        ,   { ELNRNG                        , kKrnlErrs::errcGen_IndexError }
        ,   { EUNATCH                       , kKrnlErrs::errcDev_NoDriverLoaded }
        ,   { ENOCSI                        , kKrnlErrs::errcData_NotAllData }
        ,   { EL2HLT                        , kKrnlErrs::errcGen_GeneralFault }
        ,   { EBADE                         , kKrnlErrs::errcGen_GenFailure }
        ,   { EBADR                         , kKrnlErrs::errcData_InvalidData }
        ,   { EXFULL                        , kKrnlErrs::errcGen_Full }
        ,   { ENOANO                        , kKrnlErrs::errcGen_GenFailure }
        ,   { EBADRQC                       , kKrnlErrs::errcData_InvalidData }
        ,   { EBADSLT                       , kKrnlErrs::errcGen_GenFailure }
        ,   { EBFONT                        , kKrnlErrs::errcData_InvalidFormat }
        ,   { ENOSTR                        , kKrnlErrs::errcDev_InvalidUnit }
        ,   { ENODATA                       , kKrnlErrs::errcGen_NoData }
        ,   { ETIME                         , kKrnlErrs::errcGen_Timeout }
        ,   { ENOSR                         , kKrnlErrs::errcGen_TooMany }
        ,   { ENONET                        , kKrnlErrs::errcNet_NoNetwork }
        ,   { ENOPKG                        , kKrnlErrs::errcGen_NotFound }
        ,   { EREMOTE                       , kKrnlErrs::errcNet_Remote }
        ,   { ENOLINK                       , kKrnlErrs::errcFl_FileNotFound }
        ,   { EADV                          , kKrnlErrs::errcGen_GenFailure }
        ,   { ESRMNT                        , kKrnlErrs::errcGen_GenFailure }
        ,   { ECOMM                         , kKrnlErrs::errcGen_WriteFault }
        ,   { EPROTO                        , kKrnlErrs::errcNet_ProtoType }
        ,   { EMULTIHOP                     , kKrnlErrs::errcGen_GenFailure }
        ,   { EDOTDOT                       , kKrnlErrs::errcGen_GenFailure }
        ,   { EBADMSG                       , kKrnlErrs::errcData_InvalidData }
        ,   { EOVERFLOW                     , kKrnlErrs::errcData_BufferOverflow }
        ,   { ENOTUNIQ                      , kKrnlErrs::errcNet_InvalidNetName }
        ,   { EBADFD                        , kKrnlErrs::errcData_InvalidHandle }
        ,   { EREMCHG                       , kKrnlErrs::errcNet_InvalidAddr }
        ,   { ELIBACC                       , kKrnlErrs::errcFl_FileNotFound }
        ,   { ELIBBAD                       , kKrnlErrs::errcData_InvalidFormat }
        ,   { ELIBSCN                       , kKrnlErrs::errcData_InvalidFormat }
        ,   { ELIBMAX                       , kKrnlErrs::errcGen_TooMany }
        ,   { ELIBEXEC                      , kKrnlErrs::errcAcc_InvalidAccess }
        ,   { EILSEQ                        , kKrnlErrs::errcData_InvalidFormat }
        ,   { ERESTART                      , kKrnlErrs::errcGen_Interrupted }
        ,   { ESTRPIPE                      , kKrnlErrs::errcGen_GenFailure }
        ,   { EUSERS                        , kKrnlErrs::errcNet_Users }
        ,   { ENOTSOCK                      , kKrnlErrs::errcNet_NotASocket }
        ,   { EDESTADDRREQ                  , kKrnlErrs::errcNet_DestAdrRequired }
        ,   { EMSGSIZE                      , kKrnlErrs::errcData_NotAllData }
        ,   { EPROTOTYPE                    , kKrnlErrs::errcNet_ProtoType }
        ,   { ENOPROTOOPT                   , kKrnlErrs::errcNet_NoProtoOpts }
        ,   { EPROTONOSUPPORT               , kKrnlErrs::errcNet_NoProtoOpts }
        ,   { ESOCKTNOSUPPORT               , kKrnlErrs::errcNet_NetError }
        ,   { EOPNOTSUPP                    , kKrnlErrs::errcNet_NetError }
        ,   { EPFNOSUPPORT                  , kKrnlErrs::errcNet_NetError }
        ,   { EAFNOSUPPORT                  , kKrnlErrs::errcNet_InvalidAddr }
        ,   { EADDRINUSE                    , kKrnlErrs::errcNet_InvalidAddr }
        ,   { EADDRNOTAVAIL                 , kKrnlErrs::errcNet_InvalidAddr }
        ,   { ENETDOWN                      , kKrnlErrs::errcNet_NetworkDown }
        ,   { ENETUNREACH                   , kKrnlErrs::errcNet_NetworkUnreachable }
        ,   { ENETRESET                     , kKrnlErrs::errcNet_NetworkReset }
        ,   { ECONNABORTED                  , kKrnlErrs::errcNet_ConnAborted }
        ,   { ECONNRESET                    , kKrnlErrs::errcNet_ConnReset }
        ,   { ENOBUFS                       , kKrnlErrs::errcData_InsufficientBuffer }
        ,   { EISCONN                       , kKrnlErrs::errcGen_AlreadyConnected }
        ,   { ENOTCONN                      , kKrnlErrs::errcGen_NotConnected }
        ,   { ESHUTDOWN                     , kKrnlErrs::errcNet_Shutdown }
        ,   { ETOOMANYREFS                  , kKrnlErrs::errcGen_TooMany }
        ,   { ETIMEDOUT                     , kKrnlErrs::errcGen_Timeout }
        ,   { ECONNREFUSED                  , kKrnlErrs::errcNet_ConnRefused }
        ,   { EHOSTDOWN                     , kKrnlErrs::errcNet_HostDown }
        ,   { EHOSTUNREACH                  , kKrnlErrs::errcNet_HostUnreachable }
        ,   { EALREADY                      , kKrnlErrs::errcGen_Already }
        ,   { EINPROGRESS                   , kKrnlErrs::errcGen_InProgress }
        ,   { ESTALE                        , kKrnlErrs::errcNet_Stale }
        ,   { EUCLEAN                       , kKrnlErrs::errcGen_GenFailure }
        ,   { ENOTNAM                       , kKrnlErrs::errcData_InvalidName }
        ,   { ENAVAIL                       , kKrnlErrs::errcGen_NotAvailable }
        ,   { EISNAM                        , kKrnlErrs::errcGen_GenFailure }
        ,   { EREMOTEIO                     , kKrnlErrs::errcNet_Remote }
        ,   { EDQUOT                        , kKrnlErrs::errcGen_GenFailure }
    };
    const tCIDLib::TCard4 __c4ErrCount = tCIDLib::c4ArrayElems(__amapList);


    // ---------------------------------------------------------------------------
    //  Local data
    //
    //  __keyPTDSlotId
    //      The per thread slot id for storing the last error on each thread.
    // ---------------------------------------------------------------------------
    pthread_key_t    __keyPTDSlotId;



    // ---------------------------------------------------------------------------
    //  Local methods
    // ---------------------------------------------------------------------------
    tCIDLib::TVoid __destroyThreadData(tCIDLib::TVoid* pData)
    {
        TKrnlError* kerr = static_cast<TKrnlError*>(pData);
        delete kerr;
    }

    tCIDLib::TErrCode __errcMapError(const tCIDLib::TErrCode errcToMap)
    {
        // Set up the two end points that are used to subdivide the list
        tCIDLib::TCard4 c4End = __c4ErrCount - 1;
        tCIDLib::TCard4 c4Begin = 0;

        while (c4Begin <= c4End)
        {
            // Divide the current range
            tCIDLib::TCard4 c4MidPoint = (c4Begin + c4End) / 2;

            // Check this guy. If this is it, then return the mapped error
            if (__amapList[c4MidPoint].errcHostErr == errcToMap)
                return __amapList[c4MidPoint].errcCIDErr;

            // Didn't find it, so see which way to go and adjust begin/end
            if (errcToMap < __amapList[c4MidPoint].errcHostErr)
                c4End = c4MidPoint - 1;
            else
                c4Begin = c4MidPoint + 1;
        }

        // Just return generic error
        return kKrnlErrs::errcHostErr;
    }

    TKrnlError& kerrCaller()
    {
        // Get the slot value for this thread
        TKrnlError* pkerrRet = static_cast<TKrnlError*>(::pthread_getspecific(__keyPTDSlotId));

        // If there is no object there, then allocate one and store it
        if (!pkerrRet)
        {
            pkerrRet = new TKrnlError;
            tCIDLib::TSInt iRc = ::pthread_setspecific(__keyPTDSlotId, pkerrRet);
            if (iRc)
            {
                kmodCIDKernel.ErrorPopUp
                (
                    iRc
                    , CID_FILE
                    , CID_LINE
                    , kmodCIDKernel.pszLoadCIDFacMsg(kKrnlErrs::errcErr_SetSlot)
                );
            }
        }
        return *pkerrRet;
    }
}

// ---------------------------------------------------------------------------
//  TCIDKrnlModule: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TCIDKrnlModule::__InitTermError(const tCIDLib::EInitTerm eInitTerm)
{
    if (eInitTerm == tCIDLib::EInitTerm::Initialize)
    {
        //
        //  Allocate our per-thread slot id. This is where we store last err
        //  info for each thread.
        //
        ::pthread_key_create(&__keyPTDSlotId, __destroyThreadData);

        //
        //  If we debugging, then make sure that the error id array above
        //  really is in sorted order. If not, then we are in trouble because
        //  we use a binary search to search it.
        //
        #if CID_DEBUG_ON
        for (tCIDLib::TCard4 c4Index = 0; c4Index < (__c4ErrCount-1); c4Index++)
        {
            if (__amapList[c4Index].errcHostErr
                                        > __amapList[c4Index+1].errcHostErr)
            {
                TKrnlError::ThrowKrnlError(kKrnlErrs::errcErr_OutOfOrder);
            }
        }
        #endif
    }
    else if (eInitTerm == tCIDLib::EInitTerm::Terminate)
    {
        ::pthread_key_delete(__keyPTDSlotId);
    }
}




// ---------------------------------------------------------------------------
//  TKrnlError: Public, static methods
// ---------------------------------------------------------------------------
const TKrnlError& TKrnlError::kerrLast()
{
    return kerrCaller();
}


tCIDLib::TVoid
TKrnlError::ThrowHostError(const tCIDLib::TOSErrCode errcHostId)
{
    throw TKrnlError(__errcMapError(errcHostId), errcHostId);
}


tCIDLib::TVoid TKrnlError::SetLastError(const TKrnlError& kerrToSet)
{
    kerrCaller() = kerrToSet;
}


tCIDLib::TVoid
TKrnlError::SetLastHostError(const tCIDLib::TOSErrCode errcHostId)
{
    kerrCaller().Set(__errcMapError(errcHostId), errcHostId);
}


tCIDLib::TVoid
TKrnlError::SetLastKrnlError(const  tCIDLib::TErrCode   errcKrnlId
                            , const tCIDLib::TOSErrCode errcHostId)
{
    kerrCaller().Set(errcKrnlId, errcHostId);
}
