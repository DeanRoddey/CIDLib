//
// FILE NAME: CIDXML_DTDValidator.hpp
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
//  This is the header file for the CIDXML_DTDValidator.Cpp file, which
//  implements the TDTDValidator class. This class derives from the
//  TXMLValidator class and implements that abstract API in terms of the
//  standard XML DTD validation mechanism.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TXMLEntityMgr;
class TXMLCMSpecNode;


// ---------------------------------------------------------------------------
//  CLASS: TDTDValidator
// PREFIX: xval
// ---------------------------------------------------------------------------
class CIDXMLEXP TDTDValidator : public TXMLValidator
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDTDValidator
        (
                    TXMLParserCore* const   pxprsOwner
            ,       MXMLDTDEvents* const    pmxevToUse = 0
        );

        ~TDTDValidator();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCanParseDTD() const;

        tCIDLib::TBoolean bIsValidRoot
        (
            const   TXMLElemDecl&           xdeclToTest
        );

        tCIDLib::TBoolean bValidateAttr
        (
            const   TXMLAttrDef&            xadToTest
            , const TString&                strValue
        );

        tCIDLib::TBoolean bValidateContent
        (
            const   TXMLElemDecl&           xdeclParent
            , const tCIDLib::TCard4* const  pc4Children
            , const tCIDLib::TCard4         c4ChildCount
        );

        TXMLElemDecl* pxdeclFindElemById
        (
            const   tCIDLib::TCard4         c4Id
        );

        const TXMLElemDecl* pxdeclFindElemById
        (
            const   tCIDLib::TCard4         c4Id
        )   const;

        TXMLElemDecl* pxdeclFindElemByName
        (
            const   TString&                strQName
            , const tCIDXML::EPoolOpts      eOpt
            ,       tCIDLib::TBoolean&      bAdded
        );

        const TXMLElemDecl* pxdeclFindElemByName
        (
            const   TString&                strQName
            , const tCIDXML::EPoolOpts      eOpt
            ,       tCIDLib::TBoolean&      bAdded
        )   const;

        TXMLEntityDecl* pxdeclFindEntityByName
        (
            const   TString&                strName
        );

        TXMLNotationDecl* pxdeclFindNotationByName
        (
            const   TString&                strName
        );

        tCIDLib::TVoid ParseDefExtSS
        (
            const   TString&                strRootElem
        );

        tCIDLib::TVoid ParseDTD
        (
            const   tCIDLib::TBoolean       bIgnoreDTD
        );

        tCIDLib::TVoid ParseDTDDecl();

        tCIDLib::TVoid Reset();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        MXMLDTDEvents* pmxevDTDEventHandler() const;

        MXMLDTDEvents* pmxevDTDEventHandler
        (
                    MXMLDTDEvents* const    pmxevToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TDTDValidator(const TDTDValidator&);
        tCIDLib::TVoid operator=(const TDTDValidator&);


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        //
        //  Some of these grunt work methods here (such comment, PI, id stuff)
        //  is implemented in CIDXML_DTDValidator3.Cpp. The rest of it is in
        //  CIDXML_DTDValidator2.Cpp.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bExpandCharRef
        (
                    tCIDLib::TCh&           chToFill
            ,       tCIDLib::TCh&           chSecond
        );

        tCIDXML::EEntityRes eExpandEntityRef
        (
            const   tCIDLib::TBoolean       bExtOk
            ,       tCIDLib::TCh&           chToFill
            ,       tCIDLib::TCh&           chSecond
            ,       tCIDLib::TBoolean&      bEscaped
            , const tCIDXML::ERefFrom       eFrom
        );

        tCIDLib::TBoolean bHandleWSOrPERef
        (
            const   tCIDXML::EPERefOpts  eOptions
        );

        tCIDLib::TBoolean bParseAttrDefaults
        (
                    TDTDAttrDef&            xadToFill
        );

        tCIDLib::TBoolean bParseChildModel
        (
                    TDTDElemDecl&           xdeclToFill
        );

        tCIDLib::TBoolean bParseDefAttrValue
        (
                    TDTDAttrDef&            xadToFill
        );

        tCIDLib::TBoolean bParseElemCM
        (
                    TDTDElemDecl&           xdeclToFill
        );

        tCIDLib::TBoolean bParseEntityValue
        (
                    TString&                strToFill
            , const tCIDLib::TCh            chQuote
        );

        tCIDLib::TBoolean bParseEnumList
        (
                    TDTDAttrDef&            xadToFill
            , const tCIDLib::TBoolean       bTokens
        );

        tCIDLib::TBoolean bParseIds
        (
                    TString&                strSystemId
            ,       TString&                strPublicId
            , const tCIDXML::EIDTypes       eType
        );

        tCIDLib::TBoolean bParseIntSubset();

        tCIDLib::TBoolean bParseMixedModel
        (
                    TDTDElemDecl&           xdeclToFill
        );

        tCIDLib::TBoolean bParsePublicId
        (
                    TString&                strToFill
        );

        tCIDLib::TBoolean bParseSystemId
        (
                    TString&                strToFill
        );

        tCIDLib::TVoid ExpandPEntityRef
        (
            const   tCIDLib::TBoolean       bParseExt
            , const tCIDXML::EPERefOpts  eOptions
        );

        tCIDLib::TVoid ParseAttrList();

        tCIDLib::TVoid ParseComment();

        tCIDLib::TVoid ParseDOCType
        (
            const   tCIDLib::TBoolean       bDTOnly
        );

        tCIDLib::TVoid ParseElement();

        tCIDLib::TVoid ParseEntity();

        tCIDLib::TVoid ParseExtEntity
        (
            const   tCIDLib::TBoolean       bIncludeSection
        );

        tCIDLib::TVoid ParseIgnoredSection();

        tCIDLib::TVoid ParseMarkupDecl();

        tCIDLib::TVoid ParseNotation();

        tCIDLib::TVoid ParsePI();

        tCIDLib::TVoid ParseTextDecl();

        TXMLCMSpecNode* pxcsnParseCMLevel();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bIgnoreDTD
        //      The caller can tell us to ignore the DTD contents. For an
        //      external subset, we'll just not parse it at all. For an
        //      internal subset, we have to parse it, but then we just flush
        //      it back out again.
        //
        //  m_c4RootElemId
        //      The element id for the root element. If its not been set,
        //      then its set to kCIDXML::c4InvalidElemId.
        //
        //  m_pxadIgnoredAttrDef
        //  m_pxdeclIgnoredElement
        //  m_pxdeclIgnoredEntity
        //  m_pxdeclIgnoredNotation
        //      These are markup decl objects that are used when we encounter
        //      a previously declared markup decl. XML says that the first
        //      one rules and the subsequent ones are ignored. But, to make
        //      the code simpler, we just parse into these objects instead of
        //      a newly created one.
        //
        //  m_pmxevDTD
        //      This is the optional DTD event callback object. If its set,
        //      we will call back on it with markup events.
        //
        //  m_pxnipElems
        //  m_pxnipEntities
        //  m_pxnipNotatinos
        //  m_pxnipPEs
        //      These are the name id pools used to store elements, entities,
        //      notations, etc... I.e. all of the types of markup that we
        //      are responsible for keeping up with.
        //
        //  m_strBasePath
        //      The optional base path for any file or URL file based external
        //      entities.
        //
        //  m_strPublicId
        //  m_strSystemId
        //      These are the ids of any external subset that was found
        //      during the DOCTYPE line processing.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean                   m_bIgnoreDTD;
        tCIDLib::TCard4                     m_c4RootElemId;
        TDTDAttrDef*                        m_pxadIgnoredAttrDef;
        TDTDElemDecl*                       m_pxdeclIgnoredElement;
        TDTDEntityDecl*                     m_pxdeclIgnoredEntity;
        TDTDNotationDecl*                   m_pxdeclIgnoredNotation;
        MXMLDTDEvents*                      m_pmxevDTD;
        TXMLNameIDPool<TDTDElemDecl>*       m_pxnipElems;
        TXMLNameIDPool<TDTDEntityDecl>*     m_pxnipEntities;
        TXMLNameIDPool<TDTDNotationDecl>*   m_pxnipNotations;
        TXMLNameIDPool<TDTDEntityDecl>*     m_pxnipPEs;
        TString                             m_strBasePath;
        TString                             m_strPublicId;
        TString                             m_strSystemId;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDTDValidator,TXMLValidator)
};

#pragma CIDLIB_POPPACK


