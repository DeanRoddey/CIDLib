//
// FILE NAME: CIDOrbUC_NSInfo.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/17/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "CIDOrbUC_.hpp"


// ----------------------------------------------------------------------------
// Magic macros
// ----------------------------------------------------------------------------
AdvRTTIDecls(TNameServerInfo,TObject)
AdvRTTIDecls(TNSRebindInfo, TNameServerInfo)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDOrbUC_NSInfo
{
    // -----------------------------------------------------------------------
    //  Our NS info persistent format version number. We aren't actually ever
    //  persisted, but in case of the need to support older clients we do
    //  version our data.
    //
    //  Version 2 -
    //      Added support for the three extra string values.
    //
    //  Version 3 -
    //      Added support for a fourth extra string value
    // -----------------------------------------------------------------------
    const tCIDLib::TCard2   c2NSInfoFmtVersion = 3;


    // -----------------------------------------------------------------------
    //  The persistent format for the derived rebind info class
    // -----------------------------------------------------------------------
    const tCIDLib::TCard2   c2NSRBIFmtVersion = 1;
}



// ---------------------------------------------------------------------------
//   CLASS: TNameServerInfo
//  PREFIX: nsvi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TNameServerInfo: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::ESortComps
TNameServerInfo::eCompOnName(const  TNameServerInfo&    nsvi1
                            , const TNameServerInfo&    nsvi2)
{
    return nsvi1.m_strNodeName.eCompare(nsvi2.m_strNodeName);
}


tCIDLib::ESortComps
TNameServerInfo::eCompOnDescription(const   TNameServerInfo&    nsvi1
                                    , const TNameServerInfo&    nsvi2)
{
    return nsvi1.m_strDescription.eCompare(nsvi2.m_strDescription);
}


// ---------------------------------------------------------------------------
//  TNameServerInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TNameServerInfo::TNameServerInfo()
{
}

TNameServerInfo::TNameServerInfo(const  TOrbObjId&  ooidSrvObject
                                , const TString&    strNodeName
                                , const TString&    strDescription) :

    m_ooidSrvObject(ooidSrvObject)
    , m_strDescription(strDescription)
    , m_strNodeName(strNodeName)
{
}

TNameServerInfo::TNameServerInfo(const  TOrbObjId&  ooidSrvObject
                                , const TString&    strNodeName
                                , const TString&    strDescription
                                , const TString&    strExtra1) :

    m_ooidSrvObject(ooidSrvObject)
    , m_strDescription(strDescription)
    , m_strExtra1(strExtra1)
    , m_strNodeName(strNodeName)
{
}

TNameServerInfo::TNameServerInfo(const  TOrbObjId&  ooidSrvObject
                                , const TString&    strNodeName
                                , const TString&    strDescription
                                , const TString&    strExtra1
                                , const TString&    strExtra2) :

    m_ooidSrvObject(ooidSrvObject)
    , m_strDescription(strDescription)
    , m_strExtra1(strExtra1)
    , m_strExtra2(strExtra2)
    , m_strNodeName(strNodeName)
{
}

TNameServerInfo::TNameServerInfo(const  TOrbObjId&  ooidSrvObject
                                , const TString&    strNodeName
                                , const TString&    strDescription
                                , const TString&    strExtra1
                                , const TString&    strExtra2
                                , const TString&    strExtra3
                                , const TString&    strExtra4) :

    m_ooidSrvObject(ooidSrvObject)
    , m_strDescription(strDescription)
    , m_strExtra1(strExtra1)
    , m_strExtra2(strExtra2)
    , m_strExtra3(strExtra3)
    , m_strExtra4(strExtra4)
    , m_strNodeName(strNodeName)
{
}

TNameServerInfo::TNameServerInfo(const TNameServerInfo& nsviToCopy) :

    m_ooidSrvObject(nsviToCopy.m_ooidSrvObject)
    , m_strDescription(nsviToCopy.m_strDescription)
    , m_strExtra1(nsviToCopy.m_strExtra1)
    , m_strExtra2(nsviToCopy.m_strExtra2)
    , m_strExtra3(nsviToCopy.m_strExtra3)
    , m_strExtra4(nsviToCopy.m_strExtra4)
    , m_strNodeName(nsviToCopy.m_strNodeName)
{
}

TNameServerInfo::~TNameServerInfo()
{
}

// ---------------------------------------------------------------------------
//  TNameServerInfo: Public operators
// ---------------------------------------------------------------------------
TNameServerInfo& TNameServerInfo::operator=(const TNameServerInfo& nsviToAssign)
{
    if (this != &nsviToAssign)
    {
        m_ooidSrvObject     = nsviToAssign.m_ooidSrvObject;
        m_strDescription    = nsviToAssign.m_strDescription;
        m_strExtra1         = nsviToAssign.m_strExtra1;
        m_strExtra2         = nsviToAssign.m_strExtra2;
        m_strExtra3         = nsviToAssign.m_strExtra3;
        m_strExtra4         = nsviToAssign.m_strExtra4;
        m_strNodeName       = nsviToAssign.m_strNodeName;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TNameServerInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TOrbObjId& TNameServerInfo::ooidSrvObject() const
{
    return m_ooidSrvObject;
}


const TString& TNameServerInfo::strExtra1() const
{
    return m_strExtra1;
}

const TString& TNameServerInfo::strExtra1(const TString& strToSet)
{
    m_strExtra1 = strToSet;
    return m_strExtra1;
}



const TString& TNameServerInfo::strExtra2() const
{
    return m_strExtra2;
}

const TString& TNameServerInfo::strExtra2(const TString& strToSet)
{
    m_strExtra2 = strToSet;
    return m_strExtra2;
}


const TString& TNameServerInfo::strExtra3() const
{
    return m_strExtra3;
}

const TString& TNameServerInfo::strExtra3(const TString& strToSet)
{
    m_strExtra3 = strToSet;
    return m_strExtra3;
}


const TString& TNameServerInfo::strExtra4() const
{
    return m_strExtra4;
}

const TString& TNameServerInfo::strExtra4(const TString& strToSet)
{
    m_strExtra4 = strToSet;
    return m_strExtra4;
}


const TString& TNameServerInfo::strDescription() const
{
    return m_strDescription;
}


const TString& TNameServerInfo::strNodeName() const
{
    return m_strNodeName;
}



// ---------------------------------------------------------------------------
//  TNameServerInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TNameServerInfo::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Make sure we get the start marker
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom  >> c2FmtVersion;
    if (!c2FmtVersion || (c2FmtVersion > CIDOrbUC_NSInfo::c2NSInfoFmtVersion))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_UnknownFmtVersion
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c2FmtVersion)
            , clsThis()
        );
    }

    // Looks reasonable, so stream in the data
    strmToReadFrom  >>  m_ooidSrvObject
                    >>  m_strNodeName
                    >>  m_strDescription;

    // If version 2 or beyond, stream in the extra values, else default them
    if (c2FmtVersion > 1)
    {
        strmToReadFrom  >> m_strExtra1
                        >> m_strExtra2
                        >> m_strExtra3;

        // If version 3 or beyond, there's a fourth value
        if (c2FmtVersion > 2)
            strmToReadFrom >> m_strExtra4;
        else
            m_strExtra4.Clear();
    }
     else
    {
        m_strExtra1.Clear();
        m_strExtra2.Clear();
        m_strExtra3.Clear();
        m_strExtra4.Clear();
    }

    // Make sure we get the end marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}


tCIDLib::TVoid TNameServerInfo::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // Stream start/end object markers as a safety measure
    strmToWriteTo   <<  tCIDLib::EStreamMarkers::StartObject
                    <<  CIDOrbUC_NSInfo::c2NSInfoFmtVersion
                    <<  m_ooidSrvObject
                    <<  m_strNodeName
                    <<  m_strDescription
                    <<  m_strExtra1
                    <<  m_strExtra2
                    <<  m_strExtra3
                    <<  m_strExtra4
                    <<  tCIDLib::EStreamMarkers::EndObject;
}





// ---------------------------------------------------------------------------
//   CLASS: TNSRebindInfo
//  PREFIX: nsrbi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TNSRebindInfo: Public, static methods
// ---------------------------------------------------------------------------

// A sort comparator for sorting by the next renewal time
tCIDLib::ESortComps
TNSRebindInfo::eComp(const  TNSRebindInfo&  nsrbi1
                    , const TNSRebindInfo&  nsrbi2)
{
    if (nsrbi1.m_enctNextRenewal > nsrbi2.m_enctNextRenewal)
        return tCIDLib::ESortComps::FirstGreater;
    else if (nsrbi1.m_enctNextRenewal < nsrbi2.m_enctNextRenewal)
        return tCIDLib::ESortComps::FirstLess;

    return tCIDLib::ESortComps::Equal;
}



// ---------------------------------------------------------------------------
//  TNSRebindInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TNSRebindInfo::TNSRebindInfo() :

    m_c4ErrCount(0)
    , m_enctNextRenewal(0)
{
}

TNSRebindInfo::TNSRebindInfo(const  TString&    strParPath
                            , const TOrbObjId&  ooidSrvObject
                            , const TString&    strNodeName
                            , const TString&    strDescription) :

    TNameServerInfo(ooidSrvObject, strNodeName, strDescription)
    , m_c4ErrCount(0)
    , m_enctNextRenewal(0)
    , m_strParPath(strParPath)
{
    // Build up the full path
    m_strFullPath = strParPath;
    m_strFullPath.Append(kCIDLib::chForwardSlash);
    m_strFullPath.Append(strNodeName);
}

TNSRebindInfo::TNSRebindInfo(const  TString&    strParPath
                            , const TOrbObjId&  ooidSrvObject
                            , const TString&    strNodeName
                            , const TString&    strDescription
                            , const TString&    strExtra1) :
    TNameServerInfo
    (
        ooidSrvObject
        , strNodeName
        , strDescription
        , strExtra1
    )
    , m_c4ErrCount(0)
    , m_enctNextRenewal(0)
    , m_strParPath(strParPath)
{
    // Build up the full path
    m_strFullPath = strParPath;
    m_strFullPath.Append(kCIDLib::chForwardSlash);
    m_strFullPath.Append(strNodeName);
}

TNSRebindInfo::TNSRebindInfo(const  TString&    strParPath
                            , const TOrbObjId&  ooidSrvObject
                            , const TString&    strNodeName
                            , const TString&    strDescription
                            , const TString&    strExtra1
                            , const TString&    strExtra2) :
    TNameServerInfo
    (
        ooidSrvObject
        , strNodeName
        , strDescription
        , strExtra1
        , strExtra2
    )
    , m_c4ErrCount(0)
    , m_enctNextRenewal(0)
    , m_strParPath(strParPath)
{
    // Build up the full path
    m_strFullPath = strParPath;
    m_strFullPath.Append(kCIDLib::chForwardSlash);
    m_strFullPath.Append(strNodeName);
}

TNSRebindInfo::TNSRebindInfo(const  TString&    strParPath
                            , const TOrbObjId&  ooidSrvObject
                            , const TString&    strNodeName
                            , const TString&    strDescription
                            , const TString&    strExtra1
                            , const TString&    strExtra2
                            , const TString&    strExtra3
                            , const TString&    strExtra4) :
    TNameServerInfo
    (
        ooidSrvObject
        , strNodeName
        , strDescription
        , strExtra1
        , strExtra2
        , strExtra3
        , strExtra4
    )
    , m_c4ErrCount(0)
    , m_enctNextRenewal(0)
    , m_strParPath(strParPath)
{
    // Build up the full path
    m_strFullPath = strParPath;
    m_strFullPath.Append(kCIDLib::chForwardSlash);
    m_strFullPath.Append(strNodeName);
}

TNSRebindInfo::TNSRebindInfo(const TNSRebindInfo& nsrbiSrc) :

    TNameServerInfo(nsrbiSrc)
    , m_c4ErrCount(nsrbiSrc.m_c4ErrCount)
    , m_enctNextRenewal(nsrbiSrc.m_enctNextRenewal)
    , m_strFullPath(nsrbiSrc.m_strFullPath)
    , m_strParPath(nsrbiSrc.m_strParPath)
{
}

TNSRebindInfo::~TNSRebindInfo()
{
}


// ---------------------------------------------------------------------------
//  TNSRebindInfo: Public operators
// ---------------------------------------------------------------------------
TNSRebindInfo&
TNSRebindInfo::operator=(const TNSRebindInfo& nsrbiSrc)
{
    if (&nsrbiSrc != this)
    {
        TNameServerInfo::operator=(nsrbiSrc);
        m_c4ErrCount      = nsrbiSrc.m_c4ErrCount;
        m_enctNextRenewal = nsrbiSrc.m_enctNextRenewal;
        m_strFullPath     = nsrbiSrc.m_strFullPath;
        m_strParPath      = nsrbiSrc.m_strParPath;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TNSRebindInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  If the new renewal time stamp is non-zero, then have been bound and are
//  eligible for renewals.
//
tCIDLib::TBoolean TNSRebindInfo::bIsBound() const
{
    return (m_enctNextRenewal != 0);
}


//
//  If our next renewal time is zero, then we haven't been bound, so no
//  refresh. Else, if it's less than now or the passed now time, return
//  true because it's time to refresh it.
//
tCIDLib::TBoolean TNSRebindInfo::bNeedsRefresh() const
{
    if (!m_enctNextRenewal)
        return kCIDLib::False;

    return (TTime::enctNow() > m_enctNextRenewal);
}

tCIDLib::TBoolean
TNSRebindInfo::bNeedsRefresh(const tCIDLib::TEncodedTime enctNow) const
{
    if (!m_enctNextRenewal)
        return kCIDLib::False;

    return (enctNow > m_enctNextRenewal);
}


//
//  This one sees if this guy either is not bound, or needs a renewal. This
//  is used by the rebinder generally. If the next renewal stamp is less than
//  now, it needs work. It'll be zero if not bound, so that gets caught in
//  the same check.
//
tCIDLib::TBoolean TNSRebindInfo::bNeedsWork() const
{
    return (TTime::enctNow() > m_enctNextRenewal);
}


//
//  Access or increment our binding error count. It is cleared automatically
//  when a next renewal is set, which means it's obviously been bound at
//  that point.
//
tCIDLib::TCard4 TNSRebindInfo::c4ErrCount() const
{
    return m_c4ErrCount;
}

tCIDLib::TCard4 TNSRebindInfo::c4IncErrCount()
{
    m_c4ErrCount++;
    return m_c4ErrCount;
}



// Access the full path we built from the path and binding name
const TString& TNSRebindInfo::strFullPath() const
{
    return m_strFullPath;
}


// Access the original parent path. It can't change after ctor
const TString& TNSRebindInfo::strParentPath() const
{
    return m_strParPath;
}


// Set our rnewal time to now plus the binding threshold
tCIDLib::TVoid TNSRebindInfo::SetNextRenewal()
{
    //
    //  Set our next renewal time. It's now plus the rebind time interval
    //  defined in CIDOrb.
    //
    m_enctNextRenewal = TTime::enctNow() + kCIDOrb::enctBindingThreshold;

    // Clear the error count since obviously it's been bound successfully
    m_c4ErrCount = 0;
}


//
//  Clear the renewal stamp, which means that we aren't bound and need to
//  rebound.
//
tCIDLib::TVoid TNSRebindInfo::SetNotBound()
{
    m_enctNextRenewal = 0;
}



// ---------------------------------------------------------------------------
//  TNSRebindInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TNSRebindInfo::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Make sure we get the start marker
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom  >> c2FmtVersion;
    if (!c2FmtVersion || (c2FmtVersion > CIDOrbUC_NSInfo::c2NSRBIFmtVersion))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_UnknownFmtVersion
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c2FmtVersion)
            , clsThis()
        );
    }

    // Do our parent now
    TParent::StreamFrom(strmToReadFrom);

    // At our level we only stream the parent path
    strmToReadFrom  >> m_strParPath;

    // Make sure we get the end marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);

    // Init the other stuff
    m_c4ErrCount = 0;
    m_enctNextRenewal = 0;

    m_strFullPath = m_strParPath;
    m_strFullPath.Append(kCIDLib::chForwardSlash);
    m_strFullPath.Append(strNodeName());
}


tCIDLib::TVoid TNSRebindInfo::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // Stream start/end object markers as a safety measure
    strmToWriteTo   <<  tCIDLib::EStreamMarkers::StartObject
                    <<  CIDOrbUC_NSInfo::c2NSRBIFmtVersion;

    // Do our parent class
    TParent::StreamTo(strmToWriteTo);

    // And our stuff and the end object marker
    strmToWriteTo   <<  m_strParPath
                    <<  tCIDLib::EStreamMarkers::EndObject;
}

