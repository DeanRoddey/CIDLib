//
// FILE NAME: CIDAI_BTParser.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/01/2016
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
//  This file implements the behavior tree parser.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDAI_.hpp"
#include    "CIDAI_BTParser_.hpp"


// ---------------------------------------------------------------------------
//   CLASS: TCIDAIBTParser
//  PREFIX: aibtp
// ---------------------------------------------------------------------------

// -------------------------------------------------------------------
//  TCIDAIBTParser: Constructors and Destructor
// -------------------------------------------------------------------
TCIDAIBTParser::TCIDAIBTParser() :

    m_colNodeStack(tCIDLib::EAdoptOpts::NoAdopt)
    , m_pbtnodeRoot(nullptr)
{
    TDTDValidator* pvalNew = new TDTDValidator(&m_xtprsThis);
    m_xtprsThis.pxvalValidator(pvalNew);

    // Install ourself on it as the doc type handler on the validator
    pvalNew->pmxevDTDEventHandler(this);

    // Set ourself as handlers on the parser
    m_xtprsThis.pmxevDocEvents(this);
    m_xtprsThis.pmxevErrorEvents(this);
    m_xtprsThis.pmxevEntityResolver(this);

    //
    //  Set up the fixed internal DTD in an entity reference we can just pass back
    //  to the XML parser when he asks to resolve the DTD.
    //
    m_esrDTD.SetPointer
    (
        new TMemBufEntitySrc(s_strDTDSystemId, s_strDTDPublicId, s_strDTD)
    );
}

TCIDAIBTParser::~TCIDAIBTParser()
{
}


// -------------------------------------------------------------------
//  TCIDAIBTParser: Public, non-virtual methods
// -------------------------------------------------------------------
TAIBTRootNode*
TCIDAIBTParser::pbtnodeParse(const TMemBuf& mbufSrc, const tCIDLib::TCard4 c4Bytes)
{
    m_colNodeStack.RemoveAll();

    try
    {
        // Reset our per-parse data
        if (m_pbtnodeRoot)
        {
            delete m_pbtnodeRoot;
            m_pbtnodeRoot = nullptr;
        }

        m_strCurXMLPath.Clear();

        // Create a buffer based entity source to feed to the parser
        TMemBufEntitySrc* pxsrcBuf = new TMemBufEntitySrc(L"CIDAIBTreeDef", mbufSrc, c4Bytes);
        tCIDXML::TEntitySrcRef esrBuf(pxsrcBuf);

        // Kick off the parsing
        const tCIDXML::EParseOpts   eOpts  = tCIDXML::EParseOpts::IgnoreBadChars;
        const tCIDXML::EParseFlags  eFlags = tCIDXML::EParseFlags::Chars
                                             | tCIDXML::EParseFlags::Tags;
        m_xtprsThis.ParseRootEntity(esrBuf, eOpts, eFlags);
    }

    catch(TError& errToCatch)
    {
        try
        {
            delete m_pbtnodeRoot;
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    catch(...)
    {
    }

    return m_pbtnodeRoot;
}



// ---------------------------------------------------------------------------
//  TCIDAIBTParser: Protected, virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TCIDAIBTParser::DocCharacters(  const   TString&            strChars
                                , const tCIDLib::TBoolean   bIsCDATA
                                , const tCIDLib::TBoolean   bIsIgnorable
                                , const tCIDXML::ELocations eLocation)
{
}


//
//  If the parser asks for our internal DTD's public id, we return our prefab entity
//  reference.
//
tCIDXML::TEntitySrcRef
TCIDAIBTParser::esrResolve( const   TString&                strPublicId
                            , const TString&
                            , const TString&
                            , const TString&
                            , const tCIDXML::EResolveTypes  )
{
    if (strPublicId.bCompareI(s_strDTDPublicId))
    {
        //
        //  Return a copy, which gives them their own counted ref to the entity
        //  source.
        //
        return m_esrDTD;
    }

    // Not found, so return a null
    return tCIDXML::TEntitySrcRef(0);
}


tCIDLib::TVoid TCIDAIBTParser::EndDocument(const TXMLEntitySrc& xsrcOfRoot)
{
}


tCIDLib::TVoid TCIDAIBTParser::EndTag(const TXMLElemDecl& xdeclElem)
{
    if (xdeclElem.strFullName() == s_strElem_BTNode)
    {
        // Pop the node stack now. We are done with this one
        m_colNodeStack.pobjPop();
    }
     else if (xdeclElem.strFullName() == s_strElem_NodeParams)
    {
        // If we have any parameters, set them on the node at the top of the stack
        if (!m_colParams.bIsEmpty())
            m_colNodeStack.pobjPeek()->SetParams(m_colParams);
    }

    // Pop the XML path back a level
    tCIDLib::TCard4 c4At = 0;
    if (!m_strCurXMLPath.bLastOccurrence(kCIDLib::chForwardSlash, c4At, kCIDLib::True))
    {
        CIDAssert2(L"No slash was found in XML path to remove");
    }
    m_strCurXMLPath.CapAt(c4At);
}


tCIDLib::TVoid
TCIDAIBTParser::HandleXMLError( const   tCIDLib::TErrCode   errcToPost
                                , const tCIDXML::EErrTypes  eType
                                , const TString&            strText
                                , const tCIDLib::TCard4     c4CurColumn
                                , const tCIDLib::TCard4     c4CurLine
                                , const TString&            strSystemId)
{
    facCIDAI().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kAIErrs::errcBTParse_XMLError
        , strText
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::AppStatus
        , TCardinal(c4CurLine)
        , TCardinal(c4CurColumn)
        , strSystemId
    );
}


tCIDLib::TVoid TCIDAIBTParser::ResetDocType()
{
    // A no-op for us
}


tCIDLib::TVoid TCIDAIBTParser::ResetDocument()
{
    // A no-op for us, we just do what we need when the parse method is called
}


// A no-op for us
tCIDLib::TVoid TCIDAIBTParser::ResetResolver()
{
}


tCIDLib::TVoid TCIDAIBTParser::StartDocument(const TXMLEntitySrc&)
{
}


tCIDLib::TVoid
TCIDAIBTParser::StartTag(       TXMLParserCore&     xprsSrc
                        , const TXMLElemDecl&       xdeclElem
                        , const tCIDLib::TBoolean   bEmpty
                        , const TVector<TXMLAttr>&  colAttrList
                        , const tCIDLib::TCard4     c4AttrCnt)
{
    // Add this element to the XML path
    m_strCurXMLPath.Append(kCIDLib::chForwardSlash);
    m_strCurXMLPath.Append(xdeclElem.strFullName());

    //
    //  If we have no root node, then this is the first one and it has to be the root.
    //  So create the root node. Else, we are adding a child to the current node so start
    //  collecting info for it.
    //
    if (m_pbtnodeRoot)
    {
        if (xdeclElem.strFullName() == s_strElem_NodeParams)
        {
            // Reset the parameters list
            m_colParams.RemoveAll();
        }
         else if (xdeclElem.strFullName() == s_strElem_NodeParam)
        {
            // Store this new parameter
            m_colParams.objAdd
            (
                TKeyValuePair
                (
                    strFindAttr(colAttrList, s_strAttr_Key, kCIDLib::True)
                    , strFindAttr(colAttrList, s_strAttr_Value, kCIDLib::True)
                )
            );
        }
         else if (xdeclElem.strFullName() == s_strElem_BTNode)
        {
            // Create the new node and push it onto the stack
            TAIBTNode* pbtnodeNew = m_colNodeStack.pobjPeek()->pbtnodeAddChild
            (
                strFindAttr(colAttrList, s_strAttr_NodeName, kCIDLib::False)
                , strFindAttr(colAttrList, s_strAttr_NodeType, kCIDLib::True)
                , strFindAttr(colAttrList, s_strAttr_Flag, kCIDLib::True) == L"True"
            );
            m_colNodeStack.Push(pbtnodeNew);

            // Reset the parameters since any we see now must be new ones for this node
            m_colParams.RemoveAll();
        }
    }
     else
    {
        CheckNodeName(xdeclElem, s_strElem_BTree);

        // The node stack should be empty
        CIDAssert
        (
            m_colNodeStack.bIsEmpty()
            , L"The node stack must be empty on seeing first tag"
        );

        //
        //  This should be the top level element. Get the name attribute and version,
        //  both of which are required. There are no parameters for this one.
        //
        const TString& strName = strFindAttr(colAttrList, s_strAttr_TreeName, kCIDLib::True);
        const TString& strVersion = strFindAttr(colAttrList, s_strAttr_Version, kCIDLib::True);

        tCIDLib::TCard4 c4Version;
        if (!strVersion.bToCard4(c4Version, tCIDLib::ERadices::Dec))
        {
            facCIDAI().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kAIErrs::errcBTParse_TreeVer
                , strName
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
            );
        }

        //
        //  Now create the root node. We don't need the node factory for this. The
        //  tree path for this one is just / since it's the root node and has no
        //  parent.
        //
        m_pbtnodeRoot = new TAIBTRootNode(L"/", strName, c4Version);

        // Push this as the current node and set the XML path
        m_colNodeStack.Push(m_pbtnodeRoot);
    }
}



// ---------------------------------------------------------------------------
//  TCIDAIBTParser: Private, non-virtual methods
// ---------------------------------------------------------------------------

// Checks the name of the passed element and throws if not the expected one
tCIDLib::TVoid
TCIDAIBTParser::CheckNodeName(  const   TXMLElemDecl&   xdeclElem
                                , const TString&        strExpected)
{
    if (xdeclElem.strFullName() != strExpected)
    {
        facCIDAI().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kAIErrs::errcBTParse_ExpElem
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strExpected
            , xdeclElem.strFullName()
        );
    }
}


//
//  Looks for the indicated attribute in the passed attribute list. If not found,
//  it will throw or return an empty string, depending on the required parameter.
//
const TString&
TCIDAIBTParser::strFindAttr(const   TVector<TXMLAttr>&  colAttrList
                            , const TString&            strToFind
                            , const tCIDLib::TBoolean   bRequired)
{
    const tCIDLib::TCard4 c4Count = colAttrList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (colAttrList[c4Index].strQName() == strToFind)
            return colAttrList[c4Index].strValue();
    }

    // Never found so throw or return empty
    if (bRequired)
    {
        facCIDAI().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kAIErrs::errcBTParse_AttrNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strToFind
            , m_strCurXMLPath
        );
    }
    return TString::strEmpty();
}


// ---------------------------------------------------------------------------
//  TCIDAIBTParser: Private static data
// ---------------------------------------------------------------------------

//
//  The DTD we use when parsing a behavior tree. We just load it up internally and set
//  it on the XML parser.
//
const TString TCIDAIBTParser::s_strDTD =

L"<?xml encoding='$NativeWideChar$'?>\n"

// A parameter that can be passed to a node
L"<!ELEMENT NodeParam       EMPTY>\n"
L"<!ATTLIST NodeParam       Key NMTOKEN #REQUIRED\n"
L"                          Value CDATA #REQUIRED>\n"

// Can be empty, see BTNode comments below
L"<!ELEMENT NodeParams      (NodeParam*)>\n"

//
//  A node in the tree. It can have optional parameters and zero or more child nodes.
//  It can have a name, and there's a flag for the application to use.
//
L"<!ELEMENT BTNode          (NodeParams?, BTNode*)>\n"
L"<!ATTLIST BTNode          NodeType NMTOKEN #REQUIRED\n"
L"                          NodeName CDATA ''\n"
L"                          Flag (True | False) 'False'>\n"

// A tree itself
L"<!ELEMENT BehaviorTree    (BTNode)>\n"
L"<!ATTLIST BehaviorTree    TreeName CDATA #REQUIRED\n"
L"                          Version CDATA '1'>\n"

;

const TString TCIDAIBTParser::s_strAttr_Flag(L"Flag");
const TString TCIDAIBTParser::s_strAttr_Key(L"Key");
const TString TCIDAIBTParser::s_strAttr_NodeName(L"NodeName");
const TString TCIDAIBTParser::s_strAttr_NodeType(L"NodeType");
const TString TCIDAIBTParser::s_strAttr_TreeName(L"TreeName");
const TString TCIDAIBTParser::s_strAttr_Value(L"Value");
const TString TCIDAIBTParser::s_strAttr_Version(L"Version");

const TString TCIDAIBTParser::s_strDTDPublicId(L"urn:charmedquark.com:CID-BehaviorTree.DTD");
const TString TCIDAIBTParser::s_strDTDSystemId(L"http://www.charmedquark.com/CIDLib/BehaviorTree.DTD");

const TString TCIDAIBTParser::s_strElem_BTNode(L"BTNode");
const TString TCIDAIBTParser::s_strElem_NodeParam(L"NodeParam");
const TString TCIDAIBTParser::s_strElem_NodeParams(L"NodeParams");
const TString TCIDAIBTParser::s_strElem_BTree(L"BehaviorTree");

