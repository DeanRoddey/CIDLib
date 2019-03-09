//
// FILE NAME: CIDXML_ParserCore3.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/28/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements some of the grunt work methods of the core parser
//  class.
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
//  TXMLParserCore: Public, non-virtual methods
// ---------------------------------------------------------------------------
MXMLDocEvents* TXMLParserCore::pmxevDocEvents() const
{
    return m_pmxevDocEvents;
}

MXMLDocEvents*
TXMLParserCore::pmxevDocEvents(MXMLDocEvents* const pmxevDocEvents)
{
    m_pmxevDocEvents = pmxevDocEvents;
    return m_pmxevDocEvents;
}

MXMLEntityEvents* TXMLParserCore::pmxevEntityEvents() const
{
    return m_pmxevEntityEvents;
}

MXMLEntityEvents*
TXMLParserCore::pmxevEntityEvents(MXMLEntityEvents* const pmxevEntityEvents)
{
    // Pass on to the entity manager as well as saving it for ourselves
    m_xemThis.pmxevEntityEvents(pmxevEntityEvents);
    m_pmxevEntityEvents = pmxevEntityEvents;

    return m_pmxevEntityEvents;
}


MXMLEntityResolver* TXMLParserCore::pmxevEntityResolver() const
{
    return m_pmxevEntityResolver;
}

MXMLEntityResolver*
TXMLParserCore::pmxevEntityResolver(MXMLEntityResolver* const pmxevEntityResolver)
{
    m_pmxevEntityResolver = pmxevEntityResolver;
    return m_pmxevEntityResolver;
}


MXMLErrorEvents* TXMLParserCore::pmxevErrorEvents() const
{
    return m_pmxevErrEvents;
}

MXMLErrorEvents*
TXMLParserCore::pmxevErrorEvents(MXMLErrorEvents* const pmxevErrEvents)
{
    m_pmxevErrEvents = pmxevErrEvents;
    return m_pmxevErrEvents;
}

TXMLValidator* TXMLParserCore::pxvalValidator() const
{
    return m_pxvalValidator;
}

TXMLValidator*
TXMLParserCore::pxvalValidator(TXMLValidator* const pxvalToAdopt)
{
    // If we have a previous one, then free it
    if (m_pxvalValidator)
        delete m_pxvalValidator;

    // And store the new one
    m_pxvalValidator = pxvalToAdopt;
    return m_pxvalValidator;
}


tCIDLib::TVoid TXMLParserCore::PostXMLError(const tCIDLib::TErrCode errcToPost)
{
    // Get the type of the error. If unknown, then log an error
    const tCIDXML::EErrTypes eType = TFacCIDXML::eTypeOfError(errcToPost);
    if (eType == tCIDXML::EErrTypes::Unknown)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcGen_UnknownErrorType
            , tCIDLib::ESeverities::Warn
            , tCIDLib::EErrClasses::Internal
            , TCardinal(errcToPost)
        );
    }

    // Load up the text of the message from the facility object.
    TString strMsg(128UL);
    if (!facCIDXML().bLoadCIDMsg(errcToPost, strMsg))
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcGen_CantLoadErrText
            , tCIDLib::ESeverities::Warn
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(errcToPost)
        );
    }

    //
    //  And now call the virtual method, passing along the current position
    //  info required to give a reasonable diagnostic.
    //
    if (m_pmxevErrEvents)
    {
        m_pmxevErrEvents->HandleXMLError
        (
            errcToPost
            , eType
            , strMsg
            , m_xemThis.c4CurColumn()
            , m_xemThis.c4CurLine()
            , m_xemThis.strCurSystemId()
        );
    }

    // If its a warning, then don't count it as an error
    if (eType == tCIDXML::EErrTypes::Warning)
        return;

    //
    //  Bump the number of errors. If we hit the maximum error count, then
    //  lets throw out of here. We throw a special exception which the catch
    //  block will not try to re-report through here (avoidng a circular
    //  freakout.)
    //
    //  If we were called from one of the exception catch blocks in the
    //  m_ParseRootEntity() metod, then don't throw.
    //
    m_c4ErrorCount++;
    if ((m_c4ErrorCount >= m_c4ErrorMax) && !m_bInException)
    {

        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcParse_MaxErrsReached
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::OutResource
        );
    }
}


tCIDLib::TVoid
TXMLParserCore::PostXMLError(const  tCIDLib::TErrCode   errcToPost
                            , const MFormattable&       fmtblToken1
                            , const MFormattable&       fmtblToken2
                            , const MFormattable&       fmtblToken3)
{
    // Get the type of the error. If unknown, then log an error
    const tCIDXML::EErrTypes eType = TFacCIDXML::eTypeOfError(errcToPost);
    if (eType == tCIDXML::EErrTypes::Unknown)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcGen_UnknownErrorType
            , tCIDLib::ESeverities::Warn
            , tCIDLib::EErrClasses::Internal
            , TCardinal(errcToPost)
        );
    }

    // Load up the text of the message from the facility object.
    TString strMsg(128UL);
    if (!facCIDXML().bLoadCIDMsg(errcToPost, strMsg, fmtblToken1, fmtblToken2, fmtblToken3))
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcGen_CantLoadErrText
            , tCIDLib::ESeverities::Warn
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(errcToPost)
        );
    }

    //
    //  And now call the virtual method, passing along the current position
    //  info required to give a reasonable diagnostic.
    //
    if (m_pmxevErrEvents)
    {
        m_pmxevErrEvents->HandleXMLError
        (
            errcToPost
            , eType
            , strMsg
            , m_xemThis.c4CurColumn()
            , m_xemThis.c4CurLine()
            , m_xemThis.strCurSystemId()
        );
    }

    // If its a warning, then don't count it as an error
    if (eType == tCIDXML::EErrTypes::Warning)
        return;

    //
    //  Bump the number of errors. If we hit the maximum error count, then
    //  lets throw out of here. We throw a special exception which the catch
    //  block will not try to re-report through here (avoidng a circular
    //  freakout.)
    //
    //  If we were called from one of the exception catch blocks in the
    //  m_ParseRootEntity() metod, then don't throw.
    //
    m_c4ErrorCount++;
    if ((m_c4ErrorCount >= m_c4ErrorMax) && !m_bInException)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcParse_MaxErrsReached
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::OutResource
        );
    }
}


tCIDLib::TVoid
TXMLParserCore::ThrowParseError(const tCIDLib::TErrCode errcToThrow)
{
    // Build and throw an XML exception
    throw TXMLException
    (
        errcToThrow
        , facCIDXML().strMsg(errcToThrow)
        , m_xemThis.strCurSystemId()
        , m_xemThis.c4CurLine()
        , m_xemThis.c4CurColumn()
    );
}


// ---------------------------------------------------------------------------
//  TXMLParserCore: Private, non-virtual methods
// ---------------------------------------------------------------------------
//
//  When we get called here, the '&#' part of the ref has been seen, so we
//  know its got to be a char ref.
//
tCIDLib::TBoolean
TXMLParserCore::bExpandCharRef(tCIDLib::TCh& chToFill, tCIDLib::TCh& chSecond)
{
    //
    //  The next char might be the optional hex radix indicator. If so, then
    //  adjust the radix.
    //
    tCIDLib::ERadices eRadix = tCIDLib::ERadices::Dec;
    if (m_xemThis.bSkippedChar(kCIDLib::chLatin_x))
    {
        eRadix = tCIDLib::ERadices::Hex;
    }
     else if (m_xemThis.bSkippedChar(kCIDLib::chLatin_X))
    {
        PostXMLError(kXMLErrs::errcXMLE_BadHexRadix);
        eRadix = tCIDLib::ERadices::Hex;
    }

    //
    //  Now we loop until we see the terminating semicolon. Everything we
    //  see from here to there must be legal digits in the radix we got.
    //
    tCIDLib::TCard4 c4Accum = 0;
    tCIDLib::TCard4 c4CurDigit;
    while (kCIDLib::True)
    {
        const tCIDLib::TCh chNext = m_xemThis.chGetNext();

        if (!chNext)
            ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);

        // Break out on our terminating char
        if (chNext == kCIDLib::chSemiColon)
            break;

        //
        //  Convert the digit into a binary value. If its not a legal digit
        //  then decide what to do.
        //
        if ((chNext >= kCIDLib::chDigit0) && (chNext <= kCIDLib::chDigit9))
            c4CurDigit = (tCIDLib::TCard4)(chNext - kCIDLib::chDigit0);
        else if ((chNext >= kCIDLib::chLatin_a) && (chNext <= kCIDLib::chLatin_f))
            c4CurDigit = (tCIDLib::TCard4)(10 + (chNext - kCIDLib::chLatin_a));
        else if ((chNext >= kCIDLib::chLatin_A) && (chNext <= kCIDLib::chLatin_F))
            c4CurDigit= (tCIDLib::TCard4)(10 + (chNext - kCIDLib::chLatin_A));
        else
        {
            PostXMLError(kXMLErrs::errcXMLE_InvalidCharRefChar);
            return kCIDLib::False;
        }

        //
        //  Make sure its a legal digit in the desired radix. If not, then
        //  take the same action as above.
        //
        if (c4CurDigit >= tCIDLib::TCard4(eRadix))
        {
            PostXMLError(kXMLErrs::errcXMLE_InvalidCharRefChar);
            return kCIDLib::False;
        }

        // Add the new digit into out accumulator
        c4Accum = (c4Accum * tCIDLib::c4EnumOrd(eRadix)) + c4CurDigit;
    }

    //
    //  Lets put the value into the return parameters. We have to deal wiht
    //  surrogate pairs here.
    //
    if (c4Accum > 0xFFFF)
    {
        chToFill = tCIDLib::TCh(((c4Accum - 0x00010000) >> 10) + 0xD800);
        chSecond = tCIDLib::TCh(((c4Accum - 0x00010000) & 0x3FF) + 0xDC00);
    }
     else
    {
        chSecond = kCIDLib::chNull;
        chToFill = tCIDLib::TCh(c4Accum);
    }
    return kCIDLib::True;
}


//
//  This is called after we've seen xxx= and we now need to get the quoted
//  attribute value of this attribute. We have to do the required normalization
//  of whitespace, according to the type of the attribute.
//
tCIDLib::TBoolean
TXMLParserCore::bParseAttrValue(const  TXMLAttrDef& xadCur
                                ,      TString&     strToFill)
{
    //
    //  We will do a little state machine in the loop below to handle the
    //  normalization of whitespace. We need to know when we are inside
    //  whitespace and when we are in non-whitespace.
    //
    enum EWSNormStates
    {
        EState_InWS
        , EState_InNonWS
    };

    // We have to have a quote char now or its bad
    tCIDLib::TCh chQuote;
    if (!m_xemThis.bSkippedQuote(chQuote))
    {
        PostXMLError(kXMLErrs::errcXMLE_ExpectedAttrValue);
        return kCIDLib::False;
    }

    // Reset the buffer that we are to fill
    strToFill.Clear();

    //
    //  Get the type out for quick access below. We need this in order to
    //  do whitespace normalization below.
    //
    const tCIDXML::EAttrTypes eType = xadCur.eType();

    //
    //  Get the current entity spooler id. We have to read in data until we
    //  hit the same quote char in the same entity again. We ignore quotes in
    //  nested entities.
    //
    const tCIDLib::TCard4  c4OrgSpooler = m_xemThis.c4CurSpoolerId();

    //
    //  Prime the pump by gettng the first char and figuring out what state
    //  we should start in.
    //
    EWSNormStates eState =  TXMLCharFlags::bIsSpace(m_xemThis.chPeekNext())
                            ? EState_InWS : EState_InNonWS;

    //
    //  Do a preliminary check where we just get rid of leading whitespace
    //  if its not CDATA.
    //
    if ((eType != tCIDXML::EAttrTypes::CData)
    &&  (eState == EState_InWS))
    {
        // Skip over any leading spaces
        m_xemThis.bSkippedSpaces();

        // If we hit the end, we are done already and the value is empty
        if (m_xemThis.bSkippedChar(chQuote))
            return kCIDLib::True;

        eState = EState_InNonWS;
    }

    //
    //  Now enter the main loop where we handle the rest of the value. We
    //  do whitespace normalization here, as well as general entity expansion,
    //  and surrogate pair checking.
    //
    tCIDLib::TBoolean   bEscaped = kCIDLib::False;
    tCIDLib::TCh        chNext;
    tCIDLib::TCh        chSecond = kCIDLib::chNull;
    while (kCIDLib::True)
    {
        //
        //  Get a char for the next round. We have to use a second char of
        //  a surrogate pair if there, else get another char.
        //
        if (chSecond)
        {
            chNext = chSecond;
            chSecond = kCIDLib::chNull;
        }
         else
        {
            chNext = m_xemThis.chGetNext();
            if (!chNext)
                ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
        }

        //
        //  Watch for a < character, which is not valid directly within an
        //  attribute value. It has to be via a char ref.
        //
        if (chNext == kCIDLib::chLessThan)
            PostXMLError(kXMLErrs::errcXMLE_NotLegalInAttrVal);

        //
        //  Check for our break out quote char, but it has to be in the same
        //  entity or we ignore it.
        //
        if (chNext == chQuote)
        {
            const tCIDLib::TCard4 c4CurSpooler = m_xemThis.c4CurSpoolerId();

            // If its in the original entity, we are done
            if (c4CurSpooler == c4OrgSpooler)
                break;

            //
            //  Make sure we don't propogate out of the original entity
            //  without an error being issued. If they leave off the ending
            //  quote, this will happen.
            //
            if (c4OrgSpooler > c4CurSpooler)
            {
                PostXMLError(kXMLErrs::errcXMLE_PartialMarkupErr);
                return kCIDLib::False;
            }
        }

        //
        //  Check for an enity reference. Since we are in content here, we
        //  expand all general entities we see. It will a char ref into one
        //  or two chars (two if its a surrogate pair), or it will push a
        //  general entity on the entity manager stack.
        //
        //  External entity references aren't allowed here, so we pass false
        //  for the first parameter.
        //
        if (chNext == kCIDLib::chAmpersand)
        {
            tCIDLib::TCh chCharRef;
            tCIDXML::EEntityRes eRes = eExpandEntityRef
            (
                kCIDLib::False
                , chCharRef
                , chSecond
                , bEscaped
                , tCIDXML::ERefFrom::Literal
            );

            // If it was pushed or empty, then jump to top again
            if ((eRes == tCIDXML::EEntityRes::Pushed)
            ||  (eRes == tCIDXML::EEntityRes::Empty))
            {
                continue;
            }

            // It was returned to us by value
            chNext = chCharRef;
        }

        //
        //  Ok here we handle normalization according to the attribute type.
        //  For CData we change all whitespace to spaces. For others, we
        //  turn all groups of whitespace into a single space.
        //
        if (eType == tCIDXML::EAttrTypes::CData)
        {
            if (!bEscaped)
            {
                if ((chNext == 0x09) || (chNext == 0x0A) || (chNext == 0x0D))
                {
                    chNext = kCIDLib::chSpace;
                }
                 else if (!TXMLCharFlags::bIsXMLChar(chNext))
                {
                    // Got to be at least a legal XML char
                    PostXMLError(kXMLErrs::errcXMLE_NonXMLCharacter);
                    continue;
                }
            }
             else
            {
                // Got to be at least a legal XML char
                if (!TXMLCharFlags::bIsXMLChar(chNext))
                {
                    PostXMLError(kXMLErrs::errcXMLE_NonXMLCharacter);
                    continue;
                }
            }
        }
         else
        {
            if (eState == EState_InWS)
            {
                //
                //  If its a space, just go back to the top and try again
                //  since we are already in the whitespace state.
                //
                if (TXMLCharFlags::bIsSpace(chNext))
                    continue;

                //
                //  We've moved from the WS mode to content mode, so output
                //  a single char to represent the WS we processed up to
                //  here. But, if the buffer is empty, don't do it because
                //  we eat leading spaces.
                //
                // Got to be at least a legal XML char
                //
                if (TXMLCharFlags::bIsXMLChar(chNext))
                {
                    if (!strToFill.bIsEmpty())
                        strToFill.Append(kCIDLib::chSpace);
                    eState = EState_InNonWS;
                }
                 else
                {
                     // Not even a valid XML character
                    PostXMLError(kXMLErrs::errcXMLE_NonXMLCharacter);
                    continue;
                }
            }
             else
            {
                //
                //  If we transition from content to whitespace, then we just
                //  want to eat this space, change states, and try again.
                //
                if (TXMLCharFlags::bIsSpace(chNext))
                {
                    eState = EState_InWS;
                    continue;
                }
                 else if (!TXMLCharFlags::bIsXMLChar(chNext))
                {
                    // Got to be at least a legal XML char
                    PostXMLError(kXMLErrs::errcXMLE_NonXMLCharacter);
                    continue;
                }
            }
        }

        // It was a content char so just add it in
        strToFill.Append(chNext);
    }
    return kCIDLib::True;
}


tCIDXML::EMarkupTypes
TXMLParserCore::eNextMarkupType(tCIDLib::TCard4& c4OrgSpooler)
{
    // Peek the next character which gives us the primary hint we need
    tCIDLib::TCh chNext = m_xemThis.chPeekNext();

    //
    //  Its legal at this top level to have end of input if the element
    //  stack is empty (which is the caller's call.)
    //
    if (!chNext)
        return tCIDXML::EMarkupTypes::EOI;

    //
    //  If this is not one of the special token character, then we have to
    //  assume that its just characters.
    //
    if (!TXMLCharFlags::bIsMarkupTestChar(chNext))
        return tCIDXML::EMarkupTypes::Characters;

    //
    //  See if its an ampersand. If so, we just assume that its going to be
    //  an entity reference. From our perspective, we process entity refs
    //  as character data since that code already has to look for them anyway.
    //
    if (chNext == kCIDLib::chAmpersand)
        return tCIDXML::EMarkupTypes::Characters;

    //
    //  Ok, we know we are going to eat this character one way or another so
    //  lets get it and throw it away and peek the next one.
    //
    m_xemThis.chGetNext();
    chNext = m_xemThis.chPeekNext();

    // Return the entity we found this char in
    c4OrgSpooler = m_xemThis.c4CurSpoolerId();

    //
    //  If its a question, then assume its a PI (Decls aren't allowed here).
    //  If its a forward slash, assume an end tag. If its a bang, it could be
    //  a CDATA or comment.
    //
    if (chNext == kCIDLib::chForwardSlash)
    {
        // Eat this char that we peeked
        m_xemThis.chGetNext();
        return tCIDXML::EMarkupTypes::EndTag;
    }

    if (chNext == kCIDLib::chQuestionMark)
    {
        // Eat this char that we peeked
        m_xemThis.chGetNext();
        return tCIDXML::EMarkupTypes::PI;
    }

    if (chNext == kCIDLib::chExclamation)
    {
        // Eat the previous char and get another one
        m_xemThis.chGetNext();
        chNext = m_xemThis.chGetNext();

        if (chNext == kCIDLib::chOpenBracket)
        {
            //
            //  It has to be a CDATA section here, so see if we skip the
            //  string. If so, return that markup value, else issue and
            //  error and return unknown.
            //
            if (m_xemThis.bSkippedString(L"CDATA"))
                return tCIDXML::EMarkupTypes::CDATA;

            PostXMLError(kXMLErrs::errcXMLE_ExpectedCDATA);
        }
         else if (chNext == kCIDLib::chHyphenMinus)
        {
            //
            //  Its got to be a comment here, so wee if we skip the required
            //  two dashes. If so, return comment. Else issue an error and
            //  return unknown.
            //
            if (m_xemThis.bSkippedChar(kCIDLib::chHyphenMinus))
                return tCIDXML::EMarkupTypes::Comment;

            PostXMLError(kXMLErrs::errcXMLE_CommentsMustStartWith);
        }
         else
        {
            PostXMLError(kXMLErrs::errcXMLE_ExpectedCommentOrCDATA);
        }
        return tCIDXML::EMarkupTypes::Unknown;
    }

    // Oh well, assume its an element name
    return tCIDXML::EMarkupTypes::StartTag;
}


//
//  This method is called when we see a & character in character data (i.e.
//  outside any markup.)
//
tCIDXML::EEntityRes
TXMLParserCore::eExpandEntityRef(const  tCIDLib::TBoolean   bExtOk
                                ,       tCIDLib::TCh&       chToFill
                                ,       tCIDLib::TCh&       chSecond
                                ,       tCIDLib::TBoolean&  bEscaped
                                , const tCIDXML::ERefFrom   eFrom)
{
    //
    //  If the next char is a pound sign, then its a char ref. Else its a
    //  regular general entity.
    //
    if (m_xemThis.bSkippedChar(kCIDLib::chPoundSign))
    {
        // Its going to be escaped since its a char ref
        bEscaped = kCIDLib::True;

        // Call a local to expand it
        if (!bExpandCharRef(chToFill, chSecond))
            return tCIDXML::EEntityRes::Empty;
        return tCIDXML::EEntityRes::Returned;
    }

    //
    //  Ok its not a numeric char ref, so its got to be either a general ref
    //  or one of the magical char refs. So lets get the name and look it up
    //  in the entity pool of the validator.
    //
    TXMLBufJan  janName(m_pxbmBuf, 128);
    TString&    strName = *janName;

    if (!m_xemThis.bGetName(strName))
    {
        PostXMLError(kXMLErrs::errcXMLE_ExpectedEntityName);
        if (!m_xemThis.bSkipPastChar(kCIDLib::chGreaterThan))
            ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
        return tCIDXML::EEntityRes::Empty;
    }

    // Next we skip the terminating semicolon
    if (!m_xemThis.bSkippedChar(L';'))
        PostXMLError(kXMLErrs::errcXMLE_UnterminatedERef);

    const TXMLEntityDecl* pxdeclEntity
                            = m_pxvalValidator->pxdeclFindEntityByName(strName);

    // If we didn't find it, then say so and give up
    if (!pxdeclEntity)
    {
        PostXMLError(kXMLErrs::errcXMLE_EntityNotFound, strName);
        if (!m_xemThis.bSkipPastChar(kCIDLib::chGreaterThan))
            ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
        return tCIDXML::EEntityRes::Empty;
    }

    //
    //  Check if its one of the automagical char refs. If so, we can optimize
    //  here by just returning it directly. The value is just the 0th char
    //  of the value string.
    //
    if (pxdeclEntity->bIsSpecial())
    {
        bEscaped = kCIDLib::True;
        chSecond = kCIDLib::chNull;
        chToFill = pxdeclEntity->strValue().chAt(0);
        return tCIDXML::EEntityRes::Returned;
    }

    // Its not going to be escaped
    bEscaped = kCIDLib::False;

    //
    //  If the entity is an internal entity, then we create one type of
    //  spooler, else we create one for an external entity.
    //
    TXMLEntSpooler* pxesEntity;
    if (pxdeclEntity->bIsInternal())
    {
        pxesEntity = m_xemThis.pxesMakeNewFromInt
        (
            *pxdeclEntity, eFrom, tCIDXML::EEntityTypes::General
        );
    }
     else
    {
        // Make sure an external entity is ok
        if (!bExtOk)
            PostXMLError(kXMLErrs::errcXMLE_ExtEntityNotAllowed);

        // Make sure its a parsed entity
        if (pxdeclEntity->bIsUnparsed())
            PostXMLError(kXMLErrs::errcXMLE_UnparsedNotAllowed);

        // Do the entity resolution, if we have an installed resolover
        tCIDXML::TEntitySrcRef esrRef;
        if (m_pmxevEntityResolver)
        {
            esrRef = m_pmxevEntityResolver->esrResolve
            (
                pxdeclEntity->strPublicId()
                , pxdeclEntity->strSystemId()
                , pxdeclEntity->strName()
                , m_xemThis.strLastExtId()
                , tCIDXML::EResolveTypes::GeneralEntity
            );
        }

        // If it wasn't resolved, let's do it ourself
        if (!esrRef)
        {
            //
            //  Assume its a local file path. If the new file is not fully
            //  qualified, then weave it onto the base of the parent
            //  entity. Else, take it as it is.
            //
            TPathStr pathTmp(m_xemThis.strLastExtId());

            if (pathTmp.bIsFullyQualified())
            {
                // If fully qualified, so take it as is
                esrRef.SetPointer
                (
                    new TFileEntitySrc(pxdeclEntity->strSystemId())
                );
            }
             else
            {
                //
                //  Remove the last level, which is the previous entity's
                //  actual name, and use that as the base path.
                //
                pathTmp.bRemoveLevel();
                esrRef.SetPointer
                (
                    new TFileEntitySrc(pathTmp, pxdeclEntity->strSystemId())
                );
            }
        }

        // And create a new entity spooler from this source
        pxesEntity = m_xemThis.pxesMakeNew
        (
            esrRef, kCIDLib::True, eFrom, tCIDXML::EEntityTypes::General
        );
    }

    //
    //  And now push the entity onto the stack. We don't care if it fails
    //  here. It will issue an error, and we'll just return that its pushed.
    //  It wasn't really but it won't matter to the caller.
    //
    m_xemThis.bPushEntity(pxesEntity, pxdeclEntity, *this);

    //
    //  If its external, then we need to check for a text decl and parse it
    //  out if so.
    //
    if (pxdeclEntity->bIsExternal())
    {
        tCIDLib::TBoolean bDeclFound = m_xemThis.bSkippedString(L"<?xml ");
        if (!bDeclFound)
        {
            // Watch for a decl with incorrect case, just in case... so to speak
            bDeclFound = m_xemThis.bSkippedString(L"<?XML ");
            if (bDeclFound)
                PostXMLError(kXMLErrs::errcXMLE_BadDeclCase);
        }
        if (bDeclFound)
            ParseExtDecl(tCIDXML::EDeclTypes::Text);
    }

    // We pushed it as an entity so let the caller know
    return tCIDXML::EEntityRes::Pushed;
}


//
//  When we get here, the <!-- has been seen. So we need to get the comment
//  text and find the comment termination.
//
tCIDLib::TVoid TXMLParserCore::ParseComment()
{
    // Get a buffer to parse the comment text into
    TXMLBufJan  janBuf(m_pxbmBuf, 1024);
    TString&    strComment = *janBuf;

    //
    //  We need to do a little state machine here, go catch the end of
    //  command and to check for illegal -- sequences.
    //
    enum EStates
    {
        EState_Content
        , EState_Dash
        , EState_DoubleDash
    };

    //
    //  Ok, lets loop until we hit the end of input or the correctly
    //  terminated commented.
    //
    EStates eState = EState_Content;
    while (kCIDLib::True)
    {
        // Get our next character
        const tCIDLib::TCh chCur = m_xemThis.chGetNext();

        // Watch for the end of input
        if (!chCur)
        {
            PostXMLError(kXMLErrs::errcXMLE_UnterminatedComment);
            return;
        }

        //
        //  Since these are not being tested against any other criteria
        //  we have to check that they are at least valid XML chars.
        //
        if (!TXMLCharFlags::bIsXMLChar(chCur))
            PostXMLError(kXMLErrs::errcXMLE_NonXMLCharacter);

        // According to the current state, process it
        if (eState == EState_Content)
        {
            // If its a dash, then change the state, else store it
            if (chCur == kCIDLib::chHyphenMinus)
                eState = EState_Dash;
            else
                strComment.Append(chCur);
        }
         else if (eState == EState_Dash)
        {
            //
            //  If its a dash, then we change to the double dash state.
            //  Otherwise, we go back to start state and output the deferred
            //  dash and the new char.
            //
            if (chCur == kCIDLib::chHyphenMinus)
            {
                eState = EState_DoubleDash;
            }
             else
            {
                strComment.Append(kCIDLib::chHyphenMinus);
                strComment.Append(chCur);
                eState = EState_Content;
            }
        }
         else if (eState == EState_DoubleDash)
        {
            //
            //  We have to get closing bracket here or its an invalid
            //  comment.
            //
            if (chCur != kCIDLib::chGreaterThan)
            {
                PostXMLError(kXMLErrs::errcXMLE_NotAllowedInComment);
                if (!m_xemThis.bSkipPastChar(kCIDLib::chGreaterThan))
                    ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
                return;
            }

            // We got it, so break out
            break;
        }
    }

    //
    //  If the client code wants to see comments in our current position
    //  then tell him about it.
    //
    if (m_pmxevDocEvents && bInfoWanted(tCIDXML::EParseFlags::CommentMask))
        m_pmxevDocEvents->DocComment(strComment, m_eLocation);
}


//
//  When we get here, we've parsed the <? characters. So we have to get the
//  PI target and optional PI data.
//
tCIDLib::TVoid TXMLParserCore::ParsePI()
{
    // Get a buffer to parse the target into
    TXMLBufJan  janTarget(m_pxbmBuf, 128);
    TString&    strTarget = *janTarget;

    //
    //  There aren't supposed to be any spaces before the target. Check for
    //  them and issue an error if found, then skip over them and keep going
    //
    if (m_xemThis.bSkippedSpaces())
        PostXMLError(kXMLErrs::errcXMLE_NoSpaceBeforePITarget);

    // And now try to get the target name
    if (!m_xemThis.bGetNameToken(strTarget))
    {
        PostXMLError(kXMLErrs::errcXMLE_ExpectedPITarget);
        if (!m_xemThis.bSkipPastChar(kCIDLib::chGreaterThan))
            ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
        return;
    }

    // Now get a buffer to get the value, if any, into
    TXMLBufJan  janValue(m_pxbmBuf, 1024);
    TString&    strValue = *janValue;

    //
    //  Now we do something different according to whether we get any spaces
    //  or not. If not, then we assume it has no value.
    //
    if (m_xemThis.bSkippedSpaces())
    {
        //
        //  Lets collect characters until we hit the end of the PI or the
        //  end of input.
        //
        while (kCIDLib::True)
        {
            const tCIDLib::TCh chCur = m_xemThis.chGetNext();

            if (!chCur)
            {
                PostXMLError(kXMLErrs::errcXMLE_UnterminatedPI);
                return;
            }

            if (chCur == kCIDLib::chQuestionMark)
            {
                if (m_xemThis.bSkippedChar(kCIDLib::chGreaterThan))
                    break;
            }

            // Check for invalid chars
            if (!TXMLCharFlags::bIsXMLChar(chCur))
                PostXMLError(kXMLErrs::errcXMLE_NonXMLCharacter);

            strValue.Append(chCur);
        }
    }
     else
    {
        // Make sure its terminated ok
        if (!m_xemThis.bSkippedChar(kCIDLib::chQuestionMark))
        {
            PostXMLError(kXMLErrs::errcXMLE_UnterminatedPI);
            if (!m_xemThis.bSkipPastChar(kCIDLib::chGreaterThan))
                ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
            return;
        }

        if (!m_xemThis.bSkippedChar(kCIDLib::chGreaterThan))
        {
            PostXMLError(kXMLErrs::errcXMLE_UnterminatedPI);
            if (!m_xemThis.bSkipPastChar(kCIDLib::chGreaterThan))
                ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
            return;
        }
    }

    // If the client code wants this info, then pass it on
    if (m_pmxevDocEvents && bInfoWanted(tCIDXML::EParseFlags::PIMask))
        m_pmxevDocEvents->DocPI(strTarget, strValue, m_eLocation);
}


tCIDLib::TVoid TXMLParserCore::PassContentChars(TString& strChars)
{
    // If the buffer is empty, then do nothing
    if (strChars.bIsEmpty())
        return;

    // Get the text type flag for the current element
    tCIDXML::EElemTextTypes eTextType = m_xcsElems.eTextType();

    //
    //  If the text type indicates that this element cannot accept any
    //  character data and we are validating, then lets issue the error
    //  now and give up.
    //
    if (tCIDLib::bAllBitsOn(m_eOptions, tCIDXML::EParseOpts::Validate)
    &&  (eTextType == tCIDXML::EElemTextTypes::None))
    {
        TTextStringOutStream strmCM(128);
        tCIDLib::TCard4 c4SpoolerId;
        const TXMLElemDecl& xdeclTop = m_xcsElems.xdeclTopElem(c4SpoolerId);
        xdeclTop.FormatCMTo(strmCM, *m_pxvalValidator);
        strmCM.Flush();

        PostXMLError(kXMLErrs::errcXMLV_CharsNotLegal, strmCM.strData());
        strChars.Clear();
        return;
    }

    // See if the text is all spaces
    const tCIDLib::TBoolean bAllSpaces = facCIDXML().bIsAllSpaces(strChars);

    //
    //  If we are validating and the element can only accept whitespace and
    //  its not all spaces, then issue an error and give up.
    //
    if (tCIDLib::bAllBitsOn(m_eOptions, tCIDXML::EParseOpts::Validate)
    &&  (eTextType == tCIDXML::EElemTextTypes::SpaceIgnorable)
    &&  !bAllSpaces)
    {
        TTextStringOutStream strmCM(128);
        tCIDLib::TCard4 c4SpoolerId;
        const TXMLElemDecl& xdeclTop = m_xcsElems.xdeclTopElem(c4SpoolerId);
        xdeclTop.FormatCMTo(strmCM, *m_pxvalValidator);
        strmCM.Flush();

        PostXMLError(kXMLErrs::errcXMLV_OnlyWSLegal, strmCM.strData());
        strChars.Clear();
        return;
    }

    //
    //  Figure out what our mask should be. Its either chars, or ignorable
    //  white space if its all spacse and the current element ignores spaces
    //  and we are validating.
    //
    tCIDXML::EParseFlags eMask = tCIDXML::EParseFlags::Chars;

    if (tCIDLib::bAllBitsOn(m_eOptions, tCIDXML::EParseOpts::Validate)
    &&  (eTextType == tCIDXML::EElemTextTypes::SpaceIgnorable)
    &&  bAllSpaces)
    {
        eMask = tCIDXML::EParseFlags::IgnorableSpace;
    }

    // If they want to see this content, then send it
    if (m_pmxevDocEvents && bInfoWanted(eMask))
    {
        m_pmxevDocEvents->DocCharacters
        (
            strChars
            , kCIDLib::False
            , (eMask == tCIDXML::EParseFlags::IgnorableSpace)
            , m_eLocation
        );
    }
    strChars.Clear();
}
