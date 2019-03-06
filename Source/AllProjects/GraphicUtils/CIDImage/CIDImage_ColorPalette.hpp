//
// FILE NAME: CIDImage_ColorPalette.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/24/1993
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDImage_Palette.cpp file. This file provides
//  the TClrPalette class. Palettes can have up to 8192 colors and support a
//  number of convenient palette operations such as rotations, inversion,
//  global color adjustment, etc... Palettes are streamable and duplicable so
//  they mixin the MStreamable and MDuplicable interfaces.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class TStreamLexer;


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TClrPalette
//  PREFIX: pal
// ---------------------------------------------------------------------------
class CIDIMGEXP TClrPalette :

    public TObject, public MStreamable, public MDuplicable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TClrPalette();

        TClrPalette
        (
            const   tCIDImage::EDefPalettes ePalette
        );

        TClrPalette
        (
            const   tCIDLib::TCard4         c4ClrCount
        );

        TClrPalette
        (
            const   tCIDLib::TCard4         c4ClrCnt
            ,       TColCursor<TRGBClr>&    cursInitList
        );

        TClrPalette
        (
            const   TClrPalette&            palToCopy
        );

        ~TClrPalette();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        TClrPalette& operator=
        (
            const   TClrPalette&            palAssign
        );

        tCIDLib::TBoolean operator==
        (
            const   TClrPalette&            palToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TClrPalette&            palToTest
        )   const;

        const TRGBClr& operator[]
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        TRGBClr& operator[]
        (
            const   tCIDLib::TCard4         c4Index
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4BuildGrayScale
        (
            const   tCIDImage::EBitDepths   eDepth
        );

        tCIDLib::TCard4  c4ClrCount() const;

        tCIDLib::TCard4  c4FindColor
        (
            const   TRGBClr&                rgbFind
        )   const;

        tCIDLib::TCard4 c4ParseFromText
        (
            const   TString&                strFile
            , const tCIDImage::EPalTxtFmts  eFormat = tCIDImage::EPalTxtFmts::SpaceDelimited
            ,       TTextConverter* const   ptcvtToAdopt = 0
        );

        tCIDLib::TCard4 c4ParseFromText
        (
                    TStreamLexer&           lexrSrc
            , const tCIDImage::EPalTxtFmts  eFormat = tCIDImage::EPalTxtFmts::SpaceDelimited
        );

        tCIDLib::TVoid CopyNFrom
        (
            const   TClrPalette&            palSrc
            , const tCIDLib::TCard4         c4CopyUpTo
        );

        tCIDLib::TVoid CvtToGrayScale();

        const TRGBClr& rgbAt
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        TRGBClr& rgbAt
        (
            const   tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid ResetSize
        (
            const   tCIDLib::TCard4         c4NewSize
        );

        tCIDLib::TVoid RotateBackward
        (
            const   tCIDLib::TCard4         c4Count = 1
        );

        tCIDLib::TVoid RotateForward
        (
            const   tCIDLib::TCard4         c4Count = 1
        );

        tCIDLib::TVoid SetAt
        (
            const   tCIDLib::TCard4         c4At
            , const tCIDLib::TCard1         c1Red
            , const tCIDLib::TCard1         c1Green
            , const tCIDLib::TCard1         c1Blue
        );

        tCIDLib::TVoid SetAt
        (
            const   tCIDLib::TCard4         c4At
            , const TRGBClr&                rgbToSet
        );


    protected   :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToFormat
        );

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToFormat
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  NOTE:   m_c4ClrCount MUST be first when stored, for persistence
        //          reasons. It is required so that the count can be read
        //          first and then enough space allocated.
        //
        //  m_c4ClrCount
        //      This is the number of colors in the palette. It is set at
        //      construction time and never changed.
        //
        //  m_pargbClrs
        //      This is a pointer to the array of colors allocated by the
        //      constructor.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4ClrCount;
        TRGBClr*                m_pargbClrs;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TClrPalette,TObject)
        DefPolyDup(TClrPalette)
        NulObject(TClrPalette)
};

#pragma CIDLIB_POPPACK


