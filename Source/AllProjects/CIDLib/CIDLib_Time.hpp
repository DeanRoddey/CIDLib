//
// FILE NAME: CIDLib_Time.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/05/1993
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
//  This file implements the TTime class. TTime represents a time stamp
//  that extends from 01/01/1970. Its raw form, gotten via enctTime(), is the
//  number of 100 nanosecond intervals since that base time.
//
// CAVEATS/GOTCHAS:
//
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TTime
//  PREFIX: tm
// ---------------------------------------------------------------------------
class CIDLIBEXP TTime :

    public TObject, public MDuplicable, public MFormattable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        //  Public class types
        // -------------------------------------------------------------------
        using TCompList = TFundVector<tCIDLib::ETimeComps>;


        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bDST();

        static tCIDLib::TBoolean bIsLeapYear
        (
            const   tCIDLib::TCard4         c4ToCheck
        );

        static tCIDLib::TCard4 c4CvtToJulian
        (
            const   tCIDLib::TCard4         c4Year
            , const tCIDLib::EMonths        eMonth
            , const tCIDLib::TCard4         c4Day
        );

        static tCIDLib::TCard4 c4MaxDaysForMonth
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

        static tCIDLib::TEncodedTime enctNowPlusDays
        (
            const   tCIDLib::TCard4         c4Days
        );

        static tCIDLib::TEncodedTime enctNowPlusHours
        (
            const   tCIDLib::TCard4         c4Hours
        );

        static tCIDLib::TEncodedTime enctNowPlusMSs
        (
            const   tCIDLib::TCard4         c4Millis
        );

        static tCIDLib::TEncodedTime enctNowPlusMins
        (
            const   tCIDLib::TCard4         c4Mins
        );

        static tCIDLib::TEncodedTime enctNowPlusSecs
        (
            const   tCIDLib::TCard4         c4Secs
        );

        static tCIDLib::TEncodedTime enctNowUTC();

        static tCIDLib::TVoid CvtFromJulian
        (
            const   tCIDLib::TCard4         c4Julian
            ,       tCIDLib::TCard4&        c4Year
            ,       tCIDLib::EMonths&       eMonth
            ,       tCIDLib::TCard4&        c4Day
        );

        static tCIDLib::TVoid CvtFromJulian
        (
            const   tCIDLib::TFloat8        f8Julian
            ,       tCIDLib::TCard4&        c4Year
            ,       tCIDLib::EMonths&       eMonth
            ,       tCIDLib::TCard4&        c4Day
        );

        static const TCompList& fcol12HHMM();

        static const TCompList& fcol24HHMM();

        static const TCompList& fcol24HHMMSS();

        static const TCompList& fcolCTime();

        static const TCompList& fcolDDMMYY();

        static const TCompList& fcolDDMMYYYY();

        static const TCompList& fcolDTStamp();

        static const TCompList& fcolFullDate();

        static const TCompList& fcolISO8601NTZ();

        static const TCompList& fcolMMDDYY();

        static const TCompList& fcolMMDDYYYY();

        static const TCompList& fcolRFC822();

        static const TCompList& fcolYYMMDD();

        static const TCompList& fcolYYYYMMDD();

        static const TCompList& fcolYYYYMMDD24HHMM();

        static tCIDLib::TInt4 i4Offset();

        static tCIDLib::TVoid LocalToNative
        (
            const   tCIDLib::TEncodedTime&  enctUTC
            ,       tCIDLib::TEncodedTime&  enctNative
        );

        static tCIDLib::TVoid LocalToUTC
        (
            const   tCIDLib::TEncodedTime&  enctLocal
            ,       tCIDLib::TEncodedTime&  enctUTC
        );

        static const TString& str24HHMM();

        static const TString& str24HHMMSS();

        static const TString& str24HM();

        static const TString& strCTime();

        static const TString& strDDMMYY();

        static const TString& strDDMMYYYY();

        static const TString& strDTStamp();

        static const TString& strFullDate();

        static const TString& strHM();

        static const TString& strHHMM();

        static const TString& strHHMMap();

        static const TString& strHHMMSS();

        static const TString& strMediaTime();

        static const TString& strMMDD_24HHMM();

        static const TString& strMMDD_24HHMMSS();

        static const TString& strMMDDYY();

        static const TString& strMMDDYYYY();

        static const TString& strMMDD_HHMM();

        static const TString& strMMDD_HHMMSS();

        static const TString& strMMSS();

        static const TString& strISO8601Basic();

        static const TString& strTimeZoneName();

        static const TString& strYYMMDD();

        static const TString& strYYYYMMDD();

        static const TString& strYYYYMMDD_NoSep();

        static tCIDLib::TVoid CalcRiseSetTimes
        (
            const   TTime&                  tmOn
            , const tCIDLib::TFloat8        f8Latitude
            , const tCIDLib::TFloat8        f8Longitude
            ,       tCIDLib::TCard4&        c4RiseHour
            ,       tCIDLib::TCard4&        c4RiseMin
            ,       tCIDLib::TCard4&        c4SetHour
            ,       tCIDLib::TCard4&        c4SetMin
        );

        static tCIDLib::TVoid CalcRiseSetTimes
        (
            const   TTime&                  tmOn
            , const tCIDLib::TFloat8        f8TZOffset
            , const tCIDLib::TFloat8        f8Latitude
            , const tCIDLib::TFloat8        f8Longitude
            ,       tCIDLib::TCard4&        c4RiseHour
            ,       tCIDLib::TCard4&        c4RiseMin
            ,       tCIDLib::TCard4&        c4SetHour
            ,       tCIDLib::TCard4&        c4SetMin
        );

        static tCIDLib::TVoid CalcRiseSetTimes
        (
            const   tCIDLib::TCard4         c4Year
            , const tCIDLib::EMonths        eMonth
            , const tCIDLib::TCard4         c4Day
            , const tCIDLib::TFloat8        f8TZOffset
            , const tCIDLib::TFloat8        f8Latitude
            , const tCIDLib::TFloat8        f8Longitude
            ,       tCIDLib::TCard4&        c4RiseHour
            ,       tCIDLib::TCard4&        c4RiseMin
            ,       tCIDLib::TCard4&        c4SetHour
            ,       tCIDLib::TCard4&        c4SetMin
        );

        static tCIDLib::TVoid UTCToLocal
        (
            const   tCIDLib::TEncodedTime&  enctUTC
            ,       tCIDLib::TEncodedTime&  enctLocal
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTime();

        TTime
        (
            const   tCIDLib::ESpecialTimes  eSpecial
        );

        TTime
        (
            const   TTime&                  tmToCopy
        );

        TTime
        (
            const   tCIDLib::TEncodedTime   enctTime
        );

        TTime
        (
            const   tCIDLib::TCard4         c4Year
            , const tCIDLib::EMonths        eMonth
            , const tCIDLib::TCard4         c4Day
            , const tCIDLib::TCard4         c4Hours = 0UL
            , const tCIDLib::TCard4         c4Minutes = 0UL
            , const tCIDLib::TCard4         c4Seconds = 0UL
            , const tCIDLib::TCard4         c4Millis = 0UL
        );

        ~TTime();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TBoolean operator==
        (
            const   TTime&                  tmToCompare
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TTime&                  tmToCompare
        )   const;

        TTime& operator=
        (
            const   TTime&                  tmToAssign
        );

        TTime& operator=
        (
            const   tCIDLib::TEncodedTime&  enctToSet
        );

        tCIDLib::TVoid operator+=
        (
            const   TTime&                  tmToAdd
        );

        tCIDLib::TVoid operator+=
        (
            const   tCIDLib::TEncodedTime&  enctToAdd
        );

        tCIDLib::TVoid operator-=
        (
            const   TTime&                  tmToSubtract
        );

        tCIDLib::TVoid operator-=
        (
            const   tCIDLib::TEncodedTime&  enctToSubtract
        );

        friend TTime CIDLIBEXP operator+
        (
            const   TTime&                  tmLHS
            , const TTime&                  tmRHS
        );

        friend TTime CIDLIBEXP operator-
        (
            const   TTime&                  tmLHS
            , const TTime&                  tmRHS
        );

        friend tCIDLib::TBoolean CIDLIBEXP operator<
        (
            const   TTime&                  tmLHS
            , const TTime&                  tmRHS
        );

        friend tCIDLib::TBoolean CIDLIBEXP operator<=
        (
            const   TTime&                  tmLHS
            , const TTime&                  tmRHS
        );

        friend tCIDLib::TBoolean CIDLIBEXP operator>
        (
            const   TTime&                  tmLHS
            , const TTime&                  tmRHS
        );

        friend tCIDLib::TBoolean CIDLIBEXP operator>=
        (
            const   TTime&                  tmLHS
            , const TTime&                  tmRHS
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bInPM() const;

        tCIDLib::TBoolean bParseFrom8601
        (
            const   TString&                strToParse
            , const tCIDLib::TBoolean       bAsLocal = kCIDLib::False
        );

        tCIDLib::TBoolean bParseFromText
        (
            const   TString&                strSrc
            , const TCompList&              fcolCompList
            , const tCIDLib::TCh            chDateSep = 0
            , const tCIDLib::TCh            chTimeSep = 0
            , const tCIDLib::TCh            chTZSep = 0
        );

        tCIDLib::TCard4 c4AsTimeInfo
        (
                    tCIDLib::TCard4&        c4Hour
            ,       tCIDLib::TCard4&        c4Minute
            ,       tCIDLib::TCard4&        c4Second
        )   const;

        tCIDLib::TCard4 c4ToJulian() const;

        tCIDLib::TVoid ClipToHour();

        tCIDLib::TVoid ClipToMinute();

        tCIDLib::TVoid ClipToSecond();

        tCIDLib::EWeekDays eAsDateInfo
        (
                    tCIDLib::TCard4&        c4Year
            ,       tCIDLib::EMonths&       eMonth
            ,       tCIDLib::TCard4&        c4Day
        )   const;

        tCIDLib::EWeekDays eDayOfWeek() const;

        tCIDLib::EWeekDays eExpandDetails
        (
                    tCIDLib::TCard4&        c4Year
            ,       tCIDLib::EMonths&       eMonth
            ,       tCIDLib::TCard4&        c4Day
            ,       tCIDLib::TCard4&        c4Hour
            ,       tCIDLib::TCard4&        c4Minute
            ,       tCIDLib::TCard4&        c4Second
            ,       tCIDLib::TCard4&        c4Millis
            ,       tCIDLib::TEncodedTime&  enctStamp
        );

        tCIDLib::TEncodedTime enctDifference
        (
            const   TTime&                  tmToSubtract
        )   const;

        tCIDLib::TEncodedTime enctMilliSeconds() const;

        tCIDLib::TEncodedTime enctTime() const;

        tCIDLib::TEncodedTime enctTime
        (
            const   tCIDLib::TEncodedTime&  enctToSet
        );

        tCIDLib::TVoid FormatAsElapsed
        (
                    TString&                strToWriteTo
            , const tCIDLib::TCh            chSepChar
            , const tCIDLib::TBoolean       bShowDays
        )   const;

        tCIDLib::TVoid FormatAsElapsed2
        (
                    TString&                strToWriteTo
            , const tCIDLib::TCh            chSepChar
            , const tCIDLib::TBoolean       bShowHours
        )   const;

        tCIDLib::TVoid FormatToStr
        (
                    TString&                strToWriteTo
            , const tCIDLib::EAppendOver    eAppend = tCIDLib::EAppendOver::Overwrite
        )   const;

        tCIDLib::TVoid FormatToStr
        (
                    TString&                strToWriteTo
            , const TString&                strFormat
            , const tCIDLib::EAppendOver    eAppend = tCIDLib::EAppendOver::Overwrite
        )   const;

        tCIDLib::TVoid FromDetails
        (
            const   tCIDLib::TCard4         c4Year
            , const tCIDLib::EMonths        eMonth
            , const tCIDLib::TCard4         c4Day
            , const tCIDLib::TCard4         c4Hour      = 0
            , const tCIDLib::TCard4         c4Minute    = 0
            , const tCIDLib::TCard4         c4Second    = 0
            , const tCIDLib::TCard4         c4MilliSecs = 0
        );

        tCIDLib::TVoid FromJulian
        (
            const   tCIDLib::TCard4         c4Julian
        );

        tCIDLib::TVoid FromJulian
        (
            const   tCIDLib::TFloat8        cf8Julian
        );

        tCIDLib::TVoid FromTimeDetails
        (
            const   tCIDLib::TCard4         c4Hour
            , const tCIDLib::TCard4         c4Minute
            , const tCIDLib::TCard4         c4Second
            , const tCIDLib::TCard4         c4MilliSecs
        );

        tCIDLib::TVoid ForwardByDays
        (
            const   tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid ForwardByMonths
        (
            const   tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid ForwardByYears
        (
            const   tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid LocalToUTC();

        tCIDLib::TVoid ParseFromText
        (
            const   TString&                strSrc
            , const TCompList&              fcolCompList
            , const tCIDLib::TCh            chDateSep = 0
            , const tCIDLib::TCh            chTimeSep = 0
            , const tCIDLib::TCh            chTZSep = 0
        );

        tCIDLib::TVoid SetToNow();

        tCIDLib::TVoid SetToNowPlusDays
        (
            const   tCIDLib::TCard4         c4Days
        );

        tCIDLib::TVoid SetToNowPlusHours
        (
            const   tCIDLib::TCard4         c4Hours
        );

        tCIDLib::TVoid SetToNowPlusMSs
        (
            const   tCIDLib::TCard4         c4Millis
        );

        tCIDLib::TVoid SetToNowPlusMins
        (
            const   tCIDLib::TCard4         c4Mins
        );

        tCIDLib::TVoid SetToNowPlusSecs
        (
            const   tCIDLib::TCard4         c4Secs
        );

        tCIDLib::TVoid SetTo
        (
            const   tCIDLib::ESpecialTimes  eSpecial
        );

        tCIDLib::TVoid SetToSunrise
        (
            const   tCIDLib::TFloat8        f8Latitude
            , const tCIDLib::TFloat8        f8Longitude
        );

        tCIDLib::TVoid SetToSunset
        (
            const   tCIDLib::TFloat8        f8Latitude
            , const tCIDLib::TFloat8        f8Longitude
        );

        tCIDLib::TVoid SetToSunriseAt
        (
            const   tCIDLib::TFloat8        f8Latitude
            , const tCIDLib::TFloat8        f8Longitude
            , const tCIDLib::TInt4          i4TZOffset
        );

        tCIDLib::TVoid SetToSunsetAt
        (
            const   tCIDLib::TFloat8        f8Latitude
            , const tCIDLib::TFloat8        f8Longitude
            , const tCIDLib::TInt4          i4TZOffset
        );

        const TString& strDefaultFormat() const;

        const TString& strDefaultFormat
        (
            const   TString&                strNewFormat
        );

        tCIDLib::TVoid UTCToLocal();


    protected   :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmToWriteTo
        )   const override;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        ) override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid LazyInit();

        tCIDLib::TVoid StoreValue
        (
            const   tCIDLib::TEncodedTime&  enctToStore
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_ktmsThis
        //      This is the platform time stamp wrapper object that contains
        //      the actual time stamp info.
        //
        //  m_pstrDefFormat
        //      This is the formatting option to use when formating the
        //      object via the standard formatting operators (which don't
        //      allow any class specific parameters.) Its only allocated
        //      as required. If none has been provided, then the default
        //      locale format is used.
        // -------------------------------------------------------------------
        TKrnlTimeStamp      m_ktmsThis;
        mutable TString*    m_pstrDefFormat;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTime,TObject)
        DefPolyDup(TTime)
};

// Make the most common case easy
inline tCIDLib::TVoid TTime::SetToNow()
{
    SetTo(tCIDLib::ESpecialTimes::CurrentTime);
}

#pragma CIDLIB_POPPACK

