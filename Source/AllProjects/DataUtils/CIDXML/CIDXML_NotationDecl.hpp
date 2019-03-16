//
// FILE NAME: CIDXML_NotationDecl.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/06/1999
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
//  This is the header file for the CIDXML_NotationDecl.Cpp file, which
//  implements the abstract TXMLNotationDecl class. This class represents the
//  base semantics of notation declarations. Each validator will derive its
//  own class to represent a notation declaration in its own style.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TXMLNotationDecl
// PREFIX: xdecl
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLNotationDecl : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        ~TXMLNotationDecl();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
            , const TXMLValidator&          xvalPools
        )   const = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Id() const;

        tCIDLib::TCard4 c4Id
        (
            const   tCIDLib::TCard4         c4NewId
        );

        const TString& strName() const;

        const TString& strName
        (
            const   TString&                strName
        );

        const TString& strName
        (
            const   tCIDLib::TCh* const     pszName
        );

        const TString& strPublicId() const;

        const TString& strPublicId
        (
            const   TString&                strId
        );

        const TString& strPublicId
        (
            const   tCIDLib::TCh* const     pszId
        );

        const TString& strSystemId() const;

        const TString& strSystemId
        (
            const   TString&                strId
        );

        const TString& strSystemId
        (
            const   tCIDLib::TCh* const     pszId
        );


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TXMLNotationDecl();

        TXMLNotationDecl
        (
            const   TString&                strName
        );

        TXMLNotationDecl
        (
            const   TString&                strName
            , const TString&                strPublicId
            , const TString&                strSystemId
        );

        TXMLNotationDecl
        (
            const   tCIDLib::TCh* const     pszName
            , const tCIDLib::TCh* const     pszPublicId
            , const tCIDLib::TCh* const     pszSystemId
        );

        TXMLNotationDecl
        (
            const   TXMLNotationDecl&       xdeclToCopy
        );

        TXMLNotationDecl& operator=
        (
            const   TXMLNotationDecl&       xdeclToAssign
        );



    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Id
        //      The id of this entity in its pool. This will be set by the
        //      pool when this object is inserted.
        //
        //  m_strName
        //      This is the name of the notation, which is defined in the DTD
        //      or Schema (or whatever type of validator) text.
        //
        //  m_strPublicId
        //      The public id of the notation, if any. There will be either
        //      a system of public id, but there does not have to be both.
        //
        //  m_strSystemId
        //      The system id that was parsed from the notation decl, if any.
        //      There will be either a system or public id, but there does
        //      have to be both.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4Id;
        TString         m_strName;
        TString         m_strPublicId;
        TString         m_strSystemId;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXMLNotationDecl,TObject)
};


#pragma CIDLIB_POPPACK


