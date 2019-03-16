//
// FILE NAME: CIDUPnP_AsyncFinder.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 5/10/2012
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the header for the CIDUPnP_AsyncFinder.cpp file, which implements
//  a wrapper around the kernel level UPnP async device finder class.
//
//  For each device we store the user readable name, the universal id, and
//  the type URN. We use a key/values object for that. The UID is the key,
//  the name is value 1 and the type is value 2.
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
//  CLASS: TUPnPAsyncFinderItem
// PREFIX: upnpafi
//
//  We need a small class to keep up with the devices. We store some info
//  redudantly. It's in the actual device itself but we want to keep it for
//  fast access. It's just fo ruse within the finder, so we don't bother with
//  private members.
// ---------------------------------------------------------------------------
class CIDUPNPEXP TUPnPAsyncFinderItem
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strKey
        (
            const   TUPnPAsyncFinderItem&   upnpafiSrc
        );


        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TUPnPAsyncFinderItem
        (
            const   tCIDLib::TCh* const     pszUID
            , const tCIDLib::TCh* const     pszName
            , const tCIDLib::TCh* const     pszModel
            , const tCIDLib::TCh* const     pszType
            ,       TKrnlUPnPDevice* const  pkupnpDev
            , const tCIDLib::TCard4         c4SeqId
        );

        TUPnPAsyncFinderItem(const TUPnPAsyncFinderItem&) = delete;

        ~TUPnPAsyncFinderItem();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUPnPAsyncFinderItem& operator=(const TUPnPAsyncFinderItem&) = delete;


        // -------------------------------------------------------------------
        //  Public data members
        //
        //  m_c4SeqId
        //      Any time we add a new item to the finder's device list, or
        //      update an existing one, we set this to the next available
        //      sequential id. This let's the client code know if a newer
        //      version of a device he's using is available, by comparing the
        //      id of his current one to this value.
        //
        //  m_pkupnpdThis
        //      The device that was reported available. We own this guy and
        //      will destroy it when we are destroyed.
        //
        //  m_strXXX
        //      The sort of redudant but good to have around for fast access
        //      info about the device. The UID string is the key.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4SeqId;
        TKrnlUPnPDevice*    m_pkupnpdThis;
        TString             m_strName;
        TString             m_strModel;
        TString             m_strType;
        TString             m_strUID;
};


// ---------------------------------------------------------------------------
//  CLASS: TUPnPAsyncFinder
// PREFIX: upnpaf
// ---------------------------------------------------------------------------
class CIDUPNPEXP TUPnPAsyncFinder : public TObject, public MUPnPAsyncFinderCB
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TUPnPAsyncFinder();

        TUPnPAsyncFinder(const TUPnPAsyncFinder&) = delete;

        ~TUPnPAsyncFinder();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUPnPAsyncFinder& operator=(const TUPnPAsyncFinder&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDevIsAvailable
        (
            const   TString&                strUID
        )   const;

        tCIDLib::TBoolean bListComplete
        (
            const   TString&                strSearchID
        )   const;

        tCIDLib::TBoolean bQueryDevInfo
        (
            const   TString&                strUID
            ,       TString&                strName
            ,       TString&                strTypeURN
            ,       tCIDLib::TCard4&        c4SeqId
        )   const;

        tCIDLib::TBoolean bWaitListComplete
        (
            const   TString&                strSearchID
            , const tCIDLib::TCard4         c4WaitMillis
        )   const;


        tCIDLib::TCard4 c4SerialNum() const;

        tCIDLib::ELoadRes eSetupDevice
        (
            const   TString&                strUID
            ,       TUPnPDevice&            upnpdSetup
            ,       tCIDLib::TCard4&        c4SerialNum
        );

        tCIDLib::TVoid QueryDevList
        (
                    tCIDLib::TKValsCollect& colToFill
            ,       tCIDLib::TCard4&        c4SerialNum
        )   const;

        tCIDLib::TVoid StartSearch
        (
            const   TString&                strFindType
            , const TString&                strSearchName
        );

        tCIDLib::TVoid StopSearch
        (
            const   TString&                strSearchName
        );

        const TString& strName() const;

        const TString& strName
        (
            const   TString&                strToSet
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        //
        //  These are the callback methods that the kernel class calls.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDeviceArrived
        (
            const   tCIDLib::TCh* const     pszUID
            , const tCIDLib::TCh* const     pszName
            , const tCIDLib::TCh* const     pszModel
            , const tCIDLib::TCh* const     pszType
            ,       TKrnlUPnPDevice* const  pkupnpDev
        )   override;

        tCIDLib::TVoid DeviceRemoved
        (
            const   tCIDLib::TCh* const     pszUID

        )   override;

        tCIDLib::TVoid ListComplete
        (
            const   tCIDLib::TCh* const     pszSearchID
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private data types
        // -------------------------------------------------------------------
        using TSearchList   = tCIDLib::TKValsList;
        using TDevList      = TRefKeyedHashSet<TUPnPAsyncFinderItem, TString, TStringKeyOps>;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        const TKeyValues* pkvalsFindByName
        (
            const   TString&                strName
            ,       tCIDLib::TCard4&        c4FoundAt
            , const tCIDLib::TBoolean       bThrowIfNot
        )   const;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        //
        //  m_c4NextId
        //      This isused to assign unique ids to each newly arriving
        //      device from this finder. It never will be set to zero, so that
        //      the devices can initialize their seq id to zero to indicate
        //      not set up yet.
        //
        //  m_c4SerialNum
        //      Any time the list changes (add or remove), we bump this. this
        //      provides the client code with a quick way to see if needs to
        //      bother trying to update itself with new device info or not. it
        //      will never be zero, so client code can initialize their copy
        //      to zero to start, to insure they see the initial contents.
        //
        //      We don't reset it, just to be safe, even when we are stopped
        //      and restarted.
        //
        //  m_colDevList
        //      The list of currently available devices. This is a thread
        //      safe list since it's called from callbacks as well as the
        //      outside world.
        //
        //  m_colSearches
        //      We have to keep up with the open searches. We use a list of
        //      key/values objects. The key is the client code provided search
        //      name. The first value is the search id. The second value we
        //      use as a 'list complete' flag, setting it to "1" when the
        //      list is complete for a given search.
        //
        //      This one is locked using the device list mutex. The only
        //      reason this is needed is that the callbacks will update the
        //      'list complete' flag, so it is potentially being accessed by
        //      multiple threads.
        //
        //  m_kupnpafThis
        //      Our underlying kernel level async finder object that we work
        //      in terms of.
        //
        //  m_strName
        //      A name can be given to the finder object for use in error
        //      messages and logged messages.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4NextId;
        tCIDLib::TCard4         m_c4SerialNum;
        TDevList                m_colDevList;
        TSearchList             m_colSearches;
        TKrnlUPnPAsyncFinder    m_kupnpafThis;
        TString                 m_strName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TUPnPAsyncFinder,TObject)
};

#pragma CIDLIB_POPPACK


