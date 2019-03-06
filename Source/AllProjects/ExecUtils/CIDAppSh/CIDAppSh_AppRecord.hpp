//
// FILE NAME: CIDAppSh_AppRecord.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/29/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the TAppShRecord class, which is what is used to
//  hold information about each application. Actually, we usually hold a
//  derived class, since each derived engine class can make us store extra
//  data.
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
//   CLASS: TAppShRecord
//  PREFIX: asr
// ---------------------------------------------------------------------------
class CIDAPPSHEXP TAppShRecord : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TAppShRecord
        (
            const   TString&                strTitle
            , const TString&                strApp
            , const TString&                strParms
            , const tCIDAppSh::EAppLevels   eLevel = tCIDAppSh::EAppLevels::Min
        );

        TAppShRecord
        (
            const   TString&                strTitle
            , const TString&                strApp
            , const TString&                strInitialPath
            , const TString&                strParms
            , const tCIDAppSh::EAppLevels   eLevel = tCIDAppSh::EAppLevels::Min
        );

        ~TAppShRecord();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsRunning() const;

        tCIDLib::TBoolean bStart();

        tCIDLib::TBoolean bStart
        (
            const   TEnvironment&           envToUse
        );

        tCIDLib::TBoolean bWaitForDeath
        (
                    tCIDLib::EExitCodes&    eToFill
            , const tCIDLib::TCard4         c4MilliSecs = kCIDLib::c4MaxWait
        )   const;

        tCIDLib::TCard4 c4ShutdownTime() const;

        tCIDLib::EExtProcFlags eFlags() const;

        tCIDLib::EExtProcFlags eFlags
        (
            const   tCIDLib::EExtProcFlags  eToSet
        );

        tCIDLib::EPrioClasses ePriorityClass() const;

        tCIDLib::EPrioClasses ePriorityClass
        (
            const   tCIDLib::EPrioClasses   eNewClass
        );

        tCIDAppSh::EAppLevels eLevel() const;

        tCIDAppSh::EStatus eStatus() const;

        tCIDAppSh::EStatus eStatus
        (
            const   tCIDAppSh::EStatus      eToSet
        );

        tCIDLib::EExitCodes eWaitForDeath
        (
            const   tCIDLib::TCard4         c4MilliSecs = kCIDLib::c4MaxWait
        )   const;

        TExternalProcess& extpThis();

        const TExternalProcess& extpThis() const;

        const TString& strApp() const;

        const TString& strExtra() const;

        const TString& strExtra
        (
            const   TString&                strToSet
        );

        const TString& strInitialPath() const;

        const TString& strParms() const;

        const TString& strTitle() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TAppShEngine;


        // -------------------------------------------------------------------
        //  protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ReplaceTokens
        (
                    TString&                strTarget
        );

        tCIDLib::TVoid StartShutdownClock();


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TAppShRecord();
        TAppShRecord(const TAppShRecord&);
        tCIDLib::TVoid operator=(const TAppShRecord&);


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4ShutdownTime
        //      When the start shutdown method is called, this is set to the
        //      start time. This let's us know how long we've been waiting
        //      for the shutdown to complete.
        //
        //  m_eFlags
        //      The external process flags to use. It defaults to a non-
        //      detached process that shares the same console.
        //
        //  m_eLevel
        //      In order to be able to vastly optimize shutting down the
        //      shell when there are apps that have no inter-dependencies,
        //      each app can be given a level value. Any apps with the
        //      same level are taken to have no interdependences, and only
        //      dependent on apps with a lower level. This allows for
        //      parallel shutdown of apps on the same level. By default
        //      they all have the minimum, which prevents any parallel
        //      activity.
        //
        //  m_eStatus
        //      The current active/stopped status of the record.
        //
        //  m_extpThis
        //      This is the external process object that we use to keep track
        //      of our process instance.
        //
        //  m_strApp
        //      The application. It can be fully qualified or relative, as
        //      long as it gets us to the app that needs to be run. Don't use
        //      any extensions. since they aren't portable. The extension will
        //      be provided as required.
        //
        //  m_strExtra
        //      An extra string that can be used in many cases to avoid having
        //      to create a derived class to store a little per-process data.
        //
        //  m_strInitialPath
        //      The initial path that is set before the app is started, so that
        //      it inherits that as its initial path.
        //
        //  m_strParms
        //      The parameters to pass to the application.
        //
        //  m_strTitle
        //      A short descriptive title for the app, for human consumption
        //      in messages or exceptions from the engine, since multiple
        //      instances of the same app can sometimes be running under the
        //      shell. This allows them to be differentiated.
        // -------------------------------------------------------------------
        tCIDAppSh::EAppLevels   m_eLevel;
        tCIDLib::TCard4         m_c4ShutdownTime;
        tCIDLib::EExtProcFlags  m_eFlags;
        tCIDAppSh::EStatus      m_eStatus;
        TExternalProcess        m_extpThis;
        TString                 m_strApp;
        TString                 m_strExtra;
        TString                 m_strInitialPath;
        TString                 m_strParms;
        TString                 m_strTitle;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TAppShRecord,TObject)
};

#pragma CIDLIB_POPPACK


