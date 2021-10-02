//
// FILE NAME: CIDLIb_Constants.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/14/2001
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
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
    constexpr tCIDLib::TCh              chTreeSepChar   = L'/';
    constexpr const tCIDLib::TCh* const pszTreeSepChar  = L"/";


    // -----------------------------------------------------------------------
    //  Default max size for memory buffers if no max is given, 16MB.
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TCard4   c4DefMaxBufferSz = kCIDLib::c4Sz_16M;


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
    constexpr tCIDLib::TCard4 c4TWLReason_All       = 0xFFFFFFFE;
    constexpr tCIDLib::TCard4 c4TWLReason_None      = 0xFFFFFFFF;
    constexpr tCIDLib::TCard4 c4TWLReason_WaitData  = 0xFFAFD4A2;
    constexpr tCIDLib::TCard4 c4TWLReason_WaitSpace = 0xAA8F34AC;


    // -----------------------------------------------------------------------
    //  A value used by the hash based collections to represent an unset hash
    // -----------------------------------------------------------------------
    constexpr tCIDLib::THashVal hshInvalid          = static_cast<tCIDLib::THashVal>(-1);


    // -----------------------------------------------------------------------
    //  The name we use for the company name in any of the named resources
    //  we create (which use the TResourceName class.)
    // -----------------------------------------------------------------------
    constexpr const tCIDLib::TCh* const   pszResCompany   = L"CharmedQuark";


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
    constexpr const tCIDLib::TCh* const   pszStat_Scope_Ctrl          = L"/Ctrl/";
    constexpr const tCIDLib::TCh* const   pszStat_Scope_Info          = L"/Info/";
    constexpr const tCIDLib::TCh* const   pszStat_Scope_Settings      = L"/Settings/";
    constexpr const tCIDLib::TCh* const   pszStat_Scope_Stats         = L"/Stats/";

    constexpr const tCIDLib::TCh* const   pszStat_Scope_LogThresh     = L"/Settings/LogThresh/";
    constexpr const tCIDLib::TCh* const   pszStat_Scope_LogFlags      = L"/Settings/LogFlags/";
    constexpr const tCIDLib::TCh* const   pszStat_Scope_LogTrace      = L"/Settings/LogTrace/";

    constexpr const tCIDLib::TCh* const   pszStat_Scope_AppInfo       = L"/Info/App/";
    constexpr const tCIDLib::TCh* const   pszStat_AppInfo_DroppedLogEvs = L"/Info/App/DroppedLogEvs";
    constexpr const tCIDLib::TCh* const   pszStat_AppInfo_LogErrors   = L"/Info/App/LogErrors";
    constexpr const tCIDLib::TCh* const   pszStat_AppInfo_UpSince     = L"/Info/App/UpSince";

    constexpr const tCIDLib::TCh* const   pszStat_Scope_Core          = L"/Stats/Core/";
    constexpr const tCIDLib::TCh* const   pszStat_Core_ThreadCount    = L"/Stats/Core/ThreadCnt";
}

namespace tCIDLib
{
    [[noreturn]] CIDLIBEXP tCIDLib::TVoid ThrowAssert
    (
        const   tCIDLib::TCh* const pszErr
        , const tCIDLib::TCh* const pszFile
        , const tCIDLib::TCard4     c4Line
    );

    [[noreturn]] CIDLIBEXP tCIDLib::TVoid ThrowAssert
    (
        const   TString&            strErr
        , const tCIDLib::TCh* const pszFile
        , const tCIDLib::TCard4     c4Line
    );

    [[noreturn]] CIDLIBEXP tCIDLib::TVoid ThrowAssert
    (
        const   tCIDLib::TCh* const pszErr
        , const tCIDLib::TCh* const pszFile
        , const tCIDLib::TCard4     c4Line
        , const MFormattable&       mfmtblToken1
    );

    [[noreturn]] CIDLIBEXP tCIDLib::TVoid ThrowAssert
    (
        const   TString&            strErr
        , const tCIDLib::TCh* const pszFile
        , const tCIDLib::TCard4     c4Line
        , const MFormattable&       mfmtblToken1
    );

    [[noreturn]] CIDLIBEXP tCIDLib::TVoid ThrowPreCond
    (
        const   tCIDLib::TCh* const pszCond
        , const tCIDLib::TCh* const pszFile
        , const tCIDLib::TCard4     c4Line
    );

    [[noreturn]] CIDLIBEXP tCIDLib::TVoid ThrowPreCond
    (
        const   TString&            strCond
        , const tCIDLib::TCh* const pszFile
        , const tCIDLib::TCard4     c4Line
    );

    [[noreturn]] CIDLIBEXP tCIDLib::TVoid ThrowPostCond
    (
        const   tCIDLib::TCh* const pszCond
        , const tCIDLib::TCh* const pszFile
        , const tCIDLib::TCard4     c4Line
    );

    [[noreturn]] CIDLIBEXP tCIDLib::TVoid ThrowPostCond
    (
        const   TString&            strCond
        , const tCIDLib::TCh* const pszFile
        , const tCIDLib::TCard4     c4Line
    );
}


// ---------------------------------------------------------------------------
//  Some macros for doing debug mode assertions and such. We have to do it very low
//  level here for compliance reasons. We call a helper in CIDLib.cpp, but declare
//  above so that it's visible here.
// ---------------------------------------------------------------------------
#if CID_DEBUG_ON
    #define CIDAssert(test,msg) \
    if (!(test)) { tCIDLib::ThrowAssert(msg, CID_FILE, CID_LINE); }

    #define CIDAssertX(test,msg,tok1) \
    if (!(test)) { tCIDLib::ThrowAssert(msg, CID_FILE, CID_LINE, tok1); }

    #define CIDAssert2(msg)  tCIDLib::ThrowAssert(msg, CID_FILE, CID_LINE);

    #define CIDAssert2X(msg,tok1)  tCIDLib::ThrowAssert(msg, CID_FILE, CID_LINE, tok1);

    #define CIDAssert3(test) \
    if (!(test)) { tCIDLib::ThrowAssert(L## #test, CID_FILE, CID_LINE); }

    // Never actually returns, this is for code analysis reasons
    inline tCIDLib::TBoolean
    bAssertTest(const   tCIDLib::TBoolean       bResult
                , const tCIDLib::TCh* const     pszMsg
                , const tCIDLib::TCh* const     pszFile
                , const tCIDLib::TCard4         c4Line)
    {
        if (bResult)
            return kCIDLib::True;
        tCIDLib::ThrowAssert(pszMsg, pszFile, c4Line);
        return kCIDLib::False;
    }

    inline tCIDLib::TBoolean
    bAssertTest1(const  tCIDLib::TBoolean       bResult
                , const tCIDLib::TCh* const     pszMsg
                , const tCIDLib::TCh* const     pszFile
                , const tCIDLib::TCard4         c4Line
                , const MFormattable&           mfmtblTok1)
    {
        if (bResult)
            return kCIDLib::True;
        tCIDLib::ThrowAssert(pszMsg, pszFile, c4Line, mfmtblTok1);
        return kCIDLib::False;
    }

    #define bCIDAssert(test,msg) \
    bAssertTest((test), msg, CID_FILE, CID_LINE)

    #define bCIDAssertX(test,msg, tok1) \
    bAssertTest1((test), msg, CID_FILE, CID_LINE, tok1)

#else
    #define CIDAssert(test,msg) {}
    #define CIDAssertX(test,msg,tok1) {}
    #define CIDAssert2(msg) {}
    #define CIDAssert2X(msg,tok1) {}
    #define CIDAssert3(msg) {}

    #define bCIDAssert(test,msg) kCIDLib::True
    #define bCIDAssertX(test,msg,tok1) kCIDLib::True
    #define bCIDAssert2(msg) kCIDLib::True
    #define bCIDAssert2X(msg,tok1) kCIDLib::True
#endif


// ---------------------------------------------------------------------------
// These are not debug-only, they are pre-psot condition checks
// ---------------------------------------------------------------------------
inline tCIDLib::TBoolean
bPreCondTest(const  tCIDLib::TBoolean       bResult
            , const tCIDLib::TCh* const     pszMsg
            , const tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line)
{
    if (bResult)
        return kCIDLib::True;
    tCIDLib::ThrowPreCond(pszMsg, pszFile, c4Line);
    return kCIDLib::False;
}

inline tCIDLib::TBoolean
bPostCondTest(  const   tCIDLib::TBoolean       bResult
                , const tCIDLib::TCh* const     pszMsg
                , const tCIDLib::TCh* const     pszFile
                , const tCIDLib::TCard4         c4Line)
{
    if (bResult)
        return kCIDLib::True;
    tCIDLib::ThrowPostCond(pszMsg, pszFile, c4Line);
    return kCIDLib::False;
}


#define bCIDPreCond(test) \
bPreCondTest((test), CIDLib_MakeLStr2(test), CID_FILE, CID_LINE)

#define bCIDPostCond(test) \
bPostCondTest((test), CIDLib_MakeLStr2(test), CID_FILE, CID_LINE)

#pragma CIDLIB_POPPACK
