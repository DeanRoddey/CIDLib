//
// FILE NAME: CIDLib_DirChange.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/19/2018
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  These classes wrap the kernel level support for getting notifications when any
//  contents of a subdirectory (and optionally recursively so) changes.
//
//  The read operation is synchronous unless we go through the trouble of doing an
//  async I/O, which isn't worth it. So we just create a mixin class that will spawn
//  off a thread and call a virtual any time changes arrive.
//
//  We then done one implementation of that that just queues up the change events to be
//  read later, and another that will publish them using Pub/Sub. This means that when
//  we need to destroy the mixin, we have to do a cancel call and then wait for the
//  thread to complete, since it's always blocked.
//
//  We do it as a mixin since that would allow other stuff to directly process such
//  changes without any intermediary if they want. Or they can use our canned
//  implementations.
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
//   CLASS: MDirChangeMon
//  PREFIX: mdchm
// ---------------------------------------------------------------------------
class CIDLIBEXP MDirChangeMon
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        MDirChangeMon();

        MDirChangeMon(const MDirChangeMon&) = delete;

        ~MDirChangeMon();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        MDirChangeMon& operator=(const MDirChangeMon&) = delete;


        // -------------------------------------------------------------------
        //  Public method
        // -------------------------------------------------------------------
        const TString& strDirChangePath() const;


    protected :
        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bProcessDirChanges
        (
            const   tCIDLib::TDirChanges&   colNewChanges
            , const tCIDLib::TCard4         c4Count
        ) = 0;

        virtual tCIDLib::TVoid DirChangeOutOfSync() = 0;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StartDirMonitor
        (
            const   TString&                strTargetDir
            , const tCIDLib::EDirChFilters  eFilters
            , const tCIDLib::TBoolean       bDoSubDirs
        );

        tCIDLib::TVoid StopDirMonitor();


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eMonThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_kdchmImpl
        //      The underlying kernel level object that provides the actual
        //      implementation of our functionality. We keep it here instead of a
        //      local within the thread since we want to be able to easily return
        //      an error if starting the monitor process fails. So we set it up and
        //      then start the thread.
        //
        //  m_strDirChangePath
        //      If we are set up, this is the target directory we are monitoring.
        // -------------------------------------------------------------------
        TKrnlDirChangeMon   m_kdchmImpl;
        TString             m_strDirChangePath;
        TThread             m_thrMonitor;
};



// ---------------------------------------------------------------------------
//   CLASS: TDirChangeMon
//  PREFIX: dchm
// ---------------------------------------------------------------------------
class CIDLIBEXP TDirChangeMon : public MDirChangeMon
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDirChangeMon();

        TDirChangeMon(const TDirChangeMon&) = delete;

        ~TDirChangeMon();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDirChangeMon& operator=(const TDirChangeMon&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bReadChanges
        (
                    tCIDLib::TDirChanges&   colToFill
        );

        tCIDLib::TVoid Start
        (
            const   TString&                strTargetDir
            , const tCIDLib::EDirChFilters  eFilters
            , const tCIDLib::TBoolean       bDoSubDirs
        );

        tCIDLib::TVoid Stop();


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bProcessDirChanges
        (
            const   tCIDLib::TDirChanges&   colNewChanges
            , const tCIDLib::TCard4         c4Count
        )   override;

        tCIDLib::TVoid DirChangeOutOfSync() override;


    private :
        // -------------------------------------------------------------------
        //  Private data membres
        //
        //  m_bOverflow
        //      If our changes list is overflowed becasue the caller isn't reading
        //      them, we flush, set this flag, and put a single overflow event
        //      object into the list for the caller to see. We won't add any more
        //      events until the read this one, then we clear this flag.
        //
        //  m_mtxSync
        //      We have to sync access to m_colChanges, since it is filed by calls
        //      from the mixin's thread.
        //
        //  m_colChanges
        //      We just store reported changes here until calling code reads them.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bOverflow;
        TMutex                  m_mtxSync;
        tCIDLib::TDirChanges    m_colChanges;
};



// ---------------------------------------------------------------------------
//   CLASS: TDirChangePub
//  PREFIX: dchm
// ---------------------------------------------------------------------------
class CIDLIBEXP TDirChangePub  : public MDirChangeMon
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TDirChangePub();

        TDirChangePub(const TDirChangePub&) = delete;

        ~TDirChangePub();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Start
        (
            const   TString&                strTargetDir
            , const tCIDLib::EDirChFilters  eFilters
            , const tCIDLib::TBoolean       bDoSubDirs
            , const TString&                strPubTopic
        );

        tCIDLib::TVoid Stop();


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bProcessDirChanges
        (
            const   tCIDLib::TDirChanges&   colNewChanges
            , const tCIDLib::TCard4         c4Count
        )   override;

        tCIDLib::TVoid DirChangeOutOfSync() override;


    private :
        // -------------------------------------------------------------------
        //  Pviate data members
        //
        //  m_pstopChanges
        //      The topic we publish changes to. No need for any sync even though
        //      we are being called from a thread in the mixin. No one other than
        //      him ever uses this object, except during startup/shutdown when the
        //      other thread is not active.
        //
        //  m_strPubTopic
        //      The caller tells us what topic path he wants us to use.
        // -------------------------------------------------------------------
        TPubSubTopic        m_pstopChanges;
        TString             m_strPubTopic;
};


#pragma CIDLIB_POPPACK
