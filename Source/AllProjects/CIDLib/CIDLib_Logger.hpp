//
// FILE NAME: CIDLib_Logger.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/27/1996
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
//  This file provides the abstraction of a destination for logged messages.
//  The TModule class supports logging via this abstract API. Any class that
//  wants to be a target for logged CIDLib logging must implement this
//  interface and plug itself in as the log handler.
//
//  Standard derivatives of this class are created to support logging to
//  various places. Higher level DLLs or Exes can override the default
//  logger in order to get control of logged messages, so they can be sent
//  to particular places.
//
// CAVEATS/GOTCHAS:
//
//  1)  Calls to LogMsg() are synchronized internally, so it can use static
//      resources to do the formatting and logging. However, if it logs to a
//      target that is available to multiple processes, it must provide its
//      own synchronization.
//
// LOG:
//
//  $_CIDLib_Log_$
//


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: MLogger
// PREFIX: mlgr
// ---------------------------------------------------------------------------
class CIDLIBEXP MLogger
{
    public  :
        // -------------------------------------------------------------------
        //  Destructor.
        // -------------------------------------------------------------------
        MLogger(const MLogger&) = delete;

        virtual ~MLogger() {}


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        MLogger& operator=(const MLogger&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid LogEvent
        (
            const   TLogEvent&              logevSrc
        ) = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        MLogger() = default;
};

#pragma CIDLIB_POPPACK
