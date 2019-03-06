//
// FILE NAME: CIDXML_DTDEntityDecl.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/05/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDXML_DTDEntityDecl.Cpp file, which
//  implements the TDTDEntityDecl class. This class is a simple derivative
//  of the core entity decl class, TXMLEntityDecl, that adds all the extra
//  information to make it useful as a DTD style entity decl.
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
//  CLASS: TDTDEntityDecl
// PREFIX: xdecl
// ---------------------------------------------------------------------------
class CIDXMLEXP TDTDEntityDecl : public TXMLEntityDecl
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strExtractKey
        (
            const   TDTDEntityDecl&         xdeclSrc
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDTDEntityDecl
        (
            const   TString&                strEntityName
        );

        TDTDEntityDecl
        (
            const   TString&                strEntityName
            , const TString&                strEntityValue
        );

        TDTDEntityDecl
        (
            const   tCIDLib::TCh* const     pszEntityName
        );

        TDTDEntityDecl
        (
            const   tCIDLib::TCh* const     pszEntityName
            , const tCIDLib::TCh* const     pszEntityValue
        );

        TDTDEntityDecl
        (
            const   TDTDEntityDecl&         xdeclToCopy
        );

        ~TDTDEntityDecl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDTDEntityDecl& operator=
        (
            const   TDTDEntityDecl&         xdeclToAssign
        );


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
            , const TXMLValidator&          xvalPools
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bFromIntSS() const;

        tCIDLib::TBoolean bFromIntSS
        (
            const   tCIDLib::TBoolean       bNewValue
        );

        tCIDLib::TBoolean bIsPE() const;

        tCIDLib::TBoolean bIsPE
        (
            const   tCIDLib::TBoolean       bNewValue
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bFromIntSS
        //      This is set to indicate whether the entity was defined in the
        //      internal subset or not. There are some XML rules that depend
        //      on this info.
        //
        //  m_bIsPE
        //      Indicates whether its a parameter entity or a general entity.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bFromIntSS;
        tCIDLib::TBoolean   m_bIsPE;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDTDEntityDecl,TXMLEntityDecl)
};

#pragma CIDLIB_POPPACK


