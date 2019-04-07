//
// FILE NAME: CIDLib_LogLimiter.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/08/2018
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
//  This is the header for the file CIDLib_LogLimiter.cpp file. This guy implements
//  a class that is commonly needed when there is thread that is constantly active,
//  such as a receiver thread for some server. If an error occurs we want to log it,
//  but such threads could in some bad cases get a constant stream of errors. So we
//  need a way to limit how much it can log.
//
//  You create an object of this type and pass it each exception caught. It will
//  decide, based on a threshold you set, whether to log the error or not. It will
//  keep with each unique error and how long since the last time it was logged.
//
//
// CAVEATS/GOTCHAS:
//
//  1)  Of course this guy CANNOT log errors, since that could get very self-
//      referential and cause problems. But it's very simple and shouldn't need to.
//
//  2)  THIS IS NOT threadsafe. It's for use within a thread's main loop primarily,
//      so another thread should not even be able to see it.
//
// LOG:
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TLogLimiter
//  PREFIX: loglim
// ---------------------------------------------------------------------------
class CIDLIBEXP TLogLimiter : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TLogLimiter
        (
            const   tCIDLib::TCard4         c4ThresholdSecs = 64
        );

        TLogLimiter(const TLogLimiter&) = delete;

        ~TLogLimiter();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TLogLimiter& operator=(const TLogLimiter&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bLogErr
        (
            const   TLogEvent&              errToLog
        );

        tCIDLib::TBoolean bLogErr
        (
                    TLogEvent&              errToLog
            , const tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        );


    private :
        // -------------------------------------------------------------------
        //  A small class we use to track exceptions
        // -------------------------------------------------------------------
        class TErrInfo
        {
            public :
                TErrInfo
                (
                    const   TLogEvent&              errToLog
                    , const tCIDLib::TEncodedTime   enctNext
                );

                ~TErrInfo();

                tCIDLib::TCard4         m_c4Line;
                tCIDLib::TEncodedTime   m_enctNext;
                tCIDLib::ESeverities    m_eSev;
                tCIDLib::TErrCode       m_errcLogged;
                TString                 m_strFacName;
                TString                 m_strFileName;
                TString                 m_strProcess;
        };


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bShouldLog
        (
            const   TLogEvent&                 errToLog
        );


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        //
        //  m_c4Seconds
        //      The minimum number of seconds between logging any given error.
        //
        //  m_colList
        //      The unique exceptions we've seen and info about them. We limit it
        //      to a reasonable size. If it gets full, we will empty the list and
        //      let it re-fill.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4Seconds;
        TVector<TErrInfo>   m_colList;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TLogLimiter,TObject)
};

#pragma CIDLIB_POPPACK

