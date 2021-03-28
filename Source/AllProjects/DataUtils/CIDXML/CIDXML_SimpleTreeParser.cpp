//
// FILE NAME: CIDXML_SimpleTreeParser.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/02/2000
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
//  This file implements the simple tree parser class.
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
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TXMLTreeParser,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TXMLTreeParser
//  PREFIX: xtprs
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLTreeParser: Constructors and Destructor
// ---------------------------------------------------------------------------
TXMLTreeParser::TXMLTreeParser() :

    m_bKeepIgnorable(kCIDLib::True)
    , m_bGotErrors(kCIDLib::False)
    , m_c4TextPoolInd(0)
    , m_c4ElemPoolInd(0)
    , m_eAdoptCat(tCIDLib::EAdoptOpts::Adopt)
    , m_colElemPool(tCIDLib::EAdoptOpts::Adopt, 1024)
    , m_colTextPool(tCIDLib::EAdoptOpts::Adopt, 1024)
    , m_pcolElemStack(nullptr)
    , m_pcolErrors(nullptr)
    , m_pxcatMappings(nullptr)
    , m_pxtdocThis(new TXMLTreeDocument)
    , m_pxtnodeCur(nullptr)
{
    //
    //  Create a validator, telling it about the parser. Tell the parser about
    //  the validator, which he adopts.
    //
    TDTDValidator* pvalNew = new TDTDValidator(&m_xprsThis);
    m_xprsThis.pxvalValidator(pvalNew);

    // Install ourself on it as the doc type handler on the validator
    pvalNew->pmxevDTDEventHandler(this);

    // Install ourself as the document, error, and resolver handler on the parser
    m_xprsThis.pmxevDocEvents(this);
    m_xprsThis.pmxevErrorEvents(this);
    m_xprsThis.pmxevEntityResolver(this);

    //
    //  Create the element stack. We don't own our elements, we just hold
    //  pointers to them. They belong to the document object.
    //
    m_pcolElemStack = new TRefStack<TXMLTreeElement>(tCIDLib::EAdoptOpts::NoAdopt);

    // And the collection that will hold the errors
    m_pcolErrors = new TVector<TErrInfo>;
}

TXMLTreeParser::~TXMLTreeParser()
{
    // Not much we can do if any of these fail, but log a warning
    try
    {
        delete m_pxtdocThis;
    }

    catch(...)
    {
        facCIDXML().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_DeleteDoc
            , tCIDLib::ESeverities::Warn
            , tCIDLib::EErrClasses::Internal
        );
    }

    if (m_eAdoptCat == tCIDLib::EAdoptOpts::Adopt)
    {
        try
        {
            delete m_pxcatMappings;
        }

        catch(...)
        {
            facCIDXML().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kXMLErrs::errcTree_DeleteCat
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::Internal
            );
        }
    }

    try
    {
        delete m_pcolElemStack;
    }

    catch(...)
    {
        facCIDXML().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_DeleteElemStack
            , tCIDLib::ESeverities::Warn
            , tCIDLib::EErrClasses::Internal
        );
    }

    try
    {
        delete m_pcolErrors;
    }

    catch(...)
    {
        facCIDXML().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcTree_DeleteElemStack
            , tCIDLib::ESeverities::Warn
            , tCIDLib::EErrClasses::Internal
        );
    }
}


// ---------------------------------------------------------------------------
//  TXMLTreeParser: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TXMLTreeParser::AddMapping(const TXMLEntitySrc* const pxsrcToAdopt)
{
    //
    //  If we don't have a catalog yet, then fault in a standard catalog and
    //  indicate we own it.
    //
    if (!m_pxcatMappings)
    {
        m_pxcatMappings = new TXMLStdCatalog;
        m_eAdoptCat = tCIDLib::EAdoptOpts::Adopt;
    }

    // Add this mapping to the catalog
    m_pxcatMappings->AddMapping(pxsrcToAdopt);
}


tCIDLib::TVoid TXMLTreeParser::AddMapping(tCIDXML::TEntitySrcRef& esrToAdd)
{
    //
    //  If we don't have a catalog yet, then fault in a standard catalog and
    //  indicate we own it.
    //
    if (!m_pxcatMappings)
    {
        m_pxcatMappings = new TXMLStdCatalog;
        m_eAdoptCat = tCIDLib::EAdoptOpts::Adopt;
    }

    // Add this mapping to the catalog
    m_pxcatMappings->AddMapping(esrToAdd);
}


tCIDLib::TVoid
TXMLTreeParser::AdoptDefExtSS(          TXMLEntitySrc* const    pxesDefExtSS
                                , const TString&                strDefRootElem)
{
    // Just delegate to the parser core object
    m_xprsThis.AdoptDefExtSS(pxesDefExtSS, strDefRootElem);
}


// Get/set the flag that tells us to keep or ignore ignorable space
tCIDLib::TBoolean TXMLTreeParser::bKeepIgnorableSpace() const
{
    return m_bKeepIgnorable;
}

tCIDLib::TBoolean TXMLTreeParser::bKeepIgnorableSpace(const tCIDLib::TBoolean bIgnore)
{
    m_bKeepIgnorable = bIgnore;
    return m_bKeepIgnorable;
}


//
//  Finds the attribute with the given name on the element at the given
//  path, if it exists.
//
tCIDLib::TBoolean
TXMLTreeParser::bFindAttr(  const   TString&            strPath
                            , const TString&            strAttrName
                            ,       TString&            strToFill
                            , const tCIDLib::TBoolean   bThrowIfNot) const
{
    // Use the regular path finding method first
    tCIDLib::TCard4 c4Dummy;
    const TXMLTreeElement* pxtnodePar = TXMLTreeParser::pxtnodeFindPath
    (
        strPath, c4Dummy
    );

    if (!pxtnodePar)
    {
        if (bThrowIfNot)
        {
            facCIDXML().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kXMLErrs::errcPath_NotFound
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , strPath
            );
        }
        return kCIDLib::False;
    }

    // And find the attribute
    tCIDLib::TCard4 c4Index;
    if (!pxtnodePar->bAttrExists(strAttrName, c4Index))
    {
        if (bThrowIfNot)
        {
            facCIDXML().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kXMLErrs::errcPath_AttrNotFound
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , strAttrName
                , strPath
            );
        }
        return kCIDLib::False;
    }

    // We found it, so return the value
    const TXMLTreeAttr& xtattrRet = pxtnodePar->xtattrAt(c4Index);
    strToFill = xtattrRet.strValue();
    return kCIDLib::True;
}


//
//  Finds the element with the indicated path and returns it's contained
//  text. We assume it's a single child text node under it.
//
tCIDLib::TBoolean
TXMLTreeParser::bFindText(  const   TString&            strPath
                            ,       TString&            strToFill
                            , const tCIDLib::TBoolean   bThrowIfNot) const
{
    // Use the regular path finding method first
    tCIDLib::TCard4 c4Dummy;
    const TXMLTreeElement* pxtnodePar = TXMLTreeParser::pxtnodeFindPath
    (
        strPath, c4Dummy
    );
    if (!pxtnodePar)
    {
        if (bThrowIfNot)
        {
            facCIDXML().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kXMLErrs::errcPath_NotFound
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , strPath
            );
        }
        return kCIDLib::False;
    }

    // And get the first child as text
    const TXMLTreeText& xtnodeText = pxtnodePar->xtnodeChildAtAsText(0);
    strToFill = xtnodeText.strText();
    return kCIDLib::True;
}


//
//  Just passthroughs to expose the validator locking feature of the
//  parser core.
//
tCIDLib::TBoolean TXMLTreeParser::bLockValidator() const
{
    return m_xprsThis.bLockValidator();
}

tCIDLib::TBoolean
TXMLTreeParser::bLockValidator(const tCIDLib::TBoolean bToSet)
{
    return m_xprsThis.bLockValidator(bToSet);
}


//
//  Kicks off the parsing of the document. They give us an entity source
//  ref to start parsing, plus flags. We just pass this on to the parser.
//
//  They can also just give us a path and we'll pass that to the parser
//  and he'll figure out if it's some entity type he can grok.
//
tCIDLib::TBoolean
TXMLTreeParser::bParseRootEntity(       tCIDXML::TEntitySrcRef& esrRoot
                                , const tCIDXML::EParseOpts     eOpts
                                , const tCIDXML::EParseFlags    eFlags)
{
    // Do a reset first
    Reset();

    // Pass this on to the parser core object
    m_xprsThis.ParseRootEntity(esrRoot, eOpts, eFlags);

    return !m_bGotErrors;
}

tCIDLib::TBoolean
TXMLTreeParser::bParseRootEntity(const  TString&                strRootEntity
                                , const tCIDXML::EParseOpts     eOpts
                                , const tCIDXML::EParseFlags    eFlags)
{
    // Do a reset first
    Reset();

    // Pass it on to the parser core object
    m_xprsThis.ParseRootEntity(strRootEntity, eOpts, eFlags);

    return !m_bGotErrors;
}


// Get the current line that we are in during the parsing process
tCIDLib::TCard4 TXMLTreeParser::c4CurLine() const
{
    return m_xprsThis.c4CurLine();
}


//
//  Does a search of a path and returns all the node indices along the
//  way to the target node, i.e. it provides a pretty efficient means
//  to keep track of a whole path through the tree to a given element.
//  We also return a pointer to the final node as well, since that's
//  often what is wanted and avoids them having to work bakc through
//  the list again.
//
//  The syntax is XPath style but only supporting the basic path like
//  structure of forward slash separated element names. We also support
//  the /Name[x]/ type syntax, where for now x can be the 1 based number
//  of the instance of the child element of that name.
//
//  There can be other stuff at the end of such a path, to match things
//  within the target element, but we don't support that yet.
//
tCIDLib::TCard4
TXMLTreeParser::c4FindPath( const   TString&            strPath
                            ,       TIndexList&         fcolIndices
                            , const TXMLTreeElement*&   pxtnodeTar) const
{
    // If the path is empty or no document, then return zero now
    fcolIndices.RemoveAll();
    if (strPath.bIsEmpty() || !m_pxtdocThis)
        return 0;

    //
    //  We just need to start tokenizing the passed path. For each token,
    //  we look for the the appropriate child elements by name.
    //
    TStringTokenizer stokPath(&strPath, L"/");

    //
    //  So let's start with the root node, and we'll work our way down.
    //  We have to special case the root node since it has no parent.
    //
    TXMLTreeElement* pxtnodeCur = &m_pxtdocThis->xtnodeRoot();
    TString strToken;
    stokPath.bGetNextToken(strToken);
    if (pxtnodeCur->strQName() != strToken)
        return 0;

    //
    //  If there are no more tokens, then the root is the guy. We add a
    //  dummy 0 entry for the root element, though it has no parent.
    //
    if (!stokPath.bMoreTokens())
    {
        pxtnodeTar = pxtnodeCur;
        fcolIndices.c4AddElement(0);
        return 1;
    }

    // Else, there are more, so let's start searching
    TXMLTreeElement* pxtnodePar = pxtnodeCur;
    TString strExt;
    tCIDLib::TCard4 c4FndAt;
    while(kCIDLib::True)
    {
        //
        //  Get the next token and see if it has any extension, which will
        //  be in brackets at the end.
        //
        stokPath.bGetNextToken(strToken);
        if (strToken.bSplit(strExt, kCIDLib::chOpenBracket))
        {
            // It has to be a number in the brackets
            strExt.DeleteLast();

            tCIDLib::TCard4 c4Inst;
            if (!strExt.bToCard4(c4Inst, tCIDLib::ERadices::Dec) || !c4Inst)
            {
                // Couldn't convert or it was zero, which isn't legal
                facCIDXML().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kXMLErrs::errcPath_BadInstNum
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                    , strExt
                );
            }

            //
            //  Look for the indicated instance of child elements with
            //  this name.
            //
            c4Inst -= 1;
            pxtnodeCur = pxtnodePar->pxtnodeFindElement(strToken, 0, c4FndAt, c4Inst);
        }
         else
        {
            // No extension, so it's just the name of the next child
            pxtnodeCur = pxtnodePar->pxtnodeFindElement(strToken, 0, c4FndAt);
        }

        //
        //  If not found, then we failed so break out with empty collection,
        //  otherwise add it to the list of indices.
        //
        if (!pxtnodeCur)
        {
            pxtnodeTar = 0;
            fcolIndices.RemoveAll();
            break;
        }

        // We found it. So add to the index list. If no more tokens, we are done
        fcolIndices.c4AddElement(c4FndAt);
        if (!stokPath.bMoreTokens())
        {
            pxtnodeTar = pxtnodeCur;
            break;
        }

        // Not done yet, so move forward
        pxtnodePar = pxtnodeCur;
    }
    return fcolIndices.c4ElemCount();
}


// Get access to the list of errors that occured on the last parse
const TVector<TXMLTreeParser::TErrInfo>& TXMLTreeParser::colErrors() const
{
    return *m_pcolErrors;
}


// Removes any catalog mappings that have been set
tCIDLib::TVoid TXMLTreeParser::ClearCatalog()
{
    if (m_pxcatMappings)
        m_pxcatMappings->RemoveAll();
}


// Retursn the first error after a parse
const TXMLTreeParser::TErrInfo& TXMLTreeParser::erriFirst() const
{
    return m_pcolErrors->objAt(0);
}


// Sets a catalog on us. Destroyes any previous catalog
tCIDLib::TVoid
TXMLTreeParser::InstallCatalog(         TXMLCatalog* const  pxcatToSet
                                , const tCIDLib::EAdoptOpts eAdopt)
{
    // If we have a current one and own it, delete it
    if (m_pxcatMappings)
    {
        if (m_eAdoptCat == tCIDLib::EAdoptOpts::Adopt)
            delete m_pxcatMappings;
        m_pxcatMappings = 0;
    }

    m_pxcatMappings = pxcatToSet;
    m_eAdoptCat = eAdopt;
}


//
//  Orphans a document out to the caller. We'll create a new one on the
//  next parse.
//
TXMLTreeDocument* TXMLTreeParser::pxtdocOrphan()
{
    // Save the return value in a temp
    TXMLTreeDocument* pxtdocRet = m_pxtdocThis;

    // Create a new empty document for us
    m_pxtdocThis = new TXMLTreeDocument;

    // And return the orphaned document
    return pxtdocRet;
}


//
//  This is a simple convenience method that calls the more general purpose
//  path searcher and returns the final node in the path, along with its
//  index within the parent node. For the root node the index is returned
//  as zero.
//
const TXMLTreeElement*
TXMLTreeParser::pxtnodeFindPath(const   TString&            strPath
                                ,       tCIDLib::TCard4&    c4Index) const
{
    const TXMLTreeElement* pnodeRet;
    if (!c4FindPath(strPath, m_fcolIndices, pnodeRet))
        return nullptr;

    c4Index = m_fcolIndices.tLast();
    return pnodeRet;
}



//
//  Cleans up any parser data that might have been left from a previous
//  parse, to avoid eating up memory until the parser is used next time.
//
//  They can also ask us to flush the pools in order to completely cut
//  back the memory usage. But normally we don't do that and keep them
//  around for subsequent use.
//
tCIDLib::TVoid TXMLTreeParser::Reset(const tCIDLib::TBoolean bFlushPools)
{
    // Reset the underlying parser
    m_xprsThis.Reset();

    // And flush any stuff at our level
    m_fcolIndices.RemoveAll();
    m_pcolElemStack->RemoveAll();
    m_pcolErrors->RemoveAll();

    // Reset the documents
    m_pxtdocThis->Reset();

    // Reset our node pool indices
    m_c4ElemPoolInd = 0;
    m_c4TextPoolInd = 0;

    // If asked, flush the pools
    if (bFlushPools)
    {
        m_colElemPool.RemoveAll();
        m_colTextPool.RemoveAll();
    }
}



//
//  Provide read only access to our catalog. If none is set, we fault in
//  a standard one.
//
const TXMLCatalog& TXMLTreeParser::xcatMappings() const
{
    if (!m_pxcatMappings)
    {
        m_pxcatMappings = new TXMLStdCatalog;
        m_eAdoptCat = tCIDLib::EAdoptOpts::Adopt;
    }
    return *m_pxcatMappings;
}


// Provides access to the document after a parse
const TXMLTreeDocument& TXMLTreeParser::xtdocThis() const
{
    return *m_pxtdocThis;
}


//
//  Like pxtnodeFindPath() above, but in this case returns a reference to
//  the node, so if it's not found it throws, whereas the one above returns
//  a null pointer.
//
const TXMLTreeElement&
TXMLTreeParser::xtnodeFindPath( const   TString&            strPath
                                ,       tCIDLib::TCard4&    c4Index) const
{
    const TXMLTreeElement* pxtnodeRet;
    if (!c4FindPath(strPath, m_fcolIndices, pxtnodeRet))
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcPath_NotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strPath
        );
    }
    c4Index = m_fcolIndices.tLast();
    return *pxtnodeRet;
}


// ---------------------------------------------------------------------------
//  TXMLTreeParser: Protected, virtual methods (document events)
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TXMLTreeParser::DocCharacters(  const   TString&            strChars
                                , const tCIDLib::TBoolean   bIsCDATA
                                , const tCIDLib::TBoolean   bIsIgnorable
                                , const tCIDXML::ELocations )
{
    // <TBD> Should we be using the location parameter?

    // If we aren't keeping ignorable, then get out if it is
    if (bIsIgnorable && !m_bKeepIgnorable)
        return;

    // We shouldn't see this before we get the root node, so ignore it if so
    if (!m_pxtnodeCur)
        return;

    //
    //  If we have a current node, and it's last child is a text node, then
    //  we have to append the new text to that existing child. Else we
    //  have to allocate a new one.
    //
    TXMLTreeText* pxtnodeText = 0;
    if (m_pxtnodeCur)
        pxtnodeText = m_pxtnodeCur->pxtnodeLastIsText();

    if (pxtnodeText)
    {
        // Append the passed text to the existing text child
        pxtnodeText->AppendText(strChars);
    }
     else
    {
        //
        //  Don't have one, so add a new one with the passed text info. Try
        //  to one from the pool if we have more, else add another one.
        //
        if (m_c4TextPoolInd == m_colTextPool.c4ElemCount())
        {
            // We are full so create a new one and add it to the list
            pxtnodeText = new TXMLTreeText(strChars, bIsCDATA, bIsIgnorable);
            m_colTextPool.Add(pxtnodeText);
        }
         else
        {
            // Get the next available one from the pool and update it
            pxtnodeText = m_colTextPool[m_c4TextPoolInd];
            pxtnodeText->Set(strChars, bIsCDATA, bIsIgnorable);
        }

        // Bump the pool index and add this one to the tree
        m_c4TextPoolInd++;
        m_pxtnodeCur->AddChild(pxtnodeText);
    }
}


tCIDLib::TVoid
TXMLTreeParser::DocComment( const   TString&            strCommentText
                            , const tCIDXML::ELocations)
{
    // <TBD> Should we be using the location parameter?

    // Append a comment node to the current element or document
    TXMLTreeComment* pxtnodeNew = new TXMLTreeComment(strCommentText);

    if (m_pxtnodeCur)
        m_pxtnodeCur->AddChild(pxtnodeNew);
    else
        m_pxtdocThis->AddChild(pxtnodeNew);
}


tCIDLib::TVoid
TXMLTreeParser::DocPI(  const   TString&            strTarget
                        , const TString&            strValue
                        , const tCIDXML::ELocations )
{
    // <TBD> Should we be using the location parameter?

    // Append a PI node to the current element or document
    TXMLTreePI* pxtnodeNew = new TXMLTreePI(strTarget, strValue);

    if (m_pxtnodeCur)
        m_pxtnodeCur->AddChild(pxtnodeNew);
    else
        m_pxtdocThis->AddChild(pxtnodeNew);
}


tCIDLib::TVoid TXMLTreeParser::EndDocument(const TXMLEntitySrc&)
{
}


tCIDLib::TVoid TXMLTreeParser::EndTag(const TXMLElemDecl&)
{
    // Pop off the previous current element if there is one
    if (!m_pcolElemStack->bIsEmpty())
        m_pxtnodeCur = m_pcolElemStack->pobjPop();
     else
        m_pxtnodeCur = 0;
}


tCIDLib::TVoid TXMLTreeParser::ResetDocument()
{
    // Clear out the document for the next round, and zero the current element
    m_pxtdocThis->Reset();
    m_pxtnodeCur = 0;
}


tCIDLib::TVoid TXMLTreeParser::StartDocument(const TXMLEntitySrc&)
{
}


tCIDLib::TVoid
TXMLTreeParser::StartTag(       TXMLParserCore&     xprsSrc
                        , const TXMLElemDecl&       xdeclElem
                        , const tCIDLib::TBoolean   bEmpty
                        , const TVector<TXMLAttr>&  colAttrList
                        , const tCIDLib::TCard4     c4AttrListSize)
{
    //
    //  If this is the root element, then allocate it since it's not a
    //  pooled one. Else we want to get one from the pool or add one
    //  to the pool.
    //
    TXMLTreeElement* pxtnodeNew = 0;
    if (m_pxtnodeCur)
    {
        // It's a nested one so deal with pooling
        if (m_c4ElemPoolInd == m_colElemPool.c4ElemCount())
        {
            // We are full so create a new one and add it to the list
            pxtnodeNew = new TXMLTreeElement
            (
                xdeclElem, colAttrList, c4AttrListSize
            );
            m_colElemPool.Add(pxtnodeNew);
        }
         else
        {
            // Get the next available one from the pool and update it
            pxtnodeNew = m_colElemPool[m_c4ElemPoolInd];
            pxtnodeNew->Set(xdeclElem, colAttrList, c4AttrListSize);
        }

        // Bump the pool index and add this one to the tree
        m_c4ElemPoolInd++;
        m_pxtnodeCur->AddChild(pxtnodeNew);
    }
     else
    {
        // This is the root, so allocate one and add to the document
        pxtnodeNew = new TXMLTreeElement
        (
            xdeclElem, colAttrList, c4AttrListSize
        );
        m_pxtdocThis->AddChild(pxtnodeNew);
    }

    //
    //
    //  If there is a current element, then push the current element onto
    //  the stack and set the new element as the current element.
    //
    if (m_pxtnodeCur)
        m_pcolElemStack->Push(m_pxtnodeCur);
    m_pxtnodeCur = pxtnodeNew;
}


tCIDLib::TVoid
TXMLTreeParser::XMLDecl(const   TString&    strVersion
                        , const TString&    strEncoding
                        , const TString&    strStandalone)
{
    // And a decl node to the document
    m_pxtdocThis->AddChild
    (
        new TXMLTreeDecl(strVersion, strEncoding, strStandalone)
    );
}


// ---------------------------------------------------------------------------
//  TXMLTreeParser: Protected, virtual methods (error events)
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TXMLTreeParser::HandleXMLError( const   tCIDLib::TErrCode   errcToPost
                                , const tCIDXML::EErrTypes  eType
                                , const TString&            strText
                                , const tCIDLib::TCard4     c4CurColumn
                                , const tCIDLib::TCard4     c4CurLine
                                , const TString&            strSystemId)
{
    // If greater than a warning, then remember we saw an error
    if (eType != tCIDXML::EErrTypes::Warning)
        m_bGotErrors = kCIDLib::True;

    // Add this error to our list of errors
    m_pcolErrors->objPlace
    (
        errcToPost, eType, strText, c4CurColumn, c4CurLine, strSystemId
    );
}


tCIDLib::TVoid TXMLTreeParser::ResetErrors()
{
    m_bGotErrors = kCIDLib::False;
    m_pcolErrors->RemoveAll();
}


// ---------------------------------------------------------------------------
//  TXMLTreeParser: Protected, virtual methods (Doc type events)
// ---------------------------------------------------------------------------
tCIDLib::TVoid TXMLTreeParser::DocType( const   TDTDElemDecl&   xdeclRoot
                                        , const TString&        strPublicId
                                        , const TString&        strSystemId)
{
    m_pxtdocThis->AddChild
    (
        new TXMLTreeDTD
        (
            xdeclRoot.strFullName(), strPublicId, strSystemId
        )
    );
}


tCIDLib::TVoid TXMLTreeParser::ResetDocType()
{
    // Nothing to do for this one
}


// ---------------------------------------------------------------------------
//  TXMLTreeParser: Protected, virtual methods (entity resolver)
// ---------------------------------------------------------------------------
tCIDXML::TEntitySrcRef
TXMLTreeParser::esrResolve( const   TString&                strPublicId
                            , const TString&                strSystemId
                            , const TString&
                            , const TString&
                            , const tCIDXML::EResolveTypes  )
{
    // <TBD> Should we be using the eResType parm?

    // If we don't have a catalog, then obviously not
    if (!m_pxcatMappings)
        return tCIDXML::TEntitySrcRef(nullptr);

    //
    //  Lets try to look up the public id. If not found, it will return a
    //  zero, which is what we want to return. If it does, it will return a
    //  pointer to he entity source, which is also what we want to return.
    //
    return m_pxcatMappings->esrMapId(strPublicId, strSystemId);
}


tCIDLib::TVoid TXMLTreeParser::ResetResolver()
{
    //
    //  We don't need to do anything in this case. The catalog stays the same
    //  over multiple parses, unless the client code changes it
    //
}

