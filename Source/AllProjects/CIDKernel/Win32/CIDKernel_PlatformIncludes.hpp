//
// FILE NAME: CIDKernel_PlatformIncludes.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/07/2002
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file is provided by each platform implementation, and lets them
//  bring in headers for classes that implement platform specific stuff.
//  Some platforms might just have an empty file.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// Turn this stuff on to enable memory debugging in debug builds
#if CID_DEBUG_ON

#define CID_DEBUG_MEM 1
#if CID_DEBUG_MEM
#define _CRTDBG_MAP_ALLOC 1
#include <crtdbg.h>
#endif

#endif


