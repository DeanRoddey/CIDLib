//
// FILE NAME: CIDResEd_ImgData.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/26/2010
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
//  This file implements the classes related to the various types of image
//  resources.
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
#include "CIDResEd.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TResEdImgItem,TObject)




// ---------------------------------------------------------------------------
//   CLASS: TResEdImgItem
//  PREFIX: reimgi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TResEdImgItem: Public, static methods
// ---------------------------------------------------------------------------

// Return a sort comparison for two image items via their name fields
tCIDLib::ESortComps
TResEdImgItem::eComp(const  TResEdImgItem&  reimgi1
                    , const TResEdImgItem&  reimgi2)
{
    return reimgi1.m_strName.eCompare(reimgi2.m_strName);
}


// REturn a sort comparision of a key value and the img item's name field
tCIDLib::ESortComps
TResEdImgItem::eKeyComp(const   TString&        strKey
                        , const TResEdImgItem&  reimgiComp)
{
    return strKey.eCompareI(reimgiComp.m_strName);
}


// ---------------------------------------------------------------------------
//  TResEdImgItem: Constructors and Destructor
// ---------------------------------------------------------------------------
TResEdImgItem::TResEdImgItem() :

    m_c4Id(0)
{
}

TResEdImgItem::TResEdImgItem(const  tCIDLib::TCard4 c4Id
                            , const TString&        strName) :

    m_c4Id(c4Id)
    , m_strName(strName)
{
}

TResEdImgItem::TResEdImgItem(const  tCIDLib::TCard4 c4Id
                            , const TString&        strName
                            , const TString&        strPath) :

    m_c4Id(c4Id)
    , m_strPath(strPath)
    , m_strName(strName)
{
}

TResEdImgItem::TResEdImgItem(const TResEdImgItem& reimgiToCopy) :

    m_c4Id(reimgiToCopy.m_c4Id)
    , m_strPath(reimgiToCopy.m_strPath)
    , m_strName(reimgiToCopy.m_strName)
{
}

TResEdImgItem::~TResEdImgItem()
{
}


// ---------------------------------------------------------------------------
//  TResEdImgItem: Public operators
// ---------------------------------------------------------------------------
TResEdImgItem& TResEdImgItem::operator=(const TResEdImgItem& reimgiToAssign)
{
    if (this != &reimgiToAssign)
    {
        m_c4Id      = reimgiToAssign.m_c4Id;
        m_strPath   = reimgiToAssign.m_strPath;
        m_strName   = reimgiToAssign.m_strName;
    }
    return *this;
}


tCIDLib::TBoolean
TResEdImgItem::operator==(const TResEdImgItem& reimgiToComp) const
{
    return ((m_c4Id == reimgiToComp.m_c4Id)
           && (m_strPath == reimgiToComp.m_strPath)
           && (m_strName == reimgiToComp.m_strName));
}

tCIDLib::TBoolean
TResEdImgItem::operator!=(const TResEdImgItem& reimgiToComp) const
{
    return !operator==(reimgiToComp);
}


// ---------------------------------------------------------------------------
//  TResEdImgItem: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Get or set the image item id
tCIDLib::TCard4 TResEdImgItem::c4Id() const
{
    return m_c4Id;
}

tCIDLib::TCard4 TResEdImgItem::c4Id(const tCIDLib::TCard4 c4ToSet)
{
    m_c4Id = c4ToSet;
    return m_c4Id;
}

// Get/set the image item name
const TString& TResEdImgItem::strName() const
{
    return m_strName;
}

const TString& TResEdImgItem::strName(const TString& strToSet)
{
    m_strName = strToSet;
    return m_strName;
}


// Get/set the image item text
const TString& TResEdImgItem::strPath() const
{
    return m_strPath;
}

const TString& TResEdImgItem::strPath(const TString& strToSet)
{
    m_strPath = strToSet;
    return m_strPath;
}


// Set up all the values of this image item at once
tCIDLib::TVoid
TResEdImgItem::Set( const   tCIDLib::TCard4 c4Id
                    , const TString&        strName
                    , const TString&        strPath)
{
    m_c4Id = c4Id;
    m_strPath = strPath;
    m_strName = strName;
}


