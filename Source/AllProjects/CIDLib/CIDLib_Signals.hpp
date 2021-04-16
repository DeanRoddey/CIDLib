//
// FILE NAME: CIDLib_Signals.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/25/1999
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
//  This is the header for the CIDLib_Signals.Cpp file, which implements
//  the TSignals namespace. This namespace provides access to signal support
//  services. Signal handler function can be added and removed to/from a list.
//  When signals occur, the stack is searched from the tail (the latest added
//  handler) to the head for handlers for that signal. Any which are found
//  are called. If they return kCIDLib::True then its assumed that did what
//  needs to be done and no more are called. Otherwise, the search continues
//  up the chain. If no user handler returns kCIDLib::True, then the default
//  system action is taken.
//
//  Normally, TSignals will not be used directly. Instead, the TSignalJanitor
//  will be used to apply a handler on a scoped basis. This is very safe and
//  insures that pushed handlers get removed. This class is a very simple
//  inline class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  This namespace provides access to signal related services
// ---------------------------------------------------------------------------
namespace TSignals
{
    CIDLIBEXP tCIDLib::TCard4 c4AddHandler
    (
                MSignalHandler* const   pmshToCall
    );

    CIDLIBEXP tCIDLib::TVoid GenerateCtrlCSignal();

    CIDLIBEXP tCIDLib::TVoid RemoveHandler
    (
                MSignalHandler* const   pmshToRemove
        , const tCIDLib::TCard4         c4SanityValue
    );
}


// ---------------------------------------------------------------------------
//   CLASS: TSignalJanitor
//  PREFIX: jan
// ---------------------------------------------------------------------------
class TSignalJanitor
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSignalJanitor
        (
                    MSignalHandler* const   pmshToAdd
        );

        TSignalJanitor(const TSignalJanitor&) = delete;
        TSignalJanitor(TSignalJanitor&&) = delete;

        ~TSignalJanitor();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSignalJanitor& operator=(const TSignalJanitor&) = delete;
        TSignalJanitor& operator=(TSignalJanitor&&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TSignalJanitor();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4HandlerId
        //      This is the hanler id given to us when we add the handler.
        //      We have to provide it back when we remove upon destruction.
        //
        //  m_pmshToAdd
        //      The signal handler object that was added. It must be kept in
        //      order to provide to the remove method.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4HandlerId;
        MSignalHandler* const   m_pmshToAdd;
};


#pragma CIDLIB_POPPACK

