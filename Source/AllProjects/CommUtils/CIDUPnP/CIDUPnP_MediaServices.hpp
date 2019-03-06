//
// FILE NAME: CIDUPnP_MediaServices.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/16/2011
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDUPnP_MediaServices.cpp file, which implements
//  some derivatives of the base service class that deal with media renderers,
//  AV Transport, etc...
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
//  CLASS: TUPnPTrackItem
// PREFIX: upnpti
// ---------------------------------------------------------------------------
class CIDUPNPEXP TUPnPTrackItem : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TUPnPTrackItem();

        TUPnPTrackItem
        (
            const   tCIDLib::TCard4         c4TrackNum
            , const TString&                strAlbum
            , const TString&                strArtist
            , const TString&                strTitle
            , const TString&                strArtURL
        );

        TUPnPTrackItem
        (
            const   TUPnPTrackItem&         upnptiToCopy
        );

        ~TUPnPTrackItem();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUPnPTrackItem& operator=
        (
            const   TUPnPTrackItem&         upnptiToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4TrackNum() const;

        tCIDLib::TVoid Reset();

        const TString& strAlbum() const;

        const TString& strArtist() const;

        const TString& strArtURL() const;

        const TString& strTitle() const;

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCard4         c4TrackNum
            , const TString&                strAlbum
            , const TString&                strArtist
            , const TString&                strTitle
            , const TString&                strArtURL
        );

        tCIDLib::TVoid SetFromXML
        (
            const   TXMLTreeElement&        xtnodeSrc
        );


    private :
        // -------------------------------------------------------------------
        //  Private class members
        //
        //  m_c4TrackNum
        //      The number of this track. If not available, we set it to 0.
        //
        //  m_strAlbum
        //  m_strArtist
        //  m_strArtURL
        //  m_strTitle
        //      The album, artist and title of this track.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4TrackNum;
        TString         m_strAlbum;
        TString         m_strArtist;
        TString         m_strArtURL;
        TString         m_strTitle;


        // -------------------------------------------------------------------
        //  Some commonly used strings of ours used in the XML parsing
        // -------------------------------------------------------------------
        static TString  s_strAlbum;
        static TString  s_strArtist;
        static TString  s_strArtURL;
        static TString  s_strID;
        static TString  s_strTitle;
        static TString  s_strTrackNum;
        static TString  s_strUntitled;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TUPnPTrackItem,TObject)
};



// ---------------------------------------------------------------------------
//  CLASS: TUPnPAVTransService
// PREFIX: upnps
// ---------------------------------------------------------------------------
class CIDUPNPEXP TUPnPAVTransService : public TUPnPService
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TUPnPAVTransService();

        TUPnPAVTransService(const TUPnPAVTransService&) = delete;

        ~TUPnPAVTransService();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUPnPAVTransService& operator=(const TUPnPAVTransService&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckServiceType
        (
            const   TString&                strTypeID
        )   const override;

        tCIDLib::TVoid UPnPVarChanged
        (
            const   tCIDLib::TCh* const     pszVarName
            , const tCIDLib::TCh* const     pszVarVal
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddURIToQueue
        (
            const   TString&                strURI
            , const TString&                strMetaData
            ,       tCIDLib::TCard4&        c4QueuedAtInd
            ,       tCIDLib::TCard4&        c4NewTrackCnt
            , const tCIDLib::TCard4         c4FirstTrInd = 0
            , const tCIDLib::TBoolean       bAsNext = kCIDLib::True
        );

        tCIDLib::TBoolean bCrossfadeMode() const;

        tCIDLib::TBoolean bCrossfadeMode
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TVoid BecomeStandaloneGrp();

        tCIDLib::TVoid ClearQueue();

        tCIDUPnP::EPlayModes ePlayMode() const;

        tCIDLib::TVoid Next();

        tCIDLib::TVoid Pause();

        tCIDLib::TVoid Play();

        tCIDLib::TVoid Previous();

        tCIDLib::TVoid QueryAVTransportURI
        (
                    TString&                strToFill
        );

        tCIDLib::TVoid QueryPositionInfo
        (
                    TString&                strTrackURI
            ,       tCIDLib::TEncodedTime&  enctDuration
            ,       tCIDLib::TCard4&        c4TrackNum
            ,       TUPnPTrackItem* const   pupnptiToFill = 0
        );

        tCIDLib::TVoid QueryPositionInfo
        (
                    TString&                strTrackURI
            ,       tCIDLib::TEncodedTime&  enctDuration
            ,       tCIDLib::TCard4&        c4TrackNum
            ,       tCIDLib::TEncodedTime&  enctCurTime
            ,       tCIDLib::TCard4&        c4PerComplete
            ,       TUPnPTrackItem* const   pupnptiToFill = 0
        );

        tCIDLib::TVoid QueryTransportInfo
        (
                    tCIDUPnP::ETransStates& eState
            ,       TString&                strStatus
            ,       tCIDLib::TCard4&        c4Spead
        );

        tCIDLib::TVoid QueryTransportSettings
        (
                    tCIDUPnP::EPlayModes&   ePlayMode
            ,       TString&                strRecQuality
        );

        tCIDLib::TVoid SaveQAsPlaylist
        (
            const   TString&                strName
            ,       TString&                strAssignedID
        );

        tCIDLib::TVoid SetAVTransportURI
        (
            const   TString&                strURI
            , const TString&                strArtist
            , const TString&                strTitle
            , const TString&                strItemName
            , const tCIDLib::TCard4         c4TrackNum
            , const tCIDLib::TCard4         c4Year
        );

        tCIDLib::TVoid SetAVTransportURI
        (
            const   TString&                strURI
            , const TString&                strMetaData
        );

        tCIDLib::TVoid SetPlayMode
        (
            const   tCIDUPnP::EPlayModes    eToSet
        );

        tCIDLib::TVoid StartAutoplay
        (
            const   TString&                strURI
            , const tCIDLib::TBoolean       bIncLinkedZones
            , const tCIDLib::TBoolean       bResetVolAfter = kCIDLib::False
            , const tCIDLib::TCard4         c4Volume = 0xFFFF
        );

        tCIDLib::TVoid Stop();


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SvcReset() override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bParseTimeVal
        (
            const   TString&                strTmVal
            ,       tCIDLib::TEncodedTime&  enctToSet
            , const TString&                strCmdName
            , const tCIDLib::TCard4         c4Line
            , const tCIDLib::TBoolean       bThrowIfBad = kCIDLib::False
        )   const;

        tCIDLib::TVoid ParseMetaData
        (
            const   TString&                strData
            ,       TUPnPTrackItem&         upnptiToFill
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bDoneXXX
        //      To be efficient, we don't want to continue polling evented values,
        //      but we won't get an event unless one changes. So, initially we have
        //      to query. But we then want to force those to event variables and
        //      then just wait for events for any further changes. For any things
        //      that we have to poll multiple values at once, we need to keep up
        //      with whether we've done the initial poll yet, so that we don't have
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bDonePosInfo;
        tCIDLib::TBoolean   m_bDoneTransInfo;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TUPnPAVTransService,TUPnPService)
};



// ---------------------------------------------------------------------------
//  CLASS: TUPnPContDirService
// PREFIX: upnps
// ---------------------------------------------------------------------------
class CIDUPNPEXP TUPnPContDirService : public TUPnPService
{
    public  :
        // -------------------------------------------------------------------
        //  Class types
        // -------------------------------------------------------------------
        using TTrackList = TCollection<TUPnPTrackItem>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TUPnPContDirService();

        TUPnPContDirService(const TUPnPContDirService&) = delete;

        ~TUPnPContDirService();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUPnPContDirService& operator=(const TUPnPContDirService&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckServiceType
        (
            const   TString&                strTypeID
        )   const override;

        tCIDLib::TVoid UPnPVarChanged
        (
            const   tCIDLib::TCh* const     pszVarName
            , const tCIDLib::TCh* const     pszVarVal
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Browse
        (
            const   TString&                strContID
            , const tCIDLib::TBoolean       bChildren
            , const tCIDLib::TCard4         c4StartAt
            , const tCIDLib::TCard4         c4Count
            ,       tCIDLib::TCard4&        c4Total
            ,       tCIDLib::TInt4&         i4UpdateID
            ,       TString&                strDIDL
        );

        tCIDLib::TCard4 c4BrowseFavorites
        (
            const   tCIDLib::TCard4         c4StartAt
            , const tCIDLib::TCard4         c4Count
            ,       tCIDLib::TCard4&        c4Total
            ,       tCIDLib::TKValsCollect& colToFill
            ,       tCIDLib::TInt4&         i4UpdateID
            , const tCIDLib::TBoolean       bAppend
        );

        tCIDLib::TCard4 c4BrowseFavoritesNames
        (
            const   tCIDLib::TCard4         c4StartAt
            , const tCIDLib::TCard4         c4Count
            ,       tCIDLib::TCard4&        c4Total
            ,       tCIDLib::TStrCollect&   colToFill
            ,       tCIDLib::TInt4&         i4UpdateID
            , const tCIDLib::TBoolean       bAppend
        );

        tCIDLib::TCard4 c4BrowseLists
        (
            const   TString&                strContID
            , const tCIDLib::TCard4         c4StartAt
            , const tCIDLib::TCard4         c4Count
            ,       tCIDLib::TCard4&        c4Total
            ,       tCIDLib::TKVPCollect&   colToFill
            ,       tCIDLib::TInt4&         i4UpdateID
            , const tCIDLib::TBoolean       bAppend
        );

        tCIDLib::TCard4 c4BrowseTracks
        (
            const   TString&                strContID
            , const tCIDLib::TCard4         c4StartAt
            , const tCIDLib::TCard4         c4Count
            ,       tCIDLib::TCard4&        c4Total
            ,       TTrackList&             colToFill
            ,       tCIDLib::TInt4&         i4UpdateID
            , const tCIDLib::TBoolean       bAppend
        );

        tCIDLib::TVoid DeleteObject
        (
            const   TString&                strObjectID
        );

        tCIDLib::TVoid QueryContUpdateID
        (
                    TString&                strToFill
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DoBrowse
        (
            const   TString&                strContID
            , const tCIDLib::TBoolean       bChildren
            , const tCIDLib::TCard4         c4StartAt
            , const tCIDLib::TCard4         c4ToGet
            ,       tCIDLib::TCard4&        c4RetCnt
            ,       tCIDLib::TCard4&        c4Total
            ,       tCIDLib::TInt4&         i4UpdateID
        );


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TUPnPContDirService,TUPnPService)
};




// ---------------------------------------------------------------------------
//  CLASS: TUPnPRendCtrlService
// PREFIX: upnps
// ---------------------------------------------------------------------------
class CIDUPNPEXP TUPnPRendCtrlService : public TUPnPService
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TUPnPRendCtrlService();

        TUPnPRendCtrlService(const TUPnPRendCtrlService&) = delete;

        ~TUPnPRendCtrlService();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUPnPRendCtrlService& operator=(const TUPnPRendCtrlService&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckServiceType
        (
            const   TString&                strTypeID
        )   const override;

        tCIDLib::TVoid UPnPVarChanged
        (
            const   tCIDLib::TCh* const     pszVarName
            , const tCIDLib::TCh* const     pszVarVal
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bQueryMute();

        tCIDLib::TCard4 c4QueryVolume();

        tCIDLib::TInt4 i4QueryVolumeDB();

        tCIDLib::TVoid QueryVolDBRange
        (
                    tCIDLib::TInt4&         i4MinVol
            ,       tCIDLib::TInt4&         i4MaxVol
        );

        tCIDLib::TVoid SetMute
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TVoid SetVolumeDB
        (
            const   tCIDLib::TInt4          i4ToSet
        );

        tCIDLib::TVoid SetVolume
        (
            const   tCIDLib::TCard4         c4ToSet
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SvcReset() override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bGotXXX
        //      This is used to avoid polling when we are evented. We'll set this
        //      either after we get an async event, or upon first poll if we are
        //      evented.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bGotMute;;
        tCIDLib::TBoolean   m_bGotVolume;;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TUPnPRendCtrlService,TUPnPService)
};


#pragma CIDLIB_POPPACK



