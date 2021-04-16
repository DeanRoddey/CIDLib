//
// FILE NAME: CIDJPEG_Image.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/14/2005
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
//  This is the header for the CIDJPEG_Image.cpp file. This file implements
//  the TJPEGImage class, which handles streaming in and out JPEG files. This
//  class is derived from TCIDImage so it can be dealt with polymorphically
//  via that standard image interface.
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
//  CLASS: TJPEGImage
// PREFIX: img
// ---------------------------------------------------------------------------
class CIDJPEGEXP TJPEGImage : public TCIDImage
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TJPEGImage();

        TJPEGImage
        (
            const   TPixelArray&            pixaToSet
        );

        TJPEGImage
        (
            const   TPixelArray&            pixaToSet
            , const TClrPalette&            palToUse
            , const tCIDLib::TCard4         c4ClrsImportant = 0
            , const tCIDLib::TCard4         c4ClrsUsed = 0
        );

        TJPEGImage
        (
            const   TCIDImage&              imgSrc
        );

        TJPEGImage
        (
            const   TJPEGImage&             imgSrc
        );

        TJPEGImage
        (
                    TJPEGImage&&            imgSrc
        );

        ~TJPEGImage();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TJPEGImage& operator=
        (
            const   TJPEGImage&             imgSrc
        );

        TJPEGImage& operator=
        (
                    TJPEGImage&&            imgSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bFastDecode() const;

        tCIDLib::TBoolean bFastDecode
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TBoolean bOptimalEncoding() const;

        tCIDLib::TBoolean bOptimalEncoding
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TCard4 c4CompQuality() const;

        tCIDLib::TCard4 c4CompQuality
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDJPEG::EOutSamples eOutSample() const;

        tCIDJPEG::EOutSamples eOutSample
        (
            const   tCIDJPEG::EOutSamples   eToSet
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bFastDecode
        //      This is runtime only, and indicates whether the decoding
        //      algorithms should be deoptimized for very fast, low quality
        //      loading. This is common for instance with preview windows.
        //
        //  m_bOptimalEncoding
        //      Forces the encoder to do as many passes as required for an
        //      optimal encoding. Slow and consumptive.
        //
        //  m_c4CompQuality
        //      This is runtime only, and indicates what quality level we
        //      should use when writing out. It is 0 to 100.
        //
        //  m_eOutSample
        //      Controls the sampling factors in the output (for YCrPr type
        //      output, which is almost always the case.)
        //
        //  m_pc1DecContext
        //  m_pc1EncContext
        //      This is the internal context data for encoding/decoding. They
        //      are just void pointers at this public level. Note that the
        //      encoding struct has to be mutable so it can be faulted in
        //      inside the StreamTo method.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean           m_bFastDecode;
        tCIDLib::TBoolean           m_bOptimalEncoding;
        tCIDLib::TCard4             m_c4CompQuality;
        tCIDJPEG::EOutSamples       m_eOutSample;
        tCIDLib::TCard1*            m_pc1DecContext;
        mutable tCIDLib::TCard1*    m_pc1EncContext;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TJPEGImage,TCIDImage)
        DefPolyDup(TJPEGImage)
};

#pragma CIDLIB_POPPACK

