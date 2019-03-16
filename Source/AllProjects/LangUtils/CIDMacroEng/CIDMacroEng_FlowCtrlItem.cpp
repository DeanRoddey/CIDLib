//
// FILE NAME: CIDMacroEng_FlowCtrlItem.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/08/2003
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
//  This file implements a class that represents an item pushed onto the
//  parser's flow control stack.
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
RTTIDecls(TMEngFlowCtrlItem,TObject)


// ---------------------------------------------------------------------------
//  CLASS: TMEngFlowCtrlItem
// PREFIX: mecsi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngFlowCtrlItem: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngFlowCtrlItem::TMEngFlowCtrlItem() :

    m_c2Id(0)
    , m_c4LineNum(0)
    , m_c4Offset1(0)
    , m_c4Offset2(0)
    , m_eType(tCIDMacroEng::EFlowTypes::None)
{
}

TMEngFlowCtrlItem::
TMEngFlowCtrlItem(  const   tCIDMacroEng::EFlowTypes   eType
                    , const tCIDLib::TCard4         c4Offset
                    , const tCIDLib::TCard4         c4LineNum) :
    m_c2Id(0)
    , m_c4LineNum(c4LineNum)
    , m_c4Offset1(c4Offset)
    , m_c4Offset2(0)
    , m_eType(eType)
{
}

TMEngFlowCtrlItem::
TMEngFlowCtrlItem(  const   tCIDMacroEng::EFlowTypes   eType
                    , const tCIDLib::TCard4         c4Offset
                    , const tCIDLib::TCard2         c2Id
                    , const tCIDLib::TCard4         c4LineNum) :
    m_c2Id(c2Id)
    , m_c4LineNum(c4LineNum)
    , m_c4Offset1(c4Offset)
    , m_c4Offset2(0)
    , m_eType(eType)
{
}

TMEngFlowCtrlItem::
TMEngFlowCtrlItem(  const   tCIDMacroEng::EFlowTypes   eType
                    , const tCIDLib::TCard4         c4Offset1
                    , const tCIDLib::TCard4         c4Offset2
                    , const tCIDLib::TCard4         c4LineNum) :
    m_c2Id(0)
    , m_c4LineNum(c4LineNum)
    , m_c4Offset1(c4Offset1)
    , m_c4Offset2(c4Offset2)
    , m_eType(eType)
{
}

TMEngFlowCtrlItem::TMEngFlowCtrlItem(const TMEngFlowCtrlItem& mefciToCopy) :

    m_c2Id(mefciToCopy.m_c2Id)
    , m_c4LineNum(mefciToCopy.m_c4LineNum)
    , m_c4Offset1(mefciToCopy.m_c4Offset1)
    , m_c4Offset2(mefciToCopy.m_c4Offset2)
    , m_eType(mefciToCopy.m_eType)
    , m_fcolBreaks(mefciToCopy.m_fcolBreaks)
{
}

TMEngFlowCtrlItem::~TMEngFlowCtrlItem()
{
}


// ---------------------------------------------------------------------------
//  TMEngFlowCtrlItem: Public operators
// ---------------------------------------------------------------------------
TMEngFlowCtrlItem&
TMEngFlowCtrlItem::operator=(const TMEngFlowCtrlItem& mefciToAssign)
{
    if (this != &mefciToAssign)
    {
        m_c2Id          = mefciToAssign.m_c2Id;
        m_c4LineNum     = mefciToAssign.m_c4LineNum;
        m_c4Offset1     = mefciToAssign.m_c4Offset1;
        m_c4Offset2     = mefciToAssign.m_c4Offset2;
        m_eType         = mefciToAssign.m_eType;
        m_fcolBreaks    = mefciToAssign.m_fcolBreaks;
    }
    return *this;
}



// ---------------------------------------------------------------------------
//  TMEngFlowCtrlItem: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngFlowCtrlItem::AddBreakOffset(const tCIDLib::TCard4 c4Offset)
{
    m_fcolBreaks.c4AddElement(c4Offset);
}


tCIDLib::TBoolean TMEngFlowCtrlItem::bIsLoopedType() const
{
    return (m_eType == tCIDMacroEng::EFlowTypes::DoLoop)
           || (m_eType == tCIDMacroEng::EFlowTypes::ForEach)
           || (m_eType == tCIDMacroEng::EFlowTypes::While);
}


tCIDLib::TCard2 TMEngFlowCtrlItem::c2Id() const
{
    return m_c2Id;
}


tCIDLib::TCard4 TMEngFlowCtrlItem::c4BreakCount() const
{
    return m_fcolBreaks.c4ElemCount();
}


tCIDLib::TCard4
TMEngFlowCtrlItem::c4BreakOffsetAt(const tCIDLib::TCard4 c4At) const
{
    // Throw our error for a bad index, instead of letting the collection do it
    if (c4At >= m_fcolBreaks.c4ElemCount())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_BadBreakIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4At)
        );
    }
    return m_fcolBreaks[c4At];
}


tCIDLib::TCard4 TMEngFlowCtrlItem::c4LineNum() const
{
    return m_c4LineNum;
}


tCIDLib::TCard4 TMEngFlowCtrlItem::c4Offset1() const
{
    return m_c4Offset1;
}

tCIDLib::TCard4 TMEngFlowCtrlItem::c4Offset1(const tCIDLib::TCard4 c4ToSet)
{
    m_c4Offset1 = c4ToSet;
    return m_c4Offset1;
}


tCIDLib::TCard4 TMEngFlowCtrlItem::c4Offset2() const
{
    return m_c4Offset2;
}

tCIDLib::TCard4 TMEngFlowCtrlItem::c4Offset2(const tCIDLib::TCard4 c4ToSet)
{
    m_c4Offset2 = c4ToSet;
    return m_c4Offset2;
}


tCIDMacroEng::EFlowTypes TMEngFlowCtrlItem::eType() const
{
    return m_eType;
}


