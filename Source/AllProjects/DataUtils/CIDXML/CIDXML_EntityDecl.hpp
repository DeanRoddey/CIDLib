//
// FILE NAME: CIDXML_EntityDecl.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/21/1999
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the header file for the CIDXML_EntityDecl.Cpp file, which
//  implements the TXMLEntityDecl class. This class is the basic abstraction
//  that all entity declarations used by any validator must implement. This
//  is the abstraction through which the core parser can deal with any
//  validator's entity declaration variations.
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
//  CLASS: TXMLEntityDecl
// PREFIX: xdecl
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLEntityDecl : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        ~TXMLEntityDecl();


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
        tCIDLib::TBoolean bIsExternal() const;

        tCIDLib::TBoolean bIsInternal() const;

        tCIDLib::TBoolean bIsSpecial() const;

        tCIDLib::TBoolean bIsSpecial
        (
            const   tCIDLib::TBoolean       bNewValue
        );

        tCIDLib::TBoolean bIsUnparsed() const;

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

        const TString& strValue() const;

        const TString& strValue
        (
            const   TString&                strValue
        );

        const TString& strValue
        (
            const   tCIDLib::TCh* const     pszValue
        );

        const TString& strNotationName() const;

        const TString& strNotationName
        (
            const   TString&                strName
        );

        const TString& strNotationName
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
        TXMLEntityDecl();

        TXMLEntityDecl
        (
            const   TString&                strName
        );

        TXMLEntityDecl
        (
            const   TString&                strName
            , const TString&                strValue
        );

        TXMLEntityDecl
        (
            const   tCIDLib::TCh* const     pszEntityName
        );

        TXMLEntityDecl
        (
            const   tCIDLib::TCh* const     pszEntityName
            , const tCIDLib::TCh* const     pszEntityValue
        );

        TXMLEntityDecl
        (
            const   TXMLEntityDecl&         xdeclToCopy
        );

        TXMLEntityDecl& operator=
        (
            const   TXMLEntityDecl&         xdeclToAssign
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bSpecial
        //      This is set if the entity is one of the special intrinsic
        //      character entities.
        //
        //  m_c4Id
        //      The id of this entity in its pool. This will be set by the
        //      pool when this object is inserted.
        //
        //  m_c4ValueLen
        //      If the entity is internal, then m_pstrValue holds its
        //      value, and this holds its length. Since this is needed in
        //      order to quickly create a spoolers, we do it once and store
        //      it.
        //
        //  m_strName
        //      This is the name of the entity, which is defined in the DTD
        //      or Schema (or whatever type of validator) text.
        //
        //  m_strValue
        //      This is the entity value if the entity is internal. If the
        //      entity is an external one, then this never gets created.
        //
        //  m_strNotationName
        //      If the entity declaration referenced a notation, then it is
        //      stored here. Otherwise its empty.
        //
        //  m_strPublicId
        //      The public id (if any) that was parsed from the entity decl.
        //      It can be empty since only the system id is required.
        //
        //  m_strSystemId
        //      The system id that was parsed from the entity decl. It is
        //      required so it will not be empty in a valid parse.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bSpecial;
        tCIDLib::TCard4     m_c4Id;
        tCIDLib::TCard4     m_c4ValueLen;
        TString             m_strName;
        TString             m_strValue;
        TString             m_strNotationName;
        TString             m_strPublicId;
        TString             m_strSystemId;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXMLEntityDecl,TObject)
};

#pragma CIDLIB_POPPACK

