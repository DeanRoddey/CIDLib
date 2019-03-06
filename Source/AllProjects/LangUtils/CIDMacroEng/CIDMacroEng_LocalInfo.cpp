//
// FILE NAME: CIDMacroEng_LocalInfo.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/21/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements TMacroEngLocalInfo class. This class represents a
//  local variable or constant within a procedure.
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
RTTIDecls(TMEngLocalInfo,TMEngNamedItem)



// ---------------------------------------------------------------------------
//  CLASS: TMEngLocalInfo
// PREFIX: meli
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMacroEngVarInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngLocalInfo::strKey(const TMEngLocalInfo& meliSrc)
{
    // Our key is our name, which is only unique within the containing method
    return meliSrc.strName();
}


// ---------------------------------------------------------------------------
//  TMEngLocalInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngLocalInfo::TMEngLocalInfo() :

    TMEngNamedItem()
    , m_c2ClassId(0)
    , m_eConst(tCIDMacroEng::EConstTypes::NonConst)
{
}

TMEngLocalInfo::TMEngLocalInfo(const TMEngLocalInfo& meliToCopy) :

    TMEngNamedItem(meliToCopy)
    , m_c2ClassId(meliToCopy.m_c2ClassId)
    , m_eConst(meliToCopy.m_eConst)
{
}

TMEngLocalInfo::TMEngLocalInfo( const   TString&                strName
                                , const tCIDLib::TCard2         c2ClassId
                                , const tCIDMacroEng::EConstTypes  eConst) :
    TMEngNamedItem(strName)
    , m_c2ClassId(c2ClassId)
    , m_eConst(eConst)
{
}

TMEngLocalInfo::~TMEngLocalInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngLocalInfo: Public operators
// ---------------------------------------------------------------------------
TMEngLocalInfo& TMEngLocalInfo::operator=(const TMEngLocalInfo& meliToAssign)
{
    if (this == &meliToAssign)
    {
        TParent::operator=(meliToAssign);
        m_c2ClassId = meliToAssign.m_c2ClassId;
        m_eConst    = meliToAssign.m_eConst;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TMEngLocalInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngLocalInfo::bIsConst() const
{
    return (m_eConst == tCIDMacroEng::EConstTypes::Const);
}


tCIDLib::TCard2 TMEngLocalInfo::c2ClassId() const
{
    return m_c2ClassId;
}


tCIDMacroEng::EConstTypes TMEngLocalInfo::eConst() const
{
    return m_eConst;
}


