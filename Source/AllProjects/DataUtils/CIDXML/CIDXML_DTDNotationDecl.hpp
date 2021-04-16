//
// FILE NAME: CIDXML_DTDNotationDecl.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/07/1999
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
//  This is the header file for the CIDXML_DTDNotationDecl.Cpp file, which
//  implements the TDTDNotationDecl class. This class is a simple derivative
//  of TXMLNotationDecl that defines a DTD style notation declaration.
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
//  CLASS: TDTDNotationDecl
// PREFIX: xdecl
// ---------------------------------------------------------------------------
class CIDXMLEXP TDTDNotationDecl : public TXMLNotationDecl
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strExtractKey
        (
            const   TDTDNotationDecl&       xdeclSrc
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDTDNotationDecl();

        TDTDNotationDecl
        (
            const   TString&                strName
        );

        TDTDNotationDecl
        (
            const   TString&                strName
            , const TString&                strPublicId
            , const TString&                strSystemId
        );

        TDTDNotationDecl
        (
            const   tCIDLib::TCh* const     pszName
            , const tCIDLib::TCh* const     pszPublicId
            , const tCIDLib::TCh* const     pszSystemId
        );

        TDTDNotationDecl(const TDTDNotationDecl&) = default;
        TDTDNotationDecl(TDTDNotationDecl&&) = default;

        ~TDTDNotationDecl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDTDNotationDecl& operator=(const TDTDNotationDecl&) = default;
        TDTDNotationDecl& operator=(TDTDNotationDecl&&) = default;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
            , const TXMLValidator&          xvalPools
        )   const final;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDTDNotationDecl,TXMLNotationDecl)
};


#pragma CIDLIB_POPPACK

