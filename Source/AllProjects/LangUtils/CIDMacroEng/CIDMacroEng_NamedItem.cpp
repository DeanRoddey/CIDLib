//
// FILE NAME: CIDMacroEng_NamedItem.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/14/2003
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
//  This file implements TMEngNamedItem class. This class provides a common
//  base for a number of other classes used by the engine. This is not so that
//  they can be manipulated polymorphically, but just to avoid redundant code.
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
RTTIDecls(TMEngNamedItem,TObject)


// ---------------------------------------------------------------------------
//  CLASS: TMEngNamedItem
// PREFIX: eeproc
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngNamedItem: Destructor
// ---------------------------------------------------------------------------
TMEngNamedItem::~TMEngNamedItem()
{
    // Delete any name we allocated
    delete m_pstrName;
}


// ---------------------------------------------------------------------------
//  TMEngNamedItem: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngNamedItem::operator==(const TMEngNamedItem& meniToComp) const
{
    if (m_c2Id != meniToComp.m_c2Id)
        return kCIDLib::False;

    //
    //  We have to deal with the fact that we fault in the name member.
    //
    //  If both are null, then they are equal
    //  If not, but one is null, then obviously not equal
    //  Else, compare actual strings
    //
    if (!m_pstrName && !meniToComp.m_pstrName)
        return kCIDLib::True;

    if (!m_pstrName || !meniToComp.m_pstrName)
        return  kCIDLib::False;

    return (*m_pstrName == *meniToComp.m_pstrName);
}



// ---------------------------------------------------------------------------
//  TMEngNamedItem: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard2 TMEngNamedItem::c2Id(const tCIDLib::TCard2 c2ToSet)
{
    m_c2Id = c2ToSet;
    return m_c2Id;
}


const TString& TMEngNamedItem::strName() const
{
    // If it's never been allocated, then return an empty string
    if (!m_pstrName)
        return TString::strEmpty();
    return *m_pstrName;
}


// ---------------------------------------------------------------------------
//  TMEngNamedItem: Hidden constructors and operators
// ---------------------------------------------------------------------------
TMEngNamedItem::TMEngNamedItem() :

    m_c2Id(0)
    , m_pstrName(0)
{
}

TMEngNamedItem::TMEngNamedItem(const TString& strName) :

    m_c2Id(0)
    , m_pstrName(0)
{
    // If the passed string is empty, don't allocate
    if (!strName.bIsEmpty())
        m_pstrName = new TString(strName);
}

TMEngNamedItem::TMEngNamedItem( const   TString&        strName
                                , const tCIDLib::TCard2 c2Id) :
    m_c2Id(c2Id)
    , m_pstrName(0)
{
    // If the passed string is empty, don't allocate
    if (!strName.bIsEmpty())
        m_pstrName = new TString(strName);
}

TMEngNamedItem::TMEngNamedItem(const tCIDLib::TCard2 c2Id) :

    m_c2Id(c2Id)
    , m_pstrName(0)
{
}

TMEngNamedItem::TMEngNamedItem(const TMEngNamedItem& meniToCopy) :

    m_c2Id(meniToCopy.m_c2Id)
    , m_pstrName(0)
{
    // We the source has a string allocated, then we have to copy it
    if (meniToCopy.m_pstrName)
        m_pstrName = new TString(*meniToCopy.m_pstrName);
}

tCIDLib::TVoid TMEngNamedItem::operator=(const TMEngNamedItem& meniToAssign)
{
    if (this != &meniToAssign)
    {
        m_c2Id = meniToAssign.m_c2Id;

        // If the source has a string, either assign or allocate as required
        if (meniToAssign.m_pstrName)
        {
            if (m_pstrName)
                *m_pstrName = *meniToAssign.m_pstrName;
            else
                m_pstrName = new TString(*meniToAssign.m_pstrName);
        }
    }
}


// ---------------------------------------------------------------------------
//  TMEngNamedItem: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngNamedItem::ChangeName(const TString& strToSet)
{
    if (m_pstrName)
    {
        *m_pstrName = strToSet;
    }
     else
    {
        // Only allocate if the new name isn't empty
        if (!strToSet.bIsEmpty())
            m_pstrName = new TString(strToSet);
    }
}


