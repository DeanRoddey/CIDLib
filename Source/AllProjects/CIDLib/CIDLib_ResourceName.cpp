//
// FILE NAME: CIDLib_ResourceName.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/01/1997
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
    namespace
    {
        constexpr tCIDLib::TCard2   c2FmtVersion = 1;
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TResourceName
// PREFIX: rsn
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TResourceName: Public, static methods
// ---------------------------------------------------------------------------
TResourceName& TResourceName::Nul_TResourceName() noexcept
{
    static TResourceName rsnNull;
    return rsnNull;
}


// ---------------------------------------------------------------------------
//  TResourceName: Constructors and Destructor
// ---------------------------------------------------------------------------
TResourceName::TResourceName() noexcept :

    m_eTypeCache(tCIDLib::ENamedRscTypes::Count)
{
    strFullName(tCIDLib::ENamedRscTypes::Event);
}

TResourceName::TResourceName(const TKrnlRscName& krsnToUse) noexcept :

    m_eTypeCache(tCIDLib::ENamedRscTypes::Count)
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

    // We set Count above, so this will force an update with our actual type
    strFullName(tCIDLib::ENamedRscTypes::Event);
}

TResourceName::TResourceName(   const   TStringView&        strvCompany
                                , const TStringView&        strvSubsystem
                                , const TStringView&        strvResource
                                , const tCIDLib::TProcessId pidOfName) noexcept :

    m_eTypeCache(tCIDLib::ENamedRscTypes::Count)
    , m_krsnThis(strvCompany.pszBuffer(), strvSubsystem.pszBuffer(), strvResource.pszBuffer(), pidOfName)
{
    // We set count above, so this will force an udpate
    strFullName(tCIDLib::ENamedRscTypes::Event);
}



// ---------------------------------------------------------------------------
//  TResourceName: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TResourceName::operator!=(const TResourceName& rsnToCompare) const noexcept
{
    return !operator==(rsnToCompare);
}

tCIDLib::TBoolean TResourceName::operator==(const TResourceName& rsnToCompare) const noexcept
{
    if (this == &rsnToCompare)
        return kCIDLib::True;
    return (m_krsnThis == rsnToCompare.m_krsnThis);
}



// ---------------------------------------------------------------------------
//  TResourceName: Public, non-virtual methods
// ---------------------------------------------------------------------------
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
TResourceName::SetName( const   TStringView&        strvCompany
                        , const TStringView&        strvSubsystem
                        , const TStringView&        strvResource
                        , const tCIDLib::TProcessId pidOfName)
{
    m_krsnThis.SetName
    (
        strvCompany.pszBuffer()
        , strvSubsystem.pszBuffer()
        , strvResource.pszBuffer()
        , pidOfName
    );

    // And update our name cache
    strFullName(m_eTypeCache);
}


const TString& TResourceName::strFullName(const tCIDLib::ENamedRscTypes eType) const
{
    // If this type is the cached name, then just return what we've got
    if (eType == m_eTypeCache)
        return m_strFullName;

    // Its a different type, so store the cache type
    m_eTypeCache = eType;

    // Build the full name into a temp string that should be big enough
    tCIDLib::TCh szTmp[2048];
    if (!m_krsnThis.bBuildFullName(szTmp, tCIDLib::c4MaxBufChars(szTmp), eType))
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
    // Check for a start marker and valid format version
    const tCIDLib::TCard2 c2FmtVersion = TBinInStream::c2CheckFmtVersion
    (
        strmToReadFrom
        , tCIDLib::EStreamMarkers::StartObject
        , CIDLib_ResourceName::c2FmtVersion
        , clsThis()
        , CID_FILE
        , CID_LINE
    );

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

