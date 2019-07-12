//
// FILE NAME: CIDMacroEng_ClassInfo.cpp
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
//  This file implements TMEngClassInfo class. This class represents a class
//  type in the macro system.
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
RTTIDecls(TMEngClassInfo,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TMEngClassInfo::IImportItem
// PREFIX: item
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngClassInfo::IImportItem: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngClassInfo::TImportItem::strKey(const TImportItem& itemSrc)
{
    return itemSrc.m_strImport;
}


// ---------------------------------------------------------------------------
//  TMEngClassInfo::IImportItem: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngClassInfo::TImportItem::TImportItem(const  TString&            strImport
                                        , const tCIDLib::TBoolean   bNested) :
    m_bNested(bNested)
    , m_strImport(strImport)
{
}

TMEngClassInfo::TImportItem::
TImportItem(const TMEngClassInfo::TImportItem& itemToCopy) :

    m_bNested(itemToCopy.m_bNested)
    , m_strImport(itemToCopy.m_strImport)
{
}

TMEngClassInfo::TImportItem::~TImportItem()
{
}

tCIDLib::TVoid
TMEngClassInfo::TImportItem::operator=(const TMEngClassInfo::TImportItem& itemToAssign)
{
    m_bNested   = itemToAssign.m_bNested;
    m_strImport = itemToAssign.m_strImport;
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngClassInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngClassInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngClassInfo::strKey(const TMEngClassInfo& meciSrc)
{
    // Our key is our class path
    return meciSrc.m_strClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngClassInfo: Destructor
// ---------------------------------------------------------------------------
TMEngClassInfo::~TMEngClassInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngClassInfo: Public, virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngClassInfo::bCanCastFrom(const tCIDLib::TCard2)
{
    // If htey don't override, then say we cannot do it
    return kCIDLib::False;
}


tCIDMacroEng::ECastRes
TMEngClassInfo::eCastFrom(TCIDMacroEngine&, const TMEngClassVal&,TMEngClassVal&)
{
    // If they don't override it, then say incompatible types
    return tCIDMacroEng::ECastRes::Incompatible;
}


//
//  This method is provided for those canned classes that are programmatically
//  built, mostly intrinsics, that are done as derived classes so that they
//  build their own contents.
//
tCIDLib::TVoid TMEngClassInfo::Init(TCIDMacroEngine&)
{
    // Default empty implementation
}



// ---------------------------------------------------------------------------
//  TMEngClassInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngClassInfo::AddDirective(const TString& strName, const TString& strValue)
{
    // Ignore duplicates
    if (!m_colDirectives.bKeyExists(strName))
        m_colDirectives.objAdd(TKeyValuePair(strName, strValue));
}


tCIDLib::TVoid TMEngClassInfo::AddLiteral(TMEngLiteralVal* const pmelvToAdopt)
{
    if (!m_colLiterals.bAddIfNew(pmelvToAdopt))
    {
        const TString strName(pmelvToAdopt->strName());
        delete pmelvToAdopt;

        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcClass_DupLiteral
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , strName
            , m_strClassPath
        );
    }
}


tCIDLib::TVoid
TMEngClassInfo::AddMethodImpl(TMEngMethodImpl* const pmethToAdopt)
{
    TJanitor<TMEngMethodImpl> janImpl(pmethToAdopt);

    // Make sure we have a method description for this method
    TMEngMethodInfo* pmethiTarget = pmethiFind(pmethToAdopt->strName());
    if (!pmethiTarget)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcClass_ImplWithoutInfo
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , pmethToAdopt->strName()
            , m_strClassPath
        );
    }

    // Make sure that they both have the same id, which should be the case
    if (pmethiTarget->c2Id() != pmethToAdopt->c2Id())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcClass_ImplInfoIdMatch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , m_strClassPath
            , pmethToAdopt->strName()
            , TCardinal(pmethToAdopt->c2Id())
            , TCardinal(pmethiTarget->c2Id())
        );
    }

    //
    //  Make sure we don't have another method at this level for the method
    //  id that this one says it represents.
    //
    if (pmethFind(pmethToAdopt->c2Id()))
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcClass_DupMethod
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , pmethToAdopt->strName()
            , m_strClassPath
        );
    }

    // Add it to the method implementation list
    m_colMethodImpls.Add(janImpl.pobjOrphan());
}


tCIDLib::TBoolean TMEngClassInfo::bAddClassImport(const TString& strClassPath)
{
    //
    //  We are tolerant of multiple imports of the same class, since it doesn't
    //  hurt anything. In theory the derived implementation would check and
    //  issue a warning during compilation if that is desired, but we just do
    //  the right thing silently.
    //
    tCIDLib::TBoolean bAdded;
    m_colImports.objAddIfNew(TImportItem(strClassPath, kCIDLib::False), bAdded);
    return bAdded;
}


tCIDLib::TBoolean TMEngClassInfo::bAddNestedType(const TString& strClassPath)
{
    // Add this marked as a nested type
    tCIDLib::TBoolean bAdded;
    m_colImports.objAddIfNew(TImportItem(strClassPath, kCIDLib::True), bAdded);
    return bAdded;
}


tCIDLib::TBoolean
TMEngClassInfo::bCheckDupName(          TCIDMacroEngine&    meOwner
                                , const TString&            strToCheck) const
{
    // Check members and methods first
    if (m_colMembers.bKeyExists(strToCheck) || m_colMethods.bKeyExists(strToCheck))
        return kCIDLib::True;

    //
    //  Oh well, try literals, which are inherited so we have to search the
    //  whole hierarchy up from this class.
    //
    if (pmelvFind(strToCheck, meOwner, kCIDLib::True))
        return kCIDLib::True;
    return kCIDLib::False;
}


tCIDLib::TBoolean
TMEngClassInfo::bFindDirective( const   TString&    strToFind
                                ,       TString&    strToFill)
{
    TKeyValuePair* pkvalDir = m_colDirectives.pobjFindByKey(strToFind);
    if (!pkvalDir)
        return kCIDLib::False;
    strToFill = pkvalDir->strValue();
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMEngClassInfo::bHasMember( const   TString&            strNameToCheck
                            , const tCIDLib::TBoolean   bThisClassOnly) const
{
    //
    //  If the want to check any member in our inheritance chain, we can
    //  just do the simple scenario.
    //
    if (!bThisClassOnly)
        return m_colMembers.bKeyExists(strNameToCheck);

    //
    //  Look up the member. If not found, then obviously not. If we do
    //  find it, make sure it's id is one that indicates it is one of
    //  the members of this class, not an ancestor class.
    //
    const TMEngMemberInfo* pmemiFind = m_colMembers.pobjFindByKey(strNameToCheck);
    if (!pmemiFind)
        return kCIDLib::False;
    return (pmemiFind->c2Id() >= m_c2FirstMemberId);
}


tCIDLib::TBoolean
TMEngClassInfo::bHasMethod(const TString& strNameToCheck) const
{
    return m_colMethods.bKeyExists(strNameToCheck);
}


tCIDLib::TVoid TMEngClassInfo::BaseClassInit(TCIDMacroEngine& meOwner)
{
    // Store the parent class id and inherit members and methods
    if (m_strParentClassPath == kCIDMacroEng_::pszNoParentClass)
        m_c2ParentClassId = kMacroEng::c2BadId;
    else
        m_c2ParentClassId = meOwner.c2FindClassId(m_strParentClassPath);

    //
    //  If this isn't the root object class, then we need to inherit all of
    //  our parent class' method info objects and members, since we inherit
    //  all his methods and members, and this way our method and members ids
    //  start at the correct place when we add them at our level
    //
    if (m_c2ParentClassId != kMacroEng::c2BadId)
    {
        //
        //  The index of the first member and method of our own will be the
        //  count of those in our parent class.
        //
        const TMEngClassInfo& meciParent = meOwner.meciFind(m_c2ParentClassId);
        m_c2FirstMemberId = tCIDLib::TCard2(meciParent.c4MemberCount());
        m_c2FirstMethodId = tCIDLib::TCard2(meciParent.c4MethodCount());

        for (tCIDLib::TCard2 c2Id = 0; c2Id < m_c2FirstMethodId; c2Id++)
            c2AddMethodInfo(meciParent.methiFind(c2Id));

        for (tCIDLib::TCard2 c2Id = 0; c2Id < m_c2FirstMemberId; c2Id++)
        {
            const TMEngMemberInfo& memiCur = meciParent.memiFind(c2Id);
            c2AddMember(memiCur, meOwner.meciFind(memiCur.c2ClassId()));
        }

        //
        //  If our parent class is not copyable, then we cannot be copyable
        //  so clear our flag. If our parent is, we could still not be (but
        //  only in the case of C++ wrapper classes that have control of
        //  that), so we don't just blindly take our parent class' status.
        //
        //  Even if we are currently copyable and our parent is, too, the
        //  later addition of members could make us uncopyable.
        //
        if (m_bCopyable && !meciParent.m_bCopyable)
            m_bCopyable = kCIDLib::False;
    }
}


tCIDLib::TCard2
TMEngClassInfo::c2AddMember(const   TMEngMemberInfo&    memiToAdd
                            , const TMEngClassInfo&     meciNew)
{
    // Add if it not already. If it is already, then throw an error
    tCIDLib::TBoolean bAdded;
    TMEngMemberInfo& memiNew = m_colMembers.objAddIfNew(memiToAdd, bAdded);
    if (!bAdded)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcClass_DupMember
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , memiToAdd.strName()
            , m_strClassPath
        );
    }

    facCIDMacroEng().CheckIdOverflow(m_c2NextMemberId, L"class members");

    // If we are copyable, then see if the new member changes this
    if (m_bCopyable)
        m_bCopyable = meciNew.m_bCopyable;

    // Add it to the by id list
    m_colMembersById.Add(&memiNew);

    // We kept it, so set it's stamp to our next available var stamp id
    return memiNew.c2Id(m_c2NextMemberId++);
}


tCIDLib::TCard2
TMEngClassInfo::c2AddMethodInfo(const TMEngMethodInfo& methiToAdd)
{
    // Add if it not already. If it is already, then throw an error
    tCIDLib::TBoolean bAdded;
    TMEngMethodInfo& methiNew = m_colMethods.objAddIfNew(methiToAdd, bAdded);
    if (!bAdded)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcClass_DupMethod
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , methiToAdd.strName()
            , m_strClassPath
        );
    }

    facCIDMacroEng().CheckIdOverflow(m_c2NextMethodId, L"class methods");

    // Add it to the by id list
    m_colMethodsById.Add(&methiNew);

    // We kept it, so set it's stamp to our next available proc stamp id
    const tCIDLib::TCard2 c2NewId = m_c2NextMethodId++;
    methiNew.c2Id(c2NewId);

    //
    //  If it is a default ctor, then remember it's id. If we already have
    //  one, then throw.
    //
    if (methiNew.bIsCtor() && !methiNew.c4ParmCount())
    {
        if ((m_c2DefCtorId != kMacroEng::c2BadId)
        &&  (m_c2DefCtorId >= m_c2FirstMethodId))
        {
            facCIDMacroEng().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcClass_DupDefCtor
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Already
                , m_strClassPath
            );
        }

        // Not a dup, so store it
        m_c2DefCtorId = c2NewId;
    }

    // Return the id of the new method
    return c2NewId;
}


tCIDLib::TCard2
TMEngClassInfo::c2FindMember(const  TString&            strToFind
                            , const tCIDLib::TBoolean   bThisClassOnly) const
{
    const TMEngMemberInfo* pmemiRet = m_colMembers.pobjFindByKey(strToFind);
    if (!pmemiRet)
        return kMacroEng::c2BadId;
    if (bThisClassOnly && (pmemiRet->c2Id() < m_c2FirstMemberId))
        return kMacroEng::c2BadId;
    return pmemiRet->c2Id();
}


tCIDLib::TCard2 TMEngClassInfo::c2FindMethod(const TString& strToFind) const
{
    const TMEngMethodInfo* pmethiRet = m_colMethods.pobjFindByKey(strToFind);
    if (!pmethiRet)
        return kMacroEng::c2BadId;
    return pmethiRet->c2Id();
}


tCIDLib::TCard2 TMEngClassInfo::c2FirstMemberId() const
{
    return m_c2FirstMemberId;
}


tCIDLib::TCard2 TMEngClassInfo::c2FirstMethodId() const
{
    return m_c2FirstMethodId;
}


tCIDLib::TCard2 TMEngClassInfo::c2Id() const
{
    #if CID_DEBUG_ON
    if (m_c2Id == kMacroEng::c2BadId)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcClass_NotRegistered
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , m_strClassPath
        );
    }
    #endif

    return m_c2Id;
}


tCIDLib::TCard2 TMEngClassInfo::c2MethodId(const TString& strToFind) const
{
    const TMEngMethodInfo* pmethiRet = m_colMethods.pobjFindByKey(strToFind);
    if (!pmethiRet)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcClass_MethodNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strToFind
            , m_strClassPath
        );
    }
    return pmethiRet->c2Id();
}


tCIDLib::TCard2 TMEngClassInfo::c2MemberId(const TString& strToFind) const
{
    const TMEngMemberInfo* pmemiRet = m_colMembers.pobjFindByKey(strToFind);
    if (!pmemiRet)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcClass_MemberNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strToFind
            , m_strClassPath
        );
    }
    return pmemiRet->c2Id();
}


tCIDLib::TCard2 TMEngClassInfo::c2ParentClassId() const
{
    return m_c2ParentClassId;
}


tCIDLib::TCard4
TMEngClassInfo::c4GetCtorParmList(tCIDMacroEng::TParmIdTable& pitToFill) const
{
    //
    //  We will loop through the methods of this class. For each one that is
    //  a constructor, we'll load up an item for each ctor we see.
    //
    //  NOTE:   We only want constructors defined for this class, not it's
    //          ancestor classes. So we start on our first method id.
    //
    pitToFill.c4Count = 0;
    const tCIDLib::TCard4 c4Count = m_colMethodsById.c4ElemCount();
    for (tCIDLib::TCard2 c2Id = m_c2FirstMethodId; c2Id < c4Count; c2Id++)
    {
        // Get the current method, and if not a Ctor, we don't care for it
        const TMEngMethodInfo* pmethiCur = m_colMethodsById[c2Id];
        if (!pmethiCur->bIsCtor())
            continue;

        // Get the current item we are going to fill in
        tCIDMacroEng::TParmIdItem& piiCur = pitToFill.apiiMethods[pitToFill.c4Count];

        // Set it's count to the parm count of this ctor, and store the id
        piiCur.c4Count = pmethiCur->c4ParmCount();
        piiCur.c2MethodId = pmethiCur->c2Id();
        piiCur.bDiscarded = kCIDLib::False;

        // And loop through this ctor's parms, storing type info
        for (tCIDLib::TCard2 c2Index = 0; c2Index < piiCur.c4Count; c2Index++)
            piiCur.ac2Parms[c2Index] = pmethiCur->mepiFind(c2Index).c2ClassId();

        pitToFill.c4Count++;
    }
    return pitToFill.c4Count;
}


tCIDLib::TCard4 TMEngClassInfo::c4CtorCount() const
{
    tCIDLib::TCard4 c4RetCount = 0;

    const tCIDLib::TCard4 c4Count = m_colMethodsById.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TMEngMethodInfo* pmethiCur = m_colMethodsById[c4Index];
        if (pmethiCur->bIsCtor())
            c4RetCount++;
    }
    return c4RetCount;
}


// Provide access to our literals list via a const cursor
const TMEngClassInfo::TLiteralList::TCursor TMEngClassInfo::cursLiterals() const
{
    return TLiteralList::TCursor(&m_colLiterals);
}


tCIDMacroEng::EClassExt TMEngClassInfo::eExtend() const
{
    return m_eExtend;
}

tCIDMacroEng::EClassExt TMEngClassInfo::eExtend(const tCIDMacroEng::EClassExt eToSet)
{
    m_eExtend = eToSet;
    return m_eExtend;
}


//
//  This guy will export ourself, and any of our ancestor classes, and any
//  of their nested classes, and any of those nested class' ancestor
//  classes and their nested classes, and so on, into the target class, i.e.
//  add import statements to this class for them. This is done because in CML
//  import a class implicitly imports those other classes. So basically the
//  classes imported are those directly imported, plus the transitive closure
//  of it's ancestors and their nested classes.
//
//  We want to go to the root class (Object), and start working upwards, so
//  that we get the original attributes of the class, not those of the class
//  as imported into the derived classes. So we recurse first down the chain
//  and then unwind and do the exports on the way up.
//
//  On the way up, we add an import, if that class level hasn't already been
//  imported, and then if that class has any nested classes, we also recurse
//  on it. So it's a two dimensional recursion.
//
tCIDLib::TVoid
TMEngClassInfo::ExportTo(       TCIDMacroEngine&    meOwner
                        ,       TMEngClassInfo&     meciTarget) const
{
    // If we aren't MEng.Object, then recurse down again
    if (m_c2Id != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Object))
        meOwner.meciFind(m_c2ParentClassId).ExportTo(meOwner, meciTarget);

    //
    //  Ok, we've hit the bottom already, and how we are unwinding back up
    //  and doing our imports.
    //
    //  Export ourself to the target class. If we aren't already in his list,
    //  then it will return true to say it was added, in which case we have
    //  to export our nested types.
    //
    if (meciTarget.bAddClassImport(m_strClassPath) && !m_colImports.bIsEmpty())
    {
        TImportList::TCursor cursImports(&m_colImports);
        for (; cursImports; ++cursImports)
        {
            // If a nested class, look it up and recurse on it
            const TImportItem& itemCur = *cursImports;
            if (itemCur.m_bNested)
                meOwner.meciFind(itemCur.m_strImport).ExportTo(meOwner, meciTarget);
        }
    }
}


tCIDLib::TVoid
TMEngClassInfo::Invoke(         TCIDMacroEngine&        meOwner
                        ,       TMEngClassVal&          mecvInstance
                        , const tCIDLib::TCard2         c2MethodId
                        , const tCIDMacroEng::EDispatch    eDispatch) const
{
    //
    //  Validate the call frame if we were asked to. Normally, this should
    //  have been dealt with by the compiler so it should always be valid,
    //  because the op codes would have been generated to do so, but we need
    //  to be able to validate it for debugging purposes.
    //
    if (meOwner.bValidation())
        meOwner.ValidateCallFrame(mecvInstance, c2MethodId);

    //
    //  Ok, the parms are good, so we have to dispatch it. If the dispatch
    //  flag is mono, then this is a direct call to this class, so we just
    //  start off the search here. Else, we start with the actual class of
    //  the instance value. We then work down the inheritance chain till we
    //  find someone who handles it, or we fail.
    //
    tCIDLib::TCard2 c2ClassId;

    if (eDispatch == tCIDMacroEng::EDispatch::Mono)
        c2ClassId = m_c2Id;
    else
        c2ClassId = mecvInstance.c2ClassId();
    while (c2ClassId != kMacroEng::c2BadId)
    {
        TMEngClassInfo& meciCur = meOwner.meciFind(c2ClassId);
        const TMEngMethodInfo& methiTarget = meciCur.methiFind(c2MethodId);
        if (meciCur.bInvokeMethod(meOwner, methiTarget, mecvInstance))
            break;

        // Move down the inheritance chain
        c2ClassId = meciCur.c2ParentClassId();
    }

    // If we didn't find it, that's a problem
    if (c2ClassId == kMacroEng::c2BadId)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcClass_MethodNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , TCardinal(c2MethodId)
            , m_strClassPath
        );
    }
}


const TMEngLiteralVal*
TMEngClassInfo::pmelvFind(  const   TString&            strName
                            ,       TCIDMacroEngine&    meOwner
                            , const tCIDLib::TBoolean   bRecurse) const
{
    const TMEngLiteralVal* pmelvRet;

    //
    //  Search our literals first. If found, return that. Else, if we were
    //  told to recurse, then do it.
    //
    pmelvRet = m_colLiterals.pobjFindByKey(strName, kCIDLib::False);
    if (pmelvRet || !bRecurse)
        return pmelvRet;

    //
    //  Let's search the inheritance change to see if any ancestor class
    //  has this literal.
    //
    const TMEngClassInfo* pmeciCur = this;
    while (kCIDLib::True)
    {
        // If we are at MEng.Object, then we are done
        if (pmeciCur->c2ParentClassId() == kMacroEng::c2BadId)
            break;

        // Else, move up the hierarchy
        pmeciCur = &meOwner.meciFind(pmeciCur->c2ParentClassId());

        //
        //  Tell this one not to recurse, since we are doing it here
        //  iteratively.
        //
        pmelvRet = pmeciCur->pmelvFind(strName, meOwner, kCIDLib::False);
        if (pmelvRet)
            return pmelvRet;
    }
    return nullptr;
}


const TMEngMemberInfo*
TMEngClassInfo::pmemiFind(  const   TString&            strName
                            , const tCIDLib::TBoolean   bThisClassOnly) const
{
    const TMEngMemberInfo* pmemiRet = m_colMembers.pobjFindByKey(strName);
    if (!pmemiRet || (bThisClassOnly && (pmemiRet->c2Id() < m_c2FirstMemberId)))
        return nullptr;
    return pmemiRet;
}

TMEngMemberInfo*
TMEngClassInfo::pmemiFind(  const   TString&            strName
                            , const tCIDLib::TBoolean   bThisClassOnly)
{
    TMEngMemberInfo* pmemiRet = m_colMembers.pobjFindByKey(strName);
    if (!pmemiRet || (bThisClassOnly && (pmemiRet->c2Id() < m_c2FirstMemberId)))
        return nullptr;
    return pmemiRet;
}


const TMEngMethodImpl* TMEngClassInfo::pmethDefCtor() const
{
    // The default ctor id is stored specially
    if (m_c2DefCtorId == kMacroEng::c2BadId)
        return nullptr;

    // Looks like we have one, so get the info object
    const TMEngMethodInfo* pmethiDef = m_colMethodsById[m_c2DefCtorId];

    // Find the implementation for this method
    const tCIDLib::TCard2 c2Id = pmethiDef->c2Id();
    const tCIDLib::TCard4 c4ICount = m_colMethodImpls.c4ElemCount();
    for (tCIDLib::TCard4 c4IIndex = 0; c4IIndex < c4ICount; c4IIndex++)
    {
        const TMEngMethodImpl* pmethCur = m_colMethodImpls[c4IIndex];
        if (pmethCur->c2Id() == c2Id)
            return pmethCur;
    }

    // Never found one
    return nullptr;
}


const TMEngMethodInfo* TMEngClassInfo::pmethiDefCtor() const
{
    // The default ctor id is stored specially
    if (m_c2DefCtorId == kMacroEng::c2BadId)
        return nullptr;

    // Looks like we have one, so return the info object
    return m_colMethodsById[m_c2DefCtorId];
}


const TMEngMethodImpl*
TMEngClassInfo::pmethFind(const tCIDLib::TCard2 c2Id) const
{
    //
    //  Just bute force it and search for an implementation object that claims
    //  to be for this method id.
    //
    const tCIDLib::TCard4 c4Count = m_colMethodImpls.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colMethodImpls[c4Index]->c2Id() == c2Id)
            return m_colMethodImpls[c4Index];
    }
    return nullptr;
}

TMEngMethodImpl* TMEngClassInfo::pmethFind(const tCIDLib::TCard2 c2Id)
{
    //
    //  Just bute force it and search for an implementation object that claims
    //  to be for this method id.
    //
    const tCIDLib::TCard4 c4Count = m_colMethodImpls.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colMethodImpls[c4Index]->c2Id() == c2Id)
            return m_colMethodImpls[c4Index];
    }
    return nullptr;
}


const TMEngMethodImpl* TMEngClassInfo::pmethFind(const TString& strName) const
{
    //
    //  Just bute force it and search for an implementation object that claims
    //  to be for this method name.
    //
    const tCIDLib::TCard4 c4Count = m_colMethodImpls.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colMethodImpls[c4Index]->strName() == strName)
            return m_colMethodImpls[c4Index];
    }
    return nullptr;
}

TMEngMethodImpl* TMEngClassInfo::pmethFind(const TString& strName)
{
    //
    //  Just bute force it and search for an implementation object that claims
    //  to be for this method name.
    //
    const tCIDLib::TCard4 c4Count = m_colMethodImpls.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colMethodImpls[c4Index]->strName() == strName)
            return m_colMethodImpls[c4Index];
    }
    return nullptr;
}


tCIDLib::TVoid TMEngClassInfo::SetToNested(const TString& strClassPath)
{
    // Find this one and mark it as nested
    TImportItem& itmUpdate = m_colImports.objFindByKey(strClassPath);
    itmUpdate.m_bNested = kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngClassInfo: Hidden Constructors
// ---------------------------------------------------------------------------

//
//  We don't know the parent class id yet. It'll be passed to us when we
//  get called on BaseClassInit(). At that point, we'll set the parent class
//  id, and inherit our parent's members and methods.
//
TMEngClassInfo::TMEngClassInfo( const   TString&                strName
                                , const TString&                strBasePath
                                ,       TCIDMacroEngine&
                                , const tCIDLib::TBoolean       bCopyable
                                , const tCIDMacroEng::EClassExt    eExtend
                                , const TString&                strParentClassPath) :
    m_bCopyable(bCopyable)
    , m_c2DefCtorId(kMacroEng::c2BadId)
    , m_c2FirstMemberId(0)
    , m_c2FirstMethodId(0)
    , m_c2Id(kMacroEng::c2BadId)
    , m_c2NextMemberId(0)
    , m_c2NextMethodId(0)
    , m_c2ParentClassId(0)
    , m_colDirectives(29, TStringKeyOps(), &TKeyValuePair::strExtractKey)
    , m_colImports(29, TStringKeyOps(), &TImportItem::strKey)
    , m_colLiterals
      (
        tCIDLib::EAdoptOpts::Adopt
        , 29
        , TStringKeyOps()
        , &TMEngLiteralVal::strKey
      )
    , m_colMembers(29, TStringKeyOps(), &TMEngMemberInfo::strKey)
    , m_colMembersById(tCIDLib::EAdoptOpts::NoAdopt, 16)
    , m_colMethods(29, TStringKeyOps(), &TMEngMethodInfo::strKey)
    , m_colMethodsById(tCIDLib::EAdoptOpts::NoAdopt, 16)
    , m_colMethodImpls(tCIDLib::EAdoptOpts::Adopt, 16)
    , m_eExtend(eExtend)
    , m_strBasePath(strBasePath)
    , m_strClassPath(TStrCat(strBasePath, kCIDLib::chPeriod, strName))
    , m_strName(strName)
    , m_strParentClassPath(strParentClassPath)
{
}

TMEngClassInfo::TMEngClassInfo( const   TString&                strName
                                , const TString&                strBasePath
                                ,       TCIDMacroEngine&
                                , const tCIDLib::TBoolean       bCopyable
                                , const tCIDMacroEng::EClassExt eExtend
                                , const tCIDLib::TCh* const     pszParentClassPath) :
    m_bCopyable(bCopyable)
    , m_c2DefCtorId(kMacroEng::c2BadId)
    , m_c2FirstMemberId(0)
    , m_c2FirstMethodId(0)
    , m_c2Id(kMacroEng::c2BadId)
    , m_c2NextMemberId(0)
    , m_c2NextMethodId(0)
    , m_c2ParentClassId(0)
    , m_colDirectives(29, TStringKeyOps(), &TKeyValuePair::strExtractKey)
    , m_colImports(29, TStringKeyOps(), &TImportItem::strKey)
    , m_colLiterals
      (
        tCIDLib::EAdoptOpts::Adopt, 29, TStringKeyOps(), &TMEngLiteralVal::strKey
      )
    , m_colMembers(29, TStringKeyOps(), &TMEngMemberInfo::strKey)
    , m_colMembersById(tCIDLib::EAdoptOpts::NoAdopt, 16)
    , m_colMethods(29, TStringKeyOps(), &TMEngMethodInfo::strKey)
    , m_colMethodsById(tCIDLib::EAdoptOpts::NoAdopt, 16)
    , m_colMethodImpls(tCIDLib::EAdoptOpts::Adopt, 16)
    , m_eExtend(eExtend)
    , m_strBasePath(strBasePath)
    , m_strClassPath(TStrCat(strBasePath, kCIDLib::chPeriod, strName))
    , m_strName(strName)
    , m_strParentClassPath(pszParentClassPath)
{
}


// ---------------------------------------------------------------------------
//  TMEngClassInfo: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngClassInfo::LoadMemberVals( TMEngClassVal&      mecvToLoad
                                , TCIDMacroEngine&  meOwner) const
{
    const tCIDLib::TCard4 c4Count = m_colMembers.c4ElemCount();

    //
    //  Loop through our defined members and create new storage for each
    //  one and add them to the new instance.
    //
    for (tCIDLib::TCard2 c2Id = 0; c2Id < c4Count; c2Id++)
    {
        // Get the current member info object
        const TMEngMemberInfo& memiCur = memiFind(c2Id);

        // Get the class info object for the type of this variable
        TMEngClassInfo& meciFactory = meOwner.meciFind(memiCur.c2ClassId());

        //
        //  Ask it to create an instance value for us, and add this guy to
        //  the new value object's list. If this variable has an initial
        //  value, then set it.
        //
        TMEngClassVal* pmecvNew = meciFactory.pmecvMakeStorage
        (
            memiCur.strName(), meOwner, memiCur.eConst()
        );

        // Assign it it's member id
        pmecvNew->c2Id(c2Id);

        // And it's ready so add it to the new instance
        mecvToLoad.AddMember(pmecvNew);
    }
}


tCIDLib::TVoid TMEngClassInfo::SetClassId(const tCIDLib::TCard2 c2ToSet)
{
    m_c2Id = c2ToSet;
}


// ---------------------------------------------------------------------------
//  TEngClassInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngClassInfo::CheckMethodId(  const   tCIDLib::TCard2 c2Id
                                , const tCIDLib::TCard4 c4Line) const
{
    if (c2Id >= m_colMethodsById.c4ElemCount())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , c4Line
            , kMEngErrs::errcClass_BadMethodId
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , TCardinal(c2Id)
            , m_strClassPath
            , TCardinal(m_colMethodsById.c4ElemCount())
        );
    }
}


tCIDLib::TVoid
TMEngClassInfo::CheckMemberId(  const   tCIDLib::TCard2 c2Id
                                , const tCIDLib::TCard4 c4Line) const
{
    if (c2Id >= m_colMembersById.c4ElemCount())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , c4Line
            , kMEngErrs::errcClass_BadMemberId
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , TCardinal(c2Id)
            , m_strClassPath
            , TCardinal(m_colMembersById.c4ElemCount())
        );
    }
}


