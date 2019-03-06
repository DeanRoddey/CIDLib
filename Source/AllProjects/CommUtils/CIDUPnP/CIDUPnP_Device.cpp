//
// FILE NAME: CIDUPnP_Device.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/05/2011
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TUPnPDevice class.
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
RTTIDecls(TUPnPDevice,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TUPnPDevice
// PREFIX: upnpd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TUPnPDevice: Constructors and Destructor
// ---------------------------------------------------------------------------
TUPnPDevice::TUPnPDevice() :

    m_bServicesLoaded(kCIDLib::False)
    , m_c4SeqId(0)
    , m_pkupnpdThis(nullptr)
{
}

TUPnPDevice::~TUPnPDevice()
{
    // If we have a kernel device delete it
    delete m_pkupnpdThis;
}


// ---------------------------------------------------------------------------
//  TUPnPDevice: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Indicate whether we have any nested child devices or not
tCIDLib::TBoolean TUPnPDevice::bHasChildren() const
{
    CheckDevSet(CID_LINE);

    tCIDLib::TBoolean bRet;
    if (!m_pkupnpdThis->bHasChildren(bRet))
    {
        facCIDUPnP().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcDev_GetAttr
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(L"has children")
            , m_strName
        );
    }
    return bRet;
}


// Indicate whether we are a root device or not
tCIDLib::TBoolean TUPnPDevice::bIsRoot() const
{
    CheckDevSet(CID_LINE);

    tCIDLib::TBoolean bRet;
    if (!m_pkupnpdThis->bIsRoot(bRet))
    {
        facCIDUPnP().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcDev_GetAttr
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(L"is root")
            , m_strName
        );
    }
    return bRet;
}


//
//  Return a list of service ids for any services of this device that have the
//  indicated service type. It returns true if any were found.
//
tCIDLib::TBoolean
TUPnPDevice::bQueryServiceIDsByType(const   TString&                strType
                                    ,       tCIDLib::TStrCollect&   colFound) const
{
    CheckDevSet(CID_LINE);

    // If we've not loaded the service info yet, then do so
    if (!m_bServicesLoaded)
        LoadServices();

    //
    //  Run through the list, looking for ay with the indicated service type. The
    //  type URN is the key of the k/v pair.
    //
    const tCIDLib::TCard4 c4Count = m_colServices.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colServices[c4Index].strKey() == strType)
            colFound.objAdd(m_colServices[c4Index].strValue());
    }
    return !colFound.bIsEmpty();
}


//
//  Return our sequence id, set on us generally by the async finder but other
//  finder schemes could use this as well. We don't throw if the device isn't set.
//  The id will be zero if that's the case, telling the finder this one is not yet
//  used.
//
tCIDLib::TCard4 TUPnPDevice::c4SeqId() const
{
    return m_c4SeqId;
}


//
//  Return info about any child devices of this device. We return a key/values
//  object for each one. The key is the unique id of the device. Value 1 is the
//  name, and value 2 is the type URN.
//
//  They can also pass in a type filter, which will only return devices of the
//  indicated device type. It can empty to return them all.
//
tCIDLib::TVoid
TUPnPDevice::QueryChildDevices(         tCIDLib::TKValsCollect& colToFill
                                , const TString&                strTypeFilter) const
{
    CheckDevSet(CID_LINE);

    TKrnlLList<TKrnlKVPair> kllstDevices;
    const tCIDLib::TBoolean bRes = m_pkupnpdThis->bQueryChildDevices
    (
        kllstDevices
        , strTypeFilter.bIsEmpty() ? 0 : strTypeFilter.pszBuffer()
    );

    if (!bRes)
    {
        facCIDUPnP().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcDev_QueryChildDevs
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }

    // Copy them over to the caller's list
    colToFill.RemoveAll();

    if (kllstDevices.bResetCursor())
    {
        TKrnlKVPair* pkvpCur;
        while (kllstDevices.bNext(pkvpCur))
        {
            colToFill.objAdd
            (
                TKeyValues
                (
                    pkvpCur->pszKey()
                    , pkvpCur->pszValue()
                    , pkvpCur->pszValue2()
                )
            );
        }
    }
}


//
//  Returns the base device URL. We get the description URL, which is the only
//  one we have available to us, and pull out the address/port from it.
//
tCIDLib::TVoid TUPnPDevice::QueryDeviceURL(TString& strToFill) const
{
    // Get the description URL
    TString strDescrURL;
    QueryDevDescrURL(strDescrURL);

    // Parse it out
    TURL urlDescr(strDescrURL, tCIDSock::EQualified::Full);

    strToFill = L"http://";
    strToFill.Append(urlDescr.strHost());
    strToFill.Append(kCIDLib::chColon);
    strToFill.AppendFormatted(urlDescr.ippnHost());
}



//
//  Query the device description URL for this device. THis is the one to query the
//  device description XML from, and the base part of this URL is the base URL against
//  with the other (usually relative) URLs are based.
//
tCIDLib::TVoid TUPnPDevice::QueryDevDescrURL(TString& strToFill) const
{
    CheckDevSet(CID_LINE);

    TKrnlString kstrURL;
    if (!m_pkupnpdThis->bQueryDevDescrURL(kstrURL))
    {
        facCIDUPnP().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcDev_GetAttr
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(L"description URL")
            , m_strName
        );
    }

    strToFill = kstrURL.pszValue();
}


// Query the manufacturer of this device
tCIDLib::TVoid TUPnPDevice::QueryManufacturer(TString& strToFill) const
{
    CheckDevSet(CID_LINE);

    TKrnlString kstrURL;
    if (!m_pkupnpdThis->bQueryManufacturer(kstrURL))
    {
        facCIDUPnP().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcDev_GetAttr
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(L"manufacturer")
            , m_strName
        );
    }

    strToFill = kstrURL.pszValue();
}


//
//  Query a list of service objects for this device. These will be via the base
//  service interface, so only basic stuff can be done with these. So this is
//  mostly just for basic spelunking. You can of cousre look at the service type
//  to see what it's real type is and interact with it at the fundamental UPnP
//  leve. But, generally you'd use specific service class derivatives that provide
//  a typed interface to the service's capabilities.
//
tCIDLib::TVoid TUPnPDevice::QueryServices(tCIDUPnP::TSvcCollect& colFound)
{
    CheckDevSet(CID_LINE);

    // If we've not loaded the service info yet, then do so
    if (!m_bServicesLoaded)
        LoadServices();

    TUPnPService upnpsNew;
    const tCIDLib::TCard4 c4Count = m_colServices.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TString& strSrvcID = m_colServices[c4Index].strValue();

        //
        //  Create a new service object and call our own method that will set it up
        //  given the service id. Then add it to the caller's list.
        //
        try
        {
            SetServiceFromID(strSrvcID, upnpsNew);
            colFound.objAdd(upnpsNew);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
}


//
//  Return a list of service ids and service types for all the services of this
//  device. The service ids are the key and the service types are the values.
//
//  This is backwards from our internal list, so be sure to return our value for
//  the caller's key and vice versa!
//
tCIDLib::TVoid TUPnPDevice::QueryServices(tCIDLib::TKVPCollect& colFound) const
{
    CheckDevSet(CID_LINE);

    // If we've not loaded the service info yet, then do so
    if (!m_bServicesLoaded)
        LoadServices();

    const tCIDLib::TCard4 c4Count = m_colServices.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TKeyValuePair& kvalCur = m_colServices[c4Index];
        colFound.objAdd(TKeyValuePair(kvalCur.strValue(), kvalCur.strKey()));
    }
}


//
//  This allows the device to be released before this object is destroyed, which
//  is sometimes necessary. The underlying object should be destroyed within the
//  same thread that created it.
//
//  Any subsequent use of this object will cause a 'device not set' error until
//  it is subsquently set again.
//
tCIDLib::TVoid TUPnPDevice::Release()
{
    // Clean up any previously obtained service info
    m_colServices.RemoveAll();
    m_bServicesLoaded = kCIDLib::False;

    // Clean up the underlying kernel UPnP device
    delete m_pkupnpdThis;
    m_pkupnpdThis = 0;

    //
    //  We also need to reset the sequence number to zero. This insures that
    //  the async finder won't think we are already set up.
    //
    m_c4SeqId = 0;
}


// Provide accces to the info that we get when we are first set up
const TString& TUPnPDevice::strModel() const
{
    CheckDevSet(CID_LINE);
    return m_strModel;
}

const TString& TUPnPDevice::strName() const
{
    CheckDevSet(CID_LINE);
    return m_strName;
}

const TString& TUPnPDevice::strType() const
{
    CheckDevSet(CID_LINE);
    return m_strType;
}

const TString& TUPnPDevice::strUID() const
{
    CheckDevSet(CID_LINE);
    return m_strUID;
}


//
//  This can be called to set up a service to an already known unique device ID.
//  They can optionally provide a callback handler which we'll set for them as
//  a convenience.
//
tCIDLib::TVoid
TUPnPDevice::SetServiceFromID(  const   TString&                strSvcID
                                ,       TUPnPService&           upnpsToSet
                                ,       MUPnPSvcCallback* const pupnpscbHandler)
{
    CheckDevSet(CID_LINE);

    // See if we can look up the service
    TKrnlUPnPService* pkupnpsNew;
    if (!m_pkupnpdThis->bQueryServiceByID(strSvcID.pszBuffer(), pkupnpsNew)
    ||  !pkupnpsNew)
    {
        facCIDUPnP().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcDev_GetSvc
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strSvcID
            , m_strName
        );
    }

    // Get the service type ID of the service we found
    TKrnlString kstrTypeID;
    if (!pkupnpsNew->bQueryServiceType(kstrTypeID))
    {
        // Can't get it, so release the object and throw
        delete pkupnpsNew;
        facCIDUPnP().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcSvc_GetAttr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(L"type id")
            , strSvcID
        );
    }

    // Make sure it's the right type for the derived class
    TString strTypeID(kstrTypeID.pszValue());
    if (!upnpsToSet.bCheckServiceType(strTypeID))
    {
        // It cannot, so release the object and throw
        delete pkupnpsNew;
        facCIDUPnP().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcDbg_WrongType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
            , strTypeID
            , clsIsA()
        );
    }

    //
    //  Put it on the passed service object. The ref count was set appropriately
    //  by the query method above.
    //
    upnpsToSet.SetKrnlObj(pkupnpsNew, strTypeID, pupnpscbHandler);
}



// ---------------------------------------------------------------------------
//  TUPnPDevice: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Called (initially) with the <device> element of the root device entry. We recursively
//  iterate until we find our own device. At each recursion, xtnodeDev is the new
//  <device> element we are recursing on.
//
tCIDLib::TBoolean
TUPnPDevice::bParseDevice(const TXMLTreeElement& xtnodeDev, TString& strTmp) const
{
    tCIDLib::TCard4 c4At;

    // Find the <UDN> and <deviceType> text and see if they match ours
    const TXMLTreeElement* pxtnodeUDN = xtnodeDev.pxtnodeFindElement
    (
        L"UDN", 0, c4At
    );

    const TXMLTreeElement* pxtnodeType = xtnodeDev.pxtnodeFindElement
    (
        L"deviceType", 0, c4At
    );

    // If we found both elements, test them, else continue with child devices
    if (pxtnodeUDN && pxtnodeType)
    {
        //
        //  We have to remove the prefix that windows puts on the UDN, since we'd
        //  not match right otherwise, because that prefix is not in the XML.
        //
        strTmp = pxtnodeUDN->xtnodeChildAtAsText(0).strText();
        if (strTmp.bStartsWithI(L"uuid:"))
            strTmp.Cut(0, 5);

        if ((strTmp == m_strUID)
        &&  (pxtnodeType->xtnodeChildAtAsText(0).strText() == m_strType))
        {
            // It's us, so let's parse the services and return success
            ParseServiceList(xtnodeDev, strTmp);
            return kCIDLib::True;
        }
    }

    //
    //  Not this one, so see if there is a device list under this guy. If so, then
    //  we want to iterate it and recurse on each one.
    //
    const TXMLTreeElement* pxtnodeChildren = xtnodeDev.pxtnodeFindElement
    (
        L"deviceList", 0, c4At
    );
    tCIDLib::TBoolean bRet = kCIDLib::False;
    if (pxtnodeChildren)
    {
        // Else let's iterate it's list of child devices and recurse
        const tCIDLib::TCard4 c4Count = pxtnodeChildren->c4ChildCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            const TXMLTreeNode& xtnodeCur = pxtnodeChildren->xtnodeChildAt(c4Index);
            if (xtnodeCur.eType() != tCIDXML::ENodeTypes::Element)
                continue;

            // Watch for elements nodes of type <device>
            const TXMLTreeElement& xtnodeChild = static_cast<const TXMLTreeElement&>
            (
                xtnodeCur
            );
            if (!xtnodeChild.strQName().bCompareI(L"device"))
                continue;

            // Its a device child so recurse. If success, break out and return
            bRet = bParseDevice(xtnodeChild, strTmp);
            if (bRet)
                break;
        }
    }

    // Return whether we found it at this level or not
    return bRet;
}



// Called to make sure we are set up, if not it throws
tCIDLib::TVoid TUPnPDevice::CheckDevSet(const tCIDLib::TCard4 c4Line) const
{
    if (!m_pkupnpdThis)
    {
        facCIDUPnP().ThrowErr
        (
            CID_FILE
            , c4Line
            , kUPnPErrs::errcDbg_NotInit
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
            , TString(L"device")
        );
    }
}


//
//  Called to fault in the service list upon first access of any service or iteration
//  of services. We have to do an HTTP GET on this device's description document URL.
//  We then parse the resulting XML and iterate through it to find our device. When we
//  do, we parse out the services list to our m_colServices collection.
//
tCIDLib::TVoid TUPnPDevice::LoadServices() const
{
    // Get our own device description URL
    TString strDescrURL;
    QueryDevDescrURL(strDescrURL);
    TURL urlToGet(strDescrURL, tCIDSock::EQualified::Full);

    // Create an HTTP object and query the XML contents of this URL
    tCIDLib::TCard4     c4ContLen = 0;
    tCIDLib::TKVPList   colInHdrs;
    tCIDLib::TKVPList   colOutHdrs;
    tCIDNet::EHTTPCodes eCodeType;
    TString             strContType;
    TString             strRepLine;
    THeapBuf            mbufContent(8192, 256 * 1024UL, 64 * 1024UL);
    THTTPClient         httpcLoad;

    tCIDLib::TCard4 c4Res = 0;
    TString strErrMsg;
    try
    {
        // Pass a null data source, which will do a one shot, HTTP 1.0 exchange
        c4Res = httpcLoad.c4SendGet
        (
            nullptr
            , urlToGet
            , TTime::enctNowPlusSecs(5)
            , L"CIDLib UPnP/1.0"
            , L"text/xml; application/xml"
            , eCodeType
            , strRepLine
            , colOutHdrs
            , strContType
            , mbufContent
            , c4ContLen
            , kCIDLib::False
            , colInHdrs
        );

        if (c4Res != kCIDNet::c4HTTPStatus_OK)
        {
            // Set up an error msg for throwing below
            strErrMsg = L"Got HTTP code ";
            strErrMsg.AppendFormatted(c4Res);
            strErrMsg.Append(L" querying device description XML");
        }
         else
        {
            //
            //  That worked, so get the encoding of the returned XML and call a
            //  helper to parse it and kick off the processing of the XML tree.
            //
            TString strEncoding;
            TNetCoreParser::bParseTextEncoding(strContType, strEncoding);
            ParseDeviceDescr(mbufContent, c4ContLen, strEncoding);
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);

        // Fall through to throw the real error
        strErrMsg = errToCatch.strErrText();
    }

    // If we didn't fail outright, but didn't get a success indicator, give up
    if (c4Res != kCIDNet::c4HTTPStatus_OK)
    {
        facCIDUPnP().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcDev_QueryServices
            , strErrMsg
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


//
//  This is called with the contents of the device description XML document. We have
//  to parse it, find our device entry by locating a <device> block with our device
//  type and device UDN, and then find the serice list and parse out all of the
//  services, storing them in m_colServices.
//
tCIDLib::TVoid
TUPnPDevice::ParseDeviceDescr(  const   TMemBuf&        mbufContent
                                , const tCIDLib::TCard4 c4ContLen
                                , const TString&        strEncoding) const
{
    //
    //  Create a memory buffer entity source for the incoming data and give it the
    //  encoding to use for decoding the data. And put it into a source ref to pass
    //  in to the parser.
    //
    TMemBufEntitySrc* pxesDescr = new TMemBufEntitySrc
    (
        L"UPNPDeviceDescr", mbufContent, c4ContLen
    );
    tCIDXML::TEntitySrcRef esrItems(pxesDescr);
    pxesDescr->strEncoding(strEncoding);

    //
    //  Now create a parser and try to parse the source entity. Tell it we just want
    //  content (elements and text.)
    //
    TXMLTreeParser xtprsDev;
    tCIDLib::TBoolean bRes = xtprsDev.bParseRootEntity
    (
        esrItems, tCIDXML::EParseOpts::None, tCIDXML::EParseFlags::JustContent
    );

    if (!bRes)
    {
        // Throw the first error
        const TXMLTreeParser::TErrInfo& errCur = xtprsDev.colErrors()[0];

        TTextStringOutStream strmErr(errCur.strText().c4Length() + 32);
        strmErr << L"(Line/Col=" << errCur.c4Line() << L"/" << errCur.c4Column()
                << L") - " << errCur.strText() << kCIDLib::FlushIt;

        facCIDUPnP().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcDev_ParseDevXML
            , strmErr.strData()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }

    //
    //  OK, we survived that, so let's iterate through the XML. We start at the
    //  root device. There should be a top level device element under the root
    //  element. This will throw if no such element.
    //
    tCIDLib::TCard4 c4At;
    const TXMLTreeElement& xtnodeRoot = xtprsDev.xtdocThis().xtnodeRoot();
    const TXMLTreeElement& xtnodeDev = xtnodeRoot.xtnodeFindElement
    (
        L"device", 0, c4At
    );

    //
    //  We can now start a recursive operation to find our device info and its
    //  services. If it returns success, then we have loaded our services.
    //
    m_colServices.RemoveAll();
    TString strTmp;
    m_bServicesLoaded = bParseDevice(xtnodeDev, strTmp);
}


//
//  Called when we find our own device in the device description XML. We find the
//  the service list underneath it and load up our list of service info k/v pairs
//  from that.
//
tCIDLib::TVoid
TUPnPDevice::ParseServiceList(const TXMLTreeElement& xtnodeDev, TString& strTmp) const
{
    // Find the <serviceList> node and let's iterate that
    tCIDLib::TCard4 c4At;
    const TXMLTreeElement* pxtnodeList = xtnodeDev.pxtnodeFindElement
    (
        L"serviceList", 0, c4At
    );

    // If not, then we are done. It has no services, which is wierd but don't die
    if (!pxtnodeList)
    {
        facCIDUPnP().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcDev_NoServices
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
            , m_strName
        );
        return;
    }

    // Iterate them and store the info
    TString strType;
    const tCIDLib::TCard4 c4Count = pxtnodeList->c4ChildCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TXMLTreeNode& xtnodeCur = pxtnodeList->xtnodeChildAt(c4Index);
        if (xtnodeCur.eType() != tCIDXML::ENodeTypes::Element)
            continue;

        const TXMLTreeElement& xtnodeSrv = static_cast<const TXMLTreeElement&>
        (
            xtnodeCur
        );
        if (!xtnodeSrv.strQName().bCompareI(L"service"))
            continue;

        // Get the type and service id, both must be present
        const TXMLTreeElement* pxtnodeType = xtnodeSrv.pxtnodeFindElement
        (
            L"serviceType", 0, c4At
        );

        const TXMLTreeElement* pxtnodeId = xtnodeSrv.pxtnodeFindElement
        (
            L"serviceId", 0, c4At
        );

        if (pxtnodeType && pxtnodeId)
        {
            strTmp = pxtnodeId->xtnodeChildAtAsText(0).strText();
            strType = pxtnodeType->xtnodeChildAtAsText(0).strText();
            m_colServices.objAdd(TKeyValuePair(strType, strTmp));
        }
    }
}


//
//  Set us up with the info for a device. This is called by the facility
//  class, which acts as a factory for devices. It's also called by the async
//  finder which can set up device objects based on already stored info in
//  it's device list.
//
tCIDLib::TVoid
TUPnPDevice::SetKrnlObj(        TKrnlUPnPDevice* const  pkupnpdToAdopt
                        , const tCIDLib::TCh* const     pszUID
                        , const tCIDLib::TCh* const     pszName
                        , const tCIDLib::TCh* const     pszModel
                        , const tCIDLib::TCh* const     pszType
                        , const tCIDLib::TCard4         c4SeqId)
{
    // Release any current device we have and store the new one
    delete m_pkupnpdThis;
    m_pkupnpdThis = pkupnpdToAdopt;

    //
    //  Make sure the service stuff is cleared out. It'll have to be reloaded for
    //  the new device (if ever accessed.)
    //
    m_bServicesLoaded = kCIDLib::False;
    m_colServices.RemoveAll();

    m_c4SeqId = c4SeqId;
    m_strModel = pszModel;
    m_strName = pszName;
    m_strType = pszType;
    m_strUID = pszUID;
}




