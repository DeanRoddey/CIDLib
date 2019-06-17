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
//  An alias definition, used by classes and facility namespaces.
// ---------------------------------------------------------------------------
struct TAlias
{
    THelpNode   m_hnDesc;
    TString     m_strName;
    TString     m_strType;
};

class TAliases
{
    public :
        tCIDLib::TBoolean bIsEmpty() const
        {
            return m_colList.bIsEmpty();
        }

        tCIDLib::TVoid Parse
        (
            const   TXMLTreeElement&        xtnodeSrc
        );

        tCIDLib::TVoid OutputContent
        (
                    TTextOutStream&         strmTar
            , const tCIDDocComp::EVisTypes  eVisType
        )   const;

    private :
        TVector<TAlias>     m_colList;
};



// ---------------------------------------------------------------------------
//  A constant definition. Mostly for a class but we might use it elsewhere.
// ---------------------------------------------------------------------------
struct TConstDef
{
    tCIDDocComp::EParmPB    m_eAccType = tCIDDocComp::EParmPB::Count;
    THelpNode               m_hnDesc;
    TString                 m_strName;
    TString                 m_strType;
    TString                 m_strValue;
};


// ---------------------------------------------------------------------------
//  For enums we just give the name and description. If individual values must
//  be called out, it can be done in the descriptive text.
// ---------------------------------------------------------------------------
struct TEnumDef
{
    THelpNode   m_hnDesc;
    TString     m_strName;
};


// ---------------------------------------------------------------------------
//  We document public and protected members. We need a name, a type, and
//  descriptive text.
// ---------------------------------------------------------------------------
struct TMember
{
    tCIDDocComp::EParmPB    m_eAccType = tCIDDocComp::EParmPB::Count;
    THelpNode               m_hnDesc;
    TString                 m_strName;
    TString                 m_strType;
};

class TMembers
{
    public :
        tCIDLib::TBoolean bIsEmpty() const
        {
            return m_colList.bIsEmpty();
        }

        tCIDLib::TVoid Parse
        (
            const   TXMLTreeElement&        xtnodeSrc
        );

        tCIDLib::TVoid OutputContent
        (
                    TTextOutStream&         strmTar
            , const tCIDDocComp::EVisTypes  eVisType
        )   const;

    private :
        TVector<TMember>    m_colList;
};



// ---------------------------------------------------------------------------
//  A single method parameter
// ---------------------------------------------------------------------------
struct TMethodParam
{
    tCIDLib::TVoid Parse
    (
        const   TXMLTreeElement&        xtnodeGrp
    );
    tCIDLib::TVoid OutputContent
    (
                TTextOutStream&         strmTar
        , const tCIDLib::TBoolean       bIndent
        , const tCIDLib::TCard4         c4Index
    )   const;

    tCIDLib::TBoolean       m_bRetain = kCIDLib::False;
    tCIDDocComp::EParmPB    m_ePassBy = tCIDDocComp::EParmPB::Count;
    tCIDDocComp::EParmDirs  m_eDir = tCIDDocComp::EParmDirs::Count;
    TString                 m_strDefVal;
    TString                 m_strName;
    TString                 m_strType;
};


// ---------------------------------------------------------------------------
//  We can have more than one variation on a method, which shares the same
//  descriptive text. The TMethod class owns a list of these and provides the
//  common text. If we get a return that, we use that. Else we use the one
//  defines for the containing method, which is almost always the case.
// ---------------------------------------------------------------------------
class TMethodVar
{
    public :
        tCIDLib::TVoid Parse
        (
            const   TXMLTreeElement&        xtnodeMeth
            , const TString&                strRetType
            , const tCIDDocComp::EParmPB    eRetBy
            , const tCIDDocComp::EMethAttrs eGrpAttrs
            , const TString&                strTmplParms
        );
        tCIDLib::TVoid OutputContent
        (
                    TTextOutStream&         strmTar
            , const TString&                strName
        )   const;

        tCIDLib::TBoolean       m_bAnyDirs = kCIDLib::False;
        TVector<TMethodParam>   m_colParams;
        tCIDDocComp::EMethAttrs m_eAttrs = tCIDDocComp::EMethAttrs::None;
        tCIDDocComp::EParmPB    m_eRetBy = tCIDDocComp::EParmPB::None;
        TString                 m_strRetType;
        TString                 m_strTmplParms;
};


// ---------------------------------------------------------------------------
//  The overall method description. We have the descriptive text, then the
//  definition of 1 or more variations on this method. We have the method
//  name since all variations share the same name. And we also have default
//  return type info that will be used if a variation doesn't provide one.
// ---------------------------------------------------------------------------
class TMethod
{
    public :
        tCIDLib::TVoid Parse
        (
            const   TXMLTreeElement&        xtnodeMeth
            , const tCIDDocComp::EMethAttrs eGrpAttrs
        );
        tCIDLib::TVoid OutputContent
        (
                    TTextOutStream&         strmTar
        )   const;

        // If a variation has no attributes, ours are used
        TVector<TMethodVar>     m_colMethVars;
        tCIDDocComp::EMethAttrs m_eMethAttrs = tCIDDocComp::EMethAttrs::None;
        tCIDDocComp::EParmPB    m_eRetBy = tCIDDocComp::EParmPB::None;
        THelpNode               m_hnDescr;
        TString                 m_strName;
        TString                 m_strRetType;
        TString                 m_strTmplParms;
};


// ---------------------------------------------------------------------------
//  With each visibility scope, we have one of these each for static, virtual,
//  override, and non-methods.
//
//  We can have group attributes, which are going to apply to all methods in
//  this group. They will be OR'd with any method sepecific ones. We have general
//  descriptive text for this group of methods.
// ---------------------------------------------------------------------------
class  TMethodGrp
{
    public :
        TMethodGrp(const tCIDDocComp::EMethAttrs eGrpAttrs = tCIDDocComp::EMethAttrs::None) :

            m_eGrpAttrs(eGrpAttrs)
        {
        }

        tCIDLib::TBoolean bIsEmpty() const;

        tCIDLib::TVoid Parse
        (
            const   TXMLTreeElement&        xtnodeMeths
        );
        tCIDLib::TVoid OutputContent
        (
                    TTextOutStream&         strmTar
        )   const;


        TVector<TMethod>            m_colMethods;

    private :
        tCIDLib::TVoid OutputSpecMethod
        (
                    TTextOutStream&         strmTar
            , const tCIDDocComp::ESpecMeths eMeth
            , const tCIDLib::TBoolean       bDefault
        )   const;

        tCIDLib::TVoid ParseSpecMethods
        (
            const   TXMLTreeElement&        xtnodePar
            , const TString&                strAttr
            ,       tCIDDocComp::TSpecMeths& fcolLoad
        );


        //
        //  The group attrs are set programmatically based on the type of the
        //  group. Method and variation attrs are set via XML. These are all
        //  combined for the ultimate attributes.
        //
        tCIDDocComp::EMethAttrs     m_eGrpAttrs;
        tCIDDocComp::TSpecMeths     m_fcolDefMethods;
        tCIDDocComp::TSpecMeths     m_fcolDelMethods;
        THelpNode                   m_hnDescr;
};


// ---------------------------------------------------------------------------
//  We have one of these each for public, protected, and private stuff. The
//  bIsEmpty() method lets the output code know if needs to do a section for
//  this visibility type, so we don't end up formatting out just a section header.
// ---------------------------------------------------------------------------
class TMemberGrp
{
    public :
        TMemberGrp() = delete;
        TMemberGrp(const tCIDDocComp::EVisTypes eVisType);

        tCIDLib::TBoolean bIsEmpty() const;
        tCIDLib::TVoid Parse
        (
            const   TXMLTreeElement&        xtnodeMems
        );
        tCIDLib::TVoid OutputContent
        (
                    TTextOutStream&         strmTar
        )   const;

        tCIDDocComp::EVisTypes  m_eVisType;
        TAliases                m_memgAliases;
        TMembers                m_memgMembers;
        TMethodGrp              m_methgCtors;
        TMethodGrp              m_methgNVirtMethods;
        TMethodGrp              m_methgOperators;
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


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TString& strClass() const
        {
            return m_strClass;
        }

        const TString& strPrefix() const
        {
            return m_strPrefix;
        }


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
        )   override;

        tCIDLib::TVoid OutputContent
        (
                    TTextOutStream&         strmTar
        )   const override;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid LoadMixinMethods();

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
        tCIDDocComp::EClsFlags      m_eFlags = tCIDDocComp::EClsFlags::None;
        TMemberGrp                  m_memgPublic;
        TMemberGrp                  m_memgProtected;
        TMemberGrp                  m_memgPrivate;
        THelpNode                   m_hnDesc;
        TString                     m_strClass;
        TString                     m_strParClass;
        TString                     m_strPrefix;

        static tCIDLib::TBoolean    s_bStatsDone;
        static TMethod              s_methDuplicate;
        static TMethod              s_methFormatTo;
        static TMethod              s_methStreamFrom;
        static TMethod              s_methStreamTo;
};
