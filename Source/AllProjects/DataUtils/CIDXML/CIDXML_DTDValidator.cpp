//
// FILE NAME: CIDXML_DTDValidator.Cpp
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
//  This file implements the TDTDValidator class.
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
RTTIDecls(TDTDValidator,TXMLValidator)



// ---------------------------------------------------------------------------
//  CLASS: TDTDValidator
// PREFIX: xval
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDTDValidator: Constructors and Destructor
// ---------------------------------------------------------------------------
TDTDValidator::TDTDValidator(TXMLParserCore* const  pxprsOwner
                            , MXMLDTDEvents* const  pmxevDTDEvents) :

    TXMLValidator(pxprsOwner)
    , m_bIgnoreDTD(kCIDLib::False)
    , m_c4RootElemId(kCIDXML::c4InvalidElemId)
    , m_pxadIgnoredAttrDef(0)
    , m_pxdeclIgnoredElement(0)
    , m_pxdeclIgnoredEntity(0)
    , m_pxdeclIgnoredNotation(0)
    , m_pmxevDTD(pmxevDTDEvents)
    , m_pxnipElems(0)
    , m_pxnipEntities(0)
    , m_pxnipNotations(0)
    , m_pxnipPEs(0)
{
    // Create our pools
    m_pxnipElems = new TXMLNameIDPool<TDTDElemDecl>
    (
        &TDTDElemDecl::strExtractKey
    );
    m_pxnipEntities = new TXMLNameIDPool<TDTDEntityDecl>
    (
        &TDTDEntityDecl::strExtractKey
    );
    m_pxnipNotations = new TXMLNameIDPool<TDTDNotationDecl>
    (
        &TDTDNotationDecl::strExtractKey
    );
    m_pxnipPEs = new TXMLNameIDPool<TDTDEntityDecl>
    (
        &TDTDEntityDecl::strExtractKey
    );
}

TDTDValidator::~TDTDValidator()
{
    // Clean up the pools
    delete m_pxnipElems;
    delete m_pxnipEntities;
    delete m_pxnipNotations;
    delete m_pxnipPEs;

    // Clean up any 'ignored' objects
    delete m_pxadIgnoredAttrDef;
    delete m_pxdeclIgnoredElement;
    delete m_pxdeclIgnoredEntity;
    delete m_pxdeclIgnoredNotation;
}


// ---------------------------------------------------------------------------
//  TDTDValidator: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TDTDValidator::bCanParseDTD() const
{
    // Yes we obviously do
    return kCIDLib::True;
}


tCIDLib::TBoolean
TDTDValidator::bValidateAttr(const  TXMLAttrDef&    xadToTest
                            , const TString&        strValue)
{
    //
    //  If the default type indicates its a fixed value, then we need to
    //  make sure that the passed value is that of the defined fixed value.
    //
    if (xadToTest.eDefaultType() == tCIDXML::EDefAttrTypes::Fixed)
    {
        if (xadToTest.strValue() != strValue)
        {
            xprsOwner().PostXMLError
            (
                kXMLErrs::errcXMLE_AttrNotSameAsFixed
                , xadToTest.strFullName()
            );
        }
    }

    // If its a CDATA style attribute, this is all we have to do with it
    if (xadToTest.eType() == tCIDXML::EAttrTypes::CData)
        return kCIDLib::True;

    //
    //  All the other types require that the value be non-empty, so do a
    //  quick check for that, since it will also simplify things below.
    //
    if (strValue.bIsEmpty())
    {
        xprsOwner().PostXMLError
        (
            kXMLErrs::errcXMLE_AttrValueIsEmpty
            , xadToTest.strFullName()
        );
        return kCIDLib::False;
    }

    //
    //  We made it past that, so get a little more information out that we'll
    //  need during the loop below.
    //
    const tCIDLib::TBoolean     bMultiValued = xadToTest.bMultiValued();
    const tCIDLib::TBoolean     bNamedType   = xadToTest.bNamedType();
    const tCIDLib::TBoolean     bRefType     = xadToTest.bRefType();
    const tCIDXML::EAttrTypes   eType        = xadToTest.eType();

    //
    //  Get a copy of the attribute that we can modify. This will let us stick
    //  nuls into the string to cap off each component.
    //
    tCIDLib::TCh* pszCopy = strValue.pszDupBuffer();
    TArrayJanitor<tCIDLib::TCh> janTmp(pszCopy);

    tCIDLib::TCh* pszCur = pszCopy;
    tCIDLib::TCh* pszBase = pszCopy;

    //
    //  Loop until we've handled all of the text successfully, or failed.
    //
    tCIDLib::TBoolean bAtEnd = kCIDLib::False;
    while (kCIDLib::True)
    {
        //
        //  If its a named type, then the first character must be a first name
        //  character. We don't allow leading space, so doing this check first
        //  will catch that.
        //
        if (bNamedType)
        {
            if (!TXMLCharFlags::bIsFirstNameChar(*pszCur))
            {
                xprsOwner().PostXMLError
                (
                    kXMLErrs::errcXMLV_NameTokenAttrExpected
                    , xadToTest.strFullName()
                );
            }
            pszCur++;
        }

        // And now loop through the chars up to the end or space
        while (*pszCur)
        {
            //
            //  Watch for a space character. If this is not a multi-valued type,
            //  then its an error, else break out and test this value.
            //
            if (TXMLCharFlags::bIsSpace(*pszCur))
            {
                if (!bMultiValued)
                {
                    xprsOwner().PostXMLError
                    (
                        kXMLErrs::errcXMLV_BadMultiValue
                        , xadToTest.strFullName()
                    );
                    return kCIDLib::False;
                }
                break;
            }

            // Its not a space, so its got to be a name character
            if (!TXMLCharFlags::bIsNameChar(*pszCur))
            {
                xprsOwner().PostXMLError
                (
                    kXMLErrs::errcXMLV_NameTokenAttrExpected
                    , xadToTest.strFullName()
                );
                return kCIDLib::False;
            }

            pszCur++;
        }

        // Remember if we are at the end
        if (!*pszCur)
            bAtEnd = kCIDLib::True;

        // And cap it off here
        *pszCur = 0;


        if (bRefType)
        {
            // <TBD> Implement the ID reference tracking code
        }
         else if ((eType == tCIDXML::EAttrTypes::Entity)
              ||  (eType == tCIDXML::EAttrTypes::Entities))
        {
            // The value has to be a declared entity
            TDTDEntityDecl* pxdeclEnt = m_pxnipEntities->pobjByName(pszBase);
            if (!pxdeclEnt)
            {
                xprsOwner().PostXMLError
                (
                    kXMLErrs::errcXMLV_BadEntityRefAttr
                    , xadToTest.strFullName()
                );
                return kCIDLib::False;
            }
             else
            {
                // Make sure the entity is an unparsed one
                if (!pxdeclEnt->bIsUnparsed())
                {
                    xprsOwner().PostXMLError
                    (
                        kXMLErrs::errcXMLV_MustBeUnparsed
                        , xadToTest.strFullName()
                    );
                    return kCIDLib::False;
                }
            }
        }
         else if ((eType == tCIDXML::EAttrTypes::Enumeration)
              ||  (eType == tCIDXML::EAttrTypes::Notation))
        {
            // Make sure the value is in the list of values
            if (!xadToTest.bIsInEnumList(pszBase))
            {
                xprsOwner().PostXMLError
                (
                    kXMLErrs::errcXMLV_BadEnumRefAttr
                    , TString(pszBase)
                    , xadToTest.strFullName()
                );
                return kCIDLib::False;
            }
        }

        // If we are at the end, then break out
        if (bAtEnd)
            break;

        // Update the base pointer for the next round
        pszCur++;
        pszBase = pszCur;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TDTDValidator::bValidateContent(const   TXMLElemDecl&           xdeclParent
                                , const tCIDLib::TCard4* const  pc4Children
                                , const tCIDLib::TCard4         c4ChildCount)
{
    //
    //  We know that the passed element is actually a TDTDElemDecl object,
    //  and we need to look at it as that type in order to call the validation
    //  method on it. In our DTD validator, each element can validate its
    //  own contents.
    //
    //  If debug mode, check it dynamically to be sure, else do the cheap
    //  version.
    //
    const TDTDElemDecl*
    #if CID_DEBUG_ON
    pxdeclDTDElem = dynamic_cast<const TDTDElemDecl*>(&xdeclParent);
    #else
    pxdeclDTDElem = reinterpret_cast<const TDTDElemDecl*>(&xdeclParent);
    #endif

    //
    //  Now ask this element to validate the content we got, and to tell us how
    //  and where it failed, if it does fail.
    //
    tCIDLib::TCard4 c4FailedAt;
    const tCIDXML::EValidRes eResult = pxdeclDTDElem->eValidate
    (
        pc4Children
        , c4ChildCount
        , c4FailedAt
    );

    //
    //  If it failed, then figure out why and where and tell the user about
    //  it by issuing an error.
    //
    if (eResult != tCIDXML::EValidRes::Success)
    {
        // Format the content model into a string
        TTextStringOutStream strmCM(128);
        xdeclParent.FormatCMTo(strmCM, *this);
        strmCM.Flush();

        // And issue the appropriate failure error
        if (eResult == tCIDXML::EValidRes::TooMany)
        {
            xprsOwner().PostXMLError
            (
                kXMLErrs::errcXMLV_TooManyChildren
                , strmCM.strData()
            );
        }
         else if (eResult == tCIDXML::EValidRes::TooFew)
        {
            xprsOwner().PostXMLError
            (
                kXMLErrs::errcXMLV_TooFewChildren
                , strmCM.strData()
            );
        }
         else if (eResult == tCIDXML::EValidRes::Mismatch)
        {
            // Get the element decl for the bad one
            const TXMLElemDecl* pxdeclBad = pxdeclFindElemById
            (
                pc4Children[c4FailedAt]
            );

            // If we don't find it, then give up
            if (!pxdeclBad)
            {
                facCIDXML().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kXMLErrs::errcPool_IdNotInPool
                    , tCIDLib::ESeverities::ProcFatal
                    , tCIDLib::EErrClasses::Internal
                    , TCardinal(pc4Children[c4FailedAt])
                    , facCIDXML().strMsg(kXMLMsgs::midGen_element)
                );
            }

            // We found it, so issue the error and show the content model
            xprsOwner().PostXMLError
            (
                kXMLErrs::errcXMLV_NotInCM
                , TCardinal(c4FailedAt + 1)
                , pxdeclBad->strFullName()
                , strmCM.strData()
            );
        }
        #if CID_DEBUG_ON
         else
        {
            facCIDXML().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kXMLErrs::errcGen_UnknownValidResType
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::Internal
                , TInteger(tCIDLib::i4EnumOrd(eResult))
            );
        }
        #endif
    }
    return (eResult == tCIDXML::EValidRes::Success);
}


tCIDLib::TBoolean TDTDValidator::bIsValidRoot(const TXMLElemDecl& xdeclToTest)
{
    // See if its the id of our stored root element
    return (xdeclToTest.c4Id() == m_c4RootElemId);
}


MXMLDTDEvents* TDTDValidator::pmxevDTDEventHandler() const
{
    return m_pmxevDTD;
}

MXMLDTDEvents*
TDTDValidator::pmxevDTDEventHandler(MXMLDTDEvents* const pmxevToSet)
{
    m_pmxevDTD = pmxevToSet;
    return m_pmxevDTD;
}


TXMLElemDecl* TDTDValidator::pxdeclFindElemById(const tCIDLib::TCard4 c4Id)
{
    return m_pxnipElems->pobjById(c4Id);
}

const TXMLElemDecl*
TDTDValidator::pxdeclFindElemById(const tCIDLib::TCard4 c4Id) const
{
    return m_pxnipElems->pobjById(c4Id);
}


TXMLElemDecl*
TDTDValidator::pxdeclFindElemByName(const   TString&            strQName
                                    , const tCIDXML::EPoolOpts  eOpt
                                    ,       tCIDLib::TBoolean&  bAdded)
{
    // See if we can find the element in the pool
    TDTDElemDecl* pxdeclRet = m_pxnipElems->pobjByName(strQName);

    //
    //  If we didn't find it, then look at the options. If the option is to
    //  find, then return zero now; else, create a default element and add
    //  it.
    //
    if (!pxdeclRet)
    {
        if (eOpt == tCIDXML::EPoolOpts::Find)
            return 0;

        pxdeclRet = new TDTDElemDecl(strQName);
        m_pxnipElems->c4AddNew(pxdeclRet);
        bAdded = kCIDLib::True;
    }
     else
    {
        bAdded = kCIDLib::False;
    }
    return pxdeclRet;
}


const TXMLElemDecl*
TDTDValidator::pxdeclFindElemByName(const   TString&            strQName
                                    , const tCIDXML::EPoolOpts  eOpt
                                    ,       tCIDLib::TBoolean&  bAdded) const
{
    // See if we can find the element in the pool
    TDTDElemDecl* pxdeclRet = m_pxnipElems->pobjByName(strQName);

    //
    //  If we didn't find it, then look at the options. If the option is to
    //  find, then return zero now; else, create a default element and add
    //  it.
    //
    if (!pxdeclRet)
    {
        if (eOpt == tCIDXML::EPoolOpts::Find)
            return 0;

        pxdeclRet = new TDTDElemDecl(strQName);
        m_pxnipElems->c4AddNew(pxdeclRet);
        bAdded = kCIDLib::True;
    }
     else
    {
        bAdded = kCIDLib::False;
    }
    return pxdeclRet;
}


TXMLEntityDecl*
TDTDValidator::pxdeclFindEntityByName(const TString& strName)
{
    return m_pxnipEntities->pobjByName(strName);
}


TXMLNotationDecl*
TDTDValidator::pxdeclFindNotationByName(const TString& strName)
{
    return m_pxnipNotations->pobjByName(strName);
}


//
//  This is called if the parser never say any reference to a DOCTYPE in
//  the main document, and has been provided with a default external subset.
//  It will push it on the spooler stack and call us here and we can just
//  parse away.
//
tCIDLib::TVoid TDTDValidator::ParseDefExtSS(const TString& strRootElem)
{
    // If we have a DTD handler, then send a start event
    if (m_pmxevDTD && xprsOwner().bInfoWanted(tCIDXML::EParseFlags::Topology))
        m_pmxevDTD->StartExtSubset();

    // And parse the external entity. Tell it that it can legally see a TextDecl
    ParseExtEntity(kCIDLib::True);

    //
    //  Set the root element id now. Since there is no DOCTYPE in this scenario,
    //  they have to tell us what it will be.
    //
    const TDTDElemDecl* pxdeclRoot = m_pxnipElems->pobjByName(strRootElem);
    if (!pxdeclRoot)
    {
        //
        //  It wasn't found, so report it and just set the root element id
        //  to a known unused value so we can keep going.
        //
        xprsOwner().PostXMLError(kXMLErrs::errcXMLV_UndeclElemAsRoot, strRootElem);
        m_c4RootElemId = kCIDLib::c4MaxCard;
    }
     else
    {
        m_c4RootElemId = pxdeclRoot->c4Id();
    }

    // If we have a DTD handler, then send an end event
    if (m_pmxevDTD && xprsOwner().bInfoWanted(tCIDXML::EParseFlags::Topology))
        m_pmxevDTD->EndExtSubset();
}


tCIDLib::TVoid TDTDValidator::ParseDTD(const tCIDLib::TBoolean bIgnoreDTD)
{
    // Store the ignore flag
    m_bIgnoreDTD = bIgnoreDTD;

    // Just vector it to our internal DOCTYPE parsing method
    ParseDOCType(kCIDLib::False);

    //
    //  Now we need to check all the stuff that we cannot do until the end,
    //  because we have to support 'use before declaration' as per the XML
    //  spec. We only do all this stuff if we are validting.
    //
    if (tCIDLib::bAllBitsOn(xprsOwner().eOptions(), tCIDXML::EParseOpts::Validate))
    {
        if (m_pxnipElems->bResetIter())
        {
            //
            //  Iterate through the element decls and apply any checks that
            //  are required at this point.
            do
            {
                const TDTDElemDecl& xdeclCur = m_pxnipElems->objCur();

                //
                //  Look at why this element got into the decl pool. If it was
                //  not because its declared, then its an error.
                //
                const tCIDXML::EElemReasons eReason = xdeclCur.eCreateReason();
                if (eReason != tCIDXML::EElemReasons::Declared)
                {
                    tCIDLib::TErrCode errcReason = 0;
                    if (eReason == tCIDXML::EElemReasons::AsRootElem)
                        errcReason = kXMLErrs::errcXMLV_UndeclElemAsRoot;
                    else if (eReason == tCIDXML::EElemReasons::InContentModel)
                        errcReason = kXMLErrs::errcXMLW_UndeclElemInCM;
                    else if (eReason == tCIDXML::EElemReasons::InAttList)
                        errcReason = kXMLErrs::errcXMLW_UndeclElemInAttr;

                    xprsOwner().PostXMLError(errcReason, xdeclCur.strFullName());
                }

                //
                //  Now lets check its attributes. We have to confirm that attrs
                //  of certain types reference other legal declarations.
                //
                TAttrDefCursor& adcCur = xdeclCur.adcThis();
                if (adcCur.bReset())
                {
                    tCIDLib::TBoolean bIDTrigger = kCIDLib::False;
                    do
                    {
                        const TXMLAttrDef& xadCur = adcCur.xadCur();

                        //
                        //  If this one has a default value, then it has to be
                        //  validated just like as if it were actually used. We
                        //  don't check the return, since all we care about is
                        //  that the errors were issued if they occured.
                        //
                        if (!xadCur.strValue().bIsEmpty())
                            bValidateAttr(xadCur, xadCur.strValue());

                        //
                        //  Look for various attribute types that we have to do
                        //  some special checking on. We do the following checks:
                        //
                        //  1. No more than one ID attribute per element
                        //  2. If its a NOTATION type, then all its possible values
                        //      must refer to defined notations.
                        //
                        const tCIDXML::EAttrTypes eType = xadCur.eType();
                        if (eType == tCIDXML::EAttrTypes::ID)
                        {
                            if (bIDTrigger)
                            {
                                xprsOwner().PostXMLError
                                (
                                    kXMLErrs::errcXMLV_TooManyIDAttrs
                                    , xdeclCur.strFullName()
                                );
                            }

                            // We've seen an ID, so we can't see another
                            bIDTrigger = kCIDLib::True;
                        }
                         else if (eType == tCIDXML::EAttrTypes::Notation)
                        {
                            // Get an elem cursor for the enum values
                            TXMLAttrDef::TCursor cursVals = xadCur.cursEnumValues();
                            if (cursVals.bReset())
                            {
                                do
                                {
                                    // See if this value exists in the notation decl pool
                                    if (!m_pxnipNotations->pobjByName(cursVals.objRCur()))
                                    {
                                        xprsOwner().PostXMLError
                                        (
                                            kXMLErrs::errcXMLV_BadEnumRefAttr
                                            , cursVals.objRCur()
                                            , xadCur.strFullName()
                                        );
                                    }
                                }   while (cursVals.bNext());
                            }
                        }

                    }   while (adcCur.bNext());
                }
            }   while (m_pxnipElems->bNext());

            //
            //  Iterate through the entity decls and look for any that had an
            //  NDATA statement. Those must refer to declared notations.
            //
            if (m_pxnipEntities->bResetIter())
            {
                do
                {
                    const TDTDEntityDecl& xdeclCur = m_pxnipEntities->objCur();

                    if (!xdeclCur.strNotationName().bIsEmpty())
                    {
                        if (!m_pxnipNotations->pobjByName(xdeclCur.strNotationName()))
                        {
                            xprsOwner().PostXMLError
                            (
                                kXMLErrs::errcXMLV_BadEntityNotation
                                , xdeclCur.strName()
                                , xdeclCur.strNotationName()
                            );
                        }
                    }

                }   while (m_pxnipEntities->bNext());
            }
        }
    }
}


//
//  This is used when the parser wants to use a locked validator. So the
//  contents of the validator will remain the same, but we still need to
//  eat the rest of the doctype declaration. This will throw if any
//  internal subset is seen and will ignore any external subset.
//
tCIDLib::TVoid TDTDValidator::ParseDTDDecl()
{
    ParseDOCType(kCIDLib::True);
}


// Reset ourself back to square one
tCIDLib::TVoid TDTDValidator::Reset()
{
    // Flush our decl pools
    m_pxnipElems->RemoveAll();
    m_pxnipEntities->RemoveAll();
    m_pxnipNotations->RemoveAll();
    m_pxnipPEs->RemoveAll();


    // Reset the root element id and ids
    m_c4RootElemId = kCIDLib::c4MaxCard;
    m_strPublicId.Clear();
    m_strSystemId.Clear();

    //
    //  Reload the general entity pool with the standard character entities
    //  that are always automagically there. We indicate that these are special
    //  ones via a parameter, so that we can quickly identify them later.
    //
    TDTDEntityDecl* pxdeclTmp;
    pxdeclTmp = new TDTDEntityDecl(L"amp", L"&");
    pxdeclTmp->bIsSpecial(kCIDLib::True);
    m_pxnipEntities->c4AddNew(pxdeclTmp);

    pxdeclTmp = new TDTDEntityDecl(L"lt", L"<");
    pxdeclTmp->bIsSpecial(kCIDLib::True);
    m_pxnipEntities->c4AddNew(pxdeclTmp);

    pxdeclTmp = new TDTDEntityDecl(L"gt", L">");
    pxdeclTmp->bIsSpecial(kCIDLib::True);
    m_pxnipEntities->c4AddNew(pxdeclTmp);

    pxdeclTmp = new TDTDEntityDecl(L"quot", L"\"");
    pxdeclTmp->bIsSpecial(kCIDLib::True);
    m_pxnipEntities->c4AddNew(pxdeclTmp);

    pxdeclTmp = new TDTDEntityDecl(L"apos", L"'");
    pxdeclTmp->bIsSpecial(kCIDLib::True);
    m_pxnipEntities->c4AddNew(pxdeclTmp);

    // If we have a DTD event handler, reset it also
    if (m_pmxevDTD)
        m_pmxevDTD->ResetDocType();
}

