//
// FILE NAME: CIDXML_EntityMgr.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/21/1999
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
//  This file implements the TXMLEntityMgr class.
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
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TXMLEntityMgr,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TXMLEntityMgr
// PREFIX: xem
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLEntityMgr: Constructors and Destructor
// ---------------------------------------------------------------------------
TXMLEntityMgr::TXMLEntityMgr() :

    m_bExhausted(kCIDLib::False)

    , m_bThrowAtEnd(kCIDLib::False)
    , m_c4StackTop(0)
    , m_pcolEntityStack(0)
    , m_pmxevEntityEvents(0)
    , m_pxdeclCurrent(0)
    , m_pxesCurrent(0)
{
    //
    //  Create the entity stack. For the average file, 32 nested entitied deep
    //  is more than enough so that it will never have to be reallocated.
    //
    //  The stack elements just reference the entity spoolers, so we aren't
    //  allocating 32 big buffers here, just making space to store the element
    //  objects.
    //
    m_pcolEntityStack = new TVector<TEMStackElem>(32);
}

TXMLEntityMgr::~TXMLEntityMgr()
{
    // Delete the current entity spooler
    delete m_pxesCurrent;

    //
    //  Reset the entity stack to clean up any spoolers left on it. Then
    //  delete the vector collection itself.
    //
    Reset();
    delete m_pcolEntityStack;
}


// ---------------------------------------------------------------------------
//  TXMLEntityMgr: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TXMLEntityMgr::bAtRootEntity() const
{
    // If no elems on stack, then we are at the root level
    return (m_c4StackTop == 0);
}


tCIDLib::TBoolean TXMLEntityMgr::bGetName(TString& strToFill)
{
    return m_pxesCurrent->bGetName(strToFill, kCIDLib::False);
}


tCIDLib::TBoolean TXMLEntityMgr::bGetNameToken(TString& strToFill)
{
    return m_pxesCurrent->bGetName(strToFill, kCIDLib::True);
}


tCIDLib::TBoolean TXMLEntityMgr::bGetQuotedString(TString& strToFill)
{
    //
    //  Peek at the next character. It should be either a double or single
    //  quote. If not, fail now.
    //
    const tCIDLib::TCh chQuote = chPeekNext();
    if ((chQuote != kCIDLib::chQuotation) && (chQuote != kCIDLib::chApostrophe))
        return kCIDLib::False;

    //
    //  Eat the quote char, clear the target buffer, and enter the loop. We
    //  loop until we hit the matching close quote or end of the original
    //  file.
    //
    chGetNext();
    strToFill.Clear();
    while (kCIDLib::True)
    {
        const tCIDLib::TCh chCur = chGetNext();

        // If we hit the end of file, then definitely it failed
        if (!chCur)
            return kCIDLib::False;

        // If its the close quote, we are done
        if (chCur == chQuote)
            break;

        // Assume its quoted content and save it
        strToFill.Append(chCur);
    }
    return kCIDLib::True;
}


//
//  Sets the ignore bad chars flag. This controls how we set up the
//  text converter in newly created spoolers.
//
tCIDLib::TBoolean
TXMLEntityMgr::bIgnoreBadChars(const tCIDLib::TBoolean bToSet)
{
    m_bIgnoreBadChars = bToSet;
    return m_bIgnoreBadChars;
}


// Gets the next char, whcih must be an equal sign
tCIDLib::TBoolean TXMLEntityMgr::bParseEqualSign()
{
    bSkippedSpaces();
    if (bSkippedChar(kCIDLib::chEquals))
    {
        bSkippedSpaces();
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// Peek forward some number of characters if we can
tCIDLib::TBoolean
TXMLEntityMgr::bPeekAhead(          tCIDLib::TCh&   chToFill
                            , const tCIDLib::TCard4 c4AheadBy)
{
    // Just pass it on to the current spooler
    return m_pxesCurrent->bPeekAhead(chToFill, c4AheadBy);
}


// Push a new entity on the stack
tCIDLib::TBoolean
TXMLEntityMgr::bPushEntity(         TXMLEntSpooler* const   pxesToAdd
                            , const TXMLEntityDecl* const   pxdeclToAdd
                            ,       TXMLParserCore&         xprsOwner)
{
    //
    //  Check for recursion on the stack. We just see if we already have this
    //  entity on the stack.
    //
    if (pxdeclToAdd)
    {
        const TString& strToCheck = pxdeclToAdd->strName();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4StackTop; c4Index++)
        {
            const TEMStackElem& curElem = m_pcolEntityStack->objAt(c4Index);

            // If this one has an entity declaration, check it
            if (curElem.pxdeclThis)
            {
                if (curElem.pxdeclThis->strName() == strToCheck)
                {
                    xprsOwner.PostXMLError
                    (
                        kXMLErrs::errcXMLE_RecursiveEntityRef
                        , strToCheck
                    );
                    return kCIDLib::False;
                }
            }
        }
    }

    //
    //  If we got an Entity decl, then set the spooler's interned flag to
    //  indicate whether the entity is internalized already or not. Else
    //  just assume its not.
    //
    if (pxdeclToAdd)
        pxesToAdd->bInterned(pxdeclToAdd->bIsInternal());
    else
        pxesToAdd->bInterned(kCIDLib::False);

    //
    //  If there is a current active spooler, then push the current spooler
    //  and entity decl onto the stack.
    //
    if (m_pxesCurrent)
    {
        // Add a new empty element if needed
        if (m_c4StackTop == m_pcolEntityStack->c4ElemCount())
            m_pcolEntityStack->objAdd(TEMStackElem());

        // Now get a ref to the top element and set it
        TEMStackElem& elemTop = m_pcolEntityStack->objAt(m_c4StackTop);
        elemTop.pxesThis = m_pxesCurrent;
        elemTop.pxdeclThis = m_pxdeclCurrent;

        // Now bump the stack top
        m_c4StackTop++;
    }

    // And make the new ones the current ones
    m_pxesCurrent = pxesToAdd;
    m_pxdeclCurrent = pxdeclToAdd;

    // If we have an entity event handler and an entity decl, send a start entity
    if (m_pmxevEntityEvents && pxdeclToAdd)
        m_pmxevEntityEvents->StartEntity(*pxdeclToAdd);

    // It went ok, amazingly enough
    return kCIDLib::True;
}


// Skip the next character which must be the indicated character
tCIDLib::TBoolean
TXMLEntityMgr::bSkipPastChar(const tCIDLib::TCh chToSkipPast)
{
    // Loop until end of input or we skip the char
    while (kCIDLib::True)
    {
        const tCIDLib::TCh chCur = chGetNext();
        if (!chCur)
            return kCIDLib::False;

        if (chCur == chToSkipPast)
            break;
    }
    return kCIDLib::True;
}


//
//  Return true if the next character was the indicated character, and
//  skip it. Else, do nothing and return false.
//
tCIDLib::TBoolean
TXMLEntityMgr::bSkippedChar(const tCIDLib::TCh chToSkip)
{
    // Just pass through to the current entity spooler
    return m_pxesCurrent->bSkippedChar(chToSkip);
}


//
//  Skip the next char if a quote and return true, else do nothing and return
//  false.
//
tCIDLib::TBoolean TXMLEntityMgr::bSkippedQuote(tCIDLib::TCh& chSkipped)
{
    return m_pxesCurrent->bSkippedQuote(chSkipped);
}


//
//  Skip the next char if whitespace and return true, else do nothing and
//  return false.
//
tCIDLib::TBoolean TXMLEntityMgr::bSkippedSpace()
{
    // Just pass through to the current entity spooler
    return m_pxesCurrent->bSkippedSpace();
}


//
//  Skip any whitespace and return true to indicate if we did or not.
//  The caller can tell us if we should stop at the end of the current
//  entity or not.
//
tCIDLib::TBoolean
TXMLEntityMgr::bSkippedSpaces(const tCIDLib::TBoolean bCrossEntities)
{
    //
    //  Ask the current entity to skip chars until it hits a non-space
    //  or the end of its data. If it returns success, then it hit a non
    //  space. Else, it just hit the end and we have to pop.
    //
    tCIDLib::TBoolean bSkippedSome = kCIDLib::False;
    if (!m_pxesCurrent->bSkipSpaces(bSkippedSome))
    {
        bPopEntity();

        //
        //  If they don't mind us crossing entity boundaries to skip spaces,
        //  then lets keep going.
        //
        if (bCrossEntities)
        {
            while (kCIDLib::True)
            {
                if (m_pxesCurrent->bSkipSpaces(bSkippedSome))
                    break;

                if (bPopEntity())
                    break;
            }
        }
    }
    return bSkippedSome;
}


//
//  Skip the passed string of chars if they are next in the stream. Else
//  do nothing. The return indicates whether we did or not.
//
tCIDLib::TBoolean
TXMLEntityMgr::bSkippedString(const tCIDLib::TCh* const pszToSkip)
{
    // Just pass through to the current entity spooler
    return m_pxesCurrent->bSkippedString(pszToSkip);
}

tCIDLib::TBoolean TXMLEntityMgr::bSkippedString(const TString& strToSkip)
{
    // Just pass through to the current entity spooler
    return m_pxesCurrent->bSkippedString(strToSkip);
}


// Peek the next available character
tCIDLib::TCh TXMLEntityMgr::chPeekNext()
{
    // Try to peek another char from the current entity spooler
    tCIDLib::TCh chRet;
    if (m_pxesCurrent->bPeekNext(chRet))
        return chRet;

    //
    //  Didn't work, so lets pop it and try the next spooler. If this fails
    //  then we are done, i.e. at the of the original entity.
    //
    if (!bPopEntity())
        return kCIDLib::chNull;

    // And now do it again with the new current entity
    m_pxesCurrent->bPeekNext(chRet);
    return chRet;
}


//
//  Return the current column in the top-most entity manager. If we are
//  at the end, then return max card.
//
tCIDLib::TCard4 TXMLEntityMgr::c4CurColumn() const
{
    // Just pass through to the current entity spooler
    if (!m_pxesCurrent)
        return kCIDLib::c4MaxCard;
    return m_pxesCurrent->c4CurColumn();
}


//
//  Return the current line in the top-most entity manager. If we are
//  at the end, then return max card.
//
tCIDLib::TCard4 TXMLEntityMgr::c4CurLine() const
{
    // Just pass through to the current entity spooler
    if (!m_pxesCurrent)
        return kCIDLib::c4MaxCard;
    return m_pxesCurrent->c4CurLine();
}


// Return the id of the current top-most entity spooler
tCIDLib::TCard4 TXMLEntityMgr::c4CurSpoolerId() const
{
    // Just pass through to the current entity spooler
    return m_pxesCurrent->c4SpoolerId();
}


// Destructively get the next available character
tCIDLib::TCh TXMLEntityMgr::chGetNext()
{
    // Try to get another char from the current entity spooler
    tCIDLib::TCh chRet;
    if (m_pxesCurrent->bGetNext(chRet))
        return chRet;

    // Didn't work, so lets pop it and try the next spooler.
    if (!bPopEntity())
        return kCIDLib::chNull;

    // And now do it again with the new current entity
    m_pxesCurrent->bGetNext(chRet);
    return chRet;
}


// Get the next char if not the indicated char
tCIDLib::TCh TXMLEntityMgr::chGetNextIfNot(const tCIDLib::TCh chNotToGet)
{
    // Try to get another char from the current entity spooler
    tCIDLib::TCh chRet;
    if (m_pxesCurrent->bGetNextIfNot(chRet, chNotToGet))
        return chRet;

    // Didn't work, so lets pop it and try the next spooler.
    if (!bPopEntity())
        return kCIDLib::chNull;

    // And now do it again with the new current entity
    m_pxesCurrent->bGetNextIfNot(chRet, chNotToGet);
    return chRet;
}


// Get any white space at the current parsing position
tCIDLib::TVoid TXMLEntityMgr::GetSpaces(TString& strToFill)
{
    // Clear out the buffer first
    strToFill.Clear();

    //
    //  Get the spaces from the current spooler. If it came back because
    //  of a non-space, then we are done.
    //
    if (!m_pxesCurrent->bGetSpaces(strToFill))
    {
        //
        //  We hit the end of that one, so lets pop that entity off the
        //  stack. We then break out because we don't get whitespace
        //  across entities contiguously.
        //
        bPopEntity();
    }
}


MXMLEntityEvents* TXMLEntityMgr::pmxevEntityEvents() const
{
    return m_pmxevEntityEvents;
}

MXMLEntityEvents*
TXMLEntityMgr::pmxevEntityEvents(MXMLEntityEvents* const pmxevToSet)
{
    m_pmxevEntityEvents = pmxevToSet;
    return m_pmxevEntityEvents;
}


//
//  Make a new entity spooler for the passed entity source ref. We have
//  one for regular entity sources and one for internal entities, which
//  are always already internalized strings. The external ones can be
//  any kind of source.
//
TXMLEntSpooler*
TXMLEntityMgr::pxesMakeNew(         tCIDXML::TEntitySrcRef& esrFrom
                            , const tCIDLib::TBoolean       bExpectDecl
                            , const tCIDXML::ERefFrom       eFrom
                            , const tCIDXML::EEntityTypes   eType)
{
    // <TBD> What is the bExpectedDecl parm for? We aren't using it

    // Ask the entity source to create an input stream
    TBinInStream* pstrmSrc = esrFrom->pstrmMakeNew();

    //
    //  Ok, lets create the entity spooler and return it. We pass along to it
    //  the system id (for reporting purposes), the stream to get the source
    //  data from, and the optional forced encoding from the entity source.
    //
    return new TXMLEntSpooler
    (
        esrFrom->strSystemId()
        , pstrmSrc
        , esrFrom->strEncoding()
        , eFrom
        , eType
        , m_bIgnoreBadChars
    );
}


TXMLEntSpooler*
TXMLEntityMgr::pxesMakeNewFromInt(  const   TXMLEntityDecl&         xdeclSource
                                    , const tCIDXML::ERefFrom       eFrom
                                    , const tCIDXML::EEntityTypes   eType)
{
    //
    //  We have to create a type of stream here which is not usually done
    //  so there is not a prefab class to put it together. We need to stream
    //  binary data out of a string object.
    //
    //  So we have to create a string stream input impl object first, then
    //  create a binary input stream that uses it as its input source. We
    //  don't want it to adopt the string, just reference it.
    //
    TStringInStreamImpl* pstrmiSrc = new TStringInStreamImpl
    (
        &xdeclSource.strValue()
        , tCIDLib::EAdoptOpts::NoAdopt
    );
    TBinInStream* pstrmSrc = new TBinInStream(pstrmiSrc);

    //
    //  And now lets create the spooler. In this constructor we get to pass
    //  in our own text converter. So we create a UTF converter and indicate
    //  the source is the default UTF16 format. We make the system id the
    //  name of the source entity, so that there will be something meaningful
    //  displayed in the case of errors.
    //
    TXMLEntSpooler* pxesRet = new TXMLEntSpooler
    (
        xdeclSource.strName()
        , pstrmSrc
        , new TUTFConverter(TUTFConverter::EEncodings::Def16)
        , eFrom
        , eType
        , m_bIgnoreBadChars
    );

    //
    //  Tell it that its already internalized. This will tell it that it does
    //  not have to do normalization since it was already done when the
    //  entity value was scanned.
    //
    pxesRet->bInterned(kCIDLib::True);

    return pxesRet;
}


// Used to reset the manager before a new parsing round
tCIDLib::TVoid TXMLEntityMgr::Reset()
{
    // Reset our flag members
    m_bExhausted = kCIDLib::False;
    m_bThrowAtEnd = kCIDLib::False;

    // Delete the current spooler and clear out both current pointers
    delete m_pxesCurrent;
    m_pxesCurrent = 0;
    m_pxdeclCurrent = 0;

    //
    //  Run through the stack and clean up any spoolers that were left on
    //  the stack, then reset the stack top
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4StackTop; c4Index++)
    {
        TEMStackElem& elemCur = m_pcolEntityStack->objAt(c4Index);
        delete elemCur.pxesThis;
        elemCur.pxesThis = 0;
        elemCur.pxdeclThis = 0;
    }
    m_c4StackTop = 0;
}


// Return the system id for the top-most entity spooler
const TString& TXMLEntityMgr::strCurSystemId() const
{
    return m_pxesCurrent->strSystemId();
}


//
//  Return the last external system id. This lets us provide a useful
//  error message that indicates the real entity that we are currently
//  parsing.
//
const TString& TXMLEntityMgr::strLastExtId() const
{
    //
    //  If the current entity spooler is spooling an external entity, then
    //  this is our guy.
    //
    if (m_pxdeclCurrent)
    {
        if (m_pxdeclCurrent->bIsExternal())
            return m_pxesCurrent->strSystemId();
    }
     else
    {
        return m_pxesCurrent->strSystemId();
    }

    //
    //  Oh well, we have search back through the spooler stack until we find
    //  one that is an external entity.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4StackTop; c4Index++)
    {
        const TEMStackElem& curElem = m_pcolEntityStack->objAt(c4Index);

        // If this one has an entity declaration, check it
        if (curElem.pxdeclThis)
        {
            // This is our guy
            if (curElem.pxdeclThis->bIsExternal())
                return curElem.pxesThis->strSystemId();
        }
         else
        {
            return curElem.pxesThis->strSystemId();
        }
    }

    //
    //  We must get back to some external entity eventually, since the root
    //  entity is always one. So this is some kind of horrible internal error.
    //
    #if CID_DEBUG_ON
    facCIDXML().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kXMLErrs::errcEnt_NoExtEntityFound
        , tCIDLib::ESeverities::ProcFatal
        , tCIDLib::EErrClasses::Internal
    );
    #endif

    // Make the compiler happy but we'll never get here
    return m_pxesCurrent->strSystemId();
}


// Set the text encoding we should expect
tCIDLib::TVoid TXMLEntityMgr::SetDeclEncoding(const TString& strDeclEncoding)
{
    m_pxesCurrent->SetDeclEncoding(strDeclEncoding);
}


// ---------------------------------------------------------------------------
//  TXMLEntityMgr: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  When we hit the end of the current entity, this is called to pop us back
//  to the next available one. This may not be the previous one since the
//  entity we are in may have been the last thing in the containing entity.
//
tCIDLib::TBoolean TXMLEntityMgr::bPopEntity()
{
    //
    //  If the exhausted flag is set, then we already know that we are done
    //  and there is no more data to return. The fact that we got called here
    //  means that someone isn't checking the fact that the end has been
    //  reached.
    //
    if (m_bExhausted)
        return kCIDLib::False;

    // If the stack is empty, then set the exhausted flag and return false
    if (!m_c4StackTop)
    {
        m_bExhausted = kCIDLib::True;
        return kCIDLib::False;
    }

    //
    //  Remember the entity that we are about to pop off, in case we have to
    //  throw below. And remember the spooler id of the spooler we are going
    //  to destroy, since we have to send that along.
    //
    const TXMLEntityDecl* pxdeclOld = m_pxdeclCurrent;
    tCIDLib::TCard4 c4OldId = m_pxesCurrent->c4SpoolerId();

    //
    //  Ok, lets clean up the current spooler (the entity decl is not ours,
    //  we just ref it), and pop the previous ones off the stack and store
    //  them.
    //
    delete m_pxesCurrent;
    m_c4StackTop--;
    TEMStackElem& elemTop = m_pcolEntityStack->objAt(m_c4StackTop);
    m_pxesCurrent   = elemTop.pxesThis;
    m_pxdeclCurrent = elemTop.pxdeclThis;
    elemTop.pxesThis = 0;
    elemTop.pxdeclThis = 0;

    //
    //  Send an end of entity event to the installed entity event handler,
    //  if there is one.
    //
    if (m_pmxevEntityEvents && pxdeclOld)
        m_pmxevEntityEvents->EndEntity(*pxdeclOld);

    //
    //  If there was an entity previously, then we might have to throw an
    //  end of entity exception.
    //
    if (m_bThrowAtEnd && (pxdeclOld != 0))
        throw TXMLEndOfEntity(*pxdeclOld, c4OldId);

    //
    //  Ok, lets work our way down the entity stack to find the previous
    //  entity spooler with some data left to spool. Normally it will always
    //  be the previous one already popped. But, if there was an entity ref
    //  at the very end of an entity, one could be empty already.
    //
    while (kCIDLib::True)
    {
        //
        //  Ask the current spooler if it has more data available right now,
        //  without having to check to see if it can load more. If so, then
        //  we are happy right now.
        //
        if (!m_pxesCurrent->bCharBufEmpty())
            break;

        //
        //  Oh well, then lets ask it to refresh its char buffer to see if
        //  we can get some more data from it. We are a friend, so its ok for
        //  us to call this private method. If we get some, we are done.
        //
        if (m_pxesCurrent->bReloadCharBuf())
            break;

        //
        //  That puppy wasn't interesting, so lets pop the stack again. If its
        //  empty, then we are exhausted and can give up.
        //
        if (!m_c4StackTop)
        {
            m_bExhausted = kCIDLib::True;
            return kCIDLib::False;
        }

        //
        //  Ok, lets clean up the current spooler (the entity decl is not
        //  ours, we just ref it), and pop the previous ones off the stack
        //  and store them.
        //
        delete m_pxesCurrent;
        m_c4StackTop--;
        TEMStackElem& elemTop = m_pcolEntityStack->objAt(m_c4StackTop);
        m_pxesCurrent   = elemTop.pxesThis;
        m_pxdeclCurrent = elemTop.pxdeclThis;
        elemTop.pxesThis = 0;
        elemTop.pxdeclThis = 0;
    }
    return kCIDLib::True;
}
