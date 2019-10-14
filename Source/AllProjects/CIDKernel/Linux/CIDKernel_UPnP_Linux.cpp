//
// FILE NAME: CIDKernel_UPnP_Linux.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/12/2019
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
//  This file provides the Linux specific implementation for the UPnP support
//  clases.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"
#include    "CIDKernel_InternalHelpers_.hpp"
#include    "CIDKernel_UPnP.hpp"


namespace CIDKernel_UPnP_Linux
{
}




// ---------------------------------------------------------------------------
//  There is a kernel level async finder class.
// ---------------------------------------------------------------------------
class TKrnlUPnPAsyncFinder;


// -----------------------------------------------------------------------
//  This is the structure used to store our per-platform info in the
//  async finder class.
// -----------------------------------------------------------------------
struct TAFinderInfo
{
};




// ---------------------------------------------------------------------------
//   CLASS: MUPnPSvcCallback
//  PREFIX: upnpscb
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  MUPnPSvcCallback: Destructor
// ---------------------------------------------------------------------------
MUPnPSvcCallback::~MUPnPSvcCallback()
{
}


// ---------------------------------------------------------------------------
//  MUPnPSvcCallback: Hidden constructors and operators
// ---------------------------------------------------------------------------
MUPnPSvcCallback::MUPnPSvcCallback()
{
}

MUPnPSvcCallback::MUPnPSvcCallback(const MUPnPSvcCallback&)
{
}

MUPnPSvcCallback& MUPnPSvcCallback::operator=(const MUPnPSvcCallback&)
{
    return *this;
}




// ---------------------------------------------------------------------------
//   CLASS: TKrnlUPnPService
//  PREFIX: kupnps
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlUPnPService: Destructor
// ---------------------------------------------------------------------------

TKrnlUPnPService::~TKrnlUPnPService()
{
    // Release our per-platform data
}


// ---------------------------------------------------------------------------
//  TKrnlUPnPService: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Adds a new callback handler for this service. The caller must implement
//  the callback mixin. And IT MUST NOT go away while this service object
//  exists.
//
tCIDLib::TVoid TKrnlUPnPService::AddCallback(MUPnPSvcCallback* pupnpscbTar)
{
    CIDAssert2(L"TKrnlUPnPService::AddCallback is not implemented yet on Linux");
}


// Give back our service type URN
tCIDLib::TBoolean
TKrnlUPnPService::bQueryServiceType(TKrnlString& kstrToFill) const
{
    TKrnlError::SetLast(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


// Query a particular state variable
tCIDLib::TBoolean
TKrnlUPnPService::bQueryStateVar(const  tCIDLib::TCh* const pszVarName
                                ,       TKrnlString&        kstrVal) const
{
    TKrnlError::SetLast(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


//
//  A generic action invocation method. We just send all parameters as text
//  and get all output parms as text. The underlying Windows API doesn't care.
//  It just takes what we send, so it's just vastly easier to do it this way
//  and have a single method that can serve all our needs than to try to have
//  humpteen variations.
//
//  Unused parameters should be null pointers. Any that are provided must
//  start at 1 and be contiguous, i.e. no skipped ones.
//
//  Output parameters, if any, are also formatted to text and returned as
//  list of strings.
//
tCIDLib::TBoolean
TKrnlUPnPService::bInvokeAct(const  tCIDLib::TCh* const         pszCmdName
                            , const tCIDLib::TCh*               apszInParm[]
                            , const tCIDLib::TCard4             c4ParmCnt
                            ,       TKrnlLList<TKrnlString>&    kllstOutVals
                            ,       TKrnlString&                kstrRetVal) const
{
    TKrnlError::SetLast(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}



//
//  Return a new service object that references the same COM device object,
//  bumping the reference count before we return it.
//
TKrnlUPnPService* TKrnlUPnPService::pkupnpsClone()
{
    CIDAssert2(L"TKrnlUPnPService::pkupnpsClone() is not supported yet on Linux");
    return nullptr;
}



// ---------------------------------------------------------------------------
//  TKrnlUPnPService: Hidden Constructors
// ---------------------------------------------------------------------------

TKrnlUPnPService::TKrnlUPnPService(tCIDLib::TVoid* const pData) :

    m_pData(pData)
{
}





// ---------------------------------------------------------------------------
//   CLASS: TKrnlUPnPDevice
//  PREFIX: kupnpd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlUPnPDevice: Destructor
// ---------------------------------------------------------------------------
TKrnlUPnPDevice::~TKrnlUPnPDevice()
{
    // Let our perploatfrom data go
    if (m_pData)
    {
    }
}


// ---------------------------------------------------------------------------
//  TKrnlUPnPDevice: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Get the root device of this device. If this is the root device, we return
//  true but set the device pointer to null.
//
tCIDLib::TBoolean
TKrnlUPnPDevice::bGetRootDevice(TKrnlUPnPDevice*& pkupnpdToFill)
{
    pkupnpdToFill = nullptr;

    TKrnlError::SetLast(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


// Indicate whether this device has child devices or not
tCIDLib::TBoolean
TKrnlUPnPDevice::bHasChildren(tCIDLib::TBoolean& bToFill) const
{
    TKrnlError::SetLast(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


// Indicate whether this device is a root device
tCIDLib::TBoolean TKrnlUPnPDevice::bIsRoot(tCIDLib::TBoolean& bToFill) const
{
    TKrnlError::SetLast(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


//
//  Return a list of key/value objects that provide info about the child
//  devices. The key is the unique id. Value 1 is the human readable name
//  and value 2 is the type URN.
//
//  They can indicate optionally that they only want info on devices of a
//  particular type. If it's null, we return them all. Else we only reutrn
//  those whose device type URN matches the passed type.
//
tCIDLib::TBoolean TKrnlUPnPDevice::
bQueryChildDevices(         TKrnlLList<TKrnlKVPair>&    kllistFound
                    , const tCIDLib::TCh* const         pszOfType) const
{
    // Empty the incoming list first
    kllistFound.RemoveAll();
    TKrnlError::SetLast(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}



// Query the device description URL
tCIDLib::TBoolean TKrnlUPnPDevice::bQueryDevDescrURL(TKrnlString& kstrToFill) const
{
    TKrnlError::SetLast(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


// Query some general info about the device
tCIDLib::TBoolean
TKrnlUPnPDevice::bQueryDevInfo( TKrnlString&    kstrUID
                                , TKrnlString&  kstrName
                                , TKrnlString&  kstrModel
                                , TKrnlString&  kstrType) const
{
    TKrnlError::SetLast(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


// Query the device manufacturer of this device
tCIDLib::TBoolean
TKrnlUPnPDevice::bQueryManufacturer(TKrnlString& kstrToFill) const
{
    TKrnlError::SetLast(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}



//
//  A generic service getter, used by the type specific public ones. This one
//  looks for a specific service with a particular id that has already been
//  looked up or is known in advance. The caller becomes responsible for
//  cleaning up the returned service if one is returned.
//
//  Not finding the service is not considered an error, so the return will
//  be true but the pointer will be null.
//
tCIDLib::TBoolean
TKrnlUPnPDevice::bQueryServiceByID( const   tCIDLib::TCh* const pszServiceId
                                    ,       TKrnlUPnPService*&  pkupnpsToFill)
{
    // Default just in case we fail
    pkupnpsToFill = nullptr;

    TKrnlError::SetLast(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


//
//  Return a new device object that references the same COM device object,
//  bumping the reference count before we return it.
//
TKrnlUPnPDevice* TKrnlUPnPDevice::pkupnpdClone()
{
    CIDAssert(L"TKrnlUPnPDevice::pkupnpdClone() is not supported on Linux yet");
    TKrnlError::SetLast(kKrnlErrs::errcGen_NotSupported);
    return nullptr;
}




// ---------------------------------------------------------------------------
//   CLASS: TKrnlUPnPFinder
//  PREFIX: kev
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlUPnPFinder: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlUPnPFinder::TKrnlUPnPFinder() :

    m_pData(nullptr)
{
}


TKrnlUPnPFinder::~TKrnlUPnPFinder()
{
    // Clean up our per-platform data
    if (m_pData)
    {
    }
}


// ---------------------------------------------------------------------------
//  TKrnlUPnPFinder: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Does a search for any devices that are of the indicated type. We return
//  the unique id (key), friendly name (value), and type URN (value 2) for
//  each one found.
//
tCIDLib::TBoolean
TKrnlUPnPFinder::bSearchByType( const   tCIDLib::TCh* const         pszType
                                ,       TKrnlLList<TKrnlKVPair>&    kllistFound)
{
    // Make sure we've been initialized
    if (!bCheckData())
        return kCIDLib::False;

    // Clear the output list so we can fill it in
    kllistFound.RemoveAll();

    TKrnlError::SetLast(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


//
//  Does a search for a specific device by its unique ID
//
tCIDLib::TBoolean
TKrnlUPnPFinder::bSearchByUID(  const   tCIDLib::TCh* const pszUID
                                ,       TKrnlUPnPDevice*&   pupnpdToFill)
{
    // Make sure we've been initialized
    if (!bCheckData())
        return kCIDLib::False;

    TKrnlError::SetLast(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}



// ---------------------------------------------------------------------------
//  TKrnlUPnPFinder: Private, non-virtual methods
// ---------------------------------------------------------------------------

// A helper to make sure the object is initialized and does it if so
tCIDLib::TBoolean TKrnlUPnPFinder::bCheckData()
{
    // If not initialized already, try to do it and return that status
    if (!m_pData)
        return bInit();
    return kCIDLib::True;
}


// This is called to fault in the underlying finder object
tCIDLib::TBoolean TKrnlUPnPFinder::bInit()
{
    m_pData = nullptr;
    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//   CLASS: MUPnPAsyncFinderCB
//  PREFIX: mkupnpfcb
// ---------------------------------------------------------------------------

// -------------------------------------------------------------------
//  MUPnPAsyncFinderCB: Destructor
// -------------------------------------------------------------------
MUPnPAsyncFinderCB::~MUPnPAsyncFinderCB()
{
}


// -------------------------------------------------------------------
//  MUPnPAsyncFinderCB: Hidden constructors
// -------------------------------------------------------------------
MUPnPAsyncFinderCB::MUPnPAsyncFinderCB()
{
}


// ---------------------------------------------------------------------------
//   CLASS: MUPnPAsyncFinder
//  PREFIX: mukpnpf
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlUPnPAsyncFinder: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlUPnPAsyncFinder::TKrnlUPnPAsyncFinder() :

    m_pFinderInfo(nullptr)
    , m_pmkupnpfcbTar(nullptr)
{
    // Create our per-platform info structure
    TAFinderInfo* pInfo = new TAFinderInfo;
    m_pFinderInfo = pInfo;
}

// Set the callback up front
TKrnlUPnPAsyncFinder::
TKrnlUPnPAsyncFinder(MUPnPAsyncFinderCB* const pmkupnpfcbTar) :

    m_pFinderInfo(nullptr)
    , m_pmkupnpfcbTar(pmkupnpfcbTar)
{
    //
    //  Create our per-platform info structure, and we can go ahead and set
    //  the callback object.
    //
    TAFinderInfo* pInfo = new TAFinderInfo;
    m_pFinderInfo = pInfo;
}

TKrnlUPnPAsyncFinder::~TKrnlUPnPAsyncFinder()
{
    TAFinderInfo* pInfo = static_cast<TAFinderInfo*>(m_pFinderInfo);

    // Call our own cleanup method
    bCleanup();

    m_pFinderInfo = nullptr;
    delete pInfo;
}


// ---------------------------------------------------------------------------
//  TKrnlUPnPAsyncFinder: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Allows the CIDUpnP level async finder to get us to clean up before we
//  destruct, which might be desirable in order to make sure it's done in
//  the same thread that created the finder object. After this, the callback
//  has to be set again and a new search started.
//
tCIDLib::TBoolean TKrnlUPnPAsyncFinder::bCleanup()
{
    TAFinderInfo* pInfo = static_cast<TAFinderInfo*>(m_pFinderInfo);

    return kCIDLib::True;
}


//
//  Assuming we've had our target callback setup, we will start a new search
//  for the passed type and give back the search id for this search.
//
tCIDLib::TBoolean
TKrnlUPnPAsyncFinder::bListenFor(const  tCIDLib::TCh* const pszFindType
                                ,       TKrnlString&        kstrSearchID)
{
    TAFinderInfo* pInfo = static_cast<TAFinderInfo*>(m_pFinderInfo);
    TKrnlError::SetLast(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


//
//  Set the callback object to be notified of changes. It can't already be
//  set. We don't allow it to be changed.
//
tCIDLib::TBoolean
TKrnlUPnPAsyncFinder::bSetCallback(MUPnPAsyncFinderCB* const pmkupnpfcbTar)
{
    TAFinderInfo* pInfo = static_cast<TAFinderInfo*>(m_pFinderInfo);

    // Make sure the finder hasn't already been set
    if (m_pmkupnpfcbTar)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnP_AlreadyInit);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


// Cancel a previously started sarch
tCIDLib::TBoolean
TKrnlUPnPAsyncFinder::bStopListeningFor(const tCIDLib::TCh* const pszSearchID)
{
    TAFinderInfo* pInfo = static_cast<TAFinderInfo*>(m_pFinderInfo);

    TKrnlError::SetLast(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


