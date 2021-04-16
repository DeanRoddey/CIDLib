//
// FILE NAME: CIDLib_UTFConverter.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/17/1999
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
//  to and from the family of encodings that fall within the UTF-8/16 encoding
//  family. This includes UTF-8, ASCII, and UTF-16 in either byte order.
//
//  This is the converter that's used by the text streams if none is given
//  explicitly. This one will try to autosense what the encoding is if an
//  explicit encoding is not provided (by looking for a BOM.)
//
//  There are a couple of simple derivatives that specifically use a
//  particular sub-encoding. The reason for this is that at the CIDEncode
//  facility level, user encoding names are mapped to classes which are then
//  dynamically created. So there is no way to provide the enum parameter.
//  Having separate classes for each makes this workable, but they add no
//  functionality to the base class.
//
//  Thread Safety
//
//  As long as no one changes the setup (rep char, error action) these converters
//  are completely stateless and can be used by multiple threads simultaneously.
//  They work purely on their parameters and only read the (fundamentally typed)
//  setup members.
//
//  Though it is somewhat of a risk, since it's vaguely possible that this promise
//  cannot be kept into the indefinite future, there's no current reason to think
//  this will ever change.
//
// CAVEATS/GOTCHAS:
//
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TUTFConverter
//  PREFIX: tcvt
// ---------------------------------------------------------------------------
class CIDLIBEXP TUTFConverter : public TTextConverter
{
    public :
        // -------------------------------------------------------------------
        //  Class types
        // -------------------------------------------------------------------
        enum class EEncodings
        {
            USASCII
            , UTF8
            , UTF16_LE
            , UTF16_BE

            , Count

            #if defined(CIDLIB_LITTLEENDIAN)
            , Def16 = UTF16_LE
            #endif

            #if defined(CIDLIB_BIGENDIAN)
            , Def16 = UTF16_BE
            #endif
        };


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TUTFConverter
        (
            const   EEncodings              eEncoding = EEncodings::UTF8
        );

        TUTFConverter(const TUTFConverter&) = default;
        TUTFConverter(TUTFConverter&&) = default;

        ~TUTFConverter() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUTFConverter& operator=(const TUTFConverter&) = default;
        TUTFConverter& operator=(TUTFConverter&&) = default;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::EBaseTextFmts eBaseFmt() const final;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        TUTFConverter::EEncodings eEncoding() const;


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
        //  Private data members
        //
        //  m_eEncoding
        //      The specific type of encoding within the UTF family that this
        //      converter deals with.
        // -------------------------------------------------------------------
        EEncodings  m_eEncoding;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TUTFConverter,TTextConverter)
        DefPolyDup(TUTFConverter)
};


// ---------------------------------------------------------------------------
//   CLASS: TUSASCIIConverter
//  PREFIX: tcvt
// ---------------------------------------------------------------------------
class CIDLIBEXP TUSASCIIConverter: public TUTFConverter
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TUSASCIIConverter() : TUTFConverter(TUTFConverter::EEncodings::USASCII)
        {
        }

        TUSASCIIConverter(const TUSASCIIConverter&) = default;
        TUSASCIIConverter(TUSASCIIConverter&&) = default;

        ~TUSASCIIConverter() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUSASCIIConverter& operator=(const TUSASCIIConverter&) = default;
        TUSASCIIConverter& operator=(TUSASCIIConverter&&) = default;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TUSASCIIConverter,TUTFConverter)
        DefPolyDup(TUSASCIIConverter)
};


// ---------------------------------------------------------------------------
//   CLASS: TUTF16BEConverter
//  PREFIX: tcvt
// ---------------------------------------------------------------------------
class CIDLIBEXP TUTF16BEConverter: public TUTFConverter
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TUTF16BEConverter() : TUTFConverter(TUTFConverter::EEncodings::UTF16_BE)
        {
        }

        TUTF16BEConverter(const TUTF16BEConverter&) = default;
        TUTF16BEConverter(TUTF16BEConverter&&) = default;

        ~TUTF16BEConverter() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUTF16BEConverter& operator=(const TUTF16BEConverter&) = default;
        TUTF16BEConverter& operator=(TUTF16BEConverter&&) = default;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TUTF16BEConverter,TUTFConverter)
        DefPolyDup(TUTF16BEConverter)
};


// ---------------------------------------------------------------------------
//   CLASS: TUTF16LEConverter
//  PREFIX: tcvt
// ---------------------------------------------------------------------------
class CIDLIBEXP TUTF16LEConverter: public TUTFConverter
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TUTF16LEConverter() : TUTFConverter(TUTFConverter::EEncodings::UTF16_LE)
        {
        }

        TUTF16LEConverter(const TUTF16LEConverter&) = default;
        TUTF16LEConverter(TUTF16LEConverter&&) = default;

        ~TUTF16LEConverter() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUTF16LEConverter& operator=(const TUTF16LEConverter&) = default;
        TUTF16LEConverter& operator=(TUTF16LEConverter&&) = default;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TUTF16LEConverter,TUTFConverter)
        DefPolyDup(TUTF16LEConverter)
};


// ---------------------------------------------------------------------------
//   CLASS: TUTF8Converter
//  PREFIX: tcvt
// ---------------------------------------------------------------------------
class CIDLIBEXP TUTF8Converter: public TUTFConverter
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TUTF8Converter() :TUTFConverter(TUTFConverter::EEncodings::UTF8)
        {
        }

        TUTF8Converter(const TUTF8Converter&) = default;
        TUTF8Converter(TUTF8Converter&&) = default;

        ~TUTF8Converter() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUTF8Converter& operator=(const TUTF8Converter&) = default;
        TUTF8Converter& operator=(TUTF8Converter&&) = default;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TUTF8Converter,TUTFConverter)
        DefPolyDup(TUTF8Converter)
};


#pragma CIDLIB_POPPACK


