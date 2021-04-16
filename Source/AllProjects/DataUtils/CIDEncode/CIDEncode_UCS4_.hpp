//
// FILE NAME: CIDEncode_UCS4.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/19/1999
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
//  to and from the UCS-4 encoding, both endians.
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
//   CLASS: TUCS4Converter
//  PREFIX: tcvt
// ---------------------------------------------------------------------------
class CIDENCODEEXP TUCS4Converter : public TTextConverter
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        ~TUCS4Converter() = default;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::EBaseTextFmts eBaseFmt() const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EEndianModes eMode() const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TUCS4Converter
        (
            const   tCIDLib::EEndianModes   eMode
        );

        TUCS4Converter(const TUCS4Converter&) = default;
        TUCS4Converter(TUCS4Converter&&) = default;
        TUCS4Converter& operator=(const TUCS4Converter&) = default;
        TUCS4Converter& operator=(TUCS4Converter&&) = default;


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
        //  Private data members
        //
        //  m_bSwapped
        //      This is set if the mode is opposite of the native endianess.
        //      This tells us whether we need to byte swap the source data.
        //
        //  m_eMode
        //      Indicates which endian mode this instance is doing.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bSwapped;
        tCIDLib::EEndianModes   m_eMode;

        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TUCS4Converter,TTextConverter)
};


// ---------------------------------------------------------------------------
//   CLASS: TUCS4BEConverter
//  PREFIX: tcvt
// ---------------------------------------------------------------------------
class CIDENCODEEXP TUCS4BEConverter : public TUCS4Converter
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TUCS4BEConverter();

        TUCS4BEConverter(const TUCS4BEConverter&) = default;
        TUCS4BEConverter(TUCS4BEConverter&&) = default;

        ~TUCS4BEConverter() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUCS4BEConverter& operator=(const TUCS4BEConverter&) = default;
        TUCS4BEConverter& operator=(TUCS4BEConverter&&) = default;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TUCS4BEConverter,TUCS4Converter)
        DefPolyDup(TUCS4BEConverter)
};



// ---------------------------------------------------------------------------
//   CLASS: TUCS4LEConverter
//  PREFIX: tcvt
// ---------------------------------------------------------------------------
class CIDENCODEEXP TUCS4LEConverter : public TUCS4Converter
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TUCS4LEConverter();

        TUCS4LEConverter(const TUCS4LEConverter&) = default;
        TUCS4LEConverter(TUCS4LEConverter&&) = default;

        ~TUCS4LEConverter() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUCS4LEConverter& operator=(const TUCS4LEConverter&) = default;
        TUCS4LEConverter& operator=(TUCS4LEConverter&&) = default;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TUCS4LEConverter,TUCS4Converter)
        DefPolyDup(TUCS4LEConverter)
};

#pragma CIDLIB_POPPACK


