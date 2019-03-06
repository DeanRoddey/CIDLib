//
// FILE NAME: CIDMetaExtr_AAC.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/24/2007
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the
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
//  CLASS: TCIDMetaExtrAAC
// PREFIX: metex
// ---------------------------------------------------------------------------
class CIDMETAEXTREXP TCIDMetaExtrAAC : public TCIDMetaExtractor
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDMetaExtrAAC();

        ~TCIDMetaExtrAAC();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bExtract
        (
            const   TString&                strFilePath
            ,       TString&                strGenres
            ,       TString&                strDescr
            ,       TString&                strAlbum
            ,       TString&                strAlbumArtist
            ,       TString&                strArtist
            ,       TString&                strTrack
            ,       tCIDLib::TCard4&        c4TrackNum
            ,       tCIDLib::TCard4&        c4Duration
            ,       tCIDLib::TCard4&        c4Year
            ,       tCIDLib::TCard4&        c4BitRate
            ,       tCIDLib::TCard4&        c4Rating
            , const tCIDLib::TBoolean       bWantArt
            ,       TMemBuf&                mbufArt
            ,       tCIDLib::TCard4&        c4ArtSize
        );

        tCIDLib::TBoolean bExtractImage
        (
            const   TString&                strFilePath
            ,       TMemBuf&                mbufArt
            ,       tCIDLib::TCard4&        c4ArtSize
        );

        tCIDLib::TBoolean bProbe
        (
            const   TString&                strFilePath
        );

        const TString& strTypeName() const;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bNextBox
        (
                    TBinInStream&           strmSrc
            ,       tCIDLib::TCard4&        c4Type
            ,       tCIDLib::TCard4&        c4Size
            ,       tCIDLib::TCard4&        c4End
        );

        tCIDLib::TBoolean bSkipToAttrs
        (
                    TBinInStream&           strmSrc
            ,       tCIDLib::TCard4&        c4ILSTEnd
            ,       TMemBuf&                mbufMhdr
            ,       tCIDLib::TCard4&        c4MHdrSz
        );

        tCIDLib::TCard4 c4GetImageAttr
        (
                    TBinInStream&           strmSrc
            ,       TMemBuf&                mbufToFill
            , const tCIDLib::TCard4         c4DataSz
        );

        tCIDLib::TCard4 c4GetUnsignedAttr
        (
                    TBinInStream&           strmSrc
            , const tCIDLib::TCard4         c4DataSz
        );

        tCIDLib::TVoid Get2UnsignedAttrs
        (
                    TBinInStream&           strmSrc
            , const tCIDLib::TCard4         c4DataSz
            ,       tCIDLib::TCard4&        c4Val1
            ,       tCIDLib::TCard4&        c4Val2
        );

        tCIDLib::TVoid GetTextAttr
        (
                    TBinInStream&           strmSrc
            ,       TString&                strToFill
            , const tCIDLib::TCard4         c4DataSz
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_mbufData
        //      We have to read in text values into a buffer so that we can
        //      transcode it.
        //
        //  m_strSrcName
        //      The source name given to us, and used in error messages,
        //      usually the file name.
        //
        //  m_strTmp
        //      Just a temp string to use during parsing
        //
        //  m_strTypeName
        //      Set to our file type name, and returned via strTypeName().
        //
        //  m_tcvtText
        //      The text is UTF8 formatted, so we need a convert to suck it
        //      in.
        // -------------------------------------------------------------------
        THeapBuf        m_mbufData;
        TString         m_strSrcName;
        TString         m_strTmp;
        TString         m_strTypeName;
        TUTF8Converter  m_tcvtText;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDMetaExtrAAC,TCIDMetaExtractor)
};

#pragma CIDLIB_POPPACK


