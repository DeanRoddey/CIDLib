//
// FILE NAME: CIDMacroEng_XMLTreeParserClass.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/11/2005
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
//  This file implements a set of derivatives of the class info and class
//  value classes, which implement the fundamental XMLTreeParser and
//  XMLTreeAnchor CML classes. We also create some derivatives of some
//  XML classes, in order to get the control we need.
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


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TMEngFileEntitySrc,TXMLEntitySrc)
RTTIDecls(TMEngXMLTreeParser, TXMLTreeParser)

RTTIDecls(TMEngXMLTreeAnchorVal,TMEngClassVal)
RTTIDecls(TMEngXMLTreeAnchorInfo,TMEngClassInfo)
RTTIDecls(TMEngXMLTreeParserVal,TMEngClassVal)
RTTIDecls(TMEngXMLTreeParserInfo,TMEngClassInfo)



// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace
{
    namespace CIDMacroEng_XMLTreeParserClasses
    {
        // -----------------------------------------------------------------------
        //  The names for the classes that we support here. Each derivative has to
        //  be able to return strings that contain its name and full name.
        // -----------------------------------------------------------------------
        const TString   strXMLTreeAnchor(L"XMLTreeAnchor");
        const TString   strXMLTreeAnchorClassPath(L"MEng.System.Runtime.XMLTreeAnchor");

        const TString   strXMLTreeParser(L"XMLTreeParser");
        const TString   strXMLTreeParserClassPath(L"MEng.System.Runtime.XMLTreeParser");
    }
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngFileEntitySrc
// PREFIX: xsrc
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngFileEntitySrc: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngFileEntitySrc::TMEngFileEntitySrc( const   TString&    strSystemId
                                        , const TString&    strActualPath) :

    TXMLEntitySrc(strSystemId)
    , m_strActualPath(strActualPath)
{
}

TMEngFileEntitySrc::~TMEngFileEntitySrc()
{
}


// ---------------------------------------------------------------------------
//  TMEngFileEntitySrc: Public, inherited methods
// ---------------------------------------------------------------------------
TBinInStream* TMEngFileEntitySrc::pstrmMakeNew() const
{
    // Gen up a file input stream on the actual path
    return new TBinFileInStream
    (
        m_strActualPath
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::None
        , tCIDLib::EFileFlags::SequentialScan
    );
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngXMLTreeParser
// PREFIX: xtprs
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngXMLTreeParser: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngXMLTreeParser::TMEngXMLTreeParser(TCIDMacroEngine* const pmeOwner) :

    m_pmeOwner(pmeOwner)
{
}

TMEngXMLTreeParser::~TMEngXMLTreeParser()
{
}


// ---------------------------------------------------------------------------
//  TMEngXMLTreeParser: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDXML::TEntitySrcRef
TMEngXMLTreeParser::esrResolve( const   TString&                strPublicId
                                , const TString&                strSystemId
                                , const TString&
                                , const TString&                strParentId
                                , const tCIDXML::EResolveTypes  )
{
    CIDAssert(!strParentId.bIsEmpty(), L"The incoming XML parent id was empty");
    CIDAssert(!strSystemId.bIsEmpty(), L"The incoming XML system id was empty");

    //
    //  If the system id is not fully qualified we have to add it onto the
    //  parent id first.
    //
    TPathStr pathActual;
    if ((strSystemId[0] != kCIDLib::chBackSlash)
    &&  !strSystemId.bStartsWith(L"file:")
    &&  !strSystemId.bStartsWith(L"http:"))
    {
        // Search for the last forward or back slash and terminate after that
        pathActual = strParentId;

        tCIDLib::TCard4 c4At;
        if (!pathActual.bLastOccurrence(L"\\/", c4At, kCIDLib::True, kCIDLib::False))
        {
            //
            //  It can't be valid, since it would have to include a slash
            //  before the file name or final URL target file.
            //
            facCIDMacroEng().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcRT_BadXMLId
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , strParentId
            );
        }

        //
        //  This could cause an exception if the slash was the last
        //  character, but that would mean the parent id is invalid
        //  since it didn't point to an actual entity.
        //
        pathActual.CapAt(c4At + 1);

        //
        //  And now add the system id onto that. The system id cannot start
        //  with a slash, and we left the trailing one above, so just append.
        //
        pathActual.Append(strSystemId);
    }
     else
    {
        // Just start with the system id
        pathActual = strSystemId;
    }

    //
    //  OK, now deal with file paths or file based URLs and remap those.
    //  http ones we can just allow through. http URLs we'll just return
    //  a standard URL based one in that case.
    //
    TXMLEntitySrc* pxesRet = 0;
    if (pathActual.bStartsWith(L"http:"))
    {
        pxesRet = new TURLEntitySrc(pathActual);
    }
     else if (pathActual.bStartsWith(L"file:"))
    {
        // Get the path part of the URL out
        TURL urlActual(pathActual, tCIDSock::EQualified::Full);
        TPathStr pathFull;
        m_pmeOwner->ExpandFilePath(urlActual.strPath(), pathFull);
        pxesRet = new TMEngFileEntitySrc(strSystemId, pathFull);
    }
     else
    {
        // It's gotta be a file path
        TPathStr pathFull;
        m_pmeOwner->ExpandFilePath(pathActual, pathFull);
        pxesRet = new TMEngFileEntitySrc(strSystemId, pathFull);
    }

    // Nothing we need to deal wtih, so return an empty ref
    return tCIDXML::TEntitySrcRef(pxesRet);
}





// ---------------------------------------------------------------------------
//  CLASS: TMEngXMLTreeAnchorVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngXMLTreeAnchorVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngXMLTreeAnchorVal::
TMEngXMLTreeAnchorVal(  const   TString&                strName
                        , const tCIDLib::TCard2         c2Id
                        , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_c4Index(kCIDLib::c4MaxCard)
    , m_c4ParseId(0)
    , m_c4ParserId(0)
    , m_pxtnodeValue(nullptr)
{
}

TMEngXMLTreeAnchorVal::~TMEngXMLTreeAnchorVal()
{
}


// ---------------------------------------------------------------------------
//  TMEngXMLTreeAnchorVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngXMLTreeAnchorVal::
bDbgFormat(         TTextOutStream&     strmTarget
            , const TMEngClassInfo&
            , const tCIDMacroEng::EDbgFmts eFormat
            , const tCIDLib::ERadices
            , const TCIDMacroEngine&    ) const
{
    // We don't have any formattable info currently
    return kCIDLib::False;
}


tCIDLib::TVoid
TMEngXMLTreeAnchorVal::CopyFrom(const   TMEngClassVal&      mecvToCopy
                                ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);

    const TMEngXMLTreeAnchorVal& mecvSrc = static_cast<const TMEngXMLTreeAnchorVal&>
    (
        mecvToCopy
    );

    m_c4Index = mecvSrc.m_c4Index;
    m_c4ParseId = mecvSrc.m_c4ParseId;
    m_c4ParserId = mecvSrc.m_c4ParserId;
    m_pxtnodeValue = mecvSrc.m_pxtnodeValue;
}


// ---------------------------------------------------------------------------
//  TMEngXMLTreeAnchorVal: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Makes sure that this anchor is valid for the calling parser and parse
tCIDLib::TBoolean
TMEngXMLTreeAnchorVal::bIsValid(const   tCIDLib::TCard4 c4ParserId
                                , const tCIDLib::TCard4 c4ParseId) const
{
    return ((m_pxtnodeValue != 0)
            && (m_c4ParserId == c4ParserId)
            && (m_c4ParseId == c4ParseId));
}


//
//  Moves the passed anchor, which must be a child of our element, to the
//  next or previous child.
//
tCIDLib::TBoolean TMEngXMLTreeAnchorVal::
bNextPrevChild(         TMEngXMLTreeAnchorVal&  mecvToUpdate
                , const tCIDLib::TBoolean       bNext)
{
    //
    //  Get the index of the incoming anchor and make sure our nth child
    //  is him. Otherwise, the caller did wrong. It can't be max card, since
    //  only the root would have no parent and therefore cannot ever be the
    //  incoming child anchor.
    //
    CIDAssert(m_pxtnodeValue != nullptr, L"The parent anchor is invalid");
    tCIDLib::TCard4 c4Index = mecvToUpdate.m_c4Index;
    CIDAssert(c4Index != kCIDLib::c4MaxCard, L"The child anchor is invalid");
    {
        const TXMLTreeElement& xtnodeCur = m_pxtnodeValue->xtnodeChildAtAsElement(c4Index);
        CIDAssert
        (
            &xtnodeCur == mecvToUpdate.m_pxtnodeValue
            , L"The child anchor is not a child of the provided parent anchor"
        );
    }

    // Looks ok, so let's try to move to the next or previous node
    const tCIDLib::TCard4 c4Count = m_pxtnodeValue->c4ChildCount();
    const TXMLTreeElement* pxtnodeNew = nullptr;
    if (bNext)
    {
        c4Index++;
        while (c4Index < c4Count)
        {
            const TXMLTreeNode& xtnodeCur = m_pxtnodeValue->xtnodeChildAt(c4Index);
            if (xtnodeCur.eType() == tCIDXML::ENodeTypes::Element)
            {
                pxtnodeNew = &static_cast<const TXMLTreeElement&>(xtnodeCur);
                break;
            }
            c4Index++;
        }
    }
     else
    {
        tCIDLib::TInt4 i4Index = tCIDLib::TInt4(c4Index);
        i4Index--;
        while (i4Index >= 0)
        {
            const TXMLTreeNode& xtnodeCur = m_pxtnodeValue->xtnodeChildAt(i4Index);
            if (xtnodeCur.eType() == tCIDXML::ENodeTypes::Element)
            {
                pxtnodeNew = &static_cast<const TXMLTreeElement&>(xtnodeCur);
                break;
            }
            i4Index--;
        }

        if (pxtnodeNew)
            c4Index = tCIDLib::TCard4(i4Index);
    }

    // If we got it, set up the anchor for the new stuff
    if (pxtnodeNew)
    {
        mecvToUpdate.SetAnchorInfo(m_c4ParserId, m_c4ParseId, pxtnodeNew, c4Index);
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// Reset us back to scratch
tCIDLib::TVoid TMEngXMLTreeAnchorVal::Reset()
{
    m_c4Index = kCIDLib::c4MaxCard;
    m_c4ParseId = 0;
    m_c4ParserId = 0;
    m_pxtnodeValue = 0;
}


// Set up the anchor info
tCIDLib::TVoid
TMEngXMLTreeAnchorVal::SetAnchorInfo(const  tCIDLib::TCard4         c4ParserId
                                    , const tCIDLib::TCard4         c4ParseId
                                    , const TXMLTreeElement* const  pxtnodeValue
                                    , const tCIDLib::TCard4         c4Index)
{
    m_c4Index = c4Index;
    m_c4ParseId = c4ParseId;
    m_c4ParserId = c4ParserId;
    m_pxtnodeValue = pxtnodeValue;
}


// Give access to our element node
const TXMLTreeElement& TMEngXMLTreeAnchorVal::xtnodeValue() const
{
    CIDAssert(m_pxtnodeValue != 0, L"The XML anchor node is zero");
    return *m_pxtnodeValue;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngXMLTreeAnchorInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngXMLTreeAnchorInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngXMLTreeAnchorInfo::strPath()
{
    return CIDMacroEng_XMLTreeParserClasses::strXMLTreeAnchorClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngXMLTreeAnchorInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngXMLTreeAnchorInfo::TMEngXMLTreeAnchorInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_XMLTreeParserClasses::strXMLTreeAnchor
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2MethId_DefCtor(kCIDMacroEng::c2BadId)
{
}

TMEngXMLTreeAnchorInfo::~TMEngXMLTreeAnchorInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngXMLTreeAnchorInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngXMLTreeAnchorInfo::Init(TCIDMacroEngine& meOwner)
{
    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.XMLTreeAnchor"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal* TMEngXMLTreeAnchorInfo::
pmecvMakeStorage(const  TString&                strName
                ,       TCIDMacroEngine&        meOwner
                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngXMLTreeAnchorVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngXMLTreeAnchorInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngXMLTreeAnchorInfo::bInvokeMethod(          TCIDMacroEngine&    meOwner
                                        , const TMEngMethodInfo&    methiTarget
                                        ,       TMEngClassVal&      mecvInstance)
{
    TMEngXMLTreeAnchorVal& mecvActual = static_cast<TMEngXMLTreeAnchorVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_DefCtor)
    {
        // Just reset it
        mecvActual.Reset();
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngXMLTreeParserVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngXMLTreeParserVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngXMLTreeParserVal::
TMEngXMLTreeParserVal(  const   TString&                strName
                        , const tCIDLib::TCard2         c2Id
                        , const tCIDMacroEng::EConstTypes  eConst
                        ,       TCIDMacroEngine* const  pmeOwner) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_c4ParseId(0)
    , m_c4ParserId(0)
    , m_eOpts(tCIDXML::EParseOpts::IgnoreDTD)
    , m_eFlags(tCIDXML::EParseFlags::JustContent)
    , m_xtprsValue(pmeOwner)
{

}

TMEngXMLTreeParserVal::~TMEngXMLTreeParserVal()
{
}


// ---------------------------------------------------------------------------
//  TMEngXMLTreeParserVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngXMLTreeParserVal::
bDbgFormat(         TTextOutStream&     strmTarget
            , const TMEngClassInfo&
            , const tCIDMacroEng::EDbgFmts eFormat
            , const tCIDLib::ERadices
            , const TCIDMacroEngine&    ) const
{
    // We don't have any formattable info currently
    return kCIDLib::False;
}



// ---------------------------------------------------------------------------
//  TMEngXMLTreeParserVal: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Validates the passed anchor against our ids, and it makes sure that the
//  anchor is also pointing to something.
//
tCIDLib::TBoolean
TMEngXMLTreeParserVal::bCheckAnchor(const TMEngXMLTreeAnchorVal& mecvAnchor) const
{
    // Pass our ids on to the anchor
    return mecvAnchor.bIsValid(m_c4ParserId, m_c4ParseId);
}


//
//  This is a helper that the info object calls to get a child anchor from
//  the Nth child of another anchor. The only thing that can really go wrong
//  is that the index is too large, so we return a boolean to indicate that.
//
tCIDLib::TBoolean
TMEngXMLTreeParserVal::bGetChildAnchor( const   TMEngXMLTreeAnchorVal&  mecvParent
                                        , const tCIDLib::TCard4         c4Index
                                        ,       TMEngXMLTreeAnchorVal&  mecvToFill) const
{
    // Reset the anchor whether we work or not
    mecvToFill.Reset();

    const TXMLTreeDocument& xtdocTar = m_xtprsValue.xtdocThis();
    try
    {
        tCIDLib::TCard4 c4At;
        const TXMLTreeElement& xtnodePar = mecvParent.xtnodeValue();
        const TXMLTreeElement& xtnodeNew = xtnodePar.xtnodeNthElement(c4Index, c4At);

        // We found it, so
        mecvToFill.SetAnchorInfo(m_c4ParserId, m_c4ParseId, &xtnodeNew, c4At);
    }

    catch(...)
    {
        // There's no root set yet
        return kCIDLib::False;
    }

    // We are good, so set it up
    return kCIDLib::True;
}


//
//  This is a helper that the info object uses when they want to set an
//  anchor for the root element, which will be very common. The ony thing
//  that can go wrong is that there's no document set yet, so we return a
//  boolean to indicate that.
//
tCIDLib::TBoolean
TMEngXMLTreeParserVal::bGetRootAnchor(TMEngXMLTreeAnchorVal& mecvToFill) const
{
    // Reset the anchor whether we work or not
    mecvToFill.Reset();

    const TXMLTreeDocument& xtdocTar = m_xtprsValue.xtdocThis();
    try
    {
        // If no root has been set yet, this will throw
        mecvToFill.SetAnchorInfo
        (
            m_c4ParserId, m_c4ParseId, &xtdocTar.xtnodeRoot(), kCIDLib::c4MaxCard
        );
    }

    catch(...)
    {
        // There's no root set yet
        return kCIDLib::False;
    }

    // We are good, so set it up
    return kCIDLib::True;
}


// We attempt to parse the passed entry source
tCIDLib::TCard4
TMEngXMLTreeParserVal::c4Parse(tCIDXML::TEntitySrcRef& esrToParse)
{
    m_c4ParseId++;
    tCIDLib::TCard4 c4ErrCnt = 0;
    if (!m_xtprsValue.bParseRootEntity(esrToParse, m_eOpts, m_eFlags))
        c4ErrCnt = m_xtprsValue.colErrors().c4ElemCount();
    return c4ErrCnt;
}


//
//  Called from the default CML ctor to get ut so initialize our ids.
//  We take the next available unique id and start the parse id at 1.
//
tCIDLib::TVoid TMEngXMLTreeParserVal::InitIds(TCIDMacroEngine& meOwner)
{
    m_c4ParseId = 1;
    m_c4ParserId = meOwner.c4NextUniqueId();
}


//
//  Find a given path by the string, if present. If not it returns null.
//  If found, it gives back the index within the parent node (assuming the
//  path isn't the root node.)
//
const TXMLTreeElement*
TMEngXMLTreeParserVal::pxtnodeFindPath( const   TString&            strPath
                                        ,       tCIDLib::TCard4&    c4Index) const
{
    // Just pass it on to the parser
    return m_xtprsValue.pxtnodeFindPath(strPath, c4Index);
}


//
//  This is a helper that the info object uses when they want to set an
//  anchor based on a path. We throw if it's not found. The anchor will
//  be reset if we fail.
//
tCIDLib::TVoid TMEngXMLTreeParserVal::
SetAnchor(const TString& strPath, TMEngXMLTreeAnchorVal& mecvToSet) const
{
    // Reset the anchor whether we work or not
    mecvToSet.Reset();

    // We use the version that throws if it fails to find the target element
    tCIDLib::TCard4 c4Index;
    const TXMLTreeElement& xtnodeFind = m_xtprsValue.xtnodeFindPath
    (
        strPath, c4Index
    );
    mecvToSet.SetAnchorInfo(m_c4ParserId, m_c4ParseId, &xtnodeFind, c4Index);
}


tCIDLib::TVoid
TMEngXMLTreeParserVal::SetAnchorInfo(const  TXMLTreeElement* const  pxtnodeToSet
                                    , const tCIDLib::TCard4         c4Index
                                    ,       TMEngXMLTreeAnchorVal&  mecvToSet) const
{
    mecvToSet.SetAnchorInfo(m_c4ParserId, m_c4ParseId, pxtnodeToSet, c4Index);
}


// Set the default options
tCIDLib::TVoid TMEngXMLTreeParserVal::SetDefOptions()
{
    m_eOpts = tCIDXML::EParseOpts::IgnoreDTD;
    m_eFlags = tCIDXML::EParseFlags::JustContent;
}


// Set or clear the parse options on this parser object
tCIDLib::TVoid
TMEngXMLTreeParserVal::SetOption(const  tCIDXML::EParseOpts eOption
                                , const tCIDLib::TBoolean   bToSet)
{
    if (bToSet)
        m_eOpts = tCIDLib::eOREnumBits(m_eOpts, eOption);
    else
        m_eOpts = tCIDLib::eClearEnumBits(m_eOpts, eOption);
}


// Provide read access to the root document object
const TXMLTreeDocument& TMEngXMLTreeParserVal::xtdocThis() const
{
    return m_xtprsValue.xtdocThis();
}


// Provide access to the parse object
const TXMLTreeParser& TMEngXMLTreeParserVal::xtprsValue() const
{
    return m_xtprsValue;
}

TXMLTreeParser& TMEngXMLTreeParserVal::xtprsValue()
{
    return m_xtprsValue;
}





// ---------------------------------------------------------------------------
//  CLASS: TMEngXMLTreeParserInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngXMLTreeParserInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngXMLTreeParserInfo::strPath()
{
    return CIDMacroEng_XMLTreeParserClasses::strXMLTreeParserClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngXMLTreeParserInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngXMLTreeParserInfo::TMEngXMLTreeParserInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_XMLTreeParserClasses::strXMLTreeParser
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2EnumId_Errors(kCIDMacroEng::c2BadId)
    , m_c2EnumId_NodeTypes(kCIDMacroEng::c2BadId)
    , m_c2MethId_ChildExists(kCIDMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kCIDMacroEng::c2BadId)
    , m_c2MethId_FindChildByName(kCIDMacroEng::c2BadId)
    , m_c2MethId_FormatTo(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetAnchor(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetAttribute(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetAttribute2(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetChildAnchor(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetChildText(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetElemName(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetErrorAt(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetRootAnchor(kCIDMacroEng::c2BadId)
    , m_c2MethId_NextPrevChild(kCIDMacroEng::c2BadId)
    , m_c2MethId_ParseBuf(kCIDMacroEng::c2BadId)
    , m_c2MethId_ParseFile(kCIDMacroEng::c2BadId)
    , m_c2MethId_ParseString(kCIDMacroEng::c2BadId)
    , m_c2MethId_SetDefOptions(kCIDMacroEng::c2BadId)
    , m_c2MethId_SetOption(kCIDMacroEng::c2BadId)
    , m_c2TypeId_Anchor(kCIDMacroEng::c2BadId)
    , m_c4ErrBadAnchor(kCIDLib::c4MaxCard)
    , m_c4ErrBadErrIndex(kCIDLib::c4MaxCard)
    , m_c4ErrChildNotFound(kCIDLib::c4MaxCard)
    , m_c4ErrElemNotFound(kCIDLib::c4MaxCard)
    , m_c4ErrParseFailed(kCIDLib::c4MaxCard)
    , m_c4ErrPathLookup(kCIDLib::c4MaxCard)
    , m_c4ErrPrint(kCIDLib::c4MaxCard)
    , m_pmeciErrors(0)
    , m_pmeciNodeTypes(0)
    , m_pmeciParseOpts(0)
    , m_pmeciStream(0)
{
    // Force imports of any classes we need to reference
    bAddClassImport(CIDMacroEng_XMLTreeParserClasses::strXMLTreeAnchorClassPath);
    bAddClassImport(TMEngTextOutStreamInfo::strPath());
}

TMEngXMLTreeParserInfo::~TMEngXMLTreeParserInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngXMLTreeParserInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngXMLTreeParserInfo::Init(TCIDMacroEngine& meOwner)
{
    // An enum for our errors
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"XMLTreeParserErrs"
            , strClassPath()
            , L"MEng.Enum"
            , 10
        );
        m_c4ErrAttrNotFound  = m_pmeciErrors->c4AddEnumItem(L"AttrNotFound", L"The element had no attribute named %(1)");
        m_c4ErrBadAnchor = m_pmeciErrors->c4AddEnumItem(L"BadAnchor", L"The XML Tree anchor is not valid");
        m_c4ErrBadErrIndex = m_pmeciErrors->c4AddEnumItem(L"BadIndex", L"%(1) is an invalid error index");
        m_c4ErrChildNotFound = m_pmeciErrors->c4AddEnumItem(L"ChildNotFound", L"The indicated child element was not found");
        m_c4ErrElemNotFound = m_pmeciErrors->c4AddEnumItem(L"ElemNotFound", L"Element node '%(1)' was not found in the document");
        m_c4ErrNoDocument = m_pmeciErrors->c4AddEnumItem(L"NoDocument", L"The parser contains no active document");
        m_c4ErrParseFailed = m_pmeciErrors->c4AddEnumItem(L"ParseFailed", TString::strEmpty());
        m_c4ErrPathLookup = m_pmeciErrors->c4AddEnumItem(L"PathLookup", TString::strEmpty());
        m_c4ErrBadEntityPath = m_pmeciErrors->c4AddEnumItem(L"BadEntityPath", L"'%(1)' is not a valid file path");
        m_c4ErrPrint = m_pmeciErrors->c4AddEnumItem(L"PrintErr", TString::strEmpty());

        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2EnumId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    // An enum for the node types
    {
        m_pmeciNodeTypes = new TMEngEnumInfo
        (
            meOwner
            , L"XMLNodeTypes"
            , strClassPath()
            , L"MEng.Enum"
            , 5
        );
        m_pmeciNodeTypes->c4AddEnumItem(L"Text", L"Text", tCIDXML::ENodeTypes::Text);
        m_pmeciNodeTypes->c4AddEnumItem(L"Element", L"Element", tCIDXML::ENodeTypes::Element);
        m_pmeciNodeTypes->BaseClassInit(meOwner);
        m_c2EnumId_NodeTypes = meOwner.c2AddClass(m_pmeciNodeTypes);
        bAddNestedType(m_pmeciNodeTypes->strClassPath());
    }

    // An enum for the parsing options
    {
        m_pmeciParseOpts = new TMEngEnumInfo
        (
            meOwner
            , L"XMLTreeOpts"
            , strClassPath()
            , L"MEng.Enum"
            , 5
        );
        m_pmeciParseOpts->c4AddEnumItem(L"Validate", L"Validate document", tCIDXML::EParseOpts::Validate);
        m_pmeciParseOpts->c4AddEnumItem(L"Namespaces", L"Enable namespaces", tCIDXML::EParseOpts::Namespaces);
        m_pmeciParseOpts->c4AddEnumItem(L"IgnoreDTD", L"Ignore DTD", tCIDXML::EParseOpts::IgnoreDTD);
        m_pmeciParseOpts->c4AddEnumItem(L"IgnoreBadChars", L"Ignore bad characters", tCIDXML::EParseOpts::IgnoreBadChars);
        m_pmeciParseOpts->BaseClassInit(meOwner);
        meOwner.c2AddClass(m_pmeciParseOpts);
        bAddNestedType(m_pmeciParseOpts->strClassPath());
    }

    // Look up imported classes we need
    m_c2TypeId_Anchor = meOwner.c2FindClassId
    (
        CIDMacroEng_XMLTreeParserClasses::strXMLTreeAnchorClassPath
    );
    m_pmeciStream = meOwner.pmeciFindAs<TMEngTextOutStreamInfo>
    (
        TMEngTextOutStreamInfo::strPath(), kCIDLib::True
    );


    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.XMLTreeParser"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Check to see if a child exists. If so, returns index within parent
    {
        TMEngMethodInfo methiNew
        (
            L"ChildExists"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ParAnchor", m_c2TypeId_Anchor);
        methiNew.c2AddInParm(L"ToFind", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"FoundAt", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ChildExists = c2AddMethodInfo(methiNew);
    }

    // Find a child element by name
    {
        TMEngMethodInfo methiNew
        (
            L"FindChildByName"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ParAnchor", m_c2TypeId_Anchor);
        methiNew.c2AddInParm(L"ToFind", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"StartAt", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddOutParm(L"FoundAt", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddOutParm(L"ToFill", m_c2TypeId_Anchor);
        m_c2MethId_FindChildByName = c2AddMethodInfo(methiNew);
    }

    // Format out the content in an anchor to a stream
    {
        TMEngMethodInfo methiNew
        (
            L"FormatTo"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor);
        methiNew.c2AddInOutParm(L"TarStream", m_pmeciStream->c2Id());
        m_c2MethId_FormatTo = c2AddMethodInfo(methiNew);
    }

    // Get an anchor for a given node via it's XPath
    {
        TMEngMethodInfo methiNew
        (
            L"GetAnchor"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Path", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"Anchor", m_c2TypeId_Anchor);
        m_c2MethId_GetAnchor = c2AddMethodInfo(methiNew);
    }

    //
    //  Get a named attribute of the indicated element. The return indicates
    //  whether it was explicitly provided or not.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"GetAttribute"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor);
        methiNew.c2AddInParm(L"AttrName", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"Value", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetAttribute = c2AddMethodInfo(methiNew);
    }

    // Same as above but returns true/false to indicate if it was found or not
    {
        TMEngMethodInfo methiNew
        (
            L"GetAttribute2"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor);
        methiNew.c2AddInParm(L"AttrName", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"Value", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetAttribute2 = c2AddMethodInfo(methiNew);
    }

    // Get an anchor for a child of a given node
    {
        TMEngMethodInfo methiNew
        (
            L"GetChildAnchor"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor) ;
        methiNew.c2AddInParm(L"ChildIndex", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddOutParm(L"NewAnchor", m_c2TypeId_Anchor);
        m_c2MethId_GetChildAnchor = c2AddMethodInfo(methiNew);
    }

    // Get all the accumulated text under the indicated anchor element
    {
        TMEngMethodInfo methiNew
        (
            L"GetChildText"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor) ;
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetChildText = c2AddMethodInfo(methiNew);
    }

    // Get the name of the element represented by an anchor
    {
        TMEngMethodInfo methiNew
        (
            L"GetElemName"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor);
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetElemName = c2AddMethodInfo(methiNew);
    }

    // Get an error after a failed parse
    {
        TMEngMethodInfo methiNew
        (
            L"GetErrorAt"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Index", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddOutParm(L"EntityName", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"Line", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddOutParm(L"Column", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddOutParm(L"Text", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetErrorAt = c2AddMethodInfo(methiNew);
    }

    // Get an anchor for the root element
    {
        TMEngMethodInfo methiNew
        (
            L"GetRootAnchor"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"Anchor", m_c2TypeId_Anchor);
        m_c2MethId_GetRootAnchor = c2AddMethodInfo(methiNew);
    }

    //
    //  Get the next or previous child anchor of a parent anchor. Returns
    //  true if it was above to move in the indicated direction, else
    //  false when it hits the beginning/end.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"NextPrevChild"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ParAnchor", m_c2TypeId_Anchor) ;
        methiNew.c2AddInParm(L"Next", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddInOutParm(L"NewAnchor", m_c2TypeId_Anchor);
        m_c2MethId_NextPrevChild = c2AddMethodInfo(methiNew);
    }

    // Parse from a memory buffer, file, or string
    {
        TMEngMethodInfo methiNew
        (
            L"ParseBuf"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Buffer", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInParm(L"BytesToUse", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ParseBuf = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ParseFile"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"File", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_ParseFile = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ParseString"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Content", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_ParseString = c2AddMethodInfo(methiNew);
    }

    // Set the parser to default options
    {
        TMEngMethodInfo methiNew
        (
            L"SetDefOptions"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_SetDefOptions = c2AddMethodInfo(methiNew);
    }

    // Set/clear the parsing options
    {
        TMEngMethodInfo methiNew
        (
            L"SetOption"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Option", m_pmeciParseOpts->c2Id());
        methiNew.c2AddInParm(L"State", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_SetOption = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal* TMEngXMLTreeParserInfo::
pmecvMakeStorage(const  TString&                strName
                ,       TCIDMacroEngine&        meOwner
                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngXMLTreeParserVal(strName, c2Id(), eConst, &meOwner);
}


// ---------------------------------------------------------------------------
//  TMEngXMLTreeParserInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngXMLTreeParserInfo::bInvokeMethod(          TCIDMacroEngine&    meOwner
                                        , const TMEngMethodInfo&    methiTarget
                                        ,       TMEngClassVal&      mecvInstance)
{
    TMEngXMLTreeParserVal& mecvActual = static_cast<TMEngXMLTreeParserVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_DefCtor)
    {
        // Set up his ids
        mecvActual.InitIds(meOwner);
    }
     else if (c2MethId == m_c2MethId_ChildExists)
    {
        TMEngXMLTreeAnchorVal& mecvAnchor
        (
            meOwner.mecvStackAtAs<TMEngXMLTreeAnchorVal>(c4FirstInd)
        );
        CheckAnchor(meOwner, mecvActual, mecvAnchor);
        const TXMLTreeElement& xtnodeTar = mecvAnchor.xtnodeValue();

        tCIDLib::TCard4 c4At;
        const TXMLTreeElement* pxtnodeFind = xtnodeTar.pxtnodeFindElement
        (
            meOwner.strStackValAt(c4FirstInd + 1), 0, c4At
        );

        // Set the return value, and if we found it, give back the index
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        if (mecvRet.bValue(pxtnodeFind != nullptr))
            meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 2).c4Value(c4At);
    }
     else if (c2MethId == m_c2MethId_FindChildByName)
    {
        TMEngXMLTreeAnchorVal& mecvAnchor
        (
            meOwner.mecvStackAtAs<TMEngXMLTreeAnchorVal>(c4FirstInd)
        );
        CheckAnchor(meOwner, mecvActual, mecvAnchor);
        const TXMLTreeElement& xtnodeTar = mecvAnchor.xtnodeValue();

        tCIDLib::TCard4 c4At;
        const TXMLTreeElement* pxtnodeFind = xtnodeTar.pxtnodeFindElement
        (
            meOwner.strStackValAt(c4FirstInd + 1)
            , meOwner.c4StackValAt(c4FirstInd + 2)
            , c4At
        );

        // Set the return info
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        if (mecvRet.bValue(pxtnodeFind != nullptr))
        {
            mecvRet.bValue(kCIDLib::True);
            mecvActual.SetAnchorInfo
            (
                pxtnodeFind
                , c4At
                , meOwner.mecvStackAtAs<TMEngXMLTreeAnchorVal>(c4FirstInd + 4)
            );
            meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 3).c4Value(c4At);
        }
    }
     else if (c2MethId == m_c2MethId_FormatTo)
    {
        TMEngXMLTreeAnchorVal& mecvAnchor
        (
            meOwner.mecvStackAtAs<TMEngXMLTreeAnchorVal>(c4FirstInd)
        );
        CheckAnchor(meOwner, mecvActual, mecvAnchor);
        const TXMLTreeElement& xtnodeTar = mecvAnchor.xtnodeValue();

        try
        {
            // Get the stream value object
            TMEngTextOutStreamVal& mecvTarget
                    = meOwner.mecvStackAtAs<TMEngTextOutStreamVal>(c4FirstInd + 1);
            xtnodeTar.PrintTo
            (
                mecvTarget.strmTarget(meOwner)
                , 4
                , tCIDXML::EPrintFmts::Pretty
                , tCIDXML::EPrintOpts::Escape
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrPrint, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_GetAnchor)
    {
        const TString& strPath = meOwner.strStackValAt(c4FirstInd);
        try
        {
            // Ask the parser value object to set up the anchor for us
            TMEngXMLTreeAnchorVal& mecvAnchor
            (
                meOwner.mecvStackAtAs<TMEngXMLTreeAnchorVal>(c4FirstInd + 1)
            );
            mecvActual.SetAnchor(strPath, mecvAnchor);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrPathLookup, errToCatch);
        }
    }
     else if ((c2MethId == m_c2MethId_GetAttribute)
          ||  (c2MethId == m_c2MethId_GetAttribute2))
    {
        // We get an anchor and the name of an attribute
        TMEngXMLTreeAnchorVal& mecvAnchor
        (
            meOwner.mecvStackAtAs<TMEngXMLTreeAnchorVal>(c4FirstInd)
        );
        CheckAnchor(meOwner, mecvActual, mecvAnchor);
        const TXMLTreeElement& xtnodeTar = mecvAnchor.xtnodeValue();

        // Get the return value for use below
        TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);

        const TString& strAttr = meOwner.strStackValAt(c4FirstInd + 1);
        tCIDLib::TCard4 c4Index;
        if (!xtnodeTar.bAttrExists(strAttr, c4Index))
        {
            // Either throw or return failure
            if (c2MethId == m_c2MethId_GetAttribute)
                ThrowAnErr(meOwner, m_c4ErrAttrNotFound, strAttr);
            else
                mecvRet.bValue(kCIDLib::False);
        }
         else
        {
            // Get the value out into the caller's buffer
            const TXMLTreeAttr& xtattrVal = xtnodeTar.xtattrAt(c4Index);
            meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 2).strValue
            (
                xtattrVal.strValue()
            );

            //
            //  Either return whether it was explicitly defined, or that
            //  we found it successfully.
            //
            if (c2MethId == m_c2MethId_GetAttribute)
                mecvRet.bValue(xtattrVal.bExplicit());
            else
                mecvRet.bValue(kCIDLib::True);
        }
    }
     else if (c2MethId == m_c2MethId_GetChildAnchor)
    {
        // We get an anchor and an index of a child element
        TMEngXMLTreeAnchorVal& mecvAnchor
        (
            meOwner.mecvStackAtAs<TMEngXMLTreeAnchorVal>(c4FirstInd)
        );
        CheckAnchor(meOwner, mecvActual, mecvAnchor);
        const TXMLTreeElement& xtnodeTar = mecvAnchor.xtnodeValue();

        const tCIDLib::TCard4 c4Index(meOwner.c4StackValAt(c4FirstInd + 1));
        TMEngXMLTreeAnchorVal& mecvChildAnchor
        (
            meOwner.mecvStackAtAs<TMEngXMLTreeAnchorVal>(c4FirstInd + 2)
        );

        if (!mecvActual.bGetChildAnchor(mecvAnchor, c4Index, mecvChildAnchor))
            ThrowAnErr(meOwner, m_c4ErrChildNotFound);
    }
     else if (c2MethId == m_c2MethId_GetChildText)
    {
        // First we have an anchor, so get that and test it and get out the node
        TMEngXMLTreeAnchorVal& mecvAnchor
        (
            meOwner.mecvStackAtAs<TMEngXMLTreeAnchorVal>(c4FirstInd)
        );
        CheckAnchor(meOwner, mecvActual, mecvAnchor);
        const TXMLTreeElement& xtnodeTar = mecvAnchor.xtnodeValue();

        //
        //  Now let's build up all of the text in all of the text nodes
        //  that are immediate children of this node.
        //
        TString& strTar(meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 1).strValue());

        strTar.Clear();
        TTreeChildCursor cursChildren(xtnodeTar);

        if (cursChildren.bReset())
        {
            do
            {
                // See if it's a text node
                if (cursChildren.xtnodeCur().eType() == tCIDXML::ENodeTypes::Text)
                {
                    const TXMLTreeText& xtnodeText
                    (
                        static_cast<const TXMLTreeText&>(cursChildren.xtnodeCur())
                    );

                    // If not ignorable text, then add it
                    if (!xtnodeText.bIsIgnorable())
                        strTar.Append(xtnodeText.strText());
                }
            }   while(cursChildren.bNext());
        }
    }
     else if (c2MethId == m_c2MethId_GetElemName)
    {
        // We get an anchor and the name of an attribute
        TMEngXMLTreeAnchorVal& mecvAnchor
        (
            meOwner.mecvStackAtAs<TMEngXMLTreeAnchorVal>(c4FirstInd)
        );
        CheckAnchor(meOwner, mecvActual, mecvAnchor);
        const TXMLTreeElement& xtnodeTar = mecvAnchor.xtnodeValue();

        meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 1).strValue
        (
            xtnodeTar.strQName()
        );
    }
     else if (c2MethId == m_c2MethId_GetErrorAt)
    {
        // Get the error index
        const tCIDLib::TCard4 c4Index = meOwner.c4StackValAt(c4FirstInd);
        const TXMLTreeParser& xtprsVal = mecvActual.xtprsValue();

        if (c4Index >= xtprsVal.colErrors().c4ElemCount())
            ThrowAnErr(meOwner, m_c4ErrBadErrIndex, TCardinal(c4Index));

        const TXMLTreeParser::TErrInfo& erriAt = xtprsVal.colErrors()[c4Index];
        meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 1).strValue(erriAt.strSystemId());
        meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 2).c4Value(erriAt.c4Line());
        meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 3).c4Value(erriAt.c4Column());
        meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 4).strValue(erriAt.strText());
    }
     else if (c2MethId == m_c2MethId_GetRootAnchor)
    {
        TMEngXMLTreeAnchorVal& mecvAnchor
        (
            meOwner.mecvStackAtAs<TMEngXMLTreeAnchorVal>(c4FirstInd)
        );
        if (!mecvActual.bGetRootAnchor(mecvAnchor))
            ThrowAnErr(meOwner, m_c4ErrNoDocument);
    }
     else if (c2MethId == m_c2MethId_NextPrevChild)
    {
        // The parent and child anchor has to be valid
        TMEngXMLTreeAnchorVal& mecvParAnchor
        (
            meOwner.mecvStackAtAs<TMEngXMLTreeAnchorVal>(c4FirstInd)
        );
        CheckAnchor(meOwner, mecvActual, mecvParAnchor);

        TMEngXMLTreeAnchorVal& mecvChildAnchor
        (
            meOwner.mecvStackAtAs<TMEngXMLTreeAnchorVal>(c4FirstInd + 2)
        );
        CheckAnchor(meOwner, mecvActual, mecvChildAnchor);

        // The parent anchor can do the work for us
        TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue
        (
            mecvParAnchor.bNextPrevChild
            (
                mecvChildAnchor, meOwner.bStackValAt(c4FirstInd + 1)
            )
        );

    }
     else if ((c2MethId == m_c2MethId_ParseBuf)
          ||  (c2MethId == m_c2MethId_ParseFile)
          ||  (c2MethId == m_c2MethId_ParseString))
    {
        try
        {
            //
            //  The only difference between the methods is what type of
            //  data source we got. The parser takes an entity source ref
            //  either way.
            //
            tCIDXML::TEntitySrcRef esrToParse;
            if (c2MethId == m_c2MethId_ParseBuf)
            {
                const TMEngMemBufVal& mecvBuf = meOwner.mecvStackAtAs<TMEngMemBufVal>
                (
                    c4FirstInd
                );

                esrToParse.SetPointer
                (
                    new TMemBufEntitySrc
                    (
                        L"RootEntity"
                        , mecvBuf.mbufValue()
                        , meOwner.c4StackValAt(c4FirstInd + 1)
                    )
                );
            }
             else if (c2MethId == m_c2MethId_ParseFile)
            {
                //
                //  In this one case we have to expand the path, since the
                //  initial entity is being provided by us, so the parser
                //  does not try to map it.
                //
                const TString& strFile = meOwner.strStackValAt(c4FirstInd);

                TXMLEntitySrc* pxesRoot = 0;
                if (strFile.bStartsWith(L"http:"))
                {
                    pxesRoot = new TURLEntitySrc(strFile);
                }
                 else if (strFile.bStartsWith(L"file:"))
                {
                    // Get the path part of the URL out
                    TURL urlActual(strFile, tCIDSock::EQualified::Full);
                    TPathStr pathFull;
                    CheckPath
                    (
                        meOwner, urlActual.strPath(), pathFull
                    );
                    pxesRoot = new TMEngFileEntitySrc(urlActual.strPath(), pathFull);
                }
                 else
                {
                    // It's gotta be a file path
                    TPathStr pathFull;
                    CheckPath(meOwner, strFile, pathFull);
                    pxesRoot = new TMEngFileEntitySrc(strFile, pathFull);
                }
                esrToParse.SetPointer(pxesRoot);
            }
             else
            {
                const TMEngStringVal& mecvTxt = meOwner.mecvStackAtAs<TMEngStringVal>
                (
                    c4FirstInd
                );

                esrToParse.SetPointer
                (
                    new TMemBufEntitySrc(L"RootEntity", mecvTxt.strValue())
                );
            }

            mecvActual.xtprsValue().Reset(kCIDLib::True);
            tCIDLib::TCard4 c4ErrCnt = mecvActual.c4Parse(esrToParse);
            TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
            mecvRet.c4Value(c4ErrCnt);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrParseFailed, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_SetDefOptions)
    {
        mecvActual.SetDefOptions();
    }
     else if (c2MethId == m_c2MethId_SetOption)
    {
        TMEngEnumVal& mecvRet = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);

        mecvActual.SetOption
        (
            tCIDXML::EParseOpts(m_pmeciParseOpts->c4MapValue(mecvRet))
            , meOwner.bStackValAt(c4FirstInd + 1)
        );
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngXMLTreeParserInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------

// Checks the passed anchor to see if it's valid against the parser
tCIDLib::TVoid
TMEngXMLTreeParserInfo::CheckAnchor(        TCIDMacroEngine&        meOwner
                                    , const TMEngXMLTreeParserVal&  mecvParser
                                    , const TMEngXMLTreeAnchorVal&  mecvAnchor)
{
    if (!mecvParser.bCheckAnchor(mecvAnchor))
    {
        // Set the exception info on the engine
        meOwner.SetException
        (
            m_c2EnumId_Errors
            , strClassPath()
            , m_c4ErrBadAnchor
            , m_pmeciErrors->strPartialName(m_c4ErrBadAnchor)
            , L"The XML tree anchor is out of date"
            , meOwner.c4CurLine()
        );

        // And throw the excpetion that represents a macro level exception
        throw TExceptException();
    }
}


//
tCIDLib::TVoid
TMEngXMLTreeParserInfo::CheckPath(          TCIDMacroEngine&    meOwner
                                    , const TString&            strToCheck
                                    ,       TPathStr&           pathToFill)
{
    // For our purposes, it has to be fully qualified
    const tCIDLib::TCh chFirst = strToCheck.chFirst();
    if ((chFirst != kCIDLib::chBackSlash) && (chFirst != kCIDLib::chForwardSlash))
        ThrowAnErr(meOwner, m_c4ErrBadEntityPath, strToCheck);

    //
    //  Now let's expand it out. This will also normalize it and make sure
    //  it's legal (i.e. they haven't used .. entries to get up above the
    //  macro root.)
    //
    meOwner.ExpandFilePath(strToCheck, pathToFill);
}


// These throw errors for us
tCIDLib::TVoid
TMEngXMLTreeParserInfo::ThrowAnErr(         TCIDMacroEngine&    meOwner
                                    , const tCIDLib::TCard4     c4ToThrow)
{
    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , m_pmeciErrors->strTextValue(c4ToThrow)
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

tCIDLib::TVoid
TMEngXMLTreeParserInfo::ThrowAnErr(         TCIDMacroEngine&    meOwner
                                    , const tCIDLib::TCard4     c4ToThrow
                                    ,       TError&             errCaught)
{
    // If in verbose mode we'll log the C++ error
    if (!errCaught.bLogged() && facCIDMacroEng().bLogWarnings())
    {
        errCaught.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errCaught);
    }

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , errCaught.strErrText()
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}


tCIDLib::TVoid
TMEngXMLTreeParserInfo::ThrowAnErr(         TCIDMacroEngine&    meOwner
                                    , const tCIDLib::TCard4     c4ToThrow
                                    , const TString&            strText)
{
    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , strText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}


tCIDLib::TVoid
TMEngXMLTreeParserInfo::ThrowAnErr(         TCIDMacroEngine&    meOwner
                                    , const tCIDLib::TCard4     c4ToThrow
                                    , const MFormattable&       fmtblToken1)
{
    TString strText(m_pmeciErrors->strTextValue(c4ToThrow));
    strText.eReplaceToken(fmtblToken1, kCIDLib::chDigit1);

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , strText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}


tCIDLib::TVoid
TMEngXMLTreeParserInfo::ThrowAnErr(         TCIDMacroEngine&    meOwner
                                    , const tCIDLib::TCard4     c4ToThrow
                                    , const MFormattable&       fmtblToken1
                                    , const MFormattable&       fmtblToken2)
{
    TString strText(m_pmeciErrors->strTextValue(c4ToThrow));
    strText.eReplaceToken(fmtblToken1, kCIDLib::chDigit1);
    strText.eReplaceToken(fmtblToken2, kCIDLib::chDigit2);

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , strText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}



