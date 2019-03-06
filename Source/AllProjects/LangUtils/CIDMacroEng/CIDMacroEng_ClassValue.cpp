//
// FILE NAME: CIDMacroEng_ClassVal.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements TMEngClassVal class. This class is the base class
//  of the set of classes that represent the instance data of classes. Each
//  TMEngClassInfo class must have an equiv TMEngClassVal class to provide
//  it's instance data representation.
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
RTTIDecls(TMEngClassVal,TMacroEngNamedItem)


// ---------------------------------------------------------------------------
//  CLASS: TMEngClassVal
// PREFIX: eetv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngClassVal: Destructor
// ---------------------------------------------------------------------------
TMEngClassVal::~TMEngClassVal()
{
}


// ---------------------------------------------------------------------------
//  TMEngClassVal: Public, virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngClassVal::bDbgFormat(          TTextOutStream&
                            , const TMEngClassInfo&
                            , const tCIDMacroEng::EDbgFmts
                            , const tCIDLib::ERadices
                            , const TCIDMacroEngine&) const
{
    // If not overridden, just say we cannot provide that format
    return kCIDLib::False;
}


tCIDLib::TBoolean
TMEngClassVal::bParseFromText(const TString&, const TMEngClassInfo&)
{
    // If not overridden, say we can't do it
    return kCIDLib::False;
}


//
//  If a class isn't copyable, it can just not override these methods and we'll
//  throw an exception if it manages to get called somehow.
//
tCIDLib::TVoid
TMEngClassVal::CopyFrom(const TMEngClassVal&, TCIDMacroEngine& meOwner)
{
    facCIDMacroEng().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kMEngErrs::errcPrs_NotCopyable
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Internal
        , meOwner.strClassPathForId(c2ClassId())
    );
}


// ---------------------------------------------------------------------------
//  TMEngClassVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngClassVal::bIsConst() const
{
    return (m_eConst == tCIDMacroEng::EConstTypes::Const);
}


tCIDLib::TBoolean TMEngClassVal::bIsUsed() const
{
    return m_bUsed;
}

tCIDLib::TBoolean TMEngClassVal::bIsUsed(const tCIDLib::TBoolean bToSet)
{
    m_bUsed = bToSet;
    return m_bUsed;
}


tCIDLib::TCard2 TMEngClassVal::c2ClassId() const
{
    return m_c2ClassId;
}

tCIDLib::TCard2 TMEngClassVal::c2ClassId(const tCIDLib::TCard2 c2ToSet)
{
    m_c2ClassId = c2ToSet;
    return m_c2ClassId;
}


tCIDLib::TCard4 TMEngClassVal::c4MemberCount() const
{
    return m_colMembers.c4ElemCount();
}


tCIDMacroEng::EConstTypes TMEngClassVal::eConst() const
{
    return m_eConst;
}


const TMEngClassVal&
TMEngClassVal::mecvFind(const   tCIDLib::TCard2     c2Id
                        , const TCIDMacroEngine&    meOwner) const
{
    if (c2Id >= m_colMembers.c4ElemCount())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcClass_BadMemberId
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , TCardinal(c2Id)
            , meOwner.strClassPathForId(m_c2ClassId)
            , TCardinal(m_colMembers.c4ElemCount())
        );
    }
    return *m_colMembers[c2Id];
}

TMEngClassVal&
TMEngClassVal::mecvFind(const   tCIDLib::TCard2     c2Id
                        , const TCIDMacroEngine&    meOwner)
{
    if (c2Id >= m_colMembers.c4ElemCount())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcClass_BadMemberId
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , TCardinal(c2Id)
            , meOwner.strClassPathForId(m_c2ClassId)
            , TCardinal(m_colMembers.c4ElemCount())
        );
    }
    return *m_colMembers[c2Id];
}


// Get the extra pointer value
tCIDLib::TVoid* TMEngClassVal::pExtra() const
{
    return m_pExtra;
}


TMEngClassVal* TMEngClassVal::pmecvFind(const TString& strName)
{
    // Just loop through, looking for a match
    const tCIDLib::TCard4 c4Count = m_colMembers.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colMembers[c4Index]->strName() == strName)
            return m_colMembers[c4Index];
    }
    return nullptr;
}


// Set the extra pointer value
tCIDLib::TVoid TMEngClassVal::SetExtra(tCIDLib::TVoid* const pExtra)
{
    m_pExtra = pExtra;
}


// ---------------------------------------------------------------------------
//  TMEngClassVal: Hidden constructors
// ---------------------------------------------------------------------------
TMEngClassVal::TMEngClassVal(const  TString&                strName
                            , const tCIDLib::TCard2         c2ClassId
                            , const tCIDMacroEng::EConstTypes  eConst) :
    TMEngNamedItem(strName)
    , m_bUsed(kCIDLib::False)
    , m_c2ClassId(c2ClassId)
    , m_colMembers(tCIDLib::EAdoptOpts::Adopt, 16)
    , m_eConst(eConst)
    , m_pExtra(nullptr)
{
}


// ---------------------------------------------------------------------------
//  TMEngClassVal: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngClassVal::AddMember(TMEngClassVal* const pmecvToAdopt)
{
    m_colMembers.Add(pmecvToAdopt);
}


tCIDLib::TVoid
TMEngClassVal::ForceConstType(const tCIDMacroEng::EConstTypes eToSet)
{
    m_eConst = eToSet;
}


