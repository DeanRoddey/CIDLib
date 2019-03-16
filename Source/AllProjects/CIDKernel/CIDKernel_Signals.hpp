//
// FILE NAME: CIDKernel_Signals.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/25/1999
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the header for the CIDKernel_SignalHandler.Cpp file. This file
//  implements the TKrnlSignals namespace, which is the low level mechanism for
//  handling a small set of async signals that come from the system. It
//  supports a stack of handlers which can be pushed and popped back off. At
//  the CIDLib level, safe scope based push/pop is supported via a janitor
//  class.
//
//  TSignalCallback is an abstract interface via which handlers are called.
//  This allows classes to implement the interface and have their overridden
//  method called when signals come in.
//
//  Also defined here is TKrnlSignalJanitor, which is a signal install
//  janitor for use within the kernel. For public use, CIDLib defines a
//  higher level one, so this is just for within the kernel so its not
//  exported.
//
// CAVEATS/GOTCHAS:
//
//  1)  Installed handler objects are NOT adopted, because they are always
//      some other type of class into which MSignalHandler is mixed into.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//   CLASS: MSignalHandler
//  PREFIX: msh
// ---------------------------------------------------------------------------
class KRNLEXPORT MSignalHandler
{
    public :
        // -------------------------------------------------------------------
        //  Destructor. Constructors are all hidden
        // -------------------------------------------------------------------
        virtual ~MSignalHandler();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bHandleSignal
        (
            const   tCIDLib::ESignals       eSignal
        ) = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        MSignalHandler();
        MSignalHandler(const MSignalHandler&);
        tCIDLib::TVoid operator=(const MSignalHandler&);
};



// ---------------------------------------------------------------------------
//  The TKrnlSignals namespace
// ---------------------------------------------------------------------------
namespace TKrnlSignals
{
    // -----------------------------------------------------------------------
    //  Publically exported methods
    // -----------------------------------------------------------------------
    KRNLEXPORT tCIDLib::TBoolean bRemoveHandler
    (
                MSignalHandler* const       pmshToRemove
        , const tCIDLib::TCard4             c4HandlerId
    );

    KRNLEXPORT tCIDLib::TCard4 c4AddHandler
    (
                MSignalHandler* const       pmshToAdd
    );

    KRNLEXPORT tCIDLib::TVoid GenerateCtrlCSignal();


    // -----------------------------------------------------------------------
    //  These are for internal use, and are not exported
    // -----------------------------------------------------------------------
    tCIDLib::TBoolean bCallHandlers
    (
        const   tCIDLib::ESignals           eSignal
    );

    tCIDLib::TVoid PlatformInit();
}



// ---------------------------------------------------------------------------
//   CLASS: TKrnlSignalJanitor
//  PREFIX: jan
// ---------------------------------------------------------------------------
class TKrnlSignalJanitor
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlSignalJanitor() = delete;

        TKrnlSignalJanitor
        (
                    MSignalHandler* const   pmshToAdd
        );

        TKrnlSignalJanitor(const TKrnlSignalJanitor&) =delete;

        ~TKrnlSignalJanitor();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlSignalJanitor& operator=(const TKrnlSignalJanitor&) = delete;


    private :
        // -------------------------------------------------------------------
        //  private data members
        //
        //  m_c4HandlerId
        //      The id we are given upon installation and which we must give
        //      back when removing.
        //
        //  m_pmshToAdd
        //      The handler object that is added, and then removed.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4HandlerId;
        MSignalHandler*     m_pmshToAdd;
};


#pragma CIDLIB_POPPACK


