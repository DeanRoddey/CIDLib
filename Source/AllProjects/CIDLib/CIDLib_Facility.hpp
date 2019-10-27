//
// FILE NAME: CIDLib_Facility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/25/1996
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
//  This is the header file for the CIDLib_Facility.Cpp file, which implements
//  the basic facility class. Every facility defines a facility class and
//  instantiates a single instance of it. Simple facilities can just
//  instantiate a TFacility object, but more complex ones will create a
//  derivative with its own data and functionality.
//
//  This class builds upon the TModule class. TModule is the most fundamental
//  public representation of a DLL or Exe file, and provides almost all of
//  the functionality. This class just adds to that core funtionality the
//  ideas of a facility name and version number.
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
//  CLASS: TFacility
// PREFIX: fac
// ---------------------------------------------------------------------------
class CIDLIBEXP TFacility : public TModule
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::ESeverities eGlobalLogMode
        (
            const   tCIDLib::ESeverities    eSevToSet
        );

        static tCIDLib::ESeverities eGlobalLogMode();

        static TFacility* pfacFromName
        (
            const   TString&                strFacName
        );

        static TFacility& facFromName
        (
            const   TString&                strFacName
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TFacility() = delete;

        TFacility
        (
            const   TString&                strFacName
            , const tCIDLib::EModTypes      eModType
            , const tCIDLib::TCard4         c4MajVer
            , const tCIDLib::TCard4         c4MinVer
            , const tCIDLib::TCard4         c4Revision
            , const tCIDLib::EModFlags      eFlags
            , const tCIDLib::TBoolean       bLoad = kCIDLib::False
        );

        TFacility
        (
            const   TString&                strFacName
            , const TString&                strLoadPath
            , const tCIDLib::EModTypes      eModType
            , const tCIDLib::TCard4         c4MajVer
            , const tCIDLib::TCard4         c4MinVer
            , const tCIDLib::TCard4         c4Revision
            , const tCIDLib::EModFlags      eFlags
            , const tCIDLib::TBoolean       bLoad = kCIDLib::False
        );

        TFacility(const TFacility&) = delete;

        ~TFacility();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacility& operator=(const TFacility&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bLogFailures() const
        {
            return bShouldLogAt(tCIDLib::ESeverities::Failed);
        }

        tCIDLib::TBoolean bLogInfo() const
        {
            return bShouldLogAt(tCIDLib::ESeverities::Info);
        }

        tCIDLib::TBoolean bLogStatus() const
        {
            return bShouldLogAt(tCIDLib::ESeverities::Status);
        }

        tCIDLib::TBoolean bLogWarnings() const
        {
            return bShouldLogAt(tCIDLib::ESeverities::Warn);
        }

        tCIDLib::TBoolean bShouldLogAt
        (
            const   tCIDLib::ESeverities    eToCheck
        )   const;

        tCIDLib::TBoolean bShouldLog
        (
            const   TLogEvent&              logeToCheck
        )   const;

        tCIDLib::TCard4 c4MajVersion() const;

        tCIDLib::TCard4 c4MinVersion() const;

        tCIDLib::TCard4 c4Revision() const;

        tCIDLib::ESeverities eLogThreshold() const;

        const TString& strVersion() const;

        tCIDLib::TVoid SetLoggingFlags
        (
            const   tCIDLib::TCard8         c8ToSet
            , const tCIDLib::TCard8         c8Mask
        );

        tCIDLib::TVoid SetLogThreshold
        (
            const   tCIDLib::ESeverities    eToSet
        );

        tCIDLib::TVoid SetLogTrace
        (
            const   tCIDLib::TBoolean       bToSet
        );



    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bTraceErrs() const override;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckLogFlags
        (
            const   tCIDLib::ESeverities    eToCheck
            , const tCIDLib::TCard4         c4FlagBit
        )   const;

        tCIDLib::TBoolean bCheckLogFlags
        (
            const   TLogEvent&              logeToCheck
            , const tCIDLib::TCard4         c4FlagBit
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CommonInit
        (
            const   TString&                strFacName
            , const tCIDLib::TCard4         c4MajVer
            , const tCIDLib::TCard4         c4MinVer
            , const tCIDLib::TCard4         c4Revision
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4MajVer
        //  m_c4MinVer
        //      This facility's major and minor version. Minor version
        //      releases represent minor breaking changes, and major release
        //      represent wholesale improvements and changes.
        //
        //  m_c4Revision
        //      This facility's revision number. This is intended to track
        //      non-breaking changes, i.e. if the maj/min version is the
        //      same and only this value changes, it should be backwards
        //      compatible in all ways.
        //
        //  m_sciLogFlags
        //  m_sciLogThreshold
        //  m_sciLogTrace
        //      Some logging related stuff we store in the stats cache. The flags
        //      one is interpreted by the derived class, but we store it here to
        //      avoid lots of redundant code.
        //
        //  m_strVersion
        //      This is the version string, which is formatted during
        //      construction, using the provided version values.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4MajVer;
        tCIDLib::TCard4         m_c4MinVer;
        tCIDLib::TCard4         m_c4Revision;
        TStatsCacheItem         m_sciLogFlags;
        TStatsCacheItem         m_sciLogThreshold;
        TStatsCacheItem         m_sciLogTrace;
        TString                 m_strVersion;


        // -------------------------------------------------------------------
        //  Private, static data members
        //
        //  s_eGlobalLogMode
        //      This can be set to override on a global basis any of the per
        //      facility logging thresholds. So the bShouldLogAt() and eLogMode
        //      methods will take this if it's lower than the one for this
        //      module. Defaults to Failed, which is what most programs
        //      would want to use.
        // -------------------------------------------------------------------
        static tCIDLib::ESeverities s_eGlobalLogMode;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacility,TObject)
};

#pragma CIDLIB_POPPACK
