//
// FILE NAME: CIDKernel_Time.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/20/1996
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
//  This is the header for the CIDKernel_Time.Cpp file, which implements
//  the TKrnlTimeStamp class. This class stores a time stamp as a 100
//  nano-second offset from a base time of midnight on 01/01/1970.
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
//   CLASS: TKrnlTimeStamp
//  PREFIX: ktms
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlTimeStamp
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const tCIDLib::TEncodedTime enctBaseOfs;
        static const tCIDLib::TEncodedTime enctNTPOfs;


        // -------------------------------------------------------------------
        //  Public static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bHighResTimerAvailable();

        static tCIDLib::TBoolean bHighResDelay
        (
            const   tCIDLib::TCard4         c4MicroSecs
        );

        static tCIDLib::TBoolean bIsDST
        (
                    tCIDLib::TBoolean&      bDSTState
        );

        static tCIDLib::TBoolean bTZOffset
        (
                    tCIDLib::TInt4&         i4ToFill
        );

        static tCIDLib::TBoolean bTZName
        (
                    tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        );

        static tCIDLib::TBoolean bIsLeapYear
        (
            const   tCIDLib::TCard4         c4Year
        );

        static tCIDLib::TBoolean bIsValidDate
        (
            const   tCIDLib::TCard4         c4Day
            , const tCIDLib::EMonths        eMonth
            , const tCIDLib::TCard4         c4Year
        );

        static tCIDLib::TBoolean bIsValidTime
        (
            const   tCIDLib::TCard4         c4Hours
            , const tCIDLib::TCard4         c4Minutes
            , const tCIDLib::TCard4         c4Seconds
            , const tCIDLib::TCard4         c4Hundredths
        );

        static tCIDLib::TBoolean bLocalToNative
        (
            const   tCIDLib::TEncodedTime&  enctLocal
            ,       tCIDLib::TEncodedTime&  enctNative
        );

        static tCIDLib::TBoolean bLocalToUTC
        (
            const   tCIDLib::TEncodedTime&  enctLocal
            ,       tCIDLib::TEncodedTime&  enctUTC
        );

        static tCIDLib::TBoolean bUTCToLocal
        (
            const   tCIDLib::TEncodedTime&  enctUTC
            ,       tCIDLib::TEncodedTime&  enctLocal
        );

        static tCIDLib::TCard4 c4MaxDayForMonth
        (
            const   tCIDLib::EMonths        eMonth
            , const tCIDLib::TCard4         c4Year
        );

        static tCIDLib::TCard4 c4Millis();

        static tCIDLib::TCard8 c8Millis();

        static tCIDLib::TVoid CurNTPTime
        (
                    tCIDLib::TCard4&        c4Secs
            ,       tCIDLib::TCard4&        c4Fract
            , const tCIDLib::TBoolean       bNWOrder
        );

        static tCIDLib::TEncodedTime enctNow();

        static tCIDLib::TEncodedTime enctNowUTC();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlTimeStamp() = default;

        TKrnlTimeStamp(const TKrnlTimeStamp&) = default;
        TKrnlTimeStamp(TKrnlTimeStamp&&) = default;

        ~TKrnlTimeStamp() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlTimeStamp& operator=(const TKrnlTimeStamp&) = default;
        TKrnlTimeStamp& operator=(TKrnlTimeStamp&&) = default;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bAdd
        (
            const   tCIDLib::TEncodedTime&  enctToAdd
        );

        tCIDLib::TBoolean bAsDateInfo
        (
                    tCIDLib::TCard4&        c4Year
            ,       tCIDLib::EMonths&       eMonth
            ,       tCIDLib::TCard4&        c4Day
            ,       tCIDLib::EWeekDays&     eWeekDay
        )   const;

        tCIDLib::TBoolean bAsTimeInfo
        (
                    tCIDLib::TCard4&        c4Hour
            ,       tCIDLib::TCard4&        c4Minute
            ,       tCIDLib::TCard4&        c4Second
            ,       tCIDLib::TCard4&        c4Millis
        )   const;

        tCIDLib::TBoolean bDayOfWeek
        (
                    tCIDLib::EWeekDays&     eToFill
        )   const;

        tCIDLib::TBoolean bExpandDetails
        (
                    tCIDLib::TCard4&        c4Year
            ,       tCIDLib::EMonths&       eMonth
            ,       tCIDLib::TCard4&        c4Day
            ,       tCIDLib::EWeekDays&     eWeekDay
            ,       tCIDLib::TCard4&        c4Hour
            ,       tCIDLib::TCard4&        c4Minute
            ,       tCIDLib::TCard4&        c4Second
            ,       tCIDLib::TCard4&        c4Millis
            ,       tCIDLib::TEncodedTime&  enctStamp
        )   const;

        tCIDLib::TBoolean bForwardByDays
        (
            const   tCIDLib::TCard4         c4Count
        );

        tCIDLib::TBoolean bForwardByMonths
        (
            const   tCIDLib::TCard4         c4Count
        );

        tCIDLib::TBoolean bForwardByYears
        (
            const   tCIDLib::TCard4         c4Count
        );

        tCIDLib::TBoolean bFromDetails
        (
            const   tCIDLib::TCard4         c4Year
            , const tCIDLib::EMonths        eMonth
            , const tCIDLib::TCard4         c4Day
            , const tCIDLib::TCard4         c4Hour      = 0
            , const tCIDLib::TCard4         c4Minute    = 0
            , const tCIDLib::TCard4         c4Second    = 0
            , const tCIDLib::TCard4         c4MilliSecs = 0
        );

        tCIDLib::TBoolean bFromTimeDetails
        (
            const   tCIDLib::TCard4         c4Hour
            , const tCIDLib::TCard4         c4Minute
            , const tCIDLib::TCard4         c4Second
            , const tCIDLib::TCard4         c4MilliSecs
        );

        tCIDLib::TBoolean bSetTo
        (
            const   tCIDLib::TEncodedTime&  enctNew
        );

        tCIDLib::TBoolean bSetTo
        (
            const   tCIDLib::ESpecialTimes  eSpecial
        );

        tCIDLib::TBoolean bSubtract
        (
            const   tCIDLib::TEncodedTime&  encToSubtract
        );

        tCIDLib::TBoolean bValidate() const;

        tCIDLib::TEncodedTime enctDifference
        (
            const   TKrnlTimeStamp&         ktmsToSubtract
        )   const;

        tCIDLib::TEncodedTime enctMilliSeconds() const;

        tCIDLib::TEncodedTime enctTime() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TCIDKrnlModule;


    private :
        // -------------------------------------------------------------------
        //  Private data
        //
        //  m_enctTime
        //      This is the time in the local time zone, which is the most
        //      convenient way to store it internally. Its in 100-nanosecond
        //      intervals since midnight 01/01/1970.
        // -------------------------------------------------------------------
        tCIDLib::TEncodedTime   m_enctTime = 0;
};

#pragma CIDLIB_POPPACK


