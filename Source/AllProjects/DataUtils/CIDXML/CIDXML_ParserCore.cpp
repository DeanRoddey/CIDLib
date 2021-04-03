//
// FILE NAME: CIDXML_ParserCore.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/20/1999
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
//  This file implements the TXMLParserCore class.
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
RTTIDecls(TXMLParserCore,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TXMLParserCore
// PREFIX: xed
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLParserCore: Constructors and Destructor
// ---------------------------------------------------------------------------
TXMLParserCore::TXMLParserCore() :

    m_bInException(kCIDLib::False)
    , m_bStandalone(kCIDLib::False)
    , m_bValidatorLocked(kCIDLib::False)
    , m_c4ErrorCount(0)
    , m_c4ErrorMax(1)
    , m_eFlags(tCIDXML::EParseFlags::None)
    , m_eOptions(tCIDXML::EParseOpts::None)
    , m_eLocation(tCIDXML::ELocations::BeforeContent)
    , m_pcolAttrList(nullptr)
    , m_pmxevDocEvents(nullptr)
    , m_pmxevEntityEvents(nullptr)
    , m_pmxevEntityResolver(nullptr)
    , m_pmxevErrEvents(nullptr)
    , m_pxbmBuf(nullptr)
    , m_pxvalValidator(nullptr)
    , m_strChars(kCIDLib::c4Sz_32K)
{
    // Create the buffer manager pool and the attribute vector
    m_pcolAttrList = new TVector<TXMLAttr>(64);
    m_pxbmBuf = new TXMLBufMgr(L"XML Parser Pool", 4096, 256);
}


TXMLParserCore::~TXMLParserCore()
{
    // Clear up the allocated stuff that we own
    try
    {
        delete m_pcolAttrList;
        delete m_pxbmBuf;
        delete m_pxvalValidator;
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }
}


// ---------------------------------------------------------------------------
//  TXMLParserCore: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  We can be given a default external subset to use if none is found literally.
//  We just set it on an entity source reference object who will keep up with
//  it and clean it up when it's time. If we already have one, this will drop
//  this reference to the old one and if it's no longer used, it will be freed.
//
#pragma warning(suppress : 26461) // Can't make the entity source const
tCIDLib::TVoid
TXMLParserCore::AdoptDefExtSS(          TXMLEntitySrc* const    pxesDefExtSS
                                , const TString&                strDefRootElem)
{
    m_esrDefExtSS.SetPointer(pxesDefExtSS);
    m_strDefRootElem = strDefRootElem;
}


tCIDLib::TBoolean
TXMLParserCore::bInfoWanted(const tCIDXML::EParseFlags eMask) const
{
    //
    //  Location is a different types, but it shares the same bit layout, so
    //  we can cast it and then use a standard helper.
    //
    const tCIDXML::EParseFlags eRes = tCIDLib::eANDEnumBits
    (
        tCIDXML::EParseFlags(m_eLocation), m_eFlags, eMask
    );

    // If any bits left, then return true
    return (eRes != tCIDXML::EParseFlags::None);
}



// Get the current line from the current topmost entity
tCIDLib::TCard4 TXMLParserCore::c4CurLine() const
{
    return m_xemThis.c4CurLine();
}


//
//  This is the entry point for a parsing pass. We get the entity source for
//  the main entity and flags and options, and don't come back till it all
//  parses ok or we fail.
//
tCIDLib::TVoid
TXMLParserCore::ParseRootEntity(        tCIDXML::TEntitySrcRef& esrRoot
                                , const tCIDXML::EParseOpts     eOpts
                                , const tCIDXML::EParseFlags    eFlags)
{
    // Make sure that we got a validator set before we go any further
    if (!m_pxvalValidator)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcGen_MustHaveValidator
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }

    // Store the new flags and options
    m_eFlags = eFlags;
    m_eOptions = eOpts;

    // Do a reset to clear up any previous data
    Reset();

    //
    //  And now lets reset our own data structures to get them ready for
    //  the next round. Note that the entity manager is flushed at the end
    //  because we have to insure that opened sources get closed.
    //
    m_c4ErrorCount = 0;

    // Insure that the entity manager gets flushed when we get out of here
    TXMLEMStackJan janEM(&m_xemThis);

    //
    //  Tell the entity manager how to react to bad characters during
    //  internalization of text.
    //
    m_xemThis.bIgnoreBadChars
    (
        tCIDLib::bAnyBitsOn(m_eOptions, tCIDXML::EParseOpts::IgnoreBadChars)
    );

    //
    //  Now lets try to create a spooler for the root entity. We tell it
    //  that it should expect an XMLDecl, that is not an entity referenced
    //  from within a literal, and that its neither a general nor parameter
    //  entity.
    //
    //  We do this outside of the try block below because, the error system
    //  invoked by the catches in the next try block require that there be an
    //  enity on the entity spooler stack to get line/col info from, but we
    //  can't push one until we create this first one. We will call the error
    //  handler directly ourselves in this case.
    //
    TXMLEntSpooler* pxesRoot = nullptr;
    try
    {
        pxesRoot = m_xemThis.pxesMakeNew
        (
            esrRoot
            , kCIDLib::True
            , tCIDXML::ERefFrom::OutsideLiteral
            , tCIDXML::EEntityTypes::Neither
        );
    }

    catch(TError& errToCatch)
    {
        // If there is a an error handler, then call it and return
        if (m_pmxevErrEvents)
        {
            m_pmxevErrEvents->HandleXMLError
            (
                errToCatch.errcId()
                , tCIDXML::EErrTypes::Error
                , errToCatch.strErrText()
                , 0
                , 0
                , esrRoot->strSystemId()
            );
            return;
        }

        // No error handler, so rethrow
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    try
    {
        //
        //  Push it. This one cannot fail because its impossible for the first
        //  entity to be recursive.
        //
        m_xemThis.bPushEntity(pxesRoot, 0, *this);

        //
        //  If the entity was empty, then lets call the end document event now
        //  and return. Note that is illegal to have an empty main entity, so
        //  issue an error.
        //
        if (pxesRoot->bEmpty())
        {
            PostXMLError(kXMLErrs::errcXMLE_EmptyMainEntity);

            if (m_pmxevDocEvents && bInfoWanted(tCIDXML::EParseFlags::Topology))
                m_pmxevDocEvents->EndDocument(*esrRoot);
            return;
        }

        //
        //  Ok, lets call the start document callback to let the derived class
        //  know that a new document is being started.
        //
        if (m_pmxevDocEvents && bInfoWanted(tCIDXML::EParseFlags::Topology))
            m_pmxevDocEvents->StartDocument(*esrRoot);

        //
        //  Alrighty, lets actually try to parse some input. We start by doing
        //  the pre-content stuff (XMLDecl, comments, whitespace, PIs, and
        //  external/internal DTD subset that are before the root element.
        //
        ParsePreContent();

        //
        //  Ok, update our location and now go off and parse the main XML
        //  content. Tell it we are not in an external entity (the main entity
        //  is one technically, but not from our parsing point of view.)
        //
        m_eLocation = tCIDXML::ELocations::InContent;
        ParseContent(kCIDLib::False);

        // Now set the location to miscellaneous and do the rest
        m_eLocation = tCIDXML::ELocations::AfterContent;

        // And parse that stuff
        ParsePostContent();
    }

    catch(const TXMLException& xexcToCatch)
    {
        m_bInException = kCIDLib::True;
        PostXMLError(kXMLErrs::errcXMLE_ParseException, xexcToCatch);
    }

    catch(const TError& errToCatch)
    {
        //
        //  If its the error thrown because the max errors has been reached.
        //  then don't issue that one.
        //
        if (!errToCatch.bCheckEvent(facCIDXML().strName(), kXMLErrs::errcParse_MaxErrsReached))
        {
            m_bInException = kCIDLib::True;
            PostXMLError(kXMLErrs::errcXMLE_CIDLibException, errToCatch.strErrText());
        }
    }

    catch(...)
    {
        m_bInException = kCIDLib::True;
        PostXMLError(kXMLErrs::errcXMLE_UnknownException);
        throw;
    }

    // Call the end document event callback
    if (m_pmxevDocEvents && bInfoWanted(tCIDXML::EParseFlags::Topology))
        m_pmxevDocEvents->EndDocument(*esrRoot);
}


tCIDLib::TVoid
TXMLParserCore::ParseRootEntity(const   TString&                strRootEntity
                                , const tCIDXML::EParseOpts     eOpts
                                , const tCIDXML::EParseFlags    eFlags)
{
    //
    //  The passed value must either be a file name or a fully qualified
    //  URL. So we first attempt to parse it as a URL. If it throws an
    //  exception that indicates its not a valid URL, then we create it as
    //  a file input source. If it is a valid URL, then we do a URL input
    //  source.
    //
    TXMLEntitySrc* pxsrcRoot = nullptr;

    // Assume its a local file path
    TString strTmp;
    TFileSys::QueryFullPath(strRootEntity, strTmp);
    pxsrcRoot = new TFileEntitySrc(strTmp);

    // And call the other version with our temp entity source
    tCIDXML::TEntitySrcRef esrRoot(pxsrcRoot);
    ParseRootEntity(esrRoot, eOpts, eFlags);
}


tCIDLib::TVoid
TXMLParserCore::ParseRootEntity(const   TString&                strSrcName
                                , const TMemBuf&                mbufRootEntity
                                , const tCIDLib::TCard4         c4Bytes
                                , const tCIDXML::EParseOpts     eOpts
                                , const tCIDXML::EParseFlags    eFlags)
{
    // Just set up a memory buffer entity source and call the main version
    tCIDXML::TEntitySrcRef esrSrc
    (
        new TMemBufEntitySrc(strSrcName, mbufRootEntity, c4Bytes)
    );
    ParseRootEntity(esrSrc, eOpts, eFlags);
}


//
//  We clean up data that is no longer needed after the parse is done, so
//  as not to eat up memory when the parser is not in use.
//
tCIDLib::TVoid TXMLParserCore::Reset()
{
    if (m_pmxevDocEvents)
        m_pmxevDocEvents->ResetDocument();
    if (m_pmxevErrEvents)
        m_pmxevErrEvents->ResetErrors();
    if (m_pmxevEntityEvents)
        m_pmxevEntityEvents->ResetEntities();
    if (m_pmxevEntityResolver)
        m_pmxevEntityResolver->ResetResolver();

    //
    //  Reset the validator to get it ready for the next round. We always
    //  have a validator, so no need to check that. It was confirmed above
    //  already. If the validator is locked, don't do this.
    //
    if (m_pxvalValidator && !m_bValidatorLocked)
        m_pxvalValidator->Reset();

    //
    //  Reset the context stack in case we bailed out on an error last time
    //  and didn't get all the way back out to the 0th nesting level.
    //
    m_xcsElems.Reset();

    // Make sure all buffer pool objects get released
    if (m_pxbmBuf)
        m_pxbmBuf->ReleaseAll();

    // Flush the entity manager
    m_xemThis.Reset();
}


// ---------------------------------------------------------------------------
//  TXMLParserCore: Private, non-virtual methods
// ---------------------------------------------------------------------------
//
//  This method is called to parse character data within content. Since this
//  method can be called gazzillions of times in a large document, it uses one
//  of the special fixed text buffers to accumulate chars (to avoid getting
//  one from the pool over and over again.)
//
//  In order to correctly support DOM style interfaces, which need to know
//  what makes up an entity, when we see a general entity ref, we send all
//  currently collected chars, then scan the entity reference, which might
//  get us out of here if it contains markup. It also enabled throwing at the
//  end of entities, in order to see the end of all entities, so that it can
//  indicate to client code when entities have ended.
//
//  Within character data in markup, the CDATA termination sequence "]]>' is
//  not legal, so we have to watch for it in order to be compliant.
//
//  Because we can get end of entity exceptions here, we use a double loop
//  with the inner one within a try/catch block. This avoids entering and
//  exiting the try/catch block on every round.
//
tCIDLib::TVoid TXMLParserCore::ParseChars()
{
    //
    //  In order to catch the illegal ]]> sequence mentioned in the comments
    //  above, we use a little state machine.
    //
    enum class EStates
    {
        Content
        , FirstBracket
        , SecondBracket
    };


    // Make sure the buffer is flushed on entry to the loop
    m_strChars.Clear();

    //
    //  We want the entity manager to throw end of entity exceptions while
    //  we are in here, so that we can break up text by entity.
    //
    TXMLEMThrowJan janEM(&m_xemThis, kCIDLib::True);

    //
    //  Set up some flags we need in order to maintain state within the
    //  loop below.
    //
    tCIDLib::TBoolean   bDone           = kCIDLib::False;
    tCIDLib::TBoolean   bEscaped        = kCIDLib::False;
    tCIDLib::TBoolean   bLeadingFlag    = kCIDLib::False;
    tCIDLib::TCh        chNext          = kCIDLib::chNull;
    tCIDLib::TCh        chSecond        = kCIDLib::chNull;
    EStates             eState          = EStates::Content;
    while (!bDone)
    {
        try
        {
            while (kCIDLib::True)
            {
                if (chSecond)
                {
                    chNext = chSecond;
                    chSecond = kCIDLib::chNull;
                }
                 else
                {
                    chNext = m_xemThis.chGetNextIfNot(kCIDLib::chLessThan);
                }

                //
                //  If the end of input or an angle bracket, the we consider
                //  ourselves done and break out. End of input is possible
                //  here if there is no content.
                //
                if (!chNext || (chNext == kCIDLib::chLessThan))
                {
                    //
                    //  If we were in a leading surrogate state, then this
                    //  is an error.
                    //
                    if (bLeadingFlag)
                        PostXMLError(kXMLErrs::errcXMLE_ExpectedSurrogate);

                    bDone = kCIDLib::True;
                    break;
                }

                //
                //  We feel through because of a special char. So we have to
                //  deal with this one differently.
                //
                if (chNext == kCIDLib::chAmpersand)
                {
                    //
                    //  Its an entity reference, so lets dereference it. We
                    //  have to send any accumulated data first.
                    //
                    PassContentChars(m_strChars);

                    tCIDXML::EEntityRes eRes = eExpandEntityRef
                    (
                        kCIDLib::True
                        , chNext
                        , chSecond
                        , bEscaped
                        , tCIDXML::ERefFrom::OutsideLiteral
                    );

                    // If it was pushed or empty, then jump to top again
                    if ((eRes == tCIDXML::EEntityRes::Pushed)
                    ||  (eRes == tCIDXML::EEntityRes::Empty))
                    {
                        continue;
                    }
                }
                 else
                {
                    bEscaped = kCIDLib::False;
                }

                //
                //  If its not escaped, then check for the invalid sequence
                //  that is not supposed to happen.
                //
                if (bEscaped)
                {
                    // Any escaped char would reset the states
                    eState = EStates::Content;
                    bLeadingFlag = kCIDLib::False;
                }
                 else
                {
                    // Its not escaped so maintain the state
                    if (chNext == kCIDLib::chCloseBracket)
                    {
                        if (eState == EStates::Content)
                            eState = EStates::FirstBracket;
                        else if (eState == EStates::FirstBracket)
                            eState = EStates::SecondBracket;
                    }
                     else if (chNext == kCIDLib::chGreaterThan)
                    {
                        if (eState == EStates::SecondBracket)
                            PostXMLError(kXMLErrs::errcXMLE_InvalidCharSeq);
                        eState = EStates::Content;
                    }
                     else
                    {
                        // Not a special char so reset state
                        eState = EStates::Content;
                    }
                }

                //
                //  Now we can check for correct surrogate pairs. We have to
                //  do this here because this is one of the few places where
                //  we are not checking that the char meets some specific
                //  criteria (e.g. name token, whitespace, etc...)
                //
                if ((chNext >= 0xD800) && (chNext <= 0xDBFF))
                {
                    if (bLeadingFlag)
                        PostXMLError(kXMLErrs::errcXMLE_ExpectedSurrogate);
                    else
                        bLeadingFlag = kCIDLib::True;
                }
                 else
                {
                    if ((chNext >= 0xDC00) && (chNext <= 0xDFFF))
                    {
                        if (!bLeadingFlag)
                            PostXMLError(kXMLErrs::errcXMLE_ExpectedSurrogate);
                    }
                     else
                    {
                        if (bLeadingFlag)
                            PostXMLError(kXMLErrs::errcXMLE_ExpectedSurrogate);

                        //
                        //  In this one case, if the ignore bad chars flag is
                        //  set, we'll translate the character to a space.
                        //
                        if (!TXMLCharFlags::bIsXMLChar(chNext))
                        {
                            if (tCIDLib::bAllBitsOn(m_eOptions, tCIDXML::EParseOpts::IgnoreBadChars))
                                chNext = kCIDLib::chSpace;
                            else
                                PostXMLError(kXMLErrs::errcXMLE_NonXMLCharacter);
                        }
                    }
                    bLeadingFlag = kCIDLib::False;
                }

                //
                //  Ok, we can take it, so store it in our buffer. If it
                //  hits 32K, then pass the content chars to the handler.
                //
                m_strChars.Append(chNext);
                if (m_strChars.c4Length() >= 32 * 1024)
                {
                    PassContentChars(m_strChars);
                    m_strChars.Clear();
                }
            }
        }

        catch(const TXMLEndOfEntity& errToCatch)
        {
            if (m_pmxevDocEvents)
            {
                bLeadingFlag = kCIDLib::False;
                PassContentChars(m_strChars);

                if (m_pmxevEntityEvents && bInfoWanted(tCIDXML::EParseFlags::Topology))
                    m_pmxevEntityEvents->EndEntity(errToCatch.xdeclEntityEnded());
            }
        }
    }

    // Send any trailing chars we might have gotten
    PassContentChars(m_strChars);
}


tCIDLib::TVoid TXMLParserCore::ParseContent(const tCIDLib::TBoolean)
{
    // <TBD> What is the parm for? We aren't using it

    //
    //  And now we enter the main content loop. On each round, we probe to
    //  see what's next, then call a parsing method to parse that type of
    //  markup.
    //
    tCIDLib::TBoolean bDone = kCIDLib::False;

    tCIDLib::TCard4 c4OrgSpooler = kCIDLib::c4MaxCard;
    while (!bDone)
    {
        // Probe what is the next thing we are going to parse
        const tCIDXML::EMarkupTypes eType = eNextMarkupType(c4OrgSpooler);

        //
        //  Check for some special cases first. These are not markup
        //  while the rest are.
        //
        if (eType == tCIDXML::EMarkupTypes::Characters)
        {
            ParseChars();
            continue;
        }
         else if (eType == tCIDXML::EMarkupTypes::EOI)
        {

            // Set the done flag and continue, which will get us out
            bDone = kCIDLib::True;

            // If the entity spooler stack is not empty, then its an error
            if (!m_xcsElems.bIsEmpty())
            {
                tCIDLib::TCard4 c4DummyId;
                PostXMLError
                (
                    kXMLErrs::errcXMLE_OpenTagsAtEOI
                    , m_xcsElems.xdeclTopElem(c4DummyId).strFullName()
                );
            }

            continue;
        }
         else if (eType == tCIDXML::EMarkupTypes::Unknown)
        {
            // Try to sync back up
            if (!m_xemThis.bSkipPastChar(kCIDLib::chGreaterThan))
                ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
            continue;
        }

        if (eType == tCIDXML::EMarkupTypes::CDATA)
        {
            ParseCDATA();
        }
         else if (eType == tCIDXML::EMarkupTypes::Comment)
        {
            ParseComment();
        }
         else if (eType == tCIDXML::EMarkupTypes::EndTag)
        {
            //
            //  Parse the end tag. If this is the end of the root
            //  tag, it will set bDone to make us fall out.
            //
            bDone = bParseEndTag(c4OrgSpooler);
        }
         else if (eType == tCIDXML::EMarkupTypes::PI)
        {
            ParsePI();
        }
         else if (eType == tCIDXML::EMarkupTypes::StartTag)
        {
            //
            //  Parse the end tag. If this is the root tag and it is
            //  an empty tag, it will set bDone to make us fall out.
            //
            bDone = bParseStartTag(c4OrgSpooler);
        }
         else
        {
            #if CID_DEBUG_ON
            facCIDXML().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kXMLErrs::errcGen_UnknownTokenType
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::Internal
                , TCardinal(tCIDLib::c4EnumOrd(eType))
            );
            #endif
        }

        if (c4OrgSpooler != m_xemThis.c4CurSpoolerId())
        {
            // We have a partial markup error
            PostXMLError(kXMLErrs::errcXMLE_PartialMarkupErr);
        }
    }
}


//
//  This is called when the <!CDATA string is seen. CDATA sections are not
//  supposed to be nestable, but we have to check for that and issue errors
//  though we try to keep going.
//
tCIDLib::TVoid TXMLParserCore::ParseCDATA()
{
    //
    //  The first thing we should see is the '[' character that opens up
    //  the CDATA section's contents.
    //
    if (!m_xemThis.bSkippedChar(kCIDLib::chOpenBracket))
    {
        PostXMLError(kXMLErrs::errcXMLE_ExpectedOpenBracket);

        // See if it was just spaced over
        m_xemThis.bSkippedSpaces();
        if (!m_xemThis.bSkippedChar(kCIDLib::chOpenBracket))
        {
            // It wasn't so try to skip to end of CDATA, throw if end of input
            if (!m_xemThis.bSkipPastChar(kCIDLib::chGreaterThan))
                ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
            return;
        }
    }

    //
    //  Looks ok so lets start collecting data. We'll use the fixed character
    //  data buffer member for this work.
    //
    m_strChars.Clear();

    tCIDLib::TCard4 c4NestLevel = 1;
    while (kCIDLib::True)
    {
        // Get the next char
        const tCIDLib::TCh chCur = m_xemThis.chGetNext();

        // Watch for unexpected end of input
        if (!chCur)
            ThrowParseError(kXMLErrs::errcXMLE_UnterminatedCDATA);

        //
        //  We basically have two things to watch for. One is a 'greater than'
        //  which might be an illegal nested CDATA. The other is the closing
        //  square brackets.
        //
        if (chCur == kCIDLib::chCloseBracket)
        {
            //
            //  If we are looking at ]]>, then we have to bump down the
            //  nesting count. If it hits zero, then we are done.
            //
            if (m_xemThis.bSkippedString(L"]>"))
            {
                c4NestLevel--;
                if (!c4NestLevel)
                    break;
            }
        }
         else if (chCur == kCIDLib::chGreaterThan)
        {
            // If its a nested CDATA, then bump the counter and issue an error
            if (m_xemThis.bSkippedString(L"![CDATA["))
            {
                // Just issue this once, that's enough
                if (c4NestLevel == 1)
                    PostXMLError(kXMLErrs::errcXMLE_NestedCDATA);

                c4NestLevel++;
            }
        }

        // Make sure its a legal char
        if (!TXMLCharFlags::bIsXMLChar(chCur))
            PostXMLError(kXMLErrs::errcXMLE_NonXMLCharacter);

        // Its nothing special so add it to our buffer
        m_strChars.Append(chCur);
    }

    // If we have a doc event handler and we have chars, send it
    if (m_pmxevDocEvents
    &&  bInfoWanted(tCIDXML::EParseFlags::Chars)
    &&  !m_strChars.bIsEmpty())
    {
        m_pmxevDocEvents->DocCharacters
        (
            m_strChars
            , kCIDLib::True
            , kCIDLib::False
            , m_eLocation
        );
    }
}


//
//  This method is called when an end tag is seen. It makes sure that the
//  ending tag matches with the current top of the context stack. If not, it
//  issues an error. If the element name is not known, it also issues an
//  error.
//
//  If the right element is ending, this is where the validation of the
//  element content is done.
//
//  The return value indicates if this was the end of the root element or
//  not.
//
tCIDLib::TBoolean
TXMLParserCore::bParseEndTag(const tCIDLib::TCard4 c4SpoolerId)
{
    //
    //  Get the current top of element stack. This tells us what the ending
    //  tag should be, and contains the accumulated child content.
    //
    tCIDLib::TCard4 c4StartSpoolerId;
    const TXMLElemDecl& xdeclTop = m_xcsElems.xdeclTopElem(c4StartSpoolerId);

    //
    //  Get the element name. If it fails, issue an error and try to keep
    //  going. It will probably fail terribly, but might as well go for it.
    //
    if (!m_xemThis.bGetName(m_strName))
    {
        PostXMLError(kXMLErrs::errcXMLE_ExpectedElementName);
        if (m_xemThis.bSkipPastChar(kCIDLib::chGreaterThan))
            ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
        return kCIDLib::False;
    }

    // Make sure we got back to the same spooler that the start tag was in
    if (c4SpoolerId != c4StartSpoolerId)
        PostXMLError(kXMLErrs::errcXMLE_PartialMarkupErr);

    // We can have spaces after the element name
    m_xemThis.bSkippedSpaces();

    //
    //  And we have to end with a closing angle bracket. If its not there,
    //  issue an error but keep going.
    //
    if (!m_xemThis.bSkippedChar(kCIDLib::chGreaterThan))
        PostXMLError(kXMLErrs::errcXMLE_UnterminatedTag);

    //
    //  Try to look it up by way of a QName lookup. We tell him that we are
    //  only interested in finding it, and not to fault it in if not found.
    //
    tCIDLib::TBoolean bAdded;
    TXMLElemDecl* pxdeclElem = m_pxvalValidator->pxdeclFindElemByName
    (
        m_strName, tCIDXML::EPoolOpts::Find, bAdded
    );

    //
    //  If not found, then issue an error, and return now. We don't know if
    //  we should pop the context stack top or not. Its a 50/50 proposition
    //  so we go ahead and do it on the hope that its just misspelled, i.e.
    //  that its not incorrectly nested input.
    //
    if (!pxdeclElem)
    {
        PostXMLError
        (
            kXMLErrs::errcXMLE_WrongTagEnded
            , m_strName
            , xdeclTop.strFullName()
        );
        return m_xcsElems.bPopLevel();
    }

    //
    //  Make sure that the element we found is the element on the top of the
    //  context stack. If not, issue an error saying what should have happened
    //  but keep going.
    //
    if (xdeclTop.c4Id() != pxdeclElem->c4Id())
    {
        PostXMLError
        (
            kXMLErrs::errcXMLE_WrongTagEnded
            , pxdeclElem->strFullName()
            , xdeclTop.strFullName()
        );
    }

    //
    //  If we are validating we can validate the child content of this
    //  element. We pass this info to the validator. We have to ask the
    //  contenxt stack for this info, then pass it on.
    //
    if (tCIDLib::bAllBitsOn(m_eOptions, tCIDXML::EParseOpts::Validate))
    {
        tCIDLib::TCard4 c4ChildCount;
        const tCIDLib::TCard4* pc4Children = m_xcsElems.pc4GetTopContent(c4ChildCount);
        m_pxvalValidator->bValidateContent(xdeclTop, pc4Children, c4ChildCount);
    }

    // Call the end element event
    if (m_pmxevDocEvents && bInfoWanted(tCIDXML::EParseFlags::Tags))
        m_pmxevDocEvents->EndTag(xdeclTop);

    //
    //  And pop the stack top off now. Return its return, which indicates
    //  if the stack is now empty.
    //
    return m_xcsElems.bPopLevel();
}


//
//  This method is called when a start tag is seen. The < character has
//  already been eaten, so we get the name and complete the rest of the
//  parse. This version is for non-namespace aware parsing, so it does not
//  do any special processing of xmlns attributes and assumes lexical lookup
//  of elements in the element pool and so on.
//
//  The return indicates whether this was the end end of content, i.e. if
//  this was the root and it was empty.
//
tCIDLib::TBoolean
TXMLParserCore::bParseStartTag(const tCIDLib::TCard4 c4SpoolerId)
{
    //
    //  This is the non-namespace aware optimized version of the start tag
    //  parsing. Its called if namespace processing is not enabled. So we
    //  can start off by just getting the QName and doing a lexical lookup
    //  of the element name from the validator.
    //
    //  NOTE:   This method gets to use, for performance purposes, a set of
    //          dedicated text buffers that are members of this class.
    //
    if (!m_xemThis.bGetName(m_strName))
    {
        PostXMLError(kXMLErrs::errcXMLE_ExpectedElementName);
        if (!m_xemThis.bSkipPastChar(kCIDLib::chGreaterThan))
            ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
        return m_xcsElems.bIsEmpty();
    }

    //
    //  Try to look it up by way of a QName lookup. We tell him to give us
    //  the existing declaration or provide us with a new default one that
    //  was created for this name.
    //
    tCIDLib::TBoolean bAdded;
    TXMLElemDecl* pxdeclElem = m_pxvalValidator->pxdeclFindElemByName
    (
        m_strName, tCIDXML::EPoolOpts::FindOrAdd, bAdded
    );

    //
    //  If it was added and we are validating, then we have to issue an
    //  error. We get the name to report from the element decl, since it will
    //  be correctly expanded for the type of validator used.
    //
    if (bAdded && tCIDLib::bAllBitsOn(m_eOptions, tCIDXML::EParseOpts::Validate))
        PostXMLError(kXMLErrs::errcXMLV_ElemNotDeclared, pxdeclElem->strFullName());

    //
    //  Remember if this is the root element. We know its the root if the
    //  context stack is empty.
    //
    const tCIDLib::TBoolean bIsRoot = m_xcsElems.bIsEmpty();

    //
    //  If its not the root, then this element is the child of the current
    //  element. So, before we push this new one, add this element to the
    //  child list of the parent.
    //
    if (!bIsRoot)
        m_xcsElems.AddAsChild(pxdeclElem->c4Id());

    //
    //  If this was the root element, then we need to check that the root
    //  element is a valid root element according to the validator in use.
    //
    //  If its not the root, then this element is the child of some other
    //  element so we need to add it to the child list of the old top of
    //  stack.
    //
    if (bIsRoot && tCIDLib::bAllBitsOn(m_eOptions, tCIDXML::EParseOpts::Validate))
    {
        if (!m_pxvalValidator->bIsValidRoot(*pxdeclElem))
        {
            PostXMLError
            (
                kXMLErrs::errcXMLE_IllegalRootElem, pxdeclElem->strFullName()
            );
        }
    }

    //
    //  Each element decl has (at least conceptually) a set of attributes
    //  that are defined for it. Each one has a 'provided' flag that we use
    //  to keep up with whether they were provided here or not. So we ask
    //  first that the element clear its attribute's provided flags.
    //
    //  It returns whether it has any defined attributes. We save this to
    //  speed up some work below.
    //
    pxdeclElem->bClearAttrFlags();

    tCIDLib::TBoolean bSkippedSpace = kCIDLib::False;
    const tCIDLib::TCard4 c4InitListSize = m_pcolAttrList->c4ElemCount();
    tCIDLib::TCard4 c4AttrCount = 0;
    while (kCIDLib::True)
    {
        //
        //  Peek at the next char to see what we need to do. If its not
        //  one of the special chars that would cause us to break out, or
        //  whitespace, then assume its an attribute name and process it.
        //
        const tCIDLib::TCh chNext = m_xemThis.chPeekNext();

        // Watch for end of input
        if (!chNext)
            ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);

        // Break out if its a forward slash or greater than
        if ((chNext == kCIDLib::chForwardSlash)
        ||  (chNext == kCIDLib::chGreaterThan))
        {
            break;
        }

        if (TXMLCharFlags::bIsSpace(chNext))
        {
            // Skip them and try again
            m_xemThis.bSkippedSpaces();
            bSkippedSpace = kCIDLib::True;
            continue;
        }
         else
        {
            //
            //  If we've gotten an attribute already, then dang it we have to
            //  have some whitespace here.
            //
            if (c4AttrCount && !bSkippedSpace)
                PostXMLError(kXMLErrs::errcXMLE_ExpectedWhitespace);
            bSkippedSpace = kCIDLib::False;
        }

        //
        //  Its got to be an attribute name, at least if its legal anyway.
        //  So parse it out.
        //
        if (!m_xemThis.bGetName(m_strName))
        {
            PostXMLError(kXMLErrs::errcXMLE_ExpectedAttrName);
            if (!m_xemThis.bSkipPastChar(kCIDLib::chGreaterThan))
                ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
            return m_xcsElems.bIsEmpty();
        }

        //
        //  Look up this attribute on the parent element. It will be faulted
        //  in if it does not exist.
        //
        TXMLAttrDef* pxadCur = pxdeclElem->pxadFindAttrDef(m_strName, bAdded);

        //
        //  If we are validating and it was faulted in, then it was never
        //  declared in the structural description, so issue an error. If it
        //  was found, then see if its marked as provided. If so, then this
        //  is a duplicate. Either way, mark it as provided.
        //
        if (bAdded)
        {
            if (tCIDLib::bAllBitsOn(m_eOptions, tCIDXML::EParseOpts::Validate))
            {
                PostXMLError
                (
                    kXMLErrs::errcXMLV_AttrNotDeclared
                    , pxadCur->strFullName()
                    , pxdeclElem->strFullName()
                );
            }
        }
         else
        {
            if (pxadCur->bProvided())
                PostXMLError(kXMLErrs::errcXMLE_AttrAlreadyUsed, pxadCur->strFullName());
        }

        // Mark it as provided now
        pxadCur->bProvided(kCIDLib::True);

        //
        //  Now we have to have an equal sign, with optional whitespace on
        //  either side of it.
        //
        if (!m_xemThis.bParseEqualSign())
        {
            //
            //  Issue an error, then try to recover. If the next char is a
            //  quote, then assume the = is missing. Else, just give up.
            //
            PostXMLError(kXMLErrs::errcXMLE_ExpectedEqualSign);

            const tCIDLib::TCh chTmp = m_xemThis.chPeekNext();
            if ((chTmp != kCIDLib::chQuotation)
            &&  (chTmp != kCIDLib::chApostrophe))
            {
                ThrowParseError(kXMLErrs::errcParse_CannotContinue);
            }
        }

        //
        //  Now we have to parse the attribute value. This involves general
        //  entity expansion as well as normalization according to the type
        //  of the attribute. In this non-NS oriented version, we can do this
        //  all in one relatively straightforward step.
        //
        if (!bParseAttrValue(*pxadCur, m_strValue))
        {
            //
            //  We need to try to recover here. So lets start moving forward
            //  until we see either a > char, or a name char.
            //
            while (kCIDLib::True)
            {
                const tCIDLib::TCh chTmp = m_xemThis.chPeekNext();

                if (!chTmp)
                    ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);

                if (chTmp == kCIDLib::chGreaterThan)
                    break;
                if (TXMLCharFlags::bIsNameChar(chTmp))
                    break;

                m_xemThis.chGetNext();
            }
        }

        //
        //  Now that we've got it, we need to validate it against the
        //  definition of the attribute. This is something the validator
        //  does.
        //
        if (tCIDLib::bAllBitsOn(m_eOptions, tCIDXML::EParseOpts::Validate))
            m_pxvalValidator->bValidateAttr(*pxadCur, m_strValue);

        //
        //  Ok we can add this to the list of attributes. We use a vector
        //  of TXMLAttr objects. It will expand as needed, and we just reset
        //  them to hold the new data each time.
        //
        //  Since this is the non-namespace oriented start tag parser, we
        //  just pass empty strings for the URI and prefix parts.
        //
        if (c4AttrCount >= c4InitListSize)
        {
            // We have to add another attribute element to the vector
            m_pcolAttrList->objPlace
            (
                m_strName
                , TString::strEmpty()
                , TString::strEmpty()
                , m_strValue
                , pxadCur->eType()
                , !bAdded
            );
        }
         else
        {
            //
            //  We are still within the existing allocation so we can just
            //  store the info in existing vector elements.
            //
            m_pcolAttrList->objAt(c4AttrCount).Set
            (
                m_strName
                , TString::strEmpty()
                , TString::strEmpty()
                , m_strValue
                , pxadCur->eType()
                , !bAdded
            );
        }
        c4AttrCount++;
    }

    // Ok, skip over any space that might be after the last attr
    m_xemThis.bSkippedSpaces();

    //
    //  If we are at a / character, then this is an empty tag. Else, we need
    //  to push this element as the new top of the context stack.
    //
    const tCIDLib::TBoolean bEmpty = m_xemThis.bSkippedChar(kCIDLib::chForwardSlash);
    if (!bEmpty)
        m_xcsElems.PushNewLevel(pxdeclElem, c4SpoolerId);

    //
    //  And we must now be at the trailing > character. If not, then issue
    //  an error, but try to keep going.
    //
    if (!m_xemThis.bSkippedChar(kCIDLib::chGreaterThan))
        PostXMLError(kXMLErrs::errcXMLE_UnterminatedTag);

    //
    //  Now we have to iterate the attribute defs of this element and make
    //  sure that any which have default values are defaulted in if not
    //  provided and that any fixed ones are defaulted in or have the fixed
    //  value.
    //
    TAttrDefCursor& adcThis = pxdeclElem->adcThis();

    // Reset the cursor and if there any elements, lets check them out.
    if (adcThis.bReset())
    {
        do
        {
            //
            //  Get the current attribute def and get out its default type
            //  for convenience below.
            //
            const TXMLAttrDef& xadCur = adcThis.xadCur();
            const tCIDXML::EDefAttrTypes eDefType = xadCur.eDefaultType();

            //
            //  See if it was explicitly provided. If not, then we need to
            //  see if it should have been or whether we should fault it in.
            //
            if (!xadCur.bProvided())
            {
                // If its defaulted or fixed, then lets check it
                if ((eDefType == tCIDXML::EDefAttrTypes::Default)
                ||  (eDefType == tCIDXML::EDefAttrTypes::Fixed))
                {
                    if (c4AttrCount >= c4InitListSize)
                    {
                        // We have to add another attribute element to the vector
                        m_pcolAttrList->objPlace
                        (
                            xadCur.strFullName()
                            , TString::strEmpty()
                            , TString::strEmpty()
                            , xadCur.strValue()
                            , xadCur.eType()
                            , kCIDLib::False
                        );
                    }
                     else
                    {
                        //
                        //  We are still within the existing allocation so we
                        //  can just store the info in existing vector elements.
                        //
                        m_pcolAttrList->objAt(c4AttrCount).Set
                        (
                            xadCur.strFullName()
                            , TString::strEmpty()
                            , TString::strEmpty()
                            , xadCur.strValue()
                            , xadCur.eType()
                            , kCIDLib::False
                        );
                    }
                    c4AttrCount++;
                }

                // If we are validating and its required, then an error
                if (tCIDLib::bAllBitsOn(m_eOptions, tCIDXML::EParseOpts::Validate)
                &&  (eDefType == tCIDXML::EDefAttrTypes::Required))
                {
                    PostXMLError
                    (
                        kXMLErrs::errcXMLV_ReqAttrNotProvided
                        , xadCur.strFullName()
                    );
                }
            }
        }   while (adcThis.bNext());
    }

    // Call the start element event
    if (m_pmxevDocEvents && bInfoWanted(tCIDXML::EParseFlags::Tags))
    {
        m_pmxevDocEvents->StartTag
        (
            *this, *pxdeclElem, bEmpty, *m_pcolAttrList, c4AttrCount
        );
    }

    //
    //  If its empty and we are validating we can validate the child
    //  content of this element.
    //
    if (bEmpty && tCIDLib::bAllBitsOn(m_eOptions, tCIDXML::EParseOpts::Validate))
        m_pxvalValidator->bValidateContent(*pxdeclElem, 0, 0);

    //
    //  If this is an empty tag, return the end of content status. We
    //  also need to call the end element callback if it's wanted, since
    //  there won't be an end tag.
    //
    if (bEmpty)
    {
        if (m_pmxevDocEvents && bInfoWanted(tCIDXML::EParseFlags::Tags))
            m_pmxevDocEvents->EndTag(*pxdeclElem);
        return m_xcsElems.bIsEmpty();
    }

    // Else can't be there yet, since we have to see the end tag
    return kCIDLib::False;
}


tCIDLib::TVoid TXMLParserCore::ParsePreContent()
{
    //
    //  Handle the XMLDecl if its there. If it exists, it must be at the
    //  start, so we can simplify the logic below by checking for it
    //  specially.
    //
    tCIDLib::TBoolean bDeclFound = m_xemThis.bSkippedString(L"<?xml ");
    if (!bDeclFound)
    {
        // Watch for a decl with incorrect case, just in case... so to speak
        bDeclFound = m_xemThis.bSkippedString(L"<?XML ");
        if (bDeclFound)
            PostXMLError(kXMLErrs::errcXMLE_BadDeclCase);
    }
    if (bDeclFound)
        ParseExtDecl(tCIDXML::EDeclTypes::XML);

    //
    //  Enter a loop that will continue until we hit the root element. We
    //  peek at the next char each time around, to get a feeling for where
    //  we are going to go.
    //
    tCIDLib::TBoolean bSawDOCTYPE = kCIDLib::False;
    while (kCIDLib::True)
    {
        // Get the next character
        const tCIDLib::TCh chCur = m_xemThis.chPeekNext();

        // If we hit the end, then break out
        if (!chCur)
            break;

        //
        //  According to the character we get, we probe further to see if we
        //  get what we are expecting.
        //
        if (chCur == kCIDLib::chLessThan)
        {
            if (m_xemThis.bSkippedString(L"<!--"))
            {
                ParseComment();
            }
             else if (m_xemThis.bSkippedString(L"<?"))
            {
                ParsePI();
            }
             else if (m_xemThis.bSkippedString(L"<!DOCTYPE"))
            {
                //
                //  Ask the validator if it understands how to parse a
                //  DOCTYPE. If not, we can't do anything but issue an
                //  error and give up. Otherwise, let him parse it.
                //
                if (m_pxvalValidator->bCanParseDTD())
                {
                    //
                    //  Use a flag janitor to set the location to internal
                    //  subset, and insure the location gets reset no matter
                    //  now we get out of there.
                    //
                    TGFJanitor<tCIDXML::ELocations> janLocation
                    (
                        &m_eLocation, tCIDXML::ELocations::IntSubset
                    );

                    //
                    //  If the validator is locked, then we want to just
                    //  eat the rest of the doctype declaration and move
                    //  on. We just keep the validator as it is. Else let
                    //  it parse the DTD.
                    //
                    if (m_bValidatorLocked)
                    {
                        m_pxvalValidator->ParseDTDDecl();
                    }
                     else
                    {
                        m_pxvalValidator->ParseDTD
                        (
                            tCIDLib::bAllBitsOn(m_eOptions, tCIDXML::EParseOpts::IgnoreDTD)
                        );
                    }
                }
                 else
                {
                    ThrowParseError(kXMLErrs::errcParse_NoDOCTYPESupport);
                }

                // Remember we saw a DOCTYPE
                bSawDOCTYPE = kCIDLib::True;
            }
             else
            {
                // Check for some stuff that would be illegal here
                if (m_xemThis.bSkippedString(L"<!"))
                {
                    if (m_xemThis.bSkippedString(L"[CDATA"))
                        PostXMLError(kXMLErrs::errcXMLE_CDATAInPreContent);
                     else
                        PostXMLError(kXMLErrs::errcXMLE_ExpectedPIOrComment);

                    if (!m_xemThis.bSkipPastChar(kCIDLib::chGreaterThan))
                        ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
                }
                 else
                {
                    // Assume it must be the root element, so break out
                    break;
                }
            }
        }
         else if (TXMLCharFlags::bIsSpace(chCur))
        {
            //
            //  If the client wants this info, then accumulate the white
            //  space chars and call out with them. Else, just ask the reader
            //  to skip them as efficiently as possible.
            //
            if (m_pmxevDocEvents && bInfoWanted(tCIDXML::EParseFlags::SpaceBC))
            {
                m_xemThis.GetSpaces(m_strChars);
                m_pmxevDocEvents->DocCharacters
                (
                    m_strChars
                    , kCIDLib::False
                    , kCIDLib::True
                    , m_eLocation
                );
            }
             else
            {
                m_xemThis.bSkippedSpaces();
            }
        }
         else
        {
            //
            //  This cannot happen here, so the file has to be invalid. We
            //  have to have hit some whitespace or some < character before
            //  anything else.
            //
            PostXMLError(kXMLErrs::errcXMLE_ExpectedMarkup);
        }
    }

    //
    //  If we never saw a DOCTYPE, but we have a default external subset,
    //  then push that and ask the validator to parse it.
    //
    //  Ignore it if the ignore DTD flag is set or if the validator is
    //  locked.
    //
    if (!bSawDOCTYPE
    &&  m_esrDefExtSS
    &&  !m_bValidatorLocked
    &&  tCIDLib::bAllBitsOn(m_eOptions, tCIDXML::EParseOpts::IgnoreDTD))
    {
        TXMLEntSpooler* pxesExtSS = m_xemThis.pxesMakeNew
        (
            m_esrDefExtSS
            , kCIDLib::True
            , tCIDXML::ERefFrom::OutsideLiteral
            , tCIDXML::EEntityTypes::Neither
        );

        // Push it on the entity stack and ask the validator to parse it
        if (m_xemThis.bPushEntity(pxesExtSS, 0, *this))
            m_pxvalValidator->ParseDefExtSS(m_strDefRootElem);
    }
}


//
//  This is called after the end of the root element is seen. It handles
//  any trailing whitespace, comments, and PIs.
//
tCIDLib::TVoid TXMLParserCore::ParsePostContent()
{
    while (kCIDLib::True)
    {
        // Get the next character
        const tCIDLib::TCh chCur = m_xemThis.chPeekNext();

        // If we hit the end, then break out
        if (!chCur)
            break;

        //
        //  According to the character we get, we probe further to see if we
        //  get what we are expecting.
        //
        if (chCur == kCIDLib::chLessThan)
        {
            if (m_xemThis.bSkippedString(L"<!--"))
            {
                ParseComment();
            }
             else if (m_xemThis.bSkippedString(L"<?"))
            {
                ParsePI();
            }
             else
            {
                PostXMLError(kXMLErrs::errcXMLE_ExpectedPIOrComment);
                if (!m_xemThis.bSkipPastChar(kCIDLib::chGreaterThan))
                    ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
            }
        }
         else if (TXMLCharFlags::bIsSpace(chCur))
        {
            //
            //  If the client wants this info, then accumulate the white
            //  space chars and call out with them. Else, just ask the reader
            //  to skip them as efficiently as possible.
            //
            if (m_pmxevDocEvents && bInfoWanted(tCIDXML::EParseFlags::SpaceAC))
            {
                m_xemThis.GetSpaces(m_strChars);
                m_pmxevDocEvents->DocCharacters
                (
                    m_strChars
                    , kCIDLib::False
                    , kCIDLib::True
                    , m_eLocation
                );
            }
             else
            {
                m_xemThis.bSkippedSpaces();
            }
        }
         else
        {
            PostXMLError(kXMLErrs::errcXMLE_ExpectedPIOrComment);
            if (!m_xemThis.bSkipPastChar(kCIDLib::chGreaterThan))
                ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
        }
    }
}


//
//  This method is called from the pre-content parse when the '<?xml ' string
//  is passed. So, when we get here, we parse the reset of the decl.
//
tCIDLib::TVoid TXMLParserCore::ParseExtDecl(const tCIDXML::EDeclTypes eType)
{
    //
    //  Although the strings are supposed to be in a particular order, we want
    //  to be able to issue and error recover if they are not. So we just get
    //  them all first, remembering which order we got them in, and pass
    //  judgement on the order after the fact.
    //
    enum class EOrders
    {
        Version
        , Encoding
        , Standalone
        , Unknown

        , Count
    };

    static const TString astrNameVals[] =
    {
        TString(L"version")
        , TString(L"encoding")
        , TString(L"standalone")
        , TString(L"unknown")
    };
    static TEArray<TString, EOrders, EOrders::Count> astrNames(astrNameVals);


    //
    //  We need to have some buffers to get info into. We just use an array
    //  of string objects.
    //
    TEArray<TString, EOrders, EOrders::Count> astrList(TString::strEmpty());

    //
    //  This array allows us to track whether each string has already been
    //  seen and what order they were in. Default initial values to -1 to mean
    //  not seen.
    //
    TEArray<tCIDLib::TInt4, EOrders, EOrders::Count> ai4Flags(-1);

    //
    //  Ok, lets enter the loop that gets and stores all of the decl strings
    //  in the temp buffers.
    //
    EOrders eOrder = EOrders::Unknown;
    tCIDLib::TCard4 c4StringCount = 0;
    while (kCIDLib::True)
    {
        // See if we have at least one space here
        const tCIDLib::TBoolean bGotSpace = m_xemThis.bSkippedSpaces();

        // If we are looking at a question mark, then break out
        if (m_xemThis.bSkippedChar(kCIDLib::chQuestionMark))
            break;

        //
        //  If we are not on the first string, then we have to have had a
        //  space. If not, then issue an error.
        //
        if (c4StringCount && !bGotSpace)
            PostXMLError(kXMLErrs::errcXMLE_ExpectedWhitespace);

        //
        //  Get a name string. This will be the name of the string that we
        //  are going to get next.
        //
        if (m_xemThis.bSkippedString(astrNames[EOrders::Version]))
            eOrder = EOrders::Version;
        else if (m_xemThis.bSkippedString(astrNames[EOrders::Encoding]))
            eOrder = EOrders::Encoding;
        else if (m_xemThis.bSkippedString(astrNames[EOrders::Standalone]))
            eOrder = EOrders::Standalone;
        else
            eOrder = EOrders::Unknown;

        //
        //  See if this one has already been done. But only if its not the
        //  unknown string.
        //
        if (eOrder == EOrders::Unknown)
        {
            //
            //  Get a name token, which will be the name string that we did
            //  not understand. We have to skip it to continue and we want to
            //  report it as well.
            //
            m_xemThis.bGetNameToken(astrList[EOrders::Unknown]);
            PostXMLError
            (
                kXMLErrs::errcXMLE_UnknownDeclString, astrList[EOrders::Unknown]
            );
        }
         else if (ai4Flags[eOrder] != -1)
        {
            PostXMLError(kXMLErrs::errcXMLE_DeclStringReused, astrNames[eOrder]);
        }
         else
        {
            // Store the order we got this on in
            ai4Flags[eOrder] = c4StringCount++;
        }

        //
        //  And now scan over an equal sign, which is required. This method
        //  will handle leading and trailing spaces, which are legal here.
        //
        if (!m_xemThis.bParseEqualSign())
            PostXMLError(kXMLErrs::errcXMLE_ExpectedEqualSign);

        //
        //  Now we can get the quoted string that should follow, which holds
        //  the value for this key/value pair.
        //
        //  If it fails, this is too screwed up for us, so just skip past the
        //  end of the decl and return.
        //
        if (!m_xemThis.bGetQuotedString(astrList[eOrder]))
        {
            PostXMLError(kXMLErrs::errcXMLE_ExpectedQuotedString);
            if (!m_xemThis.bSkipPastChar(kCIDLib::chGreaterThan))
                ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
            return;
        }

        //
        //  Ok, lets check the value of the string if it was a known string.
        //  Some of these strings have known valid values.
        //
        const TString& strValue = astrList[eOrder];
        if (eOrder == EOrders::Version)
        {
            if (strValue != kCIDXML::pszXMLVersion)
                PostXMLError(kXMLErrs::errcXMLE_BadVersionStr, strValue);
        }
         else if (eOrder == EOrders::Encoding)
        {
            // Just make sure its not empty
            if (strValue.bIsEmpty())
                PostXMLError(kXMLErrs::errcXMLE_EmptyEncodingStr);
        }
         else if (eOrder == EOrders::Standalone)
        {
            // This one must be yes or no. Update the standalone flag member
            if (strValue == L"yes")
                m_bStandalone = kCIDLib::True;
            else if (strValue == L"no")
                m_bStandalone = kCIDLib::False;
            else
            {
                // Emit an error, but check again to see if its just a case issue
                PostXMLError(kXMLErrs::errcXMLE_BadStandaloneStr, strValue);

                if (strValue == L"yes")
                    m_bStandalone = kCIDLib::True;
                else if (strValue == L"no")
                    m_bStandalone = kCIDLib::False;
            }
        }
    }

    //
    //  Ok, we have read the decl except to check that its correctly capped
    //  off with a closing angle bracket. If we can't even find the angle
    //  bracket, then just give up.
    //
    if (!m_xemThis.bSkippedChar(kCIDLib::chGreaterThan))
    {
        PostXMLError(kXMLErrs::errcXMLE_UnterminatedDecl);
        if (!m_xemThis.bSkipPastChar(kCIDLib::chGreaterThan))
            ThrowParseError(kXMLErrs::errcParse_UnexpectedEOI);
    }

    //
    //  We have to have seen the version string, and it must have been
    //  first. The encoding string, if present, just be second. The
    //  standalone, if present, must be second.
    //
    if (eType == tCIDXML::EDeclTypes::XML)
    {
        if (ai4Flags[EOrders::Version] != 0)
        {
            PostXMLError(kXMLErrs::errcXMLE_DeclStringsMisordered);
        }
         else if (ai4Flags[EOrders::Encoding] != -1)
        {
            //
            //  It must be the second string, and the standalone string,
            //  if present, must be third.
            //
            if (ai4Flags[EOrders::Encoding] != 1)
            {
                PostXMLError(kXMLErrs::errcXMLE_DeclStringsMisordered);
            }
            else if ((ai4Flags[EOrders::Standalone] != -1)
                 &&  (ai4Flags[EOrders::Standalone] != 2))
            {
                PostXMLError(kXMLErrs::errcXMLE_DeclStringsMisordered);
            }
        }
         else if (ai4Flags[EOrders::Standalone] != -1)
        {
            // It must be the second string
            if (ai4Flags[EOrders::Standalone] != 1)
                PostXMLError(kXMLErrs::errcXMLE_DeclStringsMisordered);
        }
    }
     else
    {
        //
        //  The version, if present, must be the first string. The encoding
        //  string is required, and must either be first or after the version.
        //  Standalone is not allowed.
        //
        if (ai4Flags[EOrders::Version] != -1)
        {
            if (ai4Flags[EOrders::Version] != 0)
                PostXMLError(kXMLErrs::errcXMLE_DeclStringsMisordered);
            else if (ai4Flags[EOrders::Encoding] != 1)
                PostXMLError(kXMLErrs::errcXMLE_DeclStringsMisordered);
        }
         else
        {
            if (ai4Flags[EOrders::Encoding] != 0)
                PostXMLError(kXMLErrs::errcXMLE_DeclStringsMisordered);
        }

        if (ai4Flags[EOrders::Standalone] != -1)
            PostXMLError(kXMLErrs::errcXMLE_StandaloneInExt);
    }

    //
    //  If the client code wants to see this information, then lets call
    //  the callback for it.
    //
    if (m_pmxevDocEvents && bInfoWanted(tCIDXML::EParseFlags::XMLDecl))
    {
        m_pmxevDocEvents->XMLDecl
        (
            astrList[EOrders::Version]
            , astrList[EOrders::Encoding]
            , astrList[EOrders::Standalone]
        );
    }

    //
    //  And, finally we have to tell the entity manager what encoding string
    //  we saw, if any. This will let it update itself now. So far, its been
    //  working off the autosensed base encoding (and the preliminary decode
    //  of the decl based on that.)
    //
    if (!astrList[EOrders::Encoding].bIsEmpty())
        m_xemThis.SetDeclEncoding(astrList[EOrders::Encoding]);
}
