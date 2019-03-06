//
// FILE NAME: CIDMacroEng_DebugIntf.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/17/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_DebugIntf.cpp file, which
//  implements MMEngDebugIntf class, which is a pure virtual mixin interface
//  that allows a class to be plugged into the macro engine to get debug
//  events and to give commands to the macro engine in debug mode.
//
//  It gives the following information:
//
//      eAtLine()       - A new current line opcode was reached. The current
//                        context info is passed.
//      CallStackChange - A new method call was pushed onto or popped off of
//                        the call stack.
//      Finished        - The run has either finished normally or was forced
//                        to end by the debugger.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: MMEngDebugIntf
// PREFIX: medbg
// ---------------------------------------------------------------------------
class CIDMACROENGEXP MMEngDebugIntf
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        MMEngDebugIntf(const MMEngDebugIntf&) = delete;

        ~MMEngDebugIntf();

        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        MMEngDebugIntf& operator=(const MMEngDebugIntf&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bSleepTest() const = 0;

        virtual tCIDLib::TVoid CallStackChange() = 0;

        virtual tCIDMacroEng::EDbgActs eAtLine
        (
            const   TMEngClassInfo&         meciCurrent
            , const TMEngMethodInfo&        methiCurrent
            , const TMEngOpMethodImpl&      methCurrent
            ,       TMEngClassVal&          mecvInstance
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TCard4         c4CurIP
        ) = 0;

        virtual tCIDLib::TVoid FinishedDbg
        (
            const   tCIDMacroEng::EDbgActs  eReason
        ) = 0;

        virtual tCIDLib::TVoid LocalsChange
        (
            const   tCIDLib::TBoolean       bCreated
        ) = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        MMEngDebugIntf();
};

#pragma CIDLIB_POPPACK


