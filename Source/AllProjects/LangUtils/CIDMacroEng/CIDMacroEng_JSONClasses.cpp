//
// FILE NAME: CIDMacroEng_JSONClasses.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/17/2005
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
//  value classes, which implement the macro level JSON parser classes.
//  classes.
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
RTTIDecls(TMEngJSONAnchorVal,TMEngClassVal)
RTTIDecls(TMEngJSONAnchorInfo,TMEngClassInfo)
RTTIDecls(TMEngJSONParserVal,TMEngClassVal)
RTTIDecls(TMEngJSONParserInfo,TMEngClassInfo)



// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace
{
    namespace CIDMacroEng_NetClasses
    {
        // -----------------------------------------------------------------------
        //  The names for the types that we support here. Each derivative has to
        //  be able to return strings that contain its name and full name.
        // -----------------------------------------------------------------------
        const TString   strJSONAnchor(L"JSONAnchor");
        const TString   strJSONAnchorClassPath(L"MEng.System.Runtime.JSONAnchor");

        const TString   strJSONParser(L"JSONParser");
        const TString   strJSONParserClassPath(L"MEng.System.Runtime.JSONParser");
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngJSONAnchorVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngJSONAnchorVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngJSONAnchorVal::
TMEngJSONAnchorVal( const   TString&                strName
                    , const tCIDLib::TCard2         c2Id
                    , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_c4Index(kCIDLib::c4MaxCard)
    , m_c4ParseId(0)
    , m_c4ParserId(0)
    , m_pjprsnValue(0)
{
}

TMEngJSONAnchorVal::~TMEngJSONAnchorVal()
{
}


// ---------------------------------------------------------------------------
//  TMEngJSONAnchorVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngJSONAnchorVal::bDbgFormat(         TTextOutStream&     strmOut
                                , const TMEngClassInfo&
                                , const tCIDMacroEng::EDbgFmts eFormat
                                , const tCIDLib::ERadices
                                , const TCIDMacroEngine&    ) const
{
    //
    //  There's an issue here that the node pointer can be invalid and there's
    //  no way for the debugger to know and this can cause exeptions. We need some
    //  better way to deal with this.
    //
    return kCIDLib::False;

#if 0
    if (eFormat == tCIDMacroEng::EDbgFmts::Short)
        return kCIDLib::False;

    if (m_pjprsnValue)
    {
        // If there's a name set, show that
        if (!m_pjprsnValue->strName().bIsEmpty())
            strmOut << L"Name=" << m_pjprsnValue->strName() << kCIDLib::NewLn;

        // If we are a child within some other node, show the index
        if (m_c4Index != kCIDLib::c4MaxCard)
            strmOut << L"Index="  << m_c4Index << kCIDLib::NewLn;

        switch(m_pjprsnValue->eType())
        {
            case tCIDNet::EJSONVTypes::Array :
            case tCIDNet::EJSONVTypes::Object :
            {
                strmOut << L"Type=Array\n";

                //
                //  Get the value node as a container, common base class
                //  of arrays and objects.
                //
                const TJSONCont& jprsnCont = *static_cast<const TJSONCont*>
                (
                    m_pjprsnValue
                );
                strmOut << L"Child Count=" << jprsnCont.c4ValCount();
                break;
            }

            case tCIDNet::EJSONVTypes::False :
                strmOut << L"Type=[False]";
                break;

            case tCIDNet::EJSONVTypes::Null :
                strmOut << L"Type=[Null]";

                break;

            case tCIDNet::EJSONVTypes::Number :
                strmOut << L"Type=Number\n"
                        << L"Value=" << m_pjprsnValue->strValue();
                break;

            case tCIDNet::EJSONVTypes::String :
                strmOut << L"Type=String\n"
                        << L"Value=" << m_pjprsnValue->strValue();
                break;

            case tCIDNet::EJSONVTypes::True :
                strmOut << L"Type=[True]";
                break;

            default :
                break;
        };

        strmOut << kCIDLib::NewLn;
    }
     else
    {
        strmOut << L"[Not initialized]";
    }

    return kCIDLib::True;
#endif
}


tCIDLib::TVoid
TMEngJSONAnchorVal::CopyFrom(const  TMEngClassVal&      mecvToCopy
                            ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);

    const TMEngJSONAnchorVal& mecvSrc = static_cast<const TMEngJSONAnchorVal&>(mecvToCopy);
    m_c4Index      = mecvSrc.m_c4Index;
    m_c4ParseId    = mecvSrc.m_c4ParseId;
    m_c4ParserId   = mecvSrc.m_c4ParserId;
    m_pjprsnValue  = mecvSrc.m_pjprsnValue;
}


// ---------------------------------------------------------------------------
//  TMEngJSONAnchorVal: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Makes sure that this anchor is valid for the calling parser and parse
tCIDLib::TBoolean
TMEngJSONAnchorVal::bIsValid(const  tCIDLib::TCard4 c4ParserId
                            , const tCIDLib::TCard4 c4ParseId) const
{
    return ((m_pjprsnValue != 0)
            && (m_c4ParserId == c4ParserId)
            && (m_c4ParseId == c4ParseId));
}


//
//  Moves the passed anchor, which must be a child of our element, to the
//  next or previous child.
//
tCIDLib::TBoolean
TMEngJSONAnchorVal::bNextPrevChild(         TMEngJSONAnchorVal& mecvToUpdate
                                    , const tCIDLib::TBoolean   bNext)
{
    //
    //  Get the index of the incoming anchor and make sure our nth child
    //  is him. Otherwise, the caller did wrong. It can't be max card, since
    //  only the root would have no parent and therefore cannot ever be the
    //  incoming child anchor.
    //
    CIDAssert(m_pjprsnValue != 0, L"The parent anchor is invalid");
    tCIDLib::TCard4 c4Index = mecvToUpdate.m_c4Index;
    CIDAssert(c4Index != kCIDLib::c4MaxCard, L"The child anchor is invalid");

    //
    //  Make sure that this is a container type value anchor. If not, just
    //  return false now.
    //
    if (!m_pjprsnValue->bIsContType())
        return kCIDLib::False;

    // Looks ok, so get our node as the base container class
    const TJSONCont& jprsnCont = *static_cast<const TJSONCont*>(m_pjprsnValue);

    // Get the current child value
    const TJSONValue& jprsnCur = jprsnCont.jprsnValueAt(c4Index);
    CIDAssert
    (
        &jprsnCur == mecvToUpdate.m_pjprsnValue
        , L"The child anchor is not a child of the provided parent anchor"
    );

    // And let's try to move to the next or previous node
    const tCIDLib::TCard4 c4Count = jprsnCont.c4ValCount();
    const TJSONValue* pjprsnNew = 0;
    if (bNext)
    {
        c4Index++;
        if (c4Index < c4Count)
            pjprsnNew = &jprsnCont.jprsnValueAt(c4Index);
    }
     else
    {
        if (c4Index)
        {
            c4Index--;
            pjprsnNew = &jprsnCont.jprsnValueAt(c4Index);
        }
    }

    // If we got a new one, set up the anchor for the new stuff
    if (pjprsnNew)
    {
        mecvToUpdate.SetAnchorInfo(m_c4ParserId, m_c4ParseId, pjprsnNew, c4Index);
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// Give access to our node
const TJSONValue& TMEngJSONAnchorVal::jprsnValue() const
{
    CIDAssert(m_pjprsnValue != 0, L"The JSON anchor is not set");
    return *m_pjprsnValue;
}


// Reset us back to scratch
tCIDLib::TVoid TMEngJSONAnchorVal::Reset()
{
    m_c4Index = kCIDLib::c4MaxCard;
    m_c4ParseId = 0;
    m_c4ParserId = 0;
    m_pjprsnValue = 0;
}


// Set up the anchor info
tCIDLib::TVoid
TMEngJSONAnchorVal::SetAnchorInfo(  const   tCIDLib::TCard4     c4ParserId
                                    , const tCIDLib::TCard4     c4ParseId
                                    , const TJSONValue* const   pjprsnValue
                                    , const tCIDLib::TCard4     c4Index)
{
    m_c4Index = c4Index;
    m_c4ParseId = c4ParseId;
    m_c4ParserId = c4ParserId;
    m_pjprsnValue = pjprsnValue;
}






// ---------------------------------------------------------------------------
//  CLASS: TMEngJSONAnchorInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngJSONAnchorInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngJSONAnchorInfo::strPath()
{
    return CIDMacroEng_NetClasses::strJSONAnchorClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngJSONAnchorInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngJSONAnchorInfo::TMEngJSONAnchorInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_NetClasses::strJSONAnchor
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2MethId_DefCtor(kCIDMacroEng::c2BadId)
{
}

TMEngJSONAnchorInfo::~TMEngJSONAnchorInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngJSONAnchorInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngJSONAnchorInfo::Init(TCIDMacroEngine& meOwner)
{
    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.JSONAnchor"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal* TMEngJSONAnchorInfo::
pmecvMakeStorage(const  TString&                strName
                ,       TCIDMacroEngine&        meOwner
                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngJSONAnchorVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngJSONAnchorInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngJSONAnchorInfo::bInvokeMethod(         TCIDMacroEngine&    meOwner
                                    , const TMEngMethodInfo&    methiTarget
                                    ,       TMEngClassVal&      mecvInstance)
{
    TMEngJSONAnchorVal& mecvActual = static_cast<TMEngJSONAnchorVal&>(mecvInstance);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_DefCtor)
    {
        // Just reset the anchor, until it is set up
        mecvActual.Reset();
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}






// ---------------------------------------------------------------------------
//  CLASS: TMEngJSONParserVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngJSONParserVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngJSONParserVal::
TMEngJSONParserVal(  const  TString&                strName
                    , const tCIDLib::TCard2         c2Id
                    , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_c4ParseId(0)
    , m_c4ParserId(0)
    , m_pjprsValue(new TJSONParser)
    , m_pjprsnRoot(new TJSONObject(L"Root"))
{
}

TMEngJSONParserVal::~TMEngJSONParserVal()
{
    // Clean up the root node object
    try
    {
        delete m_pjprsnRoot;
    }

    catch(...)
    {
    }
    m_pjprsnRoot = 0;

    // Clean up the parser object
    try
    {
        delete m_pjprsValue;
    }

    catch(...)
    {
    }
    m_pjprsValue = 0;
}


// ---------------------------------------------------------------------------
//  TMEngJSONParserVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngJSONParserVal::
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
//  TMEngJSONParserVal: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Validates the passed anchor against our ids, and it makes sure that the
//  anchor is also pointing to something.
//
tCIDLib::TBoolean
TMEngJSONParserVal::bCheckAnchor(const TMEngJSONAnchorVal& mecvAnchor) const
{
    // Pass our ids on to the anchor
    return mecvAnchor.bIsValid(m_c4ParserId, m_c4ParseId);
}


//
//  This is a helper that the info object calls to get a child anchor from
//  the Nth child of another anchor. If it's a node type that supports child
//  values, or the index is bad, then we return false.
//
tCIDLib::TBoolean
TMEngJSONParserVal::bGetChildAnchor(const   TMEngJSONAnchorVal&  mecvParent
                                    , const tCIDLib::TCard4      c4Index
                                    ,       TMEngJSONAnchorVal&  mecvToFill) const
{
    // Reset the anchor whether we work or not
    mecvToFill.Reset();

    try
    {
        const TJSONValue& jprsnPar = mecvParent.jprsnValue();

        //
        //  Make sure that this is a container type value anchor. If not,
        //  just return false now.
        //
        if (!jprsnPar.bIsContType())
            return kCIDLib::False;

        // Cast to the container type and get the child value at index
        const TJSONCont& jprsnCont = static_cast<const TJSONCont&>(jprsnPar);
        const TJSONValue& jprsnNew = jprsnCont.jprsnValueAt(c4Index);

        // We found it, so
        mecvToFill.SetAnchorInfo(m_c4ParserId, m_c4ParseId, &jprsnNew, c4Index);
    }

    catch(...)
    {
        return kCIDLib::False;
    }

    // We are good, so set it up
    return kCIDLib::True;
}


//
//  This is a helper that the info object uses when they want to set an
//  anchor for the root element, which will be very common.
//
tCIDLib::TVoid
TMEngJSONParserVal::GetRootAnchor(TMEngJSONAnchorVal& mecvToFill) const
{
    mecvToFill.Reset();
    mecvToFill.SetAnchorInfo
    (
        m_c4ParserId, m_c4ParseId, m_pjprsnRoot, kCIDLib::c4MaxCard
    );
}


//
//  Called from the default CML ctor to get ut so initialize our ids.
//  We take the next available unique id and start the parse id at 1.
//
tCIDLib::TVoid TMEngJSONParserVal::InitIds(TCIDMacroEngine& meOwner)
{
    m_c4ParseId = 1;
    m_c4ParserId = meOwner.c4NextUniqueId();
}


// Provide access to the parse object
const TJSONParser& TMEngJSONParserVal::jprsValue() const
{
    return *m_pjprsValue;
}

TJSONParser& TMEngJSONParserVal::jprsValue()
{
    return *m_pjprsValue;
}


//
//  We attempt to parse the passed source. If successful, we store a
//  new root node. If it fails, we leave an empty node.
//
tCIDLib::TVoid TMEngJSONParserVal::Parse(TTextInStream& strmSrc)
{
    // Reset the current root, so we end up empty if we fail
    m_pjprsnRoot->Reset();

    m_c4ParseId++;
    TJSONValue* pjprnsNew = m_pjprsValue->pjprsnParse(strmSrc);

    //
    //  It worked, so store the new root. REmember the old one and delete it
    //  after we have the one safely stored.
    //
    TJSONValue* pjprnsOld = m_pjprsnRoot;
    m_pjprsnRoot = pjprnsNew;

    try
    {
        delete pjprnsOld;
    }

    catch(...)
    {
    }
}



tCIDLib::TVoid
TMEngJSONParserVal::Print(TTextOutStream& strmTar, const TMEngJSONAnchorVal& mecvStart)
{
    mecvStart.jprsnValue().FormatToText(strmTar, 0, 4);
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngJSONParserInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngJSONParserInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngJSONParserInfo::strPath()
{
    return CIDMacroEng_NetClasses::strJSONParserClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngJSONParserInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngJSONParserInfo::TMEngJSONParserInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_NetClasses::strJSONParser
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2EnumId_Errors(kCIDMacroEng::c2BadId)
    , m_c2EnumId_ValTypes(kCIDMacroEng::c2BadId)
    , m_c2MethId_CheckValue(kCIDMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kCIDMacroEng::c2BadId)
    , m_c2MethId_FindArray(kCIDMacroEng::c2BadId)
    , m_c2MethId_FindBoolValue(kCIDMacroEng::c2BadId)
    , m_c2MethId_FindCardValue(kCIDMacroEng::c2BadId)
    , m_c2MethId_FindFloatValue(kCIDMacroEng::c2BadId)
    , m_c2MethId_FindIntValue(kCIDMacroEng::c2BadId)
    , m_c2MethId_FindChild(kCIDMacroEng::c2BadId)
    , m_c2MethId_FindObject(kCIDMacroEng::c2BadId)
    , m_c2MethId_FindValue(kCIDMacroEng::c2BadId)
    , m_c2MethId_FindType(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetChildAt(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetChildCount(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetFirstChild(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetLastChild(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetName(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetNameAt(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetRootAnchor(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetType(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetTypeAt(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetValue(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetValueAt(kCIDMacroEng::c2BadId)
    , m_c2MethId_NextPrevChild(kCIDMacroEng::c2BadId)
    , m_c2MethId_ParseResponse(kCIDMacroEng::c2BadId)
    , m_c2MethId_ParseResponse2(kCIDMacroEng::c2BadId)
    , m_c2MethId_PrintToStream(kCIDMacroEng::c2BadId)
    , m_c2TypeId_Anchor(kCIDMacroEng::c2BadId)
    , m_c2TypeId_TextInStream(kCIDMacroEng::c2BadId)
    , m_c2TypeId_TextOutStream(kCIDMacroEng::c2BadId)
    , m_c4ErrBadAnchor(kCIDLib::c4MaxCard)
    , m_c4ErrBadIndex(kCIDLib::c4MaxCard)
    , m_c4ErrChildNotFound(kCIDLib::c4MaxCard)
    , m_c4ErrGetValAs(kCIDLib::c4MaxCard)
    , m_c4ErrNameNotFound(kCIDLib::c4MaxCard)
    , m_c4ErrNotACont(kCIDLib::c4MaxCard)
    , m_c4ErrNotAnArray(kCIDLib::c4MaxCard)
    , m_c4ErrNotAnObject(kCIDLib::c4MaxCard)
    , m_c4ErrNotASimpleVal(kCIDLib::c4MaxCard)
    , m_c4ErrPrintErr(kCIDLib::c4MaxCard)
    , m_c4ErrParseFailed(kCIDLib::c4MaxCard)
    , m_pmeciErrors(0)
    , m_pmeciValTypes(0)
{
    // Force an import of our anchor class and the input text stream
    bAddClassImport(CIDMacroEng_NetClasses::strJSONAnchorClassPath);
    bAddClassImport(TMEngTextInStreamInfo::strPath());
}

TMEngJSONParserInfo::~TMEngJSONParserInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngJSONParserInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngJSONParserInfo::Init(TCIDMacroEngine& meOwner)
{
    // An enum for our errors
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"JSONParserErrs"
            , strClassPath()
            , L"MEng.Enum"
            , 11
        );
        m_c4ErrBadAnchor = m_pmeciErrors->c4AddEnumItem(L"BadAnchor", L"The JSON anchor is not valid");
        m_c4ErrBadIndex = m_pmeciErrors->c4AddEnumItem(L"BadIndex", L"%(1) is not a valid child index for the parent node");
        m_c4ErrChildNotFound = m_pmeciErrors->c4AddEnumItem(L"ChildNotFound", L"The indicated child element was not found");
        m_c4ErrNameNotFound = m_pmeciErrors->c4AddEnumItem(L"NameNotFound", L"No child with the name '%(1)' was found");
        m_c4ErrNotACont = m_pmeciErrors->c4AddEnumItem(L"NotACont", L"The source anchor is not an array or object type");
        m_c4ErrNotAnArray = m_pmeciErrors->c4AddEnumItem(L"NotAnArray", L"The source anchor is not an array type");
        m_c4ErrNotAnObject = m_pmeciErrors->c4AddEnumItem(L"NotAnObj", L"The source anchor is not an object type");
        m_c4ErrNotASimpleVal = m_pmeciErrors->c4AddEnumItem(L"NotASimpleVal", L"The value is not a simple value type");
        m_c4ErrParseFailed = m_pmeciErrors->c4AddEnumItem(L"ParseFailed", TString::strEmpty());
        m_c4ErrGetValAs = m_pmeciErrors->c4AddEnumItem(L"GetValAs", TString::strEmpty());
        m_c4ErrPrintErr = m_pmeciErrors->c4AddEnumItem(L"PrintErr", TString::strEmpty());
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2EnumId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    // An enum for the JSON value types
    {
        m_pmeciValTypes = new TMEngEnumInfo
        (
            meOwner
            , L"JSONValTypes"
            , strClassPath()
            , L"MEng.Enum"
            , 8
        );

        m_pmeciValTypes->c4AddEnumItem(L"Array", L"Array", tCIDNet::EJSONVTypes::Array);
        m_pmeciValTypes->c4AddEnumItem(L"False", L"False", tCIDNet::EJSONVTypes::False);
        m_pmeciValTypes->c4AddEnumItem(L"Null", L"Null", tCIDNet::EJSONVTypes::Null);
        m_pmeciValTypes->c4AddEnumItem(L"Number", L"Number", tCIDNet::EJSONVTypes::Number);
        m_pmeciValTypes->c4AddEnumItem(L"Object", L"Object", tCIDNet::EJSONVTypes::Object);
        m_pmeciValTypes->c4AddEnumItem(L"String", L"String", tCIDNet::EJSONVTypes::String);
        m_pmeciValTypes->c4AddEnumItem(L"True", L"True", tCIDNet::EJSONVTypes::True);
        m_pmeciValTypes->BaseClassInit(meOwner);
        m_c2EnumId_ValTypes = meOwner.c2AddClass(m_pmeciValTypes);
        bAddNestedType(m_pmeciValTypes->strClassPath());
    }


    // Look up the anchor class
    m_c2TypeId_Anchor = meOwner.c2FindClassId
    (
        CIDMacroEng_NetClasses::strJSONAnchorClassPath
    );

    // Look up the text input and output stream classes
    m_c2TypeId_TextInStream = meOwner.c2FindClassId(TMEngTextInStreamInfo::strPath());
    m_c2TypeId_TextOutStream = meOwner.c2FindClassId(TMEngTextOutStreamInfo::strPath());

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.JSONParser"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }


    //
    //  A method to check whether a value is available under an object, and
    //  get it if so. If the source anchor isn't an object, it will throw,
    //  but otherise it just returns true or false to indicate if it is
    //  giving back a value or not.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"CheckValue"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor) ;
        methiNew.c2AddInParm(L"ToFind", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_CheckValue = c2AddMethodInfo(methiNew);
    }


    // Find an array with the indicated name
    {
        TMEngMethodInfo methiNew
        (
            L"FindArray"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor) ;
        methiNew.c2AddInParm(L"ToFind", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"NewAnchor", m_c2TypeId_Anchor);
        methiNew.c2AddInParm(L"ThrowIfNot", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_FindArray = c2AddMethodInfo(methiNew);
    }


    //
    //  These methods find simple values of a particular type, throwing if
    //  the value found is not of the indicated type. The caller provides a
    //  value to return if the value is not found, if he doesn't indicate to
    //  throw if not found.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"FindBoolValue"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor) ;
        methiNew.c2AddInParm(L"ToFind", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"ThrowIfNot", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddInParm(L"DefVal", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_FindBoolValue = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"FindCardValue"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor) ;
        methiNew.c2AddInParm(L"ToFind", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"ThrowIfNot", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddInParm(L"DefVal", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_FindCardValue = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"FindFloatValue"
            , tCIDMacroEng::EIntrinsics::Float8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor) ;
        methiNew.c2AddInParm(L"ToFind", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"ThrowIfNot", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddInParm(L"DefVal", tCIDMacroEng::EIntrinsics::Float8);
        m_c2MethId_FindFloatValue = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"FindIntValue"
            , tCIDMacroEng::EIntrinsics::Int4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor) ;
        methiNew.c2AddInParm(L"ToFind", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"ThrowIfNot", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddInParm(L"DefVal", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_FindIntValue = c2AddMethodInfo(methiNew);
    }


    //
    //  Find a child with the indicated name, starting at the passed
    //  index. We return the index we found it at. If the passed anchor
    //  is for anything but an object, this will just return false.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"FindChild"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor) ;
        methiNew.c2AddInParm(L"StartAt", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"ToFind", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"FoundAt", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddOutParm(L"NewAnchor", m_c2TypeId_Anchor);
        m_c2MethId_FindChild = c2AddMethodInfo(methiNew);
    }


    // Find an array with the indicated name
    {
        TMEngMethodInfo methiNew
        (
            L"FindObject"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor) ;
        methiNew.c2AddInParm(L"ToFind", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"NewAnchor", m_c2TypeId_Anchor);
        methiNew.c2AddInParm(L"ThrowIfNot", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_FindObject = c2AddMethodInfo(methiNew);
    }

    // Return the type of the child with the indicated name
    {
        TMEngMethodInfo methiNew
        (
            L"FindType"
            , m_c2EnumId_ValTypes
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor) ;
        methiNew.c2AddInParm(L"ToFind", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_FindType = c2AddMethodInfo(methiNew);
    }


    // Find a value with the indicated name and return the simple value
    {
        TMEngMethodInfo methiNew
        (
            L"FindValue"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor) ;
        methiNew.c2AddInParm(L"ToFind", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"ThrowIfNot", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_FindValue = c2AddMethodInfo(methiNew);
    }


    // Get a new anchor for the nth child of the passed anchor
    {
        TMEngMethodInfo methiNew
        (
            L"GetChildAt"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor) ;
        methiNew.c2AddInParm(L"ChildIndex", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddOutParm(L"NewAnchor", m_c2TypeId_Anchor);
        m_c2MethId_GetChildAt = c2AddMethodInfo(methiNew);
    }


    //
    //  Get the number of child nodes of the indicated anchor node. If
    //  it isn't a container type node, the return will always be zero.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"GetChildCount"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor) ;
        m_c2MethId_GetChildCount = c2AddMethodInfo(methiNew);
    }


    //
    //  Get the first child of the passed anchor. Returns false if there
    //  isn't one.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"GetFirstChild"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor) ;
        methiNew.c2AddOutParm(L"NewAnchor", m_c2TypeId_Anchor);
        m_c2MethId_GetFirstChild = c2AddMethodInfo(methiNew);
    }


    //
    //  Get the last child of the passed anchor. Returns false if there
    //  isn't one.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"GetLastChild"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor) ;
        methiNew.c2AddOutParm(L"NewAnchor", m_c2TypeId_Anchor);
        m_c2MethId_GetLastChild = c2AddMethodInfo(methiNew);
    }


    //
    //  Get the name of the node represented by an anchor. It may be
    //  empty if the node isn't a named one.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"GetName"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor);
        m_c2MethId_GetName = c2AddMethodInfo(methiNew);
    }

    //
    //  Get the name of the nth child of the passed anchor. If the anchor
    //  isn't a container type, it will cause an exception.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"GetNameAt"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor);
        methiNew.c2AddInParm(L"At", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetNameAt = c2AddMethodInfo(methiNew);
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


    // Get the value type of the passed anchor
    {
        TMEngMethodInfo methiNew
        (
            L"GetType"
            , m_c2EnumId_ValTypes
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor);
        m_c2MethId_GetType = c2AddMethodInfo(methiNew);
    }

    //
    //  Get the value type of the nth child of the passed anchor. If
    //  the anchor isn't a container node, it will cause an exception.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"GetTypeAt"
            , m_c2EnumId_ValTypes
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor);
        methiNew.c2AddInParm(L"At", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetTypeAt = c2AddMethodInfo(methiNew);
    }


    //
    //  Get the simple value of the node represented by the anchor. If
    //  it isn't a simple value node, then the value will be empty.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"GetValue"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor) ;
        m_c2MethId_GetValue = c2AddMethodInfo(methiNew);
    }


    //
    //  Get the simple value of the nth child of the node represented by
    //  the anchor. If it isn't a simple value node, then the value will
    //  be empty. If the anchor isn't a container node an error will
    //  occur.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"GetValueAt"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Anchor", m_c2TypeId_Anchor) ;
        methiNew.c2AddInParm(L"At", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetValueAt = c2AddMethodInfo(methiNew);
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
        methiNew.c2AddOutParm(L"NewAnchor", m_c2TypeId_Anchor);
        m_c2MethId_NextPrevChild = c2AddMethodInfo(methiNew);
    }

    // Parse from a text input stream
    {
        TMEngMethodInfo methiNew
        (
            L"ParseResponse"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInOutParm(L"InStream", m_c2TypeId_TextInStream);
        m_c2MethId_ParseResponse = c2AddMethodInfo(methiNew);
    }

    // Parse from a memory buffer input stream
    {
        TMEngMethodInfo methiNew
        (
            L"ParseResponse2"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"DataBuf", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInParm(L"ByteCnt", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Encoding", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_ParseResponse2 = c2AddMethodInfo(methiNew);
    }

    // Print the content to a text string
    {
        TMEngMethodInfo methiNew
        (
            L"PrintToStream"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"OutStream", tCIDMacroEng::EIntrinsics::TextOutStream);
        methiNew.c2AddInParm(L"StartAnchor", m_c2TypeId_Anchor) ;
        m_c2MethId_PrintToStream = c2AddMethodInfo(methiNew);
    }

}


TMEngClassVal* TMEngJSONParserInfo::
pmecvMakeStorage(const  TString&                strName
                ,       TCIDMacroEngine&        meOwner
                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngJSONParserVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngJSONParserInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngJSONParserInfo::bInvokeMethod(         TCIDMacroEngine&    meOwner
                                    , const TMEngMethodInfo&    methiTarget
                                    ,       TMEngClassVal&      mecvInstance)
{
    TMEngJSONParserVal& mecvActual = static_cast<TMEngJSONParserVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();


    if (c2MethId == m_c2MethId_CheckValue)
    {
        // We get an anchor to search
        TMEngJSONAnchorVal& mecvParent
        (
            meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd)
        );
        CheckAnchor(meOwner, mecvActual, mecvParent);

        // Make sure it's an object node. Throw if not
        const TJSONObject* pjprsnObj = pjprsnCheckIsObj
        (
            meOwner, mecvParent.jprsnValue(), kCIDLib::True
        );

        // See if it has the requested value or not
        tCIDLib::TCard4 c4FoundAt;
        const TJSONValue* pjprsnFnd = pjprsnObj->pjprsnFindVal
        (
            meOwner.strStackValAt(c4FirstInd + 1), 0, c4FoundAt
        );

        // Get the return value and set it
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(pjprsnFnd != 0);

        //
        //  If we found it, and it's a simple value, fill in the target string
        //  else clear it.
        //
        TMEngStringVal& mecvTar = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 2);
        if (pjprsnFnd && pjprsnFnd->bIsSimpleType())
            mecvTar.strValue(pjprsnFnd->strValue());
        else
            mecvTar.strValue(TString::strEmpty());
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        // Set up his ids
        mecvActual.InitIds(meOwner);
    }
     else if (c2MethId == m_c2MethId_FindArray)
    {
        // We get an anchor to search
        TMEngJSONAnchorVal& mecvParent
        (
            meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd)
        );
        CheckAnchor(meOwner, mecvActual, mecvParent);

        // Make sure it's a container node. Throw if not
        const TJSONCont* pjprsnCont = pjprsnCheckIsCont
        (
            meOwner, mecvParent.jprsnValue(), kCIDLib::True
        );

        // Find the child, either throw or get back null based on user request
        tCIDLib::TCard4 c4Index = 0;
        const TJSONValue* pjprsnFnd = pjprsnFind
        (
            meOwner
            , *pjprsnCont
            , meOwner.strStackValAt(c4FirstInd + 1)
            , c4Index
            , meOwner.bStackValAt(c4FirstInd + 3)
        );

        // Get the return value and target child anchor objects
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        TMEngJSONAnchorVal& mecvChildAnchor
        (
            meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd + 2)
        );

        //
        //  If we found it, and it's an array, then set up the target
        //  anchor. Else, we'll either throw or just return false.
        //
        if (pjprsnFnd
        &&  pjprsnCheckIsArray(meOwner, *pjprsnFnd, meOwner.bStackValAt(c4FirstInd + 3)))
        {

            if (!mecvActual.bGetChildAnchor(mecvParent, c4Index, mecvChildAnchor))
                ThrowAnErr(meOwner, m_c4ErrChildNotFound);
            mecvRet.bValue(kCIDLib::True);
        }
         else
        {
            //
            //  Indicate it wasn't an array or wasn't found. Make sure the
            //  anchor is reset.
            //
            mecvRet.bValue(kCIDLib::False);
            mecvChildAnchor.Reset();
        }
    }
     else if ((c2MethId == m_c2MethId_FindBoolValue)
          ||  (c2MethId == m_c2MethId_FindCardValue)
          ||  (c2MethId == m_c2MethId_FindFloatValue)
          ||  (c2MethId == m_c2MethId_FindIntValue))
    {
        // We get an anchor to search
        TMEngJSONAnchorVal& mecvParent
        (
            meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd)
        );
        CheckAnchor(meOwner, mecvActual, mecvParent);

        // Make sure it's a container node. Throw if not
        const TJSONCont* pjprsnCont = pjprsnCheckIsCont
        (
            meOwner, mecvParent.jprsnValue(), kCIDLib::True
        );

        //
        //  Do the lookup. Though we will effectively do another one below, this
        //  one lets us unamiguously check for the value not being present and
        //  handling the throw if asked to, before we get into all the other
        //  stuff.
        //
        const TString& strToFind = meOwner.strStackValAt(c4FirstInd + 1);
        tCIDLib::TCard4 c4FoundAt;
        const TJSONValue* pjprsnFnd = pjprsnCont->pjprsnFindVal(strToFind, 0, c4FoundAt);

        // If not found, and they asked to throw if not, then do so
        if (!pjprsnFnd && meOwner.bStackValAt(c4FirstInd + 2))
            ThrowAnErr(meOwner, m_c4ErrNameNotFound, strToFind);

        tCIDLib::TBoolean bGoodVal = kCIDLib::False;
        if (pjprsnFnd && (pjprsnFnd->eType() != tCIDNet::EJSONVTypes::Null))
        {
            try
            {
                if (c2MethId == m_c2MethId_FindBoolValue)
                {
                    TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
                    if (pjprsnFnd->eType() == tCIDNet::EJSONVTypes::False)
                    {
                        mecvRet.bValue(kCIDLib::False);
                        bGoodVal = kCIDLib::True;
                    }
                     else if (pjprsnFnd->eType() == tCIDNet::EJSONVTypes::True)
                    {
                        mecvRet.bValue(kCIDLib::True);
                        bGoodVal = kCIDLib::True;
                    }
                }
                 else if (c2MethId == m_c2MethId_FindCardValue)
                {
                    TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
                    mecvRet.c4Value(pjprsnCont->c4FindVal(strToFind));
                    bGoodVal = kCIDLib::True;
                }
                 else if (c2MethId == m_c2MethId_FindFloatValue)
                {
                    TMEngFloat8Val& mecvRet = meOwner.mecvStackAtAs<TMEngFloat8Val>(c4FirstInd - 1);
                    mecvRet.f8Value(pjprsnCont->f8FindVal(strToFind));
                    bGoodVal = kCIDLib::True;
                }
                 else if (c2MethId == m_c2MethId_FindIntValue)
                {
                    TMEngInt4Val& mecvRet = meOwner.mecvStackAtAs<TMEngInt4Val>(c4FirstInd - 1);
                    mecvRet.i4Value(pjprsnCont->i4FindVal(strToFind));
                    bGoodVal = kCIDLib::True;
                }
            }

            catch(TError& errToCatch)
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                ThrowAnErr(meOwner, m_c4ErrGetValAs, errToCatch);
            }
        }

        if (!bGoodVal)
        {
            if (c2MethId == m_c2MethId_FindBoolValue)
            {
                TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
                mecvRet.bValue(meOwner.bStackValAt(c4FirstInd + 3));
            }
             else if (c2MethId == m_c2MethId_FindCardValue)
            {
                TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
                mecvRet.c4Value(meOwner.c4StackValAt(c4FirstInd + 3));
            }
             else if (c2MethId == m_c2MethId_FindFloatValue)
            {
                TMEngFloat8Val& mecvRet = meOwner.mecvStackAtAs<TMEngFloat8Val>(c4FirstInd - 1);
                mecvRet.f8Value(meOwner.f8StackValAt(c4FirstInd + 3));
            }
             else if (c2MethId == m_c2MethId_FindIntValue)
            {
                TMEngInt4Val& mecvRet = meOwner.mecvStackAtAs<TMEngInt4Val>(c4FirstInd - 1);
                mecvRet.i4Value(meOwner.i4StackValAt(c4FirstInd + 3));
            }
        }
    }
     else if (c2MethId == m_c2MethId_FindChild)
    {
        // We get an anchor to search
        TMEngJSONAnchorVal& mecvParent
        (
            meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd)
        );
        CheckAnchor(meOwner, mecvActual, mecvParent);

        // Make sure it's a container node. Throw if not
        const TJSONCont* pjprsnCont = pjprsnCheckIsCont
        (
            meOwner, mecvParent.jprsnValue(), kCIDLib::True
        );

        // Do the lookup
        tCIDLib::TCard4 c4FoundAt;
        const TJSONValue* pjprsnFnd = pjprsnCont->pjprsnFindVal
        (
            meOwner.strStackValAt(c4FirstInd + 2)
            , meOwner.c4StackValAt(c4FirstInd + 1)
            , c4FoundAt
        );

        // Set the result
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(pjprsnFnd != 0);

        //
        //  If we found it, return the found at and set up a new anchor. Else,
        //  make sure the new anchor is reset.
        //
        TMEngJSONAnchorVal& mecvChildAnchor
        (
            meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd + 4)
        );

        TMEngCard4Val& mecvAt = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 3);
        if (pjprsnFnd)
        {
            mecvAt.c4Value(c4FoundAt);

            if (!mecvActual.bGetChildAnchor(mecvParent, c4FoundAt, mecvChildAnchor))
                ThrowAnErr(meOwner, m_c4ErrChildNotFound);
        }
         else
        {
            mecvAt.c4Value(0);
            mecvChildAnchor.Reset();
        }
    }
     else if (c2MethId == m_c2MethId_FindObject)
    {
        // We get an anchor to search
        TMEngJSONAnchorVal& mecvParent
        (
            meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd)
        );
        CheckAnchor(meOwner, mecvActual, mecvParent);

        // Make sure it's a container node, throw if not
        const TJSONCont* pjprsnCont = pjprsnCheckIsCont
        (
            meOwner, mecvParent.jprsnValue(), kCIDLib::True
        );

        // Find the child, either throw or get back null based on user request
        tCIDLib::TCard4 c4Index = 0;
        const TJSONValue* pjprsnFnd = pjprsnFind
        (
            meOwner
            , *pjprsnCont
            , meOwner.strStackValAt(c4FirstInd + 1)
            , c4Index
            , meOwner.bStackValAt(c4FirstInd + 3)
        );

        // Get the return value and target child anchor objects
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        TMEngJSONAnchorVal& mecvChildAnchor
        (
            meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd + 2)
        );

        //
        //  If we found it, and it's an array, then set up the target
        //  anchor. Else, we'll either throw or just return false.
        //
        if (pjprsnFnd
        &&  pjprsnCheckIsObj(meOwner, *pjprsnFnd, meOwner.bStackValAt(c4FirstInd + 3)))
        {
            if (!mecvActual.bGetChildAnchor(mecvParent, c4Index, mecvChildAnchor))
                ThrowAnErr(meOwner, m_c4ErrChildNotFound);
            mecvRet.bValue(kCIDLib::True);
        }
         else
        {
            //
            //  Indicate it wasn't an array or wasn't found. Make sure the
            //  anchor is reset.
            //
            mecvRet.bValue(kCIDLib::False);
            mecvChildAnchor.Reset();
        }
    }
     else if (c2MethId == m_c2MethId_FindType)
    {
        // We get an anchor to search
        TMEngJSONAnchorVal& mecvParent
        (
            meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd)
        );
        CheckAnchor(meOwner, mecvActual, mecvParent);

        // Make sure it's a container node, throw if not
        const TJSONCont* pjprsnCont = pjprsnCheckIsCont
        (
            meOwner, mecvParent.jprsnValue(), kCIDLib::True
        );

        // Find the child by name. Throw if not found
        tCIDLib::TCard4 c4Index = 0;
        const TJSONValue* pjprsnFnd = pjprsnFind
        (
            meOwner
            , *pjprsnCont
            , meOwner.strStackValAt(c4FirstInd + 1)
            , c4Index
            , kCIDLib::True
        );

        // Give back the type
        TMEngEnumVal& mecvRet
            = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd - 1);
        mecvRet.c4Ordinal(tCIDLib::c4EnumOrd(pjprsnFnd->eType()));
    }
     else if (c2MethId == m_c2MethId_FindValue)
    {
        TMEngJSONAnchorVal& mecvParent
        (
            meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd)
        );
        CheckAnchor(meOwner, mecvActual, mecvParent);

        // Make sure it's a container node. Throw if not
        const TJSONCont* pjprsnCont = pjprsnCheckIsCont
        (
            meOwner, mecvParent.jprsnValue(), kCIDLib::True
        );

        // See if such a node exists. Throw if the user says so
        tCIDLib::TCard4 c4Index = 0;
        const TJSONValue* pjprsnFnd = pjprsnFind
        (
            meOwner
            , *pjprsnCont
            , meOwner.strStackValAt(c4FirstInd + 1)
            , c4Index
            , meOwner.bStackValAt(c4FirstInd + 2)
        );

        TMEngStringVal& mecvRet = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1);
        if (pjprsnFnd)
        {
            //
            //  We found it. This will either throw if it's not a simple
            //  value, or return the simple value or empty string if not
            //  a simple value.
            //
            mecvRet.strValue
            (
                strCheckIsSimpleVal
                (
                    meOwner, *pjprsnFnd, meOwner.bStackValAt(c4FirstInd + 2)
                )
            );
        }
         else
        {
            // Not found, and they didn't say throw, so just return empty
            mecvRet.strValue(TString::strEmpty());
        }
    }
     else if (c2MethId == m_c2MethId_GetChildAt)
    {
        // We get an anchor and an index of a child element
        TMEngJSONAnchorVal& mecvAnchor
        (
            meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd)
        );
        CheckAnchor(meOwner, mecvActual, mecvAnchor);
        const tCIDLib::TCard4 c4Index(meOwner.c4StackValAt(c4FirstInd + 1));

        // Make sure it's a container. Throw if not
        pjprsnCheckIsCont(meOwner, mecvAnchor.jprsnValue(), kCIDLib::True);

        // Get the target anchor and try to set it up
        TMEngJSONAnchorVal& mecvChildAnchor
        (
            meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd + 2)
        );

        if (!mecvActual.bGetChildAnchor(mecvAnchor, c4Index, mecvChildAnchor))
            ThrowAnErr(meOwner, m_c4ErrChildNotFound);
    }
     else if (c2MethId == m_c2MethId_GetChildCount)
    {
        // We just get an anchor
        TMEngJSONAnchorVal& mecvAnchor
        (
            meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd)
        );
        CheckAnchor(meOwner, mecvActual, mecvAnchor);

        // Make sure it's a container value. Throw if not
        const TJSONCont* pjprsnCont = pjprsnCheckIsCont
        (
            meOwner, mecvAnchor.jprsnValue(), kCIDLib::True
        );

        // Get the return value and fill it in
        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(pjprsnCont->c4ValCount());
    }
     else if ((c2MethId == m_c2MethId_GetFirstChild)
          ||  (c2MethId == m_c2MethId_GetLastChild))
    {
        // We just get an anchor
        TMEngJSONAnchorVal& mecvAnchor
        (
            meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd)
        );
        CheckAnchor(meOwner, mecvActual, mecvAnchor);

        // Make sure it's a container. Throw if not
        const TJSONCont* pjprsnCont = pjprsnCheckIsCont
        (
            meOwner, mecvAnchor.jprsnValue(), kCIDLib::True
        );

        //
        //  If it has any children, set up the target anchor. Else, make
        //  sure it's reset.
        //
        TMEngJSONAnchorVal& mecvChildAnchor
        (
            meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd + 1)
        );
        const tCIDLib::TCard4 c4Cnt = pjprsnCont->c4ValCount();
        if (c4Cnt)
        {
            tCIDLib::TCard4 c4Index = 0;
            if (c2MethId == m_c2MethId_GetLastChild)
                c4Index = c4Cnt - 1;

            if (!mecvActual.bGetChildAnchor(mecvAnchor, c4Index, mecvChildAnchor))
                ThrowAnErr(meOwner, m_c4ErrChildNotFound);
        }
         else
        {
            mecvChildAnchor.Reset();
        }

        // Return whether we set up the anchor or not
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(c4Cnt != 0);
    }
     else if (c2MethId == m_c2MethId_GetName)
    {
        // We get a source anchor
        TMEngJSONAnchorVal& mecvAnchor
        (
            meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd)
        );
        CheckAnchor(meOwner, mecvActual, mecvAnchor);

        //
        //  Put the simple value string of the anchor's value node into
        //  the output parm.
        //
        meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1).strValue
        (
            mecvAnchor.jprsnValue().strName()
        );
    }
     else if (c2MethId == m_c2MethId_GetNameAt)
    {
        TMEngJSONAnchorVal& mecvParent
        (
            meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd)
        );
        CheckAnchor(meOwner, mecvActual, mecvParent);

        // Make sure it's a container node. Throw if not
        const TJSONCont* pjprsnCont = pjprsnCheckIsCont
        (
            meOwner, mecvParent.jprsnValue(), kCIDLib::True
        );

        // Make sure the index is valid
        const tCIDLib::TCard4 c4Index = meOwner.c4StackValAt(c4FirstInd + 1);
        if (c4Index >= pjprsnCont->c4ValCount())
            ThrowAnErr(meOwner, m_c4ErrBadIndex, TCardinal(c4Index));

        // Get the indicated child and return the name. This
        const TJSONValue& jprsnVal = pjprsnCont->jprsnValueAt(c4Index);
        meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1).strValue(jprsnVal.strName());
    }
     else if (c2MethId == m_c2MethId_GetType)
    {
        // We get a source anchor
        TMEngJSONAnchorVal& mecvAnchor
        (
            meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd)
        );
        CheckAnchor(meOwner, mecvActual, mecvAnchor);

        // Get the enum return value and set it up
        TMEngEnumVal& mecvRet
            = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd - 1);
        mecvRet.c4Ordinal(tCIDLib::c4EnumOrd(mecvAnchor.jprsnValue().eType()));
    }
     else if (c2MethId == m_c2MethId_GetTypeAt)
    {
        TMEngJSONAnchorVal& mecvParent
        (
            meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd)
        );
        CheckAnchor(meOwner, mecvActual, mecvParent);

        // Make sure it's a container node. Throw if not
        const TJSONCont* pjprsnCont = pjprsnCheckIsCont
        (
            meOwner, mecvParent.jprsnValue(), kCIDLib::True
        );

        // Make sure the index is valid
        const tCIDLib::TCard4 c4Index = meOwner.c4StackValAt(c4FirstInd + 1);
        if (c4Index >= pjprsnCont->c4ValCount())
            ThrowAnErr(meOwner, m_c4ErrBadIndex, TCardinal(c4Index));

        // Looks OK, so get the value at that index
        const TJSONValue& jprsnVal = pjprsnCont->jprsnValueAt(c4Index);
        meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd - 1).c4Ordinal
        (
            tCIDLib::c4EnumOrd(jprsnVal.eType())
        );
    }
     else if (c2MethId == m_c2MethId_GetRootAnchor)
    {
        TMEngJSONAnchorVal& mecvAnchor
        (
            meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd)
        );
        mecvActual.GetRootAnchor(mecvAnchor);
    }
     else if (c2MethId == m_c2MethId_GetValue)
    {
        TMEngJSONAnchorVal& mecvAnchor
        (
            meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd)
        );

        meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1).strValue
        (
            mecvAnchor.jprsnValue().strValue()
        );
    }
     else if (c2MethId == m_c2MethId_GetValueAt)
    {
        TMEngJSONAnchorVal& mecvParent
        (
            meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd)
        );
        CheckAnchor(meOwner, mecvActual, mecvParent);

        // Make sure it's a container node. Throw if not
        const TJSONCont* pjprsnCont = pjprsnCheckIsCont
        (
            meOwner, mecvParent.jprsnValue(), kCIDLib::True
        );

        // Make sure the index is valid
        const tCIDLib::TCard4 c4Index = meOwner.c4StackValAt(c4FirstInd + 1);
        if (c4Index >= pjprsnCont->c4ValCount())
            ThrowAnErr(meOwner, m_c4ErrBadIndex, TCardinal(c4Index));

        // Get the indicated child and return the value
        const TJSONValue& jprsnVal = pjprsnCont->jprsnValueAt(c4Index);
        meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1).strValue(jprsnVal.strValue());
    }
     else if (c2MethId == m_c2MethId_NextPrevChild)
    {
        // The parent and child anchor has to be valid
        TMEngJSONAnchorVal& mecvParAnchor
        (
            meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd)
        );
        CheckAnchor(meOwner, mecvActual, mecvParAnchor);

        TMEngJSONAnchorVal& mecvChildAnchor
        (
            meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd + 2)
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
     else if (c2MethId == m_c2MethId_ParseResponse)
    {
        try
        {
            // Get the underlying C++ stream from the CML stream object
            TMEngTextInStreamVal& mecvInStrm
            (
                meOwner.mecvStackAtAs<TMEngTextInStreamVal>(c4FirstInd)
            );
            mecvActual.Parse(mecvInStrm.strmSrc(meOwner));
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrParseFailed, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_ParseResponse2)
    {
        try
        {
            //
            //  In this case we get a memory buffer to parse from and the encoding
            //  it's in.
            //
            TTextMBufInStream strmSrc
            (
                &meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd).mbufValue()
                , meOwner.c4StackValAt(c4FirstInd + 1)
                , tCIDLib::EAdoptOpts::NoAdopt
                , facCIDEncode().ptcvtMake(meOwner.strStackValAt(c4FirstInd + 2))
            );
            mecvActual.Parse(strmSrc);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrParseFailed, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_PrintToStream)
    {
        try
        {
            // Get the underlying C++ stream from the CML stream object
            TMEngTextOutStreamVal& mecvOutStrm
            (
                meOwner.mecvStackAtAs<TMEngTextOutStreamVal>(c4FirstInd)
            );

            // And we get an anchor to start at
            TMEngJSONAnchorVal& mecvAnchor
            (
                meOwner.mecvStackAtAs<TMEngJSONAnchorVal>(c4FirstInd + 1)
            );
            CheckAnchor(meOwner, mecvActual, mecvAnchor);
            mecvActual.Print(mecvOutStrm.strmTarget(meOwner), mecvAnchor);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrPrintErr, errToCatch);
        }
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngJSONParserInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------

// Checks the passed anchor to see if it's valid against the parser
tCIDLib::TVoid
TMEngJSONParserInfo::CheckAnchor(       TCIDMacroEngine&     meOwner
                                , const TMEngJSONParserVal&  mecvParser
                                , const TMEngJSONAnchorVal&  mecvAnchor)
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
            , L"The JSON anchor is out of date"
            , meOwner.c4CurLine()
        );
        throw TExceptException();
    }
}


//
//  Test to see if the passed value node is an array node. If not, either throw
//  an error or return 0, else cast it and return.
//
const TJSONArray*
TMEngJSONParserInfo::pjprsnCheckIsArray(        TCIDMacroEngine&    meOwner
                                        , const TJSONValue&         jprsnSrc
                                        , const tCIDLib::TBoolean   bThrowIfNot)
{
    if (jprsnSrc.eType() != tCIDNet::EJSONVTypes::Array)
    {
        if (bThrowIfNot)
        {
            meOwner.SetException
            (
                m_c2EnumId_Errors
                , strClassPath()
                , m_c4ErrNotAnArray
                , m_pmeciErrors->strPartialName(m_c4ErrNotAnArray)
                , m_pmeciErrors->strTextValue(m_c4ErrNotAnArray)
                , meOwner.c4CurLine()
            );
            throw TExceptException();
        }
        return 0;
    }
    return static_cast<const TJSONArray*>(&jprsnSrc);
}


//
//  Test to see if the passed value node is a container node (array or object.)
//  If not, throw an error or return null, else cast it and return.
//
const TJSONCont*
TMEngJSONParserInfo::pjprsnCheckIsCont(         TCIDMacroEngine&    meOwner
                                        , const TJSONValue&         jprsnSrc
                                        , const tCIDLib::TBoolean   bThrowIfNot)
{
    if (!jprsnSrc.bIsContType())
    {
        if (bThrowIfNot)
        {
            meOwner.SetException
            (
                m_c2EnumId_Errors
                , strClassPath()
                , m_c4ErrNotACont
                , m_pmeciErrors->strPartialName(m_c4ErrNotACont)
                , m_pmeciErrors->strTextValue(m_c4ErrNotACont)
                , meOwner.c4CurLine()
            );
            throw TExceptException();
        }
        return 0;
    }
    return static_cast<const TJSONCont*>(&jprsnSrc);
}


//
//  Test to see if the passed value node is an object node. If not, throw
//  an error, else cast it and return.
//
const TJSONObject*
TMEngJSONParserInfo::pjprsnCheckIsObj(          TCIDMacroEngine&    meOwner
                                        , const TJSONValue&         jprsnSrc
                                        , const tCIDLib::TBoolean   bThrowIfNot)
{
    if (jprsnSrc.eType() != tCIDNet::EJSONVTypes::Object)
    {
        if (bThrowIfNot)
        {
            meOwner.SetException
            (
                m_c2EnumId_Errors
                , strClassPath()
                , m_c4ErrNotAnObject
                , m_pmeciErrors->strPartialName(m_c4ErrNotAnObject)
                , m_pmeciErrors->strTextValue(m_c4ErrNotAnObject)
                , meOwner.c4CurLine()
            );
            throw TExceptException();
        }
        return 0;
    }
    return static_cast<const TJSONObject*>(&jprsnSrc);
}


//
//  Try to find the named child of the indicated parent, starting at the
//  indicated index. Optionally throw or return null if not found.
//
const TJSONValue*
TMEngJSONParserInfo::pjprsnFind(        TCIDMacroEngine&    meOwner
                                , const TJSONCont&          jprsnParent
                                , const TString&            strToFind
                                ,       tCIDLib::TCard4&    c4InOut
                                , const tCIDLib::TBoolean   bThrowIfNot)
{
    tCIDLib::TCard4 c4StartInd = c4InOut;
    const TJSONValue* pjprsnFnd = jprsnParent.pjprsnFindVal
    (
        strToFind, c4StartInd, c4InOut
    );

    // If not found, and they want to throw, do that
    if (!pjprsnFnd && bThrowIfNot)
        ThrowAnErr(meOwner, m_c4ErrNameNotFound, strToFind);

    // Else return the node or null
    return pjprsnFnd;
}


//
//  See if the passed value is a simple value. If so, then then return
//  the value string by ref, else either throw or return an empty string.
//
const TString&
TMEngJSONParserInfo::strCheckIsSimpleVal(       TCIDMacroEngine&    meOwner
                                        , const TJSONValue&         jprsnVal
                                        , const tCIDLib::TBoolean   bThrowIfNot)
{
    if (!jprsnVal.bIsSimpleType())
    {
        if (bThrowIfNot)
        {
            meOwner.SetException
            (
                m_c2EnumId_Errors
                , strClassPath()
                , m_c4ErrNotASimpleVal
                , m_pmeciErrors->strPartialName(m_c4ErrNotASimpleVal)
                , m_pmeciErrors->strTextValue(m_c4ErrNotASimpleVal)
                , meOwner.c4CurLine()
            );
            throw TExceptException();
        }
        return TString::strEmpty();
    }
    return jprsnVal.strValue();
}


// These throw errors for us
tCIDLib::TVoid
TMEngJSONParserInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
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
    throw TExceptException();
}


tCIDLib::TVoid
TMEngJSONParserInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
                                , const tCIDLib::TCard4     c4ToThrow
                                , const MFormattable&       mfmtblToken1)
{
    TString strMsg(m_pmeciErrors->strTextValue(c4ToThrow));
    strMsg.eReplaceToken( mfmtblToken1, L'1');

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , strMsg
        , meOwner.c4CurLine()
    );
    throw TExceptException();
}

tCIDLib::TVoid
TMEngJSONParserInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
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
    throw TExceptException();
}


