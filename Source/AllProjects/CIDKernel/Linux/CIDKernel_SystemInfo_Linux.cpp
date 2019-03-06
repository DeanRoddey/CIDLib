//
// FILE NAME: CIDKernel_SystemInfo_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 12/21/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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

// ---------------------------------------------------------------------------
//  Global variables
// ---------------------------------------------------------------------------

//
// These are used to pick off argc and argv as they go by. That way I don't have
// to try to reproduce the array provided by the startup code. The proc file
// system only provides the full command line, and using these global variables
// saves me a lot of possible errors involving parsing inconsistencies between
// me and Linux.
//
int CIDKernel_SystemInfo_Linux_argc = -1;
char** CIDKernel_SystemInfo_Linux_argv = 0;

namespace
{
    // ---------------------------------------------------------------------------
    //  Local types
    //
    //  This structure is used to hold the system info data that we cache during
    //  init and keep around.
    //
    //  __apszArgList
    //      This is storage for the command line parameters.
    //
    //  __c4ArgCnt
    //      This is the number of command line arguments that show to to the
    //      outside world (we strip out the standard ones.)
    //
    //  __c4CPUCount
    //      The number of CPUs in the system, from the OS's perspective. If the
    //      hardware allows some to be taken off line, then I would assume you
    //      would only see the active ones here.
    //
    //  __c4OSBuildNum
    //  __c4OSMajVersion
    //  __c4OSMinVersion
    //      These are all about the version of the OS that we find ourselves
    //      running on.
    //
    //  __c4TotalPhysicalMem
    //      The amount of memory installed in the machine.
    //
    //  __eCPUType
    //      The type of CPUs in this machine.
    //
    //  __szNodeName
    //      The name assigned to this machine in the system setup. This size
    //      should be grotesquely overkill for any node name.
    //
    //  __szProcessName
    //      The name of the process that this DLL is running in the context of.
    // ---------------------------------------------------------------------------
    struct TCachedInfo
    {
        tCIDLib::TCh*           apszArgList[kCIDLib::c4MaxCmdLineParms];
        tCIDLib::TCard4         c4ArgCnt;
        tCIDLib::TCard4         c4CPUCount;
        tCIDLib::TCard4         c4OSBuildNum;
        tCIDLib::TCard4         c4OSMajVersion;
        tCIDLib::TCard4         c4OSMinVersion;
        tCIDLib::TCard4         c4OSRev;
        tCIDLib::TCard4         c4TotalPhysicalMem;
        tCIDLib::ECPUTypes      eCPUType;
        tCIDLib::TZStr64        szNodeName;
        tCIDLib::TZStr128       szProcessName;
    };


    // ---------------------------------------------------------------------------
    //  Local data
    //
    //  __CachedInfo
    //      The cached system info that we cache up init and hang onto.
    // ---------------------------------------------------------------------------
    TCachedInfo      __CachedInfo;


    // ---------------------------------------------------------------------------
    //  Intra-facility methods
    // ---------------------------------------------------------------------------
    tCIDLib::TBoolean __bQueryVersionInfo(const struct utsname& UtsName)
    {
        tCIDLib::TSCh* pszBegin = const_cast<tCIDLib::TSCh*>(UtsName.release);
        tCIDLib::TSCh* pszEnd;
        tCIDLib::TCard4 c4Tmp = ::strtoul(pszBegin, &pszEnd, 10);
        if (pszEnd == pszBegin || *pszEnd == '\000')
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidFormat);
            return kCIDLib::False;
        }
        __CachedInfo.c4OSMajVersion = c4Tmp;

        pszBegin = ++pszEnd;
        c4Tmp = ::strtoul(pszBegin, &pszEnd, 10);
        if (pszEnd == pszBegin || *pszEnd == '\000')
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidFormat);
            return kCIDLib::False;
        }
        __CachedInfo.c4OSMinVersion = c4Tmp;

        pszBegin = ++pszEnd;
        c4Tmp = ::strtoul(pszBegin, &pszEnd, 10);
        if (pszEnd == pszBegin)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidFormat);
            return kCIDLib::False;
        }
        __CachedInfo.c4OSRev = c4Tmp;

        tCIDLib::TSCh szVerCopy[SYS_NMLN];
        ::strcpy(szVerCopy, &UtsName.version[1]);

        c4Tmp = ::strtoul(szVerCopy, &pszEnd, 10);
        if (pszEnd == szVerCopy)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidFormat);
            return kCIDLib::False;
        }
        __CachedInfo.c4OSBuildNum = c4Tmp;

        return kCIDLib::True;
    }

    tCIDLib::TBoolean __bQueryCPUInfo(const struct utsname& UtsName)
    {
        if (!::strcmp(UtsName.machine, "i386"))
        {
            __CachedInfo.eCPUType = tCIDLib::ECPUTypes::386DX;
        }
        else if (!::strcmp(UtsName.machine, "i486"))
        {
            __CachedInfo.eCPUType = tCIDLib::ECPUTypes::486DX;
        }
        else if (!::strcmp(UtsName.machine, "i586")
             ||  !::strcmp(UtsName.machine, "i686"))
        {
            __CachedInfo.eCPUType = tCIDLib::ECPUTypes::Pentium;
        }
        else
        {
            __CachedInfo.eCPUType = tCIDLib::ECPUTypes::Unknown;
        }

        FILE* CPUFile = ::fopen("/proc/cpuinfo", "r");
        if (!CPUFile)
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }

        tCIDLib::TCard4 c4ProcessorCount = 0;
        tCIDLib::TSCh szLine[128];

        const tCIDLib::TSCh szProcLine[] = "processor";
        const tCIDLib::TCard4 c4ProcLineLen = sizeof(szProcLine) - 1;

        while (::fgets(szLine, sizeof(szLine), CPUFile))
        {
            if (!::strncmp(szLine, szProcLine, c4ProcLineLen))
                c4ProcessorCount++;
        }

        ::fclose(CPUFile);

        __CachedInfo.c4CPUCount = c4ProcessorCount;

        return kCIDLib::True;
    }

    tCIDLib::TBoolean __bQueryProcMemInfo()
    {
        FILE* MemFile = ::fopen("/proc/meminfo", "r");
        if (!MemFile)
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }

        tCIDLib::TSCh szLine[128];
        if (!::fgets(szLine, sizeof(szLine), MemFile))
        {
            ::fclose(MemFile);
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidFormat);
            return kCIDLib::False;
        }

        tCIDLib::TCard4 c4Tmp;
        tCIDLib::TSInt iRc = ::fscanf(MemFile, "%*s %lu", &c4Tmp);

        ::fclose(MemFile);

        if (iRc != 1)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidFormat);
            return kCIDLib::False;
        }

        __CachedInfo.c4TotalPhysicalMem = c4Tmp;

        return kCIDLib::True;
    }

    tCIDLib::TBoolean __bQueryNameFromPID(pid_t pid)
    {
        tCIDLib::TSCh szProcPid[128];
        ::sprintf(szProcPid, "/proc/%i/stat", pid);

        FILE* StatFile = ::fopen(szProcPid, "r");
        if (!StatFile)
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }

        tCIDLib::TSCh szParenName[512];
        tCIDLib::TSInt iRc = ::fscanf(StatFile, "%*d %512s", szParenName);

        ::fclose(StatFile);

        if (iRc != 1)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidFormat);
            return kCIDLib::False;
        }

        tCIDLib::TCard4 c4Len = ::strlen(szParenName);
        if (c4Len < 2)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidFormat);
            return kCIDLib::False;
        }

        szParenName[c4Len - 1] = '\000';

        TRawStr::pszConvert(&szParenName[1]
                            , __CachedInfo.szProcessName
                            , c4MaxBufChars(__CachedInfo.szProcessName));

        return kCIDLib::True;
    }

    tCIDLib::TVoid __InitArgArray()
    {
        __CachedInfo.c4ArgCnt = CIDKernel_SystemInfo_Linux_argc;
        for (tCIDLib::TCard4 c4Idx = 0; c4Idx < __CachedInfo.c4ArgCnt; c4Idx++)
        {
            __CachedInfo.apszArgList[c4Idx] =
                TRawStr::pszConvert(CIDKernel_SystemInfo_Linux_argv[c4Idx]);
        }
        CIDKernel_SystemInfo_Linux_argc = -1;
    }
}


tCIDLib::TBoolean
TCIDKrnlModule::__bInitTermSysInfo(const tCIDLib::EInitTerm eInitTerm)
{
    // We only have pre-constructor init
    if (eInitTerm == tCIDLib::EInitTerm::Initialize)
    {
        if (!__bQueryProcMemInfo())
            return kCIDLib::False;

        tCIDLib::TProcessId pidThis = ::getpid();
        if (!__bQueryNameFromPID(pidThis))
            return kCIDLib::False;

        if (::getpagesize() != kCIDLib::c4MemPageSize)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcMem_HostPageSize);
            return kCIDLib::False;
        }

        // Find the name of the workstation that we are running in.
        struct utsname UtsName;
        if (::uname(&UtsName))
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }
        TRawStr::pszConvert(UtsName.nodename
                            , __CachedInfo.szNodeName
                            , c4MaxBufChars(__CachedInfo.szNodeName));

        if (!__bQueryVersionInfo(UtsName))
            return kCIDLib::False;

        if (!__bQueryCPUInfo(UtsName))
            return kCIDLib::False;
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
    if (CIDKernel_SystemInfo_Linux_argc != -1)
        __InitArgArray();

    if (c4Index >= __CachedInfo.c4ArgCnt)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_IndexError);
        return kCIDLib::False;
    }
    pszToFill = __CachedInfo.apszArgList[c4Index];
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSysInfo::bLoadOSMsg(const  tCIDLib::TOSErrCode  errcId
                         ,       tCIDLib::TCh* const pszBuffer
                         , const tCIDLib::TCard4     c4MaxChars)
{
    TRawStr::pszConvert(::strerror(errcId), pszBuffer, c4MaxChars);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSysInfo::bQueryUserName(       tCIDLib::TCh* const pszBuffer
                            , const tCIDLib::TCard4     c4MaxChars)
{
    struct passwd* pwd = ::getpwuid(::getuid());

    if (!pwd)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    if (c4MaxChars < ::strlen(pwd->pw_name))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InsufficientBuffer);
        return kCIDLib::False;
    }

    TRawStr::pszConvert(pwd->pw_name, pszBuffer, c4MaxChars);

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlSysInfo::bRemoveCmdLineArg(const tCIDLib::TCard4 c4Index)
{
    if (CIDKernel_SystemInfo_Linux_argc != -1)
        __InitArgArray();

    if (!c4Index || (c4Index >= __CachedInfo.c4ArgCnt))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_IndexError);
        return kCIDLib::False;
    }

    // Delete the string in this element
    delete [] __CachedInfo.apszArgList[c4Index];

    // If this is the only element, then we are done
    if (__CachedInfo.c4ArgCnt == 1)
    {
        __CachedInfo.c4ArgCnt = 0;
        return kCIDLib::True;
    }

    // If this is the last element, we are done
    if (c4Index == __CachedInfo.c4ArgCnt-1)
    {
        __CachedInfo.c4ArgCnt--;
        return kCIDLib::True;
    }

    // Move the elements down
    for (tCIDLib::TCard4 c4MoveIndex = c4Index; c4MoveIndex < __CachedInfo.c4ArgCnt-1; c4MoveIndex++)
        __CachedInfo.apszArgList[c4MoveIndex] = __CachedInfo.apszArgList[c4MoveIndex+1];

    __CachedInfo.c4ArgCnt--;
    return kCIDLib::True;
}


tCIDLib::TCard4 TKrnlSysInfo::c4CPUCount()
{
    return __CachedInfo.c4CPUCount;
}


tCIDLib::TCard4 TKrnlSysInfo::c4CmdLineArgCount()
{
    if (CIDKernel_SystemInfo_Linux_argc != -1)
        __InitArgArray();

    return __CachedInfo.c4ArgCnt;
}


tCIDLib::TCard4 TKrnlSysInfo::c4TotalPhysicalMem()
{
    return __CachedInfo.c4TotalPhysicalMem;
}


tCIDLib::ECPUTypes TKrnlSysInfo::eCPUType()
{
    return __CachedInfo.eCPUType;
}


const tCIDLib::TCh* TKrnlSysInfo::pszNodeName()
{
    return __CachedInfo.szNodeName;
}


const tCIDLib::TCh* TKrnlSysInfo::pszProcessName()
{
    return __CachedInfo.szProcessName;
}


tCIDLib::TVoid
TKrnlSysInfo::QueryOSInfo(  tCIDLib::TCard4&    c4OSMajVersion
                            , tCIDLib::TCard4&  c4OSMinVersion
                            , tCIDLib::TCard4&  c4OSRev
                            , tCIDLib::TCard4&  c4OSBuildNum)
{
    c4OSBuildNum    = __CachedInfo.c4OSBuildNum;
    c4OSMajVersion  = __CachedInfo.c4OSMajVersion;
    c4OSMinVersion  = __CachedInfo.c4OSMinVersion;
    c4OSRev         = __CachedInfo.c4OSRev;
}
