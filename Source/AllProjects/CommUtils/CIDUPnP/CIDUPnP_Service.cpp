//
// FILE NAME: CIDUPnP_Service.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/05/2011
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
//  This file implements the TUPnPService class.
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
RTTIDecls(TUPnPService,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TUPnPService
// PREFIX: upnps
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TUPnPService: Constructors and Destructor
// ---------------------------------------------------------------------------
TUPnPService::TUPnPService() :

    m_bIsEvented(kCIDLib::False)
    , m_bIsDead(kCIDLib::False)
    , m_bLogErrs(kCIDLib::False)
    , m_c4SerialNum(1)
    , m_colVarList
      (
        109, TStringKeyOps(), &TKeyValuePair::strExtractKey, tCIDLib::EMTStates::Safe
      )
    , m_pkupnpsThis(nullptr)
{
}

TUPnPService::~TUPnPService()
{
    // If we have a kernel service delete it
    delete m_pkupnpsThis;
    m_pkupnpsThis = nullptr;
}


// ---------------------------------------------------------------------------
//  TUPnPService: Public, virtual methods
// ---------------------------------------------------------------------------

//
//  At this level we will accept any service type since our API is valid for
//  any type of service. Derived classes have to override this and check
//  accordingly.
//
tCIDLib::TBoolean TUPnPService::bCheckServiceType(const TString& strTypeID) const
{
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TUPnPService: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TUPnPService::UPnPServiceDied()
{
    //
    //  If the derived class doesn't override, we don't do anything but set
    //  an 'is dead' flag.
    //
    m_bIsDead = kCIDLib::True;
}


//
//  Update or add this variable. The add or update is atomic, but we also
//  need to atomically bump the serial number, so we need to explicitly
//  lock.
//
//  We only this is only called by the UPnP system, not by derived classes,
//  which make a protected call below. Derived classes override this to handle
//  any XML based composite event values, so this is called for single values
//  and so we can bump the serial number for each one.
//
tCIDLib::TVoid
TUPnPService::UPnPVarChanged(const  tCIDLib::TCh* const pszVarName
                            , const tCIDLib::TCh* const pszVarVal)
{
    TMtxLocker mtxlVars(m_colVarList.pmtxLock());

    //
    //  See if it's there or not. Since already have to lock because of the
    //  need to bump the serial number atomically, we go ahead nd look it
    //  up and only update the value if it's already there. Else we have to
    //  construct a key/value pair to add.
    //
    TKeyValuePair* pkvalVar = m_colVarList.pobjFindByKey(pszVarName);
    if (pkvalVar)
        pkvalVar->strValue(pszVarVal);
    else
        m_colVarList.objPlace(pszVarName, pszVarVal);


    m_c4SerialNum++;
    if (!m_c4SerialNum)
        m_c4SerialNum++;
}


// ---------------------------------------------------------------------------
//  TUPnPService: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Return our is evented flag
tCIDLib::TBoolean TUPnPService::bIsEvented() const
{
    return m_bIsEvented;
}


// Return our is dead flag
tCIDLib::TBoolean TUPnPService::bIsDead() const
{
    return m_bIsDead;
}


// Get/set the verbose logging flag
tCIDLib::TBoolean TUPnPService::bLogErrs() const
{
    return m_bLogErrs;
}

tCIDLib::TBoolean TUPnPService::bLogErrs(const tCIDLib::TBoolean bToSet)
{
    m_bLogErrs = bToSet;
    return m_bLogErrs;
}


// Return our serial number
tCIDLib::TCard4 TUPnPService::c4SerialNum() const
{
    return m_c4SerialNum;
}


//
//  Get the value of a single state var, if it is in our list. Else just
//  return false to indicate we don't have it. This version is usually
//  called by derivatives that get a value either by it being evented via
//  a composite XML return, or that invoke an action to get it. So we don't
//  want to use the other version below that will do a state variable query
//  for it if not found in the evented list, since that may fail for those
//  variables that are gotten via action.
//
//  We have one version that will return the serial number (for when eventing
//  is on and you want to know if changes have occurred.) Another one doesn't
//  return it for convenience to the caller.
//
tCIDLib::TBoolean
TUPnPService::bQueryStateVar(const  TString&            strVarName
                            ,       TString&            strValToFill
                            , const tCIDLib::TBoolean   bClear) const
{
    tCIDLib::TCard4 c4Dummy;
    return bQueryStateVar(strVarName, strValToFill, c4Dummy, bClear);
}

tCIDLib::TBoolean
TUPnPService::bQueryStateVar(const  TString&            strVarName
                            ,       TString&            strValToFill
                            ,       tCIDLib::TCard4&    c4SerialNum
                            , const tCIDLib::TBoolean   bClear) const
{
    // Make sure the service object has been set
    CheckSvcSet(CID_LINE);

    // If not evented, then clearly we won't have it
    if (!m_bIsEvented)
    {
        c4SerialNum = 1;
        return kCIDLib::False;
    }

    // See if it's in our list
    TMtxLocker mtxlVars(m_colVarList.pmtxLock());

    c4SerialNum = m_c4SerialNum;
    TKeyValuePair* pkvalVar = m_colVarList.pobjFindByKey(strVarName);
    if (pkvalVar)
    {
        strValToFill = pkvalVar->strValue();

        // If asked, clear it after reading
        if (bClear)
            pkvalVar->strValue(TString::strEmpty());
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// Enable event subscription for this service
tCIDLib::TVoid TUPnPService::EnableEvents()
{
    // Make sure the service object has been set
    CheckSvcSet(CID_LINE);

    // If already evented, then do nothing
    if (!m_bIsEvented)
    {
        // Pass ourself as the callback object
        m_bIsEvented = kCIDLib::True;
        m_pkupnpsThis->AddCallback(this);
    }
}


//
//  Get the value of a single state var. One will just get it into a string,
//  and another will add it to a collection of strings. If we are evented but
//  it is not found in our cached, then they can tell us to query for it. If
//  they say don't query, then we set the value to not implemented.
//
//  We have two versions of each, one of which will return our serial number,
//  for when eventing is enabled and the caller wants to know if changes have
//  occurred.
//
tCIDLib::TVoid
TUPnPService::QueryStateVar(const   TString&            strVarName
                            ,       TString&            strValToFill
                            ,       tCIDLib::TCard4&    c4SerialNum
                            , const tCIDLib::TBoolean   bQueryIfNot
                            , const tCIDLib::TBoolean   bClear) const
{
    // Make sure the service object has been set
    CheckSvcSet(CID_LINE);

    //
    //  If evented, check our cache of values first. If we find it there,
    //  return the value we have cached.
    //
    if (m_bIsEvented)
    {
        TMtxLocker mtxlVars(m_colVarList.pmtxLock());

        c4SerialNum = m_c4SerialNum;
        TKeyValuePair* pkvalVar = m_colVarList.pobjFindByKey(strVarName);
        if (pkvalVar)
        {
            strValToFill = pkvalVar->strValue();

            // If asked, clear it after read
            if (bClear)
                pkvalVar->strValue(TString::strEmpty());
            return;
        }
    }
     else
    {
        // The serial number won't be changing in this case, so no need to lock
        c4SerialNum = m_c4SerialNum;
    }

    //
    //  If we are not evented, or we are but they say to query if not, then
    //  do a query. Else, just set it to unimplemented.
    //
    if (!m_bIsEvented || bQueryIfNot)
    {
        TKrnlString kstrVal;
        if (!m_pkupnpsThis->bQueryStateVar(strVarName.pszBuffer(), kstrVal))
        {
            facCIDUPnP().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kUPnPErrs::errcSvc_QueryStateVar
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , strVarName
            );
        }
        strValToFill = kstrVal.pszValue();
    }
     else
    {
        // Set it to the not implemented value
        strValToFill = kCIDUPnP_::strVal_NotImpl;
    }
}

tCIDLib::TVoid
TUPnPService::QueryStateVar(const   TString&            strVarName
                            ,       TString&            strValToFill
                            , const tCIDLib::TBoolean   bQueryIfNot
                            , const tCIDLib::TBoolean   bClear) const
{
    // Call the other version and eat the serial number
    tCIDLib::TCard4 c4Dummy;
    QueryStateVar(strVarName, strValToFill, c4Dummy, bQueryIfNot, bClear);
}


tCIDLib::TVoid
TUPnPService::QueryStateVar(const   TString&                strVarName
                            ,       tCIDLib::TStrCollect&   colAddTo
                            ,       tCIDLib::TCard4&        c4SerialNum
                            , const tCIDLib::TBoolean       bQueryIfNot
                            , const tCIDLib::TBoolean       bClear) const
{
    // Call the other version to fill a local string
    TString strVal;
    QueryStateVar(strVarName, strVal, c4SerialNum, bQueryIfNot, bClear);
    colAddTo.objAdd(strVal);
}


tCIDLib::TVoid
TUPnPService::QueryStateVar(const   TString&                strVarName
                            ,       tCIDLib::TStrCollect&   colAddTo
                            , const tCIDLib::TBoolean       bQueryIfNot
                            , const tCIDLib::TBoolean       bClear) const
{
    // Call the other version to fill a local string, eat the serial number
    tCIDLib::TCard4 c4Dummy;
    TString strVal;
    QueryStateVar(strVarName, strVal, c4Dummy, bQueryIfNot);
    colAddTo.objAdd(strVal);
}



//
//  This allows the service to be released before this object is destroyed,
//  which is sometimes necessary. The underlying object should be destroyed
//  within the same thread that created it.
//
//  Any subsequent use of this object will cause a 'service not set' error
//  until it is subsquently set again.
//
//  We also bump the serial number as well, so any code using this object
//  will come back and check and see it's gone.
//
tCIDLib::TVoid TUPnPService::Release()
{
    delete m_pkupnpsThis;
    m_pkupnpsThis = 0;

    m_bIsDead = kCIDLib::True;
    m_bIsEvented = kCIDLib::False;
    m_colVarList.RemoveAll();

    // Let derived classes know it's being reset
    SvcReset();

    m_c4SerialNum++;
    if (!m_c4SerialNum)
        m_c4SerialNum++;
}


// Provide access to our type id
const TString& TUPnPService::strTypeID() const
{
    CheckSvcSet(CID_LINE);
    return m_strTypeID;
}



// ---------------------------------------------------------------------------
//  TUPnPService: Protected, virtual methods
// ---------------------------------------------------------------------------

//
//  This is called by our SetKrnlObject method, and our Reset() method, to
//  let the derived class reset any data it has that needs to be redone when
//  the service is no longer valid, or when a new one is being set. We just
//  provide a default implementation for most of them to use. We do our reset
//  there in the set method, so we don't have to depend on derived classes
//  calling us.
//
tCIDLib::TVoid TUPnPService::SvcReset()
{
}


// ---------------------------------------------------------------------------
//  TUPnPService: Protected, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Given the XML returned from various service queries, we'll try to parse
//  the text into the provided tree parser. If there's an error, we'll log
//  the info and throw. This cuts down on a lot of grunt work code in derived
//  classes.
//
tCIDLib::TBoolean
TUPnPService::bParseXML(        TXMLTreeParser& xtprsItems
                        , const TString&        strXMLText
                        , const TString&        strContName)
{
    //
    //  Parse the returned XML. This guy won't have an XMLDecl, so we have to
    //  force the encoding on the entity source to be the local wide character
    //  format, which is how it will come back to us from the UPnP query.
    //
    TMemBufEntitySrc* pxesItems = new TMemBufEntitySrc(strContName, strXMLText);
    pxesItems->strEncoding(kCIDLib::pszNativeWCEncoding);
    tCIDXML::TEntitySrcRef esrItems(pxesItems);

    tCIDLib::TBoolean bRes = xtprsItems.bParseRootEntity
    (
        esrItems, tCIDXML::EParseOpts::None, tCIDXML::EParseFlags::JustContent
    );

    if (!bRes && m_bLogErrs)
    {
        const TXMLTreeParser::TErrInfo& errCur = xtprsItems.colErrors()[0];
        TTextStringOutStream strmErr(errCur.strText().c4Length() + 32);
        strmErr << L"(Line/Col=" << errCur.c4Line() << L"/" << errCur.c4Column()
                << L") - " << errCur.strText() << kCIDLib::FlushIt;

        facCIDUPnP().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcSvc_SrvRepXMLParse
            , strmErr.strData()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , strContName
        );
    }
    return bRes;
}


//
//  To let our derived classes bump our serial number if they have to handle
//  variable changes, which some do because they get multiple values via an
//  XML dump. This serial number is mutable so this is a constant, as is the
//  variable list since we have to fault variables in from reads in derived
//  classes.
//
tCIDLib::TVoid TUPnPService::BumpSerialNum() const
{
    // We have to lock since we could need to do two operations if we wrap
    TMtxLocker mtxlVars(m_colVarList.pmtxLock());

    m_c4SerialNum++;
    if (!m_c4SerialNum)
        m_c4SerialNum++;
}


//
//  A helper for derived classes to verify that at least the right number of
//  parms came back. By default it checks for an exact match, but they can
//  tell us to make sure that it's just at least that many.
//
tCIDLib::TVoid
TUPnPService::CheckRetParmCnt(  const   TString&            strCmdName
                                , const tCIDLib::TCard4     c4OutCnt
                                , const tCIDLib::TCard4     c4ExpCnt
                                , const tCIDLib::TBoolean   bExact) const
{
    if ((bExact && (c4OutCnt != c4ExpCnt))
    ||  (!bExact && (c4OutCnt < c4ExpCnt)))
    {
        facCIDUPnP().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcSvc_WrongOutParmCnt
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , strCmdName
            , TCardinal(c4ExpCnt)
            , TCardinal(c4OutCnt)
        );
    }
}


//
//  This is the method that derived classes call in order to invoke an
//  action. They pass in a command, a list of input parameters, and a
//  collection to add any output parameters to.
//
tCIDLib::TVoid
TUPnPService::InvokeAction( const   TString&                strCmd
                            , const tCIDLib::TStrCollect&   colInParms
                            ,       tCIDLib::TStrCollect&   colOutParms) const
{
    // Make sure the service object has been set
    CheckSvcSet(CID_LINE);

    const tCIDLib::TCard4 c4InCnt = colInParms.c4ElemCount();
    if (c4InCnt > 32)
    {
        facCIDUPnP().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcSvc_TooManyInParms
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , strCmd
        );
    }

    // Set up the input param list
    const tCIDLib::TCh* apszInParms[32];

    TColCursor<TString>* pcursIn = colInParms.pcursNew();
    TJanitor<TColCursor<TString> > janCursor(pcursIn);
    tCIDLib::TCard4 c4InInd = 0;
    for (; pcursIn->bIsValid(); pcursIn->bNext())
        apszInParms[c4InInd++] = pcursIn->objRCur().pszBuffer();

    CIDAssert(c4InInd == c4InCnt, L"Internal error gathering input parms");

    TKrnlString kstrRes;
    TKrnlLList<TKrnlString> kllistOutParms;
    const tCIDLib::TBoolean bRes = m_pkupnpsThis->bInvokeAct
    (
        strCmd.pszBuffer(), apszInParms, c4InCnt, kllistOutParms, kstrRes
    );

    if (!bRes)
    {
        facCIDUPnP().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcSvc_InvokeAction
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strCmd
            , m_strTypeID
            , TString(kstrRes.pszValue())
        );
    }

    // If any out parms, get them
    colOutParms.RemoveAll();
    if (kllistOutParms.bResetCursor())
    {
        TKrnlString* pkstrCur;
        while (kllistOutParms.bNext(pkstrCur))
            colOutParms.objAdd(TString(pkstrCur->pszValue()));
    }
}



//
//  This is a method available to the device class so that it can set up service
//  objects. Only a device class can do this, since only it knows about the
//  services.
//
//  The caller provides us with the type of of the underlying kernel service
//  object. It is his responsibiltiy to check if it's appropriate to set on
//  the type of service derivative this object actually is. Though, in debug
//  mode we check it also and throw if bad.
//
//  He can also provide a callback handler which we'll install for him if
//  non-zero. The handler is not adopted, the caller remains responsible for
//  it and it might be pointed to by multiple services.
//
//  Note that we don't change the serial number, we actually bump it, so that
//  it won't be the same as anyone who last looked at this object.
//
tCIDLib::TVoid
TUPnPService::SetKrnlObj(       TKrnlUPnPService* const pkupnpsToAdopt
                        , const TString&                strTypeID
                        ,       MUPnPSvcCallback* const pupnpscbHandler)
{
    // If in debug mode, redundantly check the service type
    #if CID_DEBUG_ON
    if (!bCheckServiceType(strTypeID))
    {
        delete pkupnpsToAdopt;
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
    #endif

    // If we have an object already, let it go
    delete m_pkupnpsThis;
    m_pkupnpsThis = 0;

    m_pkupnpsThis = pkupnpsToAdopt;
    m_strTypeID = strTypeID;
    m_bIsEvented = kCIDLib::False;
    m_bIsDead = kCIDLib::False;
    m_colVarList.RemoveAll();

    m_c4SerialNum++;
    if (!m_c4SerialNum)
        m_c4SerialNum++;

    // Let derived classes know it's being reset
    SvcReset();

    if (pupnpscbHandler)
        m_pkupnpsThis->AddCallback(pupnpscbHandler);
}


//
//  Sometimes our derived classes get composite evented values as XML. They
//  have to override the state var changed method and parse the XML and then
//  they call this to store the state variable info.
//
//  Note that we don't bump the serial number. The derived class must do
//  this once he has stored all of the new variable values. Otherwise,
//  callers would see a change and get data before he's stored it all.
//
//  He doesn't lock us, he just calls us repeatedly, but only bumps the
//  serial number at the end. If anyone checks a value in between that's
//  fine, since they won't see a new serial number until the setting is
//  done.
//
//  This is const since it has to be available to fault in values upon read in
//  the derived classes. The variable list is mutable.
//
tCIDLib::TVoid
TUPnPService::SetStateVar(  const   TString& strVarName
                            , const TString& strVarVal) const
{
    TMtxLocker mtxlVars(m_colVarList.pmtxLock());

    //
    //  For efficiency we lock first and do a check to see if we can
    //  just update the existing value, without having to create a
    //  key/val pair just to find out it's already in the list.
    //
    TKeyValuePair* pkvalVar = m_colVarList.pobjFindByKey(strVarName);
    if (pkvalVar)
        pkvalVar->strValue(strVarVal);
    else
        m_colVarList.objPlace(strVarName, strVarVal);
}



// A helper to throw an exception if output parms are invalid
tCIDLib::TVoid
TUPnPService::ThrowBadOutActParms(  const   TString&        strCmdName
                                    , const tCIDLib::TCard4 c4Line) const
{
    facCIDUPnP().ThrowErr
    (
        CID_FILE
        , c4Line
        , kUPnPErrs::errcSvc_InvalidOutParms
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotReady
        , strCmdName
    );
}


// ---------------------------------------------------------------------------
//  TUPnPService: Private, non-virtual methods
// ---------------------------------------------------------------------------

// An inline is used to check if the service is set. If not it calls this
tCIDLib::TVoid TUPnPService::ThrowNotSet(const tCIDLib::TCard4 c4Line) const
{
    facCIDUPnP().ThrowErr
    (
        CID_FILE
        , c4Line
        , kUPnPErrs::errcDbg_NotInit
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotReady
        , TString(L"service")
    );
}


