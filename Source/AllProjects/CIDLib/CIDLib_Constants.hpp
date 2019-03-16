//
// FILE NAME: CIDLIb_Constants.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/14/2001
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
//  Most of our constants are really in CIDKernel, because they are very
//  fundamental and need to be shared with the platform wrapper code. But
//  some are higher level so we have them here.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

namespace kCIDLib
{
    // -----------------------------------------------------------------------
    //  Tree collection constants
    // -----------------------------------------------------------------------
    const tCIDLib::TCh          chTreeSepChar   = L'/';
    const tCIDLib::TCh* const   pszTreeSepChar  = L"/";


    // -----------------------------------------------------------------------
    //  Default max size for memory buffers if no max is given, 16MB.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4       c4DefMaxBufferSz = kCIDLib::c4Sz_16M;


    // -----------------------------------------------------------------------
    //  The thread wait list provides a 'reason' mechanism, so that threads
    //  can block for a reason and threads that are blocked for that reason
    //  can be selectively awakened.
    //
    //  We provide some common codes here.
    //
    //  All         - Used to just wake up all threads. It shouldn't be used
    //                by threads blocking themselves it should just be used
    //                by threads waking up other threads.
    //
    //  None        - Just blocking for no particular reason. This can used
    //                by both sides if the reason codes aren't really needed.
    //
    //  WaitData    -
    //  WaitSpace   - Commonly used by the collections when they want to
    //                support thread farm type functionality, such as the
    //                queue collection.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4 c4TWLReason_All       = 0xFFFFFFFE;
    const tCIDLib::TCard4 c4TWLReason_None      = 0xFFFFFFFF;
    const tCIDLib::TCard4 c4TWLReason_WaitData  = 0xFFAFD4A2;
    const tCIDLib::TCard4 c4TWLReason_WaitSpace = 0xAA8F34AC;


    // -----------------------------------------------------------------------
    //  The name we use for the company name in any of the named resources
    //  we create (which use the TResourceName class.)
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const   pszResCompany   = L"CharmedQuark";


    // -----------------------------------------------------------------------
    //  Some common keys/scopes used in the stats cache. We have four
    //  fundamental scopes. One for information stuff, one for settings stuff,
    //  and one for stats stuff, and one for control stuff.
    //
    //  The logging stuff is used by each facility. So there will be facility
    //  names under these where each facility will create their items.
    //
    //  And we define the paths for some values that we maintain at this level.
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const   pszStat_Scope_Ctrl      = L"/Ctrl/";
    const tCIDLib::TCh* const   pszStat_Scope_Info      = L"/Info/";
    const tCIDLib::TCh* const   pszStat_Scope_Settings  = L"/Settings/";
    const tCIDLib::TCh* const   pszStat_Scope_Stats     = L"/Stats/";

    const tCIDLib::TCh* const   pszStat_Scope_LogThresh = L"/Settings/LogThresh/";
    const tCIDLib::TCh* const   pszStat_Scope_LogFlags  = L"/Settings/LogFlags/";
    const tCIDLib::TCh* const   pszStat_Scope_LogTrace  = L"/Settings/LogTrace/";

    const tCIDLib::TCh* const   pszStat_Scope_AppInfo   = L"/Info/App/";
    const tCIDLib::TCh* const   pszStat_AppInfo_UpSince = L"/Info/App/UpSince";

    const tCIDLib::TCh* const   pszStat_Scope_Core      = L"/Stats/Core/";
    const tCIDLib::TCh* const   pszStat_Core_ThreadCount= L"/Stats/Core/ThreadCnt";
}


// ---------------------------------------------------------------------------
//  Some macros for doing debug mode assertions and such
// ---------------------------------------------------------------------------
#if CID_DEBUG_ON
    #define CIDAssert(test,msg) \
    if (!(test)) \
    { \
        facCIDLib().ThrowErr \
        ( \
            CID_FILE \
            , CID_LINE \
            , kCIDErrs::errcDbg_AssertFailed \
            , msg \
            , tCIDLib::ESeverities::Failed \
            , tCIDLib::EErrClasses::Assert \
        ); \
    }

    #define CIDAssertX(test,msg,tok1) \
    if (!(test)) \
    { \
        TString strAssXMsg(msg); \
        strAssXMsg.eReplaceToken(tok1, L'1'); \
        facCIDLib().ThrowErr \
        ( \
            CID_FILE \
            , CID_LINE \
            , kCIDErrs::errcDbg_AssertFailed \
            , strAssXMsg \
            , tCIDLib::ESeverities::Failed \
            , tCIDLib::EErrClasses::Assert \
        ); \
    }

    #define CIDAssert2(msg) \
    facCIDLib().ThrowErr \
    ( \
        CID_FILE \
        , CID_LINE \
        , kCIDErrs::errcDbg_AssertFailed \
        , msg \
        , tCIDLib::ESeverities::Failed \
        , tCIDLib::EErrClasses::Assert \
    ); \

    #define CIDAssert2X(msg,tok1) \
    { \
        TString strAss2XMsg(msg); \
        strAss2XMsg.eReplaceToken(tok1, L'1'); \
        facCIDLib().ThrowErr \
        ( \
            CID_FILE \
            , CID_LINE \
            , kCIDErrs::errcDbg_AssertFailed \
            , strAss2XMsg \
            , tCIDLib::ESeverities::Failed \
            , tCIDLib::EErrClasses::Assert \
            , tok1 \
        ); \
    }

#else
    #define CIDAssert(test,msg)
    #define CIDAssertX(test,msg,tok1)
    #define CIDAssert2(msg)
    #define CIDAssert2X(msg,tok1)
#endif

#pragma CIDLIB_POPPACK

