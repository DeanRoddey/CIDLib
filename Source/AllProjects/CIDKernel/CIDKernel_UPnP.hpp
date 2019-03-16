//
// FILE NAME: CIDKernel_UPnP.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/10/2011
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
//  This is the header for the CIDKernel_UPnP.cpp module. This module implements
//  a few classes that provide access to UPnP services. Currently there is support
//  client to UPnP Control Points.
//
//  The CIDUPnP facility that exposes this stuff publically basically provies
//  equivalent classes. But his classes use value semantics so we provide
//  reference counting methods on our services and devices to help support
//  that. The finder class doesn't since it's hidden within the CIDUPnP facility
//  class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TKrnlUPnPFinder;
class TKrnlUPnPDevice;
class CUPnPFinderCallback;



// ---------------------------------------------------------------------------
//   CLASS: MUPnPSvcCallback
//  PREFIX: upnpscb
//
//  Classes that create services are able to install themselves as callback
//  handlers for changes that are reported on those services. THey must
//  implement this mixing and set themselves on the service.
//
//  THEY MUST NOT go away while the service object exists.
// ---------------------------------------------------------------------------
class KRNLEXPORT MUPnPSvcCallback
{
    public :
        // -------------------------------------------------------------------
        //  Destructor. Constructors are all hidden
        // -------------------------------------------------------------------
        virtual ~MUPnPSvcCallback();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid UPnPServiceDied() = 0;

        virtual tCIDLib::TVoid UPnPVarChanged
        (
            const   tCIDLib::TCh* const     pszVarName
            , const tCIDLib::TCh* const     pszVarVal
        ) = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        MUPnPSvcCallback();
        MUPnPSvcCallback(const MUPnPSvcCallback&);
        MUPnPSvcCallback& operator=(const MUPnPSvcCallback&);
};



// ---------------------------------------------------------------------------
//   CLASS: TKrnlUPnPService
//  PREFIX: kupnps
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlUPnPService
{
    public  :
        // -------------------------------------------------------------------
        //  Public Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlUPnPService() = delete;

        TKrnlUPnPService
        (
                    tCIDLib::TVoid* const   pData
        );

        TKrnlUPnPService(const TKrnlUPnPService&) = delete;

        ~TKrnlUPnPService();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlUPnPService& operator=(const TKrnlUPnPService&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddCallback
        (
                    MUPnPSvcCallback*       pupnpscbTar
        );

        tCIDLib::TBoolean bQueryServiceType
        (
                    TKrnlString&            kstrToFill
        )   const;

        tCIDLib::TBoolean bQueryStateVar
        (
            const   tCIDLib::TCh* const     pszVarName
            ,       TKrnlString&            kstrVal
        )   const;

        tCIDLib::TBoolean bInvokeAct
        (
            const   tCIDLib::TCh* const     pszCmdName
            , const tCIDLib::TCh*           apszInParms[]
            , const tCIDLib::TCard4         c4ParmCnt
            ,       TKrnlLList<TKrnlString>& kllstOutVals
            ,       TKrnlString&            kstrResStr
        )   const;

        TKrnlUPnPService* pkupnpsClone();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pData
        //      An opaque pointer to that the OS implementation can point at
        //      whatever it needs to track.
        // -------------------------------------------------------------------
        tCIDLib::TVoid*     m_pData;
};



// ---------------------------------------------------------------------------
//   CLASS: TKrnlUPnPDevice
//  PREFIX: kupnpd
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlUPnPDevice
{
    public  :
        // -------------------------------------------------------------------
        //  Public Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlUPnPDevice() = delete;

        TKrnlUPnPDevice
        (
                    tCIDLib::TVoid* const   pDevInfo
        );

        TKrnlUPnPDevice(const TKrnlUPnPDevice&) = delete;

        ~TKrnlUPnPDevice();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlUPnPDevice& operator=(const TKrnlUPnPDevice&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bGetRootDevice
        (
                    TKrnlUPnPDevice*&       pkupnpdToFill
        );

        tCIDLib::TBoolean bHasChildren
        (
                    tCIDLib::TBoolean&      bToFill
        )   const;

        tCIDLib::TBoolean bIsRoot
        (
                    tCIDLib::TBoolean&      bToFill
        )   const;

        tCIDLib::TBoolean bQueryChildDevices
        (
                    TKrnlLList<TKrnlKVPair>& kllistFound
            , const tCIDLib::TCh* const     pszOfType = 0
        )   const;

        tCIDLib::TBoolean bQueryDevDescrURL
        (
                    TKrnlString&            kstrToFill
        )   const;

        tCIDLib::TBoolean bQueryManufacturer
        (
                    TKrnlString&            kstrToFill
        )   const;

        tCIDLib::TBoolean bQueryServiceByID
        (
            const   tCIDLib::TCh* const     pszServiceID
            ,       TKrnlUPnPService*&      pkupnpstoFill
        );

        tCIDLib::TBoolean bQueryDevInfo
        (
                    TKrnlString&            kstrUID
            ,       TKrnlString&            kstrName
            ,       TKrnlString&            kstrModel
            ,       TKrnlString&            kstrType
        )   const;

        TKrnlUPnPDevice* pkupnpdClone();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pData
        //      This is an opaque pointer to whatever information the per-
        //      platform implementation needs to maintain.
        // -------------------------------------------------------------------
        tCIDLib::TVoid* m_pData;
};



// ---------------------------------------------------------------------------
//   CLASS: TKrnlUPnPFinder
//  PREFIX: kupnpf
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlUPnPFinder
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlUPnPFinder();

        TKrnlUPnPFinder(const TKrnlUPnPFinder&) = delete;

        ~TKrnlUPnPFinder();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlUPnPFinder& operator=(const TKrnlUPnPFinder&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bSearchByType
        (
            const   tCIDLib::TCh* const     pszType
            ,       TKrnlLList<TKrnlKVPair>& kllistFound
        );

        tCIDLib::TBoolean bSearchByUID
        (
            const   tCIDLib::TCh* const     pszUID
            ,       TKrnlUPnPDevice*&       pupnpdToFill
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckData();

        tCIDLib::TBoolean bInit();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4RefCount
        //      For various reasons these devices are reference counted. When
        //      it hits zero, we delete ourself.
        //
        //  m_pData
        //      This is an opaque pointer to whatever information the per-
        //      platform implementation needs to maintain.
        // -------------------------------------------------------------------
        tCIDLib::TVoid* m_pData;
};



// ---------------------------------------------------------------------------
//   CLASS: MUPnPAsyncFinderCB
//  PREFIX: mkupnpfcb
//
//  This is a callback mixin that is used by CIDUPnP to pass in async finder
//  objects to our async finder class below.
//
//  In the bDeviceArrived() method, if the caller wants to keep the passed
//  device object, it should return true. If it returns false then the
//  finder will delete it upon return.
// ---------------------------------------------------------------------------
class KRNLEXPORT MUPnPAsyncFinderCB
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        MUPnPAsyncFinderCB(const MUPnPAsyncFinderCB&) = delete;

        virtual ~MUPnPAsyncFinderCB();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        MUPnPAsyncFinderCB& operator=(const MUPnPAsyncFinderCB&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        MUPnPAsyncFinderCB();


        // -------------------------------------------------------------------
        //  Let the async callback call the protected virtuals
        // -------------------------------------------------------------------
        friend class CUPnPFinderCallback;


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bDeviceArrived
        (
            const   tCIDLib::TCh* const     pszUID
            , const tCIDLib::TCh* const     pszName
            , const tCIDLib::TCh* const     pszModel
            , const tCIDLib::TCh* const     pszType
            ,       TKrnlUPnPDevice* const  pkupnpdNew
        ) = 0;

        virtual tCIDLib::TVoid DeviceRemoved
        (
            const   tCIDLib::TCh* const     pszUID

        ) = 0;

        virtual tCIDLib::TVoid ListComplete
        (
            const   tCIDLib::TCh* const     pszSearchID
        ) = 0;
};



// ---------------------------------------------------------------------------
//   CLASS: TKrnlUPnPAsyncFinder
//  PREFIX: kupnpsf
//
//  The CIDUPnP facility's async finder class works in terms of this class.
//  He generally implements our async finder callback mixin and passes himself
//  as the callback target. The SetCallback() method must be called or any
//  calls to bListenFor will be rejected.
//
//  We can start up multiple searches that will be called back through the
//  same callback object. We give back a search id that can be passed back in
//  to cancel a search. The CIDUPnP level async finder class must maintain a
//  list of sarches it has started on this object and stop them before this
//  object destructs.
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlUPnPAsyncFinder
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlUPnPAsyncFinder();

        TKrnlUPnPAsyncFinder
        (
                    MUPnPAsyncFinderCB* const pmkupnpfcbTar
        );

        TKrnlUPnPAsyncFinder(const TKrnlUPnPAsyncFinder&) = delete;

        ~TKrnlUPnPAsyncFinder();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlUPnPAsyncFinder& operator=(const TKrnlUPnPAsyncFinder&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCleanup();

        tCIDLib::TBoolean bListenFor
        (
            const   tCIDLib::TCh* const     pszFindType
            ,       TKrnlString&            kstrSearchID
        );

        tCIDLib::TBoolean bSetCallback
        (
                    MUPnPAsyncFinderCB* const pmkupnpfcbTar
        );

        tCIDLib::TBoolean bStopListeningFor
        (
            const   tCIDLib::TCh* const     pszSearchID
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pmkupnpfcbTar
        //      A pointer to the callback object, which we see as our own
        //      simple async finder callback mixin. It'll actually be some
        //      CIDUPnP object that implements our mixin interface.
        //
        //      We don't own this, we just pass along callbacks to it. It
        //      must be set before starting any searches. It can't be changed
        //      once set. It can be set in the ctor or later via SetCallback().
        //
        //  m_pFinderInfo
        //      An opaque pointer that the per-platform implementation can
        //      use for its own needs.
        // -------------------------------------------------------------------
        MUPnPAsyncFinderCB* m_pmkupnpfcbTar;
        tCIDLib::TVoid*     m_pFinderInfo;
};

#pragma CIDLIB_POPPACK

