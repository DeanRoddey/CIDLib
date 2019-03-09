//
// FILE NAME: CIDMacroEng_Engine.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/14/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements TMacroEngEngine class. This class represents an
//  instance of the expression engine.
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
RTTIDecls(TCIDMacroEngine,TObject)


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDMacroEng_Engine
{
    // -----------------------------------------------------------------------
    //  A flag to handle local lazy init
    // -----------------------------------------------------------------------
    tCIDLib::TBoolean   m_bLocalInit;


    // -----------------------------------------------------------------------
    //  Some stats cache items we maintain
    // -----------------------------------------------------------------------
    TStatsCacheItem     m_sciMacroEngCount;
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngCallStackJan
// PREFIX: jan
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngCallStackJan: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngCallStackJan::TMEngCallStackJan(TCIDMacroEngine* const  pmeToRestore) :

    m_c4OrgTop(0)
    , m_pmeToRestore(pmeToRestore)
{
    // Could be null so check before we store
    if (m_pmeToRestore)
        m_c4OrgTop = pmeToRestore->c4StackTop();
}

TMEngCallStackJan::~TMEngCallStackJan()
{
    if (m_pmeToRestore)
    {
        try
        {
            m_pmeToRestore->PopBackTo(m_c4OrgTop);
        }

        catch(TError& errToCatch)
        {
            if (!errToCatch.bLogged())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }
        }
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TCIDMacroEngine
// PREFIX: eeng
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDMacroEngine: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDMacroEngine::TCIDMacroEngine() :

    m_bDebugMode(kCIDLib::False)
    , m_bInIDE(kCIDLib::False)
    , m_bValidation(kCIDLib::False)
    , m_c2ArrayId(kMacroEng::c2BadId)
    , m_c2NextClassId(0)
    , m_c2VectorId(kMacroEng::c2BadId)
    , m_c4CallStackTop(0)
    , m_c4CurLine(0)
    , m_c4NextUniqueId(1)
    , m_colCallStack(tCIDLib::EAdoptOpts::Adopt, 64)
    , m_colClasses
      (
        tCIDLib::EAdoptOpts::NoAdopt
        , 29
        , new TStringKeyOps
        , &TMEngClassInfo::strKey
      )
    , m_colClassesById(tCIDLib::EAdoptOpts::Adopt, 128)
    , m_colTempPool(32)
    , m_eExceptReport(tCIDMacroEng::EExceptReps::NotHandled)
    , m_pcuctxRights(nullptr)
    , m_pmedbgToUse(nullptr)
    , m_pmeehToUse(nullptr)
    , m_pmefrToUse(nullptr)
    , m_pmecvThrown(nullptr)
    , m_pstrmConsole(nullptr)
{
    // Make sure our facility has faulted in
    facCIDMacroEng();

    // Do local lazy init if required
    if (!CIDMacroEng_Engine::m_bLocalInit)
    {
        TBaseLock lockInit;
        if (!CIDMacroEng_Engine::m_bLocalInit)
        {
            TStatsCache::RegisterItem
            (
                kMacroEng::pszStat_MEng_EngInstCount
                , tCIDLib::EStatItemTypes::Counter
                , CIDMacroEng_Engine::m_sciMacroEngCount
            );

            CIDMacroEng_Engine::m_bLocalInit = kCIDLib::True;
        }
    }

    //
    //  Register the small number of instrinc classes that are always
    //  available without being asked for.
    //
    RegisterBuiltInClasses();

    // Increment the count of registered engines
    TStatsCache::c8IncCounter(CIDMacroEng_Engine::m_sciMacroEngCount);
}

TCIDMacroEngine::~TCIDMacroEngine()
{
    // Decrement the count of registered engines
    TStatsCache::c8DecCounter(CIDMacroEng_Engine::m_sciMacroEngCount);

    try
    {
        //
        //  Do some common cleanup that is down each time the entine is
        //  prepped for a new run. Tell it this time to remove all of
        //  them, whereas normally it leaves the built in classes alone.
        //
        Cleanup(kCIDLib::True);

        //
        //  And clean up some other stuff that only get's dropped when we
        //  going away.
        //
        //  Note that we don't own any of the handler callback objects or
        //  the console stream.
        //
        delete m_pmecvThrown;
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
//  TCIDMacroEngine: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDMacroEngine::bAreEquivCols( const   tCIDLib::TCard2     c2ClassId1
                                , const tCIDLib::TCard2     c2ClassId2
                                , const tCIDLib::TBoolean   bSameElemType) const
{
    const TMEngClassInfo& meci1 = meciFind(c2ClassId1);
    const TMEngClassInfo& meci2 = meciFind(c2ClassId2);

    // They have to both be collection classes
    if (!bIsCollectionClass(meci1.c2ParentClassId())
    ||  !bIsCollectionClass(meci2.c2ParentClassId()))
    {
        return kCIDLib::False;
    }

    // And they both must derive from the same base collection class
    if (meci1.c2ParentClassId() != meci2.c2ParentClassId())
        return kCIDLib::False;

    //
    //  If the caller says the element types must be the same, then check that.
    //
    if (bSameElemType)
    {
        return (static_cast<const TMEngColBaseInfo&>(meci1).c2ElemId()
                == static_cast<const TMEngColBaseInfo&>(meci2).c2ElemId());
    }

    //
    //  Else the second element type must derive from the first. This method takes
    //  the base class as the second parm and the one to test as the first.
    //
    return bIsDerivedFrom
    (
        static_cast<const TMEngColBaseInfo&>(meci2).c2ElemId()
        , static_cast<const TMEngColBaseInfo&>(meci1).c2ElemId()
    );
}


// Get set the 'in IDE' flag, which the IDE sets on us. Otherwise it's false always
tCIDLib::TBoolean TCIDMacroEngine::bInIDE() const
{
    return m_bInIDE;
}

tCIDLib::TBoolean TCIDMacroEngine::bInIDE(const tCIDLib::TBoolean bToSet)
{
    m_bInIDE = bToSet;
    return m_bInIDE;
}


// Return the debug mode flag
tCIDLib::TBoolean TCIDMacroEngine::bDebugMode(const tCIDLib::TBoolean bToSet)
{
    m_bDebugMode = bToSet;
    return m_bDebugMode;
}


// Used to format stack dumps
tCIDLib::TBoolean
TCIDMacroEngine::bFormatNextCallFrame(  TTextOutStream&     strmTarget
                                        , tCIDLib::TCard4&  c4PrevInd)
{
    //
    //  If the previous id is max card, then we are to do the first one.
    //  Else, we are working down and need to find the previous one.
    //
    TMEngCallStackItem* pmecsiCur;
    if (c4PrevInd == kCIDLib::c4MaxCard)
    {
        // If we don't have any entries, nothing to do
        if (!m_c4CallStackTop)
        {
            strmTarget << L"<Empty>";
            c4PrevInd = 0;
        }
         else
        {
            //
            //  Start at the top of the call stack and work down to the first
            //  method call opcode, which is what we want to start with.
            //
            c4PrevInd = m_c4CallStackTop;
            do
            {
                c4PrevInd--;
                pmecsiCur = m_colCallStack[c4PrevInd];
                if (pmecsiCur->eType() == tCIDMacroEng::EStackItems::MethodCall)
                    break;
            }   while (c4PrevInd != 0);

            //
            //  If we hit zero, then nothing, else call a private helper to
            //  format this one.
            //
            if (c4PrevInd)
                FormatAFrame(strmTarget, c4PrevInd);
        }
    }
     else
    {
        //
        //  Find the previous item on the stack, starting at one before the
        //  previous item.
        //
        do
        {
            c4PrevInd--;
            pmecsiCur = m_colCallStack[c4PrevInd];
            if (pmecsiCur->eType() == tCIDMacroEng::EStackItems::MethodCall)
                break;
        }   while (c4PrevInd != 0);

        //
        //  If we hit zero, then nothing, else call a private helper to
        //  format this one.
        //
        if (c4PrevInd)
            FormatAFrame(strmTarget, c4PrevInd);
    }
    return (c4PrevInd != 0);
}


tCIDLib::TBoolean
TCIDMacroEngine::bGetLastExceptInfo(TString&            strClass
                                    , TString&          strText
                                    , tCIDLib::TCard4&  c4Line)
{
    if (m_pmecvThrown)
    {
        strClass = m_pmecvThrown->strSrcClassPath();
        strText = m_pmecvThrown->strErrorText();
        c4Line = m_pmecvThrown->c4LineNum();
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean
TCIDMacroEngine::bInvokeDefCtor(        TMEngClassVal&      mecvTarget
                                , const TCIDUserCtx* const  pcuctxRights)
{
    try
    {
        // Set the passed user contact while we are in here
        TGFJanitor<const TCIDUserCtx*> janRights(&m_pcuctxRights, pcuctxRights);

        //
        //  If the temp pool is empty, then this is the first time this
        //  engine instance has been used, so we need to initialize the
        //  pool.
        //
        if (m_colTempPool.bIsEmpty())
            InitTempPool();

        // Look up it's class
        const TMEngClassInfo& meciTarget = meciFind(mecvTarget.c2ClassId());

        //
        //  And ask it for it's default ctor, if any. Note that we look up
        //  the INFO not the IMPL here, since it could be a class that is
        //  just wrapping C++ code and it won't have any CML level impl.
        //
        const TMEngMethodInfo* pmethiDefCtor = meciTarget.pmethiDefCtor();
        if (!pmethiDefCtor)
        {
            facCIDMacroEng().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcClass_NoDefCtor
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , meciTarget.strClassPath()
            );
        }

        //
        //  Push a void return value, the method call, and invoke it. We
        //  indicate a monomorphic invocation here, since it has to be
        //  our guy that we are targeting.
        //
        PushPoolValue(tCIDMacroEng::EIntrinsics::Void, tCIDMacroEng::EConstTypes::Const);
        meciPushMethodCall(meciTarget.c2Id(), pmethiDefCtor->c2Id());
        meciTarget.Invoke
        (
            *this
            , mecvTarget
            , pmethiDefCtor->c2Id()
            , tCIDMacroEng::EDispatch::Mono
        );

        // We have to pop the method call and return off
        MultiPop(2);
    }

    catch(const TDbgExitException& excptForce)
    {
        if (m_pmedbgToUse)
            m_pmedbgToUse->FinishedDbg(excptForce.m_eReason);
        return kCIDLib::False;
    }

    catch(const TExceptException&)
    {
        //
        //  If there is an herror handler, and we are in the mode where we
        //  report macros if they aren'thandled, then report it. If we are
        //  in the other mode, it will already have been reported at the point
        //  of throw.
        //
        if (m_pmeehToUse && (m_eExceptReport == tCIDMacroEng::EExceptReps::NotHandled))
            m_pmeehToUse->MacroException(*m_pmecvThrown, *this);

        if (m_pmedbgToUse)
            m_pmedbgToUse->FinishedDbg(tCIDMacroEng::EDbgActs::Exit);
        return kCIDLib::False;
    }

    catch(...)
    {
        if (m_pmedbgToUse)
            m_pmedbgToUse->FinishedDbg(tCIDMacroEng::EDbgActs::Exit);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


//
//  All the collection classes are runtime classes, so we know them and have
//  their ids stored, so we can provide a quick check.
//
tCIDLib::TBoolean
TCIDMacroEngine::bIsCollectionClass(const tCIDLib::TCard2 c2IdToCheck) const
{
    return ((c2IdToCheck == m_c2ArrayId) || (c2IdToCheck == m_c2VectorId));
}


//
//  Checks to see if one class is derived from aonther, by just walking the
//  parentage chain.
//
tCIDLib::TBoolean
TCIDMacroEngine::bIsDerivedFrom(const   tCIDLib::TCard2 c2IdToTest
                                , const tCIDLib::TCard2 c2BaseClassId) const
{
    tCIDLib::TCard2 c2Cur = c2IdToTest;
    while (kCIDLib::True)
    {
        //
        //  If we've hit the target base class, then it's good. If the test
        //  class is down to the object level, it is not gonna work.
        //
        if ((c2Cur == c2BaseClassId)
        ||  (c2Cur == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Object)))
        {
            break;
        }

        // Get the parent class id of the target class, and go again
        c2Cur = meciFind(c2Cur).c2ParentClassId();
    }
    return (c2Cur == c2BaseClassId);
}

tCIDLib::TBoolean
TCIDMacroEngine::bIsDerivedFrom(const   tCIDLib::TCard2         c2IdToTest
                                , const tCIDMacroEng::EIntrinsics  eBaseType) const
{
    return bIsDerivedFrom(c2IdToTest, tCIDLib::TCard2(eBaseType));
}



//
//  Literals can only be char, bool, string, or numeric. This is a method
//  to keep that check in one place.
//
tCIDLib::TBoolean
TCIDMacroEngine::bIsLiteralClass(const tCIDLib::TCard2 c2IdToCheck) const
{
    // If not an intrinsic, then obviously can't be
    if (c2IdToCheck >= tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Count))
        return kCIDLib::False;

    tCIDLib::TBoolean bRet;
    switch(tCIDMacroEng::EIntrinsics(c2IdToCheck))
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
        case tCIDMacroEng::EIntrinsics::Char :
        case tCIDMacroEng::EIntrinsics::String :
        case tCIDMacroEng::EIntrinsics::Card1 :
        case tCIDMacroEng::EIntrinsics::Card2 :
        case tCIDMacroEng::EIntrinsics::Card4 :
        case tCIDMacroEng::EIntrinsics::Card8 :
        case tCIDMacroEng::EIntrinsics::Float4 :
        case tCIDMacroEng::EIntrinsics::Float8 :
        case tCIDMacroEng::EIntrinsics::Int1 :
        case tCIDMacroEng::EIntrinsics::Int2 :
        case tCIDMacroEng::EIntrinsics::Int4 :
            bRet = kCIDLib::True;
            break;

        default :
            bRet = kCIDLib::False;
            break;
    };
    return bRet;
}


tCIDLib::TBoolean
TCIDMacroEngine::bStackValAt(const  tCIDLib::TCard4     c4Index
                            , const tCIDLib::TBoolean   bCheckType) const
{
    const TMEngClassVal& mecvVal = mecsiAt(c4Index).mecvPushed();
    if (bCheckType)
        CheckItemType(mecvVal.c2ClassId(), tCIDMacroEng::EIntrinsics::Boolean, CID_LINE, c4Index);
    return static_cast<const TMEngBooleanVal&>(mecvVal).bValue();
}


tCIDLib::TBoolean TCIDMacroEngine::bValidation(const tCIDLib::TBoolean bToSet)
{
    m_bValidation = bToSet;
    return m_bValidation;
}


tCIDLib::TCard1
TCIDMacroEngine::c1StackValAt(  const   tCIDLib::TCard4     c4Index
                                , const tCIDLib::TBoolean   bCheckType) const
{
    const TMEngClassVal& mecvVal = mecsiAt(c4Index).mecvPushed();
    if (bCheckType)
        CheckItemType(mecvVal.c2ClassId(), tCIDMacroEng::EIntrinsics::Card1, CID_LINE, c4Index);
    return static_cast<const TMEngCard1Val&>(mecvVal).c1Value();
}


tCIDLib::TCard2
TCIDMacroEngine::c2AddClass(TMEngClassInfo* const pmeciToAdopt)
{
    //
    //  Give it the next id and bump the id counter, then add it to the
    //  'by id' lookup vector, so it's now safely adopted.
    //
    //  NOTE:   These are related actions! The id must be a direct index
    //          into the by id vector!
    //
    facCIDMacroEng().CheckIdOverflow(m_c2NextClassId, L"classes");

    pmeciToAdopt->SetClassId(m_c2NextClassId++);
    m_colClassesById.Add(pmeciToAdopt);

    // Initialize the class now
    pmeciToAdopt->Init(*this);

    // Add if it not already. If it is already, then throw an error
    if (!m_colClasses.bAddIfNew(pmeciToAdopt))
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_DupClassName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , pmeciToAdopt->strClassPath()
        );
    }

    // Return the id we gave to this class
    return pmeciToAdopt->c2Id();
}

tCIDLib::TCard2
TCIDMacroEngine::c2AddClassDefer(TMEngClassInfo* const pmeciToAdopt)
{
    //
    //  Give it the next id and bump the id counter, then add it to the
    //  'by id' lookup vector, so it's now safely adopted.
    //
    //  NOTE:   These are related actions! The id must be a direct index
    //          into the by id vector!
    //
    facCIDMacroEng().CheckIdOverflow(m_c2NextClassId, L"classes");

    pmeciToAdopt->SetClassId(m_c2NextClassId++);
    m_colClassesById.Add(pmeciToAdopt);

    // Add if it not already. If it is already, then throw an error
    if (!m_colClasses.bAddIfNew(pmeciToAdopt))
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_DupClassName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , pmeciToAdopt->strClassPath()
        );
    }

    // Return the id we gave to this class
    return pmeciToAdopt->c2Id();
}


tCIDLib::TCard2
TCIDMacroEngine::c2EnumValAt(const  tCIDLib::TCard4     c4Index
                            , const tCIDLib::TCard2     c2ExpectedId
                            , const tCIDLib::TBoolean   bCheckType) const
{
    const TMEngClassVal& mecvVal = mecsiAt(c4Index).mecvPushed();
    if (bCheckType)
        CheckItemType(mecvVal.c2ClassId(), c2ExpectedId, CID_LINE, c4Index);
    return tCIDLib::TCard2(static_cast<const TMEngEnumVal&>(mecvVal).c4Ordinal());
}


tCIDLib::TCard2
TCIDMacroEngine::c2FindClassId( const   TString&            strClassPath
                                , const tCIDLib::TBoolean   bThrowIfNot) const
{
    const TMEngClassInfo* pmeciRet = m_colClasses.pobjFindByKey
    (
        strClassPath
        , kCIDLib::False
    );

    if (!pmeciRet)
    {
        if (bThrowIfNot)
        {
            facCIDMacroEng().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcEng_ClassNotFound
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , strClassPath
            );
        }
        return kMacroEng::c2BadId;
    }
    return pmeciRet->c2Id();
}


tCIDLib::TCard2
TCIDMacroEngine::c2FindEntryPoint(          TMEngClassInfo&     meciTarget
                                    , const tCIDLib::TBoolean   bThrowIfNot)
{
    // Find a method with the name "Start".
    TMEngMethodInfo* pmethiTarget = meciTarget.pmethiFind(L"Start");

    //
    //  If not there, or it doesn't have the right return type, or isn't
    //  final, it's bad
    //
    if (!pmethiTarget
    ||  (pmethiTarget->c2RetClassId() != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int4))
    ||  (pmethiTarget->eExtend() != tCIDMacroEng::EMethExt::Final))
    {
        if (bThrowIfNot)
        {
            facCIDMacroEng().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcEng_BadEntryPoint
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
            );
        }
        return kCIDLib::c2MaxCard;
    }
    return pmethiTarget->c2Id();
}


tCIDLib::TCard2
TCIDMacroEngine::c2StackValAt(  const   tCIDLib::TCard4     c4Index
                                , const tCIDLib::TBoolean   bCheckType) const
{
    const TMEngClassVal& mecvVal = mecsiAt(c4Index).mecvPushed();
    if (bCheckType)
        CheckItemType(mecvVal.c2ClassId(), tCIDMacroEng::EIntrinsics::Card2, CID_LINE, c4Index);
    return static_cast<const TMEngCard2Val&>(mecvVal).c2Value();
}


tCIDLib::TCard4 TCIDMacroEngine::c4ClassCount() const
{
    return m_colClasses.c4ElemCount();
}


//
//  Given a method info object, this method will create a parameter list
//  object with the appropriate types of objects to pass in to that method.
//  Generally this is used by external code that needs to call the Start()
//  method and get information into the parameters in some more complex way
//  that just passing a command line.
//
//  The passed parm list must be adopting!
//
tCIDLib::TCard4
TCIDMacroEngine::c4CreateParmList(  const   TMEngMethodInfo&    methiTarget
                                    ,       TParmList&          colParms)
{
    // Make sure that the list is adopting
    if (colParms.eAdopt() != tCIDLib::EAdoptOpts::Adopt)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcCol_MustBeAdopting
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    // Loop through and try to create the value objects
    colParms.RemoveAll();
    const tCIDLib::TCard4 c4ParmCount = methiTarget.c4ParmCount();
    TString strName;
    for (tCIDLib::TCard2 c2Id = 0; c2Id < c4ParmCount; c2Id++)
    {
        const TMEngParmInfo& mepiTar = methiTarget.mepiFind(c2Id);

        // The direction indicates whether it's const or not
        const tCIDMacroEng::EConstTypes eConst
        (
            (mepiTar.eDir() == tCIDMacroEng::EParmDirs::In)
            ? tCIDMacroEng::EConstTypes::Const : tCIDMacroEng::EConstTypes::NonConst
        );

        // The name we'll give it
        strName = L"Parm";
        strName.AppendFormatted(tCIDLib::TCard4(c2Id + 1));

        // Create a new value object of the target type
        TMEngClassInfo& meciParm = meciFind(mepiTar.c2ClassId());
        TMEngClassVal* pmecvNew = meciParm.pmecvMakeStorage
        (
            strName, *this, eConst
        );
        colParms.Add(pmecvNew);
    }
    return c4ParmCount;
}


tCIDLib::TCard4
TCIDMacroEngine::c4FindNextTry(tCIDLib::TCard4& c4CatchIP) const
{
    //
    //  Start at the stack top and search backwards for the next exception
    //  item on the stack.
    //
    tCIDLib::TCard4 c4Index = m_c4CallStackTop;
    while (c4Index)
    {
        c4Index--;
        const TMEngCallStackItem* pmecsiCur = m_colCallStack[c4Index];
        if (pmecsiCur->eType() == tCIDMacroEng::EStackItems::Try)
        {
            c4CatchIP = pmecsiCur->c4Value();
            break;
        }
    }

    //
    //  If we hit the bottom, there isn't one. Note that it could never at
    //  the 0th index, because there has to at least be the initial call
    //  frame.
    //
    if (!c4Index)
    {
        c4CatchIP = kCIDLib::c4MaxCard;
        return kCIDLib::c4MaxCard;
    }
    return c4Index;
}


tCIDLib::TCard4
TCIDMacroEngine::c4FirstParmInd(const TMEngMethodInfo& methiCalled)
{
    const tCIDLib::TCard4 c4Ofs = methiCalled.c4ParmCount() + 1;
    #if CID_DEBUG_ON
    if (c4Ofs > m_c4CallStackTop)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_CallStackUnderflow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Underflow
        );
    }
    #endif
    return m_c4CallStackTop - c4Ofs;
}


// Return the next available unique id and increment
tCIDLib::TCard4 TCIDMacroEngine::c4NextUniqueId()
{
    return m_c4NextUniqueId++;
}


//
//  Retrieve values on the stack at a given index by type, to save a lot of
//  grunt work all over the place.
//
tCIDLib::TCard4
TCIDMacroEngine::c4StackValAt(  const   tCIDLib::TCard4     c4Index
                                , const tCIDLib::TBoolean   bCheckType) const
{
    const TMEngClassVal& mecvVal = mecsiAt(c4Index).mecvPushed();
    if (bCheckType)
        CheckItemType(mecvVal.c2ClassId(), tCIDMacroEng::EIntrinsics::Card4, CID_LINE, c4Index);
    return static_cast<const TMEngCard4Val&>(mecvVal).c4Value();
}


tCIDLib::TCard8
TCIDMacroEngine::c8StackValAt(  const   tCIDLib::TCard4     c4Index
                                , const tCIDLib::TBoolean   bCheckType) const
{
    const TMEngClassVal& mecvVal = mecsiAt(c4Index).mecvPushed();
    if (bCheckType)
        CheckItemType(mecvVal.c2ClassId(), tCIDMacroEng::EIntrinsics::Card8, CID_LINE, c4Index);
    return static_cast<const TMEngCard8Val&>(mecvVal).c8Value();
}


tCIDLib::TCh
TCIDMacroEngine::chStackValAt(  const   tCIDLib::TCard4     c4Index
                                , const tCIDLib::TBoolean   bCheckType) const
{
    const TMEngClassVal& mecvVal = mecsiAt(c4Index).mecvPushed();
    if (bCheckType)
        CheckItemType(mecvVal.c2ClassId(), tCIDMacroEng::EIntrinsics::Char, CID_LINE, c4Index);
    return static_cast<const TMEngCharVal&>(mecvVal).chValue();
}


const TVector<TString>&
TCIDMacroEngine::colStackValAt( const   tCIDLib::TCard4     c4Index
                                , const tCIDLib::TBoolean   bCheckType) const
{
    const TMEngClassVal& mecvVal = mecsiAt(c4Index).mecvPushed();
    if (bCheckType)
        CheckItemType(mecvVal.c2ClassId(), tCIDMacroEng::EIntrinsics::StringList, CID_LINE, c4Index);
    return static_cast<const TMEngStrListVal&>(mecvVal).colValue();
}


//
//  Provide access to any user rights context that the user code may have set
//  on us. We don't care what it is, it's purely a passthrough for user code.
//  There is another version that returns a pointer. This is for those clients
//  that know they set one, and therefore want an exception if one is not set.
//
const TCIDUserCtx& TCIDMacroEngine::cuctxRights() const
{
    if (!m_pcuctxRights)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_NoUserContext
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }
    return *m_pcuctxRights;
}



//
//  Finds the first CML level call on the stack (we might have called down
//  into C++ code.) If found, it gets the info on the caller and calls back
//  the debugger pluggin with that information.
//
tCIDLib::TVoid TCIDMacroEngine::CheckIDEReq()
{
    // If we have no debug interface installed, nothing to do
    if (!m_pmedbgToUse)
        return;

    //
    //  Start at the top of the stack and work backwards till we see a
    //  call from a macro level class.
    //
    tCIDLib::TCard4 c4Index = m_c4CallStackTop;
    if (!c4Index)
        return;

    TMEngCallStackItem* pmecsiCur;
    do
    {
        c4Index--;
        pmecsiCur = m_colCallStack[c4Index];
        if (pmecsiCur->eType() == tCIDMacroEng::EStackItems::MethodCall)
        {
            if (pmecsiCur->bHasCallerInfo())
                break;
        }
    }   while (c4Index != 0);

    if (c4Index == 0)
        return;

    // Get out the info we need
    const TMEngClassInfo*       pmeciCaller;
    const TMEngOpMethodImpl*    pmethCaller;
    const TMEngMethodInfo*      pmethiCaller;
    TMEngClassVal*              pmecvCaller;
    tCIDLib::TCard4             c4CallIP;

    tCIDLib::TCard4 c4CallLine = pmecsiCur->c4QueryCallerInfo
    (
        pmeciCaller
        , pmethiCaller
        , pmethCaller
        , pmecvCaller
        , c4CallIP
    );

    tCIDMacroEng::EDbgActs eAct = m_pmedbgToUse->eAtLine
    (
        *pmeciCaller
        , *pmethiCaller
        , *pmethCaller
        , *pmecvCaller
        , c4CallLine
        , c4CallIP
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


tCIDLib::TVoid
TCIDMacroEngine::CheckSameClasses(  const   TMEngClassVal&   mecv1
                                    , const TMEngClassVal&   mecv2) const
{
    // If the second isn't the same or derived from the first, it's not eqiuv
    if (mecv1.c2ClassId() != mecv2.c2ClassId())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcClass_Mismatch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
            , strClassPathForId(mecv1.c2ClassId())
            , strClassPathForId(mecv2.c2ClassId())
        );
    }
}


tCIDLib::TVoid
TCIDMacroEngine::CheckStackTop( const   tCIDLib::TCard2         c2ExpectedClassId
                                , const tCIDMacroEng::EConstTypes  eConst) const
{
    //
    //  Get the value on the top of the stack. If it's not a value item,
    //  this will throw.
    //
    const TMEngClassVal& mecvVal = mecsiAt(m_c4CallStackTop - 1).mecvPushed();

    // Make sure it meets the target criteria
    if (mecvVal.c2ClassId() != c2ExpectedClassId)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_NotStackItemType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
            , strClassPathForId(c2ExpectedClassId)
        );
    }

    if (mecvVal.eConst() != eConst)
    {
        tCIDLib::TErrCode errcToThrow;
        if (eConst == tCIDMacroEng::EConstTypes::Const)
            errcToThrow = kMEngErrs::errcEng_NotConstStackItem;
        else
            errcToThrow = kMEngErrs::errcEng_NotNConstStackItem;

        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , errcToThrow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
            , strClassPathForId(c2ExpectedClassId)
        );
    }
}


//
//  Cleans up the data that is accumulated during a run of the compiler,
//  so that it's ready for another around. Not that we can be told not to
//  remove the built in classes, which is typically the case. They can stay
//  the same forever and only need to be removed when the engine is
//  destroyed.
//
tCIDLib::TVoid
TCIDMacroEngine::Cleanup(const tCIDLib::TBoolean bRemoveBuiltIns)
{
    //
    //  Clean up the classes. If removing built ins as well, then we
    //  can do the simple scenario. Else we go through and only remove
    //  the non-built ins.
    //
    try
    {
        // We flush the by name list either way
        m_colClasses.RemoveAll();

        if (bRemoveBuiltIns)
        {
            // And we are doing them all so just whack the by id list, too
            m_colClassesById.RemoveAll();
        }
         else if (m_colClassesById.bIsEmpty())
        {
            // Built ins are already removed, make sure the by name list is clean
            m_colClasses.RemoveAll();
        }
         else
        {
            //
            //  The array id that we use as the last built in id has to be
            //  beyond the last of the intrinsic ids. So sanity check that.
            //
            CIDAssert
            (
                m_c2ArrayId >= tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Count)
                , L"Invalid array id in CML engine cleanup"
            );

            //
            //  We have to leave the built ins. The most efficent way
            //  to do this is to delete all of the ones from the by id
            //  list that are past the last built in id. Then just go
            //  back and add the remaining ones back to the by name
            //  list again. Go backwards to avoid recompaction every time.
            //
            tCIDLib::TCard4 c4Index = m_colClassesById.c4ElemCount() - 1;
            const tCIDLib::TCard4 c4Last = m_c2ArrayId;
            while (c4Index > c4Last)
                m_colClassesById.RemoveAt(c4Index--);

            // Ok, now add the ones left back
            for (c4Index = 0; c4Index <= c4Last; c4Index++)
                m_colClasses.Add(m_colClassesById[c4Index]);
        }
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_CleanupErr
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
        );
    }

    // And clean up the temp pool
    try
    {
        m_colTempPool.RemoveAll();
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_CleanupErr
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
        );
    }

    // Reset the console output stream
    if (m_pstrmConsole)
        m_pstrmConsole->Reset();
}


//
//  Takes an OS level file path and converts it to the CML level file
//  path. To go the other way use bExpandFilePath().
//
tCIDLib::TVoid
TCIDMacroEngine::ContractFilePath( const   TString&    strOSPath
                                    ,       TPathStr&   pathToFill)
{
    m_pmefrToUse->ContractPath(strOSPath, pathToFill);

    // Make sure it has the right type of slashes
    pathToFill.bReplaceChar(L'/', L'\\');
}


//
//  Get or set the exption reporting flag, which controls whether we
//  report at throw or when unhandled (when in the IDE.)
//
tCIDMacroEng::EExceptReps TCIDMacroEngine::eExceptReport() const
{
    return m_eExceptReport;
}

tCIDMacroEng::EExceptReps
TCIDMacroEngine::eExceptReport(const tCIDMacroEng::EExceptReps eToSet)
{
    m_eExceptReport = eToSet;
    return m_eExceptReport;
}


//
//  Resolves a possibly partial class path to a full one. It may be
//  ambiguous so we return all possible matches.
//
tCIDMacroEng::EClassMatches TCIDMacroEngine::
eResolveClassName(  const   TString&                        strName
                    ,       TRefCollection<TMEngClassInfo>& colMatches)
{
    //
    //  There are three scenarios, which are:
    //
    //  1. Fully qualified
    //  2. Relative
    //  3. Relative plus nested
    //
    enum ETypes
    {
        EType_Fully
        , EType_Relative
        , EType_Nested
    };

    // Insure that the collection is non-adopting
    if (colMatches.eAdopt() != tCIDLib::EAdoptOpts::NoAdopt)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_MustBeNonAdopting
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }

    // Looks ok, so clean it out
    colMatches.RemoveAll();

    // First we figure out which scenario we are looking at.
    ETypes eType;
    tCIDLib::TCard4 c4Pos1;
    tCIDLib::TCard4 c4Pos2;
    if (strName.bFirstOccurrence(kCIDLib::chPeriod, c4Pos1))
    {
        //
        //  If it doesn't start with MEng and has only one period, then
        //  it's a relative path.
        //
        c4Pos2 = c4Pos1;
        if (!strName.bStartsWithI(L"MEng")
        &&  !strName.bNextOccurrence(kCIDLib::chPeriod, c4Pos2))
        {
            eType = EType_Nested;
        }
         else
        {
            eType = EType_Fully;
        }
    }
     else
    {
        // No periods so must be relative
        eType = EType_Relative;
    }

    tCIDMacroEng::EClassMatches eRet;
    if (eType == EType_Fully)
    {
        TMEngClassInfo* pmeciTarget = pmeciFind(strName);
        if (!pmeciTarget)
            return tCIDMacroEng::EClassMatches::NotFound;

        colMatches.Add(pmeciTarget);
        eRet = tCIDMacroEng::EClassMatches::Unique;
    }
     else if (eType == EType_Nested)
    {
        // Get out the first part of the name
        TString strFind(strName);
        if (eType == EType_Nested)
            strFind.Cut(c4Pos1);

        // Make sure it's unique and remember which one we hit, if any
        TMEngClassInfo* pmeciCur = 0;
        tCIDLib::TCard4 c4HitCount = 0;
        const tCIDLib::TCard4 c4Count = m_colClassesById.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            if (m_colClassesById[c4Index]->strName() == strFind)
            {
                pmeciCur = m_colClassesById[c4Index];
                c4HitCount++;
            }
        }

        // If we hit one, let's check it, else we failed
        if (c4HitCount)
        {
            // Make sure it has a nested class of the second part
            strFind = pmeciCur->strClassPath();
            strFind.Append(kCIDLib::chPeriod);
            strFind.AppendSubStr(strName, c4Pos1 + 1);

            TMEngClassInfo* pmeciTarget = pmeciFind(strFind);
            if (pmeciTarget)
            {
                eRet = tCIDMacroEng::EClassMatches::Unique;
                colMatches.Add(pmeciTarget);
            }
             else
            {
                eRet = tCIDMacroEng::EClassMatches::NotFound;
            }
        }
         else
        {
            eRet = tCIDMacroEng::EClassMatches::NotFound;
        }
    }
     else
    {
        // It's relative so it must be unique
        const tCIDLib::TCard4 c4Count = m_colClassesById.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            TMEngClassInfo* pmeciCur = m_colClassesById[c4Index];
            if (pmeciCur->strName() == strName)
                colMatches.Add(pmeciCur);
        }

        if (colMatches.bIsEmpty())
            eRet = tCIDMacroEng::EClassMatches::NotFound;
        else if (colMatches.c4ElemCount() > 1)
            eRet = tCIDMacroEng::EClassMatches::Ambiguous;
        else
            eRet = tCIDMacroEng::EClassMatches::Unique;
    }

    return eRet;
}


// Translates a numeric class id to it's intrinsic enumerated value
tCIDMacroEng::ENumTypes
TCIDMacroEngine::eXlatNumType(const tCIDLib::TCard2 c2NumClassId)
{
    if ((c2NumClassId < tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::FirstNum))
    ||  (c2NumClassId > tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::LastNum)))
    {
        return tCIDMacroEng::ENumTypes::None;
    }
    return tCIDMacroEng::ENumTypes
    (
        (c2NumClassId - tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card1)) + 1
    );
}


// Set a C++ excpetion on, which will be reported to any installed handler
tCIDLib::TVoid TCIDMacroEngine::Exception(const TError& errThrown)
{
    // If we have a handler, let it know about this one
    if (m_pmeehToUse)
    {
        m_pmeehToUse->Exception(errThrown, *this);
        errThrown.bReported(kCIDLib::True);
    }
}


//
//  Takes a macro level file path and converts it to the full OS level
//  file path. To go the other way use bContractFilePath().
//
tCIDLib::TVoid
TCIDMacroEngine::ExpandFilePath(const   TString&    strMacroPath
                                ,       TPathStr&   pathToFill)
{

    //
    //  Make sure it has the right type of slashes. Some folks use
    //  forward slashes instead of back. So if the native path sep
    //  isn't forward, then replace them with the native one.
    //
    if (kCIDLib::chPathSep != L'/')
    {
        TString strTmp(strMacroPath);
        strTmp.bReplaceChar(L'/', kCIDLib::chPathSep);
        m_pmefrToUse->ExpandPath(strTmp, pathToFill);
    }
     else
    {
        m_pmefrToUse->ExpandPath(strMacroPath, pathToFill);
    }
}


// Get a TFloat4 value on the stack at the indicated index
tCIDLib::TFloat4
TCIDMacroEngine::f4StackValAt(  const   tCIDLib::TCard4     c4Index
                                , const tCIDLib::TBoolean   bCheckType) const
{
    const TMEngClassVal& mecvVal = mecsiAt(c4Index).mecvPushed();
    if (bCheckType)
        CheckItemType(mecvVal.c2ClassId(), tCIDMacroEng::EIntrinsics::Float4, CID_LINE, c4Index);
    return static_cast<const TMEngFloat4Val&>(mecvVal).f4Value();
}


// Get a TFloat8 value on the stack at the indicated index
tCIDLib::TFloat8
TCIDMacroEngine::f8StackValAt(  const   tCIDLib::TCard4     c4Index
                                , const tCIDLib::TBoolean   bCheckType) const
{
    const TMEngClassVal& mecvVal = mecsiAt(c4Index).mecvPushed();
    if (bCheckType)
        CheckItemType(mecvVal.c2ClassId(), tCIDMacroEng::EIntrinsics::Float8, CID_LINE, c4Index);
    return static_cast<const TMEngFloat8Val&>(mecvVal).f8Value();
}


tCIDLib::TVoid TCIDMacroEngine::FlipStackTop()
{
    // These must be at least two items on the stack for this to be legal
    if (m_c4CallStackTop < 2)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_BadStackFlip
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
    m_colCallStack.SwapItems(m_c4CallStackTop - 2, m_c4CallStackTop - 1);
}


tCIDLib::TInt1
TCIDMacroEngine::i1StackValAt(  const   tCIDLib::TCard4     c4Index
                                , const tCIDLib::TBoolean   bCheckType) const
{
    const TMEngClassVal& mecvVal = mecsiAt(c4Index).mecvPushed();
    if (bCheckType)
        CheckItemType(mecvVal.c2ClassId(), tCIDMacroEng::EIntrinsics::Int1, CID_LINE, c4Index);
    return static_cast<const TMEngInt1Val&>(mecvVal).i1Value();
}


tCIDLib::TInt2
TCIDMacroEngine::i2StackValAt(  const   tCIDLib::TCard4     c4Index
                                , const tCIDLib::TBoolean   bCheckType) const
{
    const TMEngClassVal& mecvVal = mecsiAt(c4Index).mecvPushed();
    if (bCheckType)
        CheckItemType(mecvVal.c2ClassId(), tCIDMacroEng::EIntrinsics::Int2, CID_LINE, c4Index);
    return static_cast<const TMEngInt2Val&>(mecvVal).i2Value();
}


tCIDLib::TInt4
TCIDMacroEngine::i4StackValAt(  const   tCIDLib::TCard4     c4Index
                                , const tCIDLib::TBoolean   bCheckType) const
{
    const TMEngClassVal& mecvVal = mecsiAt(c4Index).mecvPushed();
    if (bCheckType)
        CheckItemType(mecvVal.c2ClassId(), tCIDMacroEng::EIntrinsics::Int4, CID_LINE, c4Index);
    return static_cast<const TMEngInt4Val&>(mecvVal).i4Value();
}


tCIDLib::TInt4
TCIDMacroEngine::i4Run(         TMEngClassVal&      mecvTarget
                        , const TString&            strParmVals
                        , const TCIDUserCtx* const  pcuctxRights)
{
    TParmList colParms(tCIDLib::EAdoptOpts::Adopt);
    try
    {
        // Set the passed user contact while we are in here
        TGFJanitor<const TCIDUserCtx*> janRights(&m_pcuctxRights, pcuctxRights);

        //
        //  Search this class for a legal entry point. It must have
        //  particular name and form.
        //
        TMEngClassInfo& meciTarget = meciFind(mecvTarget.c2ClassId());
        const tCIDLib::TCard2 c2MethodId = c2FindEntryPoint(meciTarget, 0);

        // Try to parse out the parameters
        TVector<TString> colSrcParms;
        const tCIDLib::TCard4 c4ParmCount = TExternalProcess::c4BreakOutParms
        (
            strParmVals
            , colSrcParms
        );

        // Make sure that we got the correct number of parms
        TMEngMethodInfo& methiTarget = meciTarget.methiFind(c2MethodId);
        if (c4ParmCount != methiTarget.c4ParmCount())
        {
            facCIDMacroEng().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcEng_BadEntryParmCnt
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::BadParms
                , meciTarget.strClassPath()
                , TCardinal(methiTarget.c4ParmCount())
                , TCardinal(c4ParmCount)
            );
        }

        // Loop through and try to create the value objects
        TString strName;
        for (tCIDLib::TCard2 c2Id = 0; c2Id < c4ParmCount; c2Id++)
        {
            const TMEngParmInfo& mepiTar = methiTarget.mepiFind(c2Id);
            const TString& strCur = colSrcParms[c2Id];

            // The direction indicates whether it's const or not
            const tCIDMacroEng::EConstTypes eConst
            (
                (mepiTar.eDir() == tCIDMacroEng::EParmDirs::In)
                ? tCIDMacroEng::EConstTypes::Const : tCIDMacroEng::EConstTypes::NonConst
            );

            // The name we'll give it
            strName = L"Parm";
            strName.AppendFormatted(tCIDLib::TCard4(c2Id + 1));

            // Create a new value object of the target type
            TMEngClassInfo& meciParm = meciFind(mepiTar.c2ClassId());
            TMEngClassVal* pmecvNew = meciParm.pmecvMakeStorage
            (
                strName, *this, eConst
            );
            colParms.Add(pmecvNew);

            // Ask it to parse from the current value
            if (!pmecvNew->bParseFromText(strCur, meciParm))
            {
                //
                //  If we have an error handler, then build up a macro
                //  level exception and report that.
                //
                if (m_pmeehToUse)
                {
                    //
                    //  Get the base info class info. This guy has nested
                    //  enum types that provide the errors for stuff like
                    //  this.
                    //
                    const TMEngBaseInfoInfo& meciBaseInfo
                    (
                        *pmeciFindAs<TMEngBaseInfoInfo>
                        (
                            TMEngBaseInfoInfo::strPath(), kCIDLib::True
                        )
                    );

                    //
                    //  Get the enum class object for the engine error enum
                    //  and the ordinal for the bad parm error.
                    //
                    const TMEngEnumInfo& meciErr = meciBaseInfo.meciMEngErrors();
                    const tCIDLib::TCard4 c4ErrId = meciBaseInfo.c4ErrIdBadEntryParm;

                    //
                    //  We need the line number of the entry point. The entry
                    //  point is always an opcode based method implementation,
                    //  so we can just cast to that, and then we can get the
                    //  first line number within the implementation.
                    //
                    TMEngOpMethodImpl& methEP = *static_cast<TMEngOpMethodImpl*>
                    (
                        meciTarget.pmethFind(c2MethodId)
                    );

                    TString strErrText
                    (
                        kMEngErrs::errcEng_BadTextEPParmVal
                        , facCIDMacroEng()
                        , TCardinal(c2Id + 1UL)
                        , meciTarget.strClassPath()
                        , strClassPathForId(mepiTar.c2ClassId())
                    );


                    TMEngExceptVal mecvExcept(L"BadParm", tCIDMacroEng::EConstTypes::Const);
                    mecvExcept.Set
                    (
                        meciErr.c2Id()
                        , meciTarget.strClassPath()
                        , c4ErrId
                        , meciErr.strItemName(c4ErrId)
                        , strErrText
                        , methEP.c4FirstLineNum()
                    );

                    m_pmeehToUse->MacroException(mecvExcept, *this);
                }

                //
                //  If we are in the IDE, report we are done and return a -1 status.
                //  Else, throw an exception.
                //
                if (!m_pmedbgToUse)
                {
                    facCIDMacroEng().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kMEngErrs::errcEng_BadTextEPParmVal
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::BadParms
                        , TCardinal(c2Id + 1UL)
                        , meciTarget.strClassPath()
                        , strClassPathForId(mepiTar.c2ClassId())
                    );
                }

                m_pmedbgToUse->FinishedDbg(tCIDMacroEng::EDbgActs::Exit);
                return -1;
            }
        }
    }

    catch(const TDbgExitException& excptForce)
    {
        if (m_pmedbgToUse)
            m_pmedbgToUse->FinishedDbg(excptForce.m_eReason);
        return -1;
    }

    catch(const TError& errToCatch)
    {
        Exception(errToCatch);
        if (m_pmedbgToUse)
            m_pmedbgToUse->FinishedDbg(tCIDMacroEng::EDbgActs::Exit);
        throw;
    }

    catch(...)
    {
        UnknownException();
        if (m_pmedbgToUse)
            m_pmedbgToUse->FinishedDbg(tCIDMacroEng::EDbgActs::Exit);
        throw;
    }

    // And now call the other version with the parms
    return i4Run(mecvTarget, colParms, pcuctxRights);
}


tCIDLib::TInt4
TCIDMacroEngine::i4Run(         TMEngClassVal&      mecvTarget
                        ,       TParmList&          colParms
                        , const TCIDUserCtx* const  pcuctxRights)
{
    tCIDLib::TInt4 i4Res = 0;
    try
    {
        // Set the passed user contact while we are in here
        TGFJanitor<const TCIDUserCtx*> janRights(&m_pcuctxRights, pcuctxRights);

        //
        //  Allow derived engine classes to adjust the start object and/or
        //  parms before we continue.
        //
        AdjustStart(mecvTarget, colParms);

        //
        //  If the temp pool is empty, then this is the first time this engine
        //  instance has been used, so we need to initialize the pool.
        //
        if (m_colTempPool.bIsEmpty())
            InitTempPool();

        //
        //  Search this class for a legal entry point. It must have particular
        //  name and form.
        //
        TMEngClassInfo& meciTarget = meciFind(mecvTarget.c2ClassId());
        const tCIDLib::TCard2 c2MethodId = c2FindEntryPoint(meciTarget, 0);



        // Push the return value first before any parms
        PushPoolValue(tCIDMacroEng::EIntrinsics::Int4, tCIDMacroEng::EConstTypes::NonConst);

        //
        //  Make sure we have the correct type and kind of parameters, then
        //  push them onto the stack. Say that they are parms, so that they
        //  will not get deleted when popped.
        //
        TMEngMethodInfo& methiTarget = meciTarget.methiFind(c2MethodId);
        const tCIDLib::TCard4 c4ParmCount = colParms.c4ElemCount();
        if (c4ParmCount != methiTarget.c4ParmCount())
        {
            facCIDMacroEng().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcEng_BadEntryParmCnt
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::BadParms
                , meciTarget.strClassPath()
                , TCardinal(methiTarget.c4ParmCount())
                , TCardinal(c4ParmCount)
            );
        }

        // We have the right number, so see if they are the right type
        for (tCIDLib::TCard2 c2Id = 0; c2Id < c4ParmCount; c2Id++)
        {
            const TMEngParmInfo& mepiTar = methiTarget.mepiFind(c2Id);
            const TMEngClassVal& mecvSrc = *colParms[c2Id];

            //
            //  The passed value must be the same or derived from the target,
            //  or they must be equivalent collections with identical element types.
            //
            if (!bIsDerivedFrom(mecvSrc.c2ClassId(), mepiTar.c2ClassId())
            &&  !bAreEquivCols(mecvSrc.c2ClassId(), mepiTar.c2ClassId(), kCIDLib::True))
            {
                facCIDMacroEng().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kMEngErrs::errcEng_BadEntryParmType
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::BadParms
                    , TCardinal(c2Id + 1UL)
                    , meciTarget.strClassPath()
                    , strClassPathForId(mepiTar.c2Id())
                    , strClassPathForId(mecvSrc.c2ClassId())
                );
            }

            //
            //  Looks ok, so push it. Say it's a parm and say that it's a
            //  repush so that it's clear that we own it.
            //
            PushValue(colParms[c2Id], tCIDMacroEng::EStackItems::Parm, kCIDLib::True);
        }

        // And finally push the method call
        meciPushMethodCall(meciTarget.c2Id(), c2MethodId);

        //
        //  And invoke the indicated method on this class, then pop the method
        //  call item and parms off when we get back. We can do monomorphic dispatch
        //  here since we know the target class has the method.
        //
        meciTarget.Invoke(*this, mecvTarget, c2MethodId, tCIDMacroEng::EDispatch::Mono);
        MultiPop(c4ParmCount + 1);

        #if CID_DEBUG_ON
        if (m_c4CallStackTop != 1)
        {
            facCIDMacroEng().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcEng_BadEndStackTop
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
                , TCardinal(m_c4CallStackTop)
            );
        }
        #endif

        // Get the return value out
        i4Res = i4StackValAt(0);

        // Remove the return value now
        PopTop();

        // Output a new line to the stream console if present and flush
        if (m_pstrmConsole)
        {
            *m_pstrmConsole << kCIDLib::NewLn;
            m_pstrmConsole->Flush();
        }

        // If we have a debug interface installed, post a normal exit
        if (m_pmedbgToUse)
            m_pmedbgToUse->FinishedDbg(tCIDMacroEng::EDbgActs::Exit);
    }

    catch(const TDbgExitException& excptForce)
    {
        if (m_pmedbgToUse)
            m_pmedbgToUse->FinishedDbg(excptForce.m_eReason);

        // Force a known return value
        i4Res = kCIDLib::i4MinInt;
    }

    catch(const TExceptException&)
    {
        //
        //  If there is an error handler, and we are in the mode where we
        //  report macros if they aren'thandled, then report it. If we are
        //  in the other mode, it will already have been reported at the point
        //  of throw.
        //
        if (m_pmeehToUse && (m_eExceptReport == tCIDMacroEng::EExceptReps::NotHandled))
            m_pmeehToUse->MacroException(*m_pmecvThrown, *this);

        if (m_pmedbgToUse)
            m_pmedbgToUse->FinishedDbg(tCIDMacroEng::EDbgActs::Exit);

        // It was unhandled, so rethrow to caller
        throw;
    }

    catch(const TError& errToCatch)
    {
        Exception(errToCatch);
        if (m_pmedbgToUse)
            m_pmedbgToUse->FinishedDbg(tCIDMacroEng::EDbgActs::Exit);
        throw;
    }

    catch(...)
    {
        UnknownException();
        if (m_pmedbgToUse)
            m_pmedbgToUse->FinishedDbg(tCIDMacroEng::EDbgActs::Exit);
        throw;
    }

    return i4Res;
}


// If we have a last exception, log it
tCIDLib::TVoid TCIDMacroEngine::LogLastExcept()
{
    if (m_pmecvThrown)
    {
        facCIDMacroEng().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcRT_MacroExcept
            , m_pmecvThrown->strSrcClassPath()
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
            , m_pmecvThrown->strErrorName()
            , TCardinal(m_pmecvThrown->c4LineNum())
        );
    }
}


TMEngMethodInfo&
TCIDMacroEngine::methiFind( const   tCIDLib::TCard2 c2ClassId
                            , const tCIDLib::TCard2 c2MethodId)
{
    #if CID_DEBUG_ON
    CheckClassId(c2ClassId, CID_LINE);
    #endif
    return m_colClassesById[c2ClassId]->methiFind(c2MethodId);
}


TMEngClassInfo& TCIDMacroEngine::meciFind(const TString& strClassPath)
{
    TMEngClassInfo* pmeciRet = m_colClasses.pobjFindByKey
    (
        strClassPath
        , kCIDLib::False
    );

    if (!pmeciRet)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_ClassNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strClassPath
        );
    }
    return *pmeciRet;
}

const TMEngClassInfo&
TCIDMacroEngine::meciFind(const TString& strClassPath) const
{
    const TMEngClassInfo* pmeciRet = m_colClasses.pobjFindByKey
    (
        strClassPath
        , kCIDLib::False
    );

    if (!pmeciRet)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_ClassNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strClassPath
        );
    }
    return *pmeciRet;
}


TMEngClassInfo& TCIDMacroEngine::meciFind(const tCIDLib::TCard2 c2Id)
{
    #if CID_DEBUG_ON
    CheckClassId(c2Id, CID_LINE);
    #endif
    return *m_colClassesById[c2Id];
}

TMEngClassInfo& TCIDMacroEngine::meciFind(const tCIDMacroEng::EIntrinsics eClassType)
{
    return meciFind(tCIDLib::TCard2(eClassType));
}

const TMEngClassInfo& TCIDMacroEngine::meciFind(const tCIDLib::TCard2 c2Id) const
{
    #if CID_DEBUG_ON
    CheckClassId(c2Id, CID_LINE);
    #endif
    return *m_colClassesById[c2Id];
}

const TMEngClassInfo&
TCIDMacroEngine::meciFind(const tCIDMacroEng::EIntrinsics eClassType) const
{
    return meciFind(tCIDLib::TCard2(eClassType));
}


TMEngClassInfo&
TCIDMacroEngine::meciPushMethodCall(const   tCIDLib::TCard2 c2ClassId
                                    , const tCIDLib::TCard2 c2MethodId)
{
    // Find the two bits we need to push
    TMEngClassInfo* pmeciCalled = &meciFind(c2ClassId);
    TMEngMethodInfo* pmethiCalled = &pmeciCalled->methiFind(c2MethodId);

    CheckStackExpand();

    // Get the top of stack item and reset it with the passed procedure
    TMEngCallStackItem* pmecsiRet = m_colCallStack[m_c4CallStackTop];
    pmecsiRet->Reset(pmeciCalled, pmethiCalled);
    m_c4CallStackTop++;

    // As a convenience, we return the class
    return *pmeciCalled;
}

TMEngClassInfo&
TCIDMacroEngine::meciPushMethodCall(const   tCIDLib::TCard2         c2ClassId
                                    , const tCIDLib::TCard2         c2MethodId
                                    , const TMEngClassInfo* const   pmeciCaller
                                    , const TMEngOpMethodImpl* const pmethCaller
                                    , const TMEngMethodInfo* const  pmethiCaller
                                    ,       TMEngClassVal* const    pmecvCaller
                                    , const tCIDLib::TCard4         c4CallLine
                                    , const tCIDLib::TCard4         c4CallIP)
{
    // Find the two bits we need to push
    TMEngClassInfo* pmeciCalled = &meciFind(c2ClassId);
    TMEngMethodInfo* pmethiCalled = &pmeciCalled->methiFind(c2MethodId);

    CheckStackExpand();

    // Get the top of stack item and reset it with the passed procedure
    TMEngCallStackItem* pmecsiRet = m_colCallStack[m_c4CallStackTop];
    pmecsiRet->Reset
    (
        pmeciCalled
        , pmethiCalled
        , pmeciCaller
        , pmethiCaller
        , pmethCaller
        , pmecvCaller
        , c4CallLine
        , c4CallIP
    );
    m_c4CallStackTop++;

    // As a convenience, we return the class
    return *pmeciCalled;
}


TMEngCallStackItem& TCIDMacroEngine::mecsiAt(const tCIDLib::TCard4 c4Index)
{
    CheckStackIndex(CID_LINE, c4Index);
    return *m_colCallStack[c4Index];
}

const TMEngCallStackItem&
TCIDMacroEngine::mecsiAt(const tCIDLib::TCard4 c4Index) const
{
    CheckStackIndex(CID_LINE, c4Index);
    return *m_colCallStack[c4Index];
}


TMEngCallStackItem& TCIDMacroEngine::mecsiTop()
{
    //
    //  If the stack isn't empty, give them back the top item, which is one
    //  less than the top, because the top points at the next available
    //  slot.
    //
    CheckNotEmpty(CID_LINE);
    return *m_colCallStack[m_c4CallStackTop - 1];
}


const TMEngClassVal&
TCIDMacroEngine::mecvStackAt(const tCIDLib::TCard4 c4Index) const
{
    CheckStackIndex(CID_LINE, c4Index);
    return m_colCallStack[c4Index]->mecvPushed();
}

TMEngClassVal& TCIDMacroEngine::mecvStackAt(const tCIDLib::TCard4 c4Index)
{
    CheckStackIndex(CID_LINE, c4Index);
    return m_colCallStack[c4Index]->mecvPushed();
}


const TMEngClassVal& TCIDMacroEngine::mecvStackAtTop() const
{
    CheckNotEmpty(CID_LINE);
    return m_colCallStack[m_c4CallStackTop - 1]->mecvPushed();
}

TMEngClassVal& TCIDMacroEngine::mecvStackAtTop()
{
    CheckNotEmpty(CID_LINE);
    return m_colCallStack[m_c4CallStackTop - 1]->mecvPushed();
}


//
//  Provide access to any user rights context that the user code may have set
//  on us. We don't care what it is, it's purely a passthrough for user code.
//  It may be null if they never set one.
//
const TCIDUserCtx* TCIDMacroEngine::pcuctxRights() const
{
    return m_pcuctxRights;
}


// Pop the given number of objects off the stack
tCIDLib::TVoid
TCIDMacroEngine::MultiPop(const tCIDLib::TCard4 c4Count)
{
    // Check for underflow. We have to have at least the number indicated
    if (m_c4CallStackTop < c4Count)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_CallStackUnderflow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Underflow
        );
    }

    // It's ok, so pop it
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        m_c4CallStackTop--;

        //
        //  If this is a temp value, then release it back to the pool. If
        //  it's a local, we have to clean it up. Else, it's spoken for by
        //  someone and we don't have to worry. Then, clear the item to
        //  keep the stack readable.
        //
        //  NOTE:   If it's a repush, we don't do anything, because this
        //          is not the original!
        //
        TMEngCallStackItem* pmecsiPop = m_colCallStack[m_c4CallStackTop];
        if (!pmecsiPop->bIsRepush())
        {
            if (pmecsiPop->eType() == tCIDMacroEng::EStackItems::TempVal)
                pmecsiPop->mecvPushed().bIsUsed(kCIDLib::False);
            else if (pmecsiPop->eType() == tCIDMacroEng::EStackItems::Local)
                delete &pmecsiPop->mecvPushed();
        }
        pmecsiPop->Clear();
    }
}


const TMEngClassInfo*
TCIDMacroEngine::pmeciLastMacroCaller(tCIDLib::TCard4& c4Line)
{
    //
    //  Start at the top of the stack and work backwards till we see a
    //  call from a macro level class.
    //
    tCIDLib::TCard4 c4Index = m_c4CallStackTop;
    if (!c4Index)
        return 0;

    TMEngCallStackItem* pmecsiCur;
    const TMEngClassInfo* pmeciRet = 0;
    do
    {
        c4Index--;
        pmecsiCur = m_colCallStack[c4Index];
        if (pmecsiCur->eType() == tCIDMacroEng::EStackItems::MethodCall)
        {
            pmeciRet = pmecsiCur->pmeciCaller(c4Line);
            if (pmeciRet)
                break;
        }
    }   while (c4Index != 0);

    return pmeciRet;
}


TMEngClassInfo*
TCIDMacroEngine::pmeciLoadExternalClass(const TString& strClassPathToLoad)
{
    TMEngClassInfo* pmeciRet = m_colClasses.pobjFindByKey
    (
        strClassPathToLoad
        , kCIDLib::False
    );

    // If already loaded, we are done
    if (pmeciRet)
        return pmeciRet;

    //
    //  Ask the facility class to allow all of the registered class loaders
    //  to have a chance to load this class.
    //
    pmeciRet = facCIDMacroEng().pmeciInvokeExternalLoaders(*this, strClassPathToLoad);
    if (!pmeciRet)
        return pmeciRet;

    //
    //  If this guy has any imports, then we need to recursively load them
    //  before we continue. And we need to do the same to it's parent class
    //  as well. Do the parent first, then do any imports of this class.
    //
    pmeciLoadExternalClass(pmeciRet->strParentClassPath());

    TMEngClassInfo::TImportList::TCursor cursImports(&pmeciRet->m_colImports);
    for (; cursImports; ++cursImports)
        pmeciLoadExternalClass(cursImports->m_strImport);

    //
    //  Do the base init of the class now that all it's needed class are
    //  loaded.
    //
    pmeciRet->BaseClassInit(*this);
    c2AddClass(pmeciRet);
    return pmeciRet;
}


TMEngClassInfo*
TCIDMacroEngine::pmeciFind( const   TString&            strClassPath
                            , const tCIDLib::TBoolean   bThrowIfNot)
{
    return m_colClasses.pobjFindByKey(strClassPath, bThrowIfNot);
}

const TMEngClassInfo*
TCIDMacroEngine::pmeciFind( const   TString&            strClassPath
                            , const tCIDLib::TBoolean   bThrowIfNot) const
{
    return m_colClasses.pobjFindByKey(strClassPath, bThrowIfNot);
}


const TMEngExceptVal& TCIDMacroEngine::mecvExceptionThrown() const
{
    return *m_pmecvThrown;
}

TMEngExceptVal& TCIDMacroEngine::mecvExceptionThrown()
{
    return *m_pmecvThrown;
}


//
//  This method will start at the top of the stack and search downwards
//  until it finds a call item, and return that item and it's stack index.
//  If it doesn't find one, it returns a null pointer.
//
TMEngCallStackItem*
TCIDMacroEngine::pmecsiMostRecentCall(tCIDLib::TCard4& c4ToFill)
{
    if (!m_c4CallStackTop)
    {
        c4ToFill = kCIDLib::c4MaxCard;
        return 0;
    }

    tCIDLib::TCard4 c4Index = m_c4CallStackTop;
    do
    {
        c4Index--;
        TMEngCallStackItem* pmecsiCur = m_colCallStack[c4Index];

        if (pmecsiCur->eType() == tCIDMacroEng::EStackItems::MethodCall)
        {
            c4ToFill = c4Index;
            return pmecsiCur;
        }
    }   while (c4Index > 0);

    c4ToFill = kCIDLib::c4MaxCard;
    return 0;
}


//
//  This guy allows a new value object to be created via a class path name.
//  This allows code that needs to call into macro code from C++ code to
//  create value objects without us having to expose all those headers.
//
TMEngClassVal*
TCIDMacroEngine::pmecvNewFromPath(  const   TString&                strName
                                    , const TString&                strClassPath
                                    , const tCIDMacroEng::EConstTypes  eConst)
{
    // Find the class info object for the passed class
    TMEngClassInfo& meciNew = meciFind(strClassPath);
    return meciNew.pmecvMakeStorage(strName, *this, eConst);
}


TTextOutStream* TCIDMacroEngine::pstrmConsole()
{
    #if CID_DEBUG_ON
    if (!m_pstrmConsole)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcDbg_NoConsoleStrm
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif
    return m_pstrmConsole;
}


tCIDLib::TVoid TCIDMacroEngine::PopBackTo(const tCIDLib::TCard4 c4To)
{
    // If already at that point, then nothing to do
    if (m_c4CallStackTop == c4To)
        return;

    // Else the to index must be less than the current top
    CheckStackIndex(CID_LINE, c4To);

    // Looks ok, so let's do it
    do
    {
        // Move down to the next used item
        m_c4CallStackTop--;

        //
        //  If this is a temp value, then release it back to the pool. If
        //  it's a local, we have to clean it up. Else, it's spoken for by
        //  someone and we don't have to worry. Then, clear the item to
        //  keep the stack readable.
        //
        //  NOTE:   If it's a repush, we don't do anything, because this
        //          is not the original!
        //
        TMEngCallStackItem* pmecsiPop = m_colCallStack[m_c4CallStackTop];
        if (!pmecsiPop->bIsRepush())
        {
            if (pmecsiPop->eType() == tCIDMacroEng::EStackItems::TempVal)
                pmecsiPop->mecvPushed().bIsUsed(kCIDLib::False);
            else if (pmecsiPop->eType() == tCIDMacroEng::EStackItems::Local)
                delete &pmecsiPop->mecvPushed();
        }
        pmecsiPop->Clear();

    }   while (m_c4CallStackTop > c4To);
}


tCIDLib::TVoid TCIDMacroEngine::PopTop()
{
    // Check for underflow
    CheckStackUnderflow(CID_LINE);

    // It's ok, so pop it
    m_c4CallStackTop--;

    //
    //  If this is a temp value, then release it back to the pool. If
    //  it's a local, we have to clean it up. Else, it's spoken for by
    //  someone and we don't have to worry. Then, clear the item to
    //  keep the stack readable.
    //
    //  NOTE:   If it's a repush, we don't do anything, because this
    //          is not the original!
    //
    TMEngCallStackItem* pmecsiPop = m_colCallStack[m_c4CallStackTop];
    if (!pmecsiPop->bIsRepush())
    {
        if (pmecsiPop->eType() == tCIDMacroEng::EStackItems::TempVal)
            pmecsiPop->mecvPushed().bIsUsed(kCIDLib::False);
        else if (pmecsiPop->eType() == tCIDMacroEng::EStackItems::Local)
            delete &pmecsiPop->mecvPushed();
    }
    pmecsiPop->Clear();
}


tCIDLib::TVoid
TCIDMacroEngine::PushBoolean(const  tCIDLib::TBoolean       bToPush
                            , const tCIDMacroEng::EConstTypes  eConst)
{
    CheckStackExpand();

    // Get a temp of this type from the pool and set it to the passed value
    TMEngBooleanVal* pmecvPush = static_cast<TMEngBooleanVal*>
    (
        pmecvGet(tCIDMacroEng::EIntrinsics::Boolean, eConst)
    );
    pmecvPush->bValue(bToPush);

    // Get the top of stack item and reset it with the passed temp value
    TMEngCallStackItem* pmecsiRet = m_colCallStack[m_c4CallStackTop];
    pmecsiRet->Reset(pmecvPush, tCIDMacroEng::EStackItems::TempVal);
    m_c4CallStackTop++;
}

tCIDLib::TVoid
TCIDMacroEngine::PushCard1( const   tCIDLib::TCard1         c1ToPush
                            , const tCIDMacroEng::EConstTypes  eConst)
{
    CheckStackExpand();

    // Get a temp of this type from the pool and set it to the passed value
    TMEngCard1Val* pmecvPush = static_cast<TMEngCard1Val*>
    (
        pmecvGet(tCIDMacroEng::EIntrinsics::Card1, eConst)
    );
    pmecvPush->c1Value(c1ToPush);

    // Get the top of stack item and reset it with the passed temp value
    TMEngCallStackItem* pmecsiRet = m_colCallStack[m_c4CallStackTop];
    pmecsiRet->Reset(pmecvPush, tCIDMacroEng::EStackItems::TempVal);
    m_c4CallStackTop++;
}

tCIDLib::TVoid
TCIDMacroEngine::PushCard2( const   tCIDLib::TCard2         c2ToPush
                            , const tCIDMacroEng::EConstTypes  eConst)
{
    CheckStackExpand();

    // Get a temp of this type from the pool and set it to the passed value
    TMEngCard2Val* pmecvPush = static_cast<TMEngCard2Val*>
    (
        pmecvGet(tCIDMacroEng::EIntrinsics::Card2, eConst)
    );
    pmecvPush->c2Value(c2ToPush);

    // Get the top of stack item and reset it with the passed temp value
    TMEngCallStackItem* pmecsiRet = m_colCallStack[m_c4CallStackTop];
    pmecsiRet->Reset(pmecvPush, tCIDMacroEng::EStackItems::TempVal);
    m_c4CallStackTop++;
}

tCIDLib::TVoid
TCIDMacroEngine::PushCard4( const   tCIDLib::TCard4         c4ToPush
                            , const tCIDMacroEng::EConstTypes  eConst)
{
    CheckStackExpand();

    // Get a temp of this type from the pool and set it to the passed value
    TMEngCard4Val* pmecvPush = static_cast<TMEngCard4Val*>
    (
        pmecvGet(tCIDMacroEng::EIntrinsics::Card4, eConst)
    );
    pmecvPush->c4Value(c4ToPush);

    // Get the top of stack item and reset it with the passed temp value
    TMEngCallStackItem* pmecsiRet = m_colCallStack[m_c4CallStackTop];
    pmecsiRet->Reset(pmecvPush, tCIDMacroEng::EStackItems::TempVal);
    m_c4CallStackTop++;
}

tCIDLib::TVoid
TCIDMacroEngine::PushCard8( const   tCIDLib::TCard8         c8ToPush
                            , const tCIDMacroEng::EConstTypes  eConst)
{
    CheckStackExpand();

    // Get a temp of this type from the pool and set it to the passed value
    TMEngCard8Val* pmecvPush = static_cast<TMEngCard8Val*>
    (
        pmecvGet(tCIDMacroEng::EIntrinsics::Card8, eConst)
    );
    pmecvPush->c8Value(c8ToPush);

    // Get the top of stack item and reset it with the passed temp value
    TMEngCallStackItem* pmecsiRet = m_colCallStack[m_c4CallStackTop];
    pmecsiRet->Reset(pmecvPush, tCIDMacroEng::EStackItems::TempVal);
    m_c4CallStackTop++;
}

tCIDLib::TVoid
TCIDMacroEngine::PushChar(  const   tCIDLib::TCh            chToPush
                            , const tCIDMacroEng::EConstTypes  eConst)
{
    CheckStackExpand();

    // Get a temp of this type from the pool and set it to the passed value
    TMEngCharVal* pmecvPush = static_cast<TMEngCharVal*>
    (
        pmecvGet(tCIDMacroEng::EIntrinsics::Char, eConst)
    );
    pmecvPush->chValue(chToPush);

    // Get the top of stack item and reset it with the passed temp value
    TMEngCallStackItem* pmecsiRet = m_colCallStack[m_c4CallStackTop];
    pmecsiRet->Reset(pmecvPush, tCIDMacroEng::EStackItems::TempVal);
    m_c4CallStackTop++;
}


tCIDLib::TVoid
TCIDMacroEngine::PushEnum(  const   tCIDLib::TCard2         c2ClassId
                            , const tCIDLib::TCard2         c2OrdValue
                            , const tCIDMacroEng::EConstTypes  eConst)
{
    CheckStackExpand();

    // Get a temp of this type from the pool and set it to the passed value
    TMEngEnumVal* pmecvPush = static_cast<TMEngEnumVal*>
    (
        pmecvGet(c2ClassId, eConst)
    );
    pmecvPush->c4Ordinal(c2OrdValue);

    // Get the top of stack item and reset it with the passed temp value
    TMEngCallStackItem* pmecsiRet = m_colCallStack[m_c4CallStackTop];
    pmecsiRet->Reset(pmecvPush, tCIDMacroEng::EStackItems::TempVal);
    m_c4CallStackTop++;
}


//
//  This one handles a special case scenario, which pushes a thrown exception
//  upon entry to a Catch block. It just refers to a magic value object that
//  is a member of this class, since it must potentially exist all the way
//  up the call stack.
//
tCIDLib::TVoid TCIDMacroEngine::PushException()
{
    CheckStackExpand();

    // Get the top of stack item and reset it with the passed temp value
    TMEngCallStackItem* pmecsiRet = m_colCallStack[m_c4CallStackTop];
    pmecsiRet->Reset(m_pmecvThrown, tCIDMacroEng::EStackItems::Exception);
    m_c4CallStackTop++;
}


tCIDLib::TVoid
TCIDMacroEngine::PushFloat4( const  tCIDLib::TFloat4        f4ToPush
                            , const tCIDMacroEng::EConstTypes  eConst)
{
    CheckStackExpand();

    // Get a temp of this type from the pool and set it to the passed value
    TMEngFloat4Val* pmecvPush = static_cast<TMEngFloat4Val*>
    (
        pmecvGet(tCIDMacroEng::EIntrinsics::Float4, eConst)
    );
    pmecvPush->f4Value(f4ToPush);

    // Get the top of stack item and reset it with the passed temp value
    TMEngCallStackItem* pmecsiRet = m_colCallStack[m_c4CallStackTop];
    pmecsiRet->Reset(pmecvPush, tCIDMacroEng::EStackItems::TempVal);
    m_c4CallStackTop++;
}


tCIDLib::TVoid
TCIDMacroEngine::PushFloat8( const  tCIDLib::TFloat8        f8ToPush
                            , const tCIDMacroEng::EConstTypes  eConst)
{
    CheckStackExpand();

    // Get a temp of this type from the pool and set it to the passed value
    TMEngFloat8Val* pmecvPush = static_cast<TMEngFloat8Val*>
    (
        pmecvGet(tCIDMacroEng::EIntrinsics::Float8, eConst)
    );
    pmecvPush->f8Value(f8ToPush);

    // Get the top of stack item and reset it with the passed temp value
    TMEngCallStackItem* pmecsiRet = m_colCallStack[m_c4CallStackTop];
    pmecsiRet->Reset(pmecvPush, tCIDMacroEng::EStackItems::TempVal);
    m_c4CallStackTop++;
}


tCIDLib::TVoid
TCIDMacroEngine::PushInt1(  const   tCIDLib::TInt1          i1ToPush
                            , const tCIDMacroEng::EConstTypes  eConst)
{
    CheckStackExpand();

    // Get a temp of this type from the pool and set it to the passed value
    TMEngInt1Val* pmecvPush = static_cast<TMEngInt1Val*>
    (
        pmecvGet(tCIDMacroEng::EIntrinsics::Int1, eConst)
    );
    pmecvPush->i1Value(i1ToPush);

    // Get the top of stack item and reset it with the passed temp value
    TMEngCallStackItem* pmecsiRet = m_colCallStack[m_c4CallStackTop];
    pmecsiRet->Reset(pmecvPush, tCIDMacroEng::EStackItems::TempVal);
    m_c4CallStackTop++;
}


tCIDLib::TVoid
TCIDMacroEngine::PushInt2(  const   tCIDLib::TInt2          i2ToPush
                            , const tCIDMacroEng::EConstTypes  eConst)
{
    CheckStackExpand();

    // Get a temp of this type from the pool and set it to the passed value
    TMEngInt2Val* pmecvPush = static_cast<TMEngInt2Val*>
    (
        pmecvGet(tCIDMacroEng::EIntrinsics::Int2, eConst)
    );
    pmecvPush->i2Value(i2ToPush);

    // Get the top of stack item and reset it with the passed temp value
    TMEngCallStackItem* pmecsiRet = m_colCallStack[m_c4CallStackTop];
    pmecsiRet->Reset(pmecvPush, tCIDMacroEng::EStackItems::TempVal);
    m_c4CallStackTop++;
}


tCIDLib::TVoid
TCIDMacroEngine::PushInt4(  const   tCIDLib::TInt4          i4ToPush
                            , const tCIDMacroEng::EConstTypes  eConst)
{
    CheckStackExpand();

    // Get a temp of this type from the pool and set it to the passed value
    TMEngInt4Val* pmecvPush = static_cast<TMEngInt4Val*>
    (
        pmecvGet(tCIDMacroEng::EIntrinsics::Int4, eConst)
    );
    pmecvPush->i4Value(i4ToPush);

    // Get the top of stack item and reset it with the passed temp value
    TMEngCallStackItem* pmecsiRet = m_colCallStack[m_c4CallStackTop];
    pmecsiRet->Reset(pmecvPush, tCIDMacroEng::EStackItems::TempVal);
    m_c4CallStackTop++;
}


tCIDLib::TVoid
TCIDMacroEngine::PushPoolValue( const   TString&                strClassPath
                                , const tCIDMacroEng::EConstTypes  eConst)
{
    CheckStackExpand();

    // Look up the class id for this guy and use that to get a temp
    TMEngClassVal* pmecvPush = pmecvGet(meciFind(strClassPath).c2Id(), eConst);
    TJanitor<TMEngClassVal> janPush(pmecvPush);

    // Get the top of stack item and reset it with the passed temp value
    TMEngCallStackItem* pmecsiRet = m_colCallStack[m_c4CallStackTop];
    pmecsiRet->Reset(janPush.pobjOrphan(), tCIDMacroEng::EStackItems::TempVal);
    m_c4CallStackTop++;
}

tCIDLib::TVoid
TCIDMacroEngine::PushPoolValue( const   TString&                strName
                                , const tCIDLib::TCard2         c2TypeId
                                , const tCIDMacroEng::EConstTypes  eConst)
{
    CheckStackExpand();

    // Look up the class id for this guy and use that to get a temp
    TMEngClassVal* pmecvPush = pmecvGet
    (
        strName
        , meciFind(c2TypeId).c2Id()
        , eConst
    );
    TJanitor<TMEngClassVal> janPush(pmecvPush);

    // Get the top of stack item and reset it with the passed temp value
    TMEngCallStackItem* pmecsiRet = m_colCallStack[m_c4CallStackTop];
    pmecsiRet->Reset(janPush.pobjOrphan(), tCIDMacroEng::EStackItems::TempVal);
    m_c4CallStackTop++;
}

tCIDLib::TVoid
TCIDMacroEngine::PushPoolValue( const   TString&                strName
                                , const tCIDMacroEng::EIntrinsics  eType
                                , const tCIDMacroEng::EConstTypes  eConst)
{
    PushPoolValue(strName, tCIDLib::TCard2(eType), eConst);
}

tCIDLib::TVoid
TCIDMacroEngine::PushPoolValue( const   tCIDLib::TCard2         c2Id
                                , const tCIDMacroEng::EConstTypes  eConst)
{
    CheckStackExpand();

    // Get a value from the pool, of the type passed
    TMEngClassVal* pmecvPush = pmecvGet(c2Id, eConst);
    TJanitor<TMEngClassVal> janPush(pmecvPush);

    // Get the top of stack item and reset it with the passed temp value
    TMEngCallStackItem* pmecsiRet = m_colCallStack[m_c4CallStackTop];
    pmecsiRet->Reset(janPush.pobjOrphan(), tCIDMacroEng::EStackItems::TempVal);
    m_c4CallStackTop++;
}

tCIDLib::TVoid
TCIDMacroEngine::PushPoolValue( const   tCIDMacroEng::EIntrinsics  eType
                                , const tCIDMacroEng::EConstTypes  eConst)
{
    PushPoolValue(tCIDLib::TCard2(eType), eConst);
}


tCIDLib::TVoid
TCIDMacroEngine::PushString(const   TString&                strToPush
                            , const tCIDMacroEng::EConstTypes  eConst)
{
    CheckStackExpand();

    // Get a temp of this type from the pool and set it to the passed value
    TMEngStringVal* pmecvPush = static_cast<TMEngStringVal*>
    (
        pmecvGet(tCIDMacroEng::EIntrinsics::String, eConst)
    );
    pmecvPush->strValue(strToPush);

    // Get the top of stack item and reset it with the passed temp value
    TMEngCallStackItem* pmecsiRet = m_colCallStack[m_c4CallStackTop];
    pmecsiRet->Reset(pmecvPush, tCIDMacroEng::EStackItems::TempVal);
    m_c4CallStackTop++;
}


tCIDLib::TVoid
TCIDMacroEngine::PushStringList(const   TVector<TString>&       colToPush
                                , const tCIDMacroEng::EConstTypes  eConst)
{
    CheckStackExpand();

    // Get a temp of this type from the pool and set it to the passed value
    TMEngStrListVal* pmecvPush = static_cast<TMEngStrListVal*>
    (
        pmecvGet(tCIDMacroEng::EIntrinsics::StringList, eConst)
    );
    pmecvPush->Reset(colToPush, kCIDLib::True);

    // Get the top of stack item and reset it with the passed temp value
    TMEngCallStackItem* pmecsiRet = m_colCallStack[m_c4CallStackTop];
    pmecsiRet->Reset(pmecvPush, tCIDMacroEng::EStackItems::TempVal);
    m_c4CallStackTop++;
}


tCIDLib::TVoid
TCIDMacroEngine::PushTime(  const   TTime&                  tmToPush
                            , const tCIDMacroEng::EConstTypes  eConst)
{
    CheckStackExpand();

    // Get a temp of this type from the pool and set it to the passed value
    TMEngTimeVal* pmecvPush = static_cast<TMEngTimeVal*>
    (
        pmecvGet(tCIDMacroEng::EIntrinsics::Time, eConst)
    );
    pmecvPush->tmValue(tmToPush);

    // Get the top of stack item and reset it with the passed temp value
    TMEngCallStackItem* pmecsiRet = m_colCallStack[m_c4CallStackTop];
    pmecsiRet->Reset(pmecvPush, tCIDMacroEng::EStackItems::TempVal);
    m_c4CallStackTop++;
}


tCIDLib::TVoid
TCIDMacroEngine::PushTry(const tCIDLib::TCard4 c4ToPush)
{
    CheckStackExpand();

    // Get the top of stack item and reset it with the passed value
    TMEngCallStackItem* pmecsiRet = m_colCallStack[m_c4CallStackTop];
    pmecsiRet->Reset(tCIDMacroEng::EStackItems::Try, c4ToPush);
    m_c4CallStackTop++;
}


tCIDLib::TVoid
TCIDMacroEngine::PushValue(         TMEngClassVal* const    pmecvPushed
                            , const tCIDMacroEng::EStackItems  eType
                            , const tCIDLib::TBoolean       bRepush)
{
    CheckStackExpand();

    // Get the top of stack item and reset it with the passed value
    TMEngCallStackItem* pmecsiRet = m_colCallStack[m_c4CallStackTop];
    pmecsiRet->Reset(pmecvPushed, eType);
    pmecsiRet->bIsRepush(bRepush);
    m_c4CallStackTop++;
}


//
//  This is mainly to support packaging of macros/drivers. We will find
//  all the loaded (i.e. referenced) that are in the indicated scope.
//
tCIDLib::TVoid
TCIDMacroEngine::QueryScopeClasses( const   TString&                strScope
                                    ,       tCIDLib::TStrCollect&   colToFill) const
{
    colToFill.RemoveAll();

    //
    //  We can start past the intrinsic classes, which are always there and
    //  always first.
    //
    const tCIDLib::TCard4 c4Count = m_colClassesById.c4ElemCount();
    tCIDLib::TCard4 c4Index = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Count);
    while (c4Index < c4Count)
    {
        const TMEngClassInfo* pmeciCur = m_colClassesById[c4Index];
        if (pmeciCur->strBasePath().eCompareI(strScope) == tCIDLib::ESortComps::Equal)
            colToFill.objAdd(pmeciCur->strClassPath());
        c4Index++;
    }
}



//
//  Repush the value at the indicated stack index. It's marked as being
//  repushed, so that when it's popped it won't be destroyed becasue it's
//  not the original.
//
tCIDLib::TVoid TCIDMacroEngine::RepushAt(const tCIDLib::TCard4 c4Index)
{
    CheckStackExpand();

    // Make sure it's a value item
    TMEngCallStackItem& mecsiToPush = mecsiAt(c4Index);
    const tCIDMacroEng::EStackItems eType = mecsiToPush.eType();
    if (eType == tCIDMacroEng::EStackItems::MethodCall)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_RepushMethod
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }

    // Get the value from this item
    TMEngClassVal* pmecvToPush = &mecsiToPush.mecvPushed();

    //
    //  Push it again, with the same type. But, set the 'repush' flag. This
    //  lets us know when we pop that we never have to worry about cleanup,
    //  because this isn't the real guy
    //
    PushValue(pmecvToPush, mecsiToPush.eType(), kCIDLib::True);
}


//
//  Reset the macro engine. We make sure the stack is cleared, and we clean
//  up any objects in pools and globals, and remove all classes other than
//  the instrinsic ones.
//
//  Note that we don't reset the unique id counter. We just let it continue
//  to run upwards, for increased safety.
//
tCIDLib::TVoid TCIDMacroEngine::Reset()
{
    // Make sure that the call stack get's emptied
    try
    {
        m_colCallStack.RemoveAll();
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_ResetErr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }

    //
    //  Now clean up the other stuff. Note that we don't remove the built
    //  in classes from the class list, since they can remain the same
    //  always, and it significantly improves performance. We only remove
    //  those when the engine is cleaned up. Passing false to the cleanup
    //  method does this.
    //
    Cleanup(kCIDLib::False);
    m_c4CurLine = 0;
    m_c4CallStackTop = 0;

    //
    //  Set this to the next available one after the builtins, because we
    //  didn't remove those!!!
    //
    m_c2NextClassId = m_c2ArrayId + 1;
}


// If we have an error handler and an exception, send it to the error handler
tCIDLib::TVoid TCIDMacroEngine::ReportLastExcept()
{
    if (m_pmeehToUse && m_pmecvThrown)
        m_pmeehToUse->MacroException(*m_pmecvThrown, *this);
}


const TString&
TCIDMacroEngine::strClassPathForId( const   tCIDLib::TCard2     c2Id
                                    , const tCIDLib::TBoolean   bThrowIfNot) const
{
    if (c2Id >= m_colClassesById.c4ElemCount())
    {
        if (bThrowIfNot)
        {
            facCIDMacroEng().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcClass_BadId
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Index
                , TCardinal(c2Id)
                , TCardinal(m_colClassesById.c4ElemCount())
            );
        }
        return TFacCIDMacroEng::strUnknownClass;
    }
    return m_colClassesById[c2Id]->strClassPath();
}

const TString&
TCIDMacroEngine::strClassPathForId( const   tCIDMacroEng::EIntrinsics  eType
                                    , const tCIDLib::TBoolean       bThrowIfNot) const
{
    return strClassPathForId(tCIDLib::TCard2(eType), bThrowIfNot);
}


// Get or set the special dynamic type reference value
const TString& TCIDMacroEngine::strSpecialDynRef() const
{
    return m_strSpecialDynRef;
}

const TString& TCIDMacroEngine::strSpecialDynRef(const TString& strToSet)
{
    m_strSpecialDynRef = strToSet;
    return m_strSpecialDynRef;
}


// Get a string object at the indicated index on the stack
const TString&
TCIDMacroEngine::strStackValAt( const   tCIDLib::TCard4     c4Index
                                , const tCIDLib::TBoolean   bCheckType) const
{
    const TMEngClassVal& mecvVal = mecsiAt(c4Index).mecvPushed();
    if (bCheckType)
        CheckItemType(mecvVal.c2ClassId(), tCIDMacroEng::EIntrinsics::String, CID_LINE, c4Index);
    return static_cast<const TMEngStringVal&>(mecvVal).strValue();
}


// Let's the application give us an output console stream to use
tCIDLib::TVoid TCIDMacroEngine::SetConsole(TTextOutStream* const pstrmToUse)
{
    // We don't own these console streams, so just overwrite any previous
    m_pstrmConsole = pstrmToUse;
}


// Let's the application give us a debug handler
tCIDLib::TVoid
TCIDMacroEngine::SetDbgHandler(MMEngDebugIntf* const pmedbgToUse)
{
    // We don't own these console streams, so just overwrite any previous
    m_pmedbgToUse = pmedbgToUse;
}


tCIDLib::TVoid
TCIDMacroEngine::SetErrHandler(MMEngErrHandler* const pmeehToUse)
{
    // We don't own these console streams, so just overwrite any previous
    m_pmeehToUse = pmeehToUse;
}


tCIDLib::TVoid
TCIDMacroEngine::SetException(  const   tCIDLib::TCard2 c2ErrClassId
                                , const TString&        strSrcClassPath
                                , const tCIDLib::TCard4 c4ErrorNum
                                , const TString&        strErrorName
                                , const TString&        strErrorText
                                , const tCIDLib::TCard4 c4LineNum)
{
    // If we've not created the object yet, then do it
    if (!m_pmecvThrown)
        m_pmecvThrown = new TMEngExceptVal(L"$Exception$", tCIDMacroEng::EConstTypes::Const);

    // Set up the exception info
    m_pmecvThrown->Set
    (
        c2ErrClassId
        , strSrcClassPath
        , c4ErrorNum
        , strErrorName
        , strErrorText
        , c4LineNum
    );

    //
    //  If we have a handler, and the exception reporting style is to report
    //  at the point of throw, let it know about this one now.
    //
    if (m_pmeehToUse && (m_eExceptReport == tCIDMacroEng::EExceptReps::AtThrow))
        m_pmeehToUse->MacroException(*m_pmecvThrown, *this);
}


tCIDLib::TVoid
TCIDMacroEngine::SetFileResolver(MMEngFileResolver* const pmefrToUse)
{
    // We don't own these, so just overwrite any previous
    m_pmefrToUse = pmefrToUse;
}


//
//  When the TTime class' Sleep method is called, if it's longer than a
//  second or so, then this method will be called and we'll do it in chunks
//  and call the debug interface each time, so that the debugger can get
//  us to stop if the user wants us to.
//
tCIDLib::TVoid TCIDMacroEngine::Sleep(const tCIDLib::TCard4 c4Millis)
{
    if ((c4Millis > 1500) && m_pmedbgToUse)
    {
        // Do 500ms at a time
        tCIDLib::TCard4 c4Count = 0;
        while (c4Count < c4Millis)
        {
            tCIDLib::TCard4 c4ThisTime = 500;
            if (c4Count + c4ThisTime > c4Millis)
                c4ThisTime = c4Millis - c4Count;
            c4Count += c4ThisTime;

            TThread::Sleep(c4ThisTime);
            if (!m_pmedbgToUse->bSleepTest())
                break;
        }
    }
     else
    {
        //
        //  Just do a regular sleep, though still using the thread specific
        //  one that will watch for lower level CIDLib thread stop requests.
        //
        TThread::pthrCaller()->bSleep(c4Millis);
    }
}


// Get the time object at the indicated position on the stack
const TTime&
TCIDMacroEngine::tmStackValAt(  const   tCIDLib::TCard4     c4Index
                                , const tCIDLib::TBoolean   bCheckType) const
{
    const TMEngClassVal& mecvVal = mecsiAt(c4Index).mecvPushed();
    if (bCheckType)
        CheckItemType(mecvVal.c2ClassId(), tCIDMacroEng::EIntrinsics::Time, CID_LINE, c4Index);
    return static_cast<const TMEngTimeVal&>(mecvVal).tmValue();
}


tCIDLib::TVoid TCIDMacroEngine::UnknownException()
{
    // If we have a handler, let it know about this one
    if (m_pmeehToUse)
        m_pmeehToUse->UnknownException(*this);
}


tCIDLib::TVoid
TCIDMacroEngine::ValidateCallFrame(         TMEngClassVal&  mecvInstance
                                    , const tCIDLib::TCard2 c2MethodId) const
{
    // The call stack obviously cannot be empty
    if (!m_c4CallStackTop)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_EmptyCallStack
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }

    // Get the class info object and method object
    const TMEngClassInfo&  meciTarget = meciFind(mecvInstance.c2ClassId());
    const TMEngMethodInfo& methiTarget = meciTarget.methiFind(c2MethodId);

    // The top item must be a call item
    const TMEngCallStackItem* pmecsiCur = m_colCallStack[m_c4CallStackTop - 1];
    if (pmecsiCur->eType() != tCIDMacroEng::EStackItems::MethodCall)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_BadCallFrame
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    //
    //  And now make sure that we have enough stack items to make a valid
    //  call frame for this method. It has to be one for the call itself,
    //  one for the return value, and the number of parms the method has.
    //
    const tCIDLib::TCard4 c4ParmCnt = methiTarget.c4ParmCount();
    if (m_c4CallStackTop < 2 + c4ParmCnt)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_BadCallFrame
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    // Check the return type
    const TMEngClassVal& mecvRet = mecvStackAt(m_c4CallStackTop - (c4ParmCnt + 2));
    if (methiTarget.c2RetClassId() != mecvRet.c2ClassId())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcMeth_RetType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , methiTarget.strName()
            , strClassPathForId(methiTarget.c2RetClassId())
            , strClassPathForId(mecvRet.c2ClassId())
        );
    }

    // Move down to the first parm and start working up
    for (tCIDLib::TCard2 c2Id = 0; c2Id < c4ParmCnt; c2Id++)
    {
        // Get the current parameter info object
        const TMEngParmInfo& mepiCur = methiTarget.mepiFind(c2Id);

        //
        //  And get the stack value for this object. Note that we add one
        //  because the stack top is actually pointing at the next available
        //  slot.
        //
        const TMEngClassVal& mecvCur = mecvStackAt
        (
            (m_c4CallStackTop - (c4ParmCnt + 1)) + c2Id
        );

        //
        //  Make sure that the value we have is of a class equal to or derived
        //  from the parameter class, or an equiv collection (in which the element
        //  type of the passed collection is at least derived from the element type
        //  of the parameter.
        //
        if (!bIsDerivedFrom(mecvCur.c2ClassId(), mepiCur.c2ClassId())
        &&  !bAreEquivCols(mepiCur.c2ClassId(), mecvCur.c2ClassId(), kCIDLib::False))
        {
            facCIDMacroEng().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcMeth_ParmType
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , methiTarget.strName()
                , TCardinal(c2Id + 1UL)
                , strClassPathForId(mepiCur.c2ClassId())
                , strClassPathForId(mecvCur.c2ClassId())
            );
        }
    }
}


// ---------------------------------------------------------------------------
//  TCIDMacroEngine: Protected, virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCIDMacroEngine::AdjustStart(TMEngClassVal&, TParmList&)
{
    // Default empty implementation
}


// ---------------------------------------------------------------------------
//  TCIDMacroEngine: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TCIDMacroEngine::CheckClassId(const tCIDLib::TCard2 c2Id
                            , const tCIDLib::TCard4 c4Line) const
{
    if (c2Id >= m_colClassesById.c4ElemCount())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , c4Line
            , kMEngErrs::errcEng_BadClassId
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , TCardinal(c2Id)
            , TCardinal(m_colClassesById.c4ElemCount())
        );
    }
}


tCIDLib::TVoid
TCIDMacroEngine::CheckNotEmpty(const tCIDLib::TCard4 c4Line) const
{
    if (!m_c4CallStackTop)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , c4Line
            , kMEngErrs::errcEng_EmptyCallStack
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
        );
    }
}


tCIDLib::TVoid TCIDMacroEngine::CheckStackExpand()
{
    //
    //  If our stack top indicates that there are unused items on the stack
    //  which have been allocated, then take one of those. Else, we need to
    //  allocate another one. If this goes over the current alloc, then we
    //  will cause it to expand it's allocation.
    //
    if (m_c4CallStackTop >= m_colCallStack.c4ElemCount())
        m_colCallStack.Add(new TMEngCallStackItem);
}


tCIDLib::TVoid
TCIDMacroEngine::CheckStackIndex(  const   tCIDLib::TCard4 c4Line
                                   , const tCIDLib::TCard4 c4ToCheck) const
{
    if (c4ToCheck >= m_c4CallStackTop)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , c4Line
            , kMEngErrs::errcEng_BadStackIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4ToCheck)
            , TCardinal(m_c4CallStackTop)
        );
    }
}


tCIDLib::TVoid
TCIDMacroEngine::CheckItemType( const   tCIDLib::TCard2 c2TypeId
                                , const tCIDLib::TCard2 c2ExpectedId
                                , const tCIDLib::TCard4 c4Line
                                , const tCIDLib::TCard4 c4Index) const
{
    if (c2TypeId != c2ExpectedId)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , c4Line
            , kMEngErrs::errcEng_WrongStackType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
            , strClassPathForId(c2ExpectedId)
            , TCardinal(c4Index)
            , strClassPathForId(c2TypeId)
        );
    }
}

tCIDLib::TVoid
TCIDMacroEngine::CheckItemType( const   tCIDLib::TCard2         c2TypeId
                                , const tCIDMacroEng::EIntrinsics  eType
                                , const tCIDLib::TCard4         c4Line
                                , const tCIDLib::TCard4         c4Index) const
{
    CheckItemType(c2TypeId, tCIDLib::TCard2(eType), c4Line, c4Index);
}


tCIDLib::TVoid
TCIDMacroEngine::CheckStackUnderflow(const tCIDLib::TCard4 c4Line) const
{
    if (!m_c4CallStackTop)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , c4Line
            , kMEngErrs::errcEng_CallStackUnderflow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Underflow
        );
    }
}


tCIDLib::TVoid
TCIDMacroEngine::FormatAFrame(          TTextOutStream& strmTarget
                                , const tCIDLib::TCard4 c4CallInd)
{
    // Dislay the return type, class name and method name
    const TMEngMethodInfo& methiCur = m_colCallStack[c4CallInd]->methiCalled();
    const TMEngClassInfo& meciCur = m_colCallStack[c4CallInd]->meciCalled();

    strmTarget  << meciCur.strClassPath() << kCIDLib::chPeriod
                << methiCur.strName()
                << kCIDLib::chOpenParen;

    //
    //  Handle the parameters now, if any. We have to go down to the first
    //  parameter and work our way back up.
    //
    tCIDLib::TCard4 c4ParmStack = c4CallInd - methiCur.c4ParmCount();
    tCIDLib::TCard2 c2ParmId = 0;
    while (c4ParmStack < c4CallInd)
    {
        // Get the description of the parameter and the type and value
        const TMEngParmInfo& mepiCur = methiCur.mepiFind(c2ParmId);
        const TMEngClassInfo& meciCur = *m_colClassesById[mepiCur.c2ClassId()];

        strmTarget  << mepiCur.eDir() << kCIDLib::chSpace
                    << meciCur.strClassPath() << kCIDLib::chSpace
                    << mepiCur.strName();

        c4ParmStack++;
        c2ParmId++;
        if (c4ParmStack < c4CallInd)
            strmTarget << kCIDLib::chComma;
    }

    // Close off the parms and put out the return type
    strmTarget  << kCIDLib::chCloseParen
                << L" Returns "
                << strClassPathForId(methiCur.c2RetClassId(), kCIDLib::False);
}


tCIDLib::TVoid TCIDMacroEngine::InitTempPool()
{
    const tCIDLib::TCard4 c4Count = m_colClasses.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        m_colTempPool.objAdd
        (
            TCntValList(new tCIDMacroEng::TClassValList(tCIDLib::EAdoptOpts::Adopt, 32))
        );
    }
}


//
//  These two methods provide a temp pool that is used by the methods above
//  that allow the caller to push temps onto the stack. The first one takes
//  a name, but generally it will just be an empty string since temps usually
//  don't need a name.
//
//  The second version just calls the first and passes that empty name.
//
TMEngClassVal*
TCIDMacroEngine::pmecvGet(  const   TString&                    strName
                            , const tCIDLib::TCard2             c2ClassId
                            , const tCIDMacroEng::EConstTypes   eConst)
{
    //
    //  Get the list for temp items with this class id. This is actually a
    //  counted pointer to the type list for this id.
    //
    TCntValList& cptrList = m_colTempPool[c2ClassId];

    //
    //  Get the actual counted collection out of it, and search this list for
    //  an available item.
    //
    tCIDMacroEng::TClassValList& colList = *cptrList;
    const tCIDLib::TCard4 c4Count = colList.c4ElemCount();
    tCIDLib::TCard4 c4Index;
    TMEngClassVal* pmecvRet = 0;
    for (c4Index = 0; c4Index < c4Count; c4Index++)
    {
        pmecvRet = colList[c4Index];

        if (!pmecvRet->bIsUsed())
            break;
    }

    //
    //  If we didn't find one, then create one and add it to this list. Else,
    //  force the passed const type on the one we took from the pool.
    //
    if (c4Index == c4Count)
    {
        pmecvRet = meciFind(c2ClassId).pmecvMakeStorage(strName, *this, eConst);
        colList.Add(pmecvRet);
    }
     else
    {
        pmecvRet->ForceConstType(eConst);
    }

    // Mark this one used, and return it
    pmecvRet->bIsUsed(kCIDLib::True);
    return pmecvRet;
}

TMEngClassVal*
TCIDMacroEngine::pmecvGet(  const   TString&                    strName
                            , const tCIDMacroEng::EIntrinsics   eType
                            , const tCIDMacroEng::EConstTypes   eConst)
{
    return pmecvGet(strName, tCIDLib::TCard2(eType), eConst);
}

TMEngClassVal*
TCIDMacroEngine::pmecvGet(  const   tCIDLib::TCard2             c2ClassId
                            , const tCIDMacroEng::EConstTypes   eConst)
{
    return pmecvGet(TString::strEmpty(), c2ClassId, eConst);
}

TMEngClassVal*
TCIDMacroEngine::pmecvGet(  const   tCIDMacroEng::EIntrinsics   eType
                            , const tCIDMacroEng::EConstTypes   eConst)
{
    return pmecvGet(TString::strEmpty(), tCIDLib::TCard2(eType), eConst);
}


//
//  We have to deal with a few circular dependencies here, and theintrinsics
//  MUST be added so that their ids match up with the EIntrinsicIds enum!
//
//  So, we do a deferred add of each of the intrinsic classes, then go back
//  and initalize them. This insures that none of their nested classes get
//  created until we have them all added. Otherwise, their nested
//  classes would get the indices out of sync.
//
tCIDLib::TVoid TCIDMacroEngine::RegisterBuiltInClasses()
{
    //
    //  Do a deferred add of all of the intrinsics, in the required order.
    //  This will just get them into the by name and by id lists and with
    //  the right ids. We can then go back and initialize them in their
    //  order of dependency.
    //
    TMEngClassInfo* pmeciObj = new TMEngObjectInfo(*this);
    c2AddClassDefer(pmeciObj);

    TMEngClassInfo* pmeciVoid = new TMEngVoidInfo(*this);
    c2AddClassDefer(pmeciVoid);

    TMEngClassInfo* pmeciOStrm = new TMEngTextOutStreamInfo(*this);
    c2AddClassDefer(pmeciOStrm);

    TMEngClassInfo* pmeciFmt = new TMEngFormattableInfo(*this);
    c2AddClassDefer(pmeciFmt);

    TMEngClassInfo* pmeciEnum = new TMEngEnumInfo(*this, L"Enum", L"MEng", L"MEng.Formattable", 1);
    c2AddClassDefer(pmeciEnum);

    TMEngClassInfo* pmeciBaseInfo = new TMEngBaseInfoInfo(*this);
    c2AddClassDefer(pmeciBaseInfo);

    TMEngClassInfo* pmeciBool = new TMEngBooleanInfo(*this);
    c2AddClassDefer(pmeciBool);

    TMEngClassInfo* pmeciChar = new TMEngCharInfo(*this);
    c2AddClassDefer(pmeciChar);

    TMEngClassInfo* pmeciStr = new TMEngStringInfo(*this);
    c2AddClassDefer(pmeciStr);

    TMEngClassInfo* pmeciCard1 = new TMEngCard1Info(*this);
    c2AddClassDefer(pmeciCard1);

    TMEngClassInfo* pmeciCard2 = new TMEngCard2Info(*this);
    c2AddClassDefer(pmeciCard2);

    TMEngClassInfo* pmeciCard4 = new TMEngCard4Info(*this);
    c2AddClassDefer(pmeciCard4);

    TMEngClassInfo* pmeciCard8 = new TMEngCard8Info(*this);
    c2AddClassDefer(pmeciCard8);

    TMEngClassInfo* pmeciFloat4 = new TMEngFloat4Info(*this);
    c2AddClassDefer(pmeciFloat4);

    TMEngClassInfo* pmeciFloat8 = new TMEngFloat8Info(*this);
    c2AddClassDefer(pmeciFloat8);

    TMEngClassInfo* pmeciInt1 = new TMEngInt1Info(*this);
    c2AddClassDefer(pmeciInt1);

    TMEngClassInfo* pmeciInt2 = new TMEngInt2Info(*this);
    c2AddClassDefer(pmeciInt2);

    TMEngClassInfo* pmeciInt4 = new TMEngInt4Info(*this);
    c2AddClassDefer(pmeciInt4);

    TMEngClassInfo* pmeciTime = new TMEngTimeInfo(*this);
    c2AddClassDefer(pmeciTime);

    TMEngClassInfo* pmeciStrList = new TMEngStrListInfo(*this);
    c2AddClassDefer(pmeciStrList);

    TMEngClassInfo* pmeciExcept = new TMEngExceptInfo(*this);
    c2AddClassDefer(pmeciExcept);

    TMEngClassInfo* pmeciMemBuf = new TMEngMemBufInfo(*this);
    c2AddClassDefer(pmeciMemBuf);

    TMEngClassInfo* pmeciStrOS = new TMEngStringOutStreamInfo(*this);
    c2AddClassDefer(pmeciStrOS);

    //
    //  Do the init phase, which must be done in order of derivation, so
    //  formattable before any of the formattable classes, and base text
    //  out stream before other text out streams, and so on.
    //
    pmeciObj->BaseClassInit(*this);
    pmeciObj->Init(*this);
    pmeciVoid->BaseClassInit(*this);
    pmeciVoid->Init(*this);

    pmeciFmt->BaseClassInit(*this);
    pmeciFmt->Init(*this);
    pmeciEnum->BaseClassInit(*this);
    pmeciEnum->Init(*this);
    pmeciBaseInfo->BaseClassInit(*this);
    pmeciBaseInfo->Init(*this);
    pmeciBool->BaseClassInit(*this);
    pmeciBool->Init(*this);
    pmeciChar->BaseClassInit(*this);
    pmeciChar->Init(*this);
    pmeciCard1->BaseClassInit(*this);
    pmeciCard1->Init(*this);
    pmeciCard2->BaseClassInit(*this);
    pmeciCard2->Init(*this);
    pmeciCard4->BaseClassInit(*this);
    pmeciCard4->Init(*this);
    pmeciCard8->BaseClassInit(*this);
    pmeciCard8->Init(*this);
    pmeciStr->BaseClassInit(*this);
    pmeciStr->Init(*this);
    pmeciFloat4->BaseClassInit(*this);
    pmeciFloat4->Init(*this);
    pmeciFloat8->BaseClassInit(*this);
    pmeciFloat8->Init(*this);
    pmeciInt1->BaseClassInit(*this);
    pmeciInt1->Init(*this);
    pmeciInt2->BaseClassInit(*this);
    pmeciInt2->Init(*this);
    pmeciInt4->BaseClassInit(*this);
    pmeciInt4->Init(*this);
    pmeciStrList->BaseClassInit(*this);
    pmeciStrList->Init(*this);
    pmeciExcept->BaseClassInit(*this);
    pmeciExcept->Init(*this);
    pmeciMemBuf->BaseClassInit(*this);
    pmeciMemBuf->Init(*this);
    pmeciOStrm->BaseClassInit(*this);
    pmeciOStrm->Init(*this);
    pmeciStrOS->BaseClassInit(*this);
    pmeciStrOS->Init(*this);

    //
    //  Do the base classes for the parameterized collection classes. These
    //  are not intrinsics, but need to be known at a fundamental level.
    //
    TMEngClassInfo* pmeciCol = new TMEngCollectInfo(*this);
    pmeciCol->BaseClassInit(*this);
    c2AddClass(pmeciCol);

    pmeciCol = new TMEngVectorInfo
    (
        *this
        , L"Vector"
        , L"MEng.System.Runtime"
        , L"MEng.System.Runtime.Collection"
        , 0
    );
    pmeciCol->BaseClassInit(*this);
    m_c2VectorId = c2AddClass(pmeciCol);


    // Intrinsic but DO AFTER vector above since it uses a vector parameter
    pmeciTime->BaseClassInit(*this);
    pmeciTime->Init(*this);


    //
    //  !!!! Make sure this one is last. The Cleanup() method uses this
    //  one's id to know where the built in classes end, so that it can
    //  remove all per-invocation classes but leave the built in ones
    //  alone.
    //
    pmeciCol = new TMEngArrayInfo
    (
        *this
        , L"Array"
        , L"MEng.System.Runtime"
        , L"MEng.System.Runtime.Collection"
        , 0
    );
    pmeciCol->BaseClassInit(*this);
    m_c2ArrayId = c2AddClass(pmeciCol);
}


