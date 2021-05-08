//
// FILE NAME: CIDLib_MacroExcept.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/02/1995
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
//  This header is a bit of hack. Any code that may indirectly invoke CML macro
//  code needs to be careful and handle the two special exceptions that the macro
//  engine throws to indicate that some CML exception propogated all the way back
//  out, or the debugger is asking to exit.
//
//  Since they are not derived from TError, they have to be caught separately and
//  by their specific types.
//
//  However, that is sometimes all that a piece of code needs to know about the
//  macro engine. It's a gross amount of overhead for it to have to be dependent
//  on the huge macro engine header set just for this. So we move them down to
//  here so that both the macro engine and those types of indirect users of it
//  can see.
//
// CAVEATS/GOTCHAS:
//
//  1)  We don't include this in our headers. The macro engine includes it directly
//      as do the few things that might need to invoke macro code indirectly. If
//      they invoke macro code directly they'll get this via the macro engine headers.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


namespace tCIDMacroEng
{
    // -----------------------------------------------------------------------
    //  The debugging interface can be plugged into the engine and it will
    //  call it back on every source line it crosses. The implementor of that
    //  interface returns from that callback with one of these commands.
    // -----------------------------------------------------------------------
    enum class EDbgActs
    {
        CloseSession
        , Continue
        , Exit
    };
}

#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//   CLASS: TDbgExitException
//  PREFIX: excpt
//
//  This is a small exception, purposefully not derived from TError, that can be
//  thrown internally to unwind the stack when the debugger needs to force the
//  engine to back out and exit.
// ---------------------------------------------------------------------------
class TDbgExitException
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDbgExitException(const tCIDMacroEng::EDbgActs eReason) : m_eReason(eReason)
        {
        }

        TDbgExitException(const TDbgExitException& excptSrc)  = default;
        TDbgExitException(TDbgExitException&& excptSrc)  = default;

        ~TDbgExitException() = default;

        TDbgExitException& operator=(const TDbgExitException& excptSrc)  = default;
        TDbgExitException& operator=(TDbgExitException&& excptSrc)  = default;

        tCIDMacroEng::EDbgActs m_eReason;
};


// ---------------------------------------------------------------------------
//   CLASS: TExceptException
//  PREFIX: excpt
//
//  This is a small exception, purposefully not derived from TError, that can bee
//  thrown internally whhen a macro level exception is thrown and there is no
//  catch in the call frame, so that an unwind of the stack occurs.
// ---------------------------------------------------------------------------
class TExceptException
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TExceptException() = default;

        ~TExceptException() = default;
};


#pragma CIDLIB_POPPACK

