//
// FILE NAME: CIDMacroEng_MethodImpl.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2003
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
//  This file implements TMEngMethodImpl class. This class represents the
//  actual implementation of a 'method' in the macro engine system. External
//  methods (those handled by C++ code) can each provide a derivative of
//  this class and override the Invoke() method. We provide a single, generic
//  one that is used to do all native (opcode based) methods. It's Invoke()
//  override is the one that interprets opcodes.
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
RTTIDecls(TMEngMethodImpl,TMEngNamedItem)
RTTIDecls(TMEngOpMethodImpl,TMEngMethodImpl)




// ---------------------------------------------------------------------------
//  CLASS: TMEngJumpTableItem
// PREFIX: jtbli
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngJumpTableItem: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngJumpTableItem::
TMEngJumpTableItem( const   tCIDLib::TCard4         c4IPToSet
                    ,       TMEngClassVal* const    pmecvToAdopt) :
    m_c4IP(c4IPToSet)
    , m_pmecvCase(pmecvToAdopt)
{
}

TMEngJumpTableItem::~TMEngJumpTableItem()
{
    delete m_pmecvCase;
}


// ---------------------------------------------------------------------------
//  TMEngJumpTableItem: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::ESortComps
TMEngJumpTableItem::eComp(  const   TMEngClassVal&      mecvToCheck
                            ,       tCIDLib::TCard4&    c4IP) const
{
    #if CID_DEBUG_ON
    if (m_pmecvCase->c2ClassId() != mecvToCheck.c2ClassId())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcDbg_DifferentCaseTypes
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
            , TCardinal(m_pmecvCase->c2ClassId())
            , TCardinal(mecvToCheck.c2ClassId())
        );
    }
    #endif

    tCIDLib::ESortComps eRet = tCIDLib::ESortComps::Equal;
    switch(tCIDMacroEng::EIntrinsics(mecvToCheck.c2ClassId()))
    {
        case tCIDMacroEng::EIntrinsics::Card1 :
        {
            const TMEngCard1Val& mecvThis = *static_cast<const TMEngCard1Val*>(m_pmecvCase);
            const TMEngCard1Val& mecvThat = static_cast<const TMEngCard1Val&>(mecvToCheck);
            if (mecvThis.c1Value() < mecvThat.c1Value())
                eRet = tCIDLib::ESortComps::FirstLess;
            else if (mecvThis.c1Value() > mecvThat.c1Value())
                eRet = tCIDLib::ESortComps::FirstGreater;
            break;
        }

        case tCIDMacroEng::EIntrinsics::Card2 :
        {
            const TMEngCard2Val& mecvThis = *static_cast<const TMEngCard2Val*>(m_pmecvCase);
            const TMEngCard2Val& mecvThat = static_cast<const TMEngCard2Val&>(mecvToCheck);
            if (mecvThis.c2Value() < mecvThat.c2Value())
                eRet = tCIDLib::ESortComps::FirstLess;
            else if (mecvThis.c2Value() > mecvThat.c2Value())
                eRet = tCIDLib::ESortComps::FirstGreater;
            break;
        }

        case tCIDMacroEng::EIntrinsics::Card4 :
        {
            const TMEngCard4Val& mecvThis = *static_cast<const TMEngCard4Val*>(m_pmecvCase);
            const TMEngCard4Val& mecvThat = static_cast<const TMEngCard4Val&>(mecvToCheck);
            if (mecvThis.c4Value() < mecvThat.c4Value())
                eRet = tCIDLib::ESortComps::FirstLess;
            else if (mecvThis.c4Value() > mecvThat.c4Value())
                eRet = tCIDLib::ESortComps::FirstGreater;
            break;
        }

        case tCIDMacroEng::EIntrinsics::Int1 :
        {
            const TMEngInt1Val& mecvThis = *static_cast<const TMEngInt1Val*>(m_pmecvCase);
            const TMEngInt1Val& mecvThat= static_cast<const TMEngInt1Val&>(mecvToCheck);
            if (mecvThis.i1Value() < mecvThat.i1Value())
                eRet = tCIDLib::ESortComps::FirstLess;
            else if (mecvThis.i1Value() > mecvThat.i1Value())
                eRet = tCIDLib::ESortComps::FirstGreater;
            break;
        }

        case tCIDMacroEng::EIntrinsics::Int2 :
        {
            const TMEngInt2Val& mecvThis = *static_cast<const TMEngInt2Val*>(m_pmecvCase);
            const TMEngInt2Val& mecvThat= static_cast<const TMEngInt2Val&>(mecvToCheck);
            if (mecvThis.i2Value() < mecvThat.i2Value())
                eRet = tCIDLib::ESortComps::FirstLess;
            else if (mecvThis.i2Value() > mecvThat.i2Value())
                eRet = tCIDLib::ESortComps::FirstGreater;
            break;
        }

        case tCIDMacroEng::EIntrinsics::Int4 :
        {
            const TMEngInt4Val& mecvThis = *static_cast<const TMEngInt4Val*>(m_pmecvCase);
            const TMEngInt4Val& mecvThat= static_cast<const TMEngInt4Val&>(mecvToCheck);
            if (mecvThis.i4Value() < mecvThat.i4Value())
                eRet = tCIDLib::ESortComps::FirstLess;
            else if (mecvThis.i4Value() > mecvThat.i4Value())
                eRet = tCIDLib::ESortComps::FirstGreater;
            break;
        }

        case tCIDMacroEng::EIntrinsics::Char :
        {
            const TMEngCharVal& mecvThis = *static_cast<const TMEngCharVal*>(m_pmecvCase);
            const TMEngCharVal& mecvThat= static_cast<const TMEngCharVal&>(mecvToCheck);
            if (mecvThis.chValue() < mecvThat.chValue())
                eRet = tCIDLib::ESortComps::FirstLess;
            else if (mecvThis.chValue() > mecvThat.chValue())
                eRet = tCIDLib::ESortComps::FirstGreater;
            break;
        }

        default :
        {
            //
            //  It should be some enum derivative. If debugging, then use our C++
            //  level RTTI to insure it is.
            //
            #if CID_DEBUG_ON
            if (!mecvToCheck.bIsDescendantOf(TMEngEnumVal::clsThis()))
            {
                facCIDMacroEng().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kMEngErrs::errcDbg_UnknownSwitchType
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Internal
                    , TCardinal(mecvToCheck.c2ClassId())
                );
            }
            #endif

            const TMEngEnumVal& mecvThis = *static_cast<const TMEngEnumVal*>(m_pmecvCase);
            const TMEngEnumVal& mecvThat= static_cast<const TMEngEnumVal&>(mecvToCheck);
            if (mecvThis.c4Ordinal() < mecvThat.c4Ordinal())
                eRet = tCIDLib::ESortComps::FirstLess;
            else if (mecvThis.c4Ordinal() > mecvThat.c4Ordinal())
                eRet = tCIDLib::ESortComps::FirstGreater;
            break;
        }
    }

    if (eRet == tCIDLib::ESortComps::Equal)
        c4IP = m_c4IP;

    return eRet;
}


// ---------------------------------------------------------------------------
//  CLASS: TMEngJumpTable
// PREFIX: jtbl
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngJumpTable: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngJumpTable::TMEngJumpTable() :

    m_c4DefIP(kCIDLib::c4MaxCard)
    , m_colCases(tCIDLib::EAdoptOpts::Adopt)
{
}

TMEngJumpTable::~TMEngJumpTable()
{
}


// ---------------------------------------------------------------------------
//  TMEngJumpTable: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngJumpTable::AddDefaultItem(const tCIDLib::TCard4 c4IPToSet)
{
    m_c4DefIP = c4IPToSet;
}


tCIDLib::TVoid
TMEngJumpTable::AddItem(const   tCIDLib::TCard4         c4IPToSet
                        ,       TMEngClassVal* const    pmecvToAdopt)
{
    m_colCases.Add(new TMEngJumpTableItem(c4IPToSet, pmecvToAdopt));
}


tCIDLib::TBoolean
TMEngJumpTable::bFindMatch( const   TMEngClassVal&      mecvToCheck
                            ,       tCIDLib::TCard4&    c4IP) const
{
    const tCIDLib::TCard4 c4Count = m_colCases.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colCases[c4Index]->eComp(mecvToCheck, c4IP) == tCIDLib::ESortComps::Equal)
            return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean TMEngJumpTable::bHasRequiredItems() const
{
    return (!m_colCases.bIsEmpty() && (m_c4DefIP != kCIDLib::c4MaxCard));
}


tCIDLib::TCard4 TMEngJumpTable::c4DefCaseIP() const
{
    return m_c4DefIP;
}


TMEngJumpTableItem& TMEngJumpTable::jtbliAt(const tCIDLib::TCard4 c4At)
{
    #if CID_DEBUG_ON
    if (c4At >= m_colCases.c4ElemCount())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcDbg_BadJmpTableIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4At)
        );
    }
    #endif
    return *m_colCases[c4At];
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngMethodImpl
// PREFIX: memeth
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngMethodImpl: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngMethodImpl::strKey(const TMEngMethodImpl& memethSrc)
{
    return memethSrc.strName();
}


// ---------------------------------------------------------------------------
//  TMEngMethodImpl: Destructor
// ---------------------------------------------------------------------------
TMEngMethodImpl::~TMEngMethodImpl()
{
    // Clean up the locals and string pool collections
    delete m_pcolLocalList;
    delete m_pcolStringPool;
}


// ---------------------------------------------------------------------------
//  TMEngMethodImpl: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard2 TMEngMethodImpl::c2AddLocal(const TMEngLocalInfo& meliToAdd)
{
    // If not allocated yet, create the locals collection
    if (!m_pcolLocalList)
        m_pcolLocalList = new TLocalList(16);

    // Make sure we don't have one with this name already
    const tCIDLib::TCard4 c4Count = m_pcolLocalList->c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_pcolLocalList->objAt(c4Index).strName() == meliToAdd.strName())
        {
            facCIDMacroEng().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcMeth_DupLocalName
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Index
                , strName()
                , meliToAdd.strName()
            );
        }
    }

    facCIDMacroEng().CheckIdOverflow(c4Count, L"Locals");

    // Looks ok, so take it
    const tCIDLib::TCard2 c2Id = tCIDLib::TCard2(c4Count);
    TMEngLocalInfo& meliNew = m_pcolLocalList->objAdd(meliToAdd);
    meliNew.c2Id(c2Id);
    return c2Id;
}


tCIDLib::TCard2
TMEngMethodImpl::c2AddString(const  TString&            strToAdd
                            , const tCIDLib::TBoolean   bFindDups)
{
    // Allocate the pool collection if not already
    if (!m_pcolStringPool)
        m_pcolStringPool = new TStringPool(tCIDLib::EAdoptOpts::Adopt, 8);

    //
    //  If we are to find dups, then see if we already have this string in
    //  the pool and don't add a new copy, just give back the existing id.
    //
    if (bFindDups && !m_pcolStringPool->bIsEmpty())
    {
        const tCIDLib::TCard4 c4Count = m_pcolStringPool->c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            if (strToAdd == m_pcolStringPool->pobjAt(c4Index)->strValue())
                return tCIDLib::TCard2(c4Index);
        }
    }

    // Create a string value object with the passed value
    const tCIDLib::TCard2 c2NewId(tCIDLib::TCard2(m_pcolStringPool->c4ElemCount()));

    TString strName(L"PoolItem");
    strName.AppendFormatted(c2NewId);
    TMEngStringVal* pmecvNew = new TMEngStringVal
    (
        strName
        , tCIDMacroEng::EConstTypes::Const
        , strToAdd
    );

    // Add it, and set it's id to the next id
    pmecvNew->c2Id(c2NewId);
    m_pcolStringPool->Add(pmecvNew);
    return pmecvNew->c2Id();
}


tCIDLib::TCard4 TMEngMethodImpl::c4LocalCount() const
{
    // If not allocated yet, then zero definitely
    if (!m_pcolLocalList)
        return 0;
    return m_pcolLocalList->c4ElemCount();
}


tCIDLib::TCard4 TMEngMethodImpl::c4StringCount() const
{
    // If not allocated yet, then zero definitely
    if (!m_pcolStringPool)
        return 0;
    return m_pcolStringPool->c4ElemCount();
}


const TMEngClassVal&
TMEngMethodImpl::mecvFindPoolItem(const tCIDLib::TCard2 c2Id) const
{
    if (!m_pcolStringPool || (c2Id >= m_pcolStringPool->c4ElemCount()))
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcMeth_BadStringPoolId
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c2Id)
            , strName()
        );
    }
    return *m_pcolStringPool->pobjAt(c2Id);
}

TMEngClassVal& TMEngMethodImpl::mecvFindPoolItem(const tCIDLib::TCard2 c2Id)
{
    if (!m_pcolStringPool || (c2Id >= m_pcolStringPool->c4ElemCount()))
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcMeth_BadStringPoolId
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c2Id)
            , strName()
        );
    }
    return *m_pcolStringPool->pobjAt(c2Id);
}


const TMEngLocalInfo&
TMEngMethodImpl::meliFind(const tCIDLib::TCard2 c2Id) const
{
    if (!m_pcolLocalList || (c2Id >= m_pcolLocalList->c4ElemCount()))
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcMeth_BadLocalId
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c2Id)
            , strName()
        );
    }
    return m_pcolLocalList->objAt(c2Id);
}

TMEngLocalInfo& TMEngMethodImpl::meliFind(const tCIDLib::TCard2 c2Id)
{
    if (!m_pcolLocalList || (c2Id >= m_pcolLocalList->c4ElemCount()))
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcMeth_BadLocalId
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c2Id)
            , strName()
        );
    }
    return m_pcolLocalList->objAt(c2Id);
}



const TMEngLocalInfo* TMEngMethodImpl::pmeliFind(const TString& strName) const
{
    // If we've not allocated the collection, obviously not
    if (!m_pcolLocalList)
        return nullptr;

    // Make sure we don't have one with this name already
    const tCIDLib::TCard4 c4Count = m_pcolLocalList->c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TMEngLocalInfo& meliCur = m_pcolLocalList->objAt(c4Index);
        if (meliCur.strName() == strName)
            return &meliCur;
    }
    return nullptr;
}

TMEngLocalInfo* TMEngMethodImpl::pmeliFind(const TString& strName)
{
    // If we've not allocated the collection, obviously not
    if (!m_pcolLocalList)
        return nullptr;

    // Make sure we don't have one with this name already
    const tCIDLib::TCard4 c4Count = m_pcolLocalList->c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TMEngLocalInfo& meliCur = m_pcolLocalList->objAt(c4Index);
        if (meliCur.strName() == strName)
            return &meliCur;
    }
    return nullptr;
}


tCIDLib::TVoid TMEngMethodImpl::PushLocals(TCIDMacroEngine& meOwner)
{
    // If not allocated yet, create the locals collection
    if (!m_pcolLocalList)
        m_pcolLocalList = new TLocalList(16);

    const tCIDLib::TCard4 c4Count = m_pcolLocalList->c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TMEngLocalInfo& meliCur = m_pcolLocalList->objAt(c4Index);

        //
        //  Look up the type of this parm and create a value.
        //
        //  <TBD> We cannot use pool values for locals, because the negate
        //  opcode will convert a non-const pool value in place instead
        //  of replacing it. We have to have some way to say it's a pool
        //  value, so it get's cleaned up ok, but also say it's a local
        //  so that it get's treated correctly.
        //
        //  For now, we just create a new object for all locals.
        //
        const tCIDLib::TCard2 c2Id = meliCur.c2ClassId();
        TMEngClassVal* pmecvLocal = meOwner.meciFind(c2Id).pmecvMakeStorage
        (
            meliCur.strName(), meOwner, meliCur.eConst()
        );
        meOwner.PushValue(pmecvLocal, tCIDMacroEng::EStackItems::Local);
    }
}


// ---------------------------------------------------------------------------
//  TMEngMethodImpl: Hidden Constructors
// ---------------------------------------------------------------------------
TMEngMethodImpl::TMEngMethodImpl(const  TString&        strName
                                , const tCIDLib::TCard2 c2MethodId) :

    TMEngNamedItem(strName, c2MethodId)
    , m_pcolLocalList(nullptr)
    , m_pcolStringPool(nullptr)
{
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngOpMethodImpl
// PREFIX: memeth
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngOpMethodImpl::TMEngOpMethodImpl(const  TString&        strName
                                    , const tCIDLib::TCard2 c2MethodId) :

    TMEngMethodImpl(strName, c2MethodId)
    , m_colOpCodes(128)
    , m_pjtblSwitches(nullptr)
{
}

TMEngOpMethodImpl::~TMEngOpMethodImpl()
{
    // Delete the jump table list if we have one
    try
    {
        delete m_pjtblSwitches;
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }

    catch(...)
    {
    }
}


// ---------------------------------------------------------------------------
//  TMEngMethodImpl: Public operators
// ---------------------------------------------------------------------------
const TMEngOpCode&
TMEngOpMethodImpl::operator[](const tCIDLib::TCard4 c4Index) const
{
    if (c4Index >= m_colOpCodes.c4ElemCount())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcMeth_BadIP
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , strName()
            , TCardinal(c4Index)
        );
    }
    return m_colOpCodes[c4Index];
}

TMEngOpCode& TMEngOpMethodImpl::operator[](const tCIDLib::TCard4 c4Index)
{
    if (c4Index >= m_colOpCodes.c4ElemCount())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcMeth_BadIP
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , strName()
            , TCardinal(c4Index)
        );
    }
    return m_colOpCodes[c4Index];
}


// ---------------------------------------------------------------------------
//  TMEngOpMethodImpl: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngOpMethodImpl::Invoke(          TMEngClassVal&      mecvInstance
                            , const TMEngClassInfo&     meciTarget
                            , const TMEngMethodInfo&    methiThis
                            ,       TCIDMacroEngine&    meOwner)
{
    //
    //  Remember the stack top upon entry. If we throw out of here, we have
    //  to remember to clean up back to there again.
    //
    const tCIDLib::TCard4 c4BasePointer = meOwner.c4StackTop();

    //
    //  We have two levels of exception handling, one for exceptions coming out of
    //  a called method and one that occurs within the current method. We need
    //  to know that the inner one was done, so that we don't do some redundant
    //  work on the second one after the inner rethrows.
    //
    tCIDLib::TBoolean bThrowReported = kCIDLib::False;

    //
    //  Get a pointer to the debugger interface. The macro engine owns this,
    //  so we just get a pointer to avoid having to call his getter over and
    //  over. We'll call back on this every time we hit a line opcode. It will
    //  usually be zero, in which case we do nothing with it.
    //
    MMEngDebugIntf* pmedbgToCall = meOwner.pmedbgToUse();

    #if CID_DEBUG_ON
    tCIDLib::TBoolean bDoDump = kCIDLib::False;
    if (bDoDump)
    {
        DumpOpCodes(*meOwner.pstrmConsole(), meOwner);
        meOwner.pstrmConsole()->Flush();
    }
    #endif

    //
    //  If we have any locals, then push them now. Their ids are relative
    //  to the base pointer we stored above.
    //
    if (c4LocalCount())
    {
        PushLocals(meOwner);

        // If we have a debug callback, tell him new locals were created
        if (pmedbgToCall)
            pmedbgToCall->LocalsChange(kCIDLib::True);
    }

    //
    //  And now let's enter the opcode processing loop. We process opcodes
    //  in the order found, except when we hit jump opcodes. If we hit the
    //  end of the opcodes or we hit a return opcode, we will break out.
    //
    try
    {
        // We need an index, which is our 'instruction pointer'
        tCIDLib::TCard4 c4IP = 0;

        //
        //  Remember the opcode count. If we get to this, then we are at
        //  the end of the method.
        //
        const tCIDLib::TCard4 c4EndIndex = m_colOpCodes.c4ElemCount();

        tCIDLib::TBoolean   bDone = kCIDLib::False;
        tCIDLib::TBoolean   bNoIncIP = kCIDLib::False;
        while (!bDone)
        {
            // Get the current opcode
            const TMEngOpCode& meopCur = m_colOpCodes[c4IP];

            switch(meopCur.eOpCode())
            {
                case tCIDMacroEng::EOpCodes::CallExcept :
                case tCIDMacroEng::EOpCodes::CallLocal :
                case tCIDMacroEng::EOpCodes::CallMember :
                case tCIDMacroEng::EOpCodes::CallParent :
                case tCIDMacroEng::EOpCodes::CallParm :
                case tCIDMacroEng::EOpCodes::CallStack :
                case tCIDMacroEng::EOpCodes::CallThis :
                {
                    TMEngClassVal*  pmecvTarget;
                    const TMEngClassInfo* pmeciTarget;
                    tCIDLib::TCard2 c2MethId;

                    if (meopCur.eOpCode() == tCIDMacroEng::EOpCodes::CallExcept)
                    {
                        pmecvTarget = &meOwner.mecvExceptionThrown();
                        pmeciTarget = &meOwner.meciFind(pmecvTarget->c2ClassId());
                        c2MethId = meopCur[0];
                    }
                     else if (meopCur.eOpCode() == tCIDMacroEng::EOpCodes::CallStack)
                    {
                        pmecvTarget = &meOwner.mecvStackAt
                        (
                            meOwner.c4StackTop() - meopCur[0]
                        );
                        pmeciTarget = &meOwner.meciFind(pmecvTarget->c2ClassId());
                        c2MethId = meopCur[1];
                    }
                     else if (meopCur.eOpCode() == tCIDMacroEng::EOpCodes::CallThis)
                    {
                        pmecvTarget = &mecvInstance;
                        pmeciTarget = &meciTarget;
                        c2MethId = meopCur[0];
                    }
                     else if (meopCur.eOpCode() == tCIDMacroEng::EOpCodes::CallParent)
                    {
                        pmecvTarget = &mecvInstance;
                        pmeciTarget = &meOwner.meciFind(meciTarget.c2ParentClassId());
                        c2MethId = meopCur[0];
                    }
                     else
                    {
                        //
                        //  The first index is the id of the member, local, or
                        //  parm that the call is being made against. So,
                        //  according to the opcode, look up the target value
                        //  object.
                        //
                        if (meopCur.eOpCode() == tCIDMacroEng::EOpCodes::CallLocal)
                        {
                            //
                            //  The local ids are relative to the base pointer
                            //  for our stack frame.
                            //
                            pmecvTarget = &meOwner.mecvStackAt
                            (
                                c4BasePointer + meopCur[0]
                            );
                        }
                         else if (meopCur.eOpCode() == tCIDMacroEng::EOpCodes::CallMember)
                        {
                            // Get the indicated member, in the first index
                            pmecvTarget = &mecvInstance.mecvFind(meopCur[0], meOwner);
                        }
                         else if (meopCur.eOpCode() == tCIDMacroEng::EOpCodes::CallParm)
                        {
                            // It's a parm already on the stack
                            pmecvTarget = &meOwner.mecvStackAt
                            (
                                (c4BasePointer - (methiThis.c4ParmCount() + 1))
                                + meopCur[0]
                            );
                        }
                         else
                        {
                            // Not a known target type, so throw
                            facCIDMacroEng().ThrowErr
                            (
                                CID_FILE
                                , CID_LINE
                                , kMEngErrs::errcEng_UnknownCallType
                                , tCIDLib::ESeverities::Failed
                                , tCIDLib::EErrClasses::Unknown
                                , TInteger(tCIDLib::i4EnumOrd(meopCur.eOpCode()))
                            );
                        }

                        // Get the class info for the class of the target
                        pmeciTarget = &meOwner.meciFind(pmecvTarget->c2ClassId());

                        c2MethId = meopCur[1];
                    }

                    //
                    //  And invoke it. The parameters are on the call stack,
                    //  but we have to push the method onto the call stack
                    //  before invoking it.
                    //
                    try
                    {
                        //
                        //  Push a call item on the stack and if we have a debugger
                        //  installed, let it know about the change.
                        //
                        meOwner.meciPushMethodCall
                        (
                            pmeciTarget->c2Id()
                            , c2MethId
                            , &meciTarget
                            , this
                            , &methiThis
                            , &mecvInstance
                            , meOwner.c4CurLine()
                            , c4IP
                        );
                        if (pmedbgToCall)
                            pmedbgToCall->CallStackChange();

                        //
                        //  Looks ok, so call the class with the instance and
                        //  method info. It will pass it on to the appropriate
                        //  method.
                        //
                        //  If we are doing a parent call, that's a monomorphic
                        //  dispatch, else do polymorphic to go to the most
                        //  derived.
                        //
                        tCIDMacroEng::EDispatch eDispatch;
                        if (meopCur.eOpCode() == tCIDMacroEng::EOpCodes::CallParent)
                            eDispatch = tCIDMacroEng::EDispatch::Mono;
                        else
                            eDispatch = tCIDMacroEng::EDispatch::Poly;

                        pmeciTarget->Invoke
                        (
                            meOwner
                            , *pmecvTarget
                            , c2MethId
                            , eDispatch
                        );

                        //
                        //  We clean off the call item, since we pushed it,
                        //  and let any debugger know about it.
                        //
                        meOwner.PopTop();
                        if (pmedbgToCall)
                            pmedbgToCall->CallStackChange();
                    }

                    catch(TError& errToCatch)
                    {
                        //
                        //  We clean off the call item, since we pushed it,
                        //  and let any debugger know about it.
                        //
                        meOwner.PopTop();
                        if (pmedbgToCall)
                            pmedbgToCall->CallStackChange();

                        if (facCIDMacroEng().bLogFailures() && !meOwner.bInIDE())
                        {
                            facCIDMacroEng().LogMsg
                            (
                                CID_FILE
                                , CID_LINE
                                , kMEngErrs::errcEng_ExceptionFrom
                                , tCIDLib::ESeverities::Failed
                                , tCIDLib::EErrClasses::AppStatus
                                , pmeciTarget->strClassPath()
                                , pmeciTarget->methiFind(c2MethId).strName()
                            );
                        }

                        // If the error itself hasn't been logged, then do it
                        if (facCIDMacroEng().bShouldLog(errToCatch))
                            TModule::LogEventObj(errToCatch);

                        // Let the error handler know about it, if not already
                        if (!errToCatch.bReported())
                            meOwner.Exception(errToCatch);
                        bThrowReported = kCIDLib::True;

                        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                        throw;
                    }

                    catch(const TDbgExitException&)
                    {
                        //
                        //  We clean off the call item, since we pushed it,
                        //  and let any debugger know about it.
                        //
                        meOwner.PopTop();
                        if (pmedbgToCall)
                            pmedbgToCall->CallStackChange();

                        // Just rethrow. We are being forced down
                        throw;
                    }

                    catch(const TExceptException&)
                    {
                        //
                        //  The called method threw a macro level exception
                        //  and didn't handle it. So we need to see if we
                        //  have a handler. If so, jump to the catch block,
                        //  else, rethrow.
                        //
                        tCIDLib::TCard4 c4New;
                        const tCIDLib::TCard4 c4Tmp = meOwner.c4FindNextTry(c4New);
                        if ((c4Tmp < c4BasePointer) || (c4Tmp == kCIDLib::c4MaxCard))
                            throw;

                        // Pop back to the indicated position
                        meOwner.PopBackTo(c4Tmp);
                        if (pmedbgToCall)
                            pmedbgToCall->CallStackChange();

                        // And set the IP to the catch IP
                        bNoIncIP = kCIDLib::True;
                        c4IP = c4New;
                    }

                    catch(...)
                    {
                        //
                        //  We clean off the call item, since we pushed it,
                        //  and let any debugger know about it.
                        //
                        meOwner.PopTop();
                        if (pmedbgToCall)
                            pmedbgToCall->CallStackChange();

                        if (facCIDMacroEng().bLogFailures())
                        {
                            facCIDMacroEng().LogMsg
                            (
                                CID_FILE
                                , CID_LINE
                                , kMEngErrs::errcEng_UnknownExceptFrom
                                , tCIDLib::ESeverities::Status
                                , tCIDLib::EErrClasses::AppStatus
                                , pmeciTarget->strClassPath()
                                , pmeciTarget->methiFind(c2MethId).strName()
                            );
                        }

                        // Tell the engine about it
                        meOwner.UnknownException();
                        bThrowReported = kCIDLib::True;

                        throw;
                    }
                    break;
                }

                case tCIDMacroEng::EOpCodes::ColIndex :
                {
                    //
                    //  The top of stack is an index value, and the one before
                    //  that is a collection object.
                    //
                    const tCIDLib::TCard4 c4Top = meOwner.c4StackTop();
                    TMEngClassVal& mecvIndex = meOwner.mecvStackAt(c4Top - 1);
                    TMEngClassVal& mecvCollect = meOwner.mecvStackAt(c4Top - 2);

                    #if CID_DEBUG_ON
                    if (!mecvCollect.bIsDescendantOf(TMEngCollectVal::clsThis()))
                    {
                        TMEngClassInfo& meciCol = meOwner.meciFind(mecvCollect.c2ClassId());
                        facCIDMacroEng().ThrowErr
                        (
                            CID_FILE
                            , CID_LINE
                            , kMEngErrs::errcDbg_NotCollectionType
                            , tCIDLib::ESeverities::Failed
                            , tCIDLib::EErrClasses::TypeMatch
                            , meciCol.strClassPath()
                        );
                    }
                    #endif

                    //
                    //  Cast the value object to the (C++ level) base collection
                    //  value class, and ask it to get us the object at the
                    //  indicated index.
                    //
                    //  It will throw a macro level index error if the index is
                    //  not good.
                    //
                    try
                    {
                        TMEngCollectVal& mecvActual
                        (
                            static_cast<TMEngCollectVal&>(mecvCollect)
                        );
                        TMEngClassVal* pmecvElem
                        (
                            mecvActual.pmecvGetAt(meOwner, mecvIndex)
                        );

                        // Pop the two values off the stack top
                        meOwner.MultiPop(2);

                        // And push the new value
                        meOwner.PushValue(pmecvElem, tCIDMacroEng::EStackItems::ColElem);
                    }

                    catch(const TExceptException&)
                    {
                        // Do the macro level try/catch handling
                        tCIDLib::TCard4 c4New;
                        const tCIDLib::TCard4 c4Tmp = meOwner.c4FindNextTry(c4New);
                        if ((c4Tmp < c4BasePointer) || (c4Tmp == kCIDLib::c4MaxCard))
                            throw;

                        // Pop back to the indicated position
                        meOwner.PopBackTo(c4Tmp);

                        // And set the IP to the catch IP
                        bNoIncIP = kCIDLib::True;
                        c4IP = c4New;
                    }
                    break;
                };

                case tCIDMacroEng::EOpCodes::Copy :
                {
                    const tCIDLib::TCard4 c4Top = meOwner.c4StackTop();
                    TMEngClassVal& mecvSrc = meOwner.mecvStackAt(c4Top - 1);
                    TMEngClassVal& mecvTar = meOwner.mecvStackAt(c4Top - 2);

                    if (meOwner.bValidation())
                    {
                        TMEngClassInfo& meciSrc = meOwner.meciFind(mecvSrc.c2ClassId());
                        TMEngClassInfo& meciTar = meOwner.meciFind(mecvTar.c2ClassId());

                        if (!meciSrc.bIsCopyable() || !meciTar.bIsCopyable())
                        {
                            facCIDMacroEng().ThrowErr
                            (
                                CID_FILE
                                , CID_LINE
                                , kMEngErrs::errcRT_BadCopyOpParms
                                , tCIDLib::ESeverities::Failed
                                , tCIDLib::EErrClasses::TypeMatch
                                , meciSrc.strClassPath()
                                , meciTar.strClassPath()
                            );
                        }
                    }

                    // If the source and target are not the same, do the copy
                    if (&mecvSrc != &mecvTar)
                        mecvTar.CopyFrom(mecvSrc, meOwner);

                    // And pop them both off
                    meOwner.MultiPop(2);
                    break;
                }

                case tCIDMacroEng::EOpCodes::CondEnumInc :
                case tCIDMacroEng::EOpCodes::ResetEnum :
                {
                    //
                    //  The top of stack must be an enumerated object. We will
                    //  look at it and if it is not at it's max, we will
                    //  increment it, else we'll do nothing. We leave a boolean
                    //  value on the stack to indicate whether we incremented
                    //  it or not.
                    //
                    TMEngClassVal& mecvTop = meOwner.mecvStackAtTop();

                    // If validating, make sure it's an enum
                    if (meOwner.bValidation())
                    {
                        if (!meOwner.bIsDerivedFrom(mecvTop.c2ClassId()
                                                    , tCIDMacroEng::EIntrinsics::Enum))
                        {
                            facCIDMacroEng().ThrowErr
                            (
                                CID_FILE
                                , CID_LINE
                                , kMEngErrs::errcEng_WrongStackTopType
                                , tCIDLib::ESeverities::Failed
                                , tCIDLib::EErrClasses::Internal
                                , TString(L"enumerated")
                                , meOwner.strClassPathForId(mecvTop.c2ClassId())
                            );
                        }

                        if (mecvTop.bIsConst())
                        {
                            facCIDMacroEng().ThrowErr
                            (
                                CID_FILE
                                , CID_LINE
                                , kMEngErrs::errcEng_NotNConstStackItem
                                , tCIDLib::ESeverities::Failed
                                , tCIDLib::EErrClasses::Internal
                                , TString(L"enumerated")
                            );
                        }
                    }

                    TMEngEnumVal& mecvEnum = static_cast<TMEngEnumVal&>(mecvTop);
                    if (tCIDMacroEng::EOpCodes::CondEnumInc == meopCur.eOpCode())
                    {
                        if (mecvEnum.bAtMax())
                        {
                            meOwner.PushBoolean(kCIDLib::False, tCIDMacroEng::EConstTypes::Const);
                        }
                         else
                        {
                            mecvEnum.Increment();
                            meOwner.PushBoolean(kCIDLib::True, tCIDMacroEng::EConstTypes::Const);
                        }
                    }
                     else
                    {
                        mecvEnum.c4Ordinal(0);
                    }
                    break;
                }


                case tCIDMacroEng::EOpCodes::CondJump :
                {
                    //
                    //  The top of stack is a boolean value. We get it's
                    //  value then pop it. Then we either jump or not
                    //  depending on it's value and whether we are a jump
                    //  not jump.
                    //
                    tCIDLib::TBoolean bJump = meOwner.bStackValAt
                    (
                        meOwner.c4StackTop() - 1
                    );
                    meOwner.PopTop();

                    // If we need to jump, the new IP is in an immedate
                    if (bJump)
                    {
                        c4IP = meopCur.c4Immediate();
                        bNoIncIP = kCIDLib::True;
                    }
                    break;
                }

                case tCIDMacroEng::EOpCodes::CondJumpNP :
                {
                    // If we need to jump, the new IP is in an immedate
                    if (meOwner.bStackValAt(meOwner.c4StackTop() - 1))
                    {
                        c4IP = meopCur.c4Immediate();
                        bNoIncIP = kCIDLib::True;
                    }
                    break;
                }

                case tCIDMacroEng::EOpCodes::CurLine :
                {
                    // Just store the line number stored
                    meOwner.c4CurLine(meopCur.c4Immediate());

                    //
                    //  If we have a debugger installed, then we need to
                    //  call it and take action based on what it returns.
                    //
                    if (pmedbgToCall)
                    {
                        tCIDMacroEng::EDbgActs eAct = pmedbgToCall->eAtLine
                        (
                            meciTarget
                            , methiThis
                            , *this
                            , mecvInstance
                            , meopCur.c4Immediate()
                            , c4IP
                        );

                        //
                        //  If he tells us to exit, then throw an exception
                        //  that will dump us back to the top level, who will
                        //  tell the debugger we've exited.
                        //
                        if ((eAct == tCIDMacroEng::EDbgActs::CloseSession)
                        ||  (eAct == tCIDMacroEng::EDbgActs::Exit))
                        {
                            throw TDbgExitException(eAct);
                        }
                    }
                    break;
                }

                case tCIDMacroEng::EOpCodes::EndTry :
                {
                    //
                    //  The top of stack must be a try item, which we want
                    //  to pop. If validating, then check it.
                    //
                    if (meOwner.bValidation())
                    {
                        if (meOwner.mecsiTop().eType() != tCIDMacroEng::EStackItems::Try)
                        {
                            facCIDMacroEng().ThrowErr
                            (
                                CID_FILE
                                , CID_LINE
                                , kMEngErrs::errcEng_ExpectedTry
                                , tCIDLib::ESeverities::Failed
                                , tCIDLib::EErrClasses::Internal
                                , TCardinal(meOwner.c4CurLine())
                                , meciTarget.strClassPath()
                            );
                        }
                    }
                    meOwner.PopTop();
                    break;
                }

                case tCIDMacroEng::EOpCodes::FlipTop :
                {
                    // Just ask the engine to flip the top two stack items
                    meOwner.FlipStackTop();
                    break;
                }

                case tCIDMacroEng::EOpCodes::Jump :
                {
                    // Get the new IP out
                    const tCIDLib::TCard4 c4New = meopCur.c4Immediate();

                    //
                    //  If debugging, make sure that we aren't going to jump
                    //  out of the valid range.
                    //
                    if (c4New >= c4EndIndex)
                    {
                        facCIDMacroEng().ThrowErr
                        (
                            CID_FILE
                            , CID_LINE
                            , kMEngErrs::errcMeth_BadJump
                            , tCIDLib::ESeverities::Failed
                            , tCIDLib::EErrClasses::Internal
                            , TCardinal(c4IP)
                            , meciTarget.strClassPath()
                            , strName()
                        );
                    }

                    // Looks ok, so set it
                    c4IP = c4New;
                    bNoIncIP = kCIDLib::True;
                    break;
                }

                case tCIDMacroEng::EOpCodes::LogicalAnd :
                case tCIDMacroEng::EOpCodes::LogicalOr :
                case tCIDMacroEng::EOpCodes::LogicalXor :
                {
                    // Get the two top items as booleans
                    tCIDLib::TBoolean bFirst = meOwner.bStackValAt
                    (
                        meOwner.c4StackTop() - 1
                    );
                    tCIDLib::TBoolean bSecond = meOwner.bStackValAt
                    (
                        meOwner.c4StackTop() - 2
                    );

                    // Do the indicated operation
                    if (meopCur.eOpCode() == tCIDMacroEng::EOpCodes::LogicalAnd)
                        bFirst &= bSecond;
                    else if (meopCur.eOpCode() == tCIDMacroEng::EOpCodes::LogicalOr)
                        bFirst |= bSecond;
                    else if (meopCur.eOpCode() == tCIDMacroEng::EOpCodes::LogicalXor)
                        bFirst ^= bSecond;

                    // Now pop the top two items off
                    meOwner.MultiPop(2);

                    // And put our new result on the top
                    meOwner.PushBoolean(bFirst, tCIDMacroEng::EConstTypes::Const);
                    break;
                }

                case tCIDMacroEng::EOpCodes::MultiPop :
                {
                    meOwner.MultiPop(meopCur.c4Immediate());
                    break;
                }

                case tCIDMacroEng::EOpCodes::Negate :
                {
                    //
                    //  The top of stack must be a boolean value. We have
                    //  to pop it and push back the negated version We
                    //  cannot modify the value on the stack itself, even if
                    //  a temp, since it may be referred to again later.
                    //
                    TMEngCallStackItem& mecsiTop = meOwner.mecsiTop();
                    if (meOwner.bValidation())
                    {
                        if (mecsiTop.mecvPushed().c2ClassId()
                                            != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Boolean))
                        {
                            facCIDMacroEng().ThrowErr
                            (
                                CID_FILE
                                , CID_LINE
                                , kMEngErrs::errcEng_NotStackItemType
                                , tCIDLib::ESeverities::Failed
                                , tCIDLib::EErrClasses::Internal
                                , meOwner.strClassPathForId(tCIDMacroEng::EIntrinsics::Boolean)
                            );
                        }
                    }

                    TMEngBooleanVal& mecvTop = static_cast<TMEngBooleanVal&>
                    (
                        mecsiTop.mecvPushed()
                    );

                    const tCIDMacroEng::EConstTypes eConst = mecvTop.eConst();
                    const tCIDLib::TBoolean bSave = !mecvTop.bValue();
                    meOwner.PopTop();
                    meOwner.PushBoolean(bSave, eConst);
                    break;
                }

                case tCIDMacroEng::EOpCodes::NoOp :
                    // Do nothing
                    break;

                case tCIDMacroEng::EOpCodes::NotCondJump :
                {
                    //
                    //  The top of stack is a boolean value. We get it's
                    //  value then pop it. Then we either jump or not
                    //  depending on it's value and whether we are a jump
                    //  not jump.
                    //
                    tCIDLib::TBoolean bJump = meOwner.bStackValAt
                    (
                        meOwner.c4StackTop() - 1
                    );
                    meOwner.PopTop();

                    // If we need to jump, the new IP is in an immedate
                    if (!bJump)
                    {
                        c4IP = meopCur.c4Immediate();
                        bNoIncIP = kCIDLib::True;
                    }
                    break;
                }


                case tCIDMacroEng::EOpCodes::NotCondJumpNP :
                {
                    if (!meOwner.bStackValAt(meOwner.c4StackTop() - 1))
                    {
                        c4IP = meopCur.c4Immediate();
                        bNoIncIP = kCIDLib::True;
                    }
                    break;
                }

                case tCIDMacroEng::EOpCodes::PopTop :
                {
                    // Just pop the stack
                    meOwner.PopTop();
                    break;
                }

                case tCIDMacroEng::EOpCodes::PopToReturn :
                {
                    //
                    //  Get the value on the top of the stack, and copy it to
                    //  the return value object. If debugging, make sure that
                    //  the types are the same and that the type is copyable.
                    //
                    TMEngClassVal& mecvTop = meOwner.mecvStackAtTop();
                    TMEngClassVal& mecvRet = meOwner.mecvStackAt
                    (
                        c4BasePointer - (methiThis.c4ParmCount() + 2)
                    );
                    mecvRet.CopyFrom(mecvTop, meOwner);
                    meOwner.PopTop();
                    break;
                }

                case tCIDMacroEng::EOpCodes::PushCurLine :
                {
                    meOwner.PushCard4(meOwner.c4CurLine(), tCIDMacroEng::EConstTypes::Const);
                    break;
                }

                case tCIDMacroEng::EOpCodes::PushEnum :
                {
                    meOwner.PushEnum(meopCur[0], meopCur[1], tCIDMacroEng::EConstTypes::Const);
                    break;
                }

                case tCIDMacroEng::EOpCodes::PushException :
                {
                    meOwner.PushException();
                    break;
                }

                case tCIDMacroEng::EOpCodes::PushImBoolean :
                {
                    meOwner.PushBoolean(meopCur.bImmediate(), tCIDMacroEng::EConstTypes::Const);
                    break;
                }

                case tCIDMacroEng::EOpCodes::PushImCard1 :
                {
                    meOwner.PushCard1(meopCur.c1Immediate(), tCIDMacroEng::EConstTypes::Const);
                    break;
                }

                case tCIDMacroEng::EOpCodes::PushImCard2 :
                {
                    meOwner.PushCard2(meopCur.c2Immediate(), tCIDMacroEng::EConstTypes::Const);
                    break;
                }

                case tCIDMacroEng::EOpCodes::PushImCard4 :
                {
                    meOwner.PushCard4(meopCur.c4Immediate(), tCIDMacroEng::EConstTypes::Const);
                    break;
                }

                case tCIDMacroEng::EOpCodes::PushImCard8 :
                {
                    meOwner.PushCard8(meopCur.c8Immediate(), tCIDMacroEng::EConstTypes::Const);
                    break;
                }

                case tCIDMacroEng::EOpCodes::PushImChar :
                {
                    meOwner.PushChar(meopCur.chImmediate(), tCIDMacroEng::EConstTypes::Const);
                    break;
                }

                case tCIDMacroEng::EOpCodes::PushImFloat4 :
                {
                    meOwner.PushFloat4(meopCur.f4Immediate(), tCIDMacroEng::EConstTypes::Const);
                    break;
                }

                case tCIDMacroEng::EOpCodes::PushImFloat8 :
                {
                    meOwner.PushFloat8(meopCur.f8Immediate(), tCIDMacroEng::EConstTypes::Const);
                    break;
                }

                case tCIDMacroEng::EOpCodes::PushImInt1 :
                {
                    meOwner.PushInt1(meopCur.i1Immediate(), tCIDMacroEng::EConstTypes::Const);
                    break;
                }

                case tCIDMacroEng::EOpCodes::PushImInt2 :
                {
                    meOwner.PushInt2(meopCur.i2Immediate(), tCIDMacroEng::EConstTypes::Const);
                    break;
                }

                case tCIDMacroEng::EOpCodes::PushImInt4 :
                {
                    meOwner.PushInt4(meopCur.i4Immediate(), tCIDMacroEng::EConstTypes::Const);
                    break;
                }

                case tCIDMacroEng::EOpCodes::PushLocal :
                {
                    //
                    //  The first index is the index of the local. In this
                    //  case, we are repushing a value already on the stack.
                    //  The offset of the local relative to the base pointer
                    //  is in the immediate field.
                    //
                    meOwner.RepushAt(c4BasePointer + meopCur[0]);
                    break;
                }

                case tCIDMacroEng::EOpCodes::PushMember :
                {
                    // The first index is the index of the member
                    meOwner.PushValue
                    (
                        &mecvInstance.mecvFind(meopCur[0], meOwner)
                        , tCIDMacroEng::EStackItems::Member
                    );
                    break;
                }

                case tCIDMacroEng::EOpCodes::PushParm :
                {
                    //
                    //  The first index is the id of the parameter. In this
                    //  case, we are just repushing a value already on the
                    //  stack as a parameter. Subtracting the parm count plus
                    //  one get's us to the first parm. Then we add the parm
                    //  id minus one (they are 1 based) to get to the actual
                    //  value.
                    //
                    meOwner.RepushAt
                    (
                        (c4BasePointer - (methiThis.c4ParmCount() + 1))
                        + meopCur[0]
                    );
                    break;
                }

                case tCIDMacroEng::EOpCodes::PushStrPoolItem :
                {
                    // Just push the pool item
                    meOwner.PushValue
                    (
                        &mecvFindPoolItem(meopCur[0])
                        , tCIDMacroEng::EStackItems::StringPool
                    );
                    break;
                }

                case tCIDMacroEng::EOpCodes::PushTempConst :
                {
                    //
                    //  The c2Immediate field holds the id of the type to push.
                    //
                    meOwner.PushPoolValue(meopCur.c2Immediate(), tCIDMacroEng::EConstTypes::Const);
                    break;
                }

                case tCIDMacroEng::EOpCodes::PushTempVar :
                {
                    //
                    //  The c2Immediate field holds the id of the type to push.
                    //  The context object has a method to push a value of a
                    //  give type id onto the stack from the temp pool.
                    //
                    meOwner.PushPoolValue
                    (
                        meopCur.c2Immediate()
                        , tCIDMacroEng::EConstTypes::NonConst
                    );
                    break;
                }

                case tCIDMacroEng::EOpCodes::PushThis :
                {
                    meOwner.PushValue(&mecvInstance, tCIDMacroEng::EStackItems::This);
                    break;
                }

                case tCIDMacroEng::EOpCodes::Return :
                {
                    //
                    //  There can be try blocks still on the stack at this
                    //  point, so we have to remove them. Any return value has
                    //  been extracted at this point, so there should only be
                    //  tries above the current base pointer.
                    //
                    tCIDLib::TCard4 c4New;
                    while (kCIDLib::True)
                    {
                        const tCIDLib::TCard4 c4PopTo = meOwner.c4FindNextTry(c4New);
                        if ((c4PopTo == kCIDLib::c4MaxCard)
                        ||  (c4PopTo < c4BasePointer))
                        {
                            break;
                        }

                        // We found one in our method scope, so pop back to there
                        meOwner.PopBackTo(c4PopTo);
                    }

                    // Break out of this method
                    bDone = kCIDLib::True;
                    break;
                }

                case tCIDMacroEng::EOpCodes::TableJump :
                {
                    //
                    //  This is an indirect table jump, probably for a switch
                    //  statement. The first index indicates index of the
                    //  jump table that we are to use.
                    //
                    TMEngJumpTable& jtblToUse = jtblById(meopCur[0]);

                    //
                    //  The value to switch on must be no the top of the stack,
                    //  so get it and ask the table which one matches it.
                    //
                    TMEngClassVal& mecvCase = meOwner.mecvStackAtTop();

                    //
                    //  Look this guy up. Note that if it doesn't match,
                    //  we will use the deafult.
                    //
                    tCIDLib::TCard4 c4New;
                    if (!jtblToUse.bFindMatch(mecvCase, c4New))
                        c4New = jtblToUse.c4DefCaseIP();

                    // We can trash the stack top now
                    meOwner.PopTop();

                    // And set the IP to the IP of the target case block
                    bNoIncIP = kCIDLib::True;
                    c4IP = c4New;
                    break;
                }

                case tCIDMacroEng::EOpCodes::Throw :
                {
                    //
                    //  This can be either a throw, or a rethrow, according
                    //  to the immediate boolean value. If a throw, then
                    //  get the exception info set.
                    //
                    //  There must be an enumerated type on the stack, which
                    //  is the error to throw. We will use this to set up an
                    //  exception instance that we keep around for this
                    //  purpose.
                    //
                    if (!meopCur.bImmediate())
                    {
                        TMEngEnumVal& mecvErr = meOwner.mecvStackAtTopAs<TMEngEnumVal>();
                        TMEngEnumInfo& meciErr = static_cast<TMEngEnumInfo&>
                        (
                            meOwner.meciFind(mecvErr.c2ClassId())
                        );

                        // And now we can set the exception info
                        meOwner.SetException
                        (
                            mecvErr.c2ClassId()
                            , meciTarget.strClassPath()
                            , mecvErr.c4Ordinal()
                            , meciErr.strFullName(mecvErr)
                            , meciErr.strTextValue(mecvErr)
                            , meOwner.c4CurLine()
                        );
                    }

                    //
                    //  Look for the next try block up the call chain. If it
                    //  isn't within our call frame, then we throw a special
                    //  C++ exception to cause the propogation. Else, we will
                    //  unwind the stack back to (and including) the try
                    //  block, and jump to the offset that it indicates it's
                    //  catch block is at.
                    //
                    tCIDLib::TCard4 c4New;
                    const tCIDLib::TCard4 c4Tmp = meOwner.c4FindNextTry(c4New);
                    if ((c4Tmp < c4BasePointer) || (c4Tmp == kCIDLib::c4MaxCard))
                    {
                        throw TExceptException();
                    }
                     else
                    {
                        // Pop back to the indicated position
                        meOwner.PopBackTo(c4Tmp);

                        // And set the IP to the catch IP
                        bNoIncIP = kCIDLib::True;
                        c4IP = c4New;
                    }
                    break;
                }

                case tCIDMacroEng::EOpCodes::ThrowFmt :
                {
                    //
                    //  This is a special form of throw that is used to
                    //  pass in values to be used to replace tokens in the
                    //  error text. c4Immediate indicates how many of them
                    //  there are, pushed after the error enum. So start
                    //  off by getting the error enum out, and get the
                    //  text that we will format tokens into.
                    //
                    TMEngEnumVal& mecvErr = meOwner.mecvStackAtAs<TMEngEnumVal>
                    (
                        meOwner.c4StackTop() - (1 + meopCur.c4Immediate())
                    );
                    TMEngEnumInfo& meciErr = static_cast<TMEngEnumInfo&>
                    (
                        meOwner.meciFind(mecvErr.c2ClassId())
                    );

                    TString strText(meciErr.strTextValue(mecvErr), 128);

                    //
                    //  Now, according to how many tokens indicated in the
                    //  immediate value, work down and format each one and
                    //  replace that tokens. This requires a bit of code to
                    //  do, so we call a helper method to do it.
                    //
                    FormatErrTokens
                    (
                        meOwner
                        , meopCur.c4Immediate()
                        , strText
                    );

                    // And now we can set the exception info
                    meOwner.SetException
                    (
                        mecvErr.c2ClassId()
                        , meciTarget.strClassPath()
                        , mecvErr.c4Ordinal()
                        , meciErr.strFullName(mecvErr)
                        , strText
                        , meOwner.c4CurLine()
                    );

                    //
                    //  Look for the next try block up the call chain. If it
                    //  isn't within our call frame, then we throw a special
                    //  C++ exception to cause the propogation. Else, we will
                    //  unwind the stack back to (and including) the try
                    //  block, and jump to the offset that it indicates it's
                    //  catch block is at.
                    //
                    tCIDLib::TCard4 c4New;
                    const tCIDLib::TCard4 c4Tmp = meOwner.c4FindNextTry(c4New);
                    if ((c4Tmp < c4BasePointer) || (c4Tmp == kCIDLib::c4MaxCard))
                    {
                        throw TExceptException();
                    }
                     else
                    {
                        // Pop back to the indicated position
                        meOwner.PopBackTo(c4Tmp);

                        // And set the IP to the catch IP
                        bNoIncIP = kCIDLib::True;
                        c4IP = c4New;
                    }
                    break;
                }

                case tCIDMacroEng::EOpCodes::Try :
                {
                    //
                    //  We need to push a try block on the stack. The opcode
                    //  has the IP of the start of the catch block in the
                    //  c4Immediate field, which we need to push along with it.
                    //
                    meOwner.PushTry(meopCur.c4Immediate());
                    break;
                }

                case tCIDMacroEng::EOpCodes::TypeCast :
                {
                    //
                    //  The first index is the type to cast to. The value to
                    //  cast is on the top of stack, and must be a numeric or
                    //  enumerated type, as must be the type to cast to.
                    //
                    //  So we need to get the value on the top of the stack,
                    //  convert it to the cast type, pop the old value off,
                    //  and replace it with a new value from the temp pool.
                    //
                    TMEngClassVal& mecvToCast = meOwner.mecvStackAtTop();

                    // Push a temp pool value of the target type
                    meOwner.PushPoolValue
                    (
                        meopCur.c2Immediate()
                        , tCIDMacroEng::EConstTypes::Const
                    );
                    TMEngClassVal& mecvTmp = meOwner.mecvStackAtTop();

                    // And ask the temp object to cast from the stack top value
                    TMEngClassInfo& meciTarget = meOwner.meciFind(meopCur.c2Immediate());
                    const tCIDMacroEng::ECastRes eRes = meciTarget.eCastFrom
                    (
                        meOwner
                        , mecvToCast
                        , mecvTmp
                    );

                    if (eRes == tCIDMacroEng::ECastRes::Incompatible)
                    {
                        // This should have been checked up front
                        facCIDMacroEng().ThrowErr
                        (
                            CID_FILE
                            , CID_LINE
                            , kMEngErrs::errcPrs_CannotCast
                            , tCIDLib::ESeverities::Failed
                            , tCIDLib::EErrClasses::Internal
                            , meOwner.strClassPathForId(mecvToCast.c2ClassId())
                            , meciTarget.strClassPath()
                        );
                    }

                    // Flip and pop the original off
                    meOwner.FlipStackTop();
                    meOwner.PopTop();
                    break;
                }

                case tCIDMacroEng::EOpCodes::None :
                {
                    //
                    //  We shouldn't be see this one in the opcode stream
                    //  because it's just a place holder for internal use.
                    //
                    facCIDMacroEng().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kMEngErrs::errcMeth_NoneOpCode
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Format
                        , TCardinal(meOwner.c4CurLine())
                        , meciTarget.strClassPath()
                        , strName()
                    );
                    break;
                }

                default :
                {
                    facCIDMacroEng().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kMEngErrs::errcMeth_UnknownOpCode
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Format
                        , TInteger(tCIDLib::i4EnumOrd(meopCur.eOpCode()))
                        , TCardinal(meOwner.c4CurLine())
                        , meciTarget.strClassPath()
                        , strName()
                    );
                    break;
                }
            }

            // Move forward to the next opcode
            if (bNoIncIP)
                bNoIncIP = kCIDLib::False;
            else
                c4IP++;

            // If at the end, we are done
            if (c4IP == c4EndIndex)
                break;
        }
    }

    catch(TError& errToCatch)
    {
        //
        //  If we have a debug callback, tell him locals are being removed.
        //  We don't even check if we really have any, since it just makes
        //  him clear his window.
        //
        if (pmedbgToCall)
            pmedbgToCall->LocalsChange(kCIDLib::False);

        // Pop the stack back to the base pointer and tell the debugger
        meOwner.PopBackTo(c4BasePointer);
        if (pmedbgToCall)
            pmedbgToCall->CallStackChange();

        if (!bThrowReported)
        {
            facCIDMacroEng().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcEng_ExceptionIn
                , tCIDLib::ESeverities::Status
                , tCIDLib::EErrClasses::AppStatus
                , meciTarget.strClassPath()
                , strName()
            );

            // If the error itself hasn't been logged, then do it
            if (facCIDMacroEng().bShouldLog(errToCatch))
                TModule::LogEventObj(errToCatch);

            // Let the engine know about it, if not already, then rethrow
            if (errToCatch.bReported())
                meOwner.Exception(errToCatch);
        }

        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    catch(const TDbgExitException&)
    {
        //
        //  If we have a debug callback, tell him locals are being removed.
        //  We don't even check if we really have any, since it just makes
        //  him clear his window.
        //
        if (pmedbgToCall)
            pmedbgToCall->LocalsChange(kCIDLib::False);

        // Pop the stack back to the base pointer and tell the debugger
        meOwner.PopBackTo(c4BasePointer);
        if (pmedbgToCall)
            pmedbgToCall->CallStackChange();

        throw;
    }

    catch(const TExceptException&)
    {
        //
        //  If we have a debug callback, tell him locals are being removed.
        //  We don't even check if we really have any, since it just makes
        //  him clear his window.
        //
        if (pmedbgToCall)
            pmedbgToCall->LocalsChange(kCIDLib::False);

        // Pop the stack back to the base pointer and tell the debugger
        meOwner.PopBackTo(c4BasePointer);
        if (pmedbgToCall)
            pmedbgToCall->CallStackChange();
        throw;
    }

    catch(...)
    {
        //
        //  If we have a debug callback, tell him locals are being removed.
        //  We don't even check if we really have any, since it just makes
        //  him clear his window.
        //
        if (pmedbgToCall)
            pmedbgToCall->LocalsChange(kCIDLib::False);

        // Pop the stack back to the base pointer and tell the debugger
        meOwner.PopBackTo(c4BasePointer);
        if (pmedbgToCall)
            pmedbgToCall->CallStackChange();

        if (!bThrowReported)
        {
            facCIDMacroEng().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcEng_UnknownExceptIn
                , tCIDLib::ESeverities::Status
                , tCIDLib::EErrClasses::AppStatus
                , meciTarget.strClassPath()
                , strName()
            );

            // Tell the engine about it and then rethrow
            meOwner.UnknownException();
        }
        throw;
    }

    //
    //  If we have a debug callback, tell him locals are being removed.
    //  We don't even check if we really have any, since it just makes
    //  him clear his window.
    //
    if (pmedbgToCall)
        pmedbgToCall->LocalsChange(kCIDLib::False);

    // Pop the stack back to the base pointer and tell the debugger
    meOwner.PopBackTo(c4BasePointer);
    if (pmedbgToCall)
        pmedbgToCall->CallStackChange();
}


// ---------------------------------------------------------------------------
//  TMEngOpMethodImpl: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngOpMethodImpl::bMoreLines(const tCIDLib::TCard4 c4LastIP) const
{
    const tCIDLib::TCard4   c4Count = m_colOpCodes.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = c4LastIP + 1; c4Index < c4Count; c4Index++)
    {
        const TMEngOpCode& meopCur = m_colOpCodes[c4Index];
        if (meopCur.eOpCode() == tCIDMacroEng::EOpCodes::CurLine)
        {
            //
            //  Check to see if the next opcode is the last one and a no-op.
            //  If so, then this line is the one on the EndMethod, which is
            //  really past the end of the method from a call stack point of
            //  view, so we'll return false if so.
            //
            if ((c4Index + 2 == c4Count)
            &&  (m_colOpCodes[c4Index + 1].eOpCode() == tCIDMacroEng::EOpCodes::NoOp))
            {
                return kCIDLib::False;
            }
            return kCIDLib::True;
        }
    }
    return kCIDLib::False;
}


tCIDLib::TCard2 TMEngOpMethodImpl::c2AddJumpTable()
{
    if (!m_pjtblSwitches)
        m_pjtblSwitches = new TMEngJumpTableTable(tCIDLib::EAdoptOpts::Adopt);

    const tCIDLib::TCard4 c4Ret = m_pjtblSwitches->c4ElemCount();
    m_pjtblSwitches->Add(new TMEngJumpTable);
    return tCIDLib::TCard2(c4Ret);
}


tCIDLib::TCard4 TMEngOpMethodImpl::c4AddOpCode(const TMEngOpCode& meopToAdd)
{
    //
    //  Add this opcode and return the index of the opcode, which is the
    //  current (pre-addition) count.
    //
    tCIDLib::TCard4 c4Ret = m_colOpCodes.c4ElemCount();
    m_colOpCodes.objAdd(meopToAdd);
    return c4Ret;
}


tCIDLib::TCard4 TMEngOpMethodImpl::c4CurOffset() const
{
    return m_colOpCodes.c4ElemCount();
}


tCIDLib::TCard4 TMEngOpMethodImpl::c4FirstLineNum() const
{
    const tCIDLib::TCard4 c4Count = m_colOpCodes.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TMEngOpCode& meopCur = m_colOpCodes[c4Index];
        if (meopCur.eOpCode() == tCIDMacroEng::EOpCodes::CurLine)
            return meopCur.c4Immediate();
    }

    // Just return 1, since we didn't find any current line opcodes
    return 1;
}


tCIDLib::TVoid
TMEngOpMethodImpl::DumpOpCodes(         TTextOutStream&     strmTarget
                                , const TCIDMacroEngine&    meOwner) const
{
    const TTextOutStream::Width widNums(4);
    const TTextOutStream::Width widZero(0);

    TStreamJanitor janStream(&strmTarget);
    const tCIDLib::TCard4 c4Count = m_colOpCodes.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        strmTarget << widNums << c4Index << widZero << L"- ";
        m_colOpCodes[c4Index].Format(strmTarget, meOwner);
        strmTarget << kCIDLib::NewLn;
    }
}


TMEngJumpTable& TMEngOpMethodImpl::jtblById(const tCIDLib::TCard2 c2Id)
{
    // Make sure this index is valid
    if (!m_pjtblSwitches || (c2Id >= m_pjtblSwitches->c4ElemCount()))
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcMeth_BadJumpTableId
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c2Id)
            , strName()
        );
    }
    return *m_pjtblSwitches->pobjAt(c2Id);
}


const TMEngOpCode& TMEngOpMethodImpl::meopAt(const tCIDLib::TCard4 c4At) const
{
    if (c4At >= m_colOpCodes.c4ElemCount())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcMeth_BadIP
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , strName()
            , TCardinal(c4At)
        );
    }
    return m_colOpCodes[c4At];

}

TMEngOpCode& TMEngOpMethodImpl::meopAt(const tCIDLib::TCard4 c4At)
{
    if (c4At >= m_colOpCodes.c4ElemCount())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcMeth_BadIP
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , strName()
            , TCardinal(c4At)
        );
    }
    return m_colOpCodes[c4At];
}


const TMEngOpCode& TMEngOpMethodImpl::meopLast() const
{
    const tCIDLib::TCard4 c4Count = m_colOpCodes.c4ElemCount();
    if (!c4Count)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcMeth_BadIP
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , strName()
            , TInteger(0)
        );
    }
    return m_colOpCodes[c4Count - 1];
}


TMEngOpCode& TMEngOpMethodImpl::meopLast()
{
    const tCIDLib::TCard4 c4Count = m_colOpCodes.c4ElemCount();
    if (!c4Count)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcMeth_BadIP
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , strName()
            , TInteger(0)
        );
    }
    return m_colOpCodes[c4Count - 1];
}


// ---------------------------------------------------------------------------
//  TMEngOpMethodImpl: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngOpMethodImpl::FormatErrTokens(         TCIDMacroEngine&    meOwner
                                    , const tCIDLib::TCard4     c4Tokens
                                    ,       TString&            strToFill)
{
    // Create a macro level string out stream
    TMEngClassInfo& meciStrm = meOwner.meciFind
    (
        tCIDMacroEng::EIntrinsics::StringOutStream
    );
    TMEngTextOutStreamVal* pmecvStrm = static_cast<TMEngTextOutStreamVal*>
    (
        meciStrm.pmecvMakeStorage
        (
            L"TempStream"
            , meOwner
            , tCIDMacroEng::EConstTypes::NonConst
        )
    );
    TJanitor<TMEngTextOutStreamVal> janStream(pmecvStrm);

    //
    //  Give it a stream, since we won't be constructing it at the macro
    //  language level, which normally is how it gets one.
    //
    TTextStringOutStream* pstrmTarget = new TTextStringOutStream(1024UL);
    pmecvStrm->SetStream(pstrmTarget, tCIDLib::EAdoptOpts::Adopt);

    tCIDLib::TCh chToken(kCIDLib::chDigit1);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Tokens; c4Index++)
    {
        //
        //  Get the current formattable object. One gets us down to the actual
        //  top item, then add the current index to get to the current one.
        //
        TMEngFormattableVal& mecvCur = meOwner.mecvStackAtAs<TMEngFormattableVal>
        (
            (meOwner.c4StackTop() - c4Tokens) + c4Index
        );
        TMEngClassInfo& meciTarget = meOwner.meciFind(mecvCur.c2ClassId());

        //
        //  Generate a macro level call to format this guy to the stream. Tell
        //  the call stack that the stream is a 'repush', though its not, so
        //  that it won't try to delete it on us.
        //
        meOwner.PushPoolValue(tCIDMacroEng::EIntrinsics::Void, tCIDMacroEng::EConstTypes::Const);
        meOwner.PushValue(pmecvStrm, tCIDMacroEng::EStackItems::Parm, kCIDLib::True);
        meOwner.meciPushMethodCall
        (
            meciTarget.c2Id()
            , TMEngFormattableInfo::c2FormatToId()
        );
        meciTarget.Invoke
        (
            meOwner
            , mecvCur
            , TMEngFormattableInfo::c2FormatToId()
            , tCIDMacroEng::EDispatch::Poly
        );

        //
        //  We cheated this one, so we have to pop the method item as well
        //  the parm and return!
        //
        meOwner.MultiPop(3);

        // And now replace the current token with this result. Flush first!
        pmecvStrm->Flush();
        strToFill.eReplaceToken(pstrmTarget->strData(), chToken);

        // And reset it for the next round
        pstrmTarget->Reset();

        // Move up to the next token digit
        chToken++;
    }
}

