//
// FILE NAME: CIDKernel_Error_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/12/1998
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
//  This file provides the Win32 specific implementation for the TKrnlError
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

#pragma     warning(push)
#include    <CodeAnalysis\Warnings.h>
#pragma     warning(disable : ALL_CODE_ANALYSIS_WARNINGS 26812)
#include    <comdef.h>
#pragma     warning(pop)


namespace
{
    namespace CIDKernel_Error_Win32
    {
        // -----------------------------------------------------------------------
        //  Local, const data
        //
        //  aerrmList
        //      This is the list of error mapping structures that are used to map
        //      host errors into our errors.
        //
        //      !!These MUST be in ascending order for the first column, because a
        //      binary search is used to find the CIDKernel error that maps to the
        //      first column.
        // -----------------------------------------------------------------------
        const tCIDKernel_::TErrMap amapList[] =
        {
                { NO_ERROR                      , kKrnlErrs::errcNoError }
            ,   { ERROR_INVALID_FUNCTION        , kKrnlErrs::errcGen_NotSupported }         // 1
            ,   { ERROR_FILE_NOT_FOUND          , kKrnlErrs::errcFl_FileNotFound }
            ,   { ERROR_PATH_NOT_FOUND          , kKrnlErrs::errcFl_PathNotFound }
            ,   { ERROR_TOO_MANY_OPEN_FILES     , kKrnlErrs::errcFl_TooManyOpenFiles }
            ,   { ERROR_ACCESS_DENIED           , kKrnlErrs::errcAcc_AccessDenied }
            ,   { ERROR_INVALID_HANDLE          , kKrnlErrs::errcData_InvalidHandle }
            ,   { ERROR_NOT_ENOUGH_MEMORY       , kKrnlErrs::errcMem_NotEnoughMemory }
            ,   { ERROR_BAD_ENVIRONMENT         , kKrnlErrs::errcEnv_InvalidEnvironment }
            ,   { ERROR_BAD_FORMAT              , kKrnlErrs::errcData_InvalidFormat }
            ,   { ERROR_INVALID_ACCESS          , kKrnlErrs::errcAcc_InvalidAccess }
            ,   { ERROR_INVALID_DATA            , kKrnlErrs::errcData_InvalidData }
            ,   { ERROR_OUTOFMEMORY             , kKrnlErrs::errcMem_OutOfMemory }
            ,   { ERROR_INVALID_DRIVE           , kKrnlErrs::errcFl_InvalidDrive }
            ,   { ERROR_CURRENT_DIRECTORY       , kKrnlErrs::errcFl_CurrentDirectory }
            ,   { ERROR_NOT_SAME_DEVICE         , kKrnlErrs::errcDev_NotSameDevice }
            ,   { ERROR_NO_MORE_FILES           , kKrnlErrs::errcFl_NoMoreFiles }
            ,   { ERROR_WRITE_PROTECT           , kKrnlErrs::errcAcc_WriteProtect }
            ,   { ERROR_NOT_READY               , kKrnlErrs::errcGen_NotReady }             // 21
            ,   { ERROR_BAD_COMMAND             , kKrnlErrs::errcData_InvalidCommand }
            ,   { ERROR_CRC                     , kKrnlErrs::errcGen_CRC }
            ,   { ERROR_SEEK                    , kKrnlErrs::errcDev_SeekOnDevice }
            ,   { ERROR_NOT_DOS_DISK            , kKrnlErrs::errcFl_InvalidDrive }
            ,   { ERROR_OUT_OF_PAPER            , kKrnlErrs::errcPrn_OutOfPaper }
            ,   { ERROR_WRITE_FAULT             , kKrnlErrs::errcGen_WriteFault }
            ,   { ERROR_READ_FAULT              , kKrnlErrs::errcGen_ReadFault }
            ,   { ERROR_GEN_FAILURE             , kKrnlErrs::errcGen_GenFailure }
            ,   { ERROR_SHARING_VIOLATION       , kKrnlErrs::errcAcc_SharingViolation }
            ,   { ERROR_WRONG_DISK              , kKrnlErrs::errcFl_WrongDisk }
            ,   { ERROR_DUP_NAME                , kKrnlErrs::errcData_DuplicateName }
            ,   { ERROR_BAD_NETPATH             , kKrnlErrs::errcNet_InvalidNetPath }
            ,   { ERROR_NETWORK_BUSY            , kKrnlErrs::errcNet_NetworkBusy }          // 54
            ,   { ERROR_DEV_NOT_EXIST           , kKrnlErrs::errcDev_NoSuchDevice }
            ,   { ERROR_PRINTQ_FULL             , kKrnlErrs::errcGen_Full }
            ,   { ERROR_NO_SPOOL_SPACE          , kKrnlErrs::errcPrn_NoSpoolSpace }
            ,   { ERROR_PRINT_CANCELLED         , kKrnlErrs::errcPrn_JobCancelled }
            ,   { ERROR_BAD_NET_NAME            , kKrnlErrs::errcFl_PathNotFound }
            ,   { ERROR_FILE_EXISTS             , kKrnlErrs::errcGen_AlreadyExists }        // 80
            ,   { ERROR_INVALID_PASSWORD        , kKrnlErrs::errcSec_InvalidPassword }
            ,   { ERROR_INVALID_PARAMETER       , kKrnlErrs::errcData_InvalidParameter }
            ,   { ERROR_EXCL_SEM_ALREADY_OWNED  , kKrnlErrs::errcGen_AlreadyOwned }
            ,   { ERROR_SEM_IS_SET              , kKrnlErrs::errcGen_AlreadySet }           // 102
            ,   { ERROR_TOO_MANY_SEM_REQUESTS   , kKrnlErrs::errcGen_TooMany }
            ,   { ERROR_SEM_OWNER_DIED          , kKrnlErrs::errcSem_OwnerDied }
            ,   { ERROR_DRIVE_LOCKED            , kKrnlErrs::errcGen_Locked }
            ,   { ERROR_OPEN_FAILED             , kKrnlErrs::errcGen_OpenFailed }
            ,   { ERROR_BUFFER_OVERFLOW         , kKrnlErrs::errcData_BufferOverflow }
            ,   { ERROR_DISK_FULL               , kKrnlErrs::errcGen_Full }                 // 112
            ,   { ERROR_NO_MORE_SEARCH_HANDLES  , kKrnlErrs::errcFl_NoMoreSearchHandles }
            ,   { ERROR_SEM_TIMEOUT             , kKrnlErrs::errcGen_Timeout }
            ,   { ERROR_INSUFFICIENT_BUFFER     , kKrnlErrs::errcData_InsufficientBuffer }
            ,   { ERROR_INVALID_NAME            , kKrnlErrs::errcData_InvalidName }
            ,   { ERROR_MOD_NOT_FOUND           , kKrnlErrs::errcMod_ModuleNotFound }
            ,   { ERROR_PROC_NOT_FOUND          , kKrnlErrs::errcMod_ProcNotFound }
            ,   { ERROR_NEGATIVE_SEEK           , kKrnlErrs::errcDev_NegativeSeek }
            ,   { ERROR_SEEK_ON_DEVICE          , kKrnlErrs::errcDev_SeekOnDevice }
            ,   { ERROR_DIR_NOT_ROOT            , kKrnlErrs::errcFl_NotRootDirectory }
            ,   { ERROR_DIR_NOT_EMPTY           , kKrnlErrs::errcFl_DirNotEmpty }
            ,   { ERROR_BAD_ARGUMENTS           , kKrnlErrs::errcData_InvalidArguments }
            ,   { ERROR_BAD_PATHNAME            , kKrnlErrs::errcFl_InvalidPathName }
            ,   { ERROR_MAX_THRDS_REACHED       , kKrnlErrs::errcProc_MaxThreadsReached }
            ,   { ERROR_BUSY                    , kKrnlErrs::errcGen_Busy }
            ,   { ERROR_ALREADY_EXISTS          , kKrnlErrs::errcGen_AlreadyExists }        // 183
            ,   { ERROR_SEM_NOT_FOUND           , kKrnlErrs::errcGen_NotFound }
            ,   { ERROR_ENVVAR_NOT_FOUND        , kKrnlErrs::errcGen_NotFound }
            ,   { ERROR_FILENAME_EXCED_RANGE    , kKrnlErrs::errcFl_InvalidPathName }
            ,   { ERROR_NO_DATA                 , kKrnlErrs::errcGen_NoData }
            ,   { ERROR_MORE_DATA               , kKrnlErrs::errcGen_MoreData }             // 234
            ,   { ERROR_DIRECTORY               , kKrnlErrs::errcFl_InvalidPathName }
            ,   { ERROR_NOT_OWNER               , kKrnlErrs::errcGen_NotOwner }
            ,   { ERROR_TOO_MANY_POSTS          , kKrnlErrs::errcGen_TooMany }
            ,   { ERROR_INVALID_ADDRESS         , kKrnlErrs::errcMem_InvalidAddress }       // 487
            ,   { ERROR_STACK_OVERFLOW          , kKrnlErrs::errcGen_StackOverflow }
            ,   { ERROR_UNRECOGNIZED_VOLUME     , kKrnlErrs::errcFl_UnsupportedFileSystem } // 1005
            ,   { ERROR_CANTOPEN                , kKrnlErrs::errcReg_CantOpen }             // 1011
            ,   { ERROR_CANTREAD                , kKrnlErrs::errcReg_CantRead }             // 1012
            ,   { ERROR_CANTWRITE               , kKrnlErrs::errcReg_CantWrite }            // 1013
            ,   { ERROR_REGISTRY_RECOVERED      , kKrnlErrs::errcReg_Recovered }            // 1014
            ,   { ERROR_REGISTRY_CORRUPT        , kKrnlErrs::errcReg_Corrupt }              // 1015
            ,   { ERROR_KEY_DELETED             , kKrnlErrs::errcReg_KeyDeleted }           // 1018
            ,   { ERROR_KEY_HAS_CHILDREN        , kKrnlErrs::errcReg_KeyHasChildren }
            ,   { ERROR_SERVICE_ALREADY_RUNNING , kKrnlErrs::errcSvc_AlreadyRunning }       // 1056
            ,   { ERROR_SERVICE_DISABLED        , kKrnlErrs::errcSvc_Disabled }             // 1058
            ,   { ERROR_SERVICE_DOES_NOT_EXIST  , kKrnlErrs::errcSvc_NotFound }             // 1060
            ,   { ERROR_SERVICE_NOT_ACTIVE      , kKrnlErrs::errcSvc_NotRunning }           // 1062
            ,   { ERROR_SERVICE_SPECIFIC_ERROR  , kKrnlErrs::errcSvc_ServiceSpecific }
            ,   { ERROR_SERVICE_MARKED_FOR_DELETE,kKrnlErrs::errcSvc_ScheduledForDelete }   // 1072
            ,   { ERROR_SERVICE_EXISTS          , kKrnlErrs::errcSvc_AlreadyExists }
            ,   { ERROR_DLL_INIT_FAILED         , kKrnlErrs::errcMod_DLLInitFailed }
            ,   { ERROR_SHUTDOWN_IN_PROGRESS    , kKrnlErrs::errcGen_ShutdownInProgress }   // 1115
            ,   { ERROR_COUNTER_TIMEOUT         , kKrnlErrs::errcGen_Timeout }
            ,   { ERROR_POSSIBLE_DEADLOCK       , kKrnlErrs::errcSync_PossibleDeadlock }
            ,   { ERROR_APP_WRONG_OS            , kKrnlErrs::errcGen_WrongOS }              // 1151
            ,   { ERROR_SINGLE_INSTANCE_APP     , kKrnlErrs::errcGen_TooMany }
            ,   { ERROR_INVALID_GROUPNAME       , kKrnlErrs::errcNet_InvalidGroupName }
            ,   { ERROR_INVALID_COMPUTERNAME    , kKrnlErrs::errcNet_InvalidComputerName }  // 1210
            ,   { ERROR_INVALID_DOMAINNAME      , kKrnlErrs::errcNet_InvalidDomainName }
            ,   { ERROR_INVALID_NETNAME         , kKrnlErrs::errcNet_InvalidNetName }
            ,   { ERROR_INVALID_SHARENAME       , kKrnlErrs::errcNet_InvalidShareName }
            ,   { ERROR_INVALID_PASSWORDNAME    , kKrnlErrs::errcSec_InvalidPasswordName }
            ,   { ERROR_NO_NETWORK              , kKrnlErrs::errcNet_NoNetwork }            // 1222
            ,   { ERROR_NO_SUCH_PRIVILEGE       , kKrnlErrs::errcSec_NoSuchPrivilege }
            ,   { ERROR_PRIVILEGE_NOT_HELD      , kKrnlErrs::errcSec_PrivilegeNotHeld }
            ,   { ERROR_INVALID_ACCOUNT_NAME    , kKrnlErrs::errcSec_InvalidAccountName }
            ,   { ERROR_USER_EXISTS             , kKrnlErrs::errcGen_AlreadyExists }        // 1316
            ,   { ERROR_NO_SUCH_USER            , kKrnlErrs::errcGen_NotFound }
            ,   { ERROR_GROUP_EXISTS            , kKrnlErrs::errcGen_AlreadyExists }
            ,   { ERROR_NO_SUCH_GROUP           , kKrnlErrs::errcGen_NotFound }             // 1319
            ,   { ERROR_MEMBER_IN_GROUP         , kKrnlErrs::errcSec_MemberOfGroup }
            ,   { ERROR_MEMBER_NOT_IN_GROUP     , kKrnlErrs::errcSec_NotMemberOfGroup }
            ,   { ERROR_WRONG_PASSWORD          , kKrnlErrs::errcSec_InvalidPassword }      // 1323
            ,   { ERROR_LOGON_FAILURE           , kKrnlErrs::errcSec_LogonFailed }          // 1326
            ,   { ERROR_INVALID_WINDOW_HANDLE   , kKrnlErrs::errcGen_InvalidHandle }        // 1400
            ,   { ERROR_INVALID_MENU_HANDLE     , kKrnlErrs::errcGen_InvalidHandle }        // 1401
            ,   { ERROR_INVALID_CURSOR_HANDLE   , kKrnlErrs::errcGen_InvalidHandle }        // 1402
            ,   { ERROR_INVALID_ACCEL_HANDLE    , kKrnlErrs::errcGen_InvalidHandle }        // 1403
            ,   { ERROR_INVALID_HOOK_HANDLE     , kKrnlErrs::errcGen_InvalidHandle }        // 1404
            ,   { ERROR_INVALID_DWP_HANDLE      , kKrnlErrs::errcGen_InvalidHandle }        // 1405
            ,   { ERROR_CANNOT_FIND_WND_CLASS   , kKrnlErrs::errcWnd_NoWndClass }           // 1407
            ,   { ERROR_INVALID_ICON_HANDLE     , kKrnlErrs::errcGen_InvalidHandle }        // 1414
            ,   { ERROR_NOT_CHILD_WINDOW        , kKrnlErrs::errcWnd_NotAChild }            // 1442
            ,   { ERROR_NO_SYSTEM_RESOURCES     , kKrnlErrs::errcGen_NotEnoughResources }   // 1450
            ,   { ERROR_TIMEOUT                 , kKrnlErrs::errcGen_Timeout }              // 1460
            ,   { ERROR_BAD_USERNAME            , kKrnlErrs::errcSec_InvalidUserName }      // 2202
        };
        static const tCIDLib::TCard4 c4ErrCount = tCIDLib::c4ArrayElems(amapList);


        // -----------------------------------------------------------------------
        //  Local data
        //
        //  c4PTDSlotId
        //      The per thread slot id for storing the last error on each thread.
        // -----------------------------------------------------------------------
        tCIDLib::TCard4     c4PTDSlotId;
    }
}



// ---------------------------------------------------------------------------
//  Local methods
// ---------------------------------------------------------------------------
static tCIDLib::TErrCode errcMapError(const tCIDLib::TErrCode errcToMap)
{
    // Set up the two end points that are used to subdivide the list
    tCIDLib::TCard4 c4End = CIDKernel_Error_Win32::c4ErrCount - 1;
    tCIDLib::TCard4 c4Begin = 0;

    while (c4Begin <= c4End)
    {
        // Divide the current range
        tCIDLib::TCard4 c4MidPoint = (c4Begin + c4End) / 2;

        // Check this guy. If this is it, then return the mapped error
        if (CIDKernel_Error_Win32::amapList[c4MidPoint].errcHostErr == errcToMap)
            return CIDKernel_Error_Win32::amapList[c4MidPoint].errcCIDErr;

        // Didn't find it, so see which way to go and adjust begin/end
        if (errcToMap < CIDKernel_Error_Win32::amapList[c4MidPoint].errcHostErr)
            c4End = c4MidPoint - 1;
        else
            c4Begin = c4MidPoint + 1;
    }

    // Just return generic error
    return kKrnlErrs::errcHostErr;
}

static TKrnlError& kerrCaller()
{
    // Get the slot value for this thread
    TKrnlError* pkerrRet = static_cast<TKrnlError*>
    (
        ::TlsGetValue(CIDKernel_Error_Win32::c4PTDSlotId)
    );

    // If there is no object there, then allocate one and store it
    if (!pkerrRet)
    {
        pkerrRet = new TKrnlError;
        if (!::TlsSetValue(CIDKernel_Error_Win32::c4PTDSlotId, pkerrRet))
        {
            kmodCIDKernel.ErrorPopUp
            (
                ::GetLastError()
                , CID_FILE
                , CID_LINE
                , kmodCIDKernel.pszLoadCIDFacMsg(kKrnlErrs::errcErr_SetSlot)
            );
        }
    }
    return *pkerrRet;
}


// ---------------------------------------------------------------------------
//  TCIDKrnlModule: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCIDKrnlModule::InitTermError(const tCIDLib::EInitTerm eState)
{
    if (eState == tCIDLib::EInitTerm::Initialize)
    {
        //
        //  Allocate our per-thread slot id. This is where we store last err
        //  info for each thread.
        //
        CIDKernel_Error_Win32::c4PTDSlotId = ::TlsAlloc();

        //
        //  If we debugging, then make sure that the error id array above
        //  really is in sorted order. If not, then we are in trouble because
        //  we use a binary search to search it.
        //
        #if CID_DEBUG_ON
        for (tCIDLib::TCard4 c4Index = 0;
                    c4Index < (CIDKernel_Error_Win32::c4ErrCount - 1); c4Index++)
        {
            if (CIDKernel_Error_Win32::amapList[c4Index].errcHostErr
                        > CIDKernel_Error_Win32::amapList[c4Index + 1].errcHostErr)
            {
                TKrnlError::ThrowKrnlError(kKrnlErrs::errcErr_OutOfOrder);
            }
        }
        #endif
    }
}




// ---------------------------------------------------------------------------
//  TKrnlError: Public, static methods
// ---------------------------------------------------------------------------

// Access the calling thread's last error object
const TKrnlError& TKrnlError::kerrLast()
{
    return kerrCaller();
}


// Load the message text for the indicated OS error code
tCIDLib::TBoolean
TKrnlError::bQuerySysErrMsg(const   tCIDLib::TOSErrCode     errcGet
                            ,       tCIDLib::TCh* const     pszToFill
                            , const tCIDLib::TCard4         c4MaxChars)
{
    // Shouldn't get called with zero, but just in case
    if (!errcGet)
    {
        TRawStr::CopyStr(pszToFill, L"No error occurred", c4MaxChars);
        return kCIDLib::True;
    }

    // If it's an HRESULT type error, we use  different mechanism.
    if (errcGet & 0x8000000)
    {
        _com_error error(errcGet);

        TRawStr::CopyStr(pszToFill, error.ErrorMessage(), c4MaxChars);
        return kCIDLib::True;
    }

    //
    //  If the passed error id is one of the TCP/IP error message, then we cannot
    //  get text for them. So we have to provide a message ourself.
    //
    if (errcGet > WSABASEERR)
        return TKrnlIP::bLoadTCPIPMsg(errcGet, pszToFill, c4MaxChars);

    // Looks reasonable so try to load it
    LPWSTR pmsgBuf = 0;
    size_t size = ::FormatMessage
    (
        FORMAT_MESSAGE_ALLOCATE_BUFFER
        | FORMAT_MESSAGE_FROM_SYSTEM
        | FORMAT_MESSAGE_IGNORE_INSERTS
        , NULL
        , errcGet
        , MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
        , (LPWSTR)&pmsgBuf
        , 0
        , NULL
    );

    if (!size)
        return kCIDLib::False;

    //
    //  Get rid of any CR/LF/tab stuff. This will replace any sequence of these chars
    //  with a space (and get rid of all of them on the leading/trailing ends.)
    //
    TRawStr::StripStr(pmsgBuf, L"\r\n\t", tCIDLib::EStripModes::Full);

    // Store the text and free the buffer
    TRawStr::CopyStr(pszToFill, pmsgBuf, c4MaxChars);
    ::LocalFree(pmsgBuf);

    return kCIDLib::True;
}


//
//  Throw an exception on the host error. This is sometimes used as an internal mechanism
//  to catch an error.
//
tCIDLib::TVoid
TKrnlError::ThrowHostError(const tCIDLib::TOSErrCode errcHostId)
{
    throw TKrnlError(errcMapError(errcHostId), errcHostId);
}


// Store into the calling thread's last error object
tCIDLib::TVoid TKrnlError::SetLastError(const TKrnlError& kerrToSet)
{
    kerrCaller() = kerrToSet;
}


// Set the last host error on the calling thread's last error object
tCIDLib::TVoid
TKrnlError::SetLastHostError(const tCIDLib::TOSErrCode errcHostId)
{
    kerrCaller().Set(errcMapError(errcHostId), errcHostId);
}


tCIDLib::TVoid
TKrnlError::SetLastKrnlError(const  tCIDLib::TErrCode   errcKrnlId
                            , const tCIDLib::TOSErrCode errcHostId)
{
    kerrCaller().Set(errcKrnlId, errcHostId);
}

