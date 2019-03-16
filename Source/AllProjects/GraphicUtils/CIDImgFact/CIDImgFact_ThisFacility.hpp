//
// FILE NAME: CIDImgFact_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/02/2013
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
//  This is the header file for the CIDImgFact_ThisFacility.Cpp file. This
//  file implements the facility class for this facility.
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
//  CLASS: TFacCIDImgFact
// PREFIX: fac
// ---------------------------------------------------------------------------
class CIDIMGFACTEXP TFacCIDImgFact : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TFacCIDImgFact();

        TFacCIDImgFact(const TFacCIDImgFact&) = delete;

        ~TFacCIDImgFact();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDImgFact& operator=(const TFacCIDImgFact&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDecodeImg
        (
            const   TMemBuf&                mbufImg
            , const tCIDLib::TCard4         c4ImgSz
            ,       TCIDImage*&             pimgToSet
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bDecodeImg
        (
            const   tCIDLib::TVoid* const   pImgData
            , const tCIDLib::TCard4         c4ImgSz
            ,       TCIDImage*&             pimgToSet
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bDecodeImgTo
        (
            const   TMemBuf&                mbufImg
            , const tCIDLib::TCard4         c4ImgSz
            , const tCIDImage::EImgTypes    eTarType
            ,       TCIDImage*&             pimgToSet
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bDecodeImgTo
        (
            const   tCIDLib::TVoid* const   pImgData
            , const tCIDLib::TCard4         c4ImgSz
            , const tCIDImage::EImgTypes    eTarType
            ,       TCIDImage*&             pimgToSet
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bLoadToBitmap
        (
            const   TString&                strPath
            ,       TBitmapImage&           imgToLoad
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bLoadToBitmap
        (
            const   TMemBuf&                mbufImgData
            , const tCIDLib::TCard4         c4ImgSz
            ,       TBitmapImage&           imgToLoad
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bLoadToJPEG
        (
            const   TString&                strPath
            ,       TJPEGImage&             imgToLoad
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bLoadToJPEG
        (
            const   TMemBuf&                mbufImgData
            , const tCIDLib::TCard4         c4ImgSz
            ,       TJPEGImage&             imgToLoad
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bLoadToJPEG
        (
            const   tCIDLib::TVoid* const   pImgData
            , const tCIDLib::TCard4         c4ImgSz
            ,       TJPEGImage&             imgToLoad
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bLoadToPNG
        (
            const   TString&                strPath
            ,       TPNGImage&              imgToLoad
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bLoadToPNG
        (
            const   TMemBuf&                mbufImgData
            , const tCIDLib::TCard4         c4ImgSz
            ,       TPNGImage&              imgToLoad
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bLoadToPNG
        (
            const   tCIDLib::TVoid* const   pImgData
            , const tCIDLib::TCard4         c4ImgSz
            ,       TPNGImage&              imgToLoad
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        )   const;

        tCIDLib::TCard4 c4CompactToJPEG
        (
                    TMemBuf&                mbufInOut
            , const tCIDLib::TCard4         c4OrgSize
            , const TSize&                  szMaxRes
        )   const;

        tCIDImage::EImgTypes eProbeImg
        (
            const   TMemBuf&                mbufData
            , const tCIDLib::TCard4         c4Bytes
        )   const;

        tCIDImage::EImgTypes eProbeImg
        (
            const   tCIDLib::TVoid* const   pImgData
            , const tCIDLib::TCard4         c4Bytes
        )   const;

        tCIDImage::EImgTypes eTypeFromExt
        (
            const   TString&                strExt
        )   const;

        tCIDLib::TVoid FormatImgFormat
        (
                    TString&                strToFill
            , const tCIDImage::EPixFmts     eFmt
            , const tCIDImage::EBitDepths   eDepth
            , const tCIDLib::TBoolean       bTwoLines = kCIDLib::False
        )   const;

        TCIDImage* pimgCreateNew
        (
            const   tCIDImage::EImgTypes    eType
        )   const;

        TCIDImage* pimgLoadImgFile
        (
            const   TString&                strPath
            ,       tCIDImage::EImgTypes&   eType
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        )   const;

        const TString& strImgTypeExt
        (
            const   tCIDImage::EImgTypes    eType
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strExtXXX
        //      Some strings we pre-load on ctor, used by the strImgTypeExt()
        //      method.
        // -------------------------------------------------------------------
        const TString   m_strExtBmp;
        const TString   m_strExtJPG;
        const TString   m_strExtPNG;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDImgFact,TFacility)
};

#pragma CIDLIB_POPPACK



