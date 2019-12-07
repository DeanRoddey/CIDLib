//
// FILE NAME: CIDKernel_SystemInfo_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/06/1996
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
//  This file implements the TKrnlSysInfo class for the Win32 platform. This
//  is a simple namespace that encapsulates the retrieval of general system
//  information.  It will precache info that cannot change for the run of
//  the host process, for faster return. Otherwise, it just gets when asked.
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
#include    "CIDKernel_InternalHelpers_.hpp"
#include    "CIDKernel_ServiceWin32.hpp"

#pragma     warning(push)
#include    <CodeAnalysis\Warnings.h>
#pragma     warning(disable : ALL_CODE_ANALYSIS_WARNINGS 26812)
#define     SECURITY_WIN32
#include    <Security.h>
#include    <shlobj.h>
#include    <Iphlpapi.h>
#pragma     warning(pop)


// -----------------------------------------------------------------------
//  Local types
//
//  This structure is used to hold the system info data that we cache
//  during init and keep around. Some of it isn't for providing to the
//  external world, but is for our own internal purposes or for the
//  system id method we provide. We want to get this information very
//  separately from the returning of the id, to make it much harder to
//  hack. This info is all gotten during startup, so they can't just put
//  a break point on the calls to get the info and easily find out where
//  it is used.
//
//  apszArgList
//      This is storage for the command line parameters.
//
//  c2ProcRev
//      The processor revision info. Used in creating the system id
//      internally.
//
//  c4ArgCnt
//      This is the number of command line arguments that show to to the
//      outside world (we strip out the standard ones.)
//
//  c4CPUCount
//      The number of CPUs in the system, from the OS's perspective. If
//      the hardware allows some to be taken off line, then I would assume
//      you would only see the active ones here.
//
//  c4DriveSerialHash
//      This is a hash of the serial numbers of all of the fixed hard drives
//      in the system.
//
//  c4MACAddrHash
//      This is the MAC addresses hash used by the system did interface.
//      We iterate up to the first 16 adapters and hash their MAC addresses.
//
//  c4OSBuildNum
//  c4OSMajVersion
//  c4OSMinVersion
//      These are all about the version of the OS that we find ourselves
//      running on.
//
//  m_c4SSELevel
//      Indicates the level of SSE support. 0 means none, 1 is SSE, 2 is SSE2,
//      and so forth.
//
//  c8TotalPhysicalMem
//      The amount of memory installed in the machine.
//
//  szNodeName
//      The name assigned to this machine in the system setup. This size should be
//      grotesquely overkill for any node name. This cannot be a kernel string since
//      it would construct after our module init based setup, and overwrite the
//      contents. At this level we return the NetBIOS level name. Use the TCP/IP
//      stuff if you want the DNS level name.
//
//  szProcessName
//      The name of the process that this DLL is running in the context
//      of. This cannot be a kernel string since it would construct after
//      our module init based setup, and overwrite the contents.
// -----------------------------------------------------------------------
struct TCachedInfo
{
    tCIDLib::TCh*       apszArgList[kCIDLib::c4MaxCmdLineParms];
    tCIDLib::TCard2     c2ProcRev;
    tCIDLib::TCard4     c4ArgCnt;
    tCIDLib::TCard4     c4CPUCount;
    tCIDLib::TCard4     c4DriveSerialHash;
    tCIDLib::TCard4     c4MACAddrHash;
    tCIDLib::TCard4     c4OSBuildNum;
    tCIDLib::TCard4     c4OSMajVersion;
    tCIDLib::TCard4     c4OSMinVersion;
    tCIDLib::TCard4     c4OSServicePack;
    tCIDLib::TCard4     c4SSELevel;
    tCIDLib::TCard8     c8TotalPhysicalMem;
    tCIDLib::TZStr128   szNodeName;
    tCIDLib::TZStr512   szProcessName;
};



namespace CIDKernel_SystemInfo_Win32
{
    namespace
    {
        // -----------------------------------------------------------------------
        //  Local data
        //
        //  CachedInfo
        //      The cached system info that we cache up init and hang onto.
        // -----------------------------------------------------------------------
        TCachedInfo      CachedInfo;
    }
}


// ---------------------------------------------------------------------------
//  Intra-facility methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDKrnlModule::bInitTermSysInfo(const tCIDLib::EInitTerm eState)
{
    // We only have pre-constructor init
    if (eState == tCIDLib::EInitTerm::Initialize)
    {
        // Query the OS version info
        OSVERSIONINFOEX OSVer;
        OSVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
        if (!::GetVersionEx((OSVERSIONINFO*)&OSVer))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            return kCIDLib::False;
        }

        // We only support the NT platform
        if (OSVer.dwPlatformId != VER_PLATFORM_WIN32_NT)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_WrongOS);
            return kCIDLib::False;
        }

        // Copy over the stuff we want
        CIDKernel_SystemInfo_Win32::CachedInfo.c4OSBuildNum    = OSVer.dwBuildNumber;
        CIDKernel_SystemInfo_Win32::CachedInfo.c4OSMajVersion  = OSVer.dwMajorVersion;
        CIDKernel_SystemInfo_Win32::CachedInfo.c4OSMinVersion  = OSVer.dwMinorVersion;
        CIDKernel_SystemInfo_Win32::CachedInfo.c4OSServicePack = OSVer.wServicePackMajor;

        //
        //  Query the basic system info from the OS and copy out the stuff
        //  we want into the class' members.
        //
        SYSTEM_INFO SystemInfo = {0};
        ::GetSystemInfo(&SystemInfo);

        //
        //  Get the count of CPUs, and the type of CPU. It will have defaulted
        //  to 0, which is 'unkown' if we don't set it.
        //
        CIDKernel_SystemInfo_Win32::CachedInfo.c4CPUCount = SystemInfo.dwNumberOfProcessors;
        CIDKernel_SystemInfo_Win32::CachedInfo.c2ProcRev = SystemInfo.wProcessorRevision;

        //
        //  Confirm that our page size constant matches that of the system,
        //  otherwise we will be in bad shape.
        //
        if (SystemInfo.dwPageSize != kCIDLib::c4MemPageSize)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcMem_HostPageSize);
            return kCIDLib::False;
        }

        //
        //  Query the memory information and get the stuff out that we need
        //  to the class members.
        //
        MEMORYSTATUSEX MemStatus = {0};
        MemStatus.dwLength = sizeof(MEMORYSTATUSEX);
        ::GlobalMemoryStatusEx(&MemStatus);

        // Total physical memory will not change so we can save it
        CIDKernel_SystemInfo_Win32::CachedInfo.c8TotalPhysicalMem = MemStatus.ullTotalPhys;


        //
        //  Find the name of the host that we are running in. In our case it is always
        //  the NetBIOS level name.
        //
        tCIDLib::TCard4 c4NodeNameChars
        (
            c4MaxBufChars(CIDKernel_SystemInfo_Win32::CachedInfo.szNodeName) - 1
        );
        DWORD Res = ::GetComputerNameEx
        (
            ComputerNameNetBIOS
            , CIDKernel_SystemInfo_Win32::CachedInfo.szNodeName
            , &c4NodeNameChars
        );

        if (!Res)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcSysI_QueryCompName);
            return kCIDLib::False;
        }


        //
        //  Query the module name for the 0 module, which will get us the
        //  main process Exe module's file name.
        //
        constexpr tCIDLib::TCard4 c4Len = c4MaxBufChars
        (
            CIDKernel_SystemInfo_Win32::CachedInfo.szProcessName
        );
        tCIDLib::TCh szTmp[c4Len + 1];
        if (!::GetModuleFileName(0, szTmp, c4Len))
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcMod_ModuleNotFound);
            return kCIDLib::False;
        }

        //
        //  We need to strip out the path part of it. So we find the last slash
        //  and copy everything from there on out to the member string.
        //
        tCIDLib::TCh* pszTmp = TRawStr::pszFindLastChar(szTmp, kCIDLib::chBackSlash);
        if (!pszTmp)
        {
            TRawStr::CopyStr
            (
                CIDKernel_SystemInfo_Win32::CachedInfo.szProcessName
                , szTmp
                , c4MaxBufChars(CIDKernel_SystemInfo_Win32::CachedInfo.szProcessName)
            );
        }
        else
        {
            TRawStr::CopyStr
            (
                CIDKernel_SystemInfo_Win32::CachedInfo.szProcessName
                , pszTmp+1
                , c4MaxBufChars(CIDKernel_SystemInfo_Win32::CachedInfo.szProcessName)
            );
        }

        // And get rid of the extension as well
        pszTmp = TRawStr::pszFindLastChar
        (
            CIDKernel_SystemInfo_Win32::CachedInfo.szProcessName
            , kCIDLib::chPeriod
        );
        if (pszTmp)
            *pszTmp = kCIDLib::chNull;


        // Get a pointer to the command line parms. If none, then we are done
        const tCIDLib::TCh* pszCmd = ::GetCommandLine();
        if (pszCmd)
        {
            if (!TKrnlWin32::bParseCmdLine
            (
                pszCmd
                , CIDKernel_SystemInfo_Win32::CachedInfo.apszArgList
                , CIDKernel_SystemInfo_Win32::CachedInfo.c4ArgCnt
                , kCIDLib::c4MaxCmdLineParms))
            {
                return kCIDLib::False;
            }
        }

        // Iterate the IP adapters and create the MAC address hash
        {
            CIDKernel_SystemInfo_Win32::CachedInfo.c4MACAddrHash = 0;
            constexpr tCIDLib::TCard4 c4MaxAdaptors = 16;
            IP_ADAPTER_INFO AdapterInfo[c4MaxAdaptors];
            DWORD dwBufLen = sizeof(AdapterInfo);

            if (::GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_SUCCESS)
            {
                tCIDLib::THashVal hshAccum = 0;
                PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
                while (pAdapterInfo)
                {
                    //
                    //  If it's an ethernet based one, i.e. should be a
                    //  local hardware one, not a VPN type thing, then
                    //  add it's mac.
                    //
                    if (pAdapterInfo->Type == MIB_IF_TYPE_ETHERNET)
                    {
                        hshAccum = TRawMem::hshHashBuffer
                        (
                            hshAccum
                            , pAdapterInfo->Address
                            , 29
                            , pAdapterInfo->AddressLength
                        );
                    }
                    pAdapterInfo = pAdapterInfo->Next;
                }
                CIDKernel_SystemInfo_Win32::CachedInfo.c4MACAddrHash = hshAccum;
            };

            // Iterate the fixed drives and hash their serial numbers
            {
                CIDKernel_SystemInfo_Win32::CachedInfo.c4DriveSerialHash = 0;
                const tCIDLib::TCard4 c4DrMask = ::GetLogicalDrives();
                tCIDLib::TCard4     c4MaxPathCompLen;
                tCIDLib::TZStr128   szVolumeLabel;
                tCIDLib::TZStr64    szFileSysType;
                tCIDLib::TCh        szVolumePath[4] = L" :\\";

                tCIDLib::THashVal hshAccum = 0;
                for (tCIDLib::TCard4 c4Index = 0; c4Index < 26; c4Index++)
                {
                    if (c4DrMask & (0x1UL << c4Index))
                    {
                        // There's a drive here. See if it's fixed
                        szVolumePath[0] = tCIDLib::TCh(L'A' + c4Index);
                        if (::GetDriveType(szVolumePath) == DRIVE_FIXED)
                        {
                            tCIDLib::TCard4 c4VolFlags;
                            tCIDLib::TCard4 c4SerNum;
                            if (::GetVolumeInformation
                            (
                                szVolumePath
                                , szVolumeLabel
                                , c4MaxBufChars(szVolumeLabel)
                                , &c4SerNum
                                , &c4MaxPathCompLen
                                , &c4VolFlags
                                , szFileSysType
                                , c4MaxBufChars(szFileSysType)))
                            {
                                //
                                //  Add this guy's label info to the hash. It's
                                //  just a number.
                                //
                                hshAccum = TRawMem::hshHashBuffer(hshAccum, &c4SerNum, 29, 4);
                            }
                        }
                    }
                }

                CIDKernel_SystemInfo_Win32::CachedInfo.c4DriveSerialHash = hshAccum;
            }
        }


        // Remember if SSE instructions are available
        if (::IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE) != 0)
            CIDKernel_SystemInfo_Win32::CachedInfo.c4SSELevel = 2;
        else if (::IsProcessorFeaturePresent(PF_XMMI_INSTRUCTIONS_AVAILABLE) != 0)
            CIDKernel_SystemInfo_Win32::CachedInfo.c4SSELevel = 1;
        else
            CIDKernel_SystemInfo_Win32::CachedInfo.c4SSELevel = 0;
    }
    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  TKrnlSysInfo functions
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlSysInfo::bCmdLineArg(  const   tCIDLib::TCard4 c4Index
                            , const tCIDLib::TCh*&  pszToFill)
{
    if (c4Index >= CIDKernel_SystemInfo_Win32::CachedInfo.c4ArgCnt)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_IndexError);
        return kCIDLib::False;
    }
    pszToFill = CIDKernel_SystemInfo_Win32::CachedInfo.apszArgList[c4Index];
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlSysInfo::bIsHostAdmin()
{
    //
    //  We do a cheapo check by trying to open the service control manager
    //  with an all access. This requires admin privileges.
    //
    SC_HANDLE hTmp = ::OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
    if (!hTmp)
        return kCIDLib::False;
    ::CloseServiceHandle(hTmp);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSysInfo::bQueryAvailPhysicalMem(tCIDLib::TCard8& c8ToFill)
{
    MEMORYSTATUSEX MemStatus = {0};
    MemStatus.dwLength = sizeof(MEMORYSTATUSEX);
    if (!::GlobalMemoryStatusEx(&MemStatus))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Total physical memory will not change so we can save it
    c8ToFill = MemStatus.ullAvailPhys;
    return kCIDLib::True;
}


// Queries the machine id, if it's available (only on domains)
tCIDLib::TBoolean
TKrnlSysInfo::bQueryMachineID(          tCIDLib::TCh* const pchBuffer
                                , const tCIDLib::TCard4     c4MaxChars)
{
    tCIDLib::TCard4 c4Actual = c4MaxChars;
    if (!::GetComputerObjectName(NameUniqueId, pchBuffer, &c4Actual))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSysInfo::bQuerySpecialPath(        tCIDLib::TCh* const     pszBuffer
                                , const tCIDLib::TCard4         c4MaxChars
                                , const tCIDLib::ESpecialPaths  ePath)
{
    tCIDLib::TSInt iFolder;
    switch(ePath)
    {
        case tCIDLib::ESpecialPaths::CommonAppData:
            iFolder = CSIDL_COMMON_APPDATA;
            break;

        case tCIDLib::ESpecialPaths::CommonDocs :
            iFolder = CSIDL_COMMON_DOCUMENTS;
            break;

        case tCIDLib::ESpecialPaths::ProgramFiles :
            iFolder = CSIDL_PROGRAM_FILES;
            break;

        case tCIDLib::ESpecialPaths::ProgramFilesCommon :
            iFolder = CSIDL_PROGRAM_FILES_COMMON;
            break;

        case tCIDLib::ESpecialPaths::SystemPath :
            iFolder = CSIDL_SYSTEM;
            break;

        case tCIDLib::ESpecialPaths::CommonMenuPrograms :
            iFolder = CSIDL_COMMON_PROGRAMS;
            break;

        case tCIDLib::ESpecialPaths::CommonStartMenu :
            iFolder = CSIDL_COMMON_STARTMENU;
            break;

        case tCIDLib::ESpecialPaths::CommonStartup :
            iFolder = CSIDL_COMMON_STARTUP;
            break;

        case tCIDLib::ESpecialPaths::UserAppData :
            iFolder = CSIDL_APPDATA;
            break;

        case tCIDLib::ESpecialPaths::MyMusic :
            iFolder = CSIDL_MYMUSIC;
            break;

        case tCIDLib::ESpecialPaths::MyPictures :
            iFolder = CSIDL_MYPICTURES;
            break;

        case tCIDLib::ESpecialPaths::UserFavorites :
            iFolder = CSIDL_FAVORITES;
            break;

        case tCIDLib::ESpecialPaths::UserMenuPrograms :
            iFolder = CSIDL_PROGRAMS;
            break;

        case tCIDLib::ESpecialPaths::UserMenuStartup :
            iFolder = CSIDL_STARTUP;
            break;

        case tCIDLib::ESpecialPaths::UserStartMenu :
            iFolder = CSIDL_STARTMENU;
            break;

        default :
            TKrnlError::SetLastError(kKrnlErrs::errcSysI_QuerySpecialPath);
            return kCIDLib::False;
    };

    tCIDLib::TCh szBuf[kCIDLib::c4MaxPathLen + 1];
    HRESULT hRes = ::SHGetFolderPathW
    (
        0
        , iFolder
        , 0
        , SHGFP_TYPE_CURRENT
        , szBuf
    );

    if (!SUCCEEDED(hRes))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Normalize it to the caller's buffer
    return TKrnlFileSys::bNormalizePath(szBuf, pszBuffer, c4MaxChars);
}


tCIDLib::TBoolean
TKrnlSysInfo::bQueryUserName(       tCIDLib::TCh* const pszBuffer
                            , const tCIDLib::TCard4     c4MaxChars)
{
    tCIDLib::TCard4 c4Chars = c4MaxChars;

    if (!::GetUserName(pszBuffer, &c4Chars))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    pszBuffer[c4Chars] = 0;
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSysInfo::bRemoveCmdLineArg(const tCIDLib::TCard4 c4Index)
{
    if (!c4Index || (c4Index >= CIDKernel_SystemInfo_Win32::CachedInfo.c4ArgCnt))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_IndexError);
        return kCIDLib::False;
    }

    // Delete the string in this element
    delete [] CIDKernel_SystemInfo_Win32::CachedInfo.apszArgList[c4Index];

    // If this is the only element, then we are done
    if (CIDKernel_SystemInfo_Win32::CachedInfo.c4ArgCnt == 1)
    {
        CIDKernel_SystemInfo_Win32::CachedInfo.c4ArgCnt = 0;
        return kCIDLib::True;
    }

    // If this is the last element, we are done
    if (c4Index == CIDKernel_SystemInfo_Win32::CachedInfo.c4ArgCnt-1)
    {
        CIDKernel_SystemInfo_Win32::CachedInfo.c4ArgCnt--;
        return kCIDLib::True;
    }

    // Move the elements down
    tCIDLib::TCard4 c4MoveIndex = c4Index;
    for (;c4MoveIndex < CIDKernel_SystemInfo_Win32::CachedInfo.c4ArgCnt-1; c4MoveIndex++)
    {
        CIDKernel_SystemInfo_Win32::CachedInfo.apszArgList[c4MoveIndex]
            = CIDKernel_SystemInfo_Win32::CachedInfo.apszArgList[c4MoveIndex+1];
    }

    CIDKernel_SystemInfo_Win32::CachedInfo.c4ArgCnt--;
    return kCIDLib::True;
}


//
//  This method, looking at two system ids, calculates a percent confidence
//  that the system has changed. Since the system id stuff is platform
//  specific, having such a method is very helpful so that client code
//  doesn't have to try to interpret the results themselves.
//
tCIDLib::TCard4
TKrnlSysInfo::c4CheckSysChange(const TSysId& sidFirst, const TSysId& sidSec)
{
    tCIDLib::TBoolean abChanges[TKrnlSysInfo::c4SysIdCnt];
    tCIDLib::TCard4 c4Changes = 0;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < TKrnlSysInfo::c4SysIdCnt; c4Index++)
    {
        abChanges[c4Index] = (sidFirst.ac4Vals[c4Index] != sidSec.ac4Vals[c4Index]);
        if (abChanges[c4Index])
            c4Changes++;
    }

    // If no changes, return zero now
    if (!c4Changes)
        return 0;

    // If most of them changed, then return 100%
    if (c4Changes >= TKrnlSysInfo::c4SysIdCnt - 3)
        return 100;

    // Otherwise build up a score
    tCIDLib::TCard4 c4Score = 0;

    // If the machine id changes, that's a big indicator
    if (sidFirst.ac4Vals[0] != sidSec.ac4Vals[0])
        c4Score += 30;

    // If the mac address list, that's a fairly big one
    if (sidFirst.ac4Vals[1] != sidSec.ac4Vals[1])
        c4Score += 20;

    // If the list of local hard drives changes, that's a good indicator
    if (sidFirst.ac4Vals[2] != sidSec.ac4Vals[2])
        c4Score += 20;

    // If the processor revision number changes
    if (sidFirst.ac4Vals[3] != sidSec.ac4Vals[3])
        c4Score += 20;

    // If the node name changes
    if (sidFirst.ac4Vals[4] != sidSec.ac4Vals[4])
        c4Score += 20;

    // If the amount of physical memory changes
    if (sidFirst.ac4Vals[5] != sidSec.ac4Vals[5])
        c4Score += 20;

    // Cap it to 100 percent certainty
    if (c4Score > 100)
        c4Score = 100;

    return c4Score;
}



tCIDLib::TCard4 TKrnlSysInfo::c4CPUCount()
{
    return CIDKernel_SystemInfo_Win32::CachedInfo.c4CPUCount;
}


tCIDLib::TCard4 TKrnlSysInfo::c4CmdLineArgCount()
{
    return CIDKernel_SystemInfo_Win32::CachedInfo.c4ArgCnt;
}


// Return the SSE level flag
tCIDLib::TCard4 TKrnlSysInfo::c4SSELevel()
{
    return CIDKernel_SystemInfo_Win32::CachedInfo.c4SSELevel;
}


tCIDLib::TCard8 TKrnlSysInfo::c8TotalPhysicalMem()
{
    return CIDKernel_SystemInfo_Win32::CachedInfo.c8TotalPhysicalMem;
}


const tCIDLib::TCh* TKrnlSysInfo::pszNodeName()
{
    return CIDKernel_SystemInfo_Win32::CachedInfo.szNodeName;
}


const tCIDLib::TCh* TKrnlSysInfo::pszProcessName()
{
    return CIDKernel_SystemInfo_Win32::CachedInfo.szProcessName;
}


tCIDLib::TVoid
TKrnlSysInfo::QueryOSInfo(  tCIDLib::TCard4&    c4OSMajVersion
                            , tCIDLib::TCard4&  c4OSMinVersion
                            , tCIDLib::TCard4&  c4OSRev
                            , tCIDLib::TCard4&  c4OSBuildNum)
{
    c4OSBuildNum    = CIDKernel_SystemInfo_Win32::CachedInfo.c4OSBuildNum;
    c4OSMajVersion  = CIDKernel_SystemInfo_Win32::CachedInfo.c4OSMajVersion;
    c4OSMinVersion  = CIDKernel_SystemInfo_Win32::CachedInfo.c4OSMinVersion;

    // On Windows we give the service pack number as the revision
    c4OSRev = CIDKernel_SystemInfo_Win32::CachedInfo.c4OSServicePack;
}



tCIDLib::TVoid
TKrnlSysInfo::OverrideCmdLine(  const   tCIDLib::TCard4 c4Count
                                ,       tCIDLib::TCh**  apszParms)
{
    //
    //  Just overwrite the parms that we got during the automatic parsing
    //  of the normal command line. This is called from the service handling
    //  stuff. Services get their real parms after the exe is loaded, not
    //  through the normal means.
    //
    tCIDLib::TCard4 c4Index = 0;
    for (; c4Index < CIDKernel_SystemInfo_Win32::CachedInfo.c4ArgCnt; c4Index++)
    {
        delete [] CIDKernel_SystemInfo_Win32::CachedInfo.apszArgList[c4Index];
        CIDKernel_SystemInfo_Win32::CachedInfo.apszArgList[c4Index] = 0;
    }

    for (c4Index = 0; c4Index < c4Count; c4Index++)
    {
        CIDKernel_SystemInfo_Win32::CachedInfo.apszArgList[c4Index]
                                = TRawStr::pszReplicate(apszParms[c4Index]);
    }
    CIDKernel_SystemInfo_Win32::CachedInfo.c4ArgCnt = c4Count;
}



//
//  This method is used for licensing purposes. It will generate a unique
//  id for a machine that will change on another machine. In order to allow
//  the client application to be tolerant of small changes in the system
//  configuration, it consists of a set of separate numeric values. The
//  client app can compare a new one to a stored one and decide how many
//  changes he will tolerate. The values should be arranged such that
//  the ones most likely to indicate a new machine are earlier in the
//  list.
//
//  Each platform is free to fill these values in however it wants, it
//  just has to be consistent and never change how the values are generated.
//
//  For Windows, the 6 values are:
//
//      0. A hash of the machine id, which will only be available on domains.
//         If not available it's set to max card. But if it is different,
//         that indicates a very high likelihood of a new system.
//      1. A hash of the MAC addresses of up to the first 16 interface available
//      2. A hash of the serial numbers of all of the fixed drives.
//      3. The processor revision in the high 16 bits and the CPU count in
//         the low.
//      4. A hash of the node name
//      5. The amount of physical memory installed in K
//
tCIDLib::TVoid TKrnlSysInfo::QuerySysId(TSysId& sidToFill)
{
    tCIDLib::TZStr256   szTmp;

    // The rest are already available
    sidToFill.ac4Vals[1] = CIDKernel_SystemInfo_Win32::CachedInfo.c4MACAddrHash;
    sidToFill.ac4Vals[2] = CIDKernel_SystemInfo_Win32::CachedInfo.c4DriveSerialHash;
    sidToFill.ac4Vals[3] = CIDKernel_SystemInfo_Win32::CachedInfo.c4CPUCount;
    sidToFill.ac4Vals[3] |= tCIDLib::TCard4
    (
        CIDKernel_SystemInfo_Win32::CachedInfo.c4CPUCount << 16
    );
    sidToFill.ac4Vals[5] = tCIDLib::TCard4
    (
        CIDKernel_SystemInfo_Win32::CachedInfo.c8TotalPhysicalMem / 1024
    );

    //
    //  If there's a computer object name, get that and hash it. It's only
    //  available on domain based systems.
    //
    {
        tCIDLib::TCard4 c4Actual = 256;
        if (::GetComputerObjectName(NameUniqueId, szTmp, &c4Actual))
            sidToFill.ac4Vals[0] = TRawMem::hshHashBuffer(szTmp, 29, c4Actual);
        else
            sidToFill.ac4Vals[0] = kCIDLib::c4MaxCard;
    }

    // Do the node name
    sidToFill.ac4Vals[4] = TRawMem::hshHashBuffer
    (
        CIDKernel_SystemInfo_Win32::CachedInfo.szNodeName
        , 29
        , TRawStr::c4StrLen(CIDKernel_SystemInfo_Win32::CachedInfo.szNodeName)
          * kCIDLib::c4CharBytes
    );
}


//
//  Return a non-inclusive rectangle containing the coordinates of the full virtual
//  screen area (i.e. containing the areas of all of the monitors.
//
tCIDLib::TVoid TKrnlSysInfo::QueryVirtualScrRectl(tCIDLib::THostRectl& rectVS)
{
    rectVS.i4Left = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
    rectVS.i4Top = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
    rectVS.i4Right = rectVS.i4Left + ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
    rectVS.i4Bottom = rectVS.i4Top + ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
}



