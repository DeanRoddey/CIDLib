//
// FILE NAME: CIDMacroEng_Parser3.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/31/2003
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
//  This file handles some of the grunt work methods of the parser class, to
//  keep the main file mostly oriented towards the high level parsing logic.
//  This one handles the code to parse methods. This is far and away the most
//  complex part of the parsing system, since it has to generate opcodes and
//  handle recursive descent parsing and such.
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
#include    "CIDMacroEng_ParserSrc_.hpp"


// ---------------------------------------------------------------------------
//  TMacroEngParser: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This method expects to be at the start of an object constructor (the only
//  methods in this language that can be overloaded), just past the open
//  paren. It will parse the parameters, and find the overloaded version that
//  best fits the parms.
//
//  It expects that the return value storage has been pushed. It will push the
//  parameters onto the stack, and the caller must push the call and the stack
//  cleanup.
//
//  It returns kCIDLib::c2MaxCard if no match is found.
//
tCIDLib::TCard2
TMacroEngParser::c2FindCorrectCtor( TParserSrc&             psrcClass
                                    , TMEngClassInfo&       meciTarget
                                    , TMEngClassInfo&       meciToFill
                                    , TMEngMethodInfo&      methiTarget
                                    , TMEngOpMethodImpl&    methToFill
                                    , tCIDLib::TCard4&      c4ParmCnt)
{
    // Get the ctor parameter list for the target class
    if (!meciTarget.c4GetCtorParmList(m_pitResData))
    {
        //
        //  This should not be possible, since all classes must have at
        //  least a default ctor.
        //
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcPrs_NoCtors
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , meciTarget.strClassPath()
        );
    }

    //
    //  Now we have to parse all of the initializer expressions. As we
    //  go, we look for a constructor that could take the types that are
    //  returned.
    //
    tCIDLib::TCard4         c4CIndex;
    tCIDMacroEng::EExprTypes   eExprType;

    c4ParmCnt = 0;
    while (kCIDLib::True)
    {
        // Break out on the close paren
        if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::CloseParen))
            break;

        // If not the first one we need a comma
        if (c4ParmCnt)
            bCheckComma(psrcClass);

        // Get the next expression and store the type. It must be a RHS expr
        tCIDLib::TBoolean bDummy;
        const tCIDLib::TCard2 c2Id = c2ParseExpr
        (
            psrcClass
            , tCIDMacroEng::EExprSides::RHS
            , meciToFill
            , methiTarget
            , methToFill
            , eExprType
            , bDummy
        );

        //
        //  Go through the constructors at this level and see if any
        //  would allow for the type that we got back. We skip any that
        //  have already been discarded as non-matches.
        //
        for (c4CIndex = 0; c4CIndex < m_pitResData.c4Count; c4CIndex++)
        {
            tCIDMacroEng::TParmIdItem& piiCur = m_pitResData.apiiMethods[c4CIndex];
            if (piiCur.bDiscarded)
                continue;

            // If this one has run out of defined parms, it can't match
            if (c4ParmCnt >= piiCur.c4Count)
            {
                piiCur.bDiscarded = kCIDLib::True;
                continue;
            }

            // If the expr is the the same or derived, just keep it
            const tCIDLib::TCard2 c2TargetId = piiCur.ac2Parms[c4ParmCnt];
            if (m_pmeTarget->bIsDerivedFrom(c2Id, c2TargetId))
                continue;

            //
            //  If we got a numeric literal value, then let's see if
            //  we can force it to fit by converting the type we saw
            //  or guessed (since they aren't even typed unless they have
            //  a suffix) into the type needed without a range issue.
            //
            if (eExprType == tCIDMacroEng::EExprTypes::NumLiteral)
            {
                TMEngOpCode& meopParm = methToFill.meopLast();
                if (!meopParm.bConvertNumeric(c2TargetId))
                    piiCur.bDiscarded = kCIDLib::True;
            }
             else
            {
                //
                //  Before we give up, see if they are equiv collections with
                //  identical element types.
                //
                if (!m_pmeTarget->bAreEquivCols(c2Id, c2TargetId, kCIDLib::True))
                    piiCur.bDiscarded = kCIDLib::True;
            }
        }
        c4ParmCnt++;
    }

    //
    //  Go through the list of ctors and discard the ones that don't have
    //  equal numbers of parms, since the above checks wouldn't have gotten
    //  all of those that take more parms than we got.
    //
    tCIDLib::TCard4 c4ToUse = kCIDLib::c4MaxCard;
    tCIDLib::TCard4 c4ActiveCount = 0;
    for (c4CIndex = 0; c4CIndex < m_pitResData.c4Count; c4CIndex++)
    {
        tCIDMacroEng::TParmIdItem& piiCur = m_pitResData.apiiMethods[c4CIndex];
        if (piiCur.bDiscarded)
            continue;

        if (piiCur.c4Count != c4ParmCnt)
        {
            piiCur.bDiscarded = kCIDLib::True;
            continue;
        }

        c4ToUse = c4CIndex;
        c4ActiveCount++;
    }

    //
    //  Now, if we have more than one, then it's an ambiguous call, so
    //  issue an error and just take the 0th one in order to keep going.
    //  If we have zero, then no match was available.
    //
    if (c4ActiveCount > 1)
        IssueErr(psrcClass, kMEngErrs::errcPrs_AmbiguousOverload);

    if (c4ToUse == kCIDLib::c4MaxCard)
        return kCIDLib::c2MaxCard;
    return m_pitResData.apiiMethods[c4ToUse].c2MethodId;
}



tCIDLib::TCard2
TMacroEngParser::c2ParseCallSequence(       TParserSrc&             psrcClass
                                    , const TMEngClassInfo&         meciTarget
                                    , const TMEngMethodInfo&        methiTarget
                                    ,       TMEngOpMethodImpl&      methToFill
                                    ,       tCIDMacroEng::EConstTypes& eConst
                                    , const tCIDMacroEng::ERefTypes    eRefType
                                    , const TMEngClassInfo&         meciRef
                                    , const tCIDLib::TCard2         c2RefId
                                    ,       tCIDMacroEng::EExprTypes&  eTypeParsed)
{
    //
    //  This gets reset as we go to the class of the object we are currently
    //  working on. Start it on the incoming class.
    //
    const TMEngClassInfo* pmeciCurRef = &meciRef;

    // Get a string from the pool to use
    TMEngStrPoolJan janTmp(m_pmsplParse);
    TString& strTmp = janTmp.strThis();

    //
    //  We loop until we hit the end of the sequence of method calls and/or
    //  indexing operations. On initial call, some object reference was parsed,
    //  and a period or open bracket was seen. We handle that call or index
    //  op, and if it is followed with a period or open bracket, we go again
    //  and again. All we can see in this sequence are method calls or index
    //  ops against whatever the previous returned object type was.
    //
    //  By the time we unwind, the ref type, const, and expression type will
    //  represent that eventual end object that was returned. Obviously, no
    //  method other than the last one can have a void return value, and even
    //  that might be illegal, but only the caller knows that.
    //
    tCIDLib::TCard4 c4SeqDepth = 0;
    while (kCIDLib::True)
    {
        if (((eRefType == tCIDMacroEng::ERefTypes::ThisCall) && !c4SeqDepth)
        ||  psrcClass.bIfPeeked(tCIDMacroEng::ETokens::Period))
        {
            //
            //  And now we should see a method or literal name. Check for
            //  the literal first. Note that literals are inherited, so we
            //  have to search from the initial class up the inheritance
            //  chain.
            //
            const tCIDMacroEng::ETokens eTok = psrcClass.eGetNextToken(strTmp);
            if (eTok != tCIDMacroEng::ETokens::NoMatch)
            {
                IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedDotName);
                ThrowUnrecoverable();
            }

            //
            //  First try the literal. If we find one, then let's take that
            //  and we are done, becuase the sequence cannot go any further.
            //
            const TMEngLiteralVal* pmelvTarget = meciRef.pmelvFind
            (
                strTmp
                , *m_pmeTarget
                , kCIDLib::True
            );

            if (pmelvTarget)
            {
                eTypeParsed = pmelvTarget->ePushImmediate
                (
                    methToFill
                    , m_eOptLevel >= tCIDMacroEng::EOptLevels::Medium
                );
                eConst = tCIDMacroEng::EConstTypes::Const;
                return pmelvTarget->c2ClassId();
            }

            // Not a literal, so try a method
            const TMEngMethodInfo* pmethiCall = pmeciCurRef->pmethiFind(strTmp);

            if (!pmethiCall)
            {
                IssueErr
                (
                    psrcClass
                    , kMEngErrs::errcClass_MethodNotFound
                    , strTmp
                    , pmeciCurRef->strName()
                );
                ThrowUnrecoverable();
            }

            // Push the return value
            m_meopToUse.SetSingleIndex
            (
                tCIDMacroEng::EOpCodes::PushTempVar
                , pmethiCall->c2RetClassId()
            );
            methToFill.c4AddOpCode(m_meopToUse);

            //
            //  If the method is non-const, we have to see if the target
            //  allows for a non-const call.
            //
            if (!pmethiCall->bIsConst())
            {
                //
                //  If the target object being called on is const, it's bad.
                //  This will include [In] parameters, even if the object
                //  actually passed via that parameter is non-const. The
                //  resolution mechanism will set the const flag for any in
                //  parms.
                //
                if (eConst == tCIDMacroEng::EConstTypes::Const)
                {
                    IssueErr
                    (
                        psrcClass
                        , kMEngErrs::errcPrs_MemberIsConst
                        , pmethiCall->strName()
                    );
                }

                //
                //  If the method we are filling is const, then the containing
                //  method must be const if it is on a member or this.
                //
                if (((eRefType == tCIDMacroEng::ERefTypes::Member) || (eRefType == tCIDMacroEng::ERefTypes::This))
                &&  methiTarget.bIsConst())
                {
                    IssueErr
                    (
                        psrcClass
                        , kMEngErrs::errcPrs_MethodIsConst
                        , pmethiCall->strName()
                    );
                }
            }

            // Now we should see the open paren of the method call
            bCheckOpenParen(psrcClass);

            // For each parameter the method defines, parse out an expression
            const tCIDLib::TCard4 c4ParmCount = pmethiCall->c4ParmCount();
            for (tCIDLib::TCard2 c2ParmId = 0; c2ParmId < c4ParmCount; c2ParmId++)
            {
                if (c2ParmId)
                    bCheckComma(psrcClass);

                // Get the parm info for this one
                const TMEngParmInfo& mepiCur = pmethiCall->mepiFind(c2ParmId);

                //
                //  And now recurse back into the main expression parser
                //  method. These must all be RHS expressions.
                //
                tCIDLib::TBoolean bDummy;
                tCIDMacroEng::EExprTypes eExprType;
                const tCIDLib::TCard2 c2PClassId = c2ParseExpr
                (
                    psrcClass
                    , tCIDMacroEng::EExprSides::RHS
                    , meciTarget
                    , methiTarget
                    , methToFill
                    , eExprType
                    , bDummy
                );

                //
                //  If this parameter is not In only, then then only thing we
                //  can accept is a non-const object reference. Everything else,
                //  is something that cannot be updated.
                //
                if ((mepiCur.eDir() != tCIDMacroEng::EParmDirs::In)
                &&  (eExprType != tCIDMacroEng::EExprTypes::ObjectRef))
                {
                    facCIDMacroEng().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kMEngErrs::errcPrs_InParmOnly
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Internal
                        , TCardinal(c2ParmId + 1UL)
                    );
                }

                //
                //  The expression must be the same or derived from the class
                //  that the parameter expects, or an equivalent collection.
                //
                if (!m_pmeTarget->bIsDerivedFrom(c2PClassId, mepiCur.c2ClassId()))
                {
                    //
                    //  If we got a numeric literal value, then let's see if
                    //  we can force it to fit by converting the type we saw
                    //  or guessed (since they aren't even typed unless they
                    //  have a suffix) into the type needed without a range
                    //  issue.
                    //
                    tCIDLib::TBoolean bOk = kCIDLib::False;
                    if (eExprType == tCIDMacroEng::EExprTypes::NumLiteral)
                    {
                        TMEngOpCode& meopParm = methToFill.meopLast();
                        if (meopParm.bConvertNumeric(mepiCur.c2ClassId()))
                            bOk = kCIDLib::True;
                    }

                    //
                    //  If not good so far, see if they are equiv collections.
                    //  In this case, say it's ok as long as the element type of
                    //  of the passed one derives from the element type of the
                    //  parameter (they don't have to be identical.)
                    //
                    if (!bOk)
                    {
                        bOk = m_pmeTarget->bAreEquivCols
                        (
                            mepiCur.c2ClassId(), c2PClassId, kCIDLib::False
                        );
                    }

                    if (!bOk)
                    {
                        // This one is bad, so issue an error
                        IssueErr
                        (
                            psrcClass
                            , kMEngErrs::errcMeth_ParmType
                            , pmethiCall->strName()
                            , TCardinal(c2ParmId + 1UL)
                            , m_pmeTarget->strClassPathForId(mepiCur.c2ClassId())
                            , m_pmeTarget->strClassPathForId(c2PClassId)
                        );
                    }
                }
            }

            // We have the parm pushing opcodes, so check for the close paren
            bCheckCloseParen(psrcClass);

            //
            //  Now generate the opcode to call the method. We have to use
            //  a different opcode according to the type of object. If this
            //  is a gang call (i.e. we aren't the first method call in this
            //  sequence and so are calling a method on a previously returned
            //  value), then we have to call against an object on the stack,
            //  and we have to get the return value
            //
            if (c4SeqDepth)
            {
                //
                //  The first index is how far down the stack the target object
                //  is. It will be the parm count, plus the return, plus one
                //  to get to the value.
                //
                m_meopToUse.SetDoubleIndex
                (
                    tCIDMacroEng::EOpCodes::CallStack
                    , tCIDLib::TCard2(c4ParmCount + 2)
                    , pmethiCall->c2Id()
                );
            }
             else if (eRefType == tCIDMacroEng::ERefTypes::Local)
            {
                m_meopToUse.SetDoubleIndex
                (
                    tCIDMacroEng::EOpCodes::CallLocal
                    , c2RefId
                    , pmethiCall->c2Id()
                );
            }
             else if (eRefType == tCIDMacroEng::ERefTypes::Parm)
            {
                m_meopToUse.SetDoubleIndex
                (
                    tCIDMacroEng::EOpCodes::CallParm
                    , c2RefId
                    , pmethiCall->c2Id()
                );
            }
             else if (eRefType == tCIDMacroEng::ERefTypes::Member)
            {
                m_meopToUse.SetDoubleIndex
                (
                    tCIDMacroEng::EOpCodes::CallMember
                    , c2RefId
                    , pmethiCall->c2Id()
                );
            }
             else if (eRefType == tCIDMacroEng::ERefTypes::Parent)
            {
                m_meopToUse.SetSingleIndex
                (
                    tCIDMacroEng::EOpCodes::CallParent
                    , pmethiCall->c2Id()
                );
            }
             else if (eRefType == tCIDMacroEng::ERefTypes::This)
            {
                m_meopToUse.SetSingleIndex
                (
                    tCIDMacroEng::EOpCodes::CallThis
                    , pmethiCall->c2Id()
                );
            }
             else if (eRefType == tCIDMacroEng::ERefTypes::Exception)
            {
                m_meopToUse.SetSingleIndex
                (
                    tCIDMacroEng::EOpCodes::CallExcept
                    , pmethiCall->c2Id()
                );
            }
             else if (eRefType == tCIDMacroEng::ERefTypes::ThisCall)
            {
                m_meopToUse.SetSingleIndex
                (
                    tCIDMacroEng::EOpCodes::CallThis
                    , pmethiCall->c2Id()
                );
            }
             else
            {
                // We are throwing a CIDLib error
                facCIDLib().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcGen_BadEnumValue
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Internal
                    , TInteger(tCIDLib::i4EnumOrd(eRefType))
                    , TString(L"tCIDMacroEng::ERefTypes")
                );
            }
            methToFill.c4AddOpCode(m_meopToUse);

            //
            //  Generate the pops to clean up, leaving the return on the stack.
            //  Since the called method removes the call item, we just have to
            //  remove any parms we pushed for the call.
            //
            if (c4ParmCount)
            {
                //
                //  NOTE:   We are LEAVING the return on the stack here so
                //          just pop the parms!
                //
                m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::MultiPop, c4ParmCount);
                methToFill.c4AddOpCode(m_meopToUse);
            }

            //
            //  If this is a ganged call, then we have to deal with the fact
            //  that the original object is still on the stack behind the
            //  return value. So issue a flip command then a pop.
            //
            if (c4SeqDepth)
            {
                m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::FlipTop);
                methToFill.c4AddOpCode(m_meopToUse);
                m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::PopTop);
                methToFill.c4AddOpCode(m_meopToUse);
            }

            //
            //  The return type of the method becomes the type for the next
            //  target call, if any.
            //
            pmeciCurRef = &m_pmeTarget->meciFind(pmethiCall->c2RetClassId());

            //
            //  We assume that this scenario creates a const object ref, since
            //  it will be a copy of a parameter, and therefore only const
            //  methods should be callable against it.
            //
            eTypeParsed = tCIDMacroEng::EExprTypes::ConstObjectRef;
            eConst = tCIDMacroEng::EConstTypes::Const;
        }
         else if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::OpenBracket))
        {
            //
            //  It's got to be an index operation. So the target object must
            //  be something derived from one of the indexable collection
            //  base classes.
            //
            tCIDLib::TCard2 c2ElemId;
            if (!m_pmeTarget->bIsIndexableClass(pmeciCurRef->c2ParentClassId()))
            {
                IssueErr(psrcClass, kMEngErrs::errcPrs_NotIndexableObj);
                ThrowUnrecoverable();
            }

            c2ElemId = static_cast<const TMEngColBaseInfo*>(pmeciCurRef)->c2ElemId();

            //
            //  If this is the first time through, then we have to push the
            //  original object, else the target object is already on the top
            //  of the stack.
            //
            if (!c4SeqDepth)
            {
                if (eRefType == tCIDMacroEng::ERefTypes::Local)
                {
                    m_meopToUse.SetSingleIndex(tCIDMacroEng::EOpCodes::PushLocal, c2RefId);
                }
                 else if (eRefType == tCIDMacroEng::ERefTypes::Parm)
                {
                    m_meopToUse.SetSingleIndex(tCIDMacroEng::EOpCodes::PushParm, c2RefId);
                }
                 else if (eRefType == tCIDMacroEng::ERefTypes::Member)
                {
                    m_meopToUse.SetSingleIndex(tCIDMacroEng::EOpCodes::PushMember, c2RefId);
                }
                 else if (eRefType == tCIDMacroEng::ERefTypes::This)
                {
                    m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::PushThis);
                }
                 else
                {
                    // We are throwing a CIDLib error
                    facCIDLib().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kCIDErrs::errcGen_BadEnumValue
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Internal
                        , TInteger(tCIDLib::i4EnumOrd(eRefType))
                        , TString(L"tCIDMacroEng::ERefTypes")
                    );
                }
                methToFill.c4AddOpCode(m_meopToUse);
            }

            //
            //  Next we must have an expression that evaluates to either a
            //  Card4 or enumerated value (for collections that take a numeric
            //  index), or a String (for collections that take a text index.)
            //  Clearly it must be a RHS expression.
            //
            tCIDLib::TBoolean       bDummy;
            tCIDMacroEng::EExprTypes   eDummy;
            const tCIDLib::TCard2 c2IndexId = c2ParseExpr
            (
                psrcClass
                , tCIDMacroEng::EExprSides::RHS
                , meciTarget
                , methiTarget
                , methToFill
                , eDummy
                , bDummy
            );

            // Check it for a legal type, but keep going if bad
            if ((c2IndexId != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card4))
            &&  !m_pmeTarget->bIsDerivedFrom(c2IndexId, tCIDMacroEng::EIntrinsics::Enum))
            {
                IssueErr(psrcClass, kMEngErrs::errcRT_NumericIndexType);
            }

            //
            //  Ok, the collection and index type are both on the stack, so
            //  issue the collection index opcode.
            //
            m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::ColIndex);
            methToFill.c4AddOpCode(m_meopToUse);

            // And we have to see the closing bracket
            bCheckCloseBracket(psrcClass);

            //
            //  The return type of the index op becomes element type of the
            //  collection. The const type should be from the vector, since
            //  if the collection was const, then the object we get out of it
            //  is also, and vice versa. So we leave it untouched.
            //
            pmeciCurRef = &m_pmeTarget->meciFind(c2ElemId);
        }
         else
        {
            break;
        }

        // We must see another in the sequence at this point
        c4SeqDepth++;
    }

    // The return is the type of the last thing returned
    return pmeciCurRef->c2Id();
}


//
//  At the fundamental level, everything in the body of the method comes down to two
//  things, expressions or flow control statements (if, while, etc...) Expressions are
//  either method calls or operators being applied in some way. This method is called
//  when an expression needs to be parsed. We return to the caller the type of the result
//  of the expression, so that he can evaluate whether it's appropriate for what is needed.
//
//  This method is recursive, since any expression can itself contains other nested
//  expressions. We keep up with the nesting level via a parameter that gets bump for
//  each new recursion. Except at level 0, all expressions must return a non-void return
//  type because they are only being called due to their being used as a value in another
//  expression. So, except for level 0, we expect every expression to return, having
//  generated the opcodes to get it's resulting value onto the top of stack.
//
//  Since only copyable objects can be returned from methods, all of the items pushed
//  are either immediate numerical values, or they are temp pool items. So each
//  expression evaluation will generate the opcode to push a value of their expected
//  type from the temp pool, then to get their resulting value into that pushed object.
//
//  Once we get into here, we cannot see any flow control statements. They are only
//  allowed at the top level parsing method, which is what calls us. We are only called
//  to evaluate expressions, and they can't ever contain any conditional code.
//
//  We have to be passed the method being filled, and the info on the method, and the
//  info on the class, since at every recursion we have to resolve names to either a
//  method local, a method parameter, or a class member.
//
//  NOTE:   We use the temp parse string because of our recursive nature. We
//          don't want to have to create and destroy them constantly. So this
//          means that we have to use the contents immediately before any
//          further parsing or recursion!
//
tCIDLib::TCard2
TMacroEngParser::c2ParseFactor(         TParserSrc&             psrcClass
                                , const tCIDMacroEng::EExprSides   eSide
                                , const TMEngClassInfo&         meciTarget
                                , const TMEngMethodInfo&        methiTarget
                                ,       TMEngOpMethodImpl&      methToFill
                                ,       tCIDMacroEng::EExprTypes&  eTypeParsed
                                ,       tCIDLib::TBoolean&      bDirectRef)
{
    tCIDLib::TCard2     c2RetId = 0;

    // Set some assumptions until proven otherwise
    bDirectRef  = kCIDLib::False;
    eTypeParsed = tCIDMacroEng::EExprTypes::Unknown;

    // Get a couple strings from the pool to use
    TMEngStrPoolJan janToken(m_pmsplParse);
    TString& strToken = janToken.strThis();
    TMEngStrPoolJan janTmp(m_pmsplParse);
    TString& strTmp = janTmp.strThis();

    //
    //  We have to figure out what kind of expression we are looking at
    //  here, which is going to require a little look ahead. We know it's
    //  an expression of some sort or we wouldn't be here. But, it can be
    //  a method call or some some sort of constant value or some sort of
    //  mathematical or logical expression.
    //
    tCIDMacroEng::ETokens eTok = psrcClass.eGetNextToken(strToken);

    if (eTok == tCIDMacroEng::ETokens::CurClassName)
    {
        // Check that this is legal for the side we are parsing
        CheckRHS(psrcClass, eSide);

        //
        //  Just put out an opcode to push the current class. We'll have to
        //  put a string on the string pool for this.
        //
        const tCIDLib::TCard2 c2PoolId = methToFill.c2AddString
        (
            meciTarget.strClassPath()
            , (m_eOptLevel >= tCIDMacroEng::EOptLevels::Medium)
        );

        m_meopToUse.SetSingleIndex
        (
            tCIDMacroEng::EOpCodes::PushStrPoolItem
            , c2PoolId
        );
        methToFill.c4AddOpCode(m_meopToUse);
        c2RetId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::String);
        eTypeParsed = tCIDMacroEng::EExprTypes::StringLiteral;
    }
     else if (eTok == tCIDMacroEng::ETokens::CurLine)
    {
        // Check that this is legal for the side we are parsing
        CheckRHS(psrcClass, eSide);

        // Just put out an opcode to push the current line
        m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::PushCurLine);
        methToFill.c4AddOpCode(m_meopToUse);
        c2RetId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card4);
        eTypeParsed = tCIDMacroEng::EExprTypes::CurLine;
    }
     else if (eTok == tCIDMacroEng::ETokens::Exception)
    {
        // Check that this is legal for the side we are parsing
        CheckRHS(psrcClass, eSide);

        // Make sure that we in a catch block, otherwise this isn't legal
        if (!m_bInCatch)
            IssueErr(psrcClass, kMEngErrs::errcPrs_OnlyInCatch);

        //
        //  Check to see if it's just a reference to the object, or a
        //  call on that object. If a call, then parse the call sequence.
        //
        eTok = psrcClass.ePeekNextToken();
        if (eTok == tCIDMacroEng::ETokens::Period)
        {
            tCIDMacroEng::EConstTypes  eConst = tCIDMacroEng::EConstTypes::Const;
            tCIDMacroEng::ERefTypes    eRefType = tCIDMacroEng::ERefTypes::Exception;

            c2RetId = c2ParseCallSequence
            (
                psrcClass
                , meciTarget
                , methiTarget
                , methToFill
                , eConst
                , eRefType
                , m_pmeTarget->meciFind(tCIDMacroEng::EIntrinsics::Exception)
                , 0
                , eTypeParsed
            );
        }
         else
        {
            // Just put out an opcode to push the magic exception object
            m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::PushException);
            methToFill.c4AddOpCode(m_meopToUse);

            // And set the return info appropriately
            c2RetId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Exception);
            eTypeParsed = tCIDMacroEng::EExprTypes::ConstObjectRef;
            bDirectRef = kCIDLib::True;
        }
    }
     else if (eTok == tCIDMacroEng::ETokens::False)
    {
        // Check that this is legal for the side we are parsing
        CheckRHS(psrcClass, eSide);

        // Just push a boolean constant immediate
        m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::PushImBoolean, kCIDLib::False);
        methToFill.c4AddOpCode(m_meopToUse);
        c2RetId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Boolean);
        eTypeParsed = tCIDMacroEng::EExprTypes::BoolLiteral;
    }
     else if (eTok == tCIDMacroEng::ETokens::Parent)
    {
        //
        //  It's a call to the parent class, so we have to have a period
        //  next.
        //
//        bCheckPeriod(psrcClass);

        // If the method is const, then we are const
        tCIDMacroEng::EConstTypes eConst;
        if (methiTarget.bIsConst())
        {
            eTypeParsed = tCIDMacroEng::EExprTypes::ConstObjectRef;
            eConst = tCIDMacroEng::EConstTypes::Const;
        }
         else
        {
            eTypeParsed = tCIDMacroEng::EExprTypes::ObjectRef;
            eConst = tCIDMacroEng::EConstTypes::NonConst;
        }

        // Say it's not a direct ref
        bDirectRef = kCIDLib::False;

        // And parse the method call
        c2RetId = c2ParseCallSequence
        (
            psrcClass
            , meciTarget
            , methiTarget
            , methToFill
            , eConst
            , tCIDMacroEng::ERefTypes::Parent
            , meciTarget
            , 0
            , eTypeParsed
        );
    }
     else if ((eTok == tCIDMacroEng::ETokens::NoMatch)
          ||  (eTok == tCIDMacroEng::ETokens::This))
    {
        //
        //  It's got to be an object, so we need to figure out what type
        //  of object, the class type of target object, the method called,
        //  then recurse for it's parms, if any. So first resolve the name
        //  as a local reference.
        //
        tCIDMacroEng::EConstTypes  eConst = tCIDMacroEng::EConstTypes::NonConst;
        tCIDLib::TCard2         c2Id = 0;
        tCIDMacroEng::ERefTypes    eRefType = tCIDMacroEng::ERefTypes::Unknown;
        const TMEngClassInfo*   pmeciTarget = 0;
        const TMEngLiteralVal*  pmelvTarget = 0;

        if (eTok == tCIDMacroEng::ETokens::This)
        {
            //
            //  If the target method is const, then 'this' is const within it.
            //  We don't care what the id is in this case, because we'll use
            //  an opcode that indicates a call on the current object.
            //
            eConst = methiTarget.eConst();
            c2Id = 0;
            eRefType = tCIDMacroEng::ERefTypes::This;
            pmeciTarget = &meciTarget;
        }
         else
        {
            pmeciTarget = pmeciResolveRef
            (
                psrcClass
                , strToken
                , meciTarget
                , methiTarget
                , methToFill
                , pmelvTarget
                , eConst
                , c2Id
                , eRefType
            );
        }

        // Couldn't resolve it, so give up
        if (eRefType == tCIDMacroEng::ERefTypes::Unknown)
        {
            IssueErr(psrcClass, kMEngErrs::errcPrs_BadNameRef, strToken);
            ThrowUnrecoverable();
        }

        //
        //  If it was an enum constant or literal member, then handle those.
        //  Else, it's got to be some sort of object reference.
        //
        if (eRefType == tCIDMacroEng::ERefTypes::EnumConst)
        {
            // Check that this is legal for the side we are parsing
            CheckRHS(psrcClass, eSide);

            // The return type becomes the enum class id
            c2RetId = pmeciTarget->c2Id();

            //
            //  Generate the code to push a temp of this type from the
            //  temp pool. Ordinals are 4 byte, but we have to clip it to
            //  2 bytes to get into an opcode, which limits them actually
            //  to 16K values, but that's infinitely more than needed.
            //
            //  The first index is the class type and the second is the
            //  ordinal value. It was sent back in the c2Id field.
            //
            m_meopToUse.SetDoubleIndex(tCIDMacroEng::EOpCodes::PushEnum, c2RetId, c2Id);
            methToFill.c4AddOpCode(m_meopToUse);
            eTypeParsed = tCIDMacroEng::EExprTypes::EnumValue;
            bDirectRef  = kCIDLib::False;
        }
         else if (eRefType == tCIDMacroEng::ERefTypes::LitMember)
        {
            // Check that this is legal for the side we are parsing
            CheckRHS(psrcClass, eSide);

            eTypeParsed = pmelvTarget->ePushImmediate
            (
                methToFill
                , m_eOptLevel >= tCIDMacroEng::EOptLevels::Medium
            );
            c2RetId = pmelvTarget->c2ClassId();
            bDirectRef = kCIDLib::False;
        }
         else
        {
            //
            //  If the next token is either a period or an open square bracket,
            //  or the ref type is a 'this call', then we have a method call
            //  or an indexing operation. Else, it is just the object itself
            //  that is the value, and we are done. If it's one of the first
            //  set, then we call another method which can handle going down
            //  a sequence of calls, which return object refs, which might
            //  be in turn called or indexed, and so forth.
            //
            eTok = psrcClass.ePeekNextToken();
            if ((eRefType == tCIDMacroEng::ERefTypes::ThisCall)
            ||  ((eTok == tCIDMacroEng::ETokens::Period)
            ||   (eTok == tCIDMacroEng::ETokens::OpenBracket)))
            {
                //
                //  If it's an implicit call to this, then we have to do a
                //  little work first, since this is a special case.
                //
                if (eRefType == tCIDMacroEng::ERefTypes::ThisCall)
                {
                    //
                    //  Push the name back, since the method call stuff
                    //  below will expect to see it.
                    //
                    psrcClass.PushBack(tCIDMacroEng::ETokens::NoMatch, strToken);

                    // If the method is const, then we are const
                    if (methiTarget.bIsConst())
                        eTypeParsed = tCIDMacroEng::EExprTypes::ConstObjectRef;
                    else
                        eTypeParsed = tCIDMacroEng::EExprTypes::ObjectRef;

                    // Say it's not a direct ref
                    bDirectRef = kCIDLib::False;

                    // And the target class is our current class
                    pmeciTarget = &meciTarget;
                }
                 else if (eTok == tCIDMacroEng::ETokens::OpenBracket)
                {
                    // It will be a direct ref since it's the element indexed
                    bDirectRef = kCIDLib::True;

                    //
                    //  And it will be a const or non-const object ref,
                    //  according to whether the indexed object was const
                    //  or not.
                    //
                    if (eConst == tCIDMacroEng::EConstTypes::Const)
                        eTypeParsed = tCIDMacroEng::EExprTypes::ConstObjectRef;
                    else
                        eTypeParsed = tCIDMacroEng::EExprTypes::ObjectRef;
                }

                c2RetId = c2ParseCallSequence
                (
                    psrcClass
                    , meciTarget
                    , methiTarget
                    , methToFill
                    , eConst
                    , eRefType
                    , *pmeciTarget
                    , c2Id
                    , eTypeParsed
                );
            }
             else
            {
                // The return type becomes the type of object
                c2RetId = pmeciTarget->c2Id();

                if (eRefType == tCIDMacroEng::ERefTypes::Local)
                {
                    m_meopToUse.SetSingleIndex(tCIDMacroEng::EOpCodes::PushLocal, c2Id);
                    if (methToFill.meliFind(c2Id).eConst() == tCIDMacroEng::EConstTypes::Const)
                        eTypeParsed = tCIDMacroEng::EExprTypes::ConstObjectRef;
                    else
                        eTypeParsed = tCIDMacroEng::EExprTypes::ObjectRef;
                    bDirectRef  = kCIDLib::True;
                }
                 else if (eRefType == tCIDMacroEng::ERefTypes::Parm)
                {
                    m_meopToUse.SetSingleIndex(tCIDMacroEng::EOpCodes::PushParm, c2Id);

                    // See if the parm is const or not and set the type
                    if (methiTarget.mepiFind(c2Id).eDir() == tCIDMacroEng::EParmDirs::In)
                        eTypeParsed = tCIDMacroEng::EExprTypes::ConstObjectRef;
                    else
                        eTypeParsed = tCIDMacroEng::EExprTypes::ObjectRef;
                    bDirectRef  = kCIDLib::True;
                }
                 else if (eRefType == tCIDMacroEng::ERefTypes::Member)
                {
                    m_meopToUse.SetSingleIndex(tCIDMacroEng::EOpCodes::PushMember, c2Id);

                    // See if the member is const or not and set the type
                    if (meciTarget.memiFind(c2Id).bIsConst())
                        eTypeParsed = tCIDMacroEng::EExprTypes::ConstObjectRef;
                    else
                        eTypeParsed = tCIDMacroEng::EExprTypes::ObjectRef;
                    bDirectRef  = kCIDLib::True;
                }
                 else if (eRefType == tCIDMacroEng::ERefTypes::This)
                {
                    m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::PushThis);

                    //
                    //  If the current method is const, then this object is
                    //  const within it.
                    //
                    if (methiTarget.bIsConst())
                        eTypeParsed = tCIDMacroEng::EExprTypes::ConstObjectRef;
                    else
                        eTypeParsed = tCIDMacroEng::EExprTypes::ObjectRef;
                    bDirectRef  = kCIDLib::True;
                }
                 else
                {
                    // We are throwing a CIDLib error
                    facCIDLib().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kCIDErrs::errcGen_BadEnumValue
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Internal
                        , TInteger(tCIDLib::i4EnumOrd(eRefType))
                        , TString(L"tCIDMacroEng::ERefTypes")
                    );
                }
                methToFill.c4AddOpCode(m_meopToUse);
            }
        }
    }
     else if (eTok == tCIDMacroEng::ETokens::NumericLiteral)
    {
        // Check that this is legal for the side we are parsing
        CheckRHS(psrcClass, eSide);

        //
        //  Figure out what kind of numeric literal, and then issue the
        //  opcode to push an immediate numeric value. This allows literal
        //  numerics to be generated directly into the opcode, instead of
        //  requring a temp local.
        //
        if (!bPushNumericLiteral(psrcClass, strToken, methToFill, c2RetId))
        {
            IssueErr(psrcClass, kMEngErrs::errcPrs_BadNumLiteral, strToken);

            // Push a bogus constant so we can move forward
            m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::PushImCard4, tCIDLib::TCard4(0));
            methToFill.c4AddOpCode(m_meopToUse);
        }
        eTypeParsed = tCIDMacroEng::EExprTypes::NumLiteral;
    }
     else if (eTok == tCIDMacroEng::ETokens::OpenParen)
    {
        // Check that this is legal for the side we are parsing
        CheckRHS(psrcClass, eSide);

        //
        //  If we see an open paren, then this is some nested expression so we
        //  need to recurse immediately, and then look for the closing paren
        //  when we get back.
        //
        tCIDLib::TBoolean bDummy;
        c2RetId = c2ParseExpr
        (
            psrcClass
            , tCIDMacroEng::EExprSides::RHS
            , meciTarget
            , methiTarget
            , methToFill
            , eTypeParsed
            , bDummy
        );
        bCheckCloseParen(psrcClass);
    }
     else if (eTok == tCIDMacroEng::ETokens::CharLiteral)
    {
        // Check that this is legal for the side we are parsing
        CheckRHS(psrcClass, eSide);

        // Escape the literal if needed
        const tCIDLib::TCh chLit = psrcClass.chEscape(strToken);

        // We can push this one as an immediate value
        m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::PushImChar, chLit);
        methToFill.c4AddOpCode(m_meopToUse);
        c2RetId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Char);
        eTypeParsed = tCIDMacroEng::EExprTypes::CharLiteral;
    }
     else if (eTok == tCIDMacroEng::ETokens::QuotedString)
    {
        // Check that this is legal for the side we are parsing
        CheckRHS(psrcClass, eSide);

        // Escape the string
        psrcClass.EscapeStr(strToken);

        //
        //  We can't use a literal immediate mode for strings, so we have to
        //  create a string pool entry and generate the code to push it.
        //
        const tCIDLib::TCard2 c2PoolId = methToFill.c2AddString
        (
            strToken
            , m_eOptLevel == tCIDMacroEng::EOptLevels::Medium
        );
        m_meopToUse.SetSingleIndex
        (
            tCIDMacroEng::EOpCodes::PushStrPoolItem
            , c2PoolId
        );
        methToFill.c4AddOpCode(m_meopToUse);
        c2RetId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::String);
        eTypeParsed = tCIDMacroEng::EExprTypes::StringLiteral;
    }
     else if (eTok == tCIDMacroEng::ETokens::True)
    {
        // Check that this is legal for the side we are parsing
        CheckRHS(psrcClass, eSide);

        // Just push a boolean constant immediate
        m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::PushImBoolean, kCIDLib::True);
        methToFill.c4AddOpCode(m_meopToUse);
        c2RetId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Boolean);
        eTypeParsed = tCIDMacroEng::EExprTypes::BoolLiteral;
    }
     else if (eTok == tCIDMacroEng::ETokens::TypeCast)
    {
        // Check that this is legal for the side we are parsing
        CheckRHS(psrcClass, eSide);

        //
        //  They want to do a cast, so the first parm should be a class path
        //  and the second some expression to cast. We only support casting
        //  of numerics to other numeric types, and of cardinal values to
        //  enumerations.
        //
        bCheckOpenParen(psrcClass);
        tCIDLib::TCard4 c4Dummy;
        bGetClassPath(psrcClass, strTmp, c4Dummy);
        TMEngClassInfo* pmeciTarget = m_pmeTarget->pmeciFind(strTmp);

        // If not found, issue the error and just set it to Card4 so we can go
        if (!pmeciTarget)
        {
            IssueErr(psrcClass, kMEngErrs::errcEng_ClassNotFound, strTmp);
            pmeciTarget = &m_pmeTarget->meciFind(tCIDMacroEng::EIntrinsics::Card4);
        }

        //
        //  Ok, we have the target type. Get's it's type id out, since this
        //  is what we want to return as our resulting type. When we are done,
        //  the new value will be of the target type.
        //
        c2RetId = pmeciTarget->c2Id();

        //
        //  The target type must also be either a numeric, boolean, or enum
        //  type. Issue the error and fake the type to Card4 so we can keep
        //  going.
        //
        if (!m_pmeTarget->bIsNumericClass(c2RetId)
        &&  (c2RetId != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Enum))
        &&  (c2RetId != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Boolean)))

        {
            IssueErr(psrcClass, kMEngErrs::errcPrs_BadCastType);
            pmeciTarget = &m_pmeTarget->meciFind(tCIDMacroEng::EIntrinsics::Card4);
        }

        // Next we have to have a comma, then some nested expression to cast
        bCheckComma(psrcClass);
        tCIDLib::TBoolean bDummy;
        tCIDMacroEng::EExprTypes eExprType;
        const tCIDLib::TCard2 c2CastFrom = c2ParseExpr
        (
            psrcClass
            , tCIDMacroEng::EExprSides::RHS
            , meciTarget
            , methiTarget
            , methToFill
            , eExprType
            , bDummy
        );

        // And we must see the trailing parent
        bCheckCloseParen(psrcClass);

        //
        //  The source type must also be either a numeric or enum type. Issue
        //  the error and fake the type to Card4 so we can keep going.
        //
        if (!m_pmeTarget->bIsNumericClass(c2CastFrom)
        &&  (c2CastFrom != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Enum))
        &&  (c2CastFrom != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Boolean)))
        {
            IssueErr(psrcClass, kMEngErrs::errcPrs_BadCastType);
            pmeciTarget = &m_pmeTarget->meciFind(tCIDMacroEng::EIntrinsics::Card4);
        }

        //
        //  If the actual type and cast type are the same, do nothing, and the
        //  expression parsed becomes the result and the type of expression
        //  parsed because the returned expression type. Else, issue a type
        //  cast opcode, and the return type becomes the target type and the
        //  expression type becomes a const object ref (which will be from the
        //  temp pool.)
        //
        if (c2CastFrom == c2RetId)
        {
            eTypeParsed = eExprType;
            c2RetId = c2CastFrom;
        }
         else
        {
            //
            //  <TBD> Couldn't we do some of these scenarios statically and
            //  avoid the runtime cast?
            //
            if (!pmeciTarget->bCanCastFrom(c2CastFrom))
            {
                IssueErr
                (
                    psrcClass
                    , kMEngErrs::errcPrs_CannotCast
                    , m_pmeTarget->strClassPathForId(c2CastFrom)
                    , pmeciTarget->strClassPath()
                );
            }

            m_meopToUse.SetSingleIndex(tCIDMacroEng::EOpCodes::TypeCast, c2RetId);
            methToFill.c4AddOpCode(m_meopToUse);
            eTypeParsed = tCIDMacroEng::EExprTypes::ConstObjectRef;
        }
    }
     else
    {
        // It's something we shouldn't be seeing
        IssueErr(psrcClass, kMEngErrs::errcPrs_UnexpectedToken, strToken);
        ThrowUnrecoverable();
    }
    return c2RetId;
}


tCIDLib::TCard2
TMacroEngParser::c2ParseExpr(       TParserSrc&             psrcClass
                            , const tCIDMacroEng::EExprSides   eSide
                            , const TMEngClassInfo&         meciTarget
                            , const TMEngMethodInfo&        methiTarget
                            ,       TMEngOpMethodImpl&      methToFill
                            ,       tCIDMacroEng::EExprTypes&  eTypeParsed
                            ,       tCIDLib::TBoolean&      bDirectRef)
{
    tCIDLib::TCard2     c2RetId = 0;

    //
    //  Check to see if we have a negation operator on this factor. If so,
    //  eat it and remember that we saw it.
    //
    const tCIDLib::TBoolean bNegate = psrcClass.bIfPeeked(tCIDMacroEng::ETokens::Exclaim);

    //
    //  Now get the first factor of the expression. It will return us the
    //  type id of the factor.
    //
    c2RetId = c2ParseFactor
    (
        psrcClass
        , eSide
        , meciTarget
        , methiTarget
        , methToFill
        , eTypeParsed
        , bDirectRef
    );

    if (bNegate)
    {
        if (c2RetId != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Boolean))
            IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedBoolExpr);

        // Put out the negation opcode
        m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::Negate);
        methToFill.c4AddOpCode(m_meopToUse);
    }

    //
    //  We have a special case here. If we get something like left+1 or left-1,
    //  with no spaces then it's a problem. We eat left above, and now have the
    //  sign and number. If we get the next token we will get back that it's a
    //  signed number. But really it is an add or subtract operator.
    //
    //  So, we peek. If it comes back a numeric literal, then we peek the next
    //  char. If that's a + or - sign, then we know we hit the special case. So we
    //  will force the token to be add or subtract instead. Below that will cause
    //  us to only eat a char instead of a token once we commit.
    //
    //  There is never any scenario here where the next legal token could be a
    //  signed number, so should always do the right thing.
    //
    tCIDMacroEng::ETokens eTok = psrcClass.ePeekNextToken();
    if (eTok == tCIDMacroEng::ETokens::NumericLiteral)
    {
        const tCIDLib::TCh chNext = psrcClass.chPeekNextChar();
        if (chNext == kCIDLib::chPlusSign)
        {
            eTok = tCIDMacroEng::ETokens::Add;
        }
         else if (chNext == kCIDLib::chHyphenMinus)
        {
            eTok = tCIDMacroEng::ETokens::Subtract;
        }
    }


    TMEngClassInfo& meciRet = m_pmeTarget->meciFind(c2RetId);
    tCIDLib::TCard2 c2MethId;
    tCIDMacroEng::EOpResults eOpRes = eGetOpInfo(meciRet, eTok, c2MethId);

    //
    //  If it was an operator, but it's not supported by the type that we
    //  are returning, then issue an error. If it's not an op, just fall
    //  through, else it's an op and it's supported, so try to process it.
    //
    if (eOpRes == tCIDMacroEng::EOpResults::NotSupported)
    {
        IssueErr
        (
            psrcClass
            , kMEngErrs::errcPrs_OpNotSupported
            , meciRet.strClassPath()
        );
    }
     else if (eOpRes == tCIDMacroEng::EOpResults::NotOp)
    {
        // If this is a LHS expression, we won't use the return
        if (eSide == tCIDMacroEng::EExprSides::LHS)
        {
            m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::PopTop);
            methToFill.c4AddOpCode(m_meopToUse);
        }
    }
     else
    {
        // Eat the token/char we peeked above
        if ((eTok == tCIDMacroEng::ETokens::Add) || (eTok == tCIDMacroEng::ETokens::Subtract))
            psrcClass.chGetNextChar();
        else
            psrcClass.eGetNextToken();

        // If an assign or unary, then it must be a non-const, direct ref
        if ((eOpRes == tCIDMacroEng::EOpResults::Assign)
        ||  (eOpRes == tCIDMacroEng::EOpResults::Unary))
        {
            // If an assign, this must be a LHS operation
            if ((eOpRes == tCIDMacroEng::EOpResults::Assign)
            &&  (eSide == tCIDMacroEng::EExprSides::RHS))
            {
                IssueErr(psrcClass, kMEngErrs::errcPrs_BadOpHere);
            }

            // It needs to be a direct object reference
            if (!bDirectRef)
                IssueErr(psrcClass, kMEngErrs::errcPrs_MustBeObjRef);

            // It must be a non-const for any of these
            if (eTypeParsed != tCIDMacroEng::EExprTypes::ObjectRef)
                IssueErr(psrcClass, kMEngErrs::errcPrs_NCOpOnConstObj);
        }
         else if ((eOpRes == tCIDMacroEng::EOpResults::Logical)
              ||  (eOpRes == tCIDMacroEng::EOpResults::Mathematical))
        {
            // These are not supported on the LHS
            if (eSide == tCIDMacroEng::EExprSides::LHS)
                IssueErr(psrcClass, kMEngErrs::errcPrs_BadOpHere);

            //
            //  Since both sides must be boolean in the case of logical, or
            //  of the types of the two sides in the case of mathematical,
            //  and that is what our result will be, we can just leave it
            //  alone.
            //
        }
         else if (eOpRes == tCIDMacroEng::EOpResults::Bitwise)
        {
            // The bitwise operators are not legal on a LHS
            if (eSide == tCIDMacroEng::EExprSides::LHS)
                IssueErr(psrcClass, kMEngErrs::errcPrs_BadOpHere);

            //
            //  The return will be the same as the two expressions, which
            //  is what it is already set to.
            //
        }
         else if (eOpRes == tCIDMacroEng::EOpResults::Binary)
        {
            // Binary operators are not supported on the LHS
            if (eSide == tCIDMacroEng::EExprSides::LHS)
                IssueErr(psrcClass, kMEngErrs::errcPrs_BadOpHere);

            //
            //  Force the return now to be a boolean, which will be the
            //  result of the operator call.
            //
            c2RetId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Boolean);
        }

        if (eOpRes == tCIDMacroEng::EOpResults::Assign)
        {
            //
            //  Push the return, which is always void for these LHS exprs.
            //  If it's an assign operator, then we don't push a return
            //  because that is handled via an opcode.
            //
            if (eTok != tCIDMacroEng::ETokens::Assign)
            {
                m_meopToUse.SetSingleIndex
                (
                    tCIDMacroEng::EOpCodes::PushTempConst
                    , tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Void)
                );
                methToFill.c4AddOpCode(m_meopToUse);
            }

            //
            //  Get the right hand side expression, which will put it on the
            //  stack in the correct order for the upcoming method call.
            //
            tCIDLib::TBoolean bDummy;
            tCIDMacroEng::EExprTypes eExprType;
            tCIDLib::TCard2 c2RetType = c2ParseExpr
            (
                psrcClass
                , tCIDMacroEng::EExprSides::RHS
                , meciTarget
                , methiTarget
                , methToFill
                , eExprType
                , bDummy
            );

            //
            //  The RHS type must be the same as the LHS. If it was a literal,
            //  see if we can force to the same type. Or they can be equiv
            //  collections.
            //
            if (c2RetType != meciRet.c2Id())
            {
                if (eExprType == tCIDMacroEng::EExprTypes::NumLiteral)
                {
                    TMEngOpCode& meopParm = methToFill.meopLast();
                    if (meopParm.bConvertNumeric(meciRet.c2Id()))
                        c2RetType = meciRet.c2Id();
                }
            }

            if (c2RetType != meciRet.c2Id())
            {
                // The element types must be identical as well
                if (!m_pmeTarget->bAreEquivCols(c2RetType, meciRet.c2Id(), kCIDLib::True))
                {
                    IssueErr
                    (
                        psrcClass
                        , kMEngErrs::errcPrs_BadAssign
                        , m_pmeTarget->strClassPathForId(c2RetType)
                        , meciRet.strClassPath()
                    );
                }
            }

            //
            //  Make sure the class is a copyable class if it is an assign
            //  operator. If not, issue an error but keep going.
            //
            if ((eTok == tCIDMacroEng::ETokens::Assign) && !meciRet.bIsCopyable())
            {
                IssueErr
                (
                    psrcClass
                    , kMEngErrs::errcPrs_NotCopyable
                    , meciRet.strClassPath()
                );
            }

            //
            //  Issue the appropriate method call now. We got the correct
            //  method id above. The first index is how far down from the
            //  stack top the object is.
            //
            //  If it is an assignment operator, that is handled via an
            //  opcode, which pops the two objects off after the copy.
            //
            if (eTok == tCIDMacroEng::ETokens::Assign)
            {
                m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::Copy);
                methToFill.c4AddOpCode(m_meopToUse);
            }
             else
            {
                m_meopToUse.SetDoubleIndex(tCIDMacroEng::EOpCodes::CallStack, 3, c2MethId);
                methToFill.c4AddOpCode(m_meopToUse);

                // Pop the parm, original object, and return off
                m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::MultiPop, 3UL);
                methToFill.c4AddOpCode(m_meopToUse);
            }
        }
         else if ((eOpRes == tCIDMacroEng::EOpResults::Binary)
              ||  (eOpRes == tCIDMacroEng::EOpResults::Bitwise)
              ||  (eOpRes == tCIDMacroEng::EOpResults::Mathematical))
        {
            //
            //  Push the return value. For these, it's either going to be
            //  a boolean result of some comparison, or it's going to be
            //  a bitwise in which case the return is of the type of the
            //  two sides of the bitwise (which are the same.)
            //
            if ((eOpRes == tCIDMacroEng::EOpResults::Bitwise)
            ||  (eOpRes == tCIDMacroEng::EOpResults::Mathematical))
            {
                m_meopToUse.SetSingleIndex
                (
                    tCIDMacroEng::EOpCodes::PushTempVar
                    , c2RetId
                );
            }
             else
            {
                m_meopToUse.SetSingleIndex
                (
                    tCIDMacroEng::EOpCodes::PushTempVar
                    , tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Boolean)
                );
            }
            methToFill.c4AddOpCode(m_meopToUse);

            //
            //  Get the right hand side expression, which will put it on the
            //  stack in the correct order for the upcoming method call.
            //
            tCIDLib::TBoolean bDummy;
            tCIDMacroEng::EExprTypes eExprType;
            tCIDLib::TCard2 c2RetType = c2ParseExpr
            (
                psrcClass
                , tCIDMacroEng::EExprSides::RHS
                , meciTarget
                , methiTarget
                , methToFill
                , eExprType
                , bDummy
            );

            //
            //  The RHS type must be the same as the LHS. If it was a literal,
            //  see if we can force to the same type.
            //
            if (c2RetType != meciRet.c2Id())
            {
                if (eExprType == tCIDMacroEng::EExprTypes::NumLiteral)
                {
                    TMEngOpCode& meopParm = methToFill.meopLast();
                    if (meopParm.bConvertNumeric(meciRet.c2Id()))
                        c2RetType = meciRet.c2Id();
                }
            }

            if (c2RetType != meciRet.c2Id())
                IssueErr(psrcClass, kMEngErrs::errcPrs_ExprSidesMatch);

            //
            //  Issue the appropriate method call now. We got the correct
            //  method id above. The first index is how far down from the
            //  stack top the object is.
            //
            m_meopToUse.SetDoubleIndex(tCIDMacroEng::EOpCodes::CallStack, 3, c2MethId);
            methToFill.c4AddOpCode(m_meopToUse);

            //
            //  In this case, we called a method which left a value on
            //  the stack that we want to keep. But, the original object
            //  is below it. So want to pop the parm, flip the stack top,
            //  then pop the top to get rid of the original object.
            //
            m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::PopTop);
            methToFill.c4AddOpCode(m_meopToUse);
            m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::FlipTop);
            methToFill.c4AddOpCode(m_meopToUse);
            m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::PopTop);
            methToFill.c4AddOpCode(m_meopToUse);

            // If it's a 'not equal', then put out a negate opcode
            if (eTok == tCIDMacroEng::ETokens::NotEqual)
            {
                m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::Negate);
                methToFill.c4AddOpCode(m_meopToUse);
            }
        }
         else if (eOpRes == tCIDMacroEng::EOpResults::Logical)
        {
            //
            //  The logical ops are not implemented by mapping an operator
            //  to a method call on the LHS object. These are implemented
            //  via opcodes, and they expect both sides to be boolean
            //  expressions and their results on the top of stack. It will
            //  do the logical op on both, pop them both off, and replace
            //  them with the a boolean result off the temp pool.
            //
            //  However, in order to support short circuiting of the
            //  logical expression when we know that the RHS isn't going to
            //  make a difference, we put out a conditional jump here,
            //  which will we come back later and update with the correct
            //  target. According to the operator type, we can jump over
            //  the second expression evaluation and the logical operator
            //  and just use the boolean value on the stack from the first
            //  expression.
            //
            //  We use the 'no pop' conditional jumps here, so that the
            //  boolean value of the first expression will be there to
            //  represent what would have been the output of the logical
            //  operator.
            //
            //  NOTE:   We can't short circuit a XOR.
            //
            tCIDLib::TCard4 c4SaveOfs = kCIDLib::c4MaxCard;
            if (eTok == tCIDMacroEng::ETokens::LogAnd)
            {
                m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::NotCondJumpNP);
                c4SaveOfs = methToFill.c4AddOpCode(m_meopToUse);
            }
             else if (eTok == tCIDMacroEng::ETokens::LogOr)
            {
                m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::CondJumpNP);
                c4SaveOfs = methToFill.c4AddOpCode(m_meopToUse);
            }

            // Now do the second expression evaluation
            tCIDLib::TBoolean bDummy;
            tCIDMacroEng::EExprTypes eExprType;
            tCIDLib::TCard2 c2RetType = c2ParseExpr
            (
                psrcClass
                , tCIDMacroEng::EExprSides::RHS
                , meciTarget
                , methiTarget
                , methToFill
                , eExprType
                , bDummy
            );

            //
            //  Make sure both this one and the original are boolean. If
            //  not, issue the error but keep going.
            //
            if ((c2RetId != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Boolean))
            ||  (c2RetType != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Boolean)))
            {
                IssueErr(psrcClass, kMEngErrs::errcPrs_LogOpExprs);
            }

            // Issue the correct logical opcode
            if (eTok == tCIDMacroEng::ETokens::LogAnd)
                m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::LogicalAnd);
            else if (eTok == tCIDMacroEng::ETokens::LogOr)
                m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::LogicalOr);
            else if (eTok == tCIDMacroEng::ETokens::LogXor)
                m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::LogicalXor);
            methToFill.c4AddOpCode(m_meopToUse);

            //
            //  Ok, if it was Or or And, we can go back and update the
            //  short circuit jump to come to here on the far side of the
            //  second expression.
            //
            if ((eTok == tCIDMacroEng::ETokens::LogAnd)
            ||   (eTok == tCIDMacroEng::ETokens::LogOr))
            {
                TMEngOpCode& meopJump = methToFill[c4SaveOfs];
                meopJump.c4Immediate(methToFill.c4CurOffset());
            }
        }
         else if (eOpRes == tCIDMacroEng::EOpResults::Unary)
        {
            //
            //  The target object is on the stack, but we are now going
            //  to call a method on it, so we have to get a return
            //  value under it. This is a unary op, so it will have a
            //  return of the same type as it.
            //
            m_meopToUse.SetSingleIndex(tCIDMacroEng::EOpCodes::PushTempVar, c2RetId);
            methToFill.c4AddOpCode(m_meopToUse);

            //
            //  Issue the appropriate method call now. We got the
            //  correct method id back above when we checked for a
            //  unary operator. The first index is how far down from
            //  the stack top the object is.
            //
            m_meopToUse.SetDoubleIndex(tCIDMacroEng::EOpCodes::CallStack, 2, c2MethId);
            methToFill.c4AddOpCode(m_meopToUse);

            //
            //  If this is a LHS, then pop off the call and return. If it
            //  is a RHS, then just pop the call and leave the return.
            //
            if (eSide == tCIDMacroEng::EExprSides::RHS)
            {
                m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::FlipTop);
                methToFill.c4AddOpCode(m_meopToUse);
                m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::PopTop);
                methToFill.c4AddOpCode(m_meopToUse);
            }
             else
            {
                m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::MultiPop, 2UL);
                methToFill.c4AddOpCode(m_meopToUse);
            }
        }

        //
        //  Since this is an operator now, we aren't returning either of
        //  the things we found, we are returning a temp value in which
        //  the result will be left.
        //
        eTypeParsed = tCIDMacroEng::EExprTypes::ConstObjectRef;
        bDirectRef = kCIDLib::False;
    }

    if ((eSide == tCIDMacroEng::EExprSides::RHS)
    &&  (c2RetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Void)))
    {
        IssueErr(psrcClass, kMEngErrs::errcPrs_MustReturnValue);
        ThrowUnrecoverable();
    }
    return c2RetId;
}


tCIDLib::TVoid
TMacroEngParser::CheckLHS(          TParserSrc&             psrcClass
                            , const tCIDMacroEng::EExprSides   eSide)
{
    if (eSide != tCIDMacroEng::EExprSides::LHS)
        IssueErr(psrcClass, kMEngErrs::errcPrs_NotOnRHS);
}

tCIDLib::TVoid
TMacroEngParser::CheckRHS(          TParserSrc&             psrcClass
                            , const tCIDMacroEng::EExprSides   eSide)
{
    if (eSide != tCIDMacroEng::EExprSides::RHS)
        IssueErr(psrcClass, kMEngErrs::errcPrs_NotOnLHS);
}


tCIDLib::TVoid
TMacroEngParser::GenDefMemberCtor(  TParserSrc&             psrcClass
                                    , TMEngMemberInfo&      memiTarget
                                    , TMEngOpMethodImpl&    methToFill)
{
    // Generate a default ctor for the expected member
    TMEngClassInfo* pmeciTarget = &m_pmeTarget->meciFind(memiTarget.c2ClassId());
    const TMEngMethodInfo* pmethiDefCtor = pmeciTarget->pmethiDefCtor();
    if (!pmethiDefCtor)
    {
        IssueErr
        (
            psrcClass
            , kMEngErrs::errcClass_NoDefCtor
            , pmeciTarget->strClassPath()
        );
    }
     else
    {
        m_meopToUse.SetSingleIndex
        (
            tCIDMacroEng::EOpCodes::PushTempConst, tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Void)
        );
        methToFill.c4AddOpCode(m_meopToUse);
        m_meopToUse.SetDoubleIndex
        (
            tCIDMacroEng::EOpCodes::CallMember
            , memiTarget.c2Id()
            , pmethiDefCtor->c2Id()
        );
        methToFill.c4AddOpCode(m_meopToUse);
        m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::PopTop);
        methToFill.c4AddOpCode(m_meopToUse);
    }
}


tCIDLib::TVoid
TMacroEngParser::GenDefParentCtor(          TParserSrc&         psrcClass
                                    , const TMEngClassInfo&     meciTarget
                                    ,       TMEngOpMethodImpl&  methToFill)
{
    const TMEngMethodInfo* pmethiDefCtor = meciTarget.pmethiDefCtor();

    if (pmethiDefCtor)
    {
        m_meopToUse.SetSingleIndex
        (
            tCIDMacroEng::EOpCodes::PushTempConst
            , tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Void)
        );
        methToFill.c4AddOpCode(m_meopToUse);
        m_meopToUse.SetSingleIndex(tCIDMacroEng::EOpCodes::CallParent, pmethiDefCtor->c2Id());
        methToFill.c4AddOpCode(m_meopToUse);
        m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::PopTop);
        methToFill.c4AddOpCode(m_meopToUse);
    }
     else
    {
        IssueErr
        (
            psrcClass
            , kMEngErrs::errcClass_NoDefCtor
            , meciTarget.strClassPath()
        );
    }
}


// Call to parse constructor initializers
tCIDLib::TVoid
TMacroEngParser::ParseInitializers( TParserSrc&             psrcClass
                                    , TMEngClassInfo&       meciToFill
                                    , TMEngMethodInfo&      methiTarget
                                    , TMEngOpMethodImpl&    methToFill)
{
    //
    //  This method will parse all of the parent/member initializers of a
    //  constructor. Note that the parent must be called first, then each
    //  member must be initialized, in the order declared. For each one, if
    //  it's not explicitly called, we will look for a default ctor. If
    //  there isn't one, then that's an error.
    //
    //  First, check for the Begin keyword. If we get this, it means that
    //  we really have no initializers, which is not legal because we wouldn't
    //  be here if we didn't see the colon that indicted initializers were
    //  present, so issue an error and don't bother to generate any default
    //  initializer calls.
    //
    if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::Begin))
    {
        IssueErr(psrcClass, kMEngErrs::errcPrs_NoInitializers);
        return;
    }

    //
    //  Ok, we have to have some initializers now. First, check for the parent
    //  class call. If explicitly provided, then find the right ctor, else
    //  generate a default ctor call.
    //
    TMEngClassInfo*     pmeciTarget;
    TMEngMemberInfo*    pmemiTarget;

    // Get the parent class and then generate the correct call to it
    pmeciTarget = &m_pmeTarget->meciFind(meciToFill.c2ParentClassId());
    if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::Parent))
    {
        // Generate the line number and void return value
        m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::CurLine, psrcClass.c4Line());
        methToFill.c4AddOpCode(m_meopToUse);
        m_meopToUse.SetSingleIndex
        (
            tCIDMacroEng::EOpCodes::PushTempConst
            , tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Void)
        );
        methToFill.c4AddOpCode(m_meopToUse);

        // Find the correct ctor, which will push all of the parms
        bCheckOpenParen(psrcClass);
        tCIDLib::TCard4 c4ParmCnt;
        tCIDLib::TCard2 c2ToUse = c2FindCorrectCtor
        (
            psrcClass
            , *pmeciTarget
            , meciToFill
            , methiTarget
            , methToFill
            , c4ParmCnt
        );
        bCheckSemiColon(psrcClass);

        //
        //  If we found a match, generate the call. If not, then he issued
        //  the error, so it doesn't mattter than we don't issue a call.
        //
        if (c2ToUse == kCIDMacroEng::c2BadId)
        {
            IssueErr
            (
                psrcClass
                , kMEngErrs::errcPrs_NoParentCtorMatch
                , pmeciTarget->strClassPath()
            );
        }
         else
        {
            m_meopToUse.SetSingleIndex(tCIDMacroEng::EOpCodes::CallParent, c2ToUse);
            methToFill.c4AddOpCode(m_meopToUse);
            m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::MultiPop, c4ParmCnt + 1);
            methToFill.c4AddOpCode(m_meopToUse);
        }
    }
     else
    {
        // Generate a default ctor to the parent class
        GenDefParentCtor(psrcClass, *pmeciTarget, methToFill);
    }

    // Get a temp string from the pool
    TMEngStrPoolJan janToken(m_pmsplParse);
    TString& strToken = janToken.strThis();

    //
    //  Note we all do only our members here, skipping over those of our
    //  parent class, or we'll re-init them, which would be bad.
    //
    const tCIDLib::TCard4 c4InitCount = meciToFill.c4MemberCount();
    tCIDLib::TCard2 c2MemberId = meciToFill.c2FirstMemberId();
    while (kCIDLib::True)
    {
        //
        //  Break out on the Begin keyword. If we haven't seen all of the
        //  member initializers yet, we'll do default ctors for them below.
        //
        if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::Begin))
            break;

        // Get a name token, which must match a member name
        bGetNameToken(psrcClass, strToken, kCIDLib::False);

        //
        //  Look it up and see if it's a legal member name, for this
        //  class only.
        //
        pmemiTarget = meciToFill.pmemiFind(strToken, kCIDLib::True);
        if (!pmemiTarget)
        {
            IssueErr
            (
                psrcClass
                , kMEngErrs::errcClass_MemberNotFound
                , strToken
                , meciToFill.strClassPath()
            );
            ThrowUnrecoverable();

            // Won't happen, but makes analyzer happy
            return;
        }

        //
        //  If this member is greater than the current id, then we have to
        //  generate default ctor calls for all of the members between the
        //  one we were expected next, and the one we found. If it is less
        //  than our current, then it's an out of order initializer, but we'll
        //  continue after issuing the error.
        //
        if (pmemiTarget->c2Id() < c2MemberId)
        {
            IssueErr(psrcClass, kMEngErrs::errcClass_InitOrder);
        }
         else if (pmemiTarget->c2Id() > c2MemberId)
        {
            const tCIDLib::TCard2 c2NextId = pmemiTarget->c2Id();
            for (tCIDLib::TCard2 c2Id = c2MemberId; c2Id < c2NextId; c2Id++)
            {
                TMEngMemberInfo& memiDef = meciToFill.memiFind(c2Id);
                GenDefMemberCtor(psrcClass, memiDef, methToFill);
            }

            // Move up to this new member
            c2MemberId = pmemiTarget->c2Id();
        }

        // Get the class for the target member
        pmeciTarget = &m_pmeTarget->meciFind(pmemiTarget->c2ClassId());

        // Put out a line number opcode
        m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::CurLine, psrcClass.c4Line());
        methToFill.c4AddOpCode(m_meopToUse);

        //
        //  Issue an opcode to push a void type from the temp pool. Only
        //  constructors get called here, and they all must have void return
        //  type.
        //
        m_meopToUse.SetSingleIndex
        (
            tCIDMacroEng::EOpCodes::PushTempConst
            , tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Void)
        );
        methToFill.c4AddOpCode(m_meopToUse);

        // Now we should see an open paren for the initializer
        bCheckOpenParen(psrcClass);

        // Find the right ctor, which will push the parameters
        tCIDLib::TCard4 c4ParmCnt;
        tCIDLib::TCard2 c2CtorToUse = c2FindCorrectCtor
        (
            psrcClass
            , *pmeciTarget
            , meciToFill
            , methiTarget
            , methToFill
            , c4ParmCnt
        );

        // We should see a trailing semicolon
        bCheckSemiColon(psrcClass);

        //
        //  If a match was found, then generate the call and cleanup.
        //
        if (c2CtorToUse == kCIDMacroEng::c2BadId)
        {
            IssueErr
            (
                psrcClass, kMEngErrs::errcPrs_NoCtorMatch, pmemiTarget->strName()
            );
        }
         else
        {
            m_meopToUse.SetDoubleIndex
            (
                tCIDMacroEng::EOpCodes::CallMember, pmemiTarget->c2Id(), c2CtorToUse
            );
            methToFill.c4AddOpCode(m_meopToUse);

            // And pop the return type
            m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::MultiPop, c4ParmCnt + 1);
            methToFill.c4AddOpCode(m_meopToUse);
        }

        // Move up to the next member we expect to see
        c2MemberId++;
    }

    //
    //  If we didn't get them all done before we saw Begin, then we have to
    //  do def ctors for the rest of the members.
    //
    for (; c2MemberId < c4InitCount; c2MemberId++)
    {
        TMEngMemberInfo& memiDef = meciToFill.memiFind(c2MemberId);
        GenDefMemberCtor(psrcClass, memiDef, methToFill);
    }
}


//
//  At this level we parse the individual method headers, create the new
//  method info and method impl objects using that info, and then call the
//  method that parses the actual content and generates the opcodes into the
//  method impl object. If all of that works, then the info and impl objects
//  are added to the class.
//
//  NOTE:   If the method is overriding a non-final method of a parent class,
//          then we don't create an info object because one was already put
//          into our class when it was created, based on the methods of the
//          parent (so that our method ids will start past his.)
//
//          We can though, update our copy if the method is changed to Final
//          status at this level, which can be done.
//
tCIDLib::TVoid
TMacroEngParser::ParseMethods(TParserSrc& psrcClass, TMEngClassInfo& meciToFill)
{
    // We need an equal sign after the open of the block
    bCheckEquals(psrcClass);

    //
    //  And we can see optional method attributes here. If we don't see them,
    //  then we assume the defaults.
    //
    tCIDMacroEng::EMethExt     eExtendability  = tCIDMacroEng::EMethExt::NonFinal;
    tCIDMacroEng::EVisTypes    eVisibility     = tCIDMacroEng::EVisTypes::Public;
    tCIDLib::TBoolean       bOverrides      = kCIDLib::False;
    tCIDMacroEng::EConstTypes  eConst          = tCIDMacroEng::EConstTypes::NonConst;

    tCIDMacroEng::ETokens  eTok;
    TString             strText;
    if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::OpenBracket))
    {
        while (kCIDLib::True)
        {
            // Get the next token and check it
            eTok = psrcClass.eGetNextToken(strText);

            switch(eTok)
            {
                case tCIDMacroEng::ETokens::Private :
                    eVisibility = tCIDMacroEng::EVisTypes::Private;
                    break;

                case tCIDMacroEng::ETokens::Public :
                    eVisibility = tCIDMacroEng::EVisTypes::Public;
                    break;

                case tCIDMacroEng::ETokens::Final :
                    eExtendability = tCIDMacroEng::EMethExt::Final;
                    break;

                case tCIDMacroEng::ETokens::NonFinal:
                    eExtendability = tCIDMacroEng::EMethExt::NonFinal;
                    break;

                case tCIDMacroEng::ETokens::Required :
                    eExtendability = tCIDMacroEng::EMethExt::Required;
                    break;

                case tCIDMacroEng::ETokens::Overrides :
                    bOverrides = kCIDLib::True;
                    break;

                case tCIDMacroEng::ETokens::Const :
                    eConst = tCIDMacroEng::EConstTypes::Const;
                    break;

                case tCIDMacroEng::ETokens::NonConst :
                    eConst = tCIDMacroEng::EConstTypes::NonConst;
                    break;

                default :
                    // It's not a valid attribute
                    IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedMethAttr, strText);
                    break;
            }

            //
            //  Peek and see if we hit the close bracket. If so, then break
            //  out of the loop. Else, we have to see a comma.
            //
            if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::CloseBracket))
                break;

            bCheckComma(psrcClass);
        }
    }

    // Get the parent class, since we'll need it in a few places below
    const TMEngClassInfo& meciParent = m_pmeTarget->meciFind(meciToFill.c2ParentClassId());

    //
    //  We loop until we see the EndMethods marker. On each loop through, we
    //  should see a method start.
    //
    tCIDLib::TCard4 c4CtorCount = 0;
    TString strName;
    while (kCIDLib::True)
    {
        eTok = psrcClass.eGetNextToken(strText);

        // Break out on the end of imports statement
        if (eTok == tCIDMacroEng::ETokens::EndMethods)
            break;

        //
        //  Else it has to be Method, which introduces a method, or it can be
        //  the special Contructor form of a method.
        //
        if ((eTok != tCIDMacroEng::ETokens::Method)
        &&  (eTok != tCIDMacroEng::ETokens::Constructor))
        {
            IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedMethodCtor);
            ThrowUnrecoverable();
        }

        //
        //  Ok, the next token must be a name token if it's a method. Indicate
        //  that we won't allow this to match any reserved word. If it's a
        //  constructor, then we need to create a dummy name for it, in the
        //  form "ctorX-CP", where X is a counter and CP is the class path.
        //
        if (eTok == tCIDMacroEng::ETokens::Method)
        {
            bGetNameToken(psrcClass, strName, kCIDLib::False);
        }
         else
        {
            strName = L"ctor";
            strName.AppendFormatted(++c4CtorCount);
            strName.Append(kCIDLib::chHyphenMinus);
            strName.Append(meciToFill.strClassPath());
        }

        //
        //  Create a temp method info object for us to fill in as we parse
        //  the header. We don't have all the info yet, but we'll fill it in
        //  as we go. We don't worry yet if it's an override of not. If it is,
        //  when we get to the end, we'll see if it makes for a legal
        //  override.
        //
        TMEngMethodInfo methiNew
        (
            strName
            , tCIDMacroEng::EIntrinsics::Void
            , eVisibility
            , eExtendability
            , eConst
        );

        //
        //  If it's a constructor, then set a flag on the info object, and
        //  make sure that the attributes for this block are final and public
        //  and nothing else.
        //
        if (eTok == tCIDMacroEng::ETokens::Constructor)
        {
            if ((eExtendability != tCIDMacroEng::EMethExt::Final)
            ||  (eVisibility != tCIDMacroEng::EVisTypes::Public)
            ||  (eConst == tCIDMacroEng::EConstTypes::Const))
            {
                IssueErr(psrcClass, kMEngErrs::errcPrs_CtorAttrs);
            }
            methiNew.bIsCtor(kCIDLib::True);
        }

        //
        //  Ok, we now have to loop through any parameters. They are all
        //  optional of course, unless this is an override, in which case
        //  it has to match the previous definition. We'll parse them first,
        //  then compare them after the fact.
        //
        ParseMethodParms(psrcClass, meciToFill, methiNew);

        //
        //  And now we can optionally see the Returns keyword to set the
        //  return type. We defaulted it to void above, so if it's not set
        //  here it'll be void.
        //
        if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::Returns))
        {
            // Constructors cannot return a value
            if (methiNew.bIsCtor())
                IssueErr(psrcClass, kMEngErrs::errcPrs_CtorCantReturn);

            //
            //  We have to get a class path now. It might only be the relative
            //  part, so we have to resove the class name. The default for
            //  this one is not to recover, so we tell it it can. If it does,
            //  we'll just leave it void and try to go on. But, if it gives us
            //  a class, and we can't resove it, we'll give up.
            //
            if (bGetClassPath(psrcClass, strText, tCIDMacroEng::ERecovers::Yes))
            {
                // We got a legal path, so resolve it
                const TMEngClassInfo& meciTarget = meciResolvePath
                (
                    psrcClass
                    , strText
                );

                // This must be a copyable type
                if (!meciTarget.bIsCopyable())
                {
                    IssueErr
                    (
                        psrcClass
                        , kMEngErrs::errcPrs_NotCopyable
                        , meciTarget.strName()
                    );
                }

                // Update the return class id now that we know it
                methiNew.c2RetClassId(meciTarget.c2Id());
            }
        }

        //
        //  At this point, we need to figure out if it's a new method or an
        //  override, or bad because it claims to be one of those but ain't.
        //
        //  So start by seeing if we have a method in the parent class that
        //  has this name. If so, then it must be non-final and this must be
        //  an Overrides block.
        //
        TMEngMethodInfo* pmethiTarget = meciToFill.pmethiFind(strName);
        if (pmethiTarget)
        {
            // If its a ctor, this shouldn't happen
            if (methiNew.bIsCtor())
            {
                facCIDMacroEng().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kMEngErrs::errcPrs_CtorAlreadyExists
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Internal
                    , strName
                );
            }

            //
            //  First see if it's a duplicate at this class level. If the
            //  method id is at or beyond the first method id of this class,
            //  then it's a duplicate at this level. Else, it's something
            //  that exists in a parent class.
            //
            if (pmethiTarget->c2Id() >= meciToFill.c2FirstMethodId())
            {
                IssueErr(psrcClass, kMEngErrs::errcPrs_DupMethod, strName);
            }
             else
            {
                //
                //  Ok, so we must be overriding. If this method block isn't
                //  for overrides, then that's an error, but we'll try to
                //  keep going.
                //
                if (!bOverrides)
                    IssueErr(psrcClass, kMEngErrs::errcPrs_RequiresOverride, strName);

                // If the parent class's version is final, we can't do this
                if (pmethiTarget->eExtend() == tCIDMacroEng::EMethExt::Final)
                {
                    IssueErr
                    (
                        psrcClass
                        , kMEngErrs::errcPrs_ParentMethodIsFinal
                        , strName
                    );
                }

                //
                //  And this means that it must have identical parameters and
                //  return type as the the parent version.
                //
                //  Note that the target object's parm names will be updated
                //  to match those we just parsed, so that they will match
                //  the actual names, not the originals in the base class,
                //  which is what they are set to now. It will also set our
                //  existing object's extend type that that in the new
                //  method in case some legal change in the extension type
                //  was made.
                //
                if (!pmethiTarget->bIsLegalOverride(methiNew))
                    IssueErr(psrcClass, kMEngErrs::errcPrs_BadOverride);
            }
        }
         else
        {
            //
            //  Its a new method at our level, so it cannot be in an overrides
            //  block.
            //
            if (bOverrides)
                IssueErr(psrcClass, kMEngErrs::errcPrs_ParentMethNotFound, strName);

            // Make sure this name isn't used in this class (or ancestors) yet
            if (meciToFill.bCheckDupName(*m_pmeTarget, strName))
                IssueErr(psrcClass, kMEngErrs::errcPrs_DupName, strName);

            // Add it to the class
            const tCIDLib::TCard2 c2Id = meciToFill.c2AddMethodInfo(methiNew);
            pmethiTarget = &meciToFill.methiFind(c2Id);
        }

        //
        //  Ok, looks semi coherent, so let's provisionally create an impl
        //  object for this guy, and add it to the target class. If it fails,
        //  we'll have a partially completed method in the class, but that's
        //  ok because it's already failed and won't ever be run.
        //
        TMEngOpMethodImpl* pmethNew = new TMEngOpMethodImpl
        (
            pmethiTarget->strName()
            , pmethiTarget->c2Id()
        );
        meciToFill.AddMethodImpl(pmethNew);

        //
        //  Put out a line number opcode here. This avoids the issue
        //  where an implicit ctor call of a member will cause the
        //  debugger to run directly to that child constructor instead
        //  of stopping at the main class ctor first.
        //
        m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::CurLine, psrcClass.c4Line());
        pmethNew->c4AddOpCode(m_meopToUse);

        //
        //  If this is a constructor, then we can have a colon followed by
        //  a set of initializers. We have a special parser for that, since
        //  it is somewhat different from the usual syntax. It will break out
        //  when it sees the Begin for the method body.
        //
        if (pmethiTarget->bIsCtor())
        {
            if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::Colon))
            {
                ParseInitializers(psrcClass, meciToFill, *pmethiTarget, *pmethNew);
            }
             else
            {
                // Make it stop here, in case of any default initializers
                m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::CurLine, psrcClass.c4Line());
                pmethNew->c4AddOpCode(m_meopToUse);

                // Generate a default call to the parent class' ctor
                GenDefParentCtor(psrcClass, meciParent, *pmethNew);

                // And now just generate default ctor calls for all members
                const tCIDLib::TCard4 c4MemCount = meciToFill.c4MemberCount();
                tCIDLib::TCard2 c2MemberId = 0;
                for (; c2MemberId < c4MemCount; c2MemberId++)
                {
                    TMEngMemberInfo& memiDef = meciToFill.memiFind(c2MemberId);
                    GenDefMemberCtor(psrcClass, memiDef, *pmethNew);
                }

                // And next we should see the begin
                bCheckBegin(psrcClass);
            }
        }
         else
        {
            // Else, we should see the begin here
            bCheckBegin(psrcClass);
        }

        //
        //  And now let's call the method that does the real work of parsing
        //  the method body and generating the opcodes. It'll come back when
        //  it hits the end of the method.
        //
        ParseMethodBody(psrcClass, meciToFill, *pmethiTarget, *pmethNew);

        // And we need a semicolon
        bCheckSemiColon(psrcClass);

        //
        //  If this method returns a value, then the last opcodes in the
        //  method (other than current line and noop) must be pop to return,
        //  and return.
        //
        if (methiNew.c2RetClassId() != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Void))
        {
            tCIDLib::TBoolean bGotIt = kCIDLib::False;
            tCIDLib::TBoolean bDone = kCIDLib::False;
            tCIDLib::TBoolean bSawRet = kCIDLib::False;
            tCIDLib::TCard4 c4OpCount = pmethNew->c4CurOffset();
            while (c4OpCount && !bDone)
            {
                c4OpCount--;
                const TMEngOpCode& meopCheck = pmethNew->meopAt(c4OpCount);
                switch(meopCheck.eOpCode())
                {
                    case tCIDMacroEng::EOpCodes::NoOp :
                    case tCIDMacroEng::EOpCodes::CurLine :
                        break;

                    case tCIDMacroEng::EOpCodes::Return :
                        if (bSawRet)
                            bDone = kCIDLib::True;
                        else
                            bSawRet = kCIDLib::True;
                        break;

                    case tCIDMacroEng::EOpCodes::PopToReturn :
                        bDone = kCIDLib::True;
                        if (bSawRet)
                            bGotIt = kCIDLib::True;
                        break;

                    default :
                        bDone = kCIDLib::True;
                        break;
                };
            }

            if (!bGotIt)
                IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedReturn);
        }

        //
        //  We should not have anything on the flow control stack at this
        //  point. If it's not empty, then some control flow control block
        //  was left open.
        //
        if (!m_colFlowStack.bIsEmpty())
        {
            TMEngFlowCtrlItem& mefciLast = m_colFlowStack.objPeek();
            IssueErr
            (
                psrcClass
                , kMEngErrs::errcPrs_OpenFlowStatement
                , tCIDMacroEng::strXlatEFlowTypes(mefciLast.eType())
                , TCardinal(mefciLast.c4LineNum())
            );

            // We can keep going, but flush it manually first
            m_colFlowStack.RemoveAll();
        }
    }

    // And we need a semicolon at the end of the block
    bCheckSemiColon(psrcClass);
}



tCIDLib::TVoid
TMacroEngParser::ParseMethodBody(TParserSrc&            psrcClass
                                , TMEngClassInfo&       meciWorking
                                , TMEngMethodInfo&      methiWorking
                                , TMEngOpMethodImpl&    methToFill)
{
    //
    //  From here on out, we have to put out line number opcodes, to be used
    //  in error reporting and debugging. So initialize a line counter to zero
    //  and we'll use it to keep track of when a new line needs to be put out.
    //
    tCIDLib::TCard4 c4CurLine = 0;

    // Init the catch block entry flag
    m_bInCatch = kCIDLib::False;

    //
    //  Do a special case check for locals. We don't want to do it below
    //  since it can only happen before anything else. We can have multiple
    //  blocks with different attributes, just as with members. But, unlike
    //  members, we are doing both the declaration and initialization of
    //  the objects at the same time.
    //
    while (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::Locals))
        ParseLocals(psrcClass, meciWorking, methiWorking, methToFill);

    //
    //  Loop until we hit the end of method token. Until then, we expect each
    //  loop to handle one statement and generate the relevant opcodes.
    //
    tCIDLib::TBoolean       bDirectRef;
    tCIDLib::TBoolean       bDummy;
    tCIDLib::TCard2         c2RetClass;
    tCIDMacroEng::EExprTypes   eExprType;
    TMEngFlowCtrlItem       mefciTmp;
    TString                 strText;
    while (kCIDLib::True)
    {
        // Get the next token, which should be the next statement start
        tCIDMacroEng::ETokens eTok = psrcClass.eGetNextToken(strText);

        //
        //  Now, if the line has changed, put out a current line opcode, so
        //  that we'll have a trace point here.
        //
        if (psrcClass.c4Line() != c4CurLine)
        {
            c4CurLine = psrcClass.c4Line();
            m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::CurLine, c4CurLine);
            methToFill.c4AddOpCode(m_meopToUse);
        }

        //
        //  Loop until we see the end of the method or of the constructor,
        //  according to what kind we are dealing with. Make sure we get the
        //  end of the right kind and issue an error if not.
        //
        if (eTok == tCIDMacroEng::ETokens::EndMethod)
        {
            if (methiWorking.bIsCtor())
                IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedEndCtor);
            break;
        }
         else if (eTok == tCIDMacroEng::ETokens::EndConstructor)
        {
            if (!methiWorking.bIsCtor())
                IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedEndMethod);
            break;
        }

        //
        //  Ok, at this point we can see one of a set of statements. These
        //  are:
        //
        //  1.  Flow control statement, i.e. If, While, etc...
        //  2.  Return statement to provide an early return before hitting
        //      the end of the method.
        //  2.  Some statement which will start with a LHS target object, or
        //      with the current object as the implicit target.
        //
        switch(eTok)
        {
            case tCIDMacroEng::ETokens::Break :
            {
                // Check for a semicolon
                bCheckSemiColon(psrcClass);

                //
                //  Check the top of the flow control stack. It must be one of
                //  looping flow items (while, doloop).
                //
                if (m_colFlowStack.bIsEmpty())
                {
                    IssueErr(psrcClass, kMEngErrs::errcPrs_UnexpectedBreak);
                }
                 else
                {
                    TMEngFlowCtrlItem* pmefciCur;
                    if (bLastLoopedFlowItem(pmefciCur))
                    {
                        //
                        //  Add our current IP as a break offset to the current
                        //  flow item, then output a jump item with a bogus IP
                        //  for now. We'll update it at the end of the loop.
                        //
                        pmefciCur->AddBreakOffset(methToFill.c4CurOffset());
                        m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::Jump);
                        methToFill.c4AddOpCode(m_meopToUse);
                    }
                     else
                    {
                        IssueErr(psrcClass, kMEngErrs::errcPrs_UnexpectedBreak);
                    }
                }
                break;
            }

            case tCIDMacroEng::ETokens::Case :
            case tCIDMacroEng::ETokens::FTCase :
            case tCIDMacroEng::ETokens::Default :
            {
                // Make sure that this is legal. Tell it not to pop, just look
                if (!bCheckFlowStack(psrcClass, eTok, mefciTmp, kCIDLib::False))
                    ThrowUnrecoverable();

                //
                //  If the previous was an EndCase, then we can pop the stack
                //  since it's now served it's purpose, which is to insure
                //  we saw the previous case end before starting another.
                //
                //  This way, when we hit the end of the switch, we only
                //  have the start of the cases on the stack to deal with.
                //
                if (mefciTmp.eType() == tCIDMacroEng::EFlowTypes::EndCase)
                    m_colFlowStack.TrashTop();

                ParseSwitchCase(eTok, psrcClass, meciWorking, methiWorking, methToFill);
                break;
            }

            case tCIDMacroEng::ETokens::Catch :
            {
                // Set the flag that indicates we are in a catch
                m_bInCatch = kCIDLib::True;

                //
                //  Make sure that the top of the flow stack is a end try
                //  block, else this is not legal here. We tell it to pop,
                //  and 'll put a catch on the stack to be seen later by
                //  the end catch. This one is just for syntax checking not
                //  for any fixup, so we don't store any IP. Note that the
                //  original try is still on the stack above the newly pushed
                //  catch.
                //
                bCheckFlowStack(psrcClass, eTok, mefciTmp, kCIDLib::True);
                m_colFlowStack.objPush
                (
                    TMEngFlowCtrlItem
                    (
                        tCIDMacroEng::EFlowTypes::Catch, 0, psrcClass.c4Line()
                    )
                );
                break;
            }

            case tCIDMacroEng::ETokens::DoLoop :
            {
                //
                //  For this type, the check is at the end, so we just need
                //  to push the current IP on the flow stack, so that it can
                //  be used in the conditional jump back up to here when we
                //  hit the end LoopWhile.
                //
                m_colFlowStack.objPush
                (
                    TMEngFlowCtrlItem
                    (
                        tCIDMacroEng::EFlowTypes::DoLoop
                        , methToFill.c4CurOffset()
                        , psrcClass.c4Line()
                    )
                );
                break;
            }

            case tCIDMacroEng::ETokens::DoWhile :
            {
                // Check the flow stack to make sure this is legal
                if (bCheckFlowStack(psrcClass, eTok, mefciTmp, kCIDLib::True))
                {
                    //
                    //  Ok, we should see a boolean expression now, after
                    //  which we'll put out a conditional jump back to the
                    //  loop start if the expression is true.
                    //
                    bCheckOpenParen(psrcClass);
                    c2RetClass = c2ParseExpr
                    (
                        psrcClass
                        , tCIDMacroEng::EExprSides::RHS
                        , meciWorking
                        , methiWorking
                        , methToFill
                        , eExprType
                        , bDummy
                    );
                    bCheckCloseParen(psrcClass);
                    bCheckSemiColon(psrcClass);

                    if (c2RetClass != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Boolean))
                        IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedBoolExpr);

                    m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::CondJump, mefciTmp.c4Offset1());
                    methToFill.c4AddOpCode(m_meopToUse);

                    //
                    //  And update all of the breaks that might have been
                    //  generated within this block.
                    //
                    const tCIDLib::TCard4 c4Count = mefciTmp.c4BreakCount();
                    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
                    {
                        const tCIDLib::TCard4 c4BreakIP = mefciTmp.c4BreakOffsetAt(c4Index);
                        methToFill[c4BreakIP].c4Immediate(methToFill.c4CurOffset());
                    }
                }
                break;
            }


            case tCIDMacroEng::ETokens::EndCase :
            {
                // Check the stack, but no pop
                if (!bCheckFlowStack(psrcClass, eTok, mefciTmp, kCIDLib::False))
                    ThrowUnrecoverable();

                // We should see a semi-colon after this keyword
                bCheckSemiColon(psrcClass);

                //
                //  If this was a regular case, then we need to issue a
                //  jump opcode here, which at the end of the Switch will
                //  be updated to jump to the end of switch point. We store
                //  the current offset in the case's flow control item's
                //  first offset. If this is a fall through case, we don't
                //  do anything here, sine we just want to fall through
                //  to whatever is next. But we'll set the first offset to
                //  max card in order to cause an error should it later
                //  be incorrectly used.
                //
                TMEngFlowCtrlItem& mefciCur = m_colFlowStack.objPeek();
                if (mefciCur.eType() == tCIDMacroEng::EFlowTypes::Case)
                {
                    mefciCur.c4Offset1(methToFill.c4CurOffset());
                    m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::Jump);
                    methToFill.c4AddOpCode(m_meopToUse);
                }
                 else
                {
                    mefciCur.c4Offset1(kCIDLib::c4MaxCard);
                }

                // Next must be either another case or the end of the switch
                eTok = psrcClass.ePeekNextToken();
                if ((eTok != tCIDMacroEng::ETokens::Case)
                &&  (eTok != tCIDMacroEng::ETokens::EndSwitch)
                &&  (eTok != tCIDMacroEng::ETokens::FTCase)
                &&  (eTok != tCIDMacroEng::ETokens::Default))
                {
                    IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedCaseOrEnd);
                    ThrowUnrecoverable();
                }

                //
                //  Now we push an end case flow control item, just for
                //  bookkeeping purposes, so that when we see the next
                //  case or end of switch, we know that the previous one
                //  ended. It'll get poppped off when the next case or
                //  the end is seen.
                //
                m_colFlowStack.objPush
                (
                    TMEngFlowCtrlItem
                    (
                        tCIDMacroEng::EFlowTypes::EndCase, 0, psrcClass.c4Line()
                    )
                );
                break;
            }

            case tCIDMacroEng::ETokens::EndCatch :
            {
                // Indicate we are no longer in a catch block
                m_bInCatch = kCIDLib::False;

                // Check for a semicolon at the end of the block
                bCheckSemiColon(psrcClass);

                // Make sure the top of the flow stack is a catch, and pop it
                if (bCheckFlowStack(psrcClass, eTok, mefciTmp, kCIDLib::True))
                {
                    // And now get the original try off the stack
                    PopFlowStack(psrcClass, mefciTmp);

                    // Make sure it's a try
                    if (mefciTmp.eType() != tCIDMacroEng::EFlowTypes::Try)
                    {
                        IssueErr(psrcClass, kMEngErrs::errcDbg_ExpectedTryFlowItem);
                        ThrowUnrecoverable();
                    }

                    //
                    //  We now need to go back and update the opcode at the
                    //  second opcode of our try item, which is at the end of
                    //  the try block, with the current offset. This is where
                    //  it should jump when it gets to the end of the try
                    //  without any exception.
                    //
                    methToFill[mefciTmp.c4Offset2()].c4Immediate
                    (
                        methToFill.c4CurOffset()
                    );
                }
                break;
            }

            case tCIDMacroEng::ETokens::EndForEach :
            {
                // Check for a semicolon at the end of the block
                bCheckSemiColon(psrcClass);

                // Check the flow stack to make sure this is legal
                if (bCheckFlowStack(psrcClass, eTok, mefciTmp, kCIDLib::True))
                {
                    //
                    //  Generate a call to the enum object on the top of the
                    //  stack, to see if it is at it's max value now. If not,
                    //  then we want to increment it and jump back to the top
                    //  of the loop.
                    //
                    //  We use a special opcode here. It will do a conditional
                    //  increment on an enumerated object that is on the top of
                    //  stack. It will leave a boolean results on the stack.
                    //
                    m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::CondEnumInc);
                    methToFill.c4AddOpCode(m_meopToUse);

                    //
                    //  And generate a conditional jump back up to the top of
                    //  the stack. If the result is true, then it was
                    //  incremented and we need to do another loop, so we use
                    //  a conditional jump. It will pop the stack top.
                    //
                    m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::CondJump, mefciTmp.c4Offset1());
                    methToFill.c4AddOpCode(m_meopToUse);

                    //
                    //  Update all the jumps to the current offset, where we
                    //  are about to generate the opcode to pop the enum
                    //  object off the stack. For this type of loop, we only
                    //  can have breaks that jump out of the loop.
                    //
                    const tCIDLib::TCard4 c4Count = mefciTmp.c4BreakCount();
                    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
                    {
                        const tCIDLib::TCard4 c4BreakIP = mefciTmp.c4BreakOffsetAt(c4Index);
                        methToFill[c4BreakIP].c4Immediate(methToFill.c4CurOffset());
                    }

                    // Generate the opcode to pop the enum ref off the stack
                    m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::PopTop);
                    methToFill.c4AddOpCode(m_meopToUse);
                }
                break;
            }

            case tCIDMacroEng::ETokens::EndIf :
            {
                // Check for a semicolon at the end of the block
                bCheckSemiColon(psrcClass);

                //
                //  We have two scenarios. If it was just an If/EndIf, i.e.
                //  the current top of stack is an If, then we just need to
                //  update the notcond jump with this current offset. Else,
                //  we have to loop back up through the Else and IfElse
                //  blocks.
                //
                if (m_colFlowStack.bIsEmpty())
                {
                    IssueErr
                    (
                        psrcClass
                        , kMEngErrs::errcPrs_UnexpectedEndFlow
                        , strText
                    );
                    ThrowUnrecoverable();
                }

                TMEngFlowCtrlItem& mefciTop = m_colFlowStack.objPeek();

                if (mefciTop.eType() == tCIDMacroEng::EFlowTypes::If)
                {
                    TMEngOpCode& meopJump = methToFill[mefciTop.c4Offset1()];
                    meopJump.c4Immediate(methToFill.c4CurOffset());

                    // And toss this top of stack item
                    m_colFlowStack.objPop();
                }
                 else
                {
                    //
                    //  If the top item is an elseif, then it's conditional
                    //  jump after it's decision didn't get updated by a
                    //  subsequent item, and it doesn't need an unconditional
                    //  jump at the end, since it's end is the end. So we'll
                    //  handle it separately and pop it. If it's an else,
                    //  we can just pop it since it was just to insure correct
                    //  flow control.
                    //
                    if (mefciTop.eType() == tCIDMacroEng::EFlowTypes::ElseIf)
                    {
                        TMEngOpCode& meopJump = methToFill[mefciTop.c4Offset1()];
                        meopJump.c4Immediate(methToFill.c4CurOffset());
                        m_colFlowStack.objPop();
                    }
                     else if (mefciTop.eType() == tCIDMacroEng::EFlowTypes::Else)
                    {
                        m_colFlowStack.objPop();
                    }

                    while (kCIDLib::True)
                    {
                        // Get the next item off. If the check fails, break out
                        if (!bCheckFlowStack(psrcClass, eTok, mefciTmp, kCIDLib::True))
                            break;

                        //
                        //  Each of them has an unconditional jump at it's
                        //  second offset, which needs to be updated to jump
                        //  here after the block is taken.
                        //
                        TMEngOpCode& meopJump = methToFill[mefciTmp.c4Offset2()];
                        meopJump.c4Immediate(methToFill.c4CurOffset());

                        // Break out after we hit the If block
                        if (mefciTmp.eType() == tCIDMacroEng::EFlowTypes::If)
                            break;
                    }
                }
                break;
            }

            case tCIDMacroEng::ETokens::EndSwitch :
            {
                // Check for a semicolon at the end of the block
                bCheckSemiColon(psrcClass);

                //
                //  Make sure it's a legal place to see it. Tell it to
                //  pop, becasue it casn only legally be the end of the
                //  last case block, which we just push for bookmatching
                //  purposes, and it's served that purpose now.
                //
                if (bCheckFlowStack(psrcClass, eTok, mefciTmp, kCIDLib::True))
                {
                    //
                    //  Ok, we have to work backwards down the stack, finding
                    //  each case statement and updating it's end of block
                    //  jump to here. When we hit the switch one, we just
                    //  pop it off and break out.
                    //
                    TMEngFlowCtrlItem mefciCur;
                    while (kCIDLib::True)
                    {
                        PopFlowStack(psrcClass, mefciCur);
                        if (mefciCur.eType() == tCIDMacroEng::EFlowTypes::Switch)
                        {
                            TMEngOpCode& meopSwitch = methToFill[mefciCur.c4Offset1()];
                            TMEngJumpTable& jtblSwitch = methToFill.jtblById(meopSwitch[0]);
                            if (!jtblSwitch.bHasRequiredItems())
                                IssueErr(psrcClass, kMEngErrs::errcPrs_RequiredCases);
                            break;
                        }

                        // It better be a case block
                        if ((mefciCur.eType() != tCIDMacroEng::EFlowTypes::Case)
                        &&  (mefciCur.eType() != tCIDMacroEng::EFlowTypes::FTCase))
                        {
                            facCIDMacroEng().ThrowErr
                            (
                                CID_FILE
                                , CID_LINE
                                , kMEngErrs::errcDbg_ExpectedCaseFI
                                , tCIDLib::ESeverities::Failed
                                , tCIDLib::EErrClasses::Internal
                            );
                        }

                        //
                        //  Looks ok so update it's jump to here if it's
                        //  not a fall through case. If fall through,
                        //  then there isn't any jump because it's just
                        //  going to keep going at the next case.
                        //
                        if (mefciCur.eType() == tCIDMacroEng::EFlowTypes::Case)
                        {
                            methToFill[mefciCur.c4Offset1()].c4Immediate
                            (
                                methToFill.c4CurOffset()
                            );
                        }
                    }
                }
                break;
            }

            case tCIDMacroEng::ETokens::EndTry :
            {
                // Check for a semicolon at the end of the block
                bCheckSemiColon(psrcClass);

                //
                //  Make sure that the top of the flow stack is a try. Don't
                //  pop it, since we'll need it at the catch to fix up the
                //  jump.
                //
                if (bCheckFlowStack(psrcClass, eTok, mefciTmp, kCIDLib::False))
                {
                    // We got back a copy, we need to update the real thing
                    TMEngFlowCtrlItem& mefciCur = m_colFlowStack.objPeek();

                    // Generate a pop to get the try block off the stack now
                    m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::PopTop);
                    methToFill.c4AddOpCode(m_meopToUse);

                    //
                    //  We are at the end of the try, so put out a jump that
                    //  will take us over the catch block in the case of no
                    //  exception, but we don't know the target yet, so we'll
                    //  get the top item, which is currently using offset 1,
                    //  and we'll now use offset 2 as well. At the end of the
                    //  catch block, we'll come back to this.
                    //
                    mefciCur.c4Offset2(methToFill.c4CurOffset());
                    m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::Jump);
                    methToFill.c4AddOpCode(m_meopToUse);

                    //
                    //  We need to go back now and update the first offset,
                    //  which is the try opcode, with the current offset, which
                    //  is now the start of the catch block, i.e. where we want
                    //  to go if a throw occurs within this block.
                    //
                    methToFill[mefciCur.c4Offset1()].c4Immediate
                    (
                        methToFill.c4CurOffset()
                    );
                }

                //
                //  Push an EndTry onto the flow stack, which the Catch will
                //  check for. It's just for syntax checking, not for fixup, so
                //  no IP stored.
                //
                m_colFlowStack.objPush
                (
                    TMEngFlowCtrlItem
                    (
                        tCIDMacroEng::EFlowTypes::EndTry
                        , 0
                        , psrcClass.c4Line()
                    )
                );
                break;
            }

            case tCIDMacroEng::ETokens::EndWhile :
            {
                // Check for a semicolon at the end of the block
                bCheckSemiColon(psrcClass);

                //
                //  Check the top of the flow control stack. If it's not a
                //  while item, then this is not correct.
                //
                if (bCheckFlowStack(psrcClass, eTok, mefciTmp, kCIDLib::True))
                {
                    //
                    //  Generate an unconditional jump back to the to of the
                    //  while loop, which was stored in the first offset of
                    //  the flow item.
                    //
                    m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::Jump, mefciTmp.c4Offset1());
                    methToFill.c4AddOpCode(m_meopToUse);

                    //
                    //  And let's go back and update the forward conditional
                    //  jump opcode, to point to the next opcode after the
                    //  jump back up (i.e. the next opcode to be generated.)
                    //
                    methToFill[mefciTmp.c4Offset2()].c4Immediate(methToFill.c4CurOffset());

                    //
                    //  And update all of the breaks that might have been
                    //  generated within this block.
                    //
                    const tCIDLib::TCard4 c4Count = mefciTmp.c4BreakCount();
                    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
                    {
                        const tCIDLib::TCard4 c4BreakIP = mefciTmp.c4BreakOffsetAt(c4Index);
                        methToFill[c4BreakIP].c4Immediate(methToFill.c4CurOffset());
                    }
                }
                break;
            }

            case tCIDMacroEng::ETokens::Else :
            {
                //
                //  Check the top of the flow control stack. If it's not an if
                //  or elseif item, then this is not correct. In this case,
                //  tell it not to pop the stack, since we want to leave this
                //  on until we hit the end, because If's can have one else or
                //  multiple ElseIfs plus an Else and so forth. So we can't
                //  deal with all of the issues until we hit the EndIf.
                //
                //  We don't need to remember any offset for an else, but
                //  we need to push one in order to validate that we are
                //  seeing a legal EndIf when we see it.
                //
                if (bCheckFlowStack(psrcClass, eTok, mefciTmp, kCIDLib::False))
                {
                    TMEngFlowCtrlItem& mefciPrev = m_colFlowStack.objPeek();

                    //
                    //  Update the second offset of the previous item to look at
                    //  the end of it's block, and then output an unconditional
                    //  jump that will be updated to point at the end when we hit
                    //  the EndIf.
                    //
                    mefciPrev.c4Offset2(methToFill.c4CurOffset());
                    m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::Jump);
                    methToFill.c4AddOpCode(m_meopToUse);

                    //
                    //  Now we need to get the opcode of the first offset of the
                    //  previous item, and update it's target to jump to here
                    //  if the expression is false.
                    //
                    TMEngOpCode& meopJump = methToFill[mefciPrev.c4Offset1()];
                    meopJump.c4Immediate(methToFill.c4CurOffset());

                    // Push an item just for validating the EndIf
                    m_colFlowStack.objPush
                    (
                        TMEngFlowCtrlItem
                        (
                            tCIDMacroEng::EFlowTypes::Else
                            , 0
                            , psrcClass.c4Line()
                        )
                    );
                }
                break;
            }

            case tCIDMacroEng::ETokens::ForEach :
            {
                //
                //  Next we have to see an expression here which is a reference
                //  to an enumerated object.
                //
                bCheckOpenParen(psrcClass, tCIDMacroEng::ERecovers::No);
                c2RetClass = c2ParseExpr
                (
                    psrcClass
                    , tCIDMacroEng::EExprSides::RHS
                    , meciWorking
                    , methiWorking
                    , methToFill
                    , eExprType
                    , bDummy
                );
                bCheckCloseParen(psrcClass, tCIDMacroEng::ERecovers::No);

                // Make sure it's an object ref of some type
                if ((eExprType != tCIDMacroEng::EExprTypes::ObjectRef)
                &&  (eExprType != tCIDMacroEng::EExprTypes::ConstObjectRef))
                {
                    IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedEnumObj);
                    ThrowUnrecoverable();
                }

                //
                //  And get the object and make sure it's an enum. In the
                //  case of an object ref, the returned class id is the id
                //  of the object's class, so we can get it easily.
                //
                TMEngClassInfo& meciFor = m_pmeTarget->meciFind(c2RetClass);
                if ((meciFor.c2Id() != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Enum))
                &&  (meciFor.c2ParentClassId() != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Enum)))
                {
                    IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedEnumObj);
                    ThrowUnrecoverable();
                }

                //
                //  The boolean value is on the stack now. We will leave it
                //  there until we exit the loop, so that we can get back to
                //  the object. We need to generate a call here to reset the
                //  value, to make sure it's on it's first value before we
                //  enter the loop.
                //
                //  MAKE SURE it's not a const enum object, since we have to
                //  modify it. We can continue parsing, since it is at least
                //  an enum.
                //
                if (eExprType == tCIDMacroEng::EExprTypes::ConstObjectRef)
                {
                    IssueErr
                    (
                        psrcClass
                        , kMEngErrs::errcPrs_MemberIsConst
                        , TString(L"ForEach")
                    );
                }

                //
                //  Issue an opcode to reset the enum on the top of the stack.
                //  It just leaves it in place. We need this to get the enum
                //  we are going to iterate back to it's base value.
                //
                m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::ResetEnum);
                methToFill.c4AddOpCode(m_meopToUse);

                //
                //  For this type, the check is at the end, so we just need to
                //  push the current IP on the flow stack, so that it can be
                //  used in the conditional jump back up to here when we hit
                //  the end LoopWhile.
                //
                m_colFlowStack.objPush
                (
                    TMEngFlowCtrlItem
                    (
                        tCIDMacroEng::EFlowTypes::ForEach
                        , methToFill.c4CurOffset()
                        , psrcClass.c4Line()
                    )
                );
                break;
            }

            case tCIDMacroEng::ETokens::NoMatch :
            case tCIDMacroEng::ETokens::This :
            case tCIDMacroEng::ETokens::Parent :
            {
                //
                //  Put back the token, since it's going to be processed as
                //  an expression. So call the expression parser. When it
                //  comes back it will have parsed the current expression and
                //  generated all of the required code. It returns the class
                //  id of the return type, if any. In this case we want a LHS
                //  expression.
                //
                psrcClass.PushBack(eTok, strText);
                tCIDLib::TCard2 c2RetType = c2ParseExpr
                (
                    psrcClass
                    , tCIDMacroEng::EExprSides::LHS
                    , meciWorking
                    , methiWorking
                    , methToFill
                    , eExprType
                    , bDirectRef
                );

//                <TBD> We need a warning mechanism
//                if (c2RetType != tCIDMacroEng::EIntrinsics::Void)
//                    IssueWarning(psrcClass, kMEngErrs::errcPrs_UnusedReturn);

                // And we should have a semicolon at the end of an expression
                bCheckSemiColon(psrcClass);
                break;
            }

            case tCIDMacroEng::ETokens::Rethrow :
            {
                // Check for a semicolon. This guy has no parameters
                bCheckSemiColon(psrcClass);

                // Make sure that we in a catch block, else this isn't legal
                if (!m_bInCatch)
                    IssueErr(psrcClass, kMEngErrs::errcPrs_OnlyInCatch);

                //
                //  And just issue a throw, with the immediate boolean
                //  indicating a rethrow.
                //
                m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::Throw, kCIDLib::True);
                methToFill.c4AddOpCode(m_meopToUse);
                break;
            }

            case tCIDMacroEng::ETokens::Return :
            {
                //
                //  If this method's return type is void, then we should see a
                //  semi colon next. Else, we should see an expression for the
                //  return value.
                //
                if (methiWorking.c2RetClassId() != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Void))
                {
                    // See if they forgot the return value
                    if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::SemiColon))
                    {
                        IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedReturn);
                    }
                     else
                    {
                        c2RetClass = c2ParseExpr
                        (
                            psrcClass
                            , tCIDMacroEng::EExprSides::RHS
                            , meciWorking
                            , methiWorking
                            , methToFill
                            , eExprType
                            , bDirectRef
                        );

                        //
                        //  The expression type must be that of the method
                        //  return. We don't allow polymorphic return, since
                        //  it's always by value.
                        //
                        if (c2RetClass != methiWorking.c2RetClassId())
                        {
                            //
                            //  If we got a numeric literal value, then let's
                            //  see if we can force it to fit by converting
                            //  the type we saw or guessed (since they aren't
                            //  even typed unless they have a suffix) into
                            //  the type needed without a range issue.
                            //
                            if (eExprType == tCIDMacroEng::EExprTypes::NumLiteral)
                            {
                                TMEngOpCode& meopParm = methToFill.meopLast();
                                if (!meopParm.bConvertNumeric(methiWorking.c2RetClassId()))
                                    IssueErr(psrcClass, kMEngErrs::errcPrs_NotRetClass);
                            }
                             else
                            {
                                IssueErr(psrcClass, kMEngErrs::errcPrs_NotRetClass);
                            }
                        }

                        //
                        //  The result is on the stack top, so put out an opcode to
                        //  pop it to the return value on the stack.
                        //
                        m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::PopToReturn);
                        methToFill.c4AddOpCode(m_meopToUse);

                        bCheckSemiColon(psrcClass);
                    }
                }
                 else
                {
                    bCheckSemiColon(psrcClass);
                }

                // An generate a return opcode
                m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::Return);
                methToFill.c4AddOpCode(m_meopToUse);
                break;
            }

            case tCIDMacroEng::ETokens::Switch :
            {
                //
                //  We have to see an open paren, an expression, and a close
                //  paren. This is the expression that we will switch on.
                //  It must be cardinal, integral, or enumerated.
                //
                bCheckOpenParen(psrcClass);
                tCIDLib::TCard2 c2RetType = c2ParseExpr
                (
                    psrcClass
                    , tCIDMacroEng::EExprSides::RHS
                    , meciWorking
                    , methiWorking
                    , methToFill
                    , eExprType
                    , bDirectRef
                );
                bCheckCloseParen(psrcClass);

                TMEngClassInfo& meciSwitch = m_pmeTarget->meciFind(c2RetType);
                if ((c2RetType != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card1))
                &&  (c2RetType != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card2))
                &&  (c2RetType != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card4))
                &&  (c2RetType != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int1))
                &&  (c2RetType != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int2))
                &&  (c2RetType != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int4))
                &&  (c2RetType != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Char))
                &&  (meciSwitch.c2ParentClassId() != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Enum)))
                {
                    IssueErr(psrcClass, kMEngErrs::errcPrs_BadSwitchType);
                }

                //
                //  Add a jump table to the method, and remember it's id
                //  so that we can put it into the table jump opcode.
                //
                const tCIDLib::TCard2 c2TblId = methToFill.c2AddJumpTable();

                //
                //  Push a switch item onto the flow control stack, then put
                //  out the jump table opcode, which wil make the offset we
                //  just saved point to this switch opcode. We'll have to come
                //  back to it for each case and update the jump table.
                //
                m_colFlowStack.objPush
                (
                    TMEngFlowCtrlItem
                    (
                        tCIDMacroEng::EFlowTypes::Switch
                        , methToFill.c4CurOffset()
                        , psrcClass.c4Line()
                    )
                );
                m_meopToUse.SetDoubleIndex
                (
                    tCIDMacroEng::EOpCodes::TableJump
                    , c2TblId
                    , c2RetType
                );
                methToFill.c4AddOpCode(m_meopToUse);

                //
                //  The next thing we see must be the start of a case, so
                //  peek and make sure that's what it is, but we don't process
                //  it here.
                //
                eTok = psrcClass.ePeekNextToken();
                if ((eTok != tCIDMacroEng::ETokens::Case)
                &&  (eTok != tCIDMacroEng::ETokens::Default))
                {
                    IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedCase);
                    ThrowUnrecoverable();
                }
                break;
            }

            case tCIDMacroEng::ETokens::Throw :
            {
                //
                //  We have to see an open paren, an enumerated value
                //  either through a member, parm, or literal. We'll use this
                //  to put out a throw opcode, which at run time will create
                //  an exception with this error code and throw it.
                //
                bCheckOpenParen(psrcClass);
                tCIDLib::TCard2 c2RetType = c2ParseExpr
                (
                    psrcClass
                    , tCIDMacroEng::EExprSides::RHS
                    , meciWorking
                    , methiWorking
                    , methToFill
                    , eExprType
                    , bDirectRef
                );

                if (!m_pmeTarget->bIsDerivedFrom(c2RetType, tCIDMacroEng::EIntrinsics::Enum))
                    IssueErr(psrcClass, kMEngErrs::errcPrs_ThrowType);

                //
                //  If we don't get the semi-colon next, but get a comma
                //  instead, then we have a formatted version of the throw,
                //  which allows a number of formattable objects to be passed
                //  in, and their formatted representations used to replace
                //  tokens in the error text.
                //
                tCIDLib::TCard4 c4TokenCount = 0;
                if (!psrcClass.bIfPeeked(tCIDMacroEng::ETokens::CloseParen))
                {
                    bCheckComma(psrcClass);
                    while(kCIDLib::True)
                    {

                        //
                        //  Get an expression. It must be something derived
                        //  from formattable
                        //
                        c2RetType = c2ParseExpr
                        (
                            psrcClass
                            , tCIDMacroEng::EExprSides::RHS
                            , meciWorking
                            , methiWorking
                            , methToFill
                            , eExprType
                            , bDirectRef
                        );

                        if (!m_pmeTarget->bIsDerivedFrom(c2RetType, tCIDMacroEng::EIntrinsics::Formattable))
                            IssueErr(psrcClass, kMEngErrs::errcPrs_ThrowFmtType);

                        c4TokenCount++;

                        // Break out on the close paren
                        if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::CloseParen))
                            break;

                        // Don't allow more than 8 tokens
                        if (c4TokenCount > 8)
                            IssueErr(psrcClass, kMEngErrs::errcPrs_TooManyThrowTokens);

                        // It's got to be a comma and another formattable
                        bCheckComma(psrcClass);
                    }
                }

                // And either way we have a semicolon
                bCheckSemiColon(psrcClass);

                //
                //  If we have any tokens, we do the formatted throw, else
                //  the result throw.
                //
                if (c4TokenCount)
                    m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::ThrowFmt, c4TokenCount);
                else
                    m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::Throw, kCIDLib::False);
                methToFill.c4AddOpCode(m_meopToUse);
                break;
            }

            case tCIDMacroEng::ETokens::Try :
            {
                //
                //  Push an item for a try block, giving it the current IP,
                //  which is where we are about to push the try opcode.
                //
                m_colFlowStack.objPush
                (
                    TMEngFlowCtrlItem
                    (
                        tCIDMacroEng::EFlowTypes::Try
                        , methToFill.c4CurOffset()
                        , psrcClass.c4Line()
                    )
                );

                //
                //  And push the try block. The offset of the catch block,
                //  which needs to go in it, which we'll come back and fill
                //  in later.
                //
                m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::Try);
                methToFill.c4AddOpCode(m_meopToUse);
                break;
            }

            //
            //  DO NOT move this unless you move the If below, since we
            //  fall through into it!
            //
            case tCIDMacroEng::ETokens::ElseIf :
            {
                //
                //  We have to have an if/elseif on the stack before us, or
                //  it cannot be right.
                //
                if (!bCheckFlowStack(psrcClass, eTok, mefciTmp, kCIDLib::False))
                    ThrowUnrecoverable();

                TMEngFlowCtrlItem& mefciPrev = m_colFlowStack.objPeek();

                //
                //  Update the second offset of the previous item to look at
                //  the end of it's block, and then output an unconditional
                //  jump that will be updated to point at the end when we hit
                //  the EndIf.
                //
                mefciPrev.c4Offset2(methToFill.c4CurOffset());
                m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::Jump);
                methToFill.c4AddOpCode(m_meopToUse);

                //
                //  Now we need to get the opcode of the first offset of the
                //  previous item, and update it's target to jump to here
                //  if the expression is false.
                //
                TMEngOpCode& meopJump = methToFill[mefciPrev.c4Offset1()];
                meopJump.c4Immediate(methToFill.c4CurOffset());

                // Now fall through, since it's like an If now
            }

            //
            //  DO NOT move this unless you move the ElseIf above, since it
            //  falls through into here.
            //
            case tCIDMacroEng::ETokens::If :
            {
                // Now we should see an open paren
                bCheckOpenParen(psrcClass);

                tCIDMacroEng::EFlowTypes eType;
                if (eTok == tCIDMacroEng::ETokens::If)
                    eType = tCIDMacroEng::EFlowTypes::If;
                else
                    eType = tCIDMacroEng::EFlowTypes::ElseIf;

                // And now parse the expression. It must be of boolean type
                c2RetClass = c2ParseExpr
                (
                    psrcClass
                    , tCIDMacroEng::EExprSides::RHS
                    , meciWorking
                    , methiWorking
                    , methToFill
                    , eExprType
                    , bDirectRef
                );

                if (c2RetClass != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Boolean))
                    IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedBoolExpr);

                // Now we should see an close paren
                bCheckCloseParen(psrcClass);

                m_colFlowStack.objPush
                (
                    TMEngFlowCtrlItem
                    (
                        eType
                        , methToFill.c4CurOffset()
                        , psrcClass.c4Line()
                    )
                );

                m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::NotCondJump);
                methToFill.c4AddOpCode(m_meopToUse);
                break;
            }

            case tCIDMacroEng::ETokens::While :
            {
                // Now we should see an open paren
                bCheckOpenParen(psrcClass);

                //
                //  Next we will see an expression, which will control whether
                //  we take the while loop or jump over it. So, let's push a
                //  flow item onto the flow control stack, since the current
                //  IP is where we want to jump back to at the end of the loop
                //  if it's a while.
                //
                TMEngFlowCtrlItem& mefciCur = m_colFlowStack.objPush
                (
                    TMEngFlowCtrlItem
                    (
                        tCIDMacroEng::EFlowTypes::While
                        , methToFill.c4CurOffset()
                        , psrcClass.c4Line()
                    )
                );

                // And now parse the expression. It must be of boolean type
                c2RetClass = c2ParseExpr
                (
                    psrcClass
                    , tCIDMacroEng::EExprSides::RHS
                    , meciWorking
                    , methiWorking
                    , methToFill
                    , eExprType
                    , bDirectRef
                );

                if (c2RetClass != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Boolean))
                    IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedBoolExpr);

                // Now we should see an close paren
                bCheckCloseParen(psrcClass);

                //
                //  The expression will leave a boolean value on the stack.
                //  If it's true, we fall through, else we jump. So we use
                //  the NotCondJump opcode. It will pop the stack top after
                //  checking it's value.
                //
                //  Put this IP into the flow item we pushed as well, since
                //  we are going to have to come back and update the jump
                //  item to have the correct offset, once we get to the end
                //  of the block.
                //
                mefciCur.c4Offset2(methToFill.c4CurOffset());
                m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::NotCondJump);
                methToFill.c4AddOpCode(m_meopToUse);
                break;
            }

            default :
                IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedStatement);
                break;
        };
    }

    //
    //  Spit out a no-op opcode as the last opcode. We don't technically
    //  need it, since hitting the end will cause the method to end. But,
    //  if some control loop is the last thing in the method, it will want
    //  to jump past it's last opcode if the block isn't executed, and that
    //  will jump past the end of our opcode list. So, by putting this
    //  redundant return at the end, we insure that such blocks will have
    //  somewhere to jump to.
    //
    m_meopToUse.SetOpCode(tCIDMacroEng::EOpCodes::NoOp);
    methToFill.c4AddOpCode(m_meopToUse);
}


//
//  This method is called when parsing a method header. This guy will parse
//  out the parameters, if any, and return a collection of parameter info
//  objects that describe the parameters.
//
tCIDLib::TVoid
TMacroEngParser::ParseMethodParms(          TParserSrc&         psrcClass
                                    , const TMEngClassInfo&     meciTarget
                                    ,       TMEngMethodInfo&    methiToFill)
{
    //
    //  It has to start with an open paren. This one defaults to not trying
    //  to recover, which is fine for us here.
    //
    bCheckOpenParen(psrcClass);

    // Get a temp string from the pool
    TMEngStrPoolJan janName(m_pmsplParse);
    TString& strName = janName.strThis();

    tCIDLib::TCard4         c4Count = 0;
    tCIDMacroEng::EParmDirs    eDir;
    tCIDMacroEng::ETokens      eTok;
    while (kCIDLib::True)
    {
        // Break out on the close paren
        if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::CloseParen))
            break;

        // If not the first one, then we need a comma
        if (c4Count)
            bCheckComma(psrcClass);

        //
        //  We must first see the method direction attribute [In], [Out], or
        //  [InOut].
        //
        bCheckOpenBracket(psrcClass);
        eTok = psrcClass.eGetNextToken();
        if (eTok == tCIDMacroEng::ETokens::In)
            eDir = tCIDMacroEng::EParmDirs::In;
        else if (eTok == tCIDMacroEng::ETokens::Out)
            eDir = tCIDMacroEng::EParmDirs::Out;
        else if (eTok == tCIDMacroEng::ETokens::InOut)
            eDir = tCIDMacroEng::EParmDirs::InOut;
        else
        {
            // Make compiler happy
            eDir = tCIDMacroEng::EParmDirs::InOut;
            IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedParmAttr);
        }

        // Constructor parms must be In only
        if ((eDir != tCIDMacroEng::EParmDirs::In) && methiToFill.bIsCtor())
            IssueErr(psrcClass, kMEngErrs::errcPrs_CtorOutParm);

        if (eTok != tCIDMacroEng::ETokens::CloseBracket)
            bCheckCloseBracket(psrcClass);

        // Next we should get a class path (relative or full qualified)
        bGetClassPath(psrcClass, strName);

        // Resolve the path in case it's relative
        const TMEngClassInfo& meciParm = meciResolvePath(psrcClass, strName);

        // And next should be a name token
        bGetNameToken(psrcClass, strName, kCIDLib::False);

        //
        //  Make sure it's not a duplicate of another parameter or the
        //  name has been used elsewhere in the class.
        //
        if (methiToFill.bHasParm(strName))
            IssueErr(psrcClass, kMEngErrs::errcPrs_DupParmName, strName);

        if (meciTarget.bCheckDupName(*m_pmeTarget, strName))
            IssueErr(psrcClass, kMEngErrs::errcPrs_DupName, strName);

        // Looks reasonable, so let's add it
        methiToFill.c2AddParm(TMEngParmInfo(strName, meciParm.c2Id(), eDir));

        c4Count++;
    }
}


tCIDLib::TVoid
TMacroEngParser::ParseSwitchCase(const  tCIDMacroEng::ETokens  eOrgToken
                                ,       TParserSrc&         psrcClass
                                ,       TMEngClassInfo&     meciWorking
                                ,       TMEngMethodInfo&    methiWorking
                                ,       TMEngOpMethodImpl&  methToFill)
{
    // Get the item for the switch statement we are part of
    TMEngFlowCtrlItem* pmecfiSwitch;
    if (!bLastFlowItem(tCIDMacroEng::EFlowTypes::Switch, pmecfiSwitch))
    {
        // Shouldn't be possible, but we have to check
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcPrs_FlowNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
            , TString(L"Switch")
        );
    }

    //
    //  And find the opcode of the switch statement, which has the jump
    //  table in it. Then use it's jump table index to get a ref to the
    //  jump table for this switch statement.
    //
    TMEngOpCode& meopSwitch = methToFill[pmecfiSwitch->c4Offset1()];
    TMEngJumpTable& jtblSwitch = methToFill.jtblById(meopSwitch[0]);

    // Get a temp string from the pool
    TMEngStrPoolJan janName(m_pmsplParse);
    TString& strText = janName.strThis();

    //
    //  If this isn't the default case, we must see either a numeric
    //  literal, character literal, or enumerated value. If its the
    //  default, then just add the default case. Note that we can see
    //  multiple values per case, which allows for much less redundant
    //  code at the macro level. All it costs us is just pointing
    //  multiple items in the jump table to the same place.
    //
    if ((eOrgToken == tCIDMacroEng::ETokens::Case)
    ||  (eOrgToken == tCIDMacroEng::ETokens::FTCase))
    {
        tCIDLib::TCard4 c4Count = 0;
        while (kCIDLib::True)
        {
            tCIDMacroEng::ETokens eTok = psrcClass.eGetNextToken(strText);

            // If it's the colon, then we have them all
            if (eTok == tCIDMacroEng::ETokens::Colon)
                break;

            // If the count isn't zero, it has to be a comma
            if (c4Count)
            {
                if (eTok == tCIDMacroEng::ETokens::Comma)
                {
                    // Get another token, which is the actual value
                    eTok = psrcClass.eGetNextToken(strText);
                }
                 else
                {
                    // Complain about the missing comma, but keep going
                    IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedComma);
                }
            }

            // Remember how many we saw, since we have to see at least one
            c4Count++;

            tCIDLib::TCard2 c2Id = 0;
            TMEngClassVal*  pmecvCase = nullptr;
            if (eTok == tCIDMacroEng::ETokens::CharLiteral)
            {
                // Escape the literal first
                const tCIDLib::TCh chLit = psrcClass.chEscape(strText);
                pmecvCase = new TMEngCharVal
                (
                    L"CaseVal", tCIDMacroEng::EConstTypes::Const, chLit
                );
                c2Id = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Char);
            }
             else if (eTok == tCIDMacroEng::ETokens::NumericLiteral)
            {
                //
                //  This will return a null pointer if this literal isn't
                //  good or is out of range or something, and c2Id will be
                //  unchanged, so it'll be the zero we set above, which will
                //  tell us below that it's bad.
                //
                //  We use a version of this method that lets us pass in
                //  a hint as to what we are expecting, so that if the literal
                //  isn't explicitly typed, it'll try to get it to that type.
                //  If it cannot be converted to that type, it'll fail.
                //
                //  These have no name so we just pass in the empty string.
                //
                pmecvCase = pmecvMakeNumericLiteral
                (
                    psrcClass
                    , strText
                    , c2Id
                    , TString::strEmpty()
                    , m_pmeTarget->eXlatNumType(meopSwitch[1])
                );
            }
             else if (eTok == tCIDMacroEng::ETokens::NoMatch)
            {
                tCIDMacroEng::EConstTypes  eConst = tCIDMacroEng::EConstTypes::NonConst;
                tCIDMacroEng::ERefTypes    eRefType = tCIDMacroEng::ERefTypes::Unknown;
                const TMEngLiteralVal*  pmelvTarget;

                TMEngClassInfo* pmeciTarget = pmeciResolveRef
                (
                    psrcClass
                    , strText
                    , meciWorking
                    , methiWorking
                    , methToFill
                    , pmelvTarget
                    , eConst
                    , c2Id
                    , eRefType
                );

                // It's got to be an enum const or literal member type
                if (eRefType == tCIDMacroEng::ERefTypes::EnumConst)
                {
                    pmecvCase = new TMEngEnumVal
                    (
                        L"CaseVal"
                        , pmeciTarget->c2Id()
                        , tCIDMacroEng::EConstTypes::Const
                        , static_cast<TMEngEnumInfo*>(pmeciTarget)->c4ValueCount() - 1
                        , tCIDLib::TCard4(c2Id)
                    );
                    c2Id = pmeciTarget->c2Id();
                }
                 else if (eRefType == tCIDMacroEng::ERefTypes::LitMember)
                {
                    pmecvCase = pmelvTarget->pmecvMakeNew(L"CaseVal");
                    c2Id = pmelvTarget->c2ClassId();
                }
                 else
                {
                    IssueErr
                    (
                        psrcClass
                        , kMEngErrs::errcPrs_CantConvertLiteral
                        , m_pmeTarget->strClassPathForId(meopSwitch[1])
                    );
                    ThrowUnrecoverable();

                    // Won't happen, but makes analyzer happy
                    return;
                }
            }
             else
            {
                // Obviously bad
                IssueErr(psrcClass, kMEngErrs::errcPrs_CaseMustBeLiteral);
            }

            //
            //  Make sure it's of the type originally indicated in the
            //  switch statement.
            //
            if (c2Id != meopSwitch[1])
            {
                // Issue an error but keep going
                IssueErr(psrcClass, kMEngErrs::errcPrs_NotSwitchType);
            }
             else if (pmecvCase != nullptr)
            {
                // Make sure this case hasn't already been used
                tCIDLib::TCard4 c4Dummy;
                if (jtblSwitch.bFindMatch(*pmecvCase, c4Dummy))
                {
                    delete pmecvCase;
                    IssueErr(psrcClass, kMEngErrs::errcPrs_CaseIsUsed);
                }
                 else
                {
                    jtblSwitch.AddItem(methToFill.c4CurOffset(), pmecvCase);
                }
            }
        }

        //
        //  If we didn't get a case value, that's bad. Issue an error, but
        //  we can keep going.
        //
        if (!c4Count)
            IssueErr(psrcClass, kMEngErrs::errcPrs_NoCaseValue);
    }
     else
    {
        if (jtblSwitch.c4DefCaseIP() != kCIDLib::c4MaxCard)
            IssueErr(psrcClass, kMEngErrs::errcPrs_AlreadyDefCase);
        else
            jtblSwitch.AddDefaultItem(methToFill.c4CurOffset());

        // Check for the trailing colon
        bCheckColon(psrcClass);
    }

    //
    //  Push a case item on the stack. Just leave it unset for
    //  now. We'll update it when we get to the end of our case
    //  to point to the correct opcode, and then again at the
    //  end of the switch when we update the jump in that opcode (if
    //  i'ts not a fall-through type case.
    //
    m_colFlowStack.objPush
    (
        TMEngFlowCtrlItem
        (
            (eOrgToken == tCIDMacroEng::ETokens::Case) ? tCIDMacroEng::EFlowTypes::Case
                                                  : tCIDMacroEng::EFlowTypes::FTCase
            , 0
            , psrcClass.c4Line()
        )
    );
}


