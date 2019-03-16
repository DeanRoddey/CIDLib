//
// FILE NAME: CIDUPnP_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/15/2011
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
//  This file implements TFacCIDUPnP class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDUPnP_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDUPnP,TFacility)


// ---------------------------------------------------------------------------
//  To force on some verbose logging when needed
// ---------------------------------------------------------------------------
//#define VERBOSE_LOGGING 1



// ---------------------------------------------------------------------------
//  CLASS: TFacCIDUPnP
// PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDUPnP: Public, static data
// ---------------------------------------------------------------------------

//
//  The values for booleans that we get when querying service state variables
//  or getting reports via events or getting back boolean action parameters.
//  The platform code should insure that these values are returned.
//
const TString TFacCIDUPnP::strUPnP_False(L"False");
const TString TFacCIDUPnP::strUPnP_True(L"True");



// ---------------------------------------------------------------------------
//  TFacCIDUPnP: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDUPnP::TFacCIDUPnP() :

    TFacility
    (
        L"CIDUPnP"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}

TFacCIDUPnP::~TFacCIDUPnP()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDUPnP: Public, non-virtual methods
// ---------------------------------------------------------------------------


// Get the root device of this device
tCIDLib::TVoid TUPnPDevice::GetRootDevice(TUPnPDevice& upnpdToSet)
{
    TKrnlUPnPDevice* pkupnpdRoot;
    if (!m_pkupnpdThis->bGetRootDevice(pkupnpdRoot))
    {
        facCIDUPnP().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcDev_GetRootDev
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
        );
    }

    // If the pointer is null, then we already are the root device
    if (!pkupnpdRoot)
    {
        facCIDUPnP().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcDev_AlreadyRoot
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
        );
    }

    // We got the device so query the basic info we store
    TKrnlString kstrName;
    TKrnlString kstrUID;
    TKrnlString kstrModel;
    TKrnlString kstrType;

    if (!pkupnpdRoot->bQueryDevInfo(kstrUID, kstrName, kstrModel, kstrType))
    {
        delete pkupnpdRoot;
        facCIDUPnP().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcDev_QueryDevInfo
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    //
    //  It all worked so set up the device object.
    //
    //  We pass a zero for the sequence id. The async finder uses it, but
    //  we don't set it here.
    //
    upnpdToSet.SetKrnlObj
    (
        pkupnpdRoot
        , kstrUID.pszValue()
        , kstrName.pszValue()
        , kstrModel.pszValue()
        , kstrType.pszValue()
        , 0
    );
}




//
//  Search the passed list of devices and find one with the indicated type. If
//  one is found, set up the passed device for that device UID. We can optimally
//  throw or return a false if not found.
//
tCIDLib::TBoolean
TFacCIDUPnP::bSetDeviceByType(  const   tCIDLib::TKValsList&    colDevs
                                ,       TUPnPDevice&            upnpdTar
                                , const TString&                strType
                                , const tCIDLib::TBoolean       bThrowIfNot)
{
    tCIDLib::TKValsList::TCursor cursList(&colDevs);
    for (; cursList; ++cursList)
    {
        if (cursList->strVal2() == strType)
            break;
    }

    if (!cursList)
    {
        // Never found one, so throw or return false
        if (bThrowIfNot)
        {
            facCIDUPnP().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kUPnPErrs::errcDev_TypeNotFound
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , strType
            );
        }
        return kCIDLib::False;
    }

    return bSetDeviceFromUID(cursList.objRCur().strKey(), upnpdTar);
}



//
//  Set up this device by UID. Any previous device will be released. We return
//  a boolean to indicate whether we successfully found the id and set up the
//  device or not.
//
tCIDLib::TBoolean
TFacCIDUPnP::bSetDeviceFromUID(const TString& strUID, TUPnPDevice& upnpdToSet)
{
    #if defined(VERBOSE_LOGGING)
    facCIDUPnP().LogMsg
    (
        CID_FILE
        , CID_LINE
        , TString(L"Setting device from ID: ") + strUID
        , tCIDLib::ESeverities::Status
        , tCIDLib::EErrClasses::AppStatus
    );
    #endif

    // Create a kernel level finder and look for this guy
    TKrnlUPnPFinder kupnpfLookup;

    TKrnlUPnPDevice* pkupnpdNew = nullptr;
    if (!kupnpfLookup.bSearchByUID(strUID.pszBuffer(), pkupnpdNew))
    {
        #if defined(VERBOSE_LOGGING)
        facCIDUPnP().LogMsg
        (
            CID_FILE
            , CID_LINE
            , TString(L"Lookup error for UID: ") + strUID
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
        );
        #endif

        // An error occured during lookup
        facCIDUPnP().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcFnd_FindByID
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strUID
        );
    }

    // It just wasn't found, no error
    if (!pkupnpdNew)
    {
        #if defined(VERBOSE_LOGGING)
        facCIDUPnP().LogMsg
        (
            CID_FILE
            , CID_LINE
            , TString(L"Did not find device by UID: ") + strUID
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
        );
        #endif
        return kCIDLib::False;
    }

    #if defined(VERBOSE_LOGGING)
    facCIDUPnP().LogMsg
    (
        CID_FILE
        , CID_LINE
        , TString(L"Querying device info for UID:") + strUID
        , tCIDLib::ESeverities::Status
        , tCIDLib::EErrClasses::AppStatus
    );
    #endif

    //
    //  Get the device info so that we can keep it around for error messages
    //  and such. It'll be too late to get it if something goes wrong with
    //  the device object.
    //
    TKrnlString kstrUID;
    TKrnlString kstrName;
    TKrnlString kstrModel;
    TKrnlString kstrType;
    if (!pkupnpdNew->bQueryDevInfo(kstrUID, kstrName, kstrModel, kstrType))
    {
        #if defined(VERBOSE_LOGGING)
        facCIDUPnP().LogMsg
        (
            CID_FILE
            , CID_LINE
            , TString(L"Failed to get device info for UID:") + strUID
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
        );
        #endif

        // Let the device object go
        delete pkupnpdNew;

        facCIDUPnP().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcDev_GetAttr
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , TString(L"general info")
        );
    }

    #if defined(VERBOSE_LOGGING)
    facCIDUPnP().LogMsg
    (
        CID_FILE
        , CID_LINE
        , TString(L"Successfully set up device for UID:") + strUID
        , tCIDLib::ESeverities::Status
        , tCIDLib::EErrClasses::AppStatus
    );

    if (strUID != kstrUID.pszValue())
    {
        facCIDUPnP().LogMsg
        (
            CID_FILE
            , CID_LINE
            , TString(L"Retrieved UID was different from found UID: ")
              + TString(kstrUID.pszValue())
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
        );
    }
    #endif

    //
    //  Set up the target object. The device object is already correctly
    //  reference counted by the query above. So we don't need to bump it.
    //
    //  We pass a zero for the sequence id. The async finder uses it, but
    //  we don't set it here.
    //
    upnpdToSet.SetKrnlObj
    (
        pkupnpdNew
        , kstrUID.pszValue()
        , kstrName.pszValue()
        , kstrModel.pszValue()
        , kstrType.pszValue()
        , 0
    );
    return kCIDLib::True;
}



//
//  We return a list of key/values. The key is the unique device id. Value 1
//  is the user readable name, and value 2 is the type URN. That's the order
//  we get them from the kernel level object as well.
//
tCIDLib::TBoolean
TFacCIDUPnP::bFindDevsByType(const  TString&                    strType
                            ,       tCIDLib::TKValsCollect&     colFound)
{
    // Create a finder and try the search
    TKrnlLList<TKrnlKVPair> kllistDevs;
    TKrnlUPnPFinder kupnpfSearch;
    if (!kupnpfSearch.bSearchByType(strType.pszBuffer(), kllistDevs))
    {
        facCIDUPnP().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcFnd_FindDevByType
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strType
        );
    }

    colFound.RemoveAll();
    if (kllistDevs.bResetCursor())
    {
        TKrnlKVPair* pkkvpCur;
        while (kllistDevs.bNext(pkkvpCur))
        {
            colFound.objAdd
            (
                TKeyValues
                (
                    pkkvpCur->pszKey(), pkkvpCur->pszValue(), pkkvpCur->pszValue2()
                )
            );
        }
    }

    return !colFound.bIsEmpty();
}


//
//  Search the passed list of services (which should be valid for the passed
//  device type) for one of the indicated type. If we find it, then set up
//  the service with this id.
//
tCIDLib::TBoolean
TFacCIDUPnP::bSetServiceByType( const   tCIDLib::TKVPList&  colServices
                                ,       TUPnPDevice&        upnpdPar
                                ,       TUPnPService&       upnpsTar
                                , const TString&            strType
                                , const tCIDLib::TBoolean   bThrowIfNot)
{
    tCIDLib::TKVPList::TCursor cursList(&colServices);
    for (; cursList; ++cursList)
    {
        if (cursList->strValue() == strType)
            break;
    }

    if (!cursList)
    {
        // Never found one, so throw or return false
        if (bThrowIfNot)
        {
            facCIDUPnP().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kUPnPErrs::errcSrv_TypeNotFound
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , strType
            );
        }
        return kCIDLib::False;
    }

    upnpdPar.SetServiceFromID(cursList->strKey(), upnpsTar);
    return kCIDLib::True;
}



//
//  Set up this device by UID. Any previous device will be released. We just
//  call the other version, which just returns a true/false found or not found
//  status, and we throw if not found. See the other version for details.
//
tCIDLib::TVoid
TFacCIDUPnP::SetDeviceFromUID(const TString& strUID, TUPnPDevice& upnpdToSet)
{
    if (!bSetDeviceFromUID(strUID, upnpdToSet))
    {
        facCIDUPnP().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcFnd_NoDevWithUID
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strUID
        );
    }
}


