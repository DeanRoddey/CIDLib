//
// FILE NAME: CIDUPnP_Device.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/15/2011
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
//  This is the header for the CIDUPnP_Device.cpp file, which implements
//  the class that represents a UPnP device. This is a pretty basic class,
//  which primarily exists to be able to get the associated services.
//
// CAVEATS/GOTCHAS:
//
//  1)  This class is set up by either the facilty class' synchronous finder
//      related methods, or by the async finder. So they are both friends
//      of this class in order to call the protected setup method.
//
//  2)  The async finder sets a sequence number on us, so that client code
//      can see if a newer version of the device has arrived. The sync
//      finder methods in the facility class just set it to zero. This must
//      always be set to zero though when we have no kernel level object
//      set on us.
//
//  3)  You can release a kernel object before this object destructs if
//      want, by calling Release(). Setting up this object for another
//      device URN will implicitly release any previous kernel object.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


class TFacCIDUPnP;
class TUPnPAsyncFinder;


// ---------------------------------------------------------------------------
//  CLASS: TUPnPDevice
// PREFIX: upnpd
// ---------------------------------------------------------------------------
class CIDUPNPEXP TUPnPDevice : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TUPnPDevice();

        TUPnPDevice(const TUPnPDevice&) = delete;

        ~TUPnPDevice();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUPnPDevice& operator=(const TUPnPDevice&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bHasChildren() const;

        tCIDLib::TBoolean bIsRoot() const;

        tCIDLib::TBoolean bQueryServiceIDsByType
        (
            const   TString&                strType
            ,       tCIDLib::TStrCollect&   colFound
        )   const;

        tCIDLib::TCard4 c4SeqId() const;

        tCIDLib::TVoid GetRootDevice
        (
                    TUPnPDevice&            upnpdToSet
        );

        tCIDLib::TVoid QueryChildDevices
        (
                    tCIDLib::TKValsCollect& colToFill
            , const TString&                strTypeFilter
        )   const;

        tCIDLib::TVoid QueryDeviceURL
        (
                    TString&                strToFill
        )   const;

        tCIDLib::TVoid QueryDevDescrURL
        (
                    TString&                strToFill
        )   const;

        tCIDLib::TVoid QueryManufacturer
        (
                    TString&                strToFill
        )   const;

        tCIDLib::TVoid QueryServices
        (
                    TCollection<TUPnPService>& colFound
        );

        tCIDLib::TVoid QueryServices
        (
                    tCIDLib::TKVPCollect&   colFound
        )   const;

        tCIDLib::TVoid Release();

        const TString& strModel() const;

        const TString& strName() const;

        const TString& strType() const;

        const TString& strUID() const;

        tCIDLib::TVoid SetServiceFromID
        (
            const   TString&                strSvcID
            ,       TUPnPService&           upnpsToSet
            ,       MUPnPSvcCallback* const pupnpscbHandler = 0
        );


    protected :
        // -------------------------------------------------------------------
        //  These guys need to be able to call SetKrnlObj
        // -------------------------------------------------------------------
        friend class TFacCIDUPnP;
        friend class TUPnPAsyncFinder;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bParseDevice
        (
            const   TXMLTreeElement&        xtnodeCurDev
            ,       TString&                strTmp
        )   const;

        tCIDLib::TVoid CheckDevSet(const tCIDLib::TCard4 c4Line) const;

        tCIDLib::TVoid LoadServices() const;

        tCIDLib::TVoid ParseDeviceDescr
        (
            const   TMemBuf&                mbufContent
            , const tCIDLib::TCard4         c4ContLen
            , const TString&                strEncoding
        )   const;

        tCIDLib::TVoid ParseServiceList
        (
            const   TXMLTreeElement&        xtnodeDev
            ,       TString&                strTmp
        )   const;

        tCIDLib::TVoid SetKrnlObj
        (
                    TKrnlUPnPDevice* const  pkupnpdToAdopt
            , const tCIDLib::TCh* const     pszUID
            , const tCIDLib::TCh* const     pszName
            , const tCIDLib::TCh* const     pszModel
            , const tCIDLib::TCh* const     pszType
            , const tCIDLib::TCard4         c4SeqId
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bServicesLoaded
        //      Used to fault in the m_colServices list. It's mutable because we
        //      have to fault it in from possibly const methods.
        //
        //  m_c4SeqId
        //      When used with the async finder, it will set a unique sequence
        //      id on each new device that arrives. When the client code gets
        //      the finder to set up a device, the finder calls SetKrnlObj and
        //      will pass in the Id associated with this device. The client
        //      code can use this to check whether the device has been updated
        //      and update itself. Other code will likely just pass a zero
        //      here. And we must keep it zero any time we are not set up with
        //      a kernel level device object.
        //
        //      NOTE we cannot go by the kernel device object pointer being
        //      different because we are using our own copy.
        //
        //  m_colServices
        //      We don't use the Windows service iteration scheme. It's not reliable.
        //      So we read the dvice document and then look up the services and save
        //      their info here. The values are:
        //
        //          Key  = Service Type URN
        //          Value = Service ID
        //
        //      There are more ids/URLS associated with services, but this is all we
        //      need for iteration. The type URN for lookup and the service id to
        //      create the service object. Once the caller has the service object,
        //      he can get any other info he needs.
        //
        //     It's mutable because we have to fault it in from possibly const
        //      methods.
        //
        //  m_pkupnpdThis
        //      The underlying kernel level UPnP device object that we do our
        //      work through. We are responsible for cleaning it up.
        //
        //  m_strModel
        //  m_strName
        //  m_strType
        //  m_strUID
        //      When we are set up with a device, we get this info to keep
        //      around for access and use in error messages and such. This way
        //      if something goes wrong and the underlying device doesn't work
        //      we still have this info.
        // -------------------------------------------------------------------
        mutable tCIDLib::TBoolean   m_bServicesLoaded;
        tCIDLib::TCard4             m_c4SeqId;
        mutable tCIDLib::TKVPList   m_colServices;
        TKrnlUPnPDevice*            m_pkupnpdThis;
        TString                     m_strModel;
        TString                     m_strName;
        TString                     m_strType;
        TString                     m_strUID;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TUPnPDevice,TObject)
};


// Define some lists of devices
namespace tCIDUPnP
{
    using TDevCollect   = TCollection<TUPnPDevice>;
    using TDevList      = TVector<TUPnPDevice>;
}

#pragma CIDLIB_POPPACK


