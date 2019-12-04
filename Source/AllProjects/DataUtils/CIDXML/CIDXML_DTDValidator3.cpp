//
// FILE NAME: CIDXML_DTDValidator3.Cpp
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
//  This file implements some of the grunt work DTD parsing methods from
//  CIDXML_DTDValidator2.Cpp.
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
//  TDTDValidator: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  When we get called here, the '&#' part of the ref has been seen, so we
//  know its got to be a char ref.
//
tCIDLib::TBoolean
TDTDValidator::bExpandCharRef(tCIDLib::TCh& chToFill, tCIDLib::TCh& chSecond)
{
    //
    //  The next char might be the optional hex radix indicator. If so, then
    //  adjust the radix.
    //
    tCIDLib::ERadices eRadix = tCIDLib::ERadices::Dec;
    if (xemOwner().bSkippedChar(kCIDLib::chLatin_x))
    {
        eRadix = tCIDLib::ERadices::Hex;
    }
     else if (xemOwner().bSkippedChar(kCIDLib::chLatin_X))
    {
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_BadHexRadix);
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
        const tCIDLib::TCh chNext = xemOwner().chGetNext();

        if (!chNext)
            xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);

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
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_InvalidCharRefChar);

            //
            //  Its not a valid digit. So, if we've accumulated anything
            //  besides zero, we just take that and break out. Else, we just
            //  give up altogether and fail.
            //
            if (c4Accum)
                break;

            return kCIDLib::False;
        }

        //
        //  Make sure its a legal digit in the desired radix. If not, then
        //  take the same action as above.
        //
        if (c4CurDigit >= tCIDLib::TCard4(eRadix))
        {
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_InvalidCharRefChar);
            if (c4Accum)
                break;

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
//  This method is called from many places in the DTD parsing code. At all
//  these places, a PE ref is possible. So this method will check for spaces
//  if required and for a PE ref, which it will expand.
//
tCIDLib::TBoolean
TDTDValidator::bHandleWSOrPERef(const tCIDXML::EPERefOpts eOptions)
{
    // Do an initial check for spaces
    tCIDLib::TBoolean bGotSpace = xemOwner().bSkippedSpaces(kCIDLib::True);

    //
    //  Then check for a PE ref. If it looks like it could be one, then lets
    //  expand it if so.
    //
    if (xemOwner().bSkippedChar(kCIDLib::chPercentSign))
        ExpandPEntityRef(kCIDLib::False, eOptions);

    // And skip any leading spaces in the PE ref
    if (xemOwner().bSkippedSpaces(kCIDLib::True))
        bGotSpace = kCIDLib::True;

    // If we did not get a spaces, then issue an error if it was required
    if (!bGotSpace)
    {
        if (tCIDLib::bAllBitsOn(eOptions, tCIDXML::EPERefOpts::SpaceRequired))
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedWhitespace);
    }
    return bGotSpace;
}


//
//  This method is called to parse a default/fixed attribute value in an
//  AttList. Its a quoted value and we have to handle  The return indicates
//  whether the value was syntactically correct, not whether it meets validity
//  constraints, i.e, its an indicator of whether the caller should try to keep
//  parsing, not whether the default attribute was valid.
//
tCIDLib::TBoolean TDTDValidator::bParseDefAttrValue(TDTDAttrDef& xadToFill)
{
    //
    //  We will do a little state machine in the loop below to handle the
    //  normalization of whitespace. We need to know when we are inside
    //  whitespace and when we are in non-whitespace.
    //
    enum class EWSNormStates
    {
        InWS
        , InNonWS
    };

    // We have to have a quote char now or its bad
    tCIDLib::TCh chQuote;
    if (!xemOwner().bSkippedQuote(chQuote))
    {
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedAttrValue);
        return kCIDLib::False;
    }

    //
    //  Get the type out for quick access below. We need this in order to
    //  do whitespace normalization below.
    //
    const tCIDXML::EAttrTypes eType = xadToFill.eType();

    //
    //  Get the current entity spooler id. We have to read in data until we
    //  hit the same quote char in the same entity again. We ignore quotes in
    //  nested entities.
    //
    const tCIDLib::TCard4 c4OrgSpooler = xemOwner().c4CurSpoolerId();

    //
    //  Prime the pump by gettng the first char and figuring out what state
    //  we should start in.
    //
    EWSNormStates eState =  TXMLCharFlags::bIsSpace(xemOwner().chPeekNext())
                            ? EWSNormStates::InWS : EWSNormStates::InNonWS;

    //
    //  Do a preliminary check where we just get rid of leading whitespace
    //  if its not CDATA.
    //
    if ((eType != tCIDXML::EAttrTypes::CData)
    &&  (eState == EWSNormStates::InWS))
    {
        // Skip over any leading spaces
        xemOwner().bSkippedSpaces(kCIDLib::True);

        // If we hit the end, we are done already and the value is empty
        if (xemOwner().bSkippedChar(chQuote))
            return kCIDLib::True;

        eState = EWSNormStates::InNonWS;
    }

    // Get a buffer to put the value into
    TXMLBufJan  janBuf(&xbmOwner(), 256);
    TString&    strValue = *janBuf;

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
            chNext = xemOwner().chGetNext();
        }

        //
        //  Check for our break out quote char, but it has to be in the same
        //  entity or we ignore it.
        //
        if (chNext == chQuote)
        {
            const tCIDLib::TCard4 c4CurSpooler = xemOwner().c4CurSpoolerId();

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
                xprsOwner().PostXMLError(kXMLErrs::errcXMLE_PartialMarkupErr);
                return kCIDLib::False;
            }
        }

        //
        //  Check for an enity reference. Since we are in content here, we
        //  expand all general entities we see. It will a char ref into one
        //  or two chars (two if its a surrogate pair), or it will push a
        //  general entity on the entity manager stack.
        //
        //  External entities are not allowed here, so we pass false for the
        //  first parameter.
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
                    chNext = kCIDLib::chSpace;
            }
        }
         else
        {
            if (eState == EWSNormStates::InWS)
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
                //  here.
                //
                strValue.Append(kCIDLib::chSpace);
                eState = EWSNormStates::InNonWS;
            }
             else
            {
                //
                //  If we transition from content to whitespace, then we just
                //  want to eat this space, change states, and try again.
                //
                if (TXMLCharFlags::bIsSpace(chNext))
                {
                    eState = EWSNormStates::InWS;
                    continue;
                }
            }
        }

        // It was a content char so just add it in
        strValue.Append(chNext);
    }

    // Ok we got it, so lets set the value in the passed attr def
    xadToFill.strValue(strValue);

    //
    //  If we are validating, then see if its an ID attribute. If so, then
    //  it must have a default type of required or implied. And, we need to
    //  make sure that the default value is legal for the type.
    //
    if (tCIDLib::bAllBitsOn(xprsOwner().eOptions(), tCIDXML::EParseOpts::Validate))
    {
        if (xadToFill.eType() == tCIDXML::EAttrTypes::ID)
        {
            if ((xadToFill.eDefaultType() != tCIDXML::EDefAttrTypes::Implied)
            &&  (xadToFill.eDefaultType() != tCIDXML::EDefAttrTypes::Required))
            {
                xprsOwner().PostXMLError
                (
                    kXMLErrs::errcXMLV_IDAttrDefType
                    , xadToFill.strFullName()
                );
            }
        }

        // We don't return this result, since its just about validity
        bValidateAttr(xadToFill, strValue);
    }
    return kCIDLib::True;
}


//
//  This method is called from ParseEntity() when the entity value is an
//  inline string. It handles getting the literal value in, expanding any
//  char refs, and normalizing as required.
//
tCIDLib::TBoolean
TDTDValidator::bParseEntityValue(        TString&       strToFill
                                , const tCIDLib::TCh    chQuote)
{
    // Get the current entity id, so that we can catch any partial markup
    const tCIDLib::TCard4 c4OrgSpooler = xemOwner().c4CurSpoolerId();

    // Enter a loop where we read chars until we hit the closing quote
    tCIDLib::TBoolean bLeadingFlag = kCIDLib::False;
    tCIDLib::TCh chSecond = kCIDLib::chNull;

    while (kCIDLib::True)
    {
        //
        //  Get the next char out and see what it is. If we have a second
        //  char from a pair, the do it next instead of getting another char.
        //
        tCIDLib::TCh chNext;
        if (chSecond)
        {
            chNext = chSecond;
            chSecond = kCIDLib::chNull;
        }
         else
        {
            chNext = xemOwner().chGetNext();
        }

        //
        //  If its the terminating quote, get out. It has to be a quote in
        //  the original entity.
        //
        if (chNext == chQuote)
        {
            if (xemOwner().c4CurSpoolerId() == c4OrgSpooler)
                break;
        }

        // If end of input, thats not legal
        if (!chNext)
            xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);

        // If its a PE ref, then handle it
        if (chNext == kCIDLib::chPercentSign)
        {
            ExpandPEntityRef(kCIDLib::False, tCIDXML::EPERefOpts::LiteralMarkup);
            continue;
        }

        //
        //  We don't expand most general entities, but we do expand char
        //  refs here. If its not a char ref, then we just take it as literal
        //  data BUT it must map to a real entity name (which will be
        //  expanded when/if the entity is eventually evaluated.)
        //
        if (chNext == kCIDLib::chAmpersand)
        {
            if (xemOwner().bSkippedChar(kCIDLib::chPoundSign))
            {
                if (!bExpandCharRef(chNext, chSecond))
                {
                    bLeadingFlag = kCIDLib::False;
                    continue;
                }
            }
             else
            {
                // Get a name next
                TXMLBufJan  janName(&xbmOwner(), 128);
                TString&    strName = *janName;
                if (xemOwner().bGetName(strName))
                {
                    //
                    //  We've got to put the name back into the output mow,
                    //  since its really just literal data here other than
                    //  having to insure its a legal looking reference.
                    //
                    strToFill.Append(kCIDLib::chAmpersand);
                    strToFill.Append(strName);

                    if (!xemOwner().bSkippedChar(kCIDLib::chSemiColon))
                        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_UnterminatedERef);

                    chNext = kCIDLib::chSemiColon;
                }
                 else
                {
                    xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedEntityName);
                }
            }
        }

        // Check surrogate pairs here
        if ((chNext & 0xDC00) == 0xD800)
        {
            if (bLeadingFlag)
                xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedSurrogate);
            else
                bLeadingFlag = kCIDLib::True;
        }
         else
        {
            if (bLeadingFlag)
            {
                if ((chNext & 0xDC00) != 0xDC00)
                    xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedSurrogate);
            }
             else if (!TXMLCharFlags::bIsXMLChar(chNext))
            {
                xprsOwner().PostXMLError(kXMLErrs::errcXMLE_NonXMLCharacter);
                if (!xemOwner().bSkipPastChar(chQuote))
                    xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
                return false;
            }
            bLeadingFlag = kCIDLib::False;
        }

        // It looks legal, so lets take it
        strToFill.Append(chNext);
    }

    if (c4OrgSpooler != xemOwner().c4CurSpoolerId())
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_PartialMarkupErr);

    return kCIDLib::True;
}


//
//  This method is called when we have an enumeration or notation attribute
//  and we need to parse out the values. We just have to make sure that its
//  a legal value, then we'll set it on the attribute def. Since we've already
//  told it that its a multi-valued type, it'll pull the tokens out and store
//  them.
//
tCIDLib::TBoolean
TDTDValidator::bParseEnumList(TDTDAttrDef& xadToFill, const tCIDLib::TBoolean bTokens)
{
    // Get a buffer to put the value into and one for individual names
    TXMLBufJan  janValue(&xbmOwner(), 512);
    TString&    strValue = *janValue;

    TXMLBufJan  janName(&xbmOwner(), 128);
    TString&    strName = *janName;

    //
    //  We just loop until we get the closing paren. On each loop we just
    //  check that we get sequences of name characters and space. If bTokens
    //  is set, then the first non-space each time through has to be a first
    //  name char.
    //
    tCIDLib::TBoolean bOk;
    while (kCIDLib::True)
    {
        // We can have optional space or PE ref here
        bHandleWSOrPERef(tCIDXML::EPERefOpts::InMarkup);

        //
        //  Now we must see a name or name token, according to the value of
        //  the bTokens parameter.
        //
        if (bTokens)
            bOk = xemOwner().bGetNameToken(strName);
        else
            bOk = xemOwner().bGetName(strName);

        if (!bOk)
        {
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedEnumValue);
            return kCIDLib::False;
        }

        // We can now append this name to the value buffer
        strValue.Append(strName);

        // We can have optional space or PE ref here
        bHandleWSOrPERef(tCIDXML::EPERefOpts::InMarkup);

        // Now we either have to hit the close paren, or the | marker
        if (xemOwner().bSkippedChar(kCIDLib::chCloseParen))
            break;

        // If not, try to keep going, but issue an error
        if (!xemOwner().bSkippedChar(kCIDLib::chVerticalBar))
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedEnumSep);

        // And append the vertical bar
        strValue.Append(kCIDLib::chVerticalBar);
    }

    //
    //  Looks ok, so set it on the attr def. He will parse out the values and
    //  store them.
    //
    xadToFill.SetEnumValues(strValue);
    return kCIDLib::True;
}


//
//  This method will handle parsing any external id markup. Ids literals can
//  be of either the 'public' or 'system' variety. From our perspective for
//  parsing, there's not much difference other than public ids have a more
//  limited character set.
//
//  Use of ids are limited to two variations, 'external' and 'public'. A
//  public id is just a single public id literal. An external id can be either
//  a single system id, or a public and system id together.
//
//  The eType parameter tells us which usage to expect, and in some cases it
//  will be 'either' which means we should accept whichever variatin we find.
//
tCIDLib::TBoolean
TDTDValidator::bParseIds(         TString&            strSystemId
                            ,       TString&            strPublicId
                            , const tCIDXML::EIDTypes   eType)
{
    // Clear both of the buffers before we start filling them
    strSystemId.Clear();
    strPublicId.Clear();

    //
    //  There's a lot of ways to approach this, but lets start by seeing if
    //  we find the PUBLIC string first.
    //
    tCIDLib::TBoolean bRet = kCIDLib::True;
    if (xemOwner().bSkippedString(L"PUBLIC"))
    {
        //
        //  We have to have whitespace here. If we don't find it, issue an
        //  error but try to keep going.
        //
        if (!xemOwner().bSkippedSpaces(kCIDLib::True))
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedWhitespace);

        // And try to parse a public id literal
        if (!bParsePublicId(strPublicId))
        {
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedPublicId);
            return kCIDLib::False;
        }

        // If we were looking for a public, then we are done
        if (eType == tCIDXML::EIDTypes::Public)
            return kCIDLib::True;
    }
     else if (xemOwner().bSkippedString(L"SYSTEM"))
    {
        //
        //  If they were looking for a public, we failed. But go ahead nad
        //  try to continue.
        //
        if (eType == tCIDXML::EIDTypes::Public)
        {
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedPublicId);
            bRet = kCIDLib::False;
        }
    }
     else
    {
        // We should have gotten some sort of id type
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedIdType);
        return kCIDLib::False;
    }

    //
    //  Any way now we need to get a system id, with some preceeding white
    //  space. If we don't get the space, then issue an error but try to
    //  keep going.
    //
    if (!xemOwner().bSkippedSpaces(kCIDLib::True))
    {
        // If we could do either, then its ok not to have another id
        if (eType == tCIDXML::EIDTypes::Either)
            return kCIDLib::True;

        // Else we expected whitespace before the next id
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedWhitespace);
        return kCIDLib::False;
    }

    //
    //  Peek and see if we have a ' or " character. If not, and we expected
    //  an external id, then we failed. If we could take either, then we are
    //  ok.
    //
    const tCIDLib::TCh chNext = xemOwner().chPeekNext();
    if ((chNext != kCIDLib::chApostrophe) && (chNext != kCIDLib::chQuotation))
    {
        if (eType != tCIDXML::EIDTypes::Either)
        {
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedSystemId);
            return kCIDLib::False;
        }
    }

    // And now try to parse a system id literal
    if (!bParseSystemId(strSystemId))
        return kCIDLib::False;

    return kCIDLib::True;
}


tCIDLib::TBoolean TDTDValidator::bParsePublicId(TString& strToFill)
{
    tCIDLib::TCh chQuote;
    if (!xemOwner().bSkippedQuote(chQuote))
    {
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedQuote);
        return kCIDLib::False;
    }

    strToFill.Clear();
    while (kCIDLib::True)
    {
        const tCIDLib::TCh chNext = xemOwner().chGetNext();

        // Watch for the end of input
        if (!chNext)
            xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);

        // Break out when we see the matching quote
        if (chNext == chQuote)
            break;

        // Make sure its a valid public id char
        if (!TXMLCharFlags::bIsPublicIDChar(chNext))
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_MustBePublicIDChar);

        // Else save it as an id char
        strToFill.Append(chNext);
    }
    return kCIDLib::True;
}


//
//  This one is easy. There aren't any limitations on the chars that can
//  can go into a system id, so we just look for a quote char and then
//  collect chars until we hit the closing quote.
//
tCIDLib::TBoolean TDTDValidator::bParseSystemId(TString& strToFill)
{
    tCIDLib::TCh chQuote;
    if (!xemOwner().bSkippedQuote(chQuote))
    {
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedQuote);
        return kCIDLib::False;
    }

    strToFill.Clear();
    while (kCIDLib::True)
    {
        const tCIDLib::TCh chNext = xemOwner().chGetNext();

        // Watch for the end of input
        if (!chNext)
            xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);

        // Break out when we see the matching quote
        if (chNext == chQuote)
            break;

        // Else save it as an id char
        strToFill.Append(chNext);
    }
    return kCIDLib::True;
}


//
//  This method is called when we see a & character in character data (i.e.
//  outside any markup.)
//
tCIDXML::EEntityRes
TDTDValidator::eExpandEntityRef(  const   tCIDLib::TBoolean   bExtOk
                                    ,       tCIDLib::TCh&       chToFill
                                    ,       tCIDLib::TCh&       chSecond
                                    ,       tCIDLib::TBoolean&  bEscaped
                                    , const tCIDXML::ERefFrom   eFrom)
{
    //
    //  If the next char is a pound sign, then its a char ref. Else its a
    //  regular general entity.
    //
    if (xemOwner().bSkippedChar(kCIDLib::chPoundSign))
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
    TXMLBufJan  janName(&xbmOwner(), 128);
    TString&    strName = *janName;

    if (!xemOwner().bGetNameToken(strName))
    {
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedEntityName);
        if (!xemOwner().bSkipPastChar(kCIDLib::chGreaterThan))
            xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
        return tCIDXML::EEntityRes::Empty;
    }

    // Next we skip the terminating semicolon
    if (!xemOwner().bSkippedChar(kCIDLib::chSemiColon))
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_UnterminatedERef);

    // See if we can find this entity in the general entity pool
    const TXMLEntityDecl* pxdeclEntity = m_pxnipEntities->pobjByName(strName);

    // If we didn't find it, then say so and give up
    if (!pxdeclEntity)
    {
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_EntityNotFound, strName);
        if (!xemOwner().bSkipPastChar(kCIDLib::chGreaterThan))
            xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
        return tCIDXML::EEntityRes::Empty;
    }

    //
    //  If its an external and that's not ok here, then issue an error. Return
    //  that we pushed it. This will make the caller not care it didn't get
    //  parsed.
    //
    if (!bExtOk && pxdeclEntity->bIsExternal())
    {
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExtEntityNotAllowed);
        return tCIDXML::EEntityRes::Pushed;
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
    //  Lets create an entity spooler for this entity. The entity manager has
    //  a special factory method for entities from unparsed general entities.
    //
    TXMLEntSpooler* pxesEntity = xemOwner().pxesMakeNewFromInt
    (
        *pxdeclEntity
        , eFrom
        , tCIDXML::EEntityTypes::General
    );

    //
    //  And now push the entity onto the space. We don't care if it fails
    //  here. It will issue an error, and we'll just return that its pushed.
    //  It wasn't really but it won't matter to the caller.
    //
    xemOwner().bPushEntity(pxesEntity, pxdeclEntity, xprsOwner());

    // We pushed it as an entity so let the caller know
    return tCIDXML::EEntityRes::Pushed;
}


//
//  We get called here where we see a % character where an enity reference is
//  expected. We have to get the name, look it, and either parse it if it is
//  external and the bParseExt flag is set, or just push it as an entity and
//  return.
//
tCIDLib::TVoid
TDTDValidator::ExpandPEntityRef(  const   tCIDLib::TBoolean       bParseExt
                                    , const tCIDXML::EPERefOpts  eOptions)
{
    // Get a buffer to get the name into
    TXMLBufJan  janName(&xbmOwner(), 128);
    TString&    strName = *janName;

    // Lets get a name, which shuold be the name of the entity to expand
    if (!xemOwner().bGetNameToken(strName))
    {
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedEntityName);
        if (!xemOwner().bSkipPastChar(kCIDLib::chGreaterThan))
            xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
        return;
    }

    // Next we skip the terminating semicolon
    if (!xemOwner().bSkippedChar(kCIDLib::chSemiColon))
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_UnterminatedERef);

    // See if we can find this entity in the parameter entity pool
    const TDTDEntityDecl* pxdeclEntity = m_pxnipPEs->pobjByName(strName);

    // If we didn't find it, then say so and give up
    if (!pxdeclEntity)
    {
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_EntityNotFound, strName);
        if (!xemOwner().bSkipPastChar(kCIDLib::chGreaterThan))
            xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
        return;
    }

    //
    //  If we are in standalone mode, then check whether this entity was
    //  declared in the internal subset. If not, then that's an error.
    //
    if (xprsOwner().bStandalone())
    {
        if (pxdeclEntity->bFromIntSS())
        {
            xprsOwner().PostXMLError
            (
                kXMLErrs::errcXMLE_ExtEntRefInStandalone
                , pxdeclEntity->strName()
            );
        }
    }

    //
    //  If this ref was in markup, and we are in the internal subset, then
    //  this is an error.
    //
    if (tCIDLib::bAllBitsOn(eOptions, tCIDXML::EPERefOpts::InMarkup))
    {
        if (xprsOwner().eLocation() == tCIDXML::ELocations::IntSubset)
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_PEInMarkupInISS);
    }

    //
    //  If its an internal entity, then we can just create a spooler for
    //  the already internalized entity value. Otherwise, we have an external
    //  entity to parse.
    //
    if (pxdeclEntity->bIsInternal())
    {
        //
        //  The entity manager has a special factory method for entities from
        //  internal entities.
        //
        TXMLEntSpooler* pxesEntity = xemOwner().pxesMakeNewFromInt
        (
            *pxdeclEntity
            , tCIDLib::bAllBitsOn(eOptions, tCIDXML::EPERefOpts::InLiteral)
                    ? tCIDXML::ERefFrom::Literal : tCIDXML::ERefFrom::OutsideLiteral
            , tCIDXML::EEntityTypes::Parameter
        );

        //
        //  And now push the entity onto the stack. We don't care if it fails
        //  here. It will issue an error, and we'll just return.
        //
        xemOwner().bPushEntity(pxesEntity, pxdeclEntity, xprsOwner());
    }
     else
    {
        //
        //  If we have an entity resolver, then let it take a shot at doing
        //  the entity resolution. Else, do it ourself.
        //
        tCIDXML::TEntitySrcRef esrExt;
        if (xprsOwner().pmxevEntityResolver())
        {
            // Tell the resolover this is a parameter entity
            esrExt = xprsOwner().pmxevEntityResolver()->esrResolve
            (
                pxdeclEntity->strPublicId()
                , pxdeclEntity->strSystemId()
                , pxdeclEntity->strName()
                , xemOwner().strLastExtId()
                , tCIDXML::EResolveTypes::ParamEntity
            );
        }

        // If it wasn't resolved, then do it ourself
        if (!esrExt)
        {
            //
            //  Assume its a local file path. If the new file is not fully
            //  qualified, then weave it onto the base of the parent entity.
            //  Else, take it as it is.
            //
            TPathStr pathNew(pxdeclEntity->strSystemId());

            if (pathNew.bIsFullyQualified())
            {
                // If fully qualified, so take it as is
                esrExt.SetPointer(new TFileEntitySrc(pathNew));
            }
             else
            {
                //
                //  Remove the last level, which is the previous entity's
                //  actual name, and use that as the base path.
                //
                TPathStr pathParent(xemOwner().strLastExtId());
                pathParent.bRemoveLevel();
                esrExt.SetPointer
                (
                    new TFileEntitySrc
                    (
                        pathParent, pxdeclEntity->strSystemId()
                    )
                );
            }
        }

        TXMLEntSpooler* pxesEntity = xemOwner().pxesMakeNew
        (
            esrExt
            , kCIDLib::True
            , tCIDLib::bAllBitsOn(eOptions, tCIDXML::EPERefOpts::InLiteral)
                    ? tCIDXML::ERefFrom::Literal : tCIDXML::ERefFrom::OutsideLiteral
            , tCIDXML::EEntityTypes::Parameter
        );

        //
        //  If its empty, just delete it and do nothing. Else lets parse
        //  it if they told us to.
        //
        if (pxesEntity->bEmpty())
        {
            // Call the start and end entity events
            if (xprsOwner().pmxevEntityEvents())
            {
                xprsOwner().pmxevEntityEvents()->StartEntity(*pxdeclEntity);
                xprsOwner().pmxevEntityEvents()->EndEntity(*pxdeclEntity);
            }

            // And clean up the spooler since we'll never use it now
            delete pxesEntity;
        }
         else
        {
            // Push the entity and, if asked, parse it right now
            xemOwner().bPushEntity(pxesEntity, pxdeclEntity, xprsOwner());
            if (bParseExt)
                ParseExtEntity(kCIDLib::True);
        }
    }
}


//
//  When we get here, the <!-- has been seen. So we need to get the comment
//  text and find the comment termination.
//
tCIDLib::TVoid TDTDValidator::ParseComment()
{
    // Get a buffer to parse the comment text into
    TXMLBufJan  janBuf(&xbmOwner(), 1024);
    TString&    strComment = *janBuf;

    //
    //  We need to do a little state machine here, go catch the end of
    //  command and to check for illegal -- sequences.
    //
    enum class EStates
    {
        Content
        , Dash
        , DoubleDash
    };

    //
    //  Ok, lets loop until we hit the end of input or the correctly
    //  terminated commented.
    //
    EStates eState = EStates::Content;
    while (kCIDLib::True)
    {
        // Get our next character
        const tCIDLib::TCh chCur = xemOwner().chGetNext();

        //
        //  Since these are not being tested against any other criteria
        //  we have to check that they are at least valid XML chars.
        //
        if (!TXMLCharFlags::bIsXMLChar(chCur))
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_NonXMLCharacter);

        // Watch for the end of input
        if (!chCur)
        {
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_UnterminatedComment);
            return;
        }

        // According to the current state, process it
        if (eState == EStates::Content)
        {
            // If its a dash, then change the state, else store it
            if (chCur == kCIDLib::chHyphenMinus)
                eState = EStates::Dash;
            else
                strComment.Append(chCur);
        }
         else if (eState == EStates::Dash)
        {
            //
            //  If its a dash, then we change to the double dash state.
            //  Otherwise, we go back to start state and output the deferred
            //  dash and the new char.
            //
            if (chCur == kCIDLib::chHyphenMinus)
            {
                eState = EStates::DoubleDash;
            }
             else
            {
                strComment.Append(kCIDLib::chHyphenMinus);
                strComment.Append(chCur);
                eState = EStates::Content;
            }
        }
         else if (eState == EStates::DoubleDash)
        {
            //
            //  We have to get closing bracket here or its an invalid
            //  comment.
            //
            if (chCur != kCIDLib::chGreaterThan)
            {
                xprsOwner().PostXMLError(kXMLErrs::errcXMLE_NotAllowedInComment);
                xemOwner().bSkipPastChar(kCIDLib::chGreaterThan);
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
    if (m_pmxevDTD && xprsOwner().bInfoWanted(tCIDXML::EParseFlags::CommentMask))
        m_pmxevDTD->DocTypeComment(strComment);
}


//
//  This is a recursive method that handles parsing a single level of a
//  CHILDREN content model. When we get here, the opening paren has been seen
//  for the new level. We parse until we find the trailing paren, which will
//  represent either a single element, a sequence, or an alternation. We build
//  a content spec node tree for what we find, and call ourself recursively
//  for any nested levels (weaving those returned trees into our tree.)
//
TXMLCMSpecNode* TDTDValidator::pxcsnParseCMLevel()
{
    TXMLCMSpecNode* pxcsnRet = nullptr;
    TDTDElemDecl*   pxdeclCur = nullptr;

    // Get a buffer to get element names into
    TXMLBufJan  janName(&xbmOwner(), 256);
    TString&    strName = *janName;

    //
    //  Ok, lets enter the loop. Here we can deal with four things. We can
    //  see the terminating ')' character, a '(' character which causes us
    //  to recurse, an element name, or a '|' or ',' character.
    //
    tCIDLib::TCh            chNext = kCIDLib::chNull;
    tCIDXML::ECMNodeTypes   eType = tCIDXML::ECMNodeTypes::Unknown;
    TXMLCMSpecNode*         pxcsnNew = nullptr;
    TXMLCMSpecNode*         pxcsnCur = nullptr;
    while (kCIDLib::True)
    {
        //
        //  If we are on a PE ref, then expand it and jump back to the
        //  top to try again.
        //
        if (bHandleWSOrPERef(tCIDXML::EPERefOpts::InMarkup))
            continue;

        if (xemOwner().bSkippedChar(kCIDLib::chOpenParen))
        {
            //
            //  Build the nested tree and all its nested children. Leave
            //  the root in the new node, which will be woven in below.
            //
            pxcsnNew = pxcsnParseCMLevel();
            if (!pxcsnNew)
            {
                xprsOwner().PostXMLError(kXMLErrs::errcXMLE_BadChildModel);
                delete pxcsnRet;
                return 0;
            }
        }
         else
        {
            //
            //  We must have an element name now, so try to get one and see
            //  if we can. If not, then issue an error, clean up what we've
            //  done so far, and return a failure.
            //
            if (!xemOwner().bGetName(strName))
            {
                xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedElementName);
                delete pxcsnRet;
                return 0;
            }

            //
            //  Look up this element, faulting it in if required. If we fault it in,
            //  mark it as being referenced in a content model.
            //
            pxdeclCur = m_pxnipElems->pobjByName(strName);
            if (!pxdeclCur)
            {
                pxdeclCur = new TDTDElemDecl(strName);
                pxdeclCur->eCreateReason(tCIDXML::EElemReasons::InContentModel);
                m_pxnipElems->c4AddNew(pxdeclCur);
            }

            // Create a leaf node for this guy, to be woven in below
            pxcsnNew = new TXMLCMSpecNode(pxdeclCur->c4Id());
        }

        //
        //  Check for whitespace or a PE Ref. Remember whether we saw any
        //  space since, if the next char is a rep char, its illegal.
        //
        const tCIDLib::TBoolean bSawSpace = bHandleWSOrPERef
        (
            tCIDXML::EPERefOpts::InMarkup
        );

        //
        //  Get the next char, which indicates whether we are done or have
        //  another element in the sequence or alternation.
        //
        chNext = xemOwner().chGetNext();

        // There can be whitespace after this, so it could be a PE ref
        bHandleWSOrPERef(tCIDXML::EPERefOpts::InMarkup);

        //
        //  If we've already figured out the seq/alt type, make sure that
        //  the char is of that type, else set it.
        //
        if ((chNext == kCIDLib::chPlusSign)
        ||  (chNext == kCIDLib::chAsterisk)
        ||  (chNext == kCIDLib::chQuestionMark))
        {
            if (bSawSpace)
                xprsOwner().PostXMLError(kXMLErrs::errcXMLE_UnexpectedWhitespace);

            tCIDXML::ECMNodeTypes eRepType = tCIDXML::ECMNodeTypes::OneOrMore;
            if (chNext == kCIDLib::chPlusSign)
                eRepType = tCIDXML::ECMNodeTypes::OneOrMore;
            else if (chNext == kCIDLib::chAsterisk)
                eRepType = tCIDXML::ECMNodeTypes::ZeroOrMore;
            else if (chNext == kCIDLib::chQuestionMark)
                eRepType = tCIDXML::ECMNodeTypes::ZeroOrOne;
            else
            {
                // <TBD> Do something here in debug mode
            }

            // Wrap the new node inside a rep node
            pxcsnNew = new TXMLCMSpecNode(eRepType, pxcsnNew);

            // And move up to the next char
            chNext = xemOwner().chGetNext();

            // There can be whitespace after this, so it could be a PE ref
            bHandleWSOrPERef(tCIDXML::EPERefOpts::InMarkup);
        }

        // See if we've hit the end yet
        if (chNext == kCIDLib::chCloseParen)
        {
            //
            //  If the return node is null, then we got a single element
            //  or single nested subsection. So make the new node the return
            //  node.
            //
            //  Else, add it as the right child of the last seq/alt node that
            //  we created.
            //
            if (!pxcsnRet)
                pxcsnRet = pxcsnNew;
            else
                pxcsnCur->pxcsnRight(pxcsnNew);

            break;
        }

        if ((chNext != kCIDLib::chComma) && (chNext != kCIDLib::chVerticalBar))
        {
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_BadChildModel);
            delete pxcsnRet;
            return 0;
        }

        if (eType == tCIDXML::ECMNodeTypes::Unknown)
        {
            if (chNext == kCIDLib::chComma)
                eType = tCIDXML::ECMNodeTypes::Sequence;
            else
                eType = tCIDXML::ECMNodeTypes::Alternation;

            pxcsnRet = new TXMLCMSpecNode(eType, pxcsnNew);
            pxcsnCur = pxcsnRet;
        }
         else
        {
            if (((chNext == kCIDLib::chComma) && (eType == tCIDXML::ECMNodeTypes::Alternation))
            ||  ((chNext == kCIDLib::chVerticalBar) && (eType == tCIDXML::ECMNodeTypes::Sequence)))
            {
                xprsOwner().PostXMLError(kXMLErrs::errcXMLE_MixedContentTypes);
                delete pxcsnRet;
                return 0;
            }

            //
            //  Create a new node of the current type. Then set it as the
            //  right node of the current node, then make it the new current
            //  node.
            //
            TXMLCMSpecNode* pxcsnTmp = new TXMLCMSpecNode(eType, pxcsnNew);
            pxcsnCur->pxcsnRight(pxcsnTmp);
            pxcsnCur = pxcsnTmp;
        }
    }

    #if CID_DEBUG_ON
    if (!pxcsnRet)
    {
        // <TBD> log an error here
    }
    #endif

    return pxcsnRet;
}
