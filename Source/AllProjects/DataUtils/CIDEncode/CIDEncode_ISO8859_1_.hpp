//
// FILE NAME: CIDEncode_ISO8859_1.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/29/1999
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
//  This header defines a derivative of TTextConverter that handles conversion
//  to and from the ISO-8859-1 code page, aka Latin-1. This is widely used,
//  in particular by Windows operating systems in the US and some European
//  countries.
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
//   CLASS: TLatin1Converter
//  PREFIX: tcvt
// ---------------------------------------------------------------------------
class CIDENCODEEXP TLatin1Converter : public TTextConverter
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TLatin1Converter();

        TLatin1Converter(const TLatin1Converter&) = default;
        TLatin1Converter(TLatin1Converter&&) = default;

        ~TLatin1Converter() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TLatin1Converter& operator=(const TLatin1Converter&) = default;
        TLatin1Converter& operator=(TLatin1Converter&&) = default;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::EBaseTextFmts eBaseFmt() const override;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4BlockFrom
        (
            const   tCIDLib::TCard1* const  pc1Src
            , const tCIDLib::TCard4         c4SrcBytes
            ,       tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
            ,       tCIDLib::TCard4&        c4OutChars
            ,       tCIDLib::TBoolean&      bStop
        )   final;

        tCIDLib::TCard4 c4BlockTo
        (
            const   tCIDLib::TCh* const     pszSrc
            , const tCIDLib::TCard4         c4SrcChars
            ,       tCIDLib::TCard1* const  pc1ToFill
            , const tCIDLib::TCard4         c4MaxBytes
            ,       tCIDLib::TCard4&        c4OutBytes
            ,       tCIDLib::TBoolean&      bStop
        )   final;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TLatin1Converter,TTextConverter)
        DefPolyDup(TLatin1Converter)
};

#pragma CIDLIB_POPPACK


