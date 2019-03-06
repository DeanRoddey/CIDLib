//
// FILE NAME: CIDXML_DTDElementDecl.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/31/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
            const   TDTDAttrDefCursor&      adcToCopy
        );

        ~TDTDAttrDefCursor();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDTDAttrDefCursor& operator=
        (
            const   TDTDAttrDefCursor&      adcToAssign
        );


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bNext();
        tCIDLib::TBoolean bReset();
        const TXMLAttrDef& xadCur() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_xnipcThis
        //      A name id pool cursor for the pool that the passed element
        //      uses to store its attribute defs.
        // -------------------------------------------------------------------
        TXMLNameIDPoolCursor<TDTDAttrDef>* m_pxnipcThis;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDTDAttrDefCursor,TAttrDefCursor)
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
        TDTDElemDecl
        (
            const   TString&                strName
            , const tCIDXML::EElemModels    eModel = tCIDXML::EElemModels::Any
        );

        ~TDTDElemDecl();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        TAttrDefCursor& adcThis() const;

        tCIDLib::TBoolean bClearAttrFlags();

        tCIDXML::EElemTextTypes eTextType() const;

        tCIDXML::EElemTextTypes eTextType
        (
            const   tCIDXML::EElemTextTypes eType
        );

        tCIDLib::TVoid FormatCMTo
        (
                    TTextOutStream&         strmDest
            , const TXMLValidator&          xvalPools
        )   const;

        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
            , const TXMLValidator&          xvalPools
        )   const;

        TXMLAttrDef* pxadFindAttrDef
        (
            const   TString&                strName
        );

        const TXMLAttrDef* pxadFindAttrDef
        (
            const   TString&                strName
        )   const;

        TXMLAttrDef* pxadFindAttrDef
        (
            const   TString&                strName
            ,       tCIDLib::TBoolean&      bAdded
        );

        const TString& strFullName() const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid TDTDElemDecl::AddAttrDef
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
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TDTDElemDecl();
        TDTDElemDecl(const TDTDElemDecl&);
        tCIDLib::TVoid operator=(const TDTDElemDecl&);


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


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDTDElemDecl,TXMLElemDecl)
};

#pragma CIDLIB_POPPACK


