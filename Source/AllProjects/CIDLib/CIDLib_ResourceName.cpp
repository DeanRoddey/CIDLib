//
// FILE NAME: CIDLib_ResourceName.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/01/1997
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
//  This file implements the TResourceName class, an abstraction for the
//  names of shareable resources.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TResourceName,TObject)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDLib_ResourceName
{
    const tCIDLib::TCard2   c2FmtVersion = 1;
}



// ---------------------------------------------------------------------------
//  CLASS: TResourceName
// PREFIX: rsn
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TResourceName: Public, static methods
// ---------------------------------------------------------------------------
TResourceName& TResourceName::Nul_TResourceName()
{
    static TResourceName* prsnNull = 0;
    if (!prsnNull)
    {
        TBaseLock lockInit;
        if (!prsnNull)
            TRawMem::pExchangePtr(&prsnNull, new TResourceName);
    }
    return *prsnNull;
}


// ---------------------------------------------------------------------------
//  TResourceName: Constructors and Destructor
// ---------------------------------------------------------------------------
TResourceName::TResourceName() :

    m_eTypeCache(tCIDLib::ENamedRscTypes::Event)
{
    strFullName(m_eTypeCache);
}

TResourceName::TResourceName(const TKrnlRscName& krsnToUse) :

    m_eTypeCache(tCIDLib::ENamedRscTypes::Event)
{
    try
    {
        m_krsnThis = krsnToUse;
    }

    catch(const TKrnlError& kerrToCatch)
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcRscN_Initialize
            , kerrToCatch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    strFullName(m_eTypeCache);
}

TResourceName::TResourceName(   const   TString&            strCompany
                                , const TString&            strSubsystem
                                , const TString&            strResource
                                , const tCIDLib::TProcessId pidOfName) :

    m_eTypeCache(tCIDLib::ENamedRscTypes::Event)
    , m_krsnThis
      (
        strCompany.pszBuffer()
        , strSubsystem.pszBuffer()
        , strResource.pszBuffer()
        , pidOfName
      )
{
    // Use something different from the one set above, to force an update
    strFullName(tCIDLib::ENamedRscTypes::Mutex);
}

TResourceName::TResourceName(const TResourceName& rsnToCopy) :

    m_eTypeCache(rsnToCopy.m_eTypeCache)
    , m_krsnThis(rsnToCopy.m_krsnThis)
    , m_strFullName(rsnToCopy.m_strFullName)
{
}

TResourceName::~TResourceName()
{
}


// ---------------------------------------------------------------------------
//  TResourceName: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TResourceName::operator!=(const TResourceName& rsnToCompare) const
{
    return !operator==(rsnToCompare);
}

tCIDLib::TBoolean
TResourceName::operator==(const TResourceName& rsnToCompare) const
{
    if (this == &rsnToCompare)
        return kCIDLib::True;
    return (m_krsnThis == rsnToCompare.m_krsnThis);
}

TResourceName& TResourceName::operator=(const TResourceName& rsnToAssign)
{
    if (this == &rsnToAssign)
        return *this;

    m_krsnThis == rsnToAssign.m_krsnThis;
    return *this;
}


// ---------------------------------------------------------------------------
//  TResourceName: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TResourceName::bIsValid() const
{
    return m_krsnThis.bIsValid();
}


tCIDLib::TProcessId TResourceName::pidOfName() const
{
    return m_krsnThis.pidOfName();
}


tCIDLib::TVoid
TResourceName::QueryNameParts(  TString&    strCompany
                                , TString&  strSubsystem
                                , TString&  strResource) const
{
    strCompany = m_krsnThis.pszCompanyName();
    strSubsystem = m_krsnThis.pszSubsystemName();
    strResource = m_krsnThis.pszResourceName();
}


tCIDLib::TVoid
TResourceName::SetName( const   TString&            strCompany
                        , const TString&            strSubsystem
                        , const TString&            strResource
                        , const tCIDLib::TProcessId pidOfName)
{
    m_krsnThis.SetName
    (
        strCompany.pszBuffer()
        , strSubsystem.pszBuffer()
        , strResource.pszBuffer()
        , pidOfName
    );

    // And update our name cache
    strFullName(m_eTypeCache);
}


const TString&
TResourceName::strFullName(const tCIDLib::ENamedRscTypes eType) const
{
    // If this type is the cached name, then just return what we've got
    if (eType == m_eTypeCache)
        return m_strFullName;

    // Its a different type, so store the cache type
    m_eTypeCache = eType;

    // Build the full name into a temp string that should be big enough
    tCIDLib::TCh szTmp[2048];
    if (!m_krsnThis.bBuildFullName(szTmp, c4MaxBufChars(szTmp), eType))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcRscN_GenRealName
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Now update the string memory and return it
    m_strFullName = szTmp;
    return m_strFullName;
}


// ---------------------------------------------------------------------------
//  TResource: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TResourceName::FormatTo(TTextOutStream& strmDest) const
{
    //  Format the separate name parts into the text stream.
    strmDest << m_krsnThis.pszCompanyName() << L","
             << m_krsnThis.pszSubsystemName() << L","
             << m_krsnThis.pszResourceName();
}


tCIDLib::TVoid TResourceName::StreamFrom(TBinInStream& strmToReadFrom)
{
    // We should get a start object marker
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom  >> c2FmtVersion;
    if (c2FmtVersion != CIDLib_ResourceName::c2FmtVersion)
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

    // Stream in the three name parts
    TString strCompany;
    TString strSubsystem;
    TString strResource;

    strmToReadFrom  >> strCompany
                    >> strSubsystem
                    >> strResource;

    // And stream in the process id
    tCIDLib::TProcessId pidTmp;
    strmToReadFrom >> pidTmp;

    // We should get an end object marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);

    // And set this new info on the resource name object
    m_krsnThis.SetName
    (
        strCompany.pszBuffer()
        , strSubsystem.pszBuffer()
        , strResource.pszBuffer()
        , pidTmp
    );
}

tCIDLib::TVoid TResourceName::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // Get the name parts into string objects
    TString strCompany;
    TString strSubsystem;
    TString strResource;
    QueryNameParts(strCompany, strSubsystem, strResource);

    //
    //  Now stream out the separate string objects that we just got
    //  filled in.
    //
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << CIDLib_ResourceName::c2FmtVersion
                    << strCompany
                    << strSubsystem
                    << strResource
                    << m_krsnThis.pidOfName()
                    << tCIDLib::EStreamMarkers::EndObject;
}

