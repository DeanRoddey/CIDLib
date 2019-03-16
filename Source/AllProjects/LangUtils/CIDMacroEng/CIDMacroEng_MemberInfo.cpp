//
// FILE NAME: CIDMacroEng_MemberInfo.cpp
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
//  This file implements TMEngMemberInfo class. This class represents a
//  member declared in a class.
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
RTTIDecls(TMEngMemberInfo,TMacroEngNamedItem)


// ---------------------------------------------------------------------------
//  CLASS: TMEngMemberInfo
// PREFIX: memi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngMemberInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngMemberInfo::strKey(const TMEngMemberInfo& meviSrc)
{
    // Our key is our name, which is only unique within the containing method
    return meviSrc.strName();
}


// ---------------------------------------------------------------------------
//  TMEngMemberInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngMemberInfo::TMEngMemberInfo() :

    TMEngNamedItem()
    , m_c2ClassId(0)
    , m_eConst(tCIDMacroEng::EConstTypes::NonConst)
{
}

TMEngMemberInfo::TMEngMemberInfo(const TMEngMemberInfo& memiToCopy) :

    TMEngNamedItem(memiToCopy)
    , m_c2ClassId(memiToCopy.m_c2ClassId)
    , m_eConst(memiToCopy.m_eConst)
{
}

TMEngMemberInfo::TMEngMemberInfo(const  TString&                strName
                                , const tCIDLib::TCard2         c2ClassId
                                , const tCIDMacroEng::EConstTypes  eConst) :
    TMEngNamedItem(strName)
    , m_c2ClassId(c2ClassId)
    , m_eConst(eConst)
{
}

TMEngMemberInfo::~TMEngMemberInfo()
{
}



// ---------------------------------------------------------------------------
//  TMEngMemberInfo: Public operators
// ---------------------------------------------------------------------------
TMEngMemberInfo&
TMEngMemberInfo::operator=(const TMEngMemberInfo& memiToAssign)
{
    if (this != &memiToAssign)
    {
        TParent::operator=(memiToAssign);
        m_c2ClassId = memiToAssign.m_c2ClassId;
        m_eConst    = memiToAssign.m_eConst;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TMEngMemberInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngMemberInfo::bIsConst() const
{
    return (m_eConst == tCIDMacroEng::EConstTypes::Const);
}


tCIDLib::TCard2 TMEngMemberInfo::c2ClassId() const
{
    return m_c2ClassId;
}


tCIDMacroEng::EConstTypes TMEngMemberInfo::eConst() const
{
    return m_eConst;
}


