//
// FILE NAME: CIDMacroEng_CallStackItem.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2003
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
//  macro engine's call stack.
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
RTTIDecls(TMEngCallStackItem,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TMEngCallStackItem
// PREFIX: mecsi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngCallStackItem: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngCallStackItem::TMEngCallStackItem() :

    m_bRepush(kCIDLib::False)
    , m_c4CallIP(0)
    , m_c4CallLine(0)
    , m_c4Value(0)
    , m_eType(tCIDMacroEng::EStackItems::Unused)
    , m_pmeciCalled(0)
    , m_pmeciCaller(0)
    , m_pmecvPushed(0)
    , m_pmethiCalled(0)
    , m_pmethiCaller(0)
    , m_pmethCaller(0)
{
}

TMEngCallStackItem::~TMEngCallStackItem()
{
    // We own nothing
}


// ---------------------------------------------------------------------------
//  TMEngCallStackItem: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngCallStackItem::bHasCallerInfo() const
{
    return (m_pmeciCaller != 0);
}


tCIDLib::TBoolean TMEngCallStackItem::bIsImValue() const
{
    return (m_eType == tCIDMacroEng::EStackItems::Try);
}


tCIDLib::TBoolean TMEngCallStackItem::bIsMethod() const
{
    return (m_eType == tCIDMacroEng::EStackItems::MethodCall);
}


tCIDLib::TBoolean TMEngCallStackItem::bIsRepush() const
{
    return m_bRepush;
}

tCIDLib::TBoolean
TMEngCallStackItem::bIsRepush(const tCIDLib::TBoolean bToSet)
{
    m_bRepush = bToSet;
    return m_bRepush;
}


tCIDLib::TBoolean TMEngCallStackItem::bIsTempVal() const
{
    return (m_eType == tCIDMacroEng::EStackItems::TempVal);
}


tCIDLib::TBoolean TMEngCallStackItem::bIsValue() const
{
    return (m_eType == tCIDMacroEng::EStackItems::TempVal)
           || (m_eType == tCIDMacroEng::EStackItems::Local)
           || (m_eType == tCIDMacroEng::EStackItems::Member)
           || (m_eType == tCIDMacroEng::EStackItems::Parm)
           || (m_eType == tCIDMacroEng::EStackItems::StringPool)
           || (m_eType == tCIDMacroEng::EStackItems::This)
           || (m_eType == tCIDMacroEng::EStackItems::ColElem)
           || (m_eType == tCIDMacroEng::EStackItems::Exception);
}


tCIDLib::TCard4 TMEngCallStackItem::
c4QueryCallerInfo(  const   TMEngClassInfo*&        pmeciCaller
                    , const TMEngMethodInfo*&       pmethiCaller
                    , const TMEngOpMethodImpl*&     pmethCaller
                    ,       TMEngClassVal*&         pmecvCaller
                    ,       tCIDLib::TCard4&        c4CallIP)
{
    // Make sure this one is a call
    if (!bIsMethod())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_NotACallItem
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
        );
    }

    pmeciCaller = m_pmeciCaller;
    pmethiCaller = m_pmethiCaller;
    pmethCaller = m_pmethCaller;
    pmecvCaller = m_pmecvPushed;
    c4CallIP = m_c4CallIP;
    return m_c4CallLine;
}


tCIDLib::TCard4 TMEngCallStackItem::c4Value() const
{
    // Make sure this one has an immediate value
    if (!bIsImValue())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_NotImValueItem
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
        );
    }
    return m_c4Value;
}


tCIDLib::TVoid TMEngCallStackItem::Clear()
{
    m_bRepush       = kCIDLib::False;
    m_c4CallIP      = 0;
    m_c4CallLine    = 0;
    m_c4Value       = 0;
    m_eType         = tCIDMacroEng::EStackItems::Unused;
    m_pmeciCalled = 0;
    m_pmeciCaller = 0;
    m_pmecvPushed = 0;
    m_pmethiCalled = 0;
    m_pmethiCaller = 0;
    m_pmethCaller = 0;
}


tCIDMacroEng::EStackItems TMEngCallStackItem::eType() const
{
    return m_eType;
}


const TMEngClassInfo& TMEngCallStackItem::meciCalled() const
{
    // Make sure this one is a call
    if (!bIsMethod())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_NotACallItem
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
        );
    }
    return *m_pmeciCalled;
}

TMEngClassInfo& TMEngCallStackItem::meciCalled()
{
    // Make sure this one is a call
    if (!bIsMethod())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_NotACallItem
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
        );
    }
    return *m_pmeciCalled;
}


const TMEngClassVal& TMEngCallStackItem::mecvPushed() const
{
    // Make sure this one is a value
    if (!bIsValue())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_NotAValueItem
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
        );
    }
    return *m_pmecvPushed;
}

TMEngClassVal& TMEngCallStackItem::mecvPushed()
{
    // Make sure this one is a value
    if (!bIsValue())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_NotAValueItem
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
        );
    }
    return *m_pmecvPushed;
}


const TMEngMethodInfo& TMEngCallStackItem::methiCalled() const
{
    // Make sure this one is a method call
    if (!bIsMethod())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_NotACallItem
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
        );
    }
    return *m_pmethiCalled;
}

const TMEngClassInfo*
TMEngCallStackItem::pmeciCaller(tCIDLib::TCard4& c4CallLine) const
{
    c4CallLine = m_c4CallLine;
    return m_pmeciCaller;
}


tCIDLib::TVoid
TMEngCallStackItem::Reset(          TMEngClassInfo* const   pmeciCalled
                            , const TMEngMethodInfo* const  pmethiCalled)
{
    m_bRepush       = kCIDLib::False;
    m_c4CallIP      = 0;
    m_c4CallLine    = 0;
    m_c4Value       = 0;
    m_eType         = tCIDMacroEng::EStackItems::MethodCall;
    m_pmeciCalled   = pmeciCalled;
    m_pmeciCaller   = 0;
    m_pmecvPushed   = 0;
    m_pmethiCalled  = pmethiCalled;
    m_pmethiCaller  = 0;
    m_pmethCaller   = 0;
}


tCIDLib::TVoid
TMEngCallStackItem::Reset(          TMEngClassInfo* const   pmeciCalled
                            , const TMEngMethodInfo* const  pmethiCalled
                            , const TMEngClassInfo* const   pmeciCaller
                            , const TMEngMethodInfo* const  pmethiCaller
                            , const TMEngOpMethodImpl* const pmethCaller
                            ,       TMEngClassVal* const    pmecvCaller
                            , const tCIDLib::TCard4         c4CallLine
                            , const tCIDLib::TCard4         c4CallIP)
{
    m_bRepush       = kCIDLib::False;
    m_c4CallIP      = c4CallIP;
    m_c4CallLine    = c4CallLine;
    m_c4Value       = 0;
    m_eType         = tCIDMacroEng::EStackItems::MethodCall;
    m_pmeciCalled   = pmeciCalled;
    m_pmeciCaller   = pmeciCaller;
    m_pmecvPushed   = pmecvCaller;
    m_pmethiCalled  = pmethiCalled;
    m_pmethiCaller  = pmethiCaller;
    m_pmethCaller   = pmethCaller;
}


tCIDLib::TVoid
TMEngCallStackItem::Reset(          TMEngClassVal* const    pmecvPushed
                            , const tCIDMacroEng::EStackItems  eType)
{
    // Make sure they didn't indicate a procedure type
    if (eType == tCIDMacroEng::EStackItems::MethodCall)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_BadItemResetType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
        );
    }

    m_bRepush       = kCIDLib::False;
    m_c4CallIP      = 0;
    m_c4CallLine    = 0;
    m_c4Value       = 0;
    m_eType         = eType;
    m_pmeciCalled   = 0;
    m_pmeciCaller   = 0;
    m_pmecvPushed   = pmecvPushed;
    m_pmethiCalled  = 0;
    m_pmethiCaller  = 0;
    m_pmethCaller   = 0;
}

tCIDLib::TVoid
TMEngCallStackItem::Reset(  const   tCIDMacroEng::EStackItems  eType
                            , const tCIDLib::TCard4         c4Value)
{
    m_bRepush       = kCIDLib::False;
    m_c4CallIP      = 0;
    m_c4CallLine    = 0;
    m_c4Value       = c4Value;
    m_eType         = eType;
    m_pmeciCalled   = 0;
    m_pmeciCaller   = 0;
    m_pmecvPushed   = 0;
    m_pmethiCalled  = 0;
    m_pmethiCaller  = 0;
    m_pmethCaller   = 0;
}

