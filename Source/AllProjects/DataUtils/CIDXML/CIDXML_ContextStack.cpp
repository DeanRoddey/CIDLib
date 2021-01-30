//
// FILE NAME: CIDXML_ContextStack.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/30/1999
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
//  This file implements the TXMLContextStack class.
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
#include    "CIDXML_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TXMLContextStack,TObject)


// ---------------------------------------------------------------------------
//  CLASS: TXMLContextStack
// PREFIX: xcs
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLContextStack: Constructors and Destructor
// ---------------------------------------------------------------------------
TXMLContextStack::TXMLContextStack() :

    m_c4LevelTop(0)
    , m_c4LevelMax(32)
    , m_eTextType(tCIDXML::EElemTextTypes::Any)
    , m_pLevels(0)
{
    // Do the initial allocation of the level stack array
    m_pLevels = new TLevel[m_c4LevelMax];

    // Zero it out
    TRawMem::SetMemBuf
    (
        m_pLevels
        , kCIDLib::c1MinCard
        , sizeof(TLevel) * m_c4LevelMax
    );
}

TXMLContextStack::~TXMLContextStack()
{
    CleanUp();

    // And now delete the level array itself
    delete [] m_pLevels;
}


// ---------------------------------------------------------------------------
//  TXMLContextStack: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TXMLContextStack::AddAsChild(const tCIDLib::TCard4 c4ChildId)
{
    // Make sure its not empty
    #if CID_DEBUG_ON
    if (!m_c4LevelTop)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcCntx_StackIsEmpty
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif

    //
    //  Add this child id as the child of the current top. We have to first
    //  check to see if we need to go an initial allocation of the child
    //  id list. If its allocated, see if we need to expand it and do so if
    //  required.
    //
    TLevel* pLevel = &m_pLevels[m_c4LevelTop - 1];

    if (!pLevel->pc4ChildIds)
    {
        // If in debug mode, make sure the count is also zero
        #if CID_DEBUG_ON
        if (pLevel->c4ChildCount)
        {
            facCIDXML().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kXMLErrs::errcCntx_BadChildCount
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::Internal
            );
        }
        #endif

        pLevel->c4ChildMax = 32;
        pLevel->pc4ChildIds = new tCIDLib::TCard4[32];
    }
     else if (pLevel->c4ChildCount == pLevel->c4ChildMax)
    {
        // Calc a new array size and allocate it
        const tCIDLib::TCard4 c4NewSize = tCIDLib::TCard4(pLevel->c4ChildCount * 1.5);
        tCIDLib::TCard4* pc4New = new tCIDLib::TCard4[c4NewSize];

        // Copy over the old data and delete the old array
        TRawMem::CopyMemBuf
        (
            pc4New
            , pLevel->pc4ChildIds
            , pLevel->c4ChildCount * sizeof(tCIDLib::TCard4)
        );
        delete [] pLevel->pc4ChildIds;

        // Now store the new size and we are done
        pLevel->pc4ChildIds = pc4New;
        pLevel->c4ChildMax = c4NewSize;
    }

    // Ok, lets add it in now
    pLevel->pc4ChildIds[pLevel->c4ChildCount++] = c4ChildId;
}


tCIDLib::TBoolean TXMLContextStack::bIsEmpty() const
{
    return (m_c4LevelTop == 0);
}


tCIDLib::TBoolean TXMLContextStack::bPopLevel()
{
    // Make sure its not already empty
    #if CID_DEBUG_ON
    if (!m_c4LevelTop)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcCntx_StackIsEmpty
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif

    //
    //  Ok, lets bump the stack top down one. We have to then reset this
    //  entry to default state. We don't though delete anything. These entries
    //  will be reused over and over so we want to keep the high water mark
    //  (i.e. the current allocation sizes.)
    //
    m_c4LevelTop--;
    m_pLevels[m_c4LevelTop].pxdeclLevel = 0;
    m_pLevels[m_c4LevelTop].c4SpoolerId = 0;
    m_pLevels[m_c4LevelTop].c4ChildCount = 0;
    m_pLevels[m_c4LevelTop].c4MapCount = 0;

    // Update the text type field, if we are not empty
    if (m_c4LevelTop)
    {
        TXMLElemDecl* pxdeclTop = m_pLevels[m_c4LevelTop - 1].pxdeclLevel;
        CIDAssert(pxdeclTop != nullptr, L"Popped back to null XMLDecl level");
        m_eTextType = pxdeclTop->eTextType();
    }

    // Return whether we are empty or not
    return (m_c4LevelTop == 0);
}


tCIDXML::EElemTextTypes TXMLContextStack::eTextType() const
{
    return m_eTextType;
}


const tCIDLib::TCard4*
TXMLContextStack::pc4GetTopContent(tCIDLib::TCard4& c4Count)
{
    // Make sure its not empty
    #if CID_DEBUG_ON
    if (!m_c4LevelTop)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcCntx_StackIsEmpty
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif

    c4Count = m_pLevels[m_c4LevelTop - 1].c4ChildCount;
    return m_pLevels[m_c4LevelTop - 1].pc4ChildIds;
}


const TXMLElemDecl&
TXMLContextStack::xdeclTopElem(tCIDLib::TCard4& c4SpoolerId) const
{
    // Make sure its not empty
    #if CID_DEBUG_ON
    if (!m_c4LevelTop)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcCntx_StackIsEmpty
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif

    // Make sure the element is set already
    const TXMLElemDecl* pxdeclRet = m_pLevels[m_c4LevelTop - 1].pxdeclLevel;

    #if CID_DEBUG_ON
    if (!pxdeclRet)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcCntx_TopElemNotSet
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif

    c4SpoolerId = m_pLevels[m_c4LevelTop - 1].c4SpoolerId;
    return *pxdeclRet;
}


tCIDLib::TVoid
TXMLContextStack::PushNewLevel(         TXMLElemDecl* const pxdeclNew
                                , const tCIDLib::TCard4     c4SpoolerId)
{
    // See if we need to expand the stack array
    if (m_c4LevelTop == m_c4LevelMax)
    {
        // Calculate a new size and allocate a new buffer to use.
        const tCIDLib::TCard4 c4NewSize = tCIDLib::TCard4(m_c4LevelMax * 1.25);
        TLevel* pNewLevels = new TLevel[c4NewSize];

        // Copy over the current content
        TRawMem::CopyMemBuf
        (
            pNewLevels
            , m_pLevels
            , sizeof(TLevel) * m_c4LevelMax
        );

        // Zero out the rest of it
        TRawMem::SetMemBuf
        (
            &pNewLevels[m_c4LevelMax]
            , kCIDLib::c1MinCard
            , sizeof(TLevel) * (c4NewSize - m_c4LevelMax)
        );

        // Now we can delete the old buffer, and store the new info
        delete [] m_pLevels;
        m_pLevels = pNewLevels;
        m_c4LevelMax = c4NewSize;
    }

    //
    //  Ok, lets add the new one in. Its already in the default stack so we
    //  just store the passed element (which can be null since it can be set
    //  later.)
    //
    m_pLevels[m_c4LevelTop].c4SpoolerId = c4SpoolerId;
    m_pLevels[m_c4LevelTop++].pxdeclLevel = pxdeclNew;

    // Update the text type flag if we got an element
    if (pxdeclNew)
        m_eTextType = pxdeclNew->eTextType();
}


tCIDLib::TVoid TXMLContextStack::Reset()
{
    // Cleanup the data we hold first
    CleanUp();

    // Now reset back to the start
    m_c4LevelTop = 0;
    m_eTextType = tCIDXML::EElemTextTypes::Any;
    TRawMem::SetMemBuf
    (
        m_pLevels
        , kCIDLib::c1MinCard
        , sizeof(TLevel) * m_c4LevelMax
    );
}


// ---------------------------------------------------------------------------
//  TXMLContextStack: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Cleans up the existing children and map items, freeing all of their
//  memory.
//
tCIDLib::TVoid TXMLContextStack::CleanUp()
{
    //
    //  Clean up our level stack. First we have to pass through it and clean
    //  up the child element and namespace map arrays in each one. Some
    //  them at the end won't have been used, but that's ok. They'll just
    //  have zero counts for the stuff we need to delete and we won't do
    //  anything fo those.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4LevelMax; c4Index++)
    {
        // Get the current level
        TLevel& CurLev = m_pLevels[c4Index];

        if (CurLev.pc4ChildIds)
        {
            delete [] CurLev.pc4ChildIds;
            CurLev.pc4ChildIds = 0;
            CurLev.c4ChildCount = 0;
        }

        if (CurLev.pMapItems)
        {
            delete [] CurLev.pMapItems;
            CurLev.pMapItems = 0;
            CurLev.c4MapCount = 0;
        }
    }
}

