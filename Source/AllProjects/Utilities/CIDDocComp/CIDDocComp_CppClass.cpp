//
// FILE NAME: CIDDocComp_cppClass.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/30/2019
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  A derivative of the base page class, for C++ classes
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "CIDDocComp.hpp"

// ---------------------------------------------------------------------------
//   CLASS: TMethodParam
//  PREFIX: methp
// ---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TMethodParam: Public, static methods
// ----------------------------------------------------------------------------
tCIDLib::TBoolean TMethodParam::bIsDirParm(const tCIDDocComp::EParmPB eTest)
{
    return
    (
        (eTest == tCIDDocComp::EParmPB::Ref)
        ||  (eTest == tCIDDocComp::EParmPB::PtrCRef)
        ||  (eTest == tCIDDocComp::EParmPB::PtrRef)
        ||  (eTest == tCIDDocComp::EParmPB::Ptr)
    );
}


// ----------------------------------------------------------------------------
//  TMethodParam: Public, non-virtual methods
// ----------------------------------------------------------------------------
tCIDLib::TVoid
TMethodParam::Parse(const   TXMLTreeElement&    xtnodeSrc
                    ,       TParseCtx&          ctxToUse)
{
    // The name and type we have to just take as is
    m_strName = xtnodeSrc.xtattrNamed(L"Name").strValue();
    m_strType = xtnodeSrc.xtattrNamed(L"Type").strValue();

    // We can have an default value
    xtnodeSrc.bAttrExists(kCIDDocComp::strXML_DefValue, m_strDefVal);

    // Get the pass by and convert it
    m_ePassBy = tCIDDocComp::eXlatEParmPB(xtnodeSrc.xtattrNamed(L"PBy").strValue());
    if (!tCIDDocComp::bIsValidEnum(m_ePassBy))
        ctxToUse.AddErrorMsg(L"Invalid parameter pass-by type");

    // And the parameter direction
    m_eDir = tCIDDocComp::eXlatEParmDirs(xtnodeSrc.xtattrNamed(L"Dir").strValue());
    if (!tCIDDocComp::bIsValidEnum(m_eDir))
        ctxToUse.AddErrorMsg(L"Invalid parameter direction type");
}

tCIDLib::TVoid
TMethodParam::OutputContent(        TTextOutStream&     strmTar
                            , const tCIDLib::TBoolean   bIndent) const
{
    // Output the type in deemph and the name emph
    strmTar << L"<span class=\"DeemphCode\">";

    switch(m_ePassBy)
    {
        case tCIDDocComp::EParmPB::CPtr :
            strmTar << L"const " << m_strType << L"*";
            break;

        case tCIDDocComp::EParmPB::CPtrC :
            strmTar << L"const " << m_strType << L"* const";
            break;

        case tCIDDocComp::EParmPB::CRef :
            strmTar << L"const " << m_strType << L"&";
            break;

        case tCIDDocComp::EParmPB::Ref :
            strmTar << m_strType << L"&";
            break;

        case tCIDDocComp::EParmPB::Ptr :
            strmTar << m_strType << L"*";
            break;

        case tCIDDocComp::EParmPB::PtrCRef :
            strmTar << L"const " << m_strType << L"*&";
            break;

        case tCIDDocComp::EParmPB::PtrRef :
            strmTar << m_strType << L"*&";
            break;

        case tCIDDocComp::EParmPB::Val :
            strmTar << m_strType;
            break;

        default :
            break;
    };

    strmTar << L"</span>"
            << L"<span class=\"EmphCode\"> " << m_strName;

    // if we have a default value, show that
    if (!m_strDefVal.bIsEmpty())
        strmTar << L" = " << m_strDefVal;

    strmTar << L"</span>";
}



// ---------------------------------------------------------------------------
//   CLASS: TMethod
//  PREFIX: meth
// ---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TMethod: Public, non-virtual methods
// ----------------------------------------------------------------------------
tCIDLib::TVoid TMethod::Parse(const TXMLTreeElement& xtnodeSrc, TParseCtx& ctxToUse)
{
    // Process the attributes
    const TString& strAttrs = xtnodeSrc.xtattrNamed(L"Attrs").strValue();
    if (TStringTokenizer::bParseSpacedTokens(strAttrs, ctxToUse.m_colTmp))
    {
        m_eAttrs = tCIDDocComp::EMethAttrs::None;
        tCIDLib::TBoolean bRes = ctxToUse.m_colTmp.bForEach
        (
            [this, &ctxToUse](const TString& strCur)
            {
                const tCIDDocComp::EMethAttrs eAttr = tCIDDocComp::eXlatEMethAttrs(strCur);
                if (eAttr == tCIDDocComp::EMethAttrs::None)
                    ctxToUse.AddErrorMsg(L"Invalid method attribute '%(1)'", strCur);

                // It will be the no-attrs value if none are ever set
                if (eAttr != tCIDDocComp::EMethAttrs::NoAttrs)
                    m_eAttrs |= eAttr;
                return kCIDLib::True;
            }
        );
    }
     else
    {
        ctxToUse.AddErrorMsg(L"Failed to parse method attributes");
    }

    //
    //  A variation can have multiple names if there are variations with the
    //  same parameters and return type, just different names. This saves a
    //  lot of redundancy in the XML.
    //
    const TString& strNames = xtnodeSrc.xtattrNamed(L"Names").strValue();
    if (!TStringTokenizer::bParseSpacedTokens(strNames, m_colNames))
        ctxToUse.AddErrorMsg(L"Failed to parse method variation names");

    // Get the return type which we just have to accept as is
    m_strRetType = xtnodeSrc.xtattrNamed(L"RetType").strValue();

    // First child is general description
    m_hnDescr.Parse(xtnodeSrc.xtnodeChildAtAsElement(0), ctxToUse);

    // Next is a method params element, and we do his children as parameters
    const TXMLTreeElement& xtnodeParams = xtnodeSrc.xtnodeChildAtAsElement(1);
    xtnodeParams.bForEach
    (
        [&](const TXMLTreeElement& xtnodeCur)
        {
            TMethodParam& mparamNew = m_colParams.objAdd(TMethodParam());
            mparamNew.Parse(xtnodeCur, ctxToUse);

            // If this one is one we need to indict the dir, set that flag
            if (TMethodParam::bIsDirParm(mparamNew.m_ePassBy))
                m_bAnyDirs = kCIDLib::True;

            return kCIDLib::True;
        }
    );
}

tCIDLib::TVoid TMethod::OutputContent(TTextOutStream& strmTar) const
{
    //
    //  Output the method signatures first. The only thing different for each
    //  one is the name, so we have a helper for that and just pass him each
    //  name in turn.
    //
    strmTar << L"<pre>";
    m_colNames.bForEach
    (
        [&strmTar, this](const TString& strName)
        {
            OutputSignature(strmTar, strName);
            return kCIDLib::True;
        }
    );
    strmTar << L"</pre>";

    // Indent the doc text
    strmTar << L"<blockquote>";
    m_hnDescr.OutputNodes(strmTar);

    //
    //  If any of the parameters are non-const ref, then indicate their direction
    //  This is done in a small font.
    //
    if (m_bAnyDirs)
    {
        strmTar << L"<div class=\"SmallDropNote\">";
        tCIDLib::TCard4 c4Count = 0;
        m_colParams.bForEach
        (
            [&,this](const TMethodParam& methpCur)
            {
                if (TMethodParam::bIsDirParm(methpCur.m_ePassBy))
                {
                    if (c4Count)
                        strmTar << L", ";

                    c4Count++;
                    strmTar << methpCur.m_strName << L"="
                            << methpCur.m_eDir;
                }
                return kCIDLib::True;
            }
        );
        strmTar << L"</div>";
    }

    strmTar << L"</blockquote>" << kCIDLib::NewLn;
}

// ----------------------------------------------------------------------------
//  TMethod: Private, non-virtual methods
// ----------------------------------------------------------------------------
tCIDLib::TVoid
TMethod::OutputSignature(TTextOutStream& strmTar, const TString& strName) const
{
    // Do the return type
    if (!m_strRetType.bIsEmpty())
        strmTar << m_strRetType << kCIDLib::chSpace;

    // The name, emphasized
    strmTar << L"<span class=\"EmphCode\">" << strName << L"</span>";

    //
    //  If it has two parameters or less, we'll do it on one line. Else we wrap
    //  and indent the parameters.
    //
    const tCIDLib::TCard4 c4PCnt = m_colParams.c4ElemCount();
    const tCIDLib::TBoolean bWrapped(c4PCnt > 2);
    if (bWrapped)
        strmTar << L"<br/>(<br/>    ";
    else
        strmTar << L"(";

    // Format each parameter, breaking after if needed
    m_colParams.bForEachI
    (
        [&](const TMethodParam& methpCur, const tCIDLib::TCard4 c4Index)
        {
            if (c4Index)
            {
                if (bWrapped)
                    strmTar << L"\n, ";
                else
                    strmTar << L", ";
            }
            methpCur.OutputContent(strmTar, bWrapped);
            return kCIDLib::True;
        }
    );

    // Close this one of now, wrapping after the final parameter if needed
    if (bWrapped)
        strmTar << L"<br/>";
    strmTar << L")";

    // A break after each signature
    strmTar << L"<br/>";
}



// ---------------------------------------------------------------------------
//   CLASS: TMethodGrp
//  PREFIX: methg
// ---------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//  TMethodGrp: Public, non-virtual methods
// ----------------------------------------------------------------------------
tCIDLib::TVoid
TMethodGrp::Parse(const TXMLTreeElement& xtnodeSrc, TParseCtx& ctxToUse)
{
    xtnodeSrc.bForEach
    (
        [&](const TXMLTreeElement& xtnodeCur)
        {
            if (xtnodeCur.strQName() == kCIDDocComp::strXML_Desc)
            {
                m_hnDescr.Parse(xtnodeCur, ctxToUse);
            }
             else
            {
                // Has to be a method
                TMethod& methNew = m_colMethods.objAdd(TMethod());
                methNew.Parse(xtnodeCur, ctxToUse);
            }
            return kCIDLib::True;
        }
    );
}

tCIDLib::TVoid TMethodGrp::OutputContent(TTextOutStream& strmTar) const
{
    // If there's a description first, then output that
    if (!m_hnDescr.bIsEmpty())
    {
        m_hnDescr.OutputNodes(strmTar);

        // Separate this from the first method below it with a break
        strmTar << L"<br/>";
    }

    // And then do the methods
    m_colMethods.bForEach
    (
        [this, &strmTar] (const TMethod& methCur)
        {
            methCur.OutputContent(strmTar);
            return kCIDLib::True;
        }
    );
}


// ---------------------------------------------------------------------------
//   CLASS: TMemberGrp
//  PREFIX: memg
// ---------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//  TMemberGrp: Public, non-virtual methods
// ----------------------------------------------------------------------------
tCIDLib::TBoolean TMemberGrp::bIsEmpty() const
{
    return
    (
        m_methgNVirtMethods.bIsEmpty()
        && m_methgOverMethods.bIsEmpty()
        && m_methgStatMethods.bIsEmpty()
        && m_methgVirtMethods.bIsEmpty()
    );
}

//
//  We parse all of the members for one of the visibility scopes. We don't care
//  which it is, since they all are the same format. The caller knows.
//
tCIDLib::TVoid
TMemberGrp::Parse(const TXMLTreeElement& xtnodeGrp, TParseCtx& ctxToUse)
{
    xtnodeGrp.bForEach
    (
        [&](const TXMLTreeElement& xtnodeElem)
        {
            const TString& strName = xtnodeElem.strQName();
            if (strName == L"DataTypes")
            {
            }
             else if (strName == L"Constants")
            {
            }
             else if (strName == L"OverMeths")
            {
                m_methgOverMethods.Parse(xtnodeElem, ctxToUse);
            }
             else if (strName == L"NVirtMeths")
            {
                m_methgNVirtMethods.Parse(xtnodeElem, ctxToUse);
            }
             else if (strName == L"StaticMeths")
            {
                m_methgStatMethods.Parse(xtnodeElem, ctxToUse);
            }
            else if (strName == L"VirtMeths")
            {
                m_methgVirtMethods.Parse(xtnodeElem, ctxToUse);
            }
             else if (strName == L"Members")
            {
            }
             else
            {
                ctxToUse.AddErrorMsg(L"Unknown member group element: %(1)", strName);
            }
            return kCIDLib::True;
        }
    );
}

tCIDLib::TVoid
TMemberGrp::OutputContent(TTextOutStream& strmTar) const
{
    if (!m_methgStatMethods.bIsEmpty())
    {
        strmTar << L"<p><span class='SecHdr'>"
                << tCIDDocComp::strXlatEVisTypes(m_eVisType)
                << L" Static Methods</span></p>";
        TStreamIndentJan janIndent(&strmTar, 4);
        m_methgStatMethods.OutputContent(strmTar);
    }

    if (!m_methgVirtMethods.bIsEmpty())
    {
        strmTar << L"<p><span class='SecHdr'>"
                << tCIDDocComp::strXlatEVisTypes(m_eVisType)
                << L" Virtual Methods</span></p>";
        TStreamIndentJan janIndent(&strmTar, 4);
        m_methgVirtMethods.OutputContent(strmTar);
    }

    if (!m_methgOverMethods.bIsEmpty())
    {
        strmTar << L"<p><span class='SecHdr'>"
                << tCIDDocComp::strXlatEVisTypes(m_eVisType)
                << L" Overrides</span></p>";
        TStreamIndentJan janIndent(&strmTar, 4);
        m_methgOverMethods.OutputContent(strmTar);
    }

    if (!m_methgNVirtMethods.bIsEmpty())
    {
        strmTar << L"<p><span class='SecHdr'>"
                << tCIDDocComp::strXlatEVisTypes(m_eVisType)
                << L" Non-Virtual Methods</span></p>";
        TStreamIndentJan janIndent(&strmTar, 4);
        m_methgNVirtMethods.OutputContent(strmTar);
    }
}



// ---------------------------------------------------------------------------
//   CLASS: TCppClassPage
//  PREFIX: pg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCppClassPage: Constructors and Destructor
// ---------------------------------------------------------------------------
TCppClassPage::TCppClassPage(   const   TString&    strExtTitle
                                , const TString&    strParSrcDir
                                , const TString&    strParTopic
                                , const TString&    strFileName) :

    TBasePage
    (
        strExtTitle
        , strParSrcDir
        , strParTopic
        , strFileName
        , tCIDDocComp::EPageTypes::CppClass
    )
    , m_memgProtected(tCIDDocComp::EVisTypes::Protected)
    , m_memgPrivate(tCIDDocComp::EVisTypes::Private)
    , m_memgPublic(tCIDDocComp::EVisTypes::Public)

{
}


// ---------------------------------------------------------------------------
//  TCppClassPage: Private, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TCppClassPage::bFlagOn(const tCIDDocComp::EClsFlags eCheck) const
{
    return tCIDLib::bAllBitsOn(m_eFlags, eCheck);
}


tCIDLib::TVoid
TCppClassPage::Parse(           TTopic&             topicParent
                        , const TXMLTreeElement&    xtnodeClass
                        ,       TParseCtx&          ctxToUse)
{
    // First we need to get out our class name and parent class name
    m_strClass = xtnodeClass.xtattrNamed(L"Class").strValue();
    m_strParClass = xtnodeClass.xtattrNamed(L"Parent").strValue();

    // Get the boolean flags out and parse them
    const TString& strFlags = xtnodeClass.xtattrNamed(L"Flags").strValue();
    if (!TStringTokenizer::bParseSpacedTokens(strFlags, ctxToUse.m_colTmp))
        ctxToUse.AddErrorMsg(L"Failed to parse class flags");

    // Translate any flags and set them on our flags member
    m_eFlags = tCIDDocComp::EClsFlags::None;
    ctxToUse.m_colTmp.bForEach
    (
        [this, &ctxToUse](const TString& strCur)
        {
            const tCIDDocComp::EClsFlags eFlag = tCIDDocComp::eXlatEClsFlags(strCur);
            if (eFlag == tCIDDocComp::EClsFlags::None)
                ctxToUse.AddErrorMsg(L"Invalid class flag '%(1);", strCur);

            // It could be the no-flags value
            if (eFlag != tCIDDocComp::EClsFlags::NoFlags)
                m_eFlags |= eFlag;
            return kCIDLib::True;
        }
    );

    tCIDLib::TCard4 c4At;
    const TXMLTreeElement& xtnodeDesc = xtnodeClass.xtnodeFindElement
    (
        kCIDDocComp::strXML_Desc, 0, c4At
    );
    m_hnDesc.Parse(xtnodeDesc, ctxToUse);

    //
    //  Next we have optional public, protected and private members stuff. They
    //  have the same content, so we call a helper with each one.
    //
    const TXMLTreeElement* pxtnodeGrp;
    pxtnodeGrp = xtnodeClass.pxtnodeFindElement(L"PubIntf", 0, c4At);
    if (pxtnodeGrp)
        m_memgPublic.Parse(*pxtnodeGrp, ctxToUse);

    pxtnodeGrp = xtnodeClass.pxtnodeFindElement(L"ProtIntf", 0, c4At);
    if (pxtnodeGrp)
        m_memgProtected.Parse(*pxtnodeGrp, ctxToUse);

    pxtnodeGrp = xtnodeClass.pxtnodeFindElement(L"PrivIntf", 0, c4At);
    if (pxtnodeGrp)
        m_memgPrivate.Parse(*pxtnodeGrp, ctxToUse);
}


tCIDLib::TVoid TCppClassPage::OutputContent(TTextOutStream& strmTar) const
{
    // The class name is the title
    strmTar << L"<p><span class='PageHdr'> Class: "
            << m_strClass
            << L"</span></p>";

    // We have a little table of key info at the top
    strmTar << L"<blockquote><table class='CppClsHdrTable'>";
    strmTar << L"<tr><td>Parent Class:</td><td>"
            << m_strParClass
            << L"</td></tr>"
            << L"<tr><td>Std Mixins:</td><td>";

    static const tCIDDocComp::EClsFlags eMixins
    (
        tCIDDocComp::EClsFlags::Dup
        | tCIDDocComp::EClsFlags::Fmt
        | tCIDDocComp::EClsFlags::BStrm
    );

    if (tCIDLib::bAnyBitsOn(m_eFlags, eMixins))
    {
        if (bFlagOn(tCIDDocComp::EClsFlags::Dup))
            strmTar << L"MDuplicable ";

        if (bFlagOn(tCIDDocComp::EClsFlags::Fmt))
            strmTar << L"MFormattable ";

        if (bFlagOn(tCIDDocComp::EClsFlags::BStrm))
            strmTar << L"MStreamable ";
    }

    strmTar << L"</td></tr>"
            << L"</table></blockquote><Br/>";

    // Next should be the descriptive text.
    m_hnDesc.OutputNodes(strmTar);

    // Output any non-empty member groups
    if (!m_memgPublic.bIsEmpty())
        m_memgPublic.OutputContent(strmTar);
    if (!m_memgProtected.bIsEmpty())
        m_memgProtected.OutputContent(strmTar);
    if (!m_memgPrivate.bIsEmpty())
        m_memgPrivate.OutputContent(strmTar);
}

