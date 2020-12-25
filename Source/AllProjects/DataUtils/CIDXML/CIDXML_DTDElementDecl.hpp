//
// FILE NAME: CIDXML_DTDElementDecl.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/31/1999
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
//  This is the header file for the CIDXML_DTDElementDecl.Cpp file, which
//  implements the TDTDElemDecl class. This class derives from the base
//  element declaration class and provides the functionality for a DTD
//  oriented validator's view of an element decl.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TDTDElemDecl;


// ---------------------------------------------------------------------------
//  CLASS: TDTDAttrDefCursor
// PREFIX: adc
// ---------------------------------------------------------------------------
class CIDXMLEXP TDTDAttrDefCursor : public TAttrDefCursor
{
    public :
        // -------------------------------------------------------------------
        //  Public Constructors and Destructor
        // -------------------------------------------------------------------
        TDTDAttrDefCursor
        (
            const   TDTDElemDecl&           xdeclToCursor
        );

        TDTDAttrDefCursor
        (
            const   TDTDAttrDefCursor&      adcSrc
        );

        TDTDAttrDefCursor(TDTDAttrDefCursor&&) = delete;

        ~TDTDAttrDefCursor();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDTDAttrDefCursor& operator=
        (
            const   TDTDAttrDefCursor&      adcSrc
        );

        TDTDAttrDefCursor& operator=(TDTDAttrDefCursor&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bNext() final;

        tCIDLib::TBoolean bReset() final;

        const TXMLAttrDef& xadCur() const final;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_xnipcThis
        //      A name id pool cursor for the pool that the passed element
        //      uses to store its attribute defs.
        // -------------------------------------------------------------------
        TXMLNameIDPoolCursor<TDTDAttrDef>* m_pxnipcThis;
};



// ---------------------------------------------------------------------------
//  CLASS: TDTDElemDecl
// PREFIX: xdecl
// ---------------------------------------------------------------------------
class CIDXMLEXP TDTDElemDecl : public TXMLElemDecl
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strExtractKey
        (
            const   TDTDElemDecl&           xdeclSrc
        );


        // -------------------------------------------------------------------
        //  Public Constructors and Destructor
        // -------------------------------------------------------------------
        TDTDElemDecl() = delete;

        TDTDElemDecl
        (
            const   TString&                strName
            , const tCIDXML::EElemModels    eModel = tCIDXML::EElemModels::Any
        );

        TDTDElemDecl(const TDTDElemDecl&) = delete;
        TDTDElemDecl(TDTDElemDecl&&) = delete;

        ~TDTDElemDecl();


        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TDTDElemDecl& operator=(const TDTDElemDecl&) = delete;
        TDTDElemDecl& operator=(TDTDElemDecl&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        TAttrDefCursor& adcThis() const final;

        tCIDLib::TBoolean bClearAttrFlags() final;

        tCIDXML::EElemTextTypes eTextType() const final;

        tCIDLib::TVoid FormatCMTo
        (
                    TTextOutStream&         strmDest
            , const TXMLValidator&          xvalPools
        )   const final;

        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
            , const TXMLValidator&          xvalPools
        )   const final;

        TXMLAttrDef* pxadFindAttrDef
        (
            const   TString&                strName
        )   final;

        const TXMLAttrDef* pxadFindAttrDef
        (
            const   TString&                strName
        )   const final;

        TXMLAttrDef* pxadFindAttrDef
        (
            const   TString&                strName
            ,       tCIDLib::TBoolean&      bAdded
        )   final;

        const TString& strFullName() const final;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddAttrDef
        (
                    TDTDAttrDef* const      pxadToAdopt
        );

        tCIDLib::TVoid AdoptContentModel
        (
                    TXMLContentModel* const pxcmToAdopt
        );

        tCIDXML::EElemModels eModel() const;

        tCIDXML::EElemModels eModel
        (
            const   tCIDXML::EElemModels    eModel
        );

        tCIDXML::EElemTextTypes eTextType
        (
            const   tCIDXML::EElemTextTypes eType
        );

        tCIDXML::EValidRes eValidate
        (
            const   tCIDLib::TCard4* const  pc4ChildIds
            , const tCIDLib::TCard4         c4ChildCount
            ,       tCIDLib::TCard4&        c4FailedAt
        )   const;

        const TString& strName() const;

        const TString& strName
        (
            const   TString&                strNewName
        );


    protected :
        // -------------------------------------------------------------------
        //  The attr def cursor is our friend
        // -------------------------------------------------------------------
        friend class TDTDAttrDefCursor;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eModel
        //      This is the model of this element, which indicates what type
        //      of content it can hold.
        //
        //  m_eTextType
        //      This is the text type indicator that we return upon request.
        //      It is set by the DTD validator according to various criteria.
        //
        //  m_padcThis
        //      A pointer to a cursor for our attributes. We fault it in
        //      when/if someone asks for it via the adcThis() method.
        //
        //  m_pxcmThis
        //      This is the content model object for this element. It is an
        //      abstraction from which a couple of different specific content
        //      model classes are derived.
        //
        //  m_pxnipAttrList
        //      This is the pool of attributes defined for this element. It
        //      is faulted in only if needed.
        //
        //  m_strName
        //      This is the name of the element. For DTDs, this is just the
        //      literal lexical name as it was seen in the DTD element
        //      declaration.
        // -------------------------------------------------------------------
        tCIDXML::EElemModels            m_eModel;
        tCIDXML::EElemTextTypes         m_eTextType;
        mutable TDTDAttrDefCursor*      m_padcThis;
        TXMLContentModel*               m_pxcmThis;
        TXMLNameIDPool<TDTDAttrDef>*    m_pxnipAttrList;
        TString                         m_strName;
};

#pragma CIDLIB_POPPACK


