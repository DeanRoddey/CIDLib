//
// FILE NAME: CIDLib_StdErrLogger.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/18/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDLib_StdErrLogger.Cpp file. This
//  file provides a simple implementation of MLogger, and supports logging
//  to std error.
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
//  CLASS: TStdErrLogger
// PREFIX: lgr
// ---------------------------------------------------------------------------
class CIDLIBEXP TStdErrLogger : public TObject, public MLogger
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TStdErrLogger();

        TStdErrLogger(const TStdErrLogger&) = delete;

        ~TStdErrLogger();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStdErrLogger& operator=(const TStdErrLogger&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid LogEvent
        (
            const   TLogEvent&              logevToLog
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TStdErrLogger,TObject)
};

#pragma CIDLIB_POPPACK

