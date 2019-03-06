//
// FILE NAME: CIDKernel_PlatformInternal_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/12/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file is included into the CIDKernel_.hpp file, so that it is visible
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
#pragma once


// ---------------------------------------------------------------------------
//  Include the Windows headers for internal use
//
//  Prevent inclusion of winsock.h in windows.h, so that we can force the
//  inclusion of WinSock2 instead.
// ---------------------------------------------------------------------------
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif

#include    <windows.h>
#include    <winsock2.h>

// ---------------------------------------------------------------------------
//  Define the contents of our 'handle' types. There are small structures
//  that are opaque to the outside world. Internally their contents is seen
//  but platform independent parts of the kernel still don't look inside them
//  since they can change for each platform.
//
//  For the outside world, the CIDKernel_Handles.hpp file provides wrappers
//  for these that allows the world to manipulate them without having to
//  see inside them.
// ---------------------------------------------------------------------------

//
//  A comm port handle is just a regular handle under Win32.
//
struct  TCommHandleImpl
{
    HANDLE  hComm;
};


//
//  A console handle under Win32 is a handle to the file or console. If the
//  console has been redirected, then that handle is a file handle; else, its
//  a console handle. This is really only important inside the console
//  implementation which has to know which operations to call.
//
struct  TConsoleHandleImpl
{
    HANDLE  hCon;
};


//
//  A directory search handle is just a regular handle under Win32.
//
struct  TDirSearchHandleImpl
{
    HANDLE hDir;
};


//
//  An event semaphore under Win32 is just a handle to the event.
//
struct  TEventHandleImpl
{
    HANDLE hEvent;
};


//
//  A file under Win32 is just a handle to the file.
//
struct  TFileHandleImpl
{
    HANDLE hFile;
};


//
//  A media changer handle
//
struct  TMediaChgHandleImpl
{
    HANDLE hChanger;
};



//
//  A shared memory buffer under Win32 requires a memory handle, in
//  addition to the pointer itself.
//
struct  TMemoryHandleImpl
{
    HANDLE hBacking;
};


//
//  We compile the kernel in 'strict' mode, so a module handle has a
//  structure pointer type.
//
struct  TModuleHandleImpl
{
    HINSTANCE__* hInstance;
};


//
//  An mutex semaphore under Win32 is just a handle to the mutex.
//
struct  TMutexHandleImpl
{
    HANDLE hMutex;
};


//
//  A process under Win32 is represented by a process handle and a
//  process id. By putting them together in a single blob, we abstract
//  whether the platform has one or both of these.
//
struct  TProcessHandleImpl
{
    HANDLE          hProcess;
    tCIDLib::TCard4 pidThis;
};


//
//  A semaphore under Win32 is just a handle to the semaphore.
//
struct  TSemaphoreHandleImpl
{
    HANDLE  hSem;
};


//
//  A socket handle is almost always just an integral value, but just in
//  case we still do a handle abstraction for it.
//
struct TSocketHandleImpl
{
    tCIDLib::TCard4 hSock;
};


//
//  A thread under Win32 is represented by a thread handle and a thread
//  id. By putting them together in a single blob, we abstract whether
//  the platform has one or both of these.
//
struct TThreadHandleImpl
{
    HANDLE          hThread;
    tCIDLib::TCard4 tidThis;
};


