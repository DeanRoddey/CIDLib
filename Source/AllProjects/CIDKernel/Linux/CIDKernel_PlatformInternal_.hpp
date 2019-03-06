//
// FILE NAME: CIDKernel_PlatformInternal_.Hpp
//
// AUTHOR: Will Mason
//
// CREATED: 10/11/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file is included into the CIDKernel_.Hpp file, so that it is visible
//  to all files within the kernel. So it is used to bring in any platform
//  specific stuff that needs to be visible within the kernel, such as the
//  system headers.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// Place all platform includes here, so the precompiled headers will be at
// maximum efficiency.
#include <arpa/inet.h>
#include <ctype.h>
#include <curses.h>
#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <langinfo.h>
#include <linux/limits.h>
#include <linux/uio.h>
#include <math.h>
#include <mntent.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <pwd.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/kd.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/vfs.h>
#include <sys/wait.h>
#include <term.h>
#include <termios.h>
#include <utime.h>
#include <wchar.h>
#include <wctype.h>

// Including these last eliminates some warnings
#include <readline/history.h>
#include <readline/readline.h>

// This is necessary because the file <readline/rlstdc.h>
// destroys anyone who is not gcc!
#if defined(__KCC)
#undef inline
#undef const
#endif

//
// Forward delcarations
//
namespace TKrnlLinux
{
    class TRecursiveMutex;
    class TTermFifo;
    class TTermValueNode;
}


//
// Type declarations
//
#if defined(_SEM_SEMUN_UNDEFINED)
union semun
{
    int                 val;
    struct semid_ds*    buf;
    unsigned short int* array;
    struct seminfo*     __buf;
};
#endif


//
// Internal structures
//
struct TConsoleHandleImpl
{
    struct termios              TermInfo;
    TKrnlLinux::TTermFifo*      pfifoThis;
    TKrnlLinux::TTermValueNode* pnodeTree;
    tCIDLib::TBoolean           bSingleCharMode;
    tCIDLib::TBoolean           bValid;
};

struct TDirSearchHandleImpl
{
    DIR*                pDir;
    struct dirent*      pNextDirEntry;
    tCIDLib::TSCh*      pszDirName;
    tCIDLib::TSCh*      pszFileSpec;
};

struct TEventHandleImpl
{
    // Pthread stuff
    pthread_mutex_t     mtxThis;
    pthread_cond_t      condThis;
    tCIDLib::TBoolean   bSet;

    // System V IPC stuff, for named events
    tCIDLib::TSInt      iSysVSemId;
    tCIDLib::TBoolean   bSysVOwner;

    // Standard handle stuff, enables bDuplicate() method
    tCIDLib::TCard4     c4RefCount;
};

struct TFileHandleImpl
{
    tCIDLib::TSInt      iFd;
};

struct TMemoryHandleImpl
{
    // System V IPC stuff
    tCIDLib::TSInt               iShmId;
    tCIDLib::TBoolean            bOwner;

    // To synchronize access to c4RefCount
    TKrnlLinux::TRecursiveMutex* prmtxThis;

    // Standard handle stuff, enables bDuplicate() method
    tCIDLib::TCard4              c4RefCount;
};

struct TModuleHandleImpl
{
    tCIDLib::TVoid*     pHandle;
    tCIDLib::TCh*       pszFullPath;
};

struct TMutexHandleImpl
{
    // Pthread stuff
    TKrnlLinux::TRecursiveMutex* prmtxThis;

    // System V IPC stuff, for named mutexes
    tCIDLib::TSInt               iSysVSemId;
    tCIDLib::TBoolean            bSysVOwner;
    tCIDLib::TCard4              c4LockCount;

    // Standard handle stuff, enables bDuplicate() method
    tCIDLib::TCard4              c4RefCount;
};

struct TProcessHandleImpl
{
    pid_t               pidThis;
    tCIDLib::EExitCodes eExit;
    tCIDLib::TCard4     c4JiffyStartTime;
    struct timeval      UserTime;
    struct timeval      KernelTime;
    struct timeval      EndTime;
    tCIDLib::TBoolean   bAlreadyClean;
};

struct TSemaphoreHandleImpl
{
    // Pthread stuff
    pthread_mutex_t     mtxThis;
    pthread_cond_t      condThis;
    tCIDLib::TCard4     c4CurCount;

    // System V IPC stuff, for named semaphores
    tCIDLib::TSInt      iSysVSemId;
    tCIDLib::TBoolean   bSysVOwner;

    // Standard handle stuff, enables bDuplicate() method
    tCIDLib::TCard4     c4RefCount;
};

struct TSocketHandleImpl
{
    tCIDLib::TSInt      iDescr;
};

struct TThreadHandleImpl
{
    tCIDLib::TThreadId  tidThis;
    tCIDLib::EExitCodes eExit;
    tCIDLib::TBoolean   bJoined;
};
