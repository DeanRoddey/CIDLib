//
// FILE NAME: CIDDocComp_CppClass.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/30/2019
//
// COPYRIGHT: 2015..2015, Charmed Quark Systems, Ltd
//
//  This file and its contents are the property of Charmed Quark Systems, Ltd. It
//  forms part of the internal implementation of the Charmed Quark Controller
//  product. It contains proprietary source code and is not to be distributed
//  publically in any way, shape, or form.
//
// DESCRIPTION:
//
//  This is a specialized page derivative for C++ classes. It's a fairly complex
//  bunch of data, but the biggest parts are just replications of the same info
//  for public, protected, and private content. For each of those visibility
//  scopes we need nested enums, nested fundamental types, aliases, data members,
//  and methods.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
#pragma once


// ---------------------------------------------------------------------------
//  Some small helper structs to hold info related to methods, with a parser
//  method that lets them parse their content out of a passed XML element. Each
//  layer calls the contained layers to parse our way down the hierarchy.
// ---------------------------------------------------------------------------
struct TMethodParam
{
    static tCIDLib::TBoolean bIsDirParm
    (
        const   tCIDDocComp::EParmPB    eTest
    );

    tCIDLib::TVoid Parse
    (
        const   TXMLTreeElement&        xtnodeGrp
        ,       TParseCtx&              ctxToUse
    );
    tCIDLib::TVoid OutputContent
    (
                TTextOutStream&         strmTar
        , const tCIDLib::TBoolean       bIndent
    )   const;

    tCIDDocComp::EParmPB    m_ePassBy = tCIDDocComp::EParmPB::Count;
    tCIDDocComp::EParmDirs  m_eDir = tCIDDocComp::EParmDirs::Count;
    TString                 m_strDefVal;
    TString                 m_strName;
    TString                 m_strType;
};


class TMethod
{
    public :
        tCIDLib::TVoid Parse
        (
            const   TXMLTreeElement&        xtnodeMeth
            ,       TParseCtx&              ctxToUse
        );
        tCIDLib::TVoid OutputContent
        (
                    TTextOutStream&         strmTar
        )   const;

        //
        //  Attrs is stuff like const, constexpr, nodiscard, anything else like
        //  that. AnyDirs is set if any of the parameters are non-const Ref or
        //  pointer to ref, or other where we want to indicate the direction.
        //
        tCIDLib::TBoolean       m_bAnyDirs = kCIDLib::False;
        tCIDDocComp::EMethAttrs m_eAttrs = tCIDDocComp::EMethAttrs::None;
        tCIDLib::TStrList       m_colNames;
        TVector<TMethodParam>   m_colParams;
        THelpNode               m_hnDescr;
        TString                 m_strRetType;

    private :
        tCIDLib::TVoid OutputSignature
        (
                    TTextOutStream&         strmTar
            , const TString&                strName
        )   const;
};


// One each for static, virtual, override, non-virtual
struct TMethodGrp
{
    tCIDLib::TBoolean bIsEmpty() const
    {
        return m_colMethods.bIsEmpty() && m_hnDescr.bIsEmpty();
    }

    tCIDLib::TVoid Parse
    (
        const   TXMLTreeElement&        xtnodeMeths
        ,       TParseCtx&              ctxToUse
    );
    tCIDLib::TVoid OutputContent
    (
                TTextOutStream&         strmTar
    )   const;

    TVector<TMethod>        m_colMethods;
    THelpNode               m_hnDescr;
};


//
//  We have one of these each for public, protected, and private stuff. The empty
//  method lets the output code know if needs to do a section for this visibility
//  type.
//
struct TMemberGrp
{
    TMemberGrp() = delete;
    TMemberGrp(const tCIDDocComp::EVisTypes eVisType)

        : m_eVisType(eVisType)
    {
    }

    tCIDLib::TBoolean bIsEmpty() const;
    tCIDLib::TVoid Parse
    (
        const   TXMLTreeElement&        xtnodeMems
        ,       TParseCtx&              ctxToUse
    );
    tCIDLib::TVoid OutputContent
    (
                TTextOutStream&         strmTar
    )   const;

    tCIDDocComp::EVisTypes  m_eVisType;
    TMethodGrp              m_methgNVirtMethods;
    TMethodGrp              m_methgOverMethods;
    TMethodGrp              m_methgStatMethods;
    TMethodGrp              m_methgVirtMethods;
};


// ---------------------------------------------------------------------------
//   CLASS: TCppClassPage
//  PREFIX: pg
// ---------------------------------------------------------------------------
class TCppClassPage : public TBasePage
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TCppClassPage() = delete;

        TCppClassPage
        (
            const   TString&                strTitle
            , const TString&                strParSrcDir
            , const TString&                strParTopic
            , const TString&                strFileName
        );

        ~TCppClassPage() = default;


    private :
        // -------------------------------------------------------------------
        //  Private, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bFlagOn
        (
            const   tCIDDocComp::EClsFlags  eCheck
        )   const;

        tCIDLib::TVoid Parse
        (
                    TTopic&                 topicParent
            , const TXMLTreeElement&        xtnodeRoot
            ,       TParseCtx&              ctxToUse
        )   override;

        tCIDLib::TVoid OutputContent
        (
                    TTextOutStream&         strmTar
        )   const override;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ParseMemberGrp
        (
            const   TXMLTreeElement&        xtnodeGrp
            ,       TMemberGrp&             memgTar
            ,       TParseCtx&              ctxToUse
        );

        tCIDLib::TVoid OutputMemberGrp
        (
                    TTextOutStream&         strmTar
            , const TString&                strTitle
            , const TMemberGrp&             memgSrc
        )   const;




        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eFlags
        //      These indicate various optional things that the class may or may
        //      not implement.
        //
        //  m_hnDesc
        //      The overall class description
        //
        //  m_memgXXX
        //      We have storage for the public, protected, and private members.
        //
        //  m_strClass
        //  m_strParClass
        //      Our class and our parent class. Parent class can be empty since not
        //      everything derives from TObject, and of course TObject doesn't have a
        //      parent.
        // -------------------------------------------------------------------
        tCIDDocComp::EClsFlags  m_eFlags = tCIDDocComp::EClsFlags::None;
        TMemberGrp              m_memgPublic;
        TMemberGrp              m_memgProtected;
        TMemberGrp              m_memgPrivate;
        THelpNode               m_hnDesc;
        TString                 m_strClass;
        TString                 m_strParClass;
};
