//
// FILE NAME: CIDMacroEng_ParmInfo.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/14/2003
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
//  This file implements TMEngParmInfo class. This class represents a
//  method parameter.
//
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
#include    "CIDMacroEng_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TMEngParmInfo,TMEngNamedItem)



// ---------------------------------------------------------------------------
//  CLASS: TMEngParmInfo
// PREFIX: mepi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngParmInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngParmInfo::TMEngParmInfo() :

    TMEngNamedItem()
    , m_c2ClassId(0)
    , m_eDir(tCIDMacroEng::EParmDirs::In)
{
}

TMEngParmInfo::TMEngParmInfo(const  TString&                strName
                            , const tCIDLib::TCard2         c2ClassId
                            , const tCIDMacroEng::EParmDirs    eDir) :
    TMEngNamedItem(strName)
    , m_c2ClassId(c2ClassId)
    , m_eDir(eDir)
{
}

TMEngParmInfo::TMEngParmInfo(const TMEngParmInfo& mepiToCopy) :

    TMEngNamedItem(mepiToCopy)
    , m_c2ClassId(mepiToCopy.m_c2ClassId)
    , m_eDir(mepiToCopy.m_eDir)
{
}

TMEngParmInfo::~TMEngParmInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngParmInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngParmInfo::operator==(const TMEngParmInfo& mepiToCompare) const
{
    if (this == &mepiToCompare)
        return kCIDLib::True;

    if (!TParent::operator==(mepiToCompare))
        return kCIDLib::False;

    return (m_c2ClassId == mepiToCompare.m_c2ClassId)
           && (m_eDir == mepiToCompare.m_eDir);
}

tCIDLib::TBoolean
TMEngParmInfo::operator!=(const TMEngParmInfo& mepiToCompare) const
{
    return !operator==(mepiToCompare);
}


TMEngParmInfo& TMEngParmInfo::operator=(const TMEngParmInfo& mepiToAssign)
{
    if (this != &mepiToAssign)
    {
        TParent::operator=(mepiToAssign);
        m_c2ClassId = mepiToAssign.m_c2ClassId;
        m_eDir      = mepiToAssign.m_eDir;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TMEngParmInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngParmInfo::bIsLegalOverride(const TMEngParmInfo& mepiToCheck)
{
    if ((m_eDir == mepiToCheck.m_eDir)
    &&  (m_c2ClassId == mepiToCheck.m_c2ClassId))
    {
        // Take his name
        ChangeName(mepiToCheck.strName());
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TCard2 TMEngParmInfo::c2ClassId() const
{
    return m_c2ClassId;
}


tCIDMacroEng::EParmDirs TMEngParmInfo::eDir() const
{
    return m_eDir;
}


