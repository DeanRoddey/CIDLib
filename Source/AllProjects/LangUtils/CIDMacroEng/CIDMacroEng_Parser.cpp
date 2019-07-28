//
// FILE NAME: CIDMacroEng_Parser.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/27/2003
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
//  This file implements TMacroEngParser class, which provides parsing
//  services for macros.
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
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TMacroEngParser,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TMacroEngParser
// PREFIX: meprs
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
//  TMacroEngParser: Public, static methods
// ---------------------------------------------------------------------------

//
//  Scans a supposedly numeric literal token, and tries to figure out its
//  radix and type.
//
tCIDMacroEng::ENumTypes
TMacroEngParser::eCheckNumericLiteral(  TString&                strLit
                                        , tCIDLib::ERadices&    eRadix
                                        , tCIDLib::TBoolean&    bExplicit)
{
    // Scan it to make sure it's legal
    enum EStates
    {
        EState_Sign
        , EState_Initial
        , EState_LeadingZero
        , EState_Digits
        , EState_Decimal
        , EState_Fraction
        , EState_Suffix
        , EState_Suffix2
        , EState_End
    };

    // Assume no suffix until proven otherwise
    bExplicit = kCIDLib::False;

    // Assume Card4 until proven otherwise
    tCIDMacroEng::ENumTypes    eType = tCIDMacroEng::ENumTypes::Card4;
    const tCIDLib::TCard4   c4Len = strLit.c4Length();

    tCIDLib::TBoolean bHasSuffix = kCIDLib::False;
    EStates eCurState = EState_Sign;
    tCIDLib::TCard4 c4Index = 0;
    tCIDLib::TCard4 c4DigitCnt = 0;
    while (c4Index < c4Len)
    {
        const tCIDLib::TCh chCur = strLit[c4Index];
        switch(eCurState)
        {
            case EState_Sign :
            {
                // If either sign, then assume Int4 until proven otherwise
                if (chCur == kCIDLib::chHyphenMinus)
                {
                    eType = tCIDMacroEng::ENumTypes::Int4;
                    c4Index++;
                }
                 else if (chCur == kCIDLib::chPlusSign)
                {
                    eType = tCIDMacroEng::ENumTypes::Int4;
                    c4Index++;
                }
                eCurState = EState_Initial;
                break;
            }

            case EState_Initial :
            {
                if (chCur == kCIDLib::chDigit0)
                {
                    //
                    //  Not sure what is yet, so we have a special case for this.
                    //  But, we set the digit count provisionally to 1 and the radix
                    //  to decimal, in case this is the only character.
                    //
                    eRadix = tCIDLib::ERadices::Dec;
                    eCurState = EState_LeadingZero;
                    c4DigitCnt++;
                }
                 else if ((chCur >= kCIDLib::chDigit1)
                      &&  (chCur <= kCIDLib::chDigit9))
                {
                    eRadix = tCIDLib::ERadices::Dec;
                    c4DigitCnt++;
                    eCurState = EState_Digits;
                }
                 else
                {
                    return tCIDMacroEng::ENumTypes::None;
                }
                c4Index++;
                break;
            }

            case EState_LeadingZero :
            {
                if ((chCur == kCIDLib::chLatin_x) || (chCur == kCIDLib::chLatin_X))
                {
                    //
                    //  It's a hex prefix, so move to digits and take back the one
                    //  digit count we provisionally set above and override the initial
                    //  guess at the radix.
                    //
                    eRadix = tCIDLib::ERadices::Hex;
                    c4DigitCnt = 0;

                    // Now we can start checking actual digits
                    eCurState = EState_Digits;

                    // Eat this character
                    c4Index++;
                }
                 else if (chCur == kCIDLib::chPoundSign)
                {
                    //
                    //  Has to be a zero followed by a suffix. Keep the radix and digit
                    //  count we provsionally set above.
                    //
                    eCurState = EState_Suffix;

                    // Eat this character
                    c4Index++;
                }
                 else if (chCur == kCIDLib::chPeriod)
                {
                    //
                    //  Has to be a float in the form 0.xxx. Keep our one digit
                    //  count. We know it's dec radix now and can move on to processing
                    //  the fractional bit.
                    //
                    eCurState = EState_Fraction;
                    eRadix = tCIDLib::ERadices::Dec;
                    eType = tCIDMacroEng::ENumTypes::Float8;

                    // Eat this character
                    c4Index++;
                }
                 else
                {
                    //
                    //  Has to be an octal value. Move to digits state now to start
                    //  checking digits. Go back to zero digits
                    //
                    eRadix = tCIDLib::ERadices::Oct;
                    eCurState = EState_Digits;
                    c4DigitCnt = 0;

                    // Don't increment index!! This is a digit we want to process below
                }
                break;
            }

            case EState_Digits :
            {
                if (chCur == kCIDLib::chPeriod)
                {
                    // It must have already been a decimal radix
                    if (eRadix != tCIDLib::ERadices::Dec)
                        return tCIDMacroEng::ENumTypes::None;

                    // And we have to have seen at least one digit
                    if (!c4DigitCnt)
                        return tCIDMacroEng::ENumTypes::None;

                    eCurState = EState_Fraction;
                    eType = tCIDMacroEng::ENumTypes::Float8;
                }
                 else if (chCur == kCIDLib::chPoundSign)
                {
                    // Looks like a suffix
                    eCurState = EState_Suffix;

                    // We have to have seen at least one digit
                    if (!c4DigitCnt)
                        return tCIDMacroEng::ENumTypes::None;
                }
                 else
                {
                    if (eRadix == tCIDLib::ERadices::Oct)
                    {
                        if ((chCur < kCIDLib::chDigit0)
                        ||  (chCur > kCIDLib::chDigit7))
                        {
                            return tCIDMacroEng::ENumTypes::None;
                        }
                    }
                     else if (eRadix == tCIDLib::ERadices::Dec)
                    {
                        if ((chCur < kCIDLib::chDigit0)
                        ||  (chCur > kCIDLib::chDigit9))
                        {
                            return tCIDMacroEng::ENumTypes::None;
                        }
                    }
                     else if (eRadix == tCIDLib::ERadices::Hex)
                    {
                        if (!((chCur >= kCIDLib::chDigit0)
                        &&    (chCur <= kCIDLib::chDigit9))
                        &&  !((chCur >= kCIDLib::chLatin_a)
                        &&    (chCur <= kCIDLib::chLatin_f))
                        &&  !((chCur >= kCIDLib::chLatin_A)
                        &&    (chCur <= kCIDLib::chLatin_F)))
                        {
                            return tCIDMacroEng::ENumTypes::None;
                        }
                    }

                    // This is a valid digit
                    c4DigitCnt++;
                }
                c4Index++;
                break;
            }

            case EState_Fraction :
            {
                //
                //  Watch for decimal digits until the end or possibly suffix. WE don't
                //  have to actually see any decimal digits, even if there's a decimal
                //  point. It's assumed to just be zero.
                //
                if ((chCur >= kCIDLib::chDigit0) && (chCur <= kCIDLib::chDigit9))
                {
                    c4Index++;
                }
                 else if (chCur == kCIDLib::chPoundSign)
                {
                    eCurState = EState_Suffix;
                    c4Index++;
                }
                 else
                {
                    return tCIDMacroEng::ENumTypes::None;
                }
                break;
            }

            case EState_Suffix :
            {
                //
                //  We can see C (card), I (int), or F (float), else it cannot
                //  be correct.
                //
                if (chCur == kCIDLib::chLatin_C)
                    eType = tCIDMacroEng::ENumTypes::Card4;
                else if (chCur == kCIDLib::chLatin_F)
                    eType = tCIDMacroEng::ENumTypes::Float8;
                else if (chCur == kCIDLib::chLatin_I)
                    eType = tCIDMacroEng::ENumTypes::Int4;
                else
                    return tCIDMacroEng::ENumTypes::None;

                c4Index++;
                eCurState = EState_Suffix2;
                break;
            }

            case EState_Suffix2 :
            {
                if (eType == tCIDMacroEng::ENumTypes::Card4)
                {
                    switch(chCur)
                    {
                        case kCIDLib::chDigit1 :
                            eType = tCIDMacroEng::ENumTypes::Card1;
                            break;

                        case kCIDLib::chDigit2 :
                            eType = tCIDMacroEng::ENumTypes::Card2;
                            break;

                        case kCIDLib::chDigit4 :
                            eType = tCIDMacroEng::ENumTypes::Card4;
                            break;

                        case kCIDLib::chDigit8 :
                            eType = tCIDMacroEng::ENumTypes::Card8;
                            break;

                        default :
                            return tCIDMacroEng::ENumTypes::None;
                    };
                }
                 else if (eType == tCIDMacroEng::ENumTypes::Int4)
                {
                    switch(chCur)
                    {
                        case kCIDLib::chDigit1 :
                            eType = tCIDMacroEng::ENumTypes::Int1;
                            break;

                        case kCIDLib::chDigit2 :
                            eType = tCIDMacroEng::ENumTypes::Int2;
                            break;

                        case kCIDLib::chDigit4 :
                            eType = tCIDMacroEng::ENumTypes::Int4;
                            break;

                        default :
                            return tCIDMacroEng::ENumTypes::None;
                    };
                }
                 else if (eType == tCIDMacroEng::ENumTypes::Float8)
                {
                    switch(chCur)
                    {
                        case kCIDLib::chDigit4 :
                            eType = tCIDMacroEng::ENumTypes::Float4;
                            break;

                        case kCIDLib::chDigit8 :
                            eType = tCIDMacroEng::ENumTypes::Float8;
                            break;

                        default :
                            return tCIDMacroEng::ENumTypes::None;
                    };
                }
                 else
                {
                    return tCIDMacroEng::ENumTypes::None;
                }

                // Move to end state, since this is not a legal end state
                eCurState = EState_End;
                c4Index++;
                bHasSuffix = kCIDLib::True;
                break;
            }

            case EState_End :
            {
                //
                //  There's some trailing goop, so it's bad. Otherwise we would not
                //  have gotten here since we'd have hit the end of the input above
                //  and broken out of the loop.
                //
                return tCIDMacroEng::ENumTypes::None;
            }
        };
    }

    // If we never saw any actual digits, then can't be good
    if (!c4DigitCnt)
        return tCIDMacroEng::ENumTypes::None;

    // Saw a + or - sign and nothing else
    if (eCurState == EState_Initial)
        return tCIDMacroEng::ENumTypes::None;

    // Never saw the second character of the suffix
    if (eCurState == EState_Suffix2)
        return tCIDMacroEng::ENumTypes::None;

    // Remove the suffix if we found any
    if (bHasSuffix)
    {
        strLit.Cut(c4Len - 3);
        bExplicit = kCIDLib::True;
    }
    return eType;
}


// ---------------------------------------------------------------------------
//  TMacroEngParser: Constructors and Destructor
// ---------------------------------------------------------------------------
TMacroEngParser::TMacroEngParser(const tCIDMacroEng::EOptLevels eOpt) :

    m_bInCatch(kCIDLib::False)
    , m_c4ErrCount(0)
    , m_colFlowStack()
    , m_colMatches(tCIDLib::EAdoptOpts::NoAdopt, 8)
    , m_eOptLevel(eOpt)
    , m_pmecmToUse(nullptr)
    , m_pmeehToUse(nullptr)
    , m_pmeTarget(nullptr)
    , m_pmsplParse(new TMEngStringPool)
    , m_strThisName(L"this")
{
}

TMacroEngParser::~TMacroEngParser()
{
    // Clean up the string pool, which is all we own
    delete m_pmsplParse;
}


// ---------------------------------------------------------------------------
//  TMacroEngParser: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMacroEngParser::bParse(const   TString&                    strClassPath
                        ,       TMEngClassInfo*&            pmeciMainClass
                        ,       TCIDMacroEngine* const      pmeTarget
                        ,       MMEngPrsErrHandler* const   pmeehToUse
                        ,       MMEngClassMgr* const        pmecmToUse)
{
    // Store our pointers for this round
    m_pmecmToUse = pmecmToUse;
    m_pmeehToUse = pmeehToUse;
    m_pmeTarget  = pmeTarget;

    // Try to load the initial class
    TParserSrc psrcMain
    (
        m_pmecmToUse->pstrmLoadClass(strClassPath, tCIDMacroEng::EResModes::ReadOnly)
        , strClassPath
        , pmeTarget->bDebugMode()
    );

    // Reset the engine and any of our data members that need it
    m_pmeTarget->Reset();
    m_colFlowStack.RemoveAll();

    // Clear the error count before we start the parse
    m_c4ErrCount = 0;

    //
    //  Call the local recursive method that will handle parsing the target
    //  class, and all of the classes that it imports. When it get's back, it
    //  returns us the top level class, if no errors occur.
    //
    pmeciMainClass = pmeciParseClass(psrcMain);

    // Indicate whether errors occured or not
    return (m_c4ErrCount == 0);
}


tCIDLib::TBoolean
TMacroEngParser::bParse(const   TString&                    strClassPath
                        ,       TMemBuf&                    mbufSrc
                        , const tCIDLib::TCard4             c4Bytes
                        , const TString&                    strEncoding
                        ,       TMEngClassInfo*&            pmeciMainClass
                        ,       TCIDMacroEngine* const      pmeTarget
                        ,       MMEngPrsErrHandler* const   pmeehToUse
                        ,       MMEngClassMgr* const        pmecmToUse)
{
    // Store our pointers for this round
    m_pmecmToUse = pmecmToUse;
    m_pmeehToUse = pmeehToUse;
    m_pmeTarget  = pmeTarget;

    //
    //  If the encoding is empty, use an ASCII converter, else try to create
    //  a converter for that encoding.
    //
    TTextConverter* ptcvtSrc = facCIDEncode().ptcvtMake(strEncoding);
    if (!ptcvtSrc)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcPrs_BadEncoding
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strEncoding
            , strClassPath
        );
    }

    // Create an input text stream over the passed buffer
    TParserSrc psrcMain
    (
        new TTextMBufInStream
        (
            &mbufSrc
            , c4Bytes
            , tCIDLib::EAdoptOpts::NoAdopt
            , ptcvtSrc
        )
        , strClassPath
        , m_pmeTarget->bDebugMode()
    );

    // Reset the engine and any of our data members that need it
    m_pmeTarget->Reset();
    m_colFlowStack.RemoveAll();

    // Clear the error count before we start the parse
    m_c4ErrCount = 0;

    //
    //  Call the local recursive method that will handle parsing the target
    //  class, and all of the classes that it imports. When it get's back, it
    //  returns us the top level class, if no errors occur.
    //
    pmeciMainClass = pmeciParseClass(psrcMain);

    // Indicate whether errors occured or not
    return (m_c4ErrCount == 0);
}


// ---------------------------------------------------------------------------
//  TMacroEngParser: Private, non-virtual methods
// ---------------------------------------------------------------------------
TMEngClassInfo&
TMacroEngParser::meciResolvePath(TParserSrc& psrcClass, const TString& strPath)
{
    //
    //  Look it up as a type name. It could be ambiguous, because the type is
    //  often just the short form of the name and could be ambiguous, or it
    //  could be the long form. This method handles all of that and tells us
    //  the result.
    //
    //  This method will flush our collection before he refills it.
    //
    const tCIDMacroEng::EClassMatches eMatch = m_pmeTarget->eResolveClassName
    (
        strPath, m_colMatches
    );

    // If not found or ambiguous, give up
    if (eMatch == tCIDMacroEng::EClassMatches::NotFound)
    {
        IssueErr(psrcClass, kMEngErrs::errcEng_ClassNotFound, strPath);
        ThrowUnrecoverable();
    }
     else if (eMatch == tCIDMacroEng::EClassMatches::Ambiguous)
    {
        //
        //  Build up the list of classes that it could have been, since
        //  we need to put this into the message.
        //
        const tCIDLib::TCard4 c4Count = m_colMatches.c4ElemCount();
        TString strText;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            if (c4Index > 0)
                strText.Append(L", ");
            strText.Append(m_colMatches[c4Index]->strClassPath());
        }

        IssueErr
        (
            psrcClass
            , kMEngErrs::errcPrs_AmbiguousClass
            , strPath
            , strText
        );
        ThrowUnrecoverable();
    }

    // It's a legal class and non-ambiguous, so return the only one
    return *m_colMatches[0];
}


TMEngClassInfo*
TMacroEngParser::pmeciCheckClassLoad(       TParserSrc& psrcClass
                                    , const TString&    strClassPath)
{
    TMEngClassInfo* pmeciRet = m_pmeTarget->pmeciFind(strClassPath);

    if (!pmeciRet)
    {
        //
        //  We first check to see if it's an external class, so try to
        //  load it that way. If not, then we need to try to load it
        //  as a parseable macro.
        //
        pmeciRet = m_pmeTarget->pmeciLoadExternalClass(strClassPath);
        if (!pmeciRet)
        {
            //
            //  Use the class manager to try to load the text for this macro.
            //  If that doesn't work, we iss an error, but try to keep going.
            //
            TTextInStream* pstrmNew = m_pmecmToUse->pstrmLoadClass
            (
                strClassPath, tCIDMacroEng::EResModes::ReadOnly
            );

            if (pstrmNew)
            {
                //
                //  Create a local parser source and try to parse this
                //  imported class. If it works, then add it to our list
                //  of imported lcasses.
                //
                TParserSrc psrcMain
                (
                    pstrmNew, strClassPath, m_pmeTarget->bDebugMode()
                );
                pmeciRet = pmeciParseClass(psrcMain);
            }
             else
            {
                IssueErr
                (
                    psrcClass
                    , kMEngErrs::errcPrs_CantResolvePath
                    , strClassPath
                );
            }
        }
    }
    return pmeciRet;
}


TMEngClassInfo* TMacroEngParser::pmeciParseClass(TParserSrc& psrcClass)
{
    TMEngStdClassInfo* pmeciRet = 0;
    const tCIDLib::TCard4 c4InitNestDepth = m_colClassStack.c4ElemCount();
    try
    {
        //
        //  We have to parse a few pieces of information out before we can
        //  move forward and even create the target class to start filling
        //  it in. We have to get the class path and the parent class path,
        //  and optionally the extension type.
        //
        tCIDMacroEng::ETokens      eTok;
        tCIDMacroEng::EClassExt    eExtend;
        TString                 strBasePath;
        TString                 strClassPath;
        TString                 strName;

        // First we should see Class=
        bCheckNextToken(psrcClass, tCIDMacroEng::ETokens::Class);
        bCheckEquals(psrcClass);

        // Next we can optionally see the final/non-final/abstract attribute
        if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::OpenBracket))
        {
            // The next token must either be Final, Non-Final, or Abstract
            eTok = psrcClass.eGetNextToken(strName);

            if (eTok == tCIDMacroEng::ETokens::Abstract)
            {
                eExtend = tCIDMacroEng::EClassExt::Abstract;
            }
             else if (eTok == tCIDMacroEng::ETokens::Final)
            {
                eExtend = tCIDMacroEng::EClassExt::Final;
            }
             else if (eTok == tCIDMacroEng::ETokens::NonFinal)
            {
                eExtend = tCIDMacroEng::EClassExt::NonFinal;
            }
             else
            {
                IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedClassAttr, strName);
                ThrowUnrecoverable();
            }

            bCheckCloseBracket(psrcClass);
        }

        //
        //  Next we should see ClassPath and the class path for this class.
        //  It must match the path at which we found the current source we
        //  are parsing. If not, issue an error, but continue moving forward,
        //  keeping the passed class name.
        //
        bCheckNextToken(psrcClass, tCIDMacroEng::ETokens::ClassPath);
        if (bGetClassPath(psrcClass, strClassPath, tCIDMacroEng::ERecovers::Yes))
        {
            if (psrcClass.strPath() != strClassPath)
            {
                IssueErr
                (
                    psrcClass
                    , kMEngErrs::errcPrs_ClassPathMatch
                    , strClassPath
                    , psrcClass.strPath()
                );
                return 0;
            }
        }
         else
        {
            strClassPath = psrcClass.strPath();
        }
        bCheckSemiColon(psrcClass);
        facCIDMacroEng().SplitClassPath(strClassPath, strBasePath, strName);

        //
        //  Check to see if this class is already in the nesting stack,
        //  and if so, issue a circular dependency error. Else, push it
        //  onto the stack.
        //
        if (bIsOnNestingStack(strClassPath))
        {
            IssueErr(psrcClass, kMEngErrs::errcPrs_CircularImport, strClassPath);
            ThrowUnrecoverable();
        }
        m_colClassStack.objPush(strClassPath);

        // Next should be the parent class, so get it
        bCheckNextToken(psrcClass, tCIDMacroEng::ETokens::ParentClass);
        bGetClassPath(psrcClass, strClassPath);
        bCheckSemiColon(psrcClass);

        //
        //  If the parent class isn't loaded yet, then we need to load that
        //  one up. This might require a recursive call back into here.
        //
        if (!pmeciCheckClassLoad(psrcClass, strClassPath))
        {
            IssueErr(psrcClass, kMEngErrs::errcPrs_ParentFailed, strClassPath);
            ThrowUnrecoverable();
        }

        // Next we should see the end of the class attributes block
        bCheckNextToken(psrcClass, tCIDMacroEng::ETokens::EndClass);
        bCheckSemiColon(psrcClass);

        //
        //  If the parent class was final, then this is not legal, but we
        //  can continue the parse.
        //
        const TMEngClassInfo& meciParent = m_pmeTarget->meciFind(strClassPath);
        if (meciParent.eExtend() == tCIDMacroEng::EClassExt::Final)
            IssueErr(psrcClass, kMEngErrs::errcPrs_ParentClassIsFinal, strClassPath);

        //
        //  Create the target class that we are going to load up, since we
        //  now have enough info to create it, and we need it in order to
        //  start filling it in. Put a janitor on it until we get it done.
        //
        pmeciRet = new TMEngStdClassInfo
        (
            strName
            , strBasePath
            , *m_pmeTarget
            , strClassPath
        );
        TJanitor<TMEngStdClassInfo> janClass(pmeciRet);

        // Do the base init on this class
        pmeciRet->BaseClassInit(*m_pmeTarget);

        //
        //  And all of the intrinsics and their nested classes are magically
        //  imported.
        //
        const tCIDLib::TCard2 c2MaxIntrinsic = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Count);
        for (tCIDLib::TCard2 c2Ind = 0; c2Ind < c2MaxIntrinsic; c2Ind++)
            m_pmeTarget->meciFind(c2Ind).ExportTo(*m_pmeTarget, *pmeciRet);

        //
        //  All of the parent classes of an imported class are also implicitly
        //  imported (and using a base class is the same as importing it), as
        //  well as any of the nested types of those classes. So start at the
        //  new class.
        //
        meciParent.ExportTo(*m_pmeTarget, *pmeciRet);

        //
        //  Add the class now. If we don't do this now, then there will be
        //  no way to take a parameter of this type, which is required for
        //  things like assign and equality testing and so forth. Orphan
        //  it out of the janitor.
        //
        m_pmeTarget->c2AddClass(janClass.pobjOrphan());

        //
        //  Ok, we now start checking for the major sections of the class
        //  definition. For each one we find (some are optional), we then
        //  call a helper method to parse that particular unit. When that
        //  helper sees the end of that section, it returns back here.
        //
        //  First of all we look for directives, which is optional.
        //
        if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::Directives))
            ParseDirectives(psrcClass, *pmeciRet);

        // Imports, though almost always there, are optional
        if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::Imports))
            ParseImports(psrcClass, *pmeciRet);

        // Nested class types are optional
        if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::Types))
            ParseTypes(psrcClass, *pmeciRet);

        //
        //  Literals are optional. Since literals have no attributes, we
        //  just allow for one block.
        //
        if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::Literals))
            ParseLiterals(psrcClass, *pmeciRet);

        //
        //  Members, though almost always there, are optional. Note that we
        //  can have multiple members sections, each of which applies different
        //  attributes to the members defined. So we loop while we have a
        //  members block.
        //
        while (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::Members))
            ParseMembers(psrcClass, *pmeciRet);

        //
        //  And finally we have to have at least one Methods= block, since
        //  a class is useless without at least one method. But we can also
        //  have more than one, since each one can set different attributes
        //  on the methods it holds.
        //
        bCheckNextToken(psrcClass, tCIDMacroEng::ETokens::Methods);
        ParseMethods(psrcClass, *pmeciRet);

        //
        //  Now just run till we hit the EOF, handling any other method blocks
        //  until then. Nothing else is legal here. Use the name string as a
        //  temp here, since it's no longer needed. Indicate the EOF is ok here.
        //
        //  NOTE:   Method parsing is the most complex part of this parser
        //          by orders of magnitude, so it has it's own file to implement
        //          the method parsing.
        //
        eTok = psrcClass.eGetNextToken(strName, kCIDLib::True);
        while (eTok != tCIDMacroEng::ETokens::EOF)
        {
            // It's gotta be a methods block
            if (eTok != tCIDMacroEng::ETokens::Methods)
            {
                IssueErr
                (
                    psrcClass
                    , kMEngErrs::errcPrs_ExpectedToken
                    , psrcClass.strTokenText(tCIDMacroEng::ETokens::Methods)
                    , strName
                );
                ThrowUnrecoverable();
            }

            ParseMethods(psrcClass, *pmeciRet);
            eTok = psrcClass.eGetNextToken(strName, kCIDLib::True);
        }

        // Do some standard validation
        ValidateClass(psrcClass, *pmeciRet);
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged() && !m_pmeTarget->bInIDE())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
        IssueExcept(psrcClass, errToCatch);
        pmeciRet = 0;
    }

    catch(...)
    {
        IssueExcept(psrcClass);
        pmeciRet = 0;
    }

    // Make sure we get rid of any classes we put on the stack
    while (m_colClassStack.c4ElemCount() > c4InitNestDepth)
        m_colClassStack.objPop();

    return pmeciRet;
}



//
//  This method is called when a name token is seen and a reference to some
//  parameter or local or member is expected. If no match, it returns a null
//  pointer.
//
//  The parser doesn't allow any name clashes among these types of objects,
//  so we can search them in any order.
//
TMEngClassInfo*
TMacroEngParser::pmeciResolveRef(       TParserSrc&                 psrcClass
                                , const TString&                    strToFind
                                , const TMEngClassInfo&             meciSrc
                                , const TMEngMethodInfo&            methiSrc
                                , const TMEngMethodImpl&            methSrc
                                , const TMEngLiteralVal*&           pmelvFound
                                ,       tCIDMacroEng::EConstTypes&  eConst
                                ,       tCIDLib::TCard2&            c2Id
                                ,       tCIDMacroEng::ERefTypes&    eRefType)
{
    TMEngClassInfo*         pmeciRet = nullptr;
    const TMEngMemberInfo*  pmemiTarget = nullptr;
    const TMEngLocalInfo*   pmeliTarget = nullptr;
    const TMEngParmInfo*    pmepiTarget = nullptr;

    //
    //  First of all, check the specical case of an implicit call to 'this',
    //  so look for a method with this name. If we find one, see if the
    //  next token is an open paren. If so, assume that's it. Else, put it
    //  back and try the other options.
    //
    if (meciSrc.bHasMethod(strToFind))
    {
        if (psrcClass.ePeekNextToken() == tCIDMacroEng::ETokens::OpenParen)
        {
            eRefType = tCIDMacroEng::ERefTypes::ThisCall;
            eConst = methiSrc.eConst();
            c2Id = 0;
            return nullptr;
        }
    }

    //
    //  Do another special case one, which is literals. We do inherit these
    //  from our parent, but not by literal copying them into ourself (as we
    //  do for members) since that would we wasteful. So we have to check
    //  the target class first, then move up the parentage chain.
    //
    pmelvFound = meciSrc.pmelvFind(strToFind, *m_pmeTarget, kCIDLib::True);

    // If we found one, then it's our guy
    if (pmelvFound)
    {
        eConst = tCIDMacroEng::EConstTypes::Const;
        eRefType = tCIDMacroEng::ERefTypes::LitMember;
        c2Id = pmelvFound->c2ClassId();
        return nullptr;
    }

    //
    //  Ok, start at locals and move up through params and class members
    //  so that we find the most nested one first.
    //
    pmeliTarget = methSrc.pmeliFind(strToFind);
    if (pmeliTarget)
    {
        eConst = pmeliTarget->eConst();
        eRefType = tCIDMacroEng::ERefTypes::Local;
        pmeciRet = &m_pmeTarget->meciFind(pmeliTarget->c2ClassId());
        c2Id = pmeliTarget->c2Id();
    }
     else
    {
        pmepiTarget = methiSrc.pmepiFind(strToFind);
        if (pmepiTarget)
        {
            if (pmepiTarget->eDir() == tCIDMacroEng::EParmDirs::In)
                eConst = tCIDMacroEng::EConstTypes::Const;
            else
                eConst = tCIDMacroEng::EConstTypes::NonConst;
            eRefType = tCIDMacroEng::ERefTypes::Parm;
            pmeciRet = &m_pmeTarget->meciFind(pmepiTarget->c2ClassId());
            c2Id = pmepiTarget->c2Id();
        }
         else
        {
            // Indicate that we only want members from this class
            pmemiTarget = meciSrc.pmemiFind(strToFind, kCIDLib::True);
            if (pmemiTarget)
            {
                eConst = pmemiTarget->eConst();
                eRefType = tCIDMacroEng::ERefTypes::Member;
                pmeciRet = &m_pmeTarget->meciFind(pmemiTarget->c2ClassId());
                c2Id = pmemiTarget->c2Id();
            }
        }
    }

    //
    //  If we didn't find a match, then the only thing left is that it is a
    //  class path (partial or FQ), followed by enumerated type value or a
    //  named literal. Else, we are toast. So let's check that.
    //
    if (!pmeciRet)
    {
        // Get a couple of strings from the pool
        TMEngStrPoolJan janSP1(m_pmsplParse);
        TMEngStrPoolJan janSP2(m_pmsplParse);
        TString& strTmp1 = janSP1.strThis();
        TString& strTmp2 = janSP2.strThis();

        //
        //  We would have seen the first component of a path name (we would
        //  have stopped at the period), so there must be at least one more
        //  component (the enum value name) and more if it's fully qualified.
        //  So, we have to unget the name we got, and re-get it as class
        //  path. It will really be an enum or literal value, if legal, but
        //  it'll look like a class path to the parser, so this is fine.
        //
        strTmp1 = strToFind;
        psrcClass.PushBack(tCIDMacroEng::ETokens::NoMatch, strTmp1);
        tCIDLib::TCard4 c4LastSep;
        if (bGetClassPath(psrcClass, strTmp1, c4LastSep, kCIDLib::True, tCIDMacroEng::ERecovers::CheckOnly))
        {
            if (!c4LastSep)
            {
                IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedStatement);
                ThrowUnrecoverable();
            }

            // Separate the two parts out
            strTmp1.CopyOutSubStr(strTmp2, c4LastSep + 1);
            strTmp1.CapAt(c4LastSep);

            // And resolve the first part as a class path
            pmeciRet = &meciResolvePath(psrcClass, strTmp1);

            //
            //  If we came back from that, it was a legal class, check that
            //  it's an enum class and that the last part is a valid enum
            //  value for it. We cheat and return the ordinal via the c2Id
            //  out parm.
            //
            if (pmeciRet->c2ParentClassId() == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Enum))
            {
                //
                //  See if target class has imported this class into the target
                //  class. We don't have to test for intrinsics, since they have
                //  no enum values. We'll keep going but have to issue the error.
                //
                if (!meciSrc.bImportsClass(pmeciRet->strClassPath()))
                    IssueErr(psrcClass, kMEngErrs::errcPrs_ClassNotImported, strTmp1);

                TMEngEnumInfo* pmeciEnum = static_cast<TMEngEnumInfo*>(pmeciRet);
                const tCIDLib::TCard4 c4Ord = pmeciEnum->c4FindOrdinal(strTmp2);
                if (c4Ord == kCIDLib::c4MaxCard)
                {
                    IssueErr
                    (
                        psrcClass, kMEngErrs::errcPrs_BadEnumValue, strTmp2, strTmp1
                    );

                    // Just set the ordinal to zero, so we can keep going
                    c2Id = 0;
                }
                 else
                {
                    c2Id = tCIDLib::TCard2(c4Ord);
                }
                eRefType = tCIDMacroEng::ERefTypes::EnumConst;
            }
             else
            {
                //
                //  Not an enum, so see if it's a literal. In this case, the
                //  last parm says don't search parent classes, since they
                //  are refering to it via a specific class name.
                //
                pmelvFound = pmeciRet->pmelvFind(strTmp2, *m_pmeTarget, kCIDLib::False);
                if (pmelvFound)
                {
                    eConst = tCIDMacroEng::EConstTypes::Const;
                    eRefType = tCIDMacroEng::ERefTypes::LitMember;
                    c2Id = pmelvFound->c2ClassId();
                }
                 else
                {
                    IssueErr
                    (
                        psrcClass, kMEngErrs::errcPrs_ExpectedEnumLit, pmeciRet->strClassPath()
                    );
                    ThrowUnrecoverable();
                }
            }
        }
    }
    return pmeciRet;
}


//
//  This is called to parse the optional Directives= block of the class, if
//  it is present. This one is pretty simple so it's now worth it's own file.
//  We just pull out the directive name and value and store them on the engine
//
tCIDLib::TVoid
TMacroEngParser::ParseDirectives(TParserSrc& psrcClass,  TMEngClassInfo& meciToFill)
{
    // We need an equal sign after the open of the block
    bCheckEquals(psrcClass);

    //
    //  We loop until we hit an EndDirectives token, and until then each
    //  new loop around must be a directive in the form "name=value".
    //
    TString strName;
    TString strTemp;
    TString strValue;
    while (kCIDLib::True)
    {
        // This one is either the end directives, or a name
        if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::EndDirectives))
            break;

        //
        //  So we have to see a name token and an equals. We'll be tolerant
        //  on the equals, but we have to get a valid name. Indicate that
        //  we won't allow the name to match any keyword.
        //
        bGetNameToken(psrcClass, strName, kCIDLib::False);
        if (!bCheckEquals(psrcClass))
        {
            IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedDirective);
            psrcClass.EatLineRemainder();
            continue;
        }

        //
        //  Now we have to see a quoted string with the directive content,
        //  followed by a semicolon.
        //
        bGetQuotedString(psrcClass, strValue);
        bCheckSemiColon(psrcClass);

        //
        //  Se if this directive already exists for this class. If so, then
        //  lets issue an error that's being ignored.
        //
        if (meciToFill.bFindDirective(strName, strTemp))
            IssueErr(psrcClass, kMEngErrs::errcPrs_DupDirective);
        else
            meciToFill.AddDirective(strName, strValue);
    }

    // We should see a semi colon here at the end of the EndDirectives
    bCheckSemiColon(psrcClass);
}


//
//  This is called to parse the optional Imports= block of the class, if
//  it is present. In this case, each import either loads up a built in class,
//  or causes a recursive call to parse a new class, which then get's added
//  to the target engine, then we go back to the old one.
//
tCIDLib::TVoid
TMacroEngParser::ParseImports(TParserSrc& psrcClass, TMEngClassInfo& meciToFill)
{
    // We need an equal sign after the open of the block
    bCheckEquals(psrcClass);

    // Get some strings from the string pool
    TMEngStrPoolJan     janPath(m_pmsplParse);
    TMEngStrPoolJan     janTmp(m_pmsplParse);
    TString& strPath = janPath.strThis();
    TString& strTmp = janTmp.strThis();

    //
    //  We loop until we hit an EndDirectives token, and until then each
    //  new loop around must be a directive in the form "name=value".
    //
    while (kCIDLib::True)
    {
        // Break out on the end of imports statement
        if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::EndImports))
            break;

        //
        //  We should see either a classpath, or a dynamic type reference here. If
        //  a dyntype, then it will hold either an immediate string value or a literal
        //  from an already imported class (or a special case literal that can be
        //  defined by the containing application and set on the macro engine. The special
        //  case one is in the form $DynTypeRef.
        //
        if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::DynTypeRef))
        {
            //
            //  We have to have type name inside parens. It can either
            //  be an immediate string value, or a literal which has
            //  previously been defined.
            //
            bCheckOpenParen(psrcClass);

            //
            //  Get the next token and see what it is.
            //
            if (bGetClassPath(psrcClass, strTmp, tCIDMacroEng::ERecovers::CheckOnly))
            {
                //
                //  If it has no periods in it, assume it is an unqualified
                //  name of a local class literal, else split it and look up
                //  the class and get a literal from it.
                //
                tCIDLib::TCard4 c4LastSep;
                const TMEngLiteralVal* pmelvFound = 0;
                if (strTmp.bLastOccurrence(kCIDLib::chPeriod, c4LastSep))
                {
                    strTmp.CopyOutSubStr(strPath, c4LastSep + 1);
                    strTmp.CapAt(c4LastSep);

                    // Look up the class
                    TMEngClassInfo& meciTar = meciResolvePath(psrcClass, strTmp);

                    //
                    //  And see if it has a literal with the name. Tell it not
                    //  to recurse, since they gave a specific class name.
                    //
                    pmelvFound = meciTar.pmelvFind
                    (
                        strPath, *m_pmeTarget, kCIDLib::False
                    );
                }
                 else
                {
                    // Look in our own target class and recurse
                    pmelvFound = meciToFill.pmelvFind
                    (
                        strTmp, *m_pmeTarget, kCIDLib::True
                    );
                }

                //
                //  If not found, or found but not a string literal, then
                //  we are in error.
                //
                if (!pmelvFound
                ||  (pmelvFound->c2ClassId() != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::String)))
                {
                    IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedLitStr);
                    ThrowUnrecoverable();
                }

                strPath = pmelvFound->strValueAs();
            }
             else if (bGetQuotedString(psrcClass, strPath, tCIDMacroEng::ERecovers::Yes))
            {
                //
                //  If it's the special type ref, then get the value that has been set
                //  for that. Else take it as is.
                //
                if (strPath.bCompareI(kMacroEng::pszSpecDynTypRef))
                {
                    strPath = m_pmeTarget->strSpecialDynRef();

                    // It can't be empty
                    if (strPath.bIsEmpty())
                    {
                        IssueErr(psrcClass, kMEngErrs::errcPrs_EmptySpecDynRef);
                        ThrowUnrecoverable();
                    }
                }
            }
             else
            {
                // It wasn't a literal or a string
                IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedLitStr);
                ThrowUnrecoverable();
            }
            bCheckCloseParen(psrcClass);
        }
         else
        {
            // It's got to be a class path
            bGetClassPath(psrcClass, strPath);
        }
        bCheckSemiColon(psrcClass);

        //
        //  Ok, this is a new class that we either have to already have, or
        //  a built in that we can load, or we have to resolve it and parse
        //  it.
        //
        TMEngClassInfo* pmeciLoad = pmeciCheckClassLoad(psrcClass, strPath);
        if (!pmeciLoad)
        {
            IssueErr(psrcClass, kMEngErrs::errcEng_ClassNotFound, strPath);
            ThrowUnrecoverable();
        }

        //
        //  The target class needs to see all of the classes that are legally
        //  exposed by an imported class, so do an export.
        //
        pmeciLoad->ExportTo(*m_pmeTarget, meciToFill);
    }

    // We should see a semi colon here at the end of the EndDirectives
    bCheckSemiColon(psrcClass);
}


//
//  This is called to parse the optional Literals= block of the class, if it
//  is present. These are initialized in place, using only literal values,
//  and must be of char, string, boolean, or numeric types.
//
tCIDLib::TVoid
TMacroEngParser::ParseLiterals(TParserSrc& psrcClass, TMEngClassInfo& meciToFill)
{
    // We need an equal sign after the open of the block
    bCheckEquals(psrcClass);

    tCIDMacroEng::ETokens  eTok;

    // Get some strings from the string pool
    TMEngStrPoolJan     janName(m_pmsplParse);
    TMEngStrPoolJan     janText(m_pmsplParse);
    TString& strName = janName.strThis();
    TString& strText = janText.strThis();

    //
    //  We loop until we hit an EndLiterals token, and until then each new
    //  loop around must see a literal declaration.
    //
    while (kCIDLib::True)
    {
        // Break out on the end of members statement
        if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::EndLiterals))
            break;

        //
        //  Get classpath token. It might just be the short class name, but
        //  it could be a full classpath, and this method will handle either.
        //  The default is not to recover if it fails, which is appropirate
        //  since we have to know the type of something.
        //
        bGetClassPath(psrcClass, strName);
        TMEngClassInfo& meciTarget = meciResolvePath(psrcClass, strName);

        //
        //  It must be one of the fundamental types that we support for
        //  literals, i.e. char, boolean, string, or numeric.
        //
        if (!m_pmeTarget->bIsLiteralClass(meciTarget.c2Id()))
        {
            IssueErr(psrcClass, kMEngErrs::errcPrs_ClassNotImported, strName);
            ThrowUnrecoverable();
        }

        //
        //  Next we must have the name to assign the literal. It cannot be one
        //  of the non-code value key words. If we don't get the name, then
        //  just skip this one. The error was already issued.
        //
        if (!bGetNameToken(psrcClass, strName, kCIDLib::False))
            ThrowUnrecoverable();

        //
        //  Make sure that this name hasn't been used by any other literal,
        //  member or method.
        //
        if (meciTarget.bCheckDupName(*m_pmeTarget, strName))
            IssueErr(psrcClass, kMEngErrs::errcPrs_DupName, strName);

        // Now we should see an open paren for the initializer
        bCheckOpenParen(psrcClass);

        //
        //  And, now we should see a literal value appropriate for the type
        //  of the constant.
        //
        eTok = psrcClass.eGetNextToken(strText);

        TMEngClassVal* pmecvLit = 0;
        switch(tCIDMacroEng::EIntrinsics(meciTarget.c2Id()))
        {
            case tCIDMacroEng::EIntrinsics::Boolean :
                if ((eTok == tCIDMacroEng::ETokens::True)
                ||  (eTok == tCIDMacroEng::ETokens::False))
                {
                    pmecvLit = new TMEngBooleanVal
                    (
                        strName
                        , tCIDMacroEng::EConstTypes::Const
                        , (eTok == tCIDMacroEng::ETokens::True)
                    );
                }
                break;

            case tCIDMacroEng::EIntrinsics::Char :
                if (eTok == tCIDMacroEng::ETokens::CharLiteral)
                {
                    // Escape the literal
                    const tCIDLib::TCh chLit = psrcClass.chEscape(strText);
                    pmecvLit = new TMEngCharVal
                    (
                        strName, tCIDMacroEng::EConstTypes::Const, chLit
                    );
                }
                break;

            case tCIDMacroEng::EIntrinsics::String :
                if (eTok == tCIDMacroEng::ETokens::QuotedString)
                {
                    // Escape the string first, then store
                    psrcClass.EscapeStr(strText);
                    pmecvLit = new TMEngStringVal
                    (
                        strName
                        , tCIDMacroEng::EConstTypes::Const
                        , strText
                    );
                }
                break;

            case tCIDMacroEng::EIntrinsics::Card1 :
            case tCIDMacroEng::EIntrinsics::Card2 :
            case tCIDMacroEng::EIntrinsics::Card4 :
            case tCIDMacroEng::EIntrinsics::Card8 :
            case tCIDMacroEng::EIntrinsics::Float4 :
            case tCIDMacroEng::EIntrinsics::Float8 :
            case tCIDMacroEng::EIntrinsics::Int1 :
            case tCIDMacroEng::EIntrinsics::Int2 :
            case tCIDMacroEng::EIntrinsics::Int4 :
            {
                //
                //  Call a helper that will process the value and return us a
                //  literal object that has the correct info in it. This guy
                //  assumes actual literal values, so he doesn't give it name.
                //  We slip the name in after he gets back. We don't care what
                //  the actual type was, so we just use a dummy to eat that. It
                //  either was convertable to our known type, which we pass in
                //  or it failed.
                //
                tCIDLib::TCard2 c2Dummy;
                pmecvLit = pmecvMakeNumericLiteral
                (
                    psrcClass
                    , strText
                    , c2Dummy
                    , strName
                    , m_pmeTarget->eXlatNumType(meciTarget.c2Id())
                );
                break;
            }

            default :
                #if CID_DEBUG_ON
                facCIDMacroEng().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kMEngErrs::errcDbg_BadLiteralType
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Internal
                    , meciTarget.strClassPath()
                );
                #endif
                break;
        };

        //
        //  If the value object didn't get created, then complain about it
        //  but keep going and create a dummy one.
        //
        if (!pmecvLit)
        {
            IssueErr(psrcClass, kMEngErrs::errcPrs_BadLiteralInit);
            pmecvLit = new TMEngBooleanVal
            (
                strName
                , tCIDMacroEng::EConstTypes::Const
                , kCIDLib::True
            );
        }
         else
        {
            //
            //  If the immediate value type we got isn't the same as the
            //  target literal, then see if we can convert it.
            //
            if (pmecvLit->c2ClassId() != meciTarget.c2Id())
            {
                TMEngClassVal* pmecvTmp = meciTarget.pmecvMakeStorage
                (
                    strName
                    , *m_pmeTarget
                    , tCIDMacroEng::EConstTypes::Const
                );

                const tCIDMacroEng::ECastRes eRes = meciTarget.eCastFrom
                (
                    *m_pmeTarget
                    , *pmecvLit
                    , *pmecvTmp
                );

                if (eRes == tCIDMacroEng::ECastRes::Ok)
                {
                    // Delete the original and keep the new one
                    delete pmecvLit;
                    pmecvLit = pmecvTmp;
                }
                 else
                {
                    // Issue an error but keep going
                    IssueErr(psrcClass, kMEngErrs::errcPrs_BadLiteralInit);
                }
            }
        }

        //
        //  Add a new literal value object. It's like a member info, but
        //  doesn't need the const value (because they are always const,
        //  and it stores the value for later use. Note that, unlike other
        //  stuff, it has no idea, because it's always just referred to by
        //  name and the literal value looked up and pushed as an immedate
        //  value directly in an opcode. So we never have to refer to it
        //  at runtime.
        //
        meciToFill.AddLiteral
        (
            new TMEngLiteralVal(strName, meciTarget.c2Id(), pmecvLit)
        );

        // And we should see a close paren and semicolon
        bCheckCloseParen(psrcClass);
        bCheckSemiColon(psrcClass);
    }

    // We should see a semi colon here at the end of the EndMembers
    bCheckSemiColon(psrcClass);
}


tCIDLib::TVoid
TMacroEngParser::ParseLocals(   TParserSrc&             psrcClass
                                , TMEngClassInfo&       meciToFill
                                , TMEngMethodInfo&      methiToFill
                                , TMEngOpMethodImpl&    methToFill)
{
    // We need an equal sign after the open of the block
    bCheckEquals(psrcClass);

    tCIDMacroEng::ETokens  eTok;

    TMEngStrPoolJan     janName(m_pmsplParse);
    TMEngStrPoolJan     janText(m_pmsplParse);
    TString& strName = janName.strThis();
    TString& strText = janText.strThis();

    //
    //  First we have to get the block attributes out, if any. They follow
    //  the equal sign in brackets [], but they are optional. So if we don't
    //  see an open bracket, we just set default attributes and leave it at
    //  that.
    //
    tCIDMacroEng::EConstTypes eConst = tCIDMacroEng::EConstTypes::NonConst;

    if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::OpenBracket))
    {
        while (kCIDLib::True)
        {
            // Get the next token and check it
            eTok = psrcClass.eGetNextToken(strText);

            switch(eTok)
            {
                case tCIDMacroEng::ETokens::Const :
                {
                    eConst = tCIDMacroEng::EConstTypes::Const;
                    break;
                }

                case tCIDMacroEng::ETokens::NonConst :
                {
                    eConst = tCIDMacroEng::EConstTypes::NonConst;
                    break;
                }

                default :
                {
                    // It's not a valid attribute
                    IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedLocalAttr, strText);
                    break;
                }
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

    //
    //  We loop until we hit an EndLocals token, and until then each new
    //  loop around must see a member declaration.
    //
    while (kCIDLib::True)
    {
        if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::EndLocals))
            break;

        //
        //  Get classpath token. It might just be the short class name, but
        //  it could be a full classpath, and this method will handle either.
        //  The default is not to recover if it fails, which is appropirate
        //  since we have to know the type of something.
        //
        bGetClassPath(psrcClass, strName);
        TMEngClassInfo& meciTarget = meciResolvePath(psrcClass, strName);

        // It must be imported into this class, or be an intrinsic
        if (!meciToFill.bImportsClass(meciTarget.strClassPath())
        &&  !m_pmeTarget->bIsIntrinsicClass(meciTarget.c2Id()))
        {
            IssueErr(psrcClass, kMEngErrs::errcPrs_ClassNotImported, strName);
        }

        // It also has to be a non-abstract
        if (meciTarget.eExtend() == tCIDMacroEng::EClassExt::Abstract)
        {
            IssueErr(psrcClass, kMEngErrs::errcPrs_AbstractClass, strName);
            ThrowUnrecoverable();
        }

        //
        //  Next we must have the name to assign the member. It cannot be one
        //  of the non-code value key words. If we don't get the name, then
        //  just skip this one. The error was already issued.
        //
        tCIDLib::TCard2 c2LocalId = kMacroEng::c2BadId;
        if (bGetNameToken(psrcClass, strName, kCIDLib::False))
        {
            //
            //  This name cannot have been used so far as a member or literal
            //  or method name of this class, or a parm of this method. If so,
            //  issue the error but continue forward.
            //
            if (meciToFill.bCheckDupName(*m_pmeTarget, strName)
            ||  methiToFill.bHasParm(strName))
            {
                IssueErr(psrcClass, kMEngErrs::errcPrs_DupName, strName);
            }
             else
            {
                c2LocalId = methToFill.c2AddLocal
                (
                    TMEngLocalInfo(strName, meciTarget.c2Id(), eConst)
                );
            }
        }

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

        //
        //  If we see an open parent, we have to parse parms and match them
        //  to an appropriate constructor. Else, look for a default.
        //
        tCIDLib::TCard2 c2CtorToUse = kMacroEng::c2BadId;
        tCIDLib::TCard4 c4ParmCnt;
        if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::OpenParen))
        {
            // Find the right ctor, which will push the parameters
            c2CtorToUse = c2FindCorrectCtor
            (
                psrcClass
                , meciTarget
                , meciToFill
                , methiToFill
                , methToFill
                , c4ParmCnt
            );
        }
         else
        {
            c4ParmCnt = 0;
            const TMEngMethodInfo* pmethiCtor = meciTarget.pmethiDefCtor();
            if (pmethiCtor)
                c2CtorToUse = pmethiCtor->c2Id();
        }

        // We should see a trailing semicolon
        bCheckSemiColon(psrcClass);

        // If a match was found, then generate the call and cleanup.
        if (c2CtorToUse == kMacroEng::c2BadId)
        {
            IssueErr(psrcClass, kMEngErrs::errcPrs_NoCtorMatch, strName);
        }
         else
        {
            m_meopToUse.SetDoubleIndex
            (
                tCIDMacroEng::EOpCodes::CallLocal
                , c2LocalId
                , c2CtorToUse
            );
            methToFill.c4AddOpCode(m_meopToUse);

            // And pop the return type and parms
            m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::MultiPop, c4ParmCnt + 1);
            methToFill.c4AddOpCode(m_meopToUse);
        }
    }

    // We should see a semi colon here at the end of the EndLocals
    bCheckSemiColon(psrcClass);
}



//
//  This is called to parse the optional Members= block of the class, if it
//  is present.
//
tCIDLib::TVoid
TMacroEngParser::ParseMembers(TParserSrc& psrcClass, TMEngClassInfo& meciToFill)
{
    // We need an equal sign after the open of the block
    bCheckEquals(psrcClass);

    tCIDMacroEng::ETokens  eTok;

    TMEngStrPoolJan     janName(m_pmsplParse);
    TMEngStrPoolJan     janText(m_pmsplParse);
    TString& strName = janName.strThis();
    TString& strText = janText.strThis();

    //
    //  First we have to get the block attributes out, if any. They follow
    //  the equal sign in brackets [], but they are optional. So if we don't
    //  see an open bracket, we just set default attributes and leave it at
    //  that.
    //
    tCIDMacroEng::EConstTypes eConst = tCIDMacroEng::EConstTypes::NonConst;

    if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::OpenBracket))
    {
        while (kCIDLib::True)
        {
            // Get the next token and check it
            eTok = psrcClass.eGetNextToken(strText);

            switch(eTok)
            {
                case tCIDMacroEng::ETokens::Const :
                {
                    eConst = tCIDMacroEng::EConstTypes::Const;
                    break;
                }

                case tCIDMacroEng::ETokens::NonConst :
                {
                    eConst = tCIDMacroEng::EConstTypes::NonConst;
                    break;
                }

                default :
                {
                    // It's not a valid attribute
                    IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedMembAttr);
                    break;
                }
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

    //
    //  We loop until we hit an EndMembers token, and until then each new
    //  loop around must see a member declaration.
    //
    while (kCIDLib::True)
    {
        // Break out on the end of members statement
        if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::EndMembers))
            break;

        //
        //  Get classpath token. It might just be the short class name, but
        //  it could be a full classpath, and this method will handle either.
        //  The default is not to recover if it fails, which is appropirate
        //  since we have to know the type of something.
        //
        //  Note that it can also be a DynTypeRef() reference to create a
        //  type by name, so we have to check that. If we don't get a
        //  class path, then check for the dynamic type ref. We'll just slip
        //  the name we get into the same variable so the rest of the code
        //  doesn't need to know how we got it.
        //
        if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::DynTypeRef))
        {
            //
            //  We have to have type name inside parens. It can either
            //  be an immediate string value, or a literal which has
            //  previously been defined.
            //
            bCheckOpenParen(psrcClass);

            //
            //  Get the next token. It must be either a quoted string,
            //  or a literal of string type within our class or within
            //  another class.
            //
            if (bGetClassPath(psrcClass, strText, tCIDMacroEng::ERecovers::CheckOnly))
            {
                //
                //  If it has no periods in it, assume it is an unqualified
                //  name of a local class literal, else split it and look up
                //  the class and get a literal from it.
                //
                tCIDLib::TCard4 c4LastSep;
                const TMEngLiteralVal* pmelvFound = 0;
                if (strText.bLastOccurrence(kCIDLib::chPeriod, c4LastSep))
                {
                    strText.CopyOutSubStr(strName, c4LastSep + 1);
                    strText.CapAt(c4LastSep);

                    // Look up the class
                    TMEngClassInfo& meciTar = meciResolvePath(psrcClass, strText);

                    //
                    //  And see if it has a literal with the name. Tell it not
                    //  to recurse, since they gave a specific class name.
                    //
                    pmelvFound = meciTar.pmelvFind
                    (
                        strName
                        , *m_pmeTarget
                        , kCIDLib::False
                    );
                }
                 else
                {
                    // Look in our own target class and recurse
                    pmelvFound = meciToFill.pmelvFind
                    (
                        strText
                        , *m_pmeTarget
                        , kCIDLib::True
                    );
                }

                //
                //  If not found, or found but not a string literal, then
                //  we are in error.
                //
                if (!pmelvFound
                ||  (pmelvFound->c2ClassId() != tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::String)))
                {
                    IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedLitStr);
                    ThrowUnrecoverable();
                }

                strName = pmelvFound->strValueAs();
            }
             else if (bGetQuotedString(psrcClass, strName, tCIDMacroEng::ERecovers::Yes))
            {
                //
                //  If it's the special type ref, then get the value that has been set
                //  for that. Else we take it as is.
                //
                if (strName.bCompareI(kMacroEng::pszSpecDynTypRef))
                {
                    strName= m_pmeTarget->strSpecialDynRef();

                    // It can't be empty
                    if (strName.bIsEmpty())
                    {
                        IssueErr(psrcClass, kMEngErrs::errcPrs_EmptySpecDynRef);
                        ThrowUnrecoverable();
                    }
                }
            }
             else
            {
                // It wasn't a literal or a string
                IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedLitStr);
                ThrowUnrecoverable();
            }

            bCheckCloseParen(psrcClass);
        }
         else
        {
            // It's got to be a class path
            bGetClassPath(psrcClass, strName);
        }
        TMEngClassInfo& meciTarget = meciResolvePath(psrcClass, strName);

        // It must be imported into this class, or be an intrinsic
        if (!meciToFill.bImportsClass(meciTarget.strClassPath())
        &&  !m_pmeTarget->bIsIntrinsicClass(meciTarget.c2Id()))
        {
            IssueErr(psrcClass, kMEngErrs::errcPrs_ClassNotImported, strName);
        }

        // It also has to be a non-abstract
        if (meciTarget.eExtend() == tCIDMacroEng::EClassExt::Abstract)
        {
            IssueErr(psrcClass, kMEngErrs::errcPrs_AbstractClass, strName);
            ThrowUnrecoverable();
        }

        //
        //  Next we must have the name to assign the member. It cannot be one
        //  of the non-code value key words. If we don't get the name, then
        //  just skip this one. The error was already issued.
        //
        if (bGetNameToken(psrcClass, strName, kCIDLib::False))
        {
            //
            //  This name cannot have been used so far as a member, literal,
            //  or method name of this class. If so, issue the error but
            //  continue forward.
            //
            if (meciToFill.bCheckDupName(*m_pmeTarget, strName))
            {
                IssueErr(psrcClass, kMEngErrs::errcPrs_DupName, strName);
            }
             else
            {
                meciToFill.c2AddMember
                (
                    TMEngMemberInfo(strName, meciTarget.c2Id(), eConst)
                    , meciTarget
                );
            }
        }

        // And we have to have a trailing semicolon
        bCheckSemiColon(psrcClass);
    }

    // We should see a semi colon here at the end of the EndMembers
    bCheckSemiColon(psrcClass);
}


tCIDLib::TVoid
TMacroEngParser::ParseTypes(TParserSrc& psrcClass, TMEngClassInfo& meciOwner)
{
    // We need an equal sign after the open of the block
    bCheckEquals(psrcClass);

    // Get a some strings from the pool
    TMEngStrPoolJan janName(m_pmsplParse);
    TMEngStrPoolJan janText(m_pmsplParse);
    TMEngStrPoolJan janTmp(m_pmsplParse);
    TString& strName = janName.strThis();
    TString& strText = janText.strThis();
    TString& strTmp = janTmp.strThis();

    //
    //  We loop until we hit an EndTypes token, and until then each new
    //  loop around must see a type declaration.
    //
    tCIDMacroEng::ETokens  eTok;
    while (kCIDLib::True)
    {
        // Break out on the end of types statement
        if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::EndTypes))
            break;

        //
        //  We have to see Enum or one of the parameterized collection types
        //  here.
        //
        eTok = psrcClass.eGetNextToken(strText);

        if (eTok == tCIDMacroEng::ETokens::Enum)
        {
            bCheckEquals(psrcClass);

            //
            //  And now we need a name token, which will be the name of the
            //  new type. It cannot match any reserved name, and if we don't get
            //  it, we won't try to recover.
            //
            bGetNameToken(psrcClass, strName, kCIDLib::False, tCIDMacroEng::ERecovers::No);

            //
            //  This is going to create a new class, so create the new classpath
            //  and see if a class of this type exists already. It will be the
            //  owning class' classpath, plus our name.
            //
            strText = meciOwner.strClassPath();
            strText.Append(kCIDLib::chPeriod);
            strText.Append(strName);

            //
            //  If it already exists, we can't use it, so issue an error, but
            //  go ahead and parse it. We'll just not add it at the end. This
            //  way we can keep going.
            //
            const tCIDLib::TBoolean bIsDup = m_pmeTarget->bClassIsLoaded(strText);
            if (bIsDup)
                IssueErr(psrcClass, kMEngErrs::errcEng_DupClassName, strText);

            //
            //  Looks ok, so let's create a new class info object for this new
            //  type. It will use the C++ class used to represent enums. It
            //  will derive from the MEng.Enum class. Put a janitor on it
            //  until we get it filled in. We don't know how many items it's
            //  going to have, so give an initial guess of 8.
            //
            TMEngEnumInfo* pmeciNew = new TMEngEnumInfo
            (
                *m_pmeTarget
                , strName
                , meciOwner.strClassPath()
                , L"MEng.Enum"
                , 8
            );
            TJanitor<TMEngEnumInfo> janNewClass(pmeciNew);

            //
            //  Export ourself to the enclosing class and, since that
            //  caused us to be added as a regular import, force us to
            //  be a nested class.
            //
            pmeciNew->ExportTo(*m_pmeTarget, meciOwner);
            meciOwner.SetToNested(pmeciNew->strClassPath());

            //
            //  Ok, let's parse enum items until we hit the EndEnum marker.
            //  Each round should get us one item in the form:
            //
            //      "name : textval".
            //
            //  The ordinal value is implicit in the ordering, so we only
            //  have to parse the name and text representation.
            //
            while (kCIDLib::True)
            {
                // Break out when we hit the EndEnum
                if (psrcClass.bIfPeeked(tCIDMacroEng::ETokens::EndEnum))
                    break;

                //
                //  We have to see a name token here, fail if we don't get one.
                //  If we do, then we should get a colon separator.
                //
                bGetNameToken(psrcClass, strName, kCIDLib::False, tCIDMacroEng::ERecovers::No);
                bCheckColon(psrcClass);

                // And next we have to get a quoted string
                bGetQuotedString(psrcClass, strText, tCIDMacroEng::ERecovers::Yes);

                // Ok, add this item, if it doesn't already exist
                if (pmeciNew->bItemExists(strName))
                    IssueErr(psrcClass, kMEngErrs::errcPrs_DupEnumItem, strName);
                 else
                    pmeciNew->c4AddEnumItem(strName, strText);

                // And it should end with a semicolon
                bCheckSemiColon(psrcClass);
            }

            if (!pmeciNew->c4ValueCount())
                IssueErr(psrcClass, kMEngErrs::errcEng_EmptyEnum, pmeciNew->strClassPath());

            //
            //  If it was a dup, then let the janitor toss it, else do the base
            //  init and then add it.
            //
            if (!bIsDup)
            {
                pmeciNew->BaseClassInit(*m_pmeTarget);
                m_pmeTarget->c2AddClass(janNewClass.pobjOrphan());
            }

            // And the enum declaration should end with a semicolon
            bCheckSemiColon(psrcClass);
        }
         else if ((eTok == tCIDMacroEng::ETokens::VectorOf)
              ||  (eTok == tCIDMacroEng::ETokens::ArrayOf))
        {
            //
            //  It's one of the collection types. So we must see a type
            //  name inside [] brackets. The defaults are to continue if
            //  the brackets are missing, but to give up if the class path
            //  cannot be found.
            //
            bCheckOpenBracket(psrcClass);
            bGetClassPath(psrcClass, strText);
            bCheckCloseBracket(psrcClass);

            // Resolve the class name
            const TMEngClassInfo& meciInstType = meciResolvePath(psrcClass, strText);
            const TString& strInstClass = meciInstType.strClassPath();

            // It must be imported into this class, or be an intrinsic
            if (!meciOwner.bImportsClass(strInstClass)
            &&  !m_pmeTarget->bIsIntrinsicClass(meciInstType.c2Id()))
            {
                IssueErr(psrcClass, kMEngErrs::errcPrs_ClassNotImported, strInstClass);
            }

            // It also has to be a non-abstract
            if (meciInstType.eExtend() == tCIDMacroEng::EClassExt::Abstract)
            {
                IssueErr(psrcClass, kMEngErrs::errcPrs_AbstractClass, strInstClass);
                ThrowUnrecoverable();
            }

            //
            //  And for certain types of collections it must have a default
            //  constructor so that the collection can be populated, e.g. for
            //  arrays which get their size up front and need to provide
            //  values.
            //
            if (eTok == tCIDMacroEng::ETokens::ArrayOf)
            {
                //
                //  NOTE:   We cannot call pmethDefCtor() here, since that
                //          has to return an actual method impl, but if the
                //          class is external (i.e. just wrapping C++), then
                //          there won't be one. bHasDefCtor() will work for
                //          external classes and is more efficient to boot.
                //
                if (!meciInstType.bHasDefCtor())
                    IssueErr(psrcClass, kMEngErrs::errcClass_NoDefCtor, strInstClass);
            }

            // And now we must see the name we'll give this new type
            bGetNameToken(psrcClass, strName, kCIDLib::False, tCIDMacroEng::ERecovers::No);

            // Build up the full class path of this new class
            strTmp = meciOwner.strClassPath();
            strTmp.Append(kCIDLib::chPeriod);
            strTmp.Append(strName);

            //
            //  See if its a dup of some existing class name. If so, issue an
            //  error and don't generate the new class, but keep going. If
            //  not, then we can create the new class.
            //
            const tCIDLib::TBoolean bIsDup = m_pmeTarget->bClassIsLoaded(strTmp);
            if (bIsDup)
            {
                IssueErr(psrcClass, kMEngErrs::errcEng_DupClassName, strTmp);
            }
             else
            {
                TMEngColBaseInfo* pmeciNew = 0;
                if (eTok == tCIDMacroEng::ETokens::VectorOf)
                {
                    pmeciNew = new TMEngVectorInfo
                    (
                        *m_pmeTarget
                        , strName
                        , meciOwner.strClassPath()
                        , TMEngVectorInfo::strPath()
                        , meciInstType.c2Id()
                    );
                }
                 else if (eTok == tCIDMacroEng::ETokens::ArrayOf)
                {
                    pmeciNew = new TMEngArrayInfo
                    (
                        *m_pmeTarget
                        , strName
                        , meciOwner.strClassPath()
                        , TMEngArrayInfo::strPath()
                        , meciInstType.c2Id()
                    );
                }
                TJanitor<TMEngColBaseInfo> janNewClass(pmeciNew);
                pmeciNew->BaseClassInit(*m_pmeTarget);
                m_pmeTarget->c2AddClass(janNewClass.pobjOrphan());

                //
                //  Export ourself to the enclosing class and, since that
                //  caused us to be added as a regular import, force us to
                //  be a nested class.
                //
                pmeciNew->ExportTo(*m_pmeTarget, meciOwner);
                meciOwner.SetToNested(pmeciNew->strClassPath());
            }

            // And the enum declaration should end with a semicolon
            bCheckSemiColon(psrcClass);
        }
         else
        {
            IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedNestedType);
            ThrowUnrecoverable();
        }
    }

    // We should see a semi colon here at the end of the EndTypes
    bCheckSemiColon(psrcClass);
}


tCIDLib::TVoid TMacroEngParser::ThrowUnrecoverable()
{
    facCIDMacroEng().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kMEngErrs::errcPrs_Unrecoverable
        , tCIDLib::ESeverities::Status
        , tCIDLib::EErrClasses::AppStatus
    );
}


//
//  This guy will go through and make sure that every method defined as this
//  class level has an implementation at this level.
//
//  We can only do this if it's not a built in class, since built in classes
//  never have method implementations. They are just wrappers around C++
//  code.
//
tCIDLib::TVoid
TMacroEngParser::ValidateClass(         TParserSrc&     psrcClass
                                , const TMEngClassInfo& meciToVal)
{
    // Make sure that we got at least one constructor
    if (!meciToVal.c4CtorCount())
        IssueErr(psrcClass, kMEngErrs::errcPrs_NoCtors, meciToVal.strClassPath());

    //
    //  And run through and make sure that any Required methods got
    //  overridden. We can tell by just running through all the methods
    //  we inherited and making sure none are still marked required.
    //
    tCIDLib::TCard2 c2Count = meciToVal.c2FirstMethodId();
    for (tCIDLib::TCard2 c2Id = 0; c2Id < c2Count; c2Id++)
    {
        const TMEngMethodInfo& methiCur = meciToVal.methiFind(c2Id);
        if (methiCur.eExtend() == tCIDMacroEng::EMethExt::Required)
        {
            IssueErr
            (
                psrcClass
                , kMEngErrs::errcPrs_ReqMethodNotOver
                , methiCur.strName()
            );
        }
    }

    // If in validation mode, then do some extra checks
    if (m_pmeTarget->bValidation())
    {
        const tCIDLib::TCard4 c4Count = meciToVal.c4MethodCount();
        tCIDLib::TCard2 c2Id = meciToVal.c2FirstMethodId();

        for (; c2Id < c4Count; c2Id++)
        {
            const TMEngMethodInfo& methiCur = meciToVal.methiFind(c2Id);
            const TMEngMethodImpl* pmethCur = meciToVal.pmethFind(methiCur.strName());
            if (!pmethCur)
            {
                IssueErr
                (
                    psrcClass
                    , kMEngErrs::errcClass_InfoWithoutImpl
                    , meciToVal.strClassPath()
                    , methiCur.strName()
                );
            }

            if (pmethCur->c2Id() != methiCur.c2Id())
            {
                IssueErr
                (
                    psrcClass
                    , kMEngErrs::errcClass_ImplInfoIdMatch
                    , methiCur.strName()
                    , TCardinal(pmethCur->c2Id())
                    , TCardinal(methiCur.c2Id())
                );
            }
        }
    }
}


