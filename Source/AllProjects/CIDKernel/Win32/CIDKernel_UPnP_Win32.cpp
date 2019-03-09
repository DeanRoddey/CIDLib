//
// FILE NAME: CIDKernel_UPnP_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/10/2011
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file provides the Win32 specific implementation for the UPnP support
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
#include    <Upnp.h>


namespace CIDKernel_UPnP_Win32
{

    // -----------------------------------------------------------------------
    //  Windows puts a prefix on the unique ids of devices. But we have to
    //  remain portable. So we have to remove the prefix in terms of the
    //  id we expose to the outside world. That means we have to put them
    //  back on any that we get from the outside world. So a PITA and error
    //  prone, but we have to do it. Otherwise when portable code used a
    //  device id as a parameter to some action, it wouldn't work.
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const   pszDevIDPref = L"uuid:";
    const tCIDLib::TCard4       c4DevIdPrefLen = 5;
}




// ---------------------------------------------------------------------------
//  There is a kernel level async finder class.
//
//  We also create our own derivative of the COM level finder callback, which
//  the async finder class creates one of points at itself.
// ---------------------------------------------------------------------------
class TKrnlUPnPAsyncFinder;

class CUPnPFinderCallback : public IUPnPDeviceFinderCallback
{
    public:
        CUPnPFinderCallback(MUPnPAsyncFinderCB* pmkupnpfcbTar) :

          m_lRefCount(1)
          , m_pmkupnpfcbTar(pmkupnpfcbTar)
        {
        }

        STDMETHODIMP QueryInterface(REFIID iid, LPVOID* ppvObject)
        {
            HRESULT hRes = S_OK;

            if(NULL == ppvObject)
                hRes = E_POINTER;
            else
                *ppvObject = NULL;

            if(SUCCEEDED(hRes))
            {
                if (IsEqualIID(iid, IID_IUnknown)
                ||  IsEqualIID(iid, IID_IUPnPDeviceFinderCallback))
                {
                    *ppvObject = static_cast<IUPnPDeviceFinderCallback*>(this);
                    AddRef();
                }
                 else
                {
                    hRes = E_NOINTERFACE;
                }
            }
            return hRes;
        }

        STDMETHODIMP_(ULONG) AddRef()
        {
            return ::InterlockedIncrement(&m_lRefCount);
        }

        STDMETHODIMP_(ULONG) Release()
        {
            LONG lRefCount = ::InterlockedDecrement(&m_lRefCount);
            if(0 == lRefCount)
                delete this;
            return lRefCount;
        }

        STDMETHODIMP DeviceAdded(LONG lFindData, IUPnPDevice* pDev)
        {
            tCIDLib::TBoolean bRes = kCIDLib::False;
            BSTR NameStr = 0;
            BSTR ModelStr = 0;
            BSTR TypeStr = 0;
            BSTR UIDStr = 0;

            // Make sure our callback is there
            if (!m_pmkupnpfcbTar)
            {
                pDev->Release();
                return 0;
            }

            tCIDLib::TCh* pszRealUID = L"";
            HRESULT hRes = pDev->get_FriendlyName(&NameStr);
            if (!FAILED(hRes))
            {
                hRes = pDev->get_UniqueDeviceName(&UIDStr);
                if (!FAILED(hRes))
                {
                    // Skip over the windows prefix on the UID
                    pszRealUID = UIDStr;
                    if (TRawStr::bCompareStrNI( UIDStr
                                                , CIDKernel_UPnP_Win32::pszDevIDPref
                                                , CIDKernel_UPnP_Win32::c4DevIdPrefLen))
                    {
                        pszRealUID += CIDKernel_UPnP_Win32::c4DevIdPrefLen;
                    }

                    hRes = pDev->get_Type(&TypeStr);
                    if (!FAILED(hRes))
                    {
                        hRes = pDev->get_ModelName(&ModelStr);
                        if (!FAILED(hRes))
                            bRes = kCIDLib::True;
                    }
                }
            }

            if (bRes)
            {
                //
                //  For now, our per-platform info is just the raw COM device
                //  pointer. Add another ref to the device object so that we
                //  can keep it, otherwise it'll be destroyed upon return.
                //  Our kernel level device will release it again when it
                //  destructs.
                //
                pDev->AddRef();
                TKrnlUPnPDevice* pkupnpdNew = new TKrnlUPnPDevice(pDev);
                try
                {
                    //
                    //  If the callback indicates that he did not keep the device
                    //  object, then delete it.
                    //
                    tCIDLib::TBoolean bKeptObj = m_pmkupnpfcbTar->bDeviceArrived
                    (
                        pszRealUID, NameStr, ModelStr, TypeStr, pkupnpdNew
                    );

                    if (!bKeptObj)
                        delete pkupnpdNew;
                }

                catch(...)
                {
                    // Make sure the object gets cleaned up
                    delete pkupnpdNew;
                }
            }

            // Clean up any strings we got
            if (TypeStr != 0)
                ::SysFreeString(TypeStr);

            if (NameStr != 0)
                ::SysFreeString(NameStr);

            if (ModelStr != 0)
                ::SysFreeString(ModelStr);

            if (UIDStr != 0)
                ::SysFreeString(UIDStr);

            return 0;
        }


        STDMETHODIMP DeviceRemoved(LONG lFindData, BSTR bstrUDN)
        {
            try
            {
                if (m_pmkupnpfcbTar)
                {
                    // Skip over the windows prefix on the UID
                    tCIDLib::TCh* pszRealUID = bstrUDN;
                    if (TRawStr::bCompareStrNI( bstrUDN
                                                , CIDKernel_UPnP_Win32::pszDevIDPref
                                                , CIDKernel_UPnP_Win32::c4DevIdPrefLen))
                    {
                        pszRealUID += CIDKernel_UPnP_Win32::c4DevIdPrefLen;
                    }
                    m_pmkupnpfcbTar->DeviceRemoved(pszRealUID);
                }
            }

            catch(...)
            {
            }
            return S_OK;
        }

        STDMETHODIMP SearchComplete(LONG lFindData)
        {
            try
            {
                if (m_pmkupnpfcbTar)
                {
                    tCIDLib::TCh achFindID[32];
                    TRawStr::bFormatVal(lFindData, achFindID, 31);
                    m_pmkupnpfcbTar->ListComplete(achFindID);
                }
            }

            catch(...)
            {
            }
            return S_OK;
        }

    private:
        // Our COM reference count
        LONG                m_lRefCount;
        MUPnPAsyncFinderCB* m_pmkupnpfcbTar;
};



// -----------------------------------------------------------------------
//  This is the structure used to store our per-platform info in the
//  async finder class.
// -----------------------------------------------------------------------
struct TAFinderInfo
{
    // The finder callback we created and pointed to the finder class instance
    CUPnPFinderCallback*    pcbTar;

    // The OS level async finder object
    IUPnPDeviceFinder*      pFinder;
};



// ---------------------------------------------------------------------------
//  This is a service callback handler that we have to create in order to
//  register for callbacks. Each service created will get one, and it will
//  be pointed at the service's callback handler.
// ---------------------------------------------------------------------------
class CUPnPServiceCallback : public IUPnPServiceCallback
{
    public:
        CUPnPServiceCallback(MUPnPSvcCallback* pscbTar) :

            m_pscbTar(pscbTar)
            , m_lRefCount(1)
        {
        }

        STDMETHODIMP QueryInterface(REFIID iid, LPVOID* ppvObject)
        {
            HRESULT hRes = S_OK;

            if(NULL == ppvObject)
                hRes = E_POINTER;
            else
                *ppvObject = NULL;

            if(SUCCEEDED(hRes))
            {
                if (IsEqualIID(iid, IID_IUnknown)
                ||  IsEqualIID(iid, IID_IUPnPServiceCallback))
                {
                    *ppvObject = static_cast<IUPnPServiceCallback*>(this);
                    AddRef();
                }
                 else
                {
                    *ppvObject = 0;
                    hRes = E_NOINTERFACE;
                }
            }
            return hRes;
        }

        STDMETHODIMP_(ULONG) AddRef()
        {
            return ::InterlockedIncrement(&m_lRefCount);
        }

        STDMETHODIMP_(ULONG) Release()
        {
            LONG lRefCount = ::InterlockedDecrement(&m_lRefCount);
            if(0 == lRefCount)
                delete this;
            return lRefCount;
        }

        STDMETHODIMP StateVariableChanged(  IUPnPService*   pus
                                            , LPCWSTR       pszStateVarName
                                            , VARIANT       varValue)
        {
            VARIANT AlphaVariant;
            VariantInit(&AlphaVariant);
            HRESULT hRes = ::VariantChangeType
            (
                &AlphaVariant, &varValue, VARIANT_ALPHABOOL, VT_BSTR
            );

            if(SUCCEEDED(hRes))
            {
                m_pscbTar->UPnPVarChanged
                (
                    pszStateVarName, AlphaVariant.bstrVal
                );
                ::VariantClear(&AlphaVariant);
            }
            return S_OK;
        }

        STDMETHODIMP ServiceInstanceDied(IUPnPService *pus)
        {
            m_pscbTar->UPnPServiceDied();
            return S_OK;
        }

    private:
        LONG                m_lRefCount;
        MUPnPSvcCallback*   m_pscbTar;
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
    // Let our COM level object go if we have one
    if (m_pData)
    {
        static_cast<IUPnPService*>(m_pData)->Release();
        m_pData = nullptr;
    }
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
    //
    //  Create and regster a new service callback object for the target
    //  callback handler.
    //
    IUnknown* pUPnPServiceCallback = new CUPnPServiceCallback(pupnpscbTar);
    if (pUPnPServiceCallback)
    {
        //
        //  We only neeed to keep it alive long enough for it to be registered,
        //  then we can drop our reference. It'll get destroyed when we are
        //  destroyed.
        //
        HRESULT hRes = static_cast<IUPnPService*>(m_pData)->AddCallback
        (
            pUPnPServiceCallback
        );
        pUPnPServiceCallback->Release();
    }
}

// Give back our service type URN
tCIDLib::TBoolean
TKrnlUPnPService::bQueryServiceType(TKrnlString& kstrToFill) const
{
    IUPnPService* pSrv = static_cast<IUPnPService*>(m_pData);

    BSTR TypeStr;
    HRESULT hRes = pSrv->get_ServiceTypeIdentifier(&TypeStr);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnP_QueryServiceType, hRes);
        return kCIDLib::False;
    }

    kstrToFill.Set(TypeStr);
    ::SysFreeString(TypeStr);
    return kCIDLib::True;
}


// Query a particular state variable
tCIDLib::TBoolean
TKrnlUPnPService::bQueryStateVar(const  tCIDLib::TCh* const pszVarName
                                ,       TKrnlString&        kstrVal) const
{
    // Get the var name into a BSTR
    TBSTRJanitor janName(pszVarName);

    IUPnPService* pSrv = static_cast<IUPnPService*>(m_pData);

    VARIANT VarVal;
    ::VariantInit(&VarVal);
    HRESULT hRes = pSrv->QueryStateVariable(janName.bsData, &VarVal);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnP_QueryStateVar, hRes);
        return kCIDLib::False;
    }

    hRes = ::VariantChangeType(&VarVal, &VarVal, VARIANT_ALPHABOOL, VT_BSTR);
    if (FAILED(hRes))
    {
        ::VariantClear(&VarVal);
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnP_QueryStateVar, hRes);
        return kCIDLib::False;
    }

    kstrVal = VarVal.bstrVal;
    return kCIDLib::True;
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
    TBSTRJanitor janCmd(pszCmdName);

    // Create an empty parameter list for this type of action
    SAFEARRAYBOUND ArrayBnd[1];
    ArrayBnd[0].lLbound = 0;

    // Set up our input parameter array based on the parm count
    ArrayBnd[0].cElements = c4ParmCnt;
    SAFEARRAY* pArray = ::SafeArrayCreate(VT_VARIANT, 1, ArrayBnd);
    if (!pArray)
    {
        TKrnlError::SetLastError(kKrnlErrs::errcUPnP_PlatErr);
        return kCIDLib::False;
    }

    // Now set up the list of values
    VARIANT InArgs;
    ::VariantInit(&InArgs);

    LONG lIndex = 0;
    VARIANT InParm;
    ::VariantInit(&InParm);

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ParmCnt; c4Index++)
    {
        InParm.vt = VT_BSTR;
        V_BSTR(&InParm) = ::SysAllocString(apszInParm[c4Index]);
        lIndex = (LONG)c4Index;
        ::SafeArrayPutElement(pArray, &lIndex, (void*)&InParm);
        ::VariantClear(&InParm);
    }

    // And now set the array into it's variant
    InArgs.vt = VT_VARIANT | VT_ARRAY;
    V_ARRAY(&InArgs) = pArray;

    // Set up the two output variants
    VARIANT OutArgs;
    ::VariantInit(&OutArgs);

    VARIANT RetVal;
    ::VariantInit(&RetVal);

    IUPnPService* pSrv = static_cast<IUPnPService*>(m_pData);
    HRESULT hRes = pSrv->InvokeAction(janCmd.bsData, InArgs, &OutArgs, &RetVal);

    tCIDLib::TBoolean bRet = kCIDLib::True;
    if (FAILED(hRes))
    {
        //
        //  It failed so set the last error and and return status, and fall
        //  through to clean up.
        //
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnP_InvokeAct, hRes);
        bRet = kCIDLib::False;
    }

    // Get the return value if any
    if (RetVal.vt == VT_EMPTY)
    {
        kstrRetVal.Clear();
    }
     else
    {
        ::VariantChangeType(&RetVal, &RetVal, VARIANT_ALPHABOOL, VT_BSTR);
        kstrRetVal = RetVal.bstrVal;
    }

    // Get out any output parameters if any
    if (!OutArgs.vt == VT_EMPTY)
    {
        HRESULT hRes2;
        LONG lLBound;
        LONG lUBound;
        hRes = ::SafeArrayGetLBound(OutArgs.parray, 1, &lLBound);
        hRes2 = ::SafeArrayGetUBound(OutArgs.parray, 1, &lUBound);
        if (FAILED(hRes) || FAILED(hRes2))
        {
            //
            //  We have to treat this as a failure, even if the actual action
            //  worked, since the caller expects the parameters.
            //
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnP_GetOutParms, hRes);
            bRet = kCIDLib::False;
        }

        if (SUCCEEDED(hRes))
        {
            VARIANT OutParm;
            for (lIndex = lLBound; lIndex <= lUBound; ++lIndex)
            {
                ::VariantClear(&OutParm);

                LONG lPos = lIndex;
                hRes = ::SafeArrayGetElement(OutArgs.parray, &lPos, &OutParm);
                if(FAILED(hRes))
                {
                    //
                    //  We have to treat this as a failure, even if the actual action
                    //  worked, since the caller expects the parameters.
                    //
                    TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnP_GetOutParms, hRes);
                    bRet = kCIDLib::False;
                    break;
                }

                hRes = ::VariantChangeType(&OutParm, &OutParm, VARIANT_ALPHABOOL, VT_BSTR);
                if (FAILED(hRes))
                {
                    //
                    //  We have to treat this as a failure, even if the actual action
                    //  worked, since the caller expects the parameters.
                    //
                    TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnP_GetOutParms, hRes);
                    bRet = kCIDLib::False;
                    break;
                }
                kllstOutVals.pobjAddNew(new TKrnlString(OutParm.bstrVal));
                ::VariantClear(&OutParm);
            }
        }
    }

    ::SafeArrayDestroy(pArray);
    ::VariantClear(&OutArgs);
    ::VariantClear(&RetVal);

    InArgs.vt = VT_EMPTY;
    ::VariantClear(&InArgs);

    return bRet;
}



//
//  Return a new service object that references the same COM device object,
//  bumping the reference count before we return it.
//
TKrnlUPnPService* TKrnlUPnPService::pkupnpsClone()
{
    // Get a pointer to our actual data
    IUPnPService* pSrv = static_cast<IUPnPService*>(m_pData);

    // Add a reference and give it to a new service objject
    pSrv->AddRef();
    return new TKrnlUPnPService(pSrv);
}



// ---------------------------------------------------------------------------
//  TKrnlUPnPService: Hidden Constructors
// ---------------------------------------------------------------------------

//
//  Just store away the data (should be an IUPnPService). We assume the caller
//  has bumped the ref count on it, so that it will stay alive at least until
//  we destruct and release it.
//
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
    // Let our COM level object go if we have one
    if (m_pData)
    {
        static_cast<IUPnPDevice*>(m_pData)->Release();
        m_pData = nullptr;
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

    // First see if this device is the root device
    VARIANT_BOOL IsRoot;
    HRESULT hRes = static_cast<IUPnPDevice*>(m_pData)->get_IsRootDevice(&IsRoot);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnp_AttrQuery, hRes);
        return kCIDLib::False;
    }

    IUPnPDevice* pDev = nullptr;
    if (IsRoot == VARIANT_FALSE)
    {
        HRESULT hRes = static_cast<IUPnPDevice*>(m_pData)->get_RootDevice(&pDev);
        if (FAILED(hRes) || !pDev)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnp_GetRoot, hRes);
            return kCIDLib::False;
        }
    }

    // It worked so set up the device object and return success
    pkupnpdToFill = new TKrnlUPnPDevice(pDev);
    return kCIDLib::True;
}


// Indicate whether this device has child devices or not
tCIDLib::TBoolean
TKrnlUPnPDevice::bHasChildren(tCIDLib::TBoolean& bToFill) const
{
    // See if we have any children
    VARIANT_BOOL Res;
    HRESULT hRes = static_cast<IUPnPDevice*>(m_pData)->get_HasChildren(&Res);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnp_AttrQuery, hRes);
        return kCIDLib::False;
    }

    bToFill = (Res != VARIANT_FALSE);
    return kCIDLib::True;
}


// Indicate whether this device is a root device
tCIDLib::TBoolean TKrnlUPnPDevice::bIsRoot(tCIDLib::TBoolean& bToFill) const
{
    VARIANT_BOOL Res;
    HRESULT hRes = static_cast<IUPnPDevice*>(m_pData)->get_IsRootDevice(&Res);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnp_AttrQuery, hRes);
        return kCIDLib::False;
    }

    bToFill = (Res != VARIANT_FALSE);
    return kCIDLib::True;

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
    IUPnPDevices* pDevices = nullptr;
    HRESULT hRes;

    // Empty the incoming list first
    kllistFound.RemoveAll();

    hRes = static_cast<IUPnPDevice*>(m_pData)->get_Children(&pDevices);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnP_QueryChildDevs, hRes);
        return kCIDLib::False;
    }
    TCOMJanitor<IUPnPDevices> janDevices(&pDevices);

    //
    //  It worked, so get the count of children we have.
    //
    LONG lCount;
    hRes = pDevices->get_Count(&lCount);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnP_SearchFailed, hRes);
        return kCIDLib::False;
    }

    // If it's zero, just return success now with an empty list
    if (!lCount)
        return kCIDLib::True;


    // We have something to iterate, so get an enumerator for the collection
    IUnknown* pUnk = nullptr;
    pDevices->get__NewEnum(&pUnk);
    TCOMJanitor<IUnknown> janUnk(&pUnk);
    IEnumUnknown* pEnum;
    hRes = pUnk->QueryInterface(IID_IEnumUnknown, (void**)&pEnum);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnP_SearchFailed, hRes);
        return kCIDLib::False;
    }
    TCOMJanitor<IEnumUnknown> janEnum(&pEnum);

    // Enumerate them an fill our list
    for (LONG lIndex = 0; lIndex < lCount; lIndex++)
    {
        IUPnPDevice* pDev = nullptr;
        hRes = pEnum->Next(1, (IUnknown**)&pDev, 0);
        if (!FAILED(hRes))
        {
            TCOMJanitor<IUPnPDevice> janDev(&pDev);

            tCIDLib::TCh* pszRealUID = L"";
            tCIDLib::TBoolean bRes = kCIDLib::False;
            BSTR NameStr = 0;
            BSTR TypeStr = 0;
            BSTR UIDStr = 0;
            hRes = pDev->get_FriendlyName(&NameStr);
            if (!FAILED(hRes))
            {
                hRes = pDev->get_UniqueDeviceName(&UIDStr);
                if (!FAILED(hRes))
                {
                    // Skip over the windows prefix on the UID
                    pszRealUID = UIDStr;
                    if (TRawStr::bCompareStrNI( UIDStr
                                                , CIDKernel_UPnP_Win32::pszDevIDPref
                                                , CIDKernel_UPnP_Win32::c4DevIdPrefLen))
                    {
                        pszRealUID += CIDKernel_UPnP_Win32::c4DevIdPrefLen;
                    }

                    hRes = pDev->get_Type(&TypeStr);
                    if (!FAILED(hRes))
                        bRes = kCIDLib::True;
                }
            }

            //
            //  If we got the values, add this one to the list. If we got a
            //  type filter, check that before we take it.
            //
            if (bRes)
            {
                if (!pszOfType || TRawStr::bCompareStrI(pszOfType, TypeStr))
                    kllistFound.pobjAddNew(new TKrnlKVPair(pszRealUID, NameStr, TypeStr));
            }

            // Clean up any strings we got
            if (TypeStr != 0)
                ::SysFreeString(TypeStr);

            if (NameStr != 0)
                ::SysFreeString(NameStr);

            if (UIDStr != 0)
                ::SysFreeString(UIDStr);
        }
    }

    return kCIDLib::True;
}



// Query the device description URL
tCIDLib::TBoolean TKrnlUPnPDevice::bQueryDevDescrURL(TKrnlString& kstrToFill) const
{
    // Get a pointer to our actual data
    IUPnPDevice* pDev = static_cast<IUPnPDevice*>(m_pData);

    IUPnPDeviceDocumentAccess* pDocAcc = nullptr;
    HRESULT hRes = pDev->QueryInterface(IID_IUPnPDeviceDocumentAccess, (void**)&pDocAcc);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnp_AttrQuery, hRes);
        return kCIDLib::False;
    }
    TCOMJanitor<IUPnPDeviceDocumentAccess> janServices(&pDocAcc);

    BSTR TmpStr;
    hRes = pDocAcc->GetDocumentURL(&TmpStr);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnp_AttrQuery, hRes);
        return kCIDLib::False;
    }

    kstrToFill.Set(TmpStr);
    ::SysFreeString(TmpStr);

    return kCIDLib::True;
}


// Query some general info about the device
tCIDLib::TBoolean
TKrnlUPnPDevice::bQueryDevInfo( TKrnlString&    kstrUID
                                , TKrnlString&  kstrName
                                , TKrnlString&  kstrModel
                                , TKrnlString&  kstrType) const
{
    // Get a pointer to our actual data
    IUPnPDevice* pDev = static_cast<IUPnPDevice*>(m_pData);

    //
    //  And try to get each string. If any of them fail we just return an
    //  'unknown' type string.
    //
    tCIDLib::TBoolean bRes = kCIDLib::True;
    BSTR TmpStr;
    HRESULT hRes = pDev->get_FriendlyName(&TmpStr);
    if (FAILED(hRes))
    {
        kstrName.Set(L"<Unknown Dev Name>");
        bRes = kCIDLib::False;
    }
     else
    {
        kstrName.Set(TmpStr);
        ::SysFreeString(TmpStr);
    }

    hRes = pDev->get_UniqueDeviceName(&TmpStr);
    if (FAILED(hRes))
    {
        kstrUID.Set(L"<Unknown UID>");
        bRes = kCIDLib::False;
    }
     else
    {
        // Skip the uid: prefix
        if (TRawStr::bCompareStrNI( TmpStr
                                    , CIDKernel_UPnP_Win32::pszDevIDPref
                                    , CIDKernel_UPnP_Win32::c4DevIdPrefLen))
        {
            kstrUID.Set(TmpStr + CIDKernel_UPnP_Win32::c4DevIdPrefLen);
        }
         else
        {
            kstrUID.Set(TmpStr);
        }
        ::SysFreeString(TmpStr);
    }

    hRes = pDev->get_ModelName(&TmpStr);
    if (FAILED(hRes))
    {
        kstrModel.Set(L"<Unknown Model>");
        bRes = kCIDLib::False;
    }
     else
    {
        kstrModel.Set(TmpStr);
        ::SysFreeString(TmpStr);
    }

    // Get the device type URN
    hRes = pDev->get_Type(&TmpStr);
    if (FAILED(hRes))
    {
        kstrType.Set(L"<Unknown Type>");
        bRes = kCIDLib::False;
    }
     else
    {
        kstrType.Set(TmpStr);
        ::SysFreeString(TmpStr);
    }

    return bRes;
}


// Query the device manufacturer of this device
tCIDLib::TBoolean
TKrnlUPnPDevice::bQueryManufacturer(TKrnlString& kstrToFill) const
{
    // Get a pointer to our actual data
    IUPnPDevice* pDev = static_cast<IUPnPDevice*>(m_pData);

    BSTR TmpStr;
    HRESULT hRes = pDev->get_ModelName(&TmpStr);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnp_AttrQuery, hRes);
        return kCIDLib::False;
    }

    kstrToFill.Set(TmpStr);
    ::SysFreeString(TmpStr);

    return kCIDLib::True;
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
    pkupnpsToFill = 0;

    // Get a pointer to our actual data
    IUPnPDevice* pDev = static_cast<IUPnPDevice*>(m_pData);

    // Get the known id for the service into a BSTR
    TBSTRJanitor janID(pszServiceId);

    // Get a services list
    IUPnPServices * pServices = 0;
    HRESULT hRes = pDev->get_Services(&pServices);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnP_GetService, hRes);
        return kCIDLib::False;
    }
    TCOMJanitor<IUPnPServices> janServices(&pServices);


    // Retrieve the service we are interested in
    IUPnPService* pService = 0;
    hRes = pServices->get_Item(janID.bsData, &pService);
    if (FAILED(hRes))
    {
        // Not found. We return true, but a zero pointer, since no actual error
        return kCIDLib::True;
    }

    //
    //  Our kernel level service object assumes that caller has bumped the
    //  ref count on the COM object, so we don't do any release here. The
    //  service object will do the release when it destructs.
    //
    pkupnpsToFill = new TKrnlUPnPService(pService);
    return kCIDLib::True;
}


//
//  Return a new device object that references the same COM device object,
//  bumping the reference count before we return it.
//
TKrnlUPnPDevice* TKrnlUPnPDevice::pkupnpdClone()
{
    // Get a pointer to our actual data
    IUPnPDevice* pDev = static_cast<IUPnPDevice*>(m_pData);

    // Add a ref count and give it to a new device object
    pDev->AddRef();
    return new TKrnlUPnPDevice(pDev);
}


// ---------------------------------------------------------------------------
//  TKrnlUPnPDevice: Hidden Constructors
// ---------------------------------------------------------------------------

//
//  It is assumed that the caller bumped the passed device COM object's ref
//  count already, so it'll stay alive until we destruct and release it.
//
TKrnlUPnPDevice::TKrnlUPnPDevice(tCIDLib::TVoid* const pDevInfo) :

    m_pData(pDevInfo)
{
}



// ---------------------------------------------------------------------------
//   CLASS: TKrnlUPnPFinder
//  PREFIX: kev
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlUPnPFinder: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlUPnPFinder::TKrnlUPnPFinder() :

    m_pData(0)
{
}


TKrnlUPnPFinder::~TKrnlUPnPFinder()
{
    // Clean up our data
    if (m_pData)
    {
        // Get a pointer as our type and then we can clear the public pointer
        IUPnPDeviceFinder* pFinder = static_cast<IUPnPDeviceFinder*>(m_pData);
        m_pData = 0;

        // If we still have the COM object, release it
        if (pFinder)
            pFinder->Release();
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

    // Get our internal pointer
    IUPnPDeviceFinder* pFinder = static_cast<IUPnPDeviceFinder*>(m_pData);

    // Get the type into correct COM form of the type URN
    TBSTRJanitor janType(pszType);

    IUPnPDevices* pDevices;
    HRESULT hRes = pFinder->FindByType(janType.bsData, 0, &pDevices);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnP_SearchFailed, hRes);
        return kCIDLib::False;
    }
    TCOMJanitor<IUPnPDevices> janDevices(&pDevices);

    //
    //  It worked, so gen up a list of our devices and add them to the linked
    //
    LONG lCount;
    hRes = pDevices->get_Count(&lCount);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnP_SearchFailed, hRes);
        return kCIDLib::False;
    }

    IUnknown* pUnk;
    pDevices->get__NewEnum(&pUnk);
    TCOMJanitor<IUnknown> janUnk(&pUnk);

    // Get the enum interface from this enumerator
    IEnumUnknown* pEnum;
    hRes = pUnk->QueryInterface(IID_IEnumUnknown, (void**)&pEnum);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnP_SearchFailed, hRes);
        return kCIDLib::False;
    }
    TCOMJanitor<IEnumUnknown> janEnum(&pEnum);

    // Enumerate them an fill our list
    for (LONG lIndex = 0; lIndex < lCount; lIndex++)
    {
        IUPnPDevice* pDev;
        hRes = pEnum->Next(1, (IUnknown**)&pDev, 0);
        if (!FAILED(hRes))
        {
            TCOMJanitor<IUPnPDevice> janDev(&pDev);

            tCIDLib::TCh* pszRealUID = L"";
            tCIDLib::TBoolean bRes = kCIDLib::False;
            BSTR NameStr = 0;
            BSTR TypeStr = 0;
            BSTR UIDStr = 0;
            hRes = pDev->get_FriendlyName(&NameStr);
            if (!FAILED(hRes))
            {
                hRes = pDev->get_UniqueDeviceName(&UIDStr);
                if (!FAILED(hRes))
                {
                    // Skip over the windows prefix on the UID
                    pszRealUID = UIDStr;
                    if (TRawStr::bCompareStrNI( UIDStr
                                                , CIDKernel_UPnP_Win32::pszDevIDPref
                                                , CIDKernel_UPnP_Win32::c4DevIdPrefLen))
                    {
                        pszRealUID += CIDKernel_UPnP_Win32::c4DevIdPrefLen;
                    }
                    hRes = pDev->get_Type(&TypeStr);
                    if (!FAILED(hRes))
                        bRes = kCIDLib::True;
                }
            }

            // If we got the values, add this one to the list
            if (bRes)
                kllistFound.pobjAddNew(new TKrnlKVPair(pszRealUID, NameStr, TypeStr));

            // Clean up any strings we got
            if (TypeStr != 0)
                ::SysFreeString(TypeStr);

            if (NameStr != 0)
                ::SysFreeString(NameStr);

            if (UIDStr != 0)
                ::SysFreeString(UIDStr);
        }
    }
    return kCIDLib::True;
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

    // Get our internal pointer
    IUPnPDeviceFinder* pFinder = static_cast<IUPnPDeviceFinder*>(m_pData);

    // Create a new string we can put the stupid Windows uid prefix on
    const tCIDLib::TCard4 c4RealUIDLen
    (
        TRawStr::c4StrLen(pszUID) + CIDKernel_UPnP_Win32::c4DevIdPrefLen
    );
    tCIDLib::TCh* pszRealUID = new tCIDLib::TCh[c4RealUIDLen + 1];
    TArrayJanitor<tCIDLib::TCh> janRealUID(pszRealUID);
    TRawStr::CopyCatStr
    (
        pszRealUID, c4RealUIDLen, CIDKernel_UPnP_Win32::pszDevIDPref, pszUID
    );

    // Get the type into correct COM form of the ID
    TBSTRJanitor janType(pszRealUID);

    IUPnPDevice* pDev;
    HRESULT hRes = pFinder->FindByUDN(janType.bsData, &pDev);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnP_UIDLookupFailed, hRes);
        return kCIDLib::False;
    }

    //
    //  If it didn't fail, but just didn't find anything, we return true but
    //  a null pointer.
    //
    if (!pDev)
    {
        pupnpdToFill = 0;
        return kCIDLib::True;
    }

    pupnpdToFill = new TKrnlUPnPDevice(pDev);
    return kCIDLib::True;
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
    // Get a pointer to our internal struct
    IUPnPDeviceFinder* pFinder = static_cast<IUPnPDeviceFinder*>(m_pData);

    // If there is already a finder, that's an error
    if (pFinder)
    {
        TKrnlError::SetLastError(kKrnlErrs::errcUPnP_AlreadyInit);
        return kCIDLib::False;
    }

    // Try to create the interface
    IUPnPDeviceFinder* pDevFinder;
    HRESULT hRes = ::CoCreateInstance
    (
        CLSID_UPnPDeviceFinder
        , NULL
        , CLSCTX_INPROC_SERVER
        , IID_IUPnPDeviceFinder
        , (void**)&pDevFinder
    );

    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnP_CreateFinder, hRes);
        return kCIDLib::False;
    }

    // It worked so store it in our data
    m_pData = pDevFinder;
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

    m_pFinderInfo(0)
    , m_pmkupnpfcbTar(0)
{
    // Create our per-platform info structure
    TAFinderInfo* pInfo = new TAFinderInfo;
    pInfo->pcbTar = 0;
    pInfo->pFinder = 0;
    m_pFinderInfo = pInfo;
}

// Set the callback up front
TKrnlUPnPAsyncFinder::
TKrnlUPnPAsyncFinder(MUPnPAsyncFinderCB* const pmkupnpfcbTar) :

    m_pFinderInfo(0)
    , m_pmkupnpfcbTar(pmkupnpfcbTar)
{
    //
    //  Create our per-platform info structure, and we can go ahead and set
    //  the callback object.
    //
    TAFinderInfo* pInfo = new TAFinderInfo;
    pInfo->pcbTar = 0;
    pInfo->pFinder = 0;
    m_pFinderInfo = pInfo;

    CUPnPFinderCallback* pcbNew = new CUPnPFinderCallback(pmkupnpfcbTar);
    pInfo->pcbTar = pcbNew;
}

TKrnlUPnPAsyncFinder::~TKrnlUPnPAsyncFinder()
{
    TAFinderInfo* pInfo = static_cast<TAFinderInfo*>(m_pFinderInfo);

    // Call our own cleanup method
    bCleanup();

    m_pFinderInfo = 0;
    delete pInfo;
}


// ---------------------------------------------------------------------------
//  TKrnlUPnPAsyncFinder: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Allows the CIDUpnP level async finder to get us to clean up before we
//  destruct, which migth be desirable in order to make sure it's done in
//  the same thread that careated the finder object. After this, the callback
//  has to be set again and a new search started.
//
tCIDLib::TBoolean TKrnlUPnPAsyncFinder::bCleanup()
{
    TAFinderInfo* pInfo = static_cast<TAFinderInfo*>(m_pFinderInfo);

    // Destroy the finder if set
    if (pInfo->pFinder)
    {
        pInfo->pFinder->Release();
        pInfo->pFinder = 0;
    }

    // And the callback
    if (pInfo->pcbTar)
    {
        pInfo->pcbTar->Release();
        pInfo->pcbTar = 0;
    }

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

    // Make sure that the callback has been set
    if (!pInfo->pcbTar)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    // Create a finder object for our purposes if not already
    if (!pInfo->pFinder)
    {
        IUPnPDeviceFinder* pNewFinder;
        HRESULT hRes = ::CoCreateInstance
        (
            CLSID_UPnPDeviceFinder
            , NULL
            , CLSCTX_INPROC_SERVER
            , IID_IUPnPDeviceFinder
            , (void**)&pNewFinder
        );

        if ((hRes != S_OK) || !pNewFinder)
        {
            TKrnlError::SetLastKrnlError
            (
                kKrnlErrs::errcUPnP_InitAsyncFinder, ::GetLastError()
            );
            return kCIDLib::False;
        }

        // We created it so store it away
        pInfo->pFinder = pNewFinder;
    }

    // Try to create a new find for our finder
    LONG lFindData = 0;
    HRESULT hRes;

    TBSTRJanitor janType(pszFindType);
    hRes = pInfo->pFinder->CreateAsyncFind
    (
        janType.bsData, 0, pInfo->pcbTar, &lFindData
    );

    if (hRes != S_OK)
    {
        TKrnlError::SetLastKrnlError
        (
            kKrnlErrs::errcUPnP_StartAsyncFind, ::GetLastError()
        );
        return kCIDLib::False;
    }

    // Start the find
    hRes = pInfo->pFinder->StartAsyncFind(lFindData);
    if (hRes != S_OK)
    {
        // Cancel the search since it didn't work
        pInfo->pFinder->CancelAsyncFind(lFindData);
        TKrnlError::SetLastKrnlError
        (
            kKrnlErrs::errcUPnP_StartAsyncFind, ::GetLastError()
        );
        return kCIDLib::False;
    }

    // Give back the search id
    tCIDLib::TCh achFindID[32];
    TRawStr::bFormatVal(lFindData, achFindID, 31);
    kstrSearchID.Set(achFindID);

    return kCIDLib::True;
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

    //
    //  Create the OS level callback object and point it at our callback. We
    //  have to add a ref to keep it alive.
    //
    CUPnPFinderCallback* pcbNew = new CUPnPFinderCallback(pmkupnpfcbTar);

    // And store it in our platform data
    pInfo->pcbTar = pcbNew;
    return kCIDLib::True;
}


// Cancel a previously started sarch
tCIDLib::TBoolean
TKrnlUPnPAsyncFinder::bStopListeningFor(const tCIDLib::TCh* const pszSearchID)
{
    TAFinderInfo* pInfo = static_cast<TAFinderInfo*>(m_pFinderInfo);

    // It has to be the long value we formatted into earlier
    tCIDLib::TBoolean bValid;
    LONG lID = TRawStr::i4AsBinary(pszSearchID, bValid, tCIDLib::ERadices::Dec);
    if (!bValid)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcUPnP_BadSearchID);
        return kCIDLib::False;
    }

    pInfo->pFinder->CancelAsyncFind(lID);
    return kCIDLib::True;
}


