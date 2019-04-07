//
// FILE NAME: CIDOrb_ObjId.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/12/2000
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
//  This is implementation file for the orb object id class.
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
#include    "CIDOrb_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TOrbObjId,TObject)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDOrb_ObjId
{
    // -----------------------------------------------------------------------
    //  Our persistent format version.
    //
    //
    //  Note that, so far, these are never persisted, they are just streamed
    //  around live and stored in the name server and such. So there's been
    //  no need to bump this, though the format has changed.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard2   c2FmtVersion = 1;
}

// ---------------------------------------------------------------------------
//   CLASS: TOrbObjId
//  PREFIX: ooid
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TOrbObjId: Constructors and Destructor
// ---------------------------------------------------------------------------
TOrbObjId::TOrbObjId() :

    m_bHasCachedAddr(kCIDLib::False)
    , m_enctCache(0)
    , m_ippnHost(0)
{
}

TOrbObjId::TOrbObjId(const  TString&            strInterfaceId
                    , const TString&            strInstanceId
                    , const TString&            strHostName
                    , const tCIDLib::TIPPortNum ippnHost
                    , const TString&            strClientProxyClass) :

    m_bHasCachedAddr(kCIDLib::False)
    , m_enctCache(0)
    , m_ippnHost(ippnHost)
    , m_oidKey(strInterfaceId, strInstanceId)
    , m_strClientProxyClass(strClientProxyClass)
    , m_strHostName(strHostName)
{
}

TOrbObjId::TOrbObjId(const  TString&            strInterfaceId
                    , const TMD5Hash&           mhashInstanceId
                    , const TString&            strHostName
                    , const tCIDLib::TIPPortNum ippnHost
                    , const TString&            strClientProxyClass) :

    m_bHasCachedAddr(kCIDLib::False)
    , m_enctCache(0)
    , m_ippnHost(ippnHost)
    , m_oidKey(strInterfaceId, mhashInstanceId)
    , m_strClientProxyClass(strClientProxyClass)
    , m_strHostName(strHostName)
{
}

TOrbObjId::TOrbObjId(const TOrbObjId& ooidSrc) :

    m_bHasCachedAddr(ooidSrc.m_bHasCachedAddr)
    , m_enctCache(ooidSrc.m_enctCache)
    , m_ipaSrvCache(ooidSrc.m_ipaSrvCache)
    , m_ippnHost(ooidSrc.m_ippnHost)
    , m_oidKey(ooidSrc.m_oidKey)
    , m_strClientProxyClass(ooidSrc.m_strClientProxyClass)
    , m_strHostName(ooidSrc.m_strHostName)
{
}

TOrbObjId::~TOrbObjId()
{
}


// ---------------------------------------------------------------------------
//  TOrbObjId: Public operators
// ---------------------------------------------------------------------------
TOrbObjId& TOrbObjId::operator=(const TOrbObjId& ooidSrc)
{
    if (this != &ooidSrc)
    {
        m_bHasCachedAddr        = ooidSrc.m_bHasCachedAddr;
        m_enctCache             = ooidSrc.m_enctCache;
        m_ipaSrvCache           = ooidSrc.m_ipaSrvCache;
        m_ippnHost              = ooidSrc.m_ippnHost;
        m_oidKey                = ooidSrc.m_oidKey;
        m_strClientProxyClass   = ooidSrc.m_strClientProxyClass;
        m_strHostName           = ooidSrc.m_strHostName;
    }
    return *this;
}


// None of the caching stuff is involved here
tCIDLib::TBoolean TOrbObjId::operator==(const TOrbObjId& ooidToCompare) const
{
    // Compare the keys
    const tCIDLib::TBoolean bRet = (m_oidKey == ooidToCompare.m_oidKey);

    //
    //  If in debug mode, and the keys are equal, then sanity check that they
    //  have the same client proxy class and ip end point.
    //
    #if CID_DEBUG_ON
    if (bRet)
    {
        if ((m_strClientProxyClass != ooidToCompare.m_strClientProxyClass)
        ||  (m_strHostName != ooidToCompare.m_strHostName)
        ||  (m_ippnHost != ooidToCompare.m_ippnHost))
        {
            facCIDOrb().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbErrs::errcOOId_Inconsistent
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
            );
        }
    }
    #endif
    return bRet;
}


// ---------------------------------------------------------------------------
//  TOrbObjId: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Return whether we have a cached address or not
tCIDLib::TBoolean TOrbObjId::bHasCachedAddr() const
{
    return m_bHasCachedAddr;
}


tCIDLib::TVoid TOrbObjId::ClearCachedAddr()
{
    m_bHasCachedAddr = kCIDLib::False;
}


// Get or set the cache time stamp
tCIDLib::TEncodedTime TOrbObjId::enctCache() const
{
    return m_enctCache;
}

tCIDLib::TEncodedTime
TOrbObjId::enctCache(const tCIDLib::TEncodedTime enctToSet)
{
    m_enctCache = enctToSet;
    return m_enctCache;
}


//
//  Used by the object caching in the facility class. This has no meaning
//  if m_bHasCachedAddr isn't set.
//
const TIPAddress& TOrbObjId::ipaCached() const
{
    return m_ipaSrvCache;
}


// Get the pre-done has of our object id key
tCIDLib::THashVal TOrbObjId::hshKey() const
{
    return m_oidKey.hshThis();
}


// Get the port number
tCIDLib::TIPPortNum TOrbObjId::ippnHost() const
{
    return m_ippnHost;
}


// Get the object id
const TOrbId& TOrbObjId::oidKey() const
{
    return m_oidKey;
}


//Get the client proxy class name
const TString& TOrbObjId::strClientProxyClass() const
{
    return m_strClientProxyClass;
}


const TString& TOrbObjId::strAddrText() const
{
    return m_strHostName;
}


// Used by the object id caching support
tCIDLib::TVoid TOrbObjId::SetCachedAddr(const TIPAddress& ipaToSet)
{
    m_ipaSrvCache = ipaToSet;
    m_bHasCachedAddr = kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TOrbObjId: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TOrbObjId::FormatTo(TTextOutStream& strmDest) const
{
    strmDest    << L'{' << m_oidKey
                << L',' << m_strClientProxyClass
                << L',' << m_strHostName
                << L'.' << m_ippnHost << L'}';

    // If we have a resolved cached address, format that out
    if (m_bHasCachedAddr)
        strmDest << L" [" << m_ipaSrvCache << L']';
}


tCIDLib::TVoid TOrbObjId::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Make sure we get the start marker
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom  >> c2FmtVersion;
    if (c2FmtVersion != CIDOrb_ObjId::c2FmtVersion)
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
    strmToReadFrom  >> m_strClientProxyClass
                    >> m_oidKey
                    >> m_ippnHost
                    >> m_strHostName;

    // And check the end marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);

    // Reset any purely runtime stuff
    m_bHasCachedAddr = kCIDLib::False;
    m_enctCache = 0;
}


tCIDLib::TVoid TOrbObjId::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // Stream a start object marker as a safety measure
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << CIDOrb_ObjId::c2FmtVersion
                    << m_strClientProxyClass
                    << m_oidKey
                    << m_ippnHost
                    << m_strHostName
                    << tCIDLib::EStreamMarkers::EndObject;
}


