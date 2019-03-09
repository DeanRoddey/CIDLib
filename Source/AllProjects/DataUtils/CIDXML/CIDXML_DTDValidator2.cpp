//
// FILE NAME: CIDXML_DTDValidator2.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/30/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the DTD parsing code of the TDTDValidator class.
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
//  This method is called after the type of the attribute has been determined.
//  We we need to handle the required/implied stuff and any default value.
//
tCIDLib::TBoolean TDTDValidator::bParseAttrDefaults(TDTDAttrDef& xadToFill)
{
    //
    //  So lets do the simple ones first. If its required or implied, then
    //  we are done.
    //
    if (xemOwner().bSkippedString(L"#IMPLIED"))
    {
        xadToFill.eDefaultType(tCIDXML::EDefAttrTypes::Implied);
        return kCIDLib::True;
    }

    if (xemOwner().bSkippedString(L"#REQUIRED"))
    {
        xadToFill.eDefaultType(tCIDXML::EDefAttrTypes::Required);
        return kCIDLib::True;
    }

    //
    //  Ok, so now try the fixed string. If not, then its default. In either
    //  case we have to go on and get the default/fixed value.
    //
    if (xemOwner().bSkippedString(L"#FIXED"))
    {
        xadToFill.eDefaultType(tCIDXML::EDefAttrTypes::Fixed);

        // We have to have white space now or a PE ref
        bHandleWSOrPERef(tCIDXML::EPERefOpts::SpaceInMarkup);
    }
     else
    {
        xadToFill.eDefaultType(tCIDXML::EDefAttrTypes::Default);
    }

    //
    //  Now we can call the attribute value parsing method. It will parse
    //  the value and do all the required entity expansion stuff ect...
    //
    return bParseDefAttrValue(xadToFill);
}


//
//  This method is called after the '(' character is seen and the first token
//  after that is not '#PCDATA'. This means that it has to be a regular
//  expression type content mode. This operation requires recursion to handle
//  nested parenthesized clauses, so we call a helper method in DTDValidator3
//  which does the actual recursion and returns back here with a content spec
//  node tree built. We just store it in the passed element.
//
tCIDLib::TBoolean
TDTDValidator::bParseChildModel(        TDTDElemDecl&   xdeclToFill)
{
    // Do the recursive parse of the content model
    TXMLCMSpecNode* pxcsnRoot = pxcsnParseCMLevel();

    //
    //  See if there is a trailing repetition character. But first check
    //  for a PE ref or optional spaces. Remember if we saw spaces because
    //  its really not legal and we have to issue an error.
    //
    const tCIDLib::TBoolean bGotSpaces = bHandleWSOrPERef(tCIDXML::EPERefOpts::InMarkup);

    tCIDLib::TCh chRep = xemOwner().chPeekNext();
    if ((chRep == kCIDLib::chPlusSign)
    ||  (chRep == kCIDLib::chAsterisk)
    ||  (chRep == kCIDLib::chQuestionMark))
    {
        // If we got spaces, issue an error
        if (bGotSpaces)
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_UnexpectedWhitespace);

        // Eat the character
        xemOwner().chGetNext();

        // Convert it to the correct enum type
        tCIDXML::ECMNodeTypes eType = tCIDXML::ECMNodeTypes::OneOrMore;
        if (chRep == kCIDLib::chPlusSign)
            eType = tCIDXML::ECMNodeTypes::OneOrMore;
        else if (chRep == kCIDLib::chAsterisk)
            eType = tCIDXML::ECMNodeTypes::ZeroOrMore;
        else if (chRep == kCIDLib::chQuestionMark)
            eType = tCIDXML::ECMNodeTypes::ZeroOrOne;
        else
        {
            CIDAssert2(L"Unknown DTD repetition node type");
        }

        //
        //  And add a last unary op node, making it the new root and making
        //  the returned root from above its child node.
        //
        pxcsnRoot = new TXMLCMSpecNode(eType, pxcsnRoot);
    }

    //
    //  Create a child content model object, give it the root of the spec
    //  node tree, and then install it on the element decl.
    //
    xdeclToFill.AdoptContentModel(new TXMLChildCM(pxcsnRoot));
    return kCIDLib::True;
}


//
//  This method is called from ParseElement(), after the element name has
//  been parsed and the element decl looked up or created. We are responsible
//  for filling in the content model info in the passed element decl object.
//
tCIDLib::TBoolean TDTDValidator::bParseElemCM(TDTDElemDecl& xdeclToFill)
{
    // Remember the spooler we got the open paren on
    const tCIDLib::TCard4 c4OrgSpoolerId = xemOwner().c4CurSpoolerId();

    // We must have white space and optionally a PE ref here
    bHandleWSOrPERef(tCIDXML::EPERefOpts::SpaceInMarkup);

    // We check for the two simple scenarios first, of EMPTY and ANY
    if (xemOwner().bSkippedString(L"ANY"))
    {
        xdeclToFill.eModel(tCIDXML::EElemModels::Any);
        return kCIDLib::True;
    }

    if (xemOwner().bSkippedString(L"EMPTY"))
    {
        xdeclToFill.eModel(tCIDXML::EElemModels::Empty);
        return kCIDLib::True;
    }

    // Its got to be an open parameter of a content model now
    if (!xemOwner().bSkippedChar(kCIDLib::chOpenParen))
    {
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedContentSpec);
        return kCIDLib::False;
    }

    // We can have space or a PE ref here, but space not required
    bHandleWSOrPERef(tCIDXML::EPERefOpts::InMarkup);

    //
    //  Its a regular expression type content model. However, we can sense
    //  a specific subtype for MIXED model. These will start with #PCDATA.
    //  We can make these much faster by just maintaining a list of element
    //  ids, instead of building a complicated DFA.
    //
    tCIDLib::TBoolean   bResult;
    if (xemOwner().bSkippedString(L"#PCDATA"))
    {
        xdeclToFill.eModel(tCIDXML::EElemModels::Mixed);
        bResult = bParseMixedModel(xdeclToFill);
    }
     else
    {
        //
        //  The child mode pasing method is recursive, so we get a buffer
        //  here and pass it in to be used. This avoids having to get another
        //  one over and over again.
        //
        xdeclToFill.eModel(tCIDXML::EElemModels::Children);
        bResult = bParseChildModel(xdeclToFill);
    }

    // Make sure we got back to the spooler were the open paren was seen
    if (xemOwner().c4CurSpoolerId() != c4OrgSpoolerId)
    {
        bResult = kCIDLib::False;
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_PartialMarkupErr);
    }

    //
    //  Force the model back to 'Any' if we failed to parse the content model
    //  This will let us continue to move forward.
    //
    if (!bResult)
        xdeclToFill.eModel(tCIDXML::EElemModels::Any);

    return bResult;
}


//
//  This method is called after the [ character is seen, and goes to the
//  ending ] character.
//
tCIDLib::TBoolean TDTDValidator::bParseIntSubset()
{
    // Get a buffer to use if they wanted to get whitespace
    TXMLBufJan  janWS(&xbmOwner(), 512);
    TString&    strWS = *janWS;

    //
    //  Go ahead and do the calculation of whether they want to get spaces
    //  from the internal subset, so we don't have to do it over and over
    //  again below.
    //
    const tCIDLib::TBoolean bSendWS = xprsOwner().bInfoWanted
    (
        tCIDXML::EParseFlags::SpaceMask
    ) && m_pmxevDTD;

    //
    //  Ok, we now enter a loop where we look for white space, another markup
    //  decl, a PE ref, or the closing ] character.
    //
    while (kCIDLib::True)
    {
        // See what the next char is
        const tCIDLib::TCh chNext = xemOwner().chPeekNext();

        if (!chNext)
            xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);

        // Break out on the close bracket
        if (chNext == kCIDLib::chCloseBracket)
        {
            // Eat the char before we break out
            xemOwner().chGetNext();
            break;
        }

        //
        //  Watch for a PE entity. Tell it to parse external references. If
        //  not external, then it will push it as a reader. In either case,
        //  we just jump back to the top and start parsing again.
        //
        if (chNext == kCIDLib::chPercentSign)
        {
            // Eat the percent before we call
            xemOwner().chGetNext();
            ExpandPEntityRef(kCIDLib::True, tCIDXML::EPERefOpts::None);
            continue;
        }

        //
        //  If its an open angle bracket, then assume we have a markup decl
        //  and go parse it.
        //
        //  If its whitespace, then either get and send the whitespace or
        //  just skip over it if the client code doesn't want to see it.
        //
        if (chNext == kCIDLib::chLessThan)
        {
            ParseMarkupDecl();
        }
         else if (TXMLCharFlags::bIsSpace(chNext))
        {
            if (bSendWS)
            {
                xemOwner().GetSpaces(strWS);
                m_pmxevDTD->DocTypeWS(strWS);
            }
             else
            {
                xemOwner().bSkippedSpaces(kCIDLib::True);
            }
        }
         else
        {
            //
            //  Its some invalid content for the top level of a DTD. So issue
            //  an error and try to recover.
            //
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_InvalidTopLevelDTD);

            //
            //  See if its a > character. If so, then probably they just
            //  forgot the ] character, so return as though it were
            //  terminated.
            //
            if (chNext == kCIDLib::chGreaterThan)
                return kCIDLib::True;

            //
            //  It wasn't that, so try to sync back up by skipping forward to
            //  the next < character. If we hit the end of input trying to do
            //  that, then throw an exception.
            //
            if (!xemOwner().bSkipPastChar(kCIDLib::chLessThan))
                xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
        }
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TDTDValidator::bParseMixedModel(TDTDElemDecl& xdeclToFill)
{
    TXMLBufJan  janNameBuf(&xbmOwner(), 256);
    TString&    strName = *janNameBuf;

    //
    //  Ok, we enter a loop and search for element names separated by commas.
    //  Each new element name is looked up in (or faulted into) the element
    //  decl pool and its id stored in a vector.
    //
    TFundVector<tCIDLib::TCard4> fcolElems(64);
    while (kCIDLib::True)
    {
        // We can have whitespace or a PE ref here
        bHandleWSOrPERef(tCIDXML::EPERefOpts::InMarkup);

        // Next we have to have a comma or the closing paren
        if (xemOwner().bSkippedChar(kCIDLib::chCloseParen))
            break;
        else if (!xemOwner().bSkippedChar(kCIDLib::chVerticalBar))
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedComma);

        // And again we can have whitespace or a PE Ref
        bHandleWSOrPERef(tCIDXML::EPERefOpts::InMarkup);

        // Next we have to have an element name
        if (!xemOwner().bGetName(strName))
        {
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedElementName);
            if (!xemOwner().bSkipPastChar(kCIDLib::chGreaterThan))
                xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
            return kCIDLib::False;
        }

        //
        //  Look it up, faulting it in if required. If its faulted in, then
        //  mark it as being referenced in a content model.
        //
        TDTDElemDecl* pxdeclCur = m_pxnipElems->pobjByName(strName);
        if (!pxdeclCur)
        {
            pxdeclCur = new TDTDElemDecl(strName);
            pxdeclCur->eCreateReason(tCIDXML::EElemReasons::InContentModel);
            m_pxnipElems->c4AddNew(pxdeclCur);
        }

        // If we are validating, then we can't have dups in this model
        if (tCIDLib::bAllBitsOn(xprsOwner().eOptions(), tCIDXML::EParseOpts::Validate))
        {
            if (fcolElems.bElementPresent(pxdeclCur->c4Id()))
                xprsOwner().PostXMLError(kXMLErrs::errcXMLV_DupInMixedModel);
        }

        // Store the id of this element in the vector
        fcolElems.c4AddElement(pxdeclCur->c4Id());
    }

    //
    //  We can have an optional * repetition character here, with optional
    //  leading whitespace.
    //
    bHandleWSOrPERef(tCIDXML::EPERefOpts::InMarkup);
    xemOwner().bSkippedChar(kCIDLib::chAsterisk);

    //
    //  We made it, so we have to create a mixed content model and store it.
    //  It will copy the vector of ids we have.
    //
    xdeclToFill.AdoptContentModel
    (
        new TXMLMixedCM(fcolElems.ptElements(), fcolElems.c4ElemCount())
    );

    return kCIDLib::True;
}


tCIDLib::TVoid TDTDValidator::ParseAttrList()
{
    // We can have whitespace or a PE ref here
    bHandleWSOrPERef(tCIDXML::EPERefOpts::InMarkup);

    //
    //  We need a name buffer for getting the name of the element and for
    //  getting in the name of the attributes. And we need a buffer for
    //  getting whitespace chars.
    //
    TXMLBufJan  janNameBuf(&xbmOwner(), 512);
    TString&    strName = *janNameBuf;
    TXMLBufJan  janWSBuf(&xbmOwner(), 512);
    TString&    strWS = *janWSBuf;

    //
    //  And now we should see the name of the element to which this
    //  attribute list applies.
    //
    if (!xemOwner().bGetName(strName))
    {
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedElementName);
        xprsOwner().ThrowParseError(kXMLErrs::errcParse_CannotContinue);
    }

    // Look it up and fault it in if its not found
    TDTDElemDecl* pxdeclCur = m_pxnipElems->pobjByName(strName);
    if (!pxdeclCur)
    {
        pxdeclCur = new TDTDElemDecl(strName);
        pxdeclCur->eCreateReason(tCIDXML::EElemReasons::InAttList);
        m_pxnipElems->c4AddNew(pxdeclCur);
    }

    //
    //  If we have a handler and they want this info, then tell it an
    //  attribute list is starting.
    //
    if (m_pmxevDTD && xprsOwner().bInfoWanted(tCIDXML::EParseFlags::MarkupMask))
        m_pmxevDTD->StartAttList(*pxdeclCur);

    //
    //  Set up the flag we want to pass for asking if they want whitespace
    //  here. We have to ask for the right type of whitespace, in the internal
    //  or external subset.
    //
    const tCIDXML::EParseFlags eSpaceReq
                = (xprsOwner().eLocation() == tCIDXML::ELocations::IntSubset)
                ? tCIDXML::EParseFlags::SpaceISS : tCIDXML::EParseFlags::SpaceESS;

    //
    //  Enter a loop where we process attributes until we hit the '>'
    //  character that represents the end of the attribute list.
    //
    while (kCIDLib::True)
    {
        // Peek the next char to know what we need to do
        const tCIDLib::TCh chNext = xemOwner().chPeekNext();

        // If we hit the end, that's not legal so freak out
        if (!chNext)
            xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);

        if (chNext == kCIDLib::chPercentSign)
        {
            // Eat the percent and expand the entity
            xemOwner().chGetNext();
            ExpandPEntityRef(kCIDLib::False, tCIDXML::EPERefOpts::InMarkup);
            continue;
        }

        //
        //  If its space, get it and send if they want to see it, else just
        //  skip it. If its a / or >, then we are done so break out. Else
        //  it must be an attribute so try to process it.
        //
        if (TXMLCharFlags::bIsSpace(chNext))
        {
            if (m_pmxevDTD && xprsOwner().bInfoWanted(eSpaceReq))
            {
                xemOwner().GetSpaces(strWS);
                m_pmxevDTD->DocTypeWS(strWS);
            }
             else
            {
                xemOwner().bSkippedSpaces(kCIDLib::True);
            }
        }
         else if (chNext == kCIDLib::chGreaterThan)
        {
            // Eat the character
            xemOwner().chGetNext();
            break;
        }
         else
        {
            //
            //  Ok, gotta assume its an attribute decl. So lets get the
            //  attribute name and look it up.
            //
            if (!xemOwner().bGetName(strName))
            {
                xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedElementName);
                xprsOwner().ThrowParseError(kXMLErrs::errcParse_CannotContinue);
            }

            //
            //  If it was found, then its already been seen and the old def
            //  wins. So we want to use the dummy def to parse into. Else, we
            //  create one to parse into and add it to the element's pool. We
            //  have to cast the lookup return since it returns as the base
            //  attr def class.
            //
            tCIDLib::TBoolean bIgnored;
            TDTDAttrDef* pxadCur = static_cast<TDTDAttrDef*>
            (
                pxdeclCur->pxadFindAttrDef(strName)
            );

            if (pxadCur)
            {
                if (!m_pxadIgnoredAttrDef)
                    m_pxadIgnoredAttrDef = new TDTDAttrDef(strName);
                else
                    m_pxadIgnoredAttrDef->strName(strName);

                pxadCur = m_pxadIgnoredAttrDef;
                bIgnored = kCIDLib::True;
            }
             else
            {
                pxadCur = new TDTDAttrDef(strName);
                pxdeclCur->AddAttrDef(pxadCur);
                bIgnored = kCIDLib::False;
            }

            // We have to have space or a PE ref here
            bHandleWSOrPERef(tCIDXML::EPERefOpts::SpaceInMarkup);

            //
            //  First we find the type of attribute. Its either one of the
            //  standard key words (CDATA, ID, etc...) or its a list of
            //  enumerated values.
            //
            //  !!!!!The order is important. We can have ID before IDREF
            //       for instance, because it will trigger ID.
            //
            if (xemOwner().bSkippedString(L"CDATA"))
            {
                pxadCur->eType(tCIDXML::EAttrTypes::CData);
            }
             else if (xemOwner().bSkippedString(L"IDREF"))
            {
                if (xemOwner().bSkippedChar(kCIDLib::chLatin_S))
                    pxadCur->eType(tCIDXML::EAttrTypes::IDRefs);
                else
                    pxadCur->eType(tCIDXML::EAttrTypes::IDRef);
            }
             else if (xemOwner().bSkippedString(L"ID"))
            {
                pxadCur->eType(tCIDXML::EAttrTypes::ID);
            }
             else if (xemOwner().bSkippedString(L"ENTITY"))
            {
                pxadCur->eType(tCIDXML::EAttrTypes::Entity);
            }
             else if (xemOwner().bSkippedString(L"ENTITIES"))
            {
                pxadCur->eType(tCIDXML::EAttrTypes::Entities);
            }
             else if (xemOwner().bSkippedString(L"NMTOKEN"))
            {
                if (xemOwner().bSkippedChar(kCIDLib::chLatin_S))
                    pxadCur->eType(tCIDXML::EAttrTypes::NmTokens);
                else
                    pxadCur->eType(tCIDXML::EAttrTypes::NmToken);
            }
             else if (xemOwner().bSkippedString(L"NOTATION"))
            {
                pxadCur->eType(tCIDXML::EAttrTypes::Notation);

                // We have to have space then an opening paren
                bHandleWSOrPERef(tCIDXML::EPERefOpts::SpaceInMarkup);
                if (!xemOwner().bSkippedChar(kCIDLib::chOpenParen))
                {
                    xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedOpenParen);
                    if (!xemOwner().bSkipPastChar(kCIDLib::chGreaterThan))
                        xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
                    break;
                }
            }
             else if (xemOwner().bSkippedChar(kCIDLib::chOpenParen))
            {
                pxadCur->eType(tCIDXML::EAttrTypes::Enumeration);
            }
             else
            {
                //
                //  Cannot be valid, so issue an error and get out. Try to
                //  resync, and throw out if we cannot.
                //
                xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedAttrType);
                if (!xemOwner().bSkipPastChar(kCIDLib::chGreaterThan))
                    xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
                break;
            }

            //
            //  If we got a notation or enumeration, then we have to parse
            //  the list of values.
            //
            if ((pxadCur->eType() == tCIDXML::EAttrTypes::Enumeration)
            ||  (pxadCur->eType() == tCIDXML::EAttrTypes::Notation))
            {
                if (!bParseEnumList(*pxadCur, pxadCur->eType() == tCIDXML::EAttrTypes::Enumeration))
                {
                    if (!xemOwner().bSkipPastChar(kCIDLib::chGreaterThan))
                        xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
                    break;
                }
            }

            // Check for PERef and required WS
            bHandleWSOrPERef(tCIDXML::EPERefOpts::SpaceInMarkup);

            // Ok, lets now call the method that parses the default stuff
            if (!bParseAttrDefaults(*pxadCur))
            {
                if (!xemOwner().bSkipPastChar(kCIDLib::chGreaterThan))
                    xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
                break;
            }

            // If we have a doc type event, then tell it about the attribute
            if (m_pmxevDTD && xprsOwner().bInfoWanted(tCIDXML::EParseFlags::MarkupMask))
                m_pmxevDTD->AttrDef(*pxadCur, bIgnored);
        }
    }

    // If we have a handler, then tell the attribute list is done
    if (m_pmxevDTD && xprsOwner().bInfoWanted(tCIDXML::EParseFlags::MarkupMask))
        m_pmxevDTD->EndAttList(*pxdeclCur);
}


//
//  This method is the top level method for parsing the DTD. Its called after
//  the '<!DOCTYPE' string has been parsed in the main document. On exit from
//  here, if all goes well, we will have parsed the internal and external
//  subsets.
//
//  They can tell us only to parse the doctype and no more. In that case,
//  we disallow any internal subset and ignore any external. This is used
//  when the validator is locked and we just want to eat the DOCTYPE
//  statement so that the parse can continue after it.
//
tCIDLib::TVoid TDTDValidator::ParseDOCType(const tCIDLib::TBoolean bDTOnly)
{
    //
    //  There has to be space after the <!DOCTYPE string. If not, then issue
    //  an error but try to keep going.
    //
    if (!xemOwner().bSkippedSpaces())
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedWhitespace);

    // We need to get a buffer to parse the root element into
    TXMLBufJan  janRootBuf(&xbmOwner(), 128);
    TString&    strRoot = *janRootBuf;

    //
    //  And now try to get the root element name. If we cannot, then we really
    //  are probably out of luck, so throw a parser exception.
    //
    if (!xemOwner().bGetName(strRoot))
    {
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedRootElem);
        xprsOwner().ThrowParseError(kXMLErrs::errcParse_CannotContinue);
    }

    // Make sure its not empty either
    if (strRoot.bIsEmpty())
    {
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedRootElem);
        xprsOwner().ThrowParseError(kXMLErrs::errcParse_CannotContinue);
    }

    //
    //  Lets add this element to the element pool. It will not already be
    //  there since we haven't parsed any DTD info yet. So do an unconditional
    //  add. Mark the element as having been created due to reference as the
    //  root element.
    //
    //  We store the element id as a member so that we can later know which
    //  element is the root element.
    //
    //  Don't do this if only doing the doctype eating type pass, since all
    //  of this info should already be in the validator.
    //
    TDTDElemDecl* pxdeclRoot = 0;
    if (!bDTOnly)
    {
        pxdeclRoot = new TDTDElemDecl(strRoot);
        pxdeclRoot->eCreateReason(tCIDXML::EElemReasons::AsRootElem);
        m_c4RootElemId = m_pxnipElems->c4AddNew(pxdeclRoot);
    }

    // There can to be whitespace here (but not required)
    xemOwner().bSkippedSpaces();

    //
    //  Though its not very useful, you can have a DOCTYPE with no internal or
    //  external subset. So check for it and return if so.
    //
    if (xemOwner().bSkippedChar(kCIDLib::chGreaterThan))
        return;

    //
    //  Ok, we could see an internal or external subset, or both. So we have
    //  flags to indicate which and strings to store the public and system
    //  ids in if its external.
    //
    tCIDLib::TBoolean bExtSubset = kCIDLib::False;
    tCIDLib::TBoolean bIntSubset = kCIDLib::False;

    //
    //  Get some buffers to parse the id(s) into. Get refs to the buffers
    //  for convenience.
    //
    TXMLBufJan  janPubId(&xbmOwner(), 512);
    TString&    strPubId = *janPubId;
    TXMLBufJan  janSysId(&xbmOwner(), 512);
    TString&    strSysId = *janSysId;


    //
    //  If the next char is a [, then its just an internal subset by itself.
    //  Otherwise, its got to be an external subset (with potentially an
    //  internal subset, though we won't know until after we scan the id(s).)
    //
    if (xemOwner().bSkippedChar(kCIDLib::chOpenBracket))
    {
        bIntSubset = kCIDLib::True;
    }
     else
    {
        // Got to have an external subset or its not well formed
        bExtSubset = kCIDLib::True;

        //
        //  Try to parse the external id(s). If it fails, then we basically
        //  have to give up because the likihood of anything at all working
        //  is pretty low.
        //
        if (!bParseIds(strSysId, strPubId, tCIDXML::EIDTypes::External))
            xprsOwner().ThrowParseError(kXMLErrs::errcParse_CannotContinue);

        //
        //  Now we can skip over any spaces and look again for a [ character,
        //  since there can still be an internal subset.
        //
        xemOwner().bSkippedSpaces();
        if (xemOwner().bSkippedChar(kCIDLib::chOpenBracket))
            bIntSubset = kCIDLib::True;

        // Store the ids we got if not doing a dummy DOCtype parse
        if (!bDTOnly)
        {
            m_strPublicId = strPubId;
            m_strSystemId = strSysId;
        }
    }

    //
    //  If we have a handler, then call the doc type event callback, and
    //  we aren't just doing the doctype parsing pass, then call it.
    //
    if (m_pmxevDTD
    &&  xprsOwner().bInfoWanted(tCIDXML::EParseFlags::DOCType)
    &&  !bDTOnly)
    {
        m_pmxevDTD->DocType(*pxdeclRoot, m_strPublicId, m_strSystemId);
    }

    //
    //  If we have an internal subset, then we have to parse it first (the
    //  XML spec requires this since internal stuff overrides external.)
    //
    if (bIntSubset)
    {
        // This the caller doesn't want to allow this, then throw
        if (bDTOnly)
        {
            facCIDXML().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kXMLErrs::errcXMLV_NoIntSSAllowed
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
            );
        }

        //
        //  Remember the current entity id. When we get back here, we should
        //  definitely be in the same one. If not, then some entity was not
        //  correct and 'leaked' out.
        //
        const tCIDLib::TCard4 c4OrgSpooler = xemOwner().c4CurSpoolerId();

        // If we have a handler, then call the start int subset method
        if (m_pmxevDTD && xprsOwner().bInfoWanted(tCIDXML::EParseFlags::Topology))
            m_pmxevDTD->StartIntSubset();

        //
        //  And now go off and parse the internal subset. If it fails, try
        //  to skip up to the terminating angle brack and continue. If we
        //  hit the end while trying to skip, then give up.
        //
        if (!bParseIntSubset())
        {
            if (!xemOwner().bSkipPastChar(kCIDLib::chGreaterThan))
                xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
            return;
        }

        // If we have a handler, then call the end int subset method
        if (m_pmxevDTD && xprsOwner().bInfoWanted(tCIDXML::EParseFlags::Topology))
            m_pmxevDTD->EndIntSubset();

        //
        //  See if we got back to the same entity. If not, then issue an
        //  error and flush the entity stack back to the original entity we
        //  started on.
        //
        if (c4OrgSpooler !=  xemOwner().c4CurSpoolerId())
        {
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_PartialMarkupErr);

            // <TBD> Flush the entity stack
        }

        //
        //  If we were asked to ignore the DTD, then we can now flush back
        //  out all the stuff we just parsed.
        //
        if (m_bIgnoreDTD)
            Reset();
    }

    //
    //  We should now be at the closing angle bracket of the internal subset.
    //  If not, then issue an error, but try to keep going.
    //
    if (!xemOwner().bSkippedChar(kCIDLib::chGreaterThan))
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_DOCTYPENotEnded);

    //
    //  If we have a handler, then call the end doc type event callback.
    //  Don't call it if doing the DOCTYpe parsing pass.
    //
    if (m_pmxevDTD
    &&  xprsOwner().bInfoWanted(tCIDXML::EParseFlags::Topology)
    &&  !bDTOnly)
    {
        m_pmxevDTD->EndDocType();
    }

    //
    //  And if we had an external subset, we can now parse that. unless
    //  asked not to. We may be ignoring DTDs in general or be using a
    //  locked validator and only want to parse the DOCType statement
    //  and nothing else.
    //
    if (bExtSubset && !m_bIgnoreDTD && !bDTOnly)
    {
        // Set the location to external subset
        xprsOwner().eLocation(tCIDXML::ELocations::ExtSubset);

        //
        //  If we have an entity resolver, then let it take a shot at doing
        //  the entity resolution. Else, do it ourself.
        //
        tCIDXML::TEntitySrcRef esrExtSS;
        if (xprsOwner().pmxevEntityResolver())
        {
            //
            //  Tell the resolover this is the external subset entity. This
            //  one doesn't have an enity name, since its really not an
            //  entity reference, so pass an empty string.
            //
            esrExtSS = xprsOwner().pmxevEntityResolver()->esrResolve
            (
                m_strPublicId
                , m_strSystemId
                , TString::strEmpty()
                , xemOwner().strLastExtId()
                , tCIDXML::EResolveTypes::ExternalSubset
            );
        }

        // If not resolved, try to do it ourself
        if (!esrExtSS)
        {
            //
            //  Assume its a local file path. If the new file is not fully
            //  qualified, then weave it onto the base of the parent entity.
            //  Else, take it as it is.
            //
            TPathStr pathNew(m_strSystemId);

            if (pathNew.bIsFullyQualified())
            {
                // If fully qualified, so take it as is
                esrExtSS.SetPointer(new TFileEntitySrc(pathNew));
            }
             else
            {
                //
                //  Remove the last level, which is the previous entity's
                //  actual name, and use that as the base path.
                //
                TPathStr pathParent(xemOwner().strLastExtId());
                pathParent.bRemoveLevel();
                esrExtSS.SetPointer
                (
                    new TFileEntitySrc(pathParent, m_strSystemId)
                );
            }
        }

        //
        //  And create an entity spooler for it and push it. This one has
        //  no entity declaration, so pass a zero for that.
        //
        TXMLEntSpooler* pxesExtSS = xemOwner().pxesMakeNew
        (
            esrExtSS
            , kCIDLib::True
            , tCIDXML::ERefFrom::OutsideLiteral
            , tCIDXML::EEntityTypes::Neither
        );

        // Push it on the entity stack
        if (xemOwner().bPushEntity(pxesExtSS, 0, xprsOwner()))
        {
            // If we have a DTD handler, then send a start event
            if (m_pmxevDTD && xprsOwner().bInfoWanted(tCIDXML::EParseFlags::Topology))
                m_pmxevDTD->StartExtSubset();

            //
            //  And parse the external entity. Tell it that it can legally
            //  see a TextDecl.
            //
            ParseExtEntity(kCIDLib::True);

            // If we have a DTD handler, then send an end event
            if (m_pmxevDTD && xprsOwner().bInfoWanted(tCIDXML::EParseFlags::Topology))
                m_pmxevDTD->EndExtSubset();
        }
         else
        {
            // <TBD>
        }
    }
}


//
//  This method is called after the '<!ELEMENT' string is seen. We pick it up
//  from there and parse it and the content model information. Because of the
//  content model parsing variations, this one calls a number of different
//  helper methods.
//
tCIDLib::TVoid TDTDValidator::ParseElement()
{
    // We have to have whitespace here
    bHandleWSOrPERef(tCIDXML::EPERefOpts::SpaceInMarkup);

    // Get a buffer to use for the element name
    TXMLBufJan  janName(&xbmOwner(), 128);
    TString&    strName = *janName;

    if (!xemOwner().bGetName(strName))
    {
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedElementName);
        if (!xemOwner().bSkipPastChar(kCIDLib::chGreaterThan))
            xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
        return;
    }

    //
    //  Lets see if this element already exists in the element pool. If so,
    //  then this is a redundant declaration and we'll use a dummy object
    //  to parse into.
    //
    TDTDElemDecl* pxdeclTarget = m_pxnipElems->pobjByName(strName);

    //
    //  If its already declared, then lets use the 'ignored' object that we
    //  keep around for this purpose. Else, we have to create a new one to
    //  fill in.
    //
    tCIDLib::TBoolean bIsIgnored = kCIDLib::False;
    if (pxdeclTarget)
    {
        //
        //  If it its create reason is 'declared', then we ignore this
        //  declaration and issue an error. Though some decls can be multiply
        //  defined (and the first wins), elements cannot. Otherwise, it was just
        //  referenced somewhere.
        //
        if (pxdeclTarget->eCreateReason() == tCIDXML::EElemReasons::Declared)
        {
            // If validating, then its an error
            if (tCIDLib::bAllBitsOn(xprsOwner().eOptions(), tCIDXML::EParseOpts::Validate))
            {
                xprsOwner().PostXMLError
                (
                    kXMLErrs::errcXMLV_ElemRedecl
                    , pxdeclTarget->strFullName()
                );
            }

            //
            //  If we haven't create the dummy one, create it. Else, just set
            //  the name to the name we just got.
            //
            if (!m_pxdeclIgnoredElement)
                m_pxdeclIgnoredElement = new TDTDElemDecl(strName);
            else
                m_pxdeclIgnoredElement->strName(strName);

            bIsIgnored = kCIDLib::True;
            pxdeclTarget = m_pxdeclIgnoredElement;
        }
    }
     else
    {
        // It does not already exist, so lets create it and add it
        pxdeclTarget = new TDTDElemDecl(strName);
        m_pxnipElems->c4AddNew(pxdeclTarget);
    }

    // Mark it as having been declared now
    pxdeclTarget->eCreateReason(tCIDXML::EElemReasons::Declared);

    //
    //  Ok, now to keep this from turning into the method from hell, we call
    //  out to a helper mether that does the content model scan. Its a good
    //  bit of stuff.
    //
    if (!bParseElemCM(*pxdeclTarget))
    {
        // Try to resync and return now
        if (!xemOwner().bSkipPastChar(kCIDLib::chGreaterThan))
            xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
        return;
    }

    //
    //  Now we have to set the text type. This is something that normally might
    //  be set in the element decl itself. But, it requires some info that he
    //  doesn't have access to. Only do it if we are not ignoring this one.
    //
    if (!bIsIgnored)
    {
        switch (pxdeclTarget->eModel())
        {
            case tCIDXML::EElemModels::Children :
                //
                //  If this is a standalone document, and this is an external
                //  decl and we are validating, then it cannot have any optional
                //  whitespace.
                //
                if (xprsOwner().bStandalone()
                && (xprsOwner().eLocation() == tCIDXML::ELocations::ExtSubset)
                && tCIDLib::bAllBitsOn(xprsOwner().eOptions(), tCIDXML::EParseOpts::Validate))
                {
                    pxdeclTarget->eTextType(tCIDXML::EElemTextTypes::None);
                }
                 else
                {
                    pxdeclTarget->eTextType(tCIDXML::EElemTextTypes::SpaceIgnorable);
                }
                break;


            case tCIDXML::EElemModels::Mixed :
            case tCIDXML::EElemModels::Any :
                pxdeclTarget->eTextType(tCIDXML::EElemTextTypes::Any);
                break;

            case tCIDXML::EElemModels::Empty :
                pxdeclTarget->eTextType(tCIDXML::EElemTextTypes::None);
                break;

            default :
                // If debugging, then freak out here
                #if CID_DEBUG_ON
                facCIDXML().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kXMLErrs::errcCM_UnknownCMType
                    , tCIDLib::ESeverities::ProcFatal
                    , tCIDLib::EErrClasses::Internal
                    , TInteger(tCIDLib::i4EnumOrd(pxdeclTarget->eModel()))
                );
                #endif
                break;
        }
    }

    // We can have white space or optionally a PE ref here
    bHandleWSOrPERef(tCIDXML::EPERefOpts::InMarkup);

    // Now we must have the closing angle bracket
    if (!xemOwner().bSkippedChar(kCIDLib::chGreaterThan))
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_UnterminatedMarkup);

    //
    //  Ok, if we have a DTD handler installed, lets call out with this
    //  markup information.
    //
    if (m_pmxevDTD && xprsOwner().bInfoWanted(tCIDXML::EParseFlags::MarkupMask))
        m_pmxevDTD->ElementDecl(*pxdeclTarget, bIsIgnored);
}


//
//  This method is called after the '<!ENTITY' string is seen. We pick it up
//  from there to see if its a PE or general entity, then grab the rest of
//  the info and store it away.
//
tCIDLib::TVoid TDTDValidator::ParseEntity()
{
    // We have to have whitespace here
    if (!xemOwner().bSkippedSpaces())
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedWhitespace);

    //
    //  If we are now looking at a % character, then its a PE and we have to
    //  have following space (with a possible PE ref), else its a general
    //  entity and we have to be looking at the entity name.
    //
    const tCIDLib::TBoolean bIsPE = xemOwner().bSkippedChar(kCIDLib::chPercentSign);
    if (bIsPE)
        bHandleWSOrPERef(tCIDXML::EPERefOpts::SpaceInMarkup);

    // Get a buffer to use for the entity name
    TXMLBufJan  janName(&xbmOwner(), 128);
    TString&    strName = *janName;

    if (!xemOwner().bGetName(strName))
    {
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedEntityName);
        if (!xemOwner().bSkipPastChar(kCIDLib::chGreaterThan))
            xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
        return;
    }

    //
    //  Lets see if this entity already exists in the appropriate pool. If
    //  so, then this is a redundant declaration and we'll use a dummy object
    //  to parse into.
    //
    TDTDEntityDecl* pxdeclTarget = 0;
    if (bIsPE)
        pxdeclTarget = m_pxnipPEs->pobjByName(strName);
    else
        pxdeclTarget = m_pxnipEntities->pobjByName(strName);

    //
    //  If its already declared, then lets use the 'ignored' object that we
    //  keep around for this purpose. Else, we have to create a new one to
    //  fill in.
    //
    tCIDLib::TBoolean bIsIgnored = kCIDLib::False;
    if (pxdeclTarget)
    {
        //
        //  If we haven't create the dummy one, create it. Else, just set
        //  the name to the name we just got.
        //
        if (!m_pxdeclIgnoredEntity)
            m_pxdeclIgnoredEntity = new TDTDEntityDecl(strName);
        else
            m_pxdeclIgnoredEntity->strName(strName);

        // Remember that it is an ignored one
        bIsIgnored = kCIDLib::True;

        pxdeclTarget = m_pxdeclIgnoredEntity;
    }
     else
    {
        // It does not already exist, so lets create it and add it
        pxdeclTarget = new TDTDEntityDecl(strName);

        if (bIsPE)
            m_pxnipPEs->c4AddNew(pxdeclTarget);
        else
            m_pxnipEntities->c4AddNew(pxdeclTarget);
    }

    // Tell it if its a PE or not
    pxdeclTarget->bIsPE(bIsPE);

    // We must have white space and optionally a PE ref here
    bHandleWSOrPERef(tCIDXML::EPERefOpts::SpaceInMarkup);

    //
    //  Alrighty, lets go on now and parse the rest of the decl. Its either
    //  going to be a literal value or the id for some external entity.
    //
    tCIDLib::TCh chQuote;
    if (xemOwner().bSkippedQuote(chQuote))
    {
        //
        //  Its a literal value. So call the entity literally parsing method.
        //  It has a good number of rules, so we do it separately. We have to
        //  get a temp buffer to use for this.
        //
        TXMLBufJan janLiteral(&xbmOwner(), 1024);
        if (bParseEntityValue(*janLiteral, chQuote))
            pxdeclTarget->strValue(*janLiteral);
    }
     else
    {
        //
        //  Its got to be an external entity at this point, so lets parse
        //  the ids. If this fails, its wrong.
        //
        TXMLBufJan  janPublic(&xbmOwner(), 256);
        TString&    strPublic = *janPublic;
        TXMLBufJan  janSystem(&xbmOwner(), 256);
        TString&    strSystem = *janSystem;

        if (!bParseIds(strSystem, strPublic, tCIDXML::EIDTypes::External))
            return;

        // We got them so put them into the entity object
        pxdeclTarget->strSystemId(strSystem);
        pxdeclTarget->strPublicId(strPublic);

        // We can have optional white space or a PE ref here
        const tCIDLib::TBoolean bGotSpace = bHandleWSOrPERef
        (
            tCIDXML::EPERefOpts::InMarkup
        );

        //
        //  We could end here, so check for the closing angle. If so, go
        //  ahead and send the decl event if they want it.
        //
        if (xemOwner().bSkippedChar(kCIDLib::chGreaterThan))
        {
            if (m_pmxevDTD && xprsOwner().bInfoWanted(tCIDXML::EParseFlags::MarkupMask))
                m_pmxevDTD->EntityDecl(*pxdeclTarget, bIsIgnored);
            return;
        }

        //
        //  If it did not end, then we had to have gotten at least one space
        //  above in the previous PERef/Space check.
        //
        if (!bGotSpace)
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedWhitespace);

        //
        //  The only valid thing at this point is that it must be an NDATA
        //  type entity.
        //
        if (!xemOwner().bSkippedString(L"NDATA"))
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedNDATA);

        // We must have whitespace and optionally a PE Ref here
        bHandleWSOrPERef(tCIDXML::EPERefOpts::SpaceInMarkup);

        // If this is a PE, then this is not legal
        if (bIsPE)
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_PEHadNDATA);

        //
        //  Now there must be a name, which is the notation name. If we get
        //  it, then store it in the entity. Else issue an error.
        //
        TXMLBufJan janNotationName(&xbmOwner(), 128);
        if (xemOwner().bGetName(*janNotationName))
            pxdeclTarget->strNotationName(*janNotationName);
        else
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedNotName);
    }

    // We can have optional whitespace or a PE here
    bHandleWSOrPERef(tCIDXML::EPERefOpts::InMarkup);

    // And we have to see a closing angle bracket
    if (!xemOwner().bSkippedChar(kCIDLib::chGreaterThan))
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_UnterminatedMarkup);

    //
    //  Ok, if we have a DTD handler installed, lets call out with this
    //  markup information.
    //
    if (m_pmxevDTD && xprsOwner().bInfoWanted(tCIDXML::EParseFlags::MarkupMask))
        m_pmxevDTD->EntityDecl(*pxdeclTarget, bIsIgnored);
}


tCIDLib::TVoid
TDTDValidator::ParseExtEntity(const tCIDLib::TBoolean bExpectDecl)
{
    // Remember the entity that we are in now
    const tCIDLib::TCard4 c4OrgSpooler = xemOwner().c4CurSpoolerId();

    //
    //  Go ahead and do the calculation of whether they want to get spaces
    //  from the external subset, so we don't have to do it over and over
    //  again below.
    //
    const tCIDLib::TBoolean bSendWS = xprsOwner().bInfoWanted
    (
        tCIDXML::EParseFlags::SpaceMask
    ) && m_pmxevDTD;

    // Get a buffer to use for gathering whitespace
    TXMLBufJan  janWS(&xbmOwner(), 256);
    TString&    strWS = *janWS;

    //
    //  Look for a TextDecl. If it exists, it must be here at the start and
    //  we simplify the logic below by handling it up front.
    //
    if (bExpectDecl)
    {
        tCIDLib::TBoolean bDeclFound = xemOwner().bSkippedString(L"<?xml ");
        if (!bDeclFound)
        {
            // Watch for a decl with incorrect case, just in case... so to speak
            bDeclFound = xemOwner().bSkippedString(L"<?XML ");

            if (bDeclFound)
                xprsOwner().PostXMLError(kXMLErrs::errcXMLE_BadDeclCase);
        }

        if (bDeclFound)
            ParseTextDecl();
    }

    //
    //  We have to loop here as long as we have data. We deal with whitespace
    //  data and markup decls.
    //
    while (kCIDLib::True)
    {
        // Peek at the next character to see what we are looking at
        const tCIDLib::TCh chNext = xemOwner().chPeekNext();

        // If we've dropped out of the original entity, we are done
        if (xemOwner().c4CurSpoolerId() < c4OrgSpooler)
            break;

        //
        //  See if its whitespace first. If so, then gather it up and send
        //  it out. If its a < char, then gotta be markup so go scan the
        //  markup. We also deal with PE references and ends of ignore
        //  sections if we are in one.
        //
        if (TXMLCharFlags::bIsSpace(chNext))
        {
            if (bSendWS)
            {
                //
                //  Enable end of entity exceptions so that we won't cross
                //  over an entity. We have to separate the whitespace info
                //  by entity.
                //
                TXMLEMThrowJan janThrow(&xemOwner(), kCIDLib::True);

                //
                //  Now get the spaces and send it. If an exception occurs,
                //  send what we got and then
                try
                {
                    xemOwner().GetSpaces(strWS);
                    m_pmxevDTD->DocTypeWS(strWS);
                }

                catch(const TXMLEndOfEntity&)
                {
                    //
                    //  Send what we got, if any, and then go around again to
                    //  see what we get next.
                    //
                    if (!strWS.bIsEmpty())
                        m_pmxevDTD->DocTypeWS(strWS);
                }
            }
             else
            {
                // We don't tell it it can over entity boundaries here!!
                xemOwner().bSkippedSpaces();
            }
        }
         else if (chNext == kCIDLib::chLessThan)
        {
            ParseMarkupDecl();
        }
         else if (chNext == kCIDLib::chPercentSign)
        {
            xemOwner().chGetNext();
            ExpandPEntityRef(kCIDLib::True, tCIDXML::EPERefOpts::None);
            continue;
        }
         else if (!bExpectDecl & (chNext == kCIDLib::chCloseBracket))
        {
            // Eat the close bracket
            xemOwner().chGetNext();
            if (!xemOwner().bSkippedChar(kCIDLib::chCloseBracket))
            {
                xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedEOC);
                if (!xemOwner().bSkipPastChar(kCIDLib::chGreaterThan))
                    xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
            }
             else if (!xemOwner().bSkippedChar(kCIDLib::chGreaterThan))
            {
                xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedEOC);
                if (!xemOwner().bSkipPastChar(kCIDLib::chGreaterThan))
                    xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
            }
            break;
        }
         else
        {
            //
            //  Its some invalid content for the top level of a DTD. So
            //  issue an error and try to recover.
            //
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_InvalidTopLevelDTD);

            //
            //  See if its a > character. If so, then probably they just
            //  forgot the ] character, so return as though it were terminated.
            //  Else, just try to sync back up.
            //
            if (chNext != kCIDLib::chGreaterThan)
            {
                if (!xemOwner().bSkipPastChar(kCIDLib::chLessThan))
                    xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
            }
        }
    }
}


//
//  When we get here, we've seen a <!IGNORE[ string. We have to parse until we
//  hit the end of this ignore section, handling any nested ones we might see
//  along the way. Everything else can be ignored.
//
tCIDLib::TVoid TDTDValidator::ParseIgnoredSection()
{
    tCIDLib::TCard4 c4NestCount = 1;

    while (kCIDLib::True)
    {
        const tCIDLib::TCh chNext = xemOwner().chGetNext();

        // We should never get an end of input here
        if (!chNext)
            xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);

        if (chNext == kCIDLib::chCloseBracket)
        {
            //
            //  Watch for the closing sequence. Could be the end of a set of
            //  nested sequences.
            //
            if (xemOwner().bSkippedChar(kCIDLib::chCloseBracket))
            {
                // Skip any subsequence close brackets
                while (xemOwner().bSkippedChar(kCIDLib::chCloseBracket))
                {
                    // Just skip them
                }

                // If we ended on a greater than, then we exited a level
                if (xemOwner().bSkippedChar(kCIDLib::chGreaterThan))
                {
                    c4NestCount--;

                    // Break out if we have backed all the way out
                    if (!c4NestCount)
                        break;
                }
            }
        }
         else if (chNext == kCIDLib::chLessThan)
        {
            // If we see <![ then bump up the nesting count
            if (xemOwner().bSkippedChar(kCIDLib::chExclamation)
            &&  xemOwner().bSkippedChar(kCIDLib::chOpenBracket))
            {
                c4NestCount++;
            }
        }
         else if (!TXMLCharFlags::bIsXMLChar(chNext))
        {
            // It wasn't event a legal XML char
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_NonXMLCharacter);
        }
    }
}


tCIDLib::TVoid TDTDValidator::ParseMarkupDecl()
{
    // Make sure we start with a < character
    if (!xemOwner().bSkippedChar(kCIDLib::chLessThan))
        xprsOwner().ThrowParseError(kXMLErrs::errcParse_MarkupStartsWith);

    //
    //  Get the current entity id. We use this to watch for partial
    //  markup errors after parsing the markup.
    //
    const tCIDLib::TCard4 c4SpoolerId = xemOwner().c4CurSpoolerId();

    //
    //  Lets look at each possible type of markup (or it could be a
    //  comment or PI) that it could be and call the appropriate
    //  parsing method.
    //
    if (xemOwner().bSkippedChar(kCIDLib::chExclamation))
    {
        if (xemOwner().bSkippedChar(kCIDLib::chOpenBracket))
        {
            //
            //  Its got to be an include or ignore section or its invalid. We
            //  can have whitespace before the keyword, so check that first.
            //
            bHandleWSOrPERef(tCIDXML::EPERefOpts::InMarkup);

            if (xemOwner().bSkippedString(L"INCLUDE"))
            {
                // If we are not in the external subset, then this is not legal
                if (xprsOwner().eLocation() != tCIDXML::ELocations::ExtSubset)
                    xprsOwner().ThrowParseError(kXMLErrs::errcXMLE_ConditionalInISS);

                //
                //  Check for optional space, then the second open bracet, then
                //  more optional space.
                //
                bHandleWSOrPERef(tCIDXML::EPERefOpts::InMarkup);
                if (!xemOwner().bSkippedChar(kCIDLib::chOpenBracket))
                    xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedOpenBracket);
                bHandleWSOrPERef(tCIDXML::EPERefOpts::InMarkup);

                //
                //  And now we just recurse on the external subset parsing method,
                //  to handle any info in the included section (and correctly
                //  handle the closing of it.)
                //
                ParseExtEntity(kCIDLib::False);
            }
             else if (xemOwner().bSkippedString(L"IGNORE"))
            {
                // If we are not in the external subset, then this is not legal
                if (xprsOwner().eLocation() != tCIDXML::ELocations::ExtSubset)
                    xprsOwner().ThrowParseError(kXMLErrs::errcXMLE_ConditionalInISS);

                //
                //  Check for optional space, then the second open bracet, then
                //  more optional space.
                //
                bHandleWSOrPERef(tCIDXML::EPERefOpts::InMarkup);
                if (!xemOwner().bSkippedChar(kCIDLib::chOpenBracket))
                    xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedOpenBracket);
                bHandleWSOrPERef(tCIDXML::EPERefOpts::InMarkup);

                //
                //  Call the ignore section parser, which can just ignore until
                //  it falls out of any nested ignore sections.
                //
                ParseIgnoredSection();
            }
             else
            {
                xprsOwner().ThrowParseError(kXMLErrs::errcXMLE_ExpectedIncludeIgnore);
            }
        }
         else if (xemOwner().bSkippedString(L"--"))
        {
            ParseComment();
        }
         else if (xemOwner().bSkippedString(L"ELEMENT"))
        {
            ParseElement();
        }
         else if (xemOwner().bSkippedString(L"ATTLIST"))
        {
            ParseAttrList();
        }
         else if (xemOwner().bSkippedString(L"ENTITY"))
        {
            ParseEntity();
        }
         else if (xemOwner().bSkippedString(L"NOTATION"))
        {
            ParseNotation();
        }
         else
        {
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_InvalidTopLevelDTD);
            if (!xemOwner().bSkipPastChar(kCIDLib::chGreaterThan))
                xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
        }
    }
     else if (xemOwner().bSkippedChar(kCIDLib::chQuestionMark))
    {
        //
        //  Its got to be a PI since we'll never see a TextDecl here. We always
        //  handle it specially up front. So check for the decl string and
        //  issue an error if found.
        //
        if (xemOwner().bSkippedString(L"xml "))
        {
            if ((xemOwner().c4CurLine() == 1) && (xemOwner().c4CurColumn() == 1))
            {
                xprsOwner().PostXMLError(kXMLErrs::errcXMLE_NoXMLDeclHere);
                if (!xemOwner().bSkipPastChar(kCIDLib::chGreaterThan))
                    xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
            }
        }

        // It could be a legal PI so parse it
        ParsePI();
    }
     else
    {
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_InvalidTopLevelDTD);
        if (!xemOwner().bSkipPastChar(kCIDLib::chGreaterThan))
            xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
    }

    // Make sure we got back to the original entity
    if (c4SpoolerId != xemOwner().c4CurSpoolerId())
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_PartialMarkupErr);
}


tCIDLib::TVoid TDTDValidator::ParseNotation()
{
    // We have to have whitespace here
    bHandleWSOrPERef(tCIDXML::EPERefOpts::SpaceInMarkup);

    //
    //  First we have to have the notation name. So lets get a buffer to
    //  get the name into and pull it in.
    //
    TXMLBufJan  janName(&xbmOwner(), 128);
    TString&    strName = *janName;

    if (!xemOwner().bGetName(strName))
    {
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedNotName);
        if (!xemOwner().bSkipPastChar(kCIDLib::chGreaterThan))
            xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
        return;
    }

    //
    //  Look this puppy up in the notation decl pool. If its already there
    //  then we parse into a dummy decl. Else we create one and parse into
    //  it.
    //
    TDTDNotationDecl* pxdeclTarget = m_pxnipNotations->pobjByName(strName);

    //
    //  If its already declared, then lets use the 'ignored' object that we
    //  keep around for this purpose. Else, we have to create a new one to
    //  fill in.
    //
    tCIDLib::TBoolean bIsIgnored = kCIDLib::False;
    if (pxdeclTarget)
    {
        //
        //  If we haven't create the dummy one, create it. Else, just set
        //  the name to the name we just got.
        //
        if (!m_pxdeclIgnoredNotation)
            m_pxdeclIgnoredNotation = new TDTDNotationDecl(strName);
        else
            m_pxdeclIgnoredNotation->strName(strName);

        // Remember that it is an ignored one
        bIsIgnored = kCIDLib::True;

        pxdeclTarget = m_pxdeclIgnoredNotation;
    }
     else
    {
        // It does not already exist, so lets create it and add it
        pxdeclTarget = new TDTDNotationDecl(strName);
        m_pxnipNotations->c4AddNew(pxdeclTarget);
    }

    // We can have optional whitespace or a PE here
    bHandleWSOrPERef(tCIDXML::EPERefOpts::InMarkup);

    //
    //  Now we can have either an external or public id, so parse the id(s)
    //  into a couple of temp buffers.
    //
    TXMLBufJan  janPubId(&xbmOwner(), 256);
    TString&    strPubId = *janPubId;
    TXMLBufJan  janSysId(&xbmOwner(), 256);
    TString&    strSysId = *janSysId;

    if (!bParseIds(strSysId, strPubId, tCIDXML::EIDTypes::Either))
        xprsOwner().ThrowParseError(kXMLErrs::errcParse_CannotContinue);

    // We got one or two ids, so store them
    pxdeclTarget->strPublicId(strPubId);
    pxdeclTarget->strSystemId(strSysId);

    // We can have optional whitespace or a PE here
    bHandleWSOrPERef(tCIDXML::EPERefOpts::InMarkup);

    // And we have to see a closing angle bracket
    if (!xemOwner().bSkippedChar(kCIDLib::chGreaterThan))
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_UnterminatedMarkup);

    //
    //  And if we have a DTD handler, call it with this notation decl
    //  event.
    //
    if (m_pmxevDTD && xprsOwner().bInfoWanted(tCIDXML::EParseFlags::MarkupMask))
        m_pmxevDTD->NotationDecl(*pxdeclTarget, bIsIgnored);
}


tCIDLib::TVoid TDTDValidator::ParsePI()
{
    // Get a buffer to parse the target into
    TXMLBufJan  janTarget(&xbmOwner(), 128);
    TString&    strTarget = *janTarget;

    //
    //  There aren't supposed to be any spaces before the target. Check for
    //  them and issue an error if found, then skip over them and keep going
    //
    if (xemOwner().bSkippedSpaces())
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_NoSpaceBeforePITarget);

    // Now try to get the PI target name
    if (!xemOwner().bGetNameToken(strTarget))
    {
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedPITarget);
        if (!xemOwner().bSkipPastChar(kCIDLib::chGreaterThan))
            xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
        return;
    }

    // Now get a buffer to get the value, if any, into
    TXMLBufJan  janValue(&xbmOwner(), 512);
    TString&    strValue = *janValue;

    //
    //  Now we do something different according to whether we get any spaces
    //  or not. If not, then we assume it has no value.
    //
    if (xemOwner().bSkippedSpaces(kCIDLib::True))
    {
        //
        //  Lets collect characters until we hit the end of the PI or the
        //  end of input.
        //
        while (kCIDLib::True)
        {
            const tCIDLib::TCh chCur = xemOwner().chGetNext();

            if (!chCur)
            {
                xprsOwner().PostXMLError(kXMLErrs::errcXMLE_UnterminatedPI);
                return;
            }

            if (chCur == kCIDLib::chQuestionMark)
            {
                if (xemOwner().bSkippedChar(kCIDLib::chGreaterThan))
                    break;
            }

            // Check for invalid chars
            if (!TXMLCharFlags::bIsXMLChar(chCur))
                xprsOwner().PostXMLError(kXMLErrs::errcXMLE_NonXMLCharacter);

            strValue.Append(chCur);
        }
    }
     else
    {
        // Make sure its terminated ok
        if (!xemOwner().bSkippedChar(kCIDLib::chQuestionMark))
        {
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_UnterminatedPI);
            if (!xemOwner().bSkipPastChar(kCIDLib::chGreaterThan))
                xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
            return;
        }

        if (!xemOwner().bSkippedChar(kCIDLib::chGreaterThan))
        {
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_UnterminatedPI);
            if (!xemOwner().bSkipPastChar(kCIDLib::chGreaterThan))
                xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
            return;
        }
    }

    // If the client code wants this info, then pass it on
    if (m_pmxevDTD && xprsOwner().bInfoWanted(tCIDXML::EParseFlags::PIMask))
        m_pmxevDTD->DocTypePI(strTarget, strValue, xprsOwner().eLocation());
}


tCIDLib::TVoid TDTDValidator::ParseTextDecl()
{
    // Now get a buffer to get the value, if any, into
    TXMLBufJan  janVersion(&xbmOwner(), 64);
    TString&    strVersion = *janVersion;

    TXMLBufJan  janEncoding(&xbmOwner(), 64);
    TString&    strEncoding = *janEncoding;

    // Skip any spaces
    xemOwner().bSkippedSpaces();

    //
    //  We don't have to have a version string here, but it can be. So check
    //  for it and handle it if there.
    //
    if (xemOwner().bSkippedString(L"version"))
    {
        // Next we have an eq sign (with possible whitespace around it)
        if (!xemOwner().bParseEqualSign())
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedEqualSign);

        // Get the value string
        if (!xemOwner().bGetQuotedString(strVersion))
        {
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedQuotedString);
            if (!xemOwner().bSkipPastChar(kCIDLib::chGreaterThan))
                xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
            return;
        }
    }

    // Skip more possible spaces
    xemOwner().bSkippedSpaces();

    if (xemOwner().bSkippedString(L"encoding"))
    {
        // Next we have an eq sign (with possible whitespace around it)
        if (!xemOwner().bParseEqualSign())
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedEqualSign);

        // Get the value string
        if (!xemOwner().bGetQuotedString(strEncoding))
        {
            xprsOwner().PostXMLError(kXMLErrs::errcXMLE_ExpectedQuotedString);
            if (!xemOwner().bSkipPastChar(kCIDLib::chGreaterThan))
                xprsOwner().ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
            return;
        }
    }
     else
    {
        // The encoding string is required in a text decl
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_EncodingRequired);
    }

    // Skip more possible spaces
    xemOwner().bSkippedSpaces();

    // Now we have to have the '?>' termination of the decl
    if (!xemOwner().bSkippedChar(kCIDLib::chQuestionMark))
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_UnterminatedDecl);

    if (!xemOwner().bSkippedChar(kCIDLib::chGreaterThan))
        xprsOwner().PostXMLError(kXMLErrs::errcXMLE_UnterminatedDecl);

    // If we have a DTD handler, tell him about it
    if (m_pmxevDTD && xprsOwner().bInfoWanted(tCIDXML::EParseFlags::TextDecl))
        m_pmxevDTD->TextDecl(strVersion, strEncoding);

    //
    //  Tell the current entity spooler about the encoding so that he can update
    //  his text converter if needed.
    //
    if (!strEncoding.bIsEmpty())
        xemOwner().SetDeclEncoding(strEncoding);
}
