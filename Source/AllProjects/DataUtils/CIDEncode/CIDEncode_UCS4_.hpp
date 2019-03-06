//
// FILE NAME: CIDEncode_UCS4.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/19/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
        ~TUCS4Converter();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::EBaseTextFmts eBaseFmt() const;

        tCIDLib::TVoid Reset();


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

        TUCS4Converter
        (
            const   TUCS4Converter&         tcvtToCopy
        );

        TUCS4Converter& operator=
        (
            const   TUCS4Converter&         tcvtToAssign
        );


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
        );

        tCIDLib::TCard4 c4BlockTo
        (
            const   tCIDLib::TCh* const     pszSrc
            , const tCIDLib::TCard4         c4SrcChars
            ,       tCIDLib::TCard1* const  pc1ToFill
            , const tCIDLib::TCard4         c4MaxBytes
            ,       tCIDLib::TCard4&        c4OutBytes
            ,       tCIDLib::TBoolean&      bStop
        );


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TUCS4Converter,TTextConverter)


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

        TUCS4BEConverter
        (
            const   TUCS4BEConverter&       tcvtToCopy
        );

        ~TUCS4BEConverter();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUCS4BEConverter& operator=
        (
            const   TUCS4BEConverter&       tcvtToAssign
        );


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

        TUCS4LEConverter
        (
            const   TUCS4LEConverter&       tcvtToCopy
        );

        ~TUCS4LEConverter();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUCS4LEConverter& operator=
        (
            const   TUCS4LEConverter&       tcvtToAssign
        );


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TUCS4LEConverter,TUCS4Converter)
        DefPolyDup(TUCS4LEConverter)
};

#pragma CIDLIB_POPPACK


