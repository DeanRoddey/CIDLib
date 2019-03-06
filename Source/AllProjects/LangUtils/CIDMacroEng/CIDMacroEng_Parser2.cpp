//
// FILE NAME: CIDMacroEng_Parser2.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/27/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file handles some of the grunt work methods of the parser class, to
//  keep the main file mostly oriented towards the high level parsing logic.
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
tCIDLib::TBoolean
TMacroEngParser::bCheckBegin(       TParserSrc&             psrcClass
                            , const tCIDMacroEng::ERecovers    eRecover)
{
    if (!psrcClass.bIfPeeked(tCIDMacroEng::ETokens::Begin))
    {
        IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedBegin);
        if (eRecover == tCIDMacroEng::ERecovers::No)
            ThrowUnrecoverable();
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMacroEngParser::bCheckCloseBracket(        TParserSrc&             psrcClass
                                    , const tCIDMacroEng::ERecovers    eRecover)
{
    if (!psrcClass.bIfPeeked(tCIDMacroEng::ETokens::CloseBracket))
    {
        IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedCloseBracket);
        if (eRecover == tCIDMacroEng::ERecovers::No)
            ThrowUnrecoverable();
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMacroEngParser::bCheckCloseParen(          TParserSrc&             psrcClass
                                    , const tCIDMacroEng::ERecovers    eRecover)
{
    if (!psrcClass.bIfPeeked(tCIDMacroEng::ETokens::CloseParen))
    {
        IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedCloseParen);
        if (eRecover == tCIDMacroEng::ERecovers::No)
            ThrowUnrecoverable();
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMacroEngParser::bCheckColon(       TParserSrc&             psrcClass
                            , const tCIDMacroEng::ERecovers    eRecover)
{
    if (!psrcClass.bIfPeeked(tCIDMacroEng::ETokens::Colon))
    {
        IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedColon);
        if (eRecover == tCIDMacroEng::ERecovers::No)
            ThrowUnrecoverable();
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMacroEngParser::bCheckComma(       TParserSrc&             psrcClass
                            , const tCIDMacroEng::ERecovers    eRecover)
{
    if (!psrcClass.bIfPeeked(tCIDMacroEng::ETokens::Comma))
    {
        IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedComma);
        if (eRecover == tCIDMacroEng::ERecovers::No)
            ThrowUnrecoverable();
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMacroEngParser::bCheckEquals(          TParserSrc&             psrcClass
                                , const tCIDMacroEng::ERecovers    eRecover)
{
    if (!psrcClass.bIfPeeked(tCIDMacroEng::ETokens::EqualSign))
    {
        IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedEquals);
        if (eRecover == tCIDMacroEng::ERecovers::No)
            ThrowUnrecoverable();
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMacroEngParser::bCheckFlowStack(       TParserSrc&         psrcClass
                                , const tCIDMacroEng::ETokens  eTokenSeen
                                ,       TMEngFlowCtrlItem&  mefciToFill
                                , const tCIDLib::TBoolean   bPop)
{
    // If the flow stack item is empty, then obviously bad
    if (m_colFlowStack.bIsEmpty())
    {
        IssueErr
        (
            psrcClass
            , kMEngErrs::errcPrs_UnexpectedEndFlow
            , psrcClass.strTokenText(eTokenSeen)
        );
        return kCIDLib::False;
    }

    //
    //  Get the top of stack item into the caller's buffer. Either pop
    //  or peek, according to the parm.
    //
    if (bPop)
        mefciToFill = m_colFlowStack.objPop();
    else
        mefciToFill = m_colFlowStack.objPeek();

    //
    //  According to the token we saw, check whether it makes sense against
    //  the top of stack flow item.
    //
    tCIDLib::TBoolean bRet = kCIDLib::False;

    if ((eTokenSeen == tCIDMacroEng::ETokens::Case)
    ||  (eTokenSeen == tCIDMacroEng::ETokens::FTCase)
    ||  (eTokenSeen == tCIDMacroEng::ETokens::Default))
    {
        if ((mefciToFill.eType() == tCIDMacroEng::EFlowTypes::Switch)
        ||  (mefciToFill.eType() == tCIDMacroEng::EFlowTypes::EndCase))
        {
            bRet = kCIDLib::True;
        }
    }
     else if (eTokenSeen == tCIDMacroEng::ETokens::EndCase)
    {
        if ((mefciToFill.eType() == tCIDMacroEng::EFlowTypes::Case)
        ||  (mefciToFill.eType() == tCIDMacroEng::EFlowTypes::FTCase))
        {
            bRet = kCIDLib::True;
        }
    }
     else if (eTokenSeen == tCIDMacroEng::ETokens::DoWhile)
    {
        if (mefciToFill.eType() == tCIDMacroEng::EFlowTypes::DoLoop)
            bRet = kCIDLib::True;
    }
     else if (eTokenSeen == tCIDMacroEng::ETokens::Else)
    {
        if ((mefciToFill.eType() == tCIDMacroEng::EFlowTypes::If)
        ||  (mefciToFill.eType() == tCIDMacroEng::EFlowTypes::ElseIf))
        {
            bRet = kCIDLib::True;
        }
    }
     else if (eTokenSeen == tCIDMacroEng::ETokens::ElseIf)
    {
        if ((mefciToFill.eType() == tCIDMacroEng::EFlowTypes::If)
        ||  (mefciToFill.eType() == tCIDMacroEng::EFlowTypes::ElseIf))
        {
            bRet = kCIDLib::True;
        }
    }
     else if (eTokenSeen == tCIDMacroEng::ETokens::Catch)
    {
        if (mefciToFill.eType() == tCIDMacroEng::EFlowTypes::EndTry)
            bRet = kCIDLib::True;
    }
     else if (eTokenSeen == tCIDMacroEng::ETokens::EndCatch)
    {
        if (mefciToFill.eType() == tCIDMacroEng::EFlowTypes::Catch)
            bRet = kCIDLib::True;
    }
     else if (eTokenSeen == tCIDMacroEng::ETokens::EndTry)
    {
        if (mefciToFill.eType() == tCIDMacroEng::EFlowTypes::Try)
            bRet = kCIDLib::True;
    }
     else if (eTokenSeen == tCIDMacroEng::ETokens::EndIf)
    {
        if ((mefciToFill.eType() == tCIDMacroEng::EFlowTypes::If)
        ||  (mefciToFill.eType() == tCIDMacroEng::EFlowTypes::Else)
        ||  (mefciToFill.eType() == tCIDMacroEng::EFlowTypes::ElseIf))
        {
            bRet = kCIDLib::True;
        }
    }
     else if (eTokenSeen == tCIDMacroEng::ETokens::EndSwitch)
    {
        //
        //  Switch statements cannot be empty, so the last thing we
        //  saw has to be an end case of some sort, at least the default
        //  case.
        //
        if (mefciToFill.eType() == tCIDMacroEng::EFlowTypes::EndCase)
            bRet = kCIDLib::True;
    }
     else if (eTokenSeen == tCIDMacroEng::ETokens::EndWhile)
    {
        if (mefciToFill.eType() == tCIDMacroEng::EFlowTypes::While)
            bRet = kCIDLib::True;
    }
     else if (eTokenSeen == tCIDMacroEng::ETokens::EndForEach)
    {
        if (mefciToFill.eType() == tCIDMacroEng::EFlowTypes::ForEach)
            bRet = kCIDLib::True;
    }

    if (!bRet)
    {
        IssueErr
        (
            psrcClass
            , kMEngErrs::errcPrs_ExpectedEndFlow
            , tCIDMacroEng::strXlatEFlowTypes(mefciToFill.eType())
            , TCardinal(mefciToFill.c4LineNum())
            , psrcClass.strTokenText(eTokenSeen)
        );
    }
    return bRet;
}


tCIDLib::TBoolean
TMacroEngParser::bCheckNextToken(       TParserSrc&             psrcClass
                                , const tCIDMacroEng::ETokens      eToCheck
                                , const tCIDMacroEng::ERecovers    eRecover)
{
    if (!psrcClass.bIfPeeked(eToCheck))
    {
        IssueErr
        (
            psrcClass
            , kMEngErrs::errcPrs_ExpectedToken
            , psrcClass.strTokenText(eToCheck)
        );
        if (eRecover == tCIDMacroEng::ERecovers::No)
            ThrowUnrecoverable();
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMacroEngParser::bCheckPeriod(          TParserSrc&             psrcClass
                                , const tCIDMacroEng::ERecovers    eRecover)
{
    if (!psrcClass.bIfPeeked(tCIDMacroEng::ETokens::Period))
    {
        IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedPeriod);
        if (eRecover == tCIDMacroEng::ERecovers::No)
            ThrowUnrecoverable();
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMacroEngParser::bCheckSemiColon(       TParserSrc&             psrcClass
                                , const tCIDMacroEng::ERecovers    eRecover)
{
    if (!psrcClass.bIfPeeked(tCIDMacroEng::ETokens::SemiColon))
    {
        IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedSemiColon);
        if (eRecover == tCIDMacroEng::ERecovers::No)
            ThrowUnrecoverable();
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMacroEngParser::bCheckOpenParen(       TParserSrc&             psrcClass
                                , const tCIDMacroEng::ERecovers    eRecover)
{
    if (!psrcClass.bIfPeeked(tCIDMacroEng::ETokens::OpenParen))
    {
        IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedOpenParen);
        if (eRecover == tCIDMacroEng::ERecovers::No)
            ThrowUnrecoverable();
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMacroEngParser::bCheckOpenBracket(         TParserSrc&             psrcClass
                                    , const tCIDMacroEng::ERecovers    eRecover)
{
    if (!psrcClass.bIfPeeked(tCIDMacroEng::ETokens::OpenBracket))
    {
        IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedOpenBracket);
        if (eRecover == tCIDMacroEng::ERecovers::No)
            ThrowUnrecoverable();
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMacroEngParser::bGetBoolToken(         TParserSrc&             psrcClass
                                ,       tCIDLib::TBoolean&      bResult
                                , const tCIDMacroEng::ERecovers    eRecover)
{
    const tCIDMacroEng::ETokens eTok = psrcClass.eGetNextToken(kCIDLib::False);

    if (eTok == tCIDMacroEng::ETokens::False)
        bResult = kCIDLib::False;
    else if (eTok == tCIDMacroEng::ETokens::True)
        bResult = kCIDLib::True;
    else
    {
        IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedBoolConst);
        if (eRecover == tCIDMacroEng::ERecovers::No)
            ThrowUnrecoverable();

        bResult = kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  This one is kind of special, since a class path ends up looking like
//  multiple tokens because isn't quoted but contains periods. The period makes
//  the tokenizer stop. So we do this one specially, scanning while we see
//  chars and periods. We stop on the first whitespace or other special char.
//
//  To help optimize splitting out the actual class, which is the last part
//  of the path, we return the index of the last part.
//
//  Since this is also sometimes used to parse out expected enum or literal
//  values, which are referenced by class name (optionally so for literals),
//  the bAllowName flag will allow name chars in the path, since names can
//  have a couple of chars that a straight class path cannot.
//
//  <TBD> Why isn't bAllowName used?
//
tCIDLib::TBoolean
TMacroEngParser::bGetClassPath(         TParserSrc&             psrcClass
                                ,       TString&                strToFill
                                ,       tCIDLib::TCard4&        c4LastSep
                                , const tCIDLib::TBoolean       bAllowName
                                , const tCIDMacroEng::ERecovers    eRecover)
{
    strToFill.Clear();

    // Skip any whitespace, then remember this as the new save position
    psrcClass.EatWhitespace();
    psrcClass.SavePos();

    //
    //  Test first to see if we have a name character. It must not be a
    //  special char, and that includes a period here in the first pos.
    //
    tCIDLib::TCh chCur = psrcClass.chGetNextChar();
    if (!TRawStr::bIsAlpha(chCur) || psrcClass.bIsSpecialChar(chCur))
    {
        psrcClass.PushBack(chCur);
        if (eRecover != tCIDMacroEng::ERecovers::CheckOnly)
        {
            IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedClassPath);
            if (eRecover == tCIDMacroEng::ERecovers::No)
                ThrowUnrecoverable();
        }
        return kCIDLib::False;
    }

    tCIDLib::TBoolean bRet = kCIDLib::False;
    c4LastSep = 0;
    while (kCIDLib::True)
    {
        // Remember the current count as the last separator offset
        if (chCur == kCIDLib::chPeriod)
            c4LastSep = strToFill.c4Length();

        // Append the current char to our output string
        strToFill.Append(chCur);

        //
        //  And now we can get another one. Check it and if it's a whitespace
        //  or any of the special chars other than period, dash, or underscore
        //  then unget it and we are done.
        //
        chCur = psrcClass.chGetNextChar();

        if (chCur == kCIDLib::chPeriod)
        {
            // Don't allow two periods in a row
            const tCIDLib::TCard4 c4Len = strToFill.c4Length();
            if ((c4Len > 1) && (strToFill.chLast() == kCIDLib::chPeriod))
            {
                if (eRecover == tCIDMacroEng::ERecovers::CheckOnly)
                {
                    psrcClass.PushBack(strToFill);
                }
                 else
                {
                    IssueErr(psrcClass, kMEngErrs::errcPrs_EmptyPathPart);
                    if (eRecover == tCIDMacroEng::ERecovers::No)
                        ThrowUnrecoverable();
                }
                break;
            }
        }
         else if ((chCur == kCIDLib::chHyphenMinus)
              ||  (chCur == kCIDLib::chUnderscore))
        {
            //
            //  We allow these specifically here, but they cannot be the
            //  first char in the path.
            //
            if (strToFill.bIsEmpty())
            {
                if (eRecover == tCIDMacroEng::ERecovers::CheckOnly)
                    psrcClass.PushBack(strToFill);
                else
                    IssueErr(psrcClass, kMEngErrs::errcPrs_BadClassPathChar);
                break;
            }
        }
         else if (TRawStr::bIsSpace(chCur) || psrcClass.bIsSpecialChar(chCur))
        {
            //
            //  Never allow spaces, or any special chars other than those we
            //  just specially checked for above.
            //
            psrcClass.PushBack(chCur);
            bRet = kCIDLib::True;
            break;
        }
         else if (!TRawStr::bIsAlphaNum(chCur))
        {
            if (eRecover == tCIDMacroEng::ERecovers::CheckOnly)
                psrcClass.PushBack(strToFill);
            else
                IssueErr(psrcClass, kMEngErrs::errcPrs_BadClassPathChar);
            break;
        }
    }

    // The last char cannot be a period
    if (strToFill.chLast() == kCIDLib::chPeriod)
    {
        if (eRecover == tCIDMacroEng::ERecovers::CheckOnly)
        {
            psrcClass.PushBack(strToFill);
        }
         else
        {
            IssueErr(psrcClass, kMEngErrs::errcPrs_TrailPathPart);
            if (eRecover == tCIDMacroEng::ERecovers::No)
                ThrowUnrecoverable();
        }
        bRet = kCIDLib::False;
    }
    return bRet;
}


tCIDLib::TBoolean
TMacroEngParser::bGetNameToken(         TParserSrc&             psrcClass
                                ,       TString&                strToFill
                                , const tCIDLib::TBoolean       bKeywordOk
                                , const tCIDMacroEng::ERecovers    eRecover)
{
    // Get a token out and let's check it
    const tCIDMacroEng::ETokens eTok = psrcClass.eGetNextToken(strToFill);

    // If it's not 'no match', then it hit some keyword or special char
    if (eTok != tCIDMacroEng::ETokens::NoMatch)
    {
        //
        //  If it's one of the special ones, then it's bad right off, so
        //  return failure.
        //
        if (eTok >= tCIDMacroEng::ETokens::FirstSymbolic)
        {
            IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedName);
            if (eRecover == tCIDMacroEng::ERecovers::No)
                ThrowUnrecoverable();
            return kCIDLib::False;
        }

        //
        //  It's matched a key word. If they can it can't be allowed, then
        //  return failure. But, we special case a few of them because they
        //  can never be used. These are special because they are values used
        //  in code, and so we could never distinguish them. The others are
        //  only part of the housekeeping of the class so it wouldn't
        //  necessarily be horrible to allow them as long as the caller say's
        //  its ok.
        //
        if (!bKeywordOk)
        {
            IssueErr(psrcClass, kMEngErrs::errcPrs_ReservedWord, strToFill);
            if (eRecover == tCIDMacroEng::ERecovers::No)
                ThrowUnrecoverable();
            return kCIDLib::False;
        }

        if ((strToFill == TFacCIDMacroEng::strTrueKeyword)
        ||  (strToFill == TFacCIDMacroEng::strFalseKeyword))
        {
            IssueErr(psrcClass, kMEngErrs::errcPrs_ReservedWord, strToFill);
            ThrowUnrecoverable();
        }
    }

    //
    //  Make sure that the token starts with an alpha, and the rest is
    //  composed only of alphanumerics and underscores.
    //
    if (!TRawStr::bIsAlpha(strToFill.chFirst()))
    {
        IssueErr(psrcClass, kMEngErrs::errcPrs_FirstNameChar);
        if (eRecover == tCIDMacroEng::ERecovers::No)
            ThrowUnrecoverable();
    }

    const tCIDLib::TCard4 c4Len = strToFill.c4Length();
    for (tCIDLib::TCard4 c4Index = 1; c4Index < c4Len; c4Index++)
    {
        if (!TRawStr::bIsAlphaNum(strToFill[c4Index])
        &&  (strToFill[c4Index] != kCIDLib::chUnderscore))
        {
            IssueErr(psrcClass, kMEngErrs::errcPrs_BadNameChar);
            if (eRecover == tCIDMacroEng::ERecovers::No)
                ThrowUnrecoverable();
            break;
        }
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMacroEngParser::bGetQuotedString(          TParserSrc&             psrcClass
                                    ,       TString&                strToFill
                                    , const tCIDMacroEng::ERecovers    eRecover)
{
    // Get a token out and let's check it
    const tCIDMacroEng::ETokens eTok = psrcClass.eGetNextToken(strToFill);

    if (eTok != tCIDMacroEng::ETokens::QuotedString)
    {
        IssueErr(psrcClass, kMEngErrs::errcPrs_ExpectedQuoted);
        if (eRecover == tCIDMacroEng::ERecovers::No)
            ThrowUnrecoverable();
        psrcClass.PushBack(eTok, strToFill);
        return kCIDLib::False;
    }

    // Escape the string
    psrcClass.EscapeStr(strToFill);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMacroEngParser::bIsOnNestingStack(const TString& strClassPath) const
{
    TClassStack::TCursor cursClasses(&m_colClassStack);
    for (; cursClasses; ++cursClasses)
    {
        if (*cursClasses == strClassPath)
            return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean
TMacroEngParser::bLastFlowItem( const   tCIDMacroEng::EFlowTypes   eType
                                ,       TMEngFlowCtrlItem*&     pmefciToFill)
{
    //
    //  We need to start at the top and work our way back down the stack
    //  until we find a looped flow item.
    //
    TFlowStack::TNCCursor cursItems(&m_colFlowStack);
    for (; cursItems; ++cursItems)
    {
        if (cursItems->eType() == eType)
        {
            pmefciToFill = &cursItems.objWCur();
            return kCIDLib::True;
        }
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean
TMacroEngParser::bLastLoopedFlowItem(TMEngFlowCtrlItem*& pmefciToFill)
{
    //
    //  We need to start at the top and work our way back down the stack
    //  until we find a looped flow item.
    //
    TFlowStack::TNCCursor cursItems(&m_colFlowStack);
    for (; cursItems; ++cursItems)
    {
        if (cursItems->bIsLoopedType())
        {
            pmefciToFill = &cursItems.objWCur();
            return kCIDLib::True;
        }
    }
    return kCIDLib::False;
}


//
//  This is called when a numeric literal is parsed out. It needs to get pushed
//  as an immediate value (i.e. put into the opcode literally.) So we need to
//  figure out the type of the numeric and generate the appropriate push opcode.
//
//  Note that, sans any suffix, we use the largest type available of each type
//  (i.e. Card4, Int4, or Float8) since literals don't have specific types, and
//  if it's used in an expression we will usually convert to the largest type
//  of the category anyway.
//
tCIDLib::TBoolean
TMacroEngParser::bPushNumericLiteral(TParserSrc&            psrcClass
                                    , TString&              strLit
                                    , TMEngOpMethodImpl&    methToFill
                                    , tCIDLib::TCard2&      c2ClassId)
{
    //
    //  Scan the literal using our little state machine to make sure it's
    //  a legal literal, and decide on the radix and data type. This guy can
    //  change the string, by removing suffixes.
    //
    tCIDLib::TBoolean       bExplicit;
    tCIDMacroEng::ENumTypes    eType;
    tCIDLib::ERadices       eRadix;
    eType = eCheckNumericLiteral(strLit, eRadix, bExplicit);
    if (eType == tCIDMacroEng::ENumTypes::None)
        return kCIDLib::False;

    //
    //  Ok, we know what type it is, and that it should be legal, so let's
    //  push the correct type.
    //
    tCIDLib::TBoolean   bOk;
    if ((eType == tCIDMacroEng::ENumTypes::Card1)
    ||  (eType == tCIDMacroEng::ENumTypes::Card2)
    ||  (eType == tCIDMacroEng::ENumTypes::Card4)
    ||  (eType == tCIDMacroEng::ENumTypes::Card8))
    {
        tCIDLib::TCard8 c8Val = TRawStr::c8AsBinary(strLit.pszBuffer(), bOk, eRadix);
        if (!bOk)
            c8Val = 0;

        if (eType == tCIDMacroEng::ENumTypes::Card1)
        {
            c2ClassId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card1);
            if (c8Val > kCIDLib::c1MaxCard)
                IssueErr(psrcClass, kMEngErrs::errcPrs_NumRangeErr);
            m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::PushImCard1, tCIDLib::TCard1(c8Val));
        }
         else if (eType == tCIDMacroEng::ENumTypes::Card2)
        {
            c2ClassId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card2);
            if (c8Val > kCIDLib::c2MaxCard)
                IssueErr(psrcClass, kMEngErrs::errcPrs_NumRangeErr);
            m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::PushImCard2, tCIDLib::TCard2(c8Val));
        }
         else
        {
            if ((c8Val > kCIDLib::c4MaxCard)
            ||  (bExplicit && (eType == tCIDMacroEng::ENumTypes::Card8)))
            {
                // If it was explicitly a Card4, this is bad, but keep going
                if ((eType == tCIDMacroEng::ENumTypes::Card4) && bExplicit)
                    IssueErr(psrcClass, kMEngErrs::errcPrs_NumRangeErr);

                c2ClassId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card8);
                m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::PushImCard8, c8Val);
            }
             else
            {
                c2ClassId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card4);
                m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::PushImCard4, tCIDLib::TCard4(c8Val));
            }
        }
    }
     else if ((eType == tCIDMacroEng::ENumTypes::Int1)
          ||  (eType == tCIDMacroEng::ENumTypes::Int2)
          ||  (eType == tCIDMacroEng::ENumTypes::Int4))
    {
        tCIDLib::TInt4 i4Val = TRawStr::i4AsBinary(strLit.pszBuffer(), bOk, eRadix);
        if (!bOk)
            i4Val = 0;

        if (eType == tCIDMacroEng::ENumTypes::Int1)
        {
            c2ClassId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int1);
            if ((i4Val < kCIDLib::i1MinInt) || (i4Val > kCIDLib::i1MaxInt))
                IssueErr(psrcClass, kMEngErrs::errcPrs_NumRangeErr);
            m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::PushImInt1, tCIDLib::TInt1(i4Val));
        }
         else if (eType == tCIDMacroEng::ENumTypes::Int2)
        {
            c2ClassId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int2);
            if ((i4Val < kCIDLib::i2MinInt) || (i4Val > kCIDLib::i2MaxInt))
                IssueErr(psrcClass, kMEngErrs::errcPrs_NumRangeErr);
            m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::PushImInt2, tCIDLib::TInt2(i4Val));
        }
         else if (eType == tCIDMacroEng::ENumTypes::Int4)
        {
            c2ClassId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int4);
            m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::PushImInt4, i4Val);
        }
    }
     else
    {
        tCIDLib::TFloat8 f8Val = TRawStr::f8AsBinary(strLit.pszBuffer(), bOk);
        if (!bOk)
            f8Val = 0;

        if (eType == tCIDMacroEng::ENumTypes::Float4)
        {
            c2ClassId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Float4);
            if ((f8Val < -kCIDLib::f4MaxFloat) || (f8Val > kCIDLib::f4MaxFloat))
                IssueErr(psrcClass, kMEngErrs::errcPrs_NumRangeErr);
            m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::PushImFloat4, tCIDLib::TFloat4(f8Val));
        }
         else if (eType == tCIDMacroEng::ENumTypes::Float8)
        {
            c2ClassId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Float8);
            m_meopToUse.SetImmediate(tCIDMacroEng::EOpCodes::PushImFloat8, f8Val);
        }
    }

    // If it worked, then add the new opcode
    if (bOk)
        methToFill.c4AddOpCode(m_meopToUse);

    return bOk;
}


tCIDMacroEng::EOpResults
TMacroEngParser::eGetOpInfo(const   TMEngClassInfo&     meciTarget
                            , const tCIDMacroEng::ETokens  eToken
                            ,       tCIDLib::TCard2&    c2MethId)
{
    const tCIDLib::TCh* pszOpName = 0;

    tCIDMacroEng::EOpResults eRet;
    switch(eToken)
    {
        case tCIDMacroEng::ETokens::Add :
            pszOpName = kMacroEng::pszOp_Add;
            eRet = tCIDMacroEng::EOpResults::Mathematical;
            break;

        case tCIDMacroEng::ETokens::And :
            pszOpName = kMacroEng::pszOp_And;
            eRet = tCIDMacroEng::EOpResults::Bitwise;
            break;

        case tCIDMacroEng::ETokens::Assign :
            eRet = tCIDMacroEng::EOpResults::Assign;
            break;

        case tCIDMacroEng::ETokens::AndEq :
            pszOpName = kMacroEng::pszOp_AndEq;
            eRet = tCIDMacroEng::EOpResults::Assign;
            break;

        case tCIDMacroEng::ETokens::Dec :
            pszOpName = kMacroEng::pszOp_Dec;
            eRet = tCIDMacroEng::EOpResults::Unary;
            break;

        case tCIDMacroEng::ETokens::Divide :
            pszOpName = kMacroEng::pszOp_Divide;
            eRet = tCIDMacroEng::EOpResults::Mathematical;
            break;

        case tCIDMacroEng::ETokens::DivideEq :
            pszOpName = kMacroEng::pszOp_DivideEq;
            eRet = tCIDMacroEng::EOpResults::Assign;
            break;

        case tCIDMacroEng::ETokens::EqualSign :
            pszOpName = kMacroEng::pszOp_Equal;
            eRet = tCIDMacroEng::EOpResults::Binary;
            break;

        case tCIDMacroEng::ETokens::GtThan :
            pszOpName = kMacroEng::pszOp_GtThan;
            eRet = tCIDMacroEng::EOpResults::Binary;
            break;

        case tCIDMacroEng::ETokens::GtThanEq :
            pszOpName = kMacroEng::pszOp_GtThanEq;
            eRet = tCIDMacroEng::EOpResults::Binary;
            break;

        case tCIDMacroEng::ETokens::Inc :
            pszOpName = kMacroEng::pszOp_Inc;
            eRet = tCIDMacroEng::EOpResults::Unary;
            break;

        case tCIDMacroEng::ETokens::LogAnd :
            eRet = tCIDMacroEng::EOpResults::Logical;
            break;

        case tCIDMacroEng::ETokens::LogOr :
            eRet = tCIDMacroEng::EOpResults::Logical;
            break;

        case tCIDMacroEng::ETokens::LogXor :
            eRet = tCIDMacroEng::EOpResults::Logical;
            break;

        case tCIDMacroEng::ETokens::LsThan :
            pszOpName = kMacroEng::pszOp_LsThan;
            eRet = tCIDMacroEng::EOpResults::Binary;
            break;

        case tCIDMacroEng::ETokens::LsThanEq :
            pszOpName = kMacroEng::pszOp_LsThanEq;
            eRet = tCIDMacroEng::EOpResults::Binary;
            break;

        case tCIDMacroEng::ETokens::ModDiv :
            pszOpName = kMacroEng::pszOp_ModDiv;
            eRet = tCIDMacroEng::EOpResults::Mathematical;
            break;

        case tCIDMacroEng::ETokens::ModDivEq :
            pszOpName = kMacroEng::pszOp_ModDivEq;
            eRet = tCIDMacroEng::EOpResults::Assign;
            break;

        case tCIDMacroEng::ETokens::MinusEq :
            pszOpName = kMacroEng::pszOp_MinusEq;
            eRet = tCIDMacroEng::EOpResults::Assign;
            break;

        case tCIDMacroEng::ETokens::Multiply :
            pszOpName = kMacroEng::pszOp_Multiply;
            eRet = tCIDMacroEng::EOpResults::Mathematical;
            break;

        case tCIDMacroEng::ETokens::MultiplyEq :
            pszOpName = kMacroEng::pszOp_MultiplyEq;
            eRet = tCIDMacroEng::EOpResults::Assign;
            break;

        case tCIDMacroEng::ETokens::NotEqual :
            pszOpName = kMacroEng::pszOp_Equal;
            eRet = tCIDMacroEng::EOpResults::Binary;
            break;

        case tCIDMacroEng::ETokens::Or :
            pszOpName = kMacroEng::pszOp_Or;
            eRet = tCIDMacroEng::EOpResults::Bitwise;
            break;

        case tCIDMacroEng::ETokens::OrEq :
            pszOpName = kMacroEng::pszOp_OrEq;
            eRet = tCIDMacroEng::EOpResults::Assign;
            break;

        case tCIDMacroEng::ETokens::PlusEq :
            pszOpName = kMacroEng::pszOp_PlusEq;
            eRet = tCIDMacroEng::EOpResults::Assign;
            break;

        case tCIDMacroEng::ETokens::Subtract :
            pszOpName = kMacroEng::pszOp_Subtract;
            eRet = tCIDMacroEng::EOpResults::Mathematical;
            break;

        case tCIDMacroEng::ETokens::Xor :
            pszOpName = kMacroEng::pszOp_Xor;
            eRet = tCIDMacroEng::EOpResults::Bitwise;
            break;

        case tCIDMacroEng::ETokens::XorEq :
            pszOpName = kMacroEng::pszOp_XorEq;
            eRet = tCIDMacroEng::EOpResults::Assign;
            break;

        default :
            return tCIDMacroEng::EOpResults::NotOp;
            break;
    };

    //
    //  If it has a mapped method, make sure the class supports it and get
    //  the method id.
    //
    if (pszOpName)
    {
        c2MethId = meciTarget.c2FindMethod(pszOpName);
        if (c2MethId == kMacroEng::c2BadId)
            eRet = tCIDMacroEng::EOpResults::NotSupported;
    }
     else
    {
        c2MethId = kMacroEng::c2BadId;
    }
    return eRet;
}


//
//  These are called internally. They bump the error counter, and then call
//  the public virtuals that the derived class implements.
//
tCIDLib::TVoid
TMacroEngParser::IssueErr(const TParserSrc& psrcClass, const TString& strText)
{
    m_c4ErrCount++;
    m_pmeehToUse->ParseEvent
    (
        tCIDMacroEng::EPrsEvs::ErrorMsg
        , strText
        , psrcClass.c4Line()
        , psrcClass.c4Col()
        , psrcClass.strPath()
    );
}

tCIDLib::TVoid
TMacroEngParser::IssueErr(  const   TParserSrc&         psrcClass
                            , const tCIDLib::TErrCode   errcToIssue)
{
    // Get a string from the pool to use
    TMEngStrPoolJan janTmp(m_pmsplParse);
    TString& strTmp = janTmp.strThis();

    m_c4ErrCount++;
    strTmp.LoadFromMsg(errcToIssue, facCIDMacroEng());
    m_pmeehToUse->ParseEvent
    (
        tCIDMacroEng::EPrsEvs::ErrorMsg
        , strTmp
        , psrcClass.c4Line()
        , psrcClass.c4Col()
        , psrcClass.strPath()
    );
}

tCIDLib::TVoid
TMacroEngParser::IssueErr(  const   TParserSrc&         psrcClass
                            , const tCIDLib::TErrCode   errcToIssue
                            , const MFormattable&       fmtblToken1)
{
    // Get a string from the pool to use
    TMEngStrPoolJan janTmp(m_pmsplParse);
    TString& strTmp = janTmp.strThis();

    m_c4ErrCount++;
    strTmp.LoadFromMsg(errcToIssue, facCIDMacroEng(), fmtblToken1);
    m_pmeehToUse->ParseEvent
    (
        tCIDMacroEng::EPrsEvs::ErrorMsg
        , strTmp
        , psrcClass.c4Line()
        , psrcClass.c4Col()
        , psrcClass.strPath()
    );
}

tCIDLib::TVoid
TMacroEngParser::IssueErr(  const   TParserSrc&         psrcClass
                            , const tCIDLib::TErrCode   errcToIssue
                            , const MFormattable&       fmtblToken1
                            , const MFormattable&       fmtblToken2)
{
    // Get a string from the pool to use
    TMEngStrPoolJan janTmp(m_pmsplParse);
    TString& strTmp = janTmp.strThis();

    m_c4ErrCount++;
    strTmp.LoadFromMsg
    (
        errcToIssue
        , facCIDMacroEng()
        , fmtblToken1
        , fmtblToken2
    );
    m_pmeehToUse->ParseEvent
    (
        tCIDMacroEng::EPrsEvs::ErrorMsg
        , strTmp
        , psrcClass.c4Line()
        , psrcClass.c4Col()
        , psrcClass.strPath()
    );
}

tCIDLib::TVoid
TMacroEngParser::IssueErr(  const   TParserSrc&         psrcClass
                            , const tCIDLib::TErrCode   errcToIssue
                            , const MFormattable&       fmtblToken1
                            , const MFormattable&       fmtblToken2
                            , const MFormattable&       fmtblToken3)
{
    // Get a string from the pool to use
    TMEngStrPoolJan janTmp(m_pmsplParse);
    TString& strTmp = janTmp.strThis();

    m_c4ErrCount++;
    strTmp.LoadFromMsg
    (
        errcToIssue
        , facCIDMacroEng()
        , fmtblToken1
        , fmtblToken2
        , fmtblToken3
    );
    m_pmeehToUse->ParseEvent
    (
        tCIDMacroEng::EPrsEvs::ErrorMsg
        , strTmp
        , psrcClass.c4Line()
        , psrcClass.c4Col()
        , psrcClass.strPath()
    );
}

tCIDLib::TVoid
TMacroEngParser::IssueErr(  const   TParserSrc&         psrcClass
                            , const tCIDLib::TErrCode   errcToIssue
                            , const MFormattable&       fmtblToken1
                            , const MFormattable&       fmtblToken2
                            , const MFormattable&       fmtblToken3
                            , const MFormattable&       fmtblToken4)
{
    // Get a string from the pool to use
    TMEngStrPoolJan janTmp(m_pmsplParse);
    TString& strTmp = janTmp.strThis();

    m_c4ErrCount++;
    strTmp.LoadFromMsg
    (
        errcToIssue
        , facCIDMacroEng()
        , fmtblToken1
        , fmtblToken2
        , fmtblToken3
        , fmtblToken4
    );
    m_pmeehToUse->ParseEvent
    (
        tCIDMacroEng::EPrsEvs::ErrorMsg
        , strTmp
        , psrcClass.c4Line()
        , psrcClass.c4Col()
        , psrcClass.strPath()
    );
}

tCIDLib::TVoid
TMacroEngParser::IssueExcept(const  TParserSrc& psrcClass
                            , const TError&     errCaught)
{
    m_c4ErrCount++;
    m_pmeehToUse->ParseException
    (
        errCaught
        , psrcClass.c4Line()
        , psrcClass.c4Col()
        , psrcClass.strPath()
    );
}


tCIDLib::TVoid TMacroEngParser::IssueExcept(const TParserSrc& psrcClass)
{
    m_c4ErrCount++;
    m_pmeehToUse->ParseException
    (
        psrcClass.c4Line()
        , psrcClass.c4Col()
        , psrcClass.strPath()
    );
}


//
//  This is called do parse out a literal value and create a class value
//  object to hold the value. We give them back the class id. They provide
//  an optional name (since it's also called in the Literals= section to parse
//  named literals, as well as to immediate values, which are not named),
//  and optionally an expected type.
//
//  If they provided an expected type, then we override the determined type,
//  and see if we can convert to the indicated type.
//
TMEngClassVal* TMacroEngParser::
pmecvMakeNumericLiteral(        TParserSrc&             psrcClass
                        ,       TString&                strLit
                        ,       tCIDLib::TCard2&        c2ClassId
                        , const TString&                strName
                        , const tCIDMacroEng::ENumTypes    eExpected)
{
    tCIDLib::TBoolean bExplicit;
    tCIDLib::ERadices eRadix;
    tCIDMacroEng::ENumTypes  eType=  eCheckNumericLiteral(strLit, eRadix, bExplicit);
    if (eType == tCIDMacroEng::ENumTypes::None)
    {
        // Leave class id unchanged and return null pointer
        return 0;
    }

    //
    //  Override the determined type if needed. We still have to do the check
    //  above, since it removes suffixes, if present. If it cannot be converted
    //  to the expected type, then we fail.

    if (eExpected != tCIDMacroEng::ENumTypes::None)
        eType = eExpected;

    //
    //  Ok, we know what type it is, and that it should be legal, so let's
    //  push the correct type.
    //
    TMEngClassVal*      pmecvRet = 0;
    tCIDLib::TBoolean   bOk;
    if ((eType == tCIDMacroEng::ENumTypes::Card1)
    ||  (eType == tCIDMacroEng::ENumTypes::Card2)
    ||  (eType == tCIDMacroEng::ENumTypes::Card4)
    ||  (eType == tCIDMacroEng::ENumTypes::Card8))
    {
        tCIDLib::TCard8 c8Val = TRawStr::c8AsBinary(strLit.pszBuffer(), bOk, eRadix);
        if (!bOk)
            c8Val = 0;

        if (eType == tCIDMacroEng::ENumTypes::Card1)
        {
            c2ClassId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card1);
            if (c8Val > kCIDLib::c1MaxCard)
                IssueErr(psrcClass, kMEngErrs::errcPrs_NumRangeErr);
            pmecvRet = new TMEngCard1Val
            (
                strName, tCIDMacroEng::EConstTypes::Const, tCIDLib::TCard1(c8Val)
            );
        }
         else if (eType == tCIDMacroEng::ENumTypes::Card2)
        {
            c2ClassId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card2);
            if (c8Val > kCIDLib::c2MaxCard)
                IssueErr(psrcClass, kMEngErrs::errcPrs_NumRangeErr);
            pmecvRet = new TMEngCard2Val
            (
                strName, tCIDMacroEng::EConstTypes::Const, tCIDLib::TCard2(c8Val)
            );
        }
         else
        {
            if (c8Val > kCIDLib::c4MaxCard)
            {
                // If it was explicitly a Card4, then bad, but keep going
                if ((eType == tCIDMacroEng::ENumTypes::Card2) && bExplicit)
                    IssueErr(psrcClass, kMEngErrs::errcPrs_NumRangeErr);

                c2ClassId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card8);
                pmecvRet = new TMEngCard8Val
                (
                    strName, tCIDMacroEng::EConstTypes::Const, c8Val
                );
            }
             else
            {
                c2ClassId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card4);
                pmecvRet = new TMEngCard4Val
                (
                    strName, tCIDMacroEng::EConstTypes::Const, tCIDLib::TCard4(c8Val)
                );
            }
        }
    }
     else if ((eType == tCIDMacroEng::ENumTypes::Int1)
          ||  (eType == tCIDMacroEng::ENumTypes::Int2)
          ||  (eType == tCIDMacroEng::ENumTypes::Int4))
    {
        tCIDLib::TInt4 i4Val = TRawStr::i4AsBinary(strLit.pszBuffer(), bOk, eRadix);
        if (!bOk)
            i4Val = 0;

        if (eType == tCIDMacroEng::ENumTypes::Int1)
        {
            c2ClassId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int1);
            if ((i4Val < kCIDLib::i1MinInt) || (i4Val > kCIDLib::i1MaxInt))
                IssueErr(psrcClass, kMEngErrs::errcPrs_NumRangeErr);
            pmecvRet = new TMEngInt1Val
            (
                strName, tCIDMacroEng::EConstTypes::Const, tCIDLib::TInt1(i4Val)
            );
        }
         else if (eType == tCIDMacroEng::ENumTypes::Int2)
        {
            c2ClassId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int2);
            if ((i4Val < kCIDLib::i2MinInt) || (i4Val > kCIDLib::i2MaxInt))
                IssueErr(psrcClass, kMEngErrs::errcPrs_NumRangeErr);
            pmecvRet = new TMEngInt2Val
            (
                TString::strEmpty()
                , tCIDMacroEng::EConstTypes::Const
                , tCIDLib::TInt2(i4Val)
            );
        }
         else if (eType == tCIDMacroEng::ENumTypes::Int4)
        {
            c2ClassId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int4);
            pmecvRet = new TMEngInt4Val(strName, tCIDMacroEng::EConstTypes::Const, i4Val);
        }
    }
     else if ((eType == tCIDMacroEng::ENumTypes::Float4)
          ||  (eType == tCIDMacroEng::ENumTypes::Float8))
    {
        tCIDLib::TFloat8 f8Val = TRawStr::f8AsBinary(strLit.pszBuffer(), bOk);
        if (!bOk)
            f8Val = 0;

        if (eType == tCIDMacroEng::ENumTypes::Float4)
        {
            c2ClassId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Float4);
            if ((f8Val < -kCIDLib::f4MaxFloat) || (f8Val > kCIDLib::f4MaxFloat))
                IssueErr(psrcClass, kMEngErrs::errcPrs_NumRangeErr);
            pmecvRet = new TMEngFloat4Val
            (
                strName, tCIDMacroEng::EConstTypes::Const, tCIDLib::TFloat4(f8Val)
            );
        }
         else if (eType == tCIDMacroEng::ENumTypes::Float8)
        {
            c2ClassId = tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Float8);
            pmecvRet = new TMEngFloat8Val(strName, tCIDMacroEng::EConstTypes::Const, f8Val);
        }
    }
    return pmecvRet;
}


tCIDLib::TVoid
TMacroEngParser::PopFlowStack(const TParserSrc&, TMEngFlowCtrlItem&  mefciToFill)
{
    // If the flow stack item is empty, then obviously bad
    #if CID_DEBUG_ON
    if (m_colFlowStack.bIsEmpty())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcDbg_BadFlowStack
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif

    mefciToFill = m_colFlowStack.objPop();
}

