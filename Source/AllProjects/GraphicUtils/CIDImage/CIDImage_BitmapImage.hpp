//
// FILE NAME: CIDImage_BitmapImage.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/17/1998
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
//  This is the header for the CIDImage_BitmapImage.cpp file. This file
//  implements the TBitmapImage class, which handles streaming in and out
//  Windows and OS/2 bitmap formats.
//
//  This class is derived from TCIDImage so it can be dealt with
//  polymorphically via that standard image interface.
//
// CAVEATS/GOTCHAS:
//
//  1)  This class does not represent a system bitmap handle. This is a
//      bitmap image, i.e. the actual data that is stored for a bitmap. It is
//      platform independent.
//
//  2)  The default constructor creates an 1x1 two color bitmap with all
//      of the pixels set to the first color.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TBitmapImage
// PREFIX: img
// ---------------------------------------------------------------------------
class CIDIMGEXP TBitmapImage : public TCIDImage
{
    public  :
        // -------------------------------------------------------------------
        //  Public types.
        //
        //  TBmpHdrInfo
        //      This is a raw structure that is used internally to manipulate
        //      the header info sometimes.
        // -------------------------------------------------------------------
        struct TBmpHdrInfo
        {
            tCIDImage::EBmpTypes        eType;
            tCIDImage::EBmpComp  eCompression;
            tCIDImage::ERowOrders       eRowOrder;
            tCIDLib::TCard4             c4BitDepth;
            tCIDLib::TCard4             c4ClrsImportant;
            tCIDLib::TCard4             c4ClrsUsed;
            tCIDLib::TCard4             c4Height;
            tCIDLib::TCard4             c4Width;
            tCIDLib::TCard4             c4HorzRes;
            tCIDLib::TCard4             c4VertRes;
            tCIDLib::TCard4             c4LineByteRounding;
            tCIDLib::TCard4             c4CompressedSize;

            tCIDLib::TCard4             c4RedMask;
            tCIDLib::TCard4             c4GreenMask;
            tCIDLib::TCard4             c4BlueMask;
            tCIDLib::TCard4             c4AlphaMask;
        };


        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bIsValidBmpFmt
        (
            const   TCIDImage&              imgToCheck
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBitmapImage();

        TBitmapImage
        (
            const   TPixelArray&            pixaToSet
            , const TClrPalette&            palToSet
            , const tCIDImage::EBmpTypes    eBmpType = tCIDImage::EBmpTypes::Win_3x
            , const tCIDLib::TCard4         c4ClrsImportant = 0
            , const tCIDLib::TCard4         c4ClrsUsed = 0
        );

        TBitmapImage
        (
            const   TPixelArray&            pixaToSet
            , const tCIDImage::EBmpTypes    eBmpType = tCIDImage::EBmpTypes::Win_3x
            , const tCIDLib::TCard4         c4ClrsImportant = 0
            , const tCIDLib::TCard4         c4ClrsUsed = 0
        );

        TBitmapImage
        (
            const   TCIDImage&              imgSrc
        );

        TBitmapImage
        (
            const   TBitmapImage&           imgToCopy
        );

        ~TBitmapImage();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBitmapImage& operator=
        (
            const   TBitmapImage&           imgToAssign
        );

        TBitmapImage& operator=
        (
            const   TCIDImage&              imgToAssign
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        );

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Load32BitData
        (
                    TBinInStream&           strmFrom
            ,       tCIDLib::TCard1* const  pc1ToLoad
            , const TSize&                  szImg
            , const tCIDLib::TCard4         c4LineWidth
        );

        tCIDLib::TVoid ParseBits
        (
                    TBinInStream&           strmFrom
            , const TBmpHdrInfo&            bhiSrc
        );

        tCIDLib::TVoid ParseHeader
        (
                    TBinInStream&           strmFrom
            , const tCIDLib::TCard2         c2FileType
            ,       TBmpHdrInfo&            bhiToFill
        );

        tCIDLib::TVoid ParsePalette
        (
                    TBinInStream&           strmFrom
        );

        tCIDLib::TVoid ParseRLE4Bits
        (
                    TBinInStream&           strmFrom
            ,       TPixelArray&            pixaToFill
        );

        tCIDLib::TVoid ParseRLE8Bits
        (
                    TBinInStream&           strmFrom
            ,       TPixelArray&            pixaToFill
        );

        tCIDLib::TVoid SetFromBH
        (
            const   TBmpHdrInfo&            bhiToSet
        );

        tCIDLib::TVoid TestBitDepth
        (
            const   tCIDLib::TCard2         c2ToTest
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4CompressedSize
        //      This is the size of the data as it is compressed in the
        //      stored format. If there is no compression, then it will be
        //      set to the uncompressed size.
        //
        //  m_eCompression
        //      The compression of the bitmap this info represents.
        //
        //  m_eBmpType
        //      The type of bitmap that this info represents.
        //
        //  m_c4BlueMask
        //  m_c4GreenMask
        //  m_c4RedMask
        //      These are the masks used when the compression is of the
        //      'bit fields' style.
        // -------------------------------------------------------------------
        tCIDLib::TCard4             m_c4CompressedSize;
        tCIDImage::EBmpComp  m_eCompression;
        tCIDImage::EBmpTypes        m_eBmpType;

        tCIDLib::TCard4             m_c4BlueMask;
        tCIDLib::TCard4             m_c4GreenMask;
        tCIDLib::TCard4             m_c4RedMask;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TBitmapImage,TCIDImage)
        DefPolyDup(TBitmapImage)
};

#pragma CIDLIB_POPPACK


