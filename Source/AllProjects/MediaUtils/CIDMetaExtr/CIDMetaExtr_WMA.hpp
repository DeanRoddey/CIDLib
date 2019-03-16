//
// FILE NAME: CIDMetaExtr_WMA.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/27/2007
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
//  This file implements the extractor derivative for WMA files. We use the
//  COM interfaces from the media SDKS to pull tag info out.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

struct IWMHeaderInfo3;

// ---------------------------------------------------------------------------
//  CLASS: TCIDMetaExtrWMA
// PREFIX: metex
// ---------------------------------------------------------------------------
class CIDMETAEXTREXP TCIDMetaExtrWMA : public TCIDMetaExtractor
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDMetaExtrWMA();

        ~TCIDMetaExtrWMA();


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
            ,       tCIDLib::TCard4&        c4Bitrate
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


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bLoadImage
        (
                    IWMHeaderInfo3* const   pHdrInfo
            ,       tCIDLib::TCard4&        c4ArtSize
            ,       TMemBuf&                mbufToFill
        );

        tCIDLib::TVoid StatusCallback
        (
            const   tCIDLib::TCard4         c4SysErr
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Result
        //      The status callback leaves the result here. If non-zero, then
        //      the open failed.
        //
        //  m_evStatus
        //      We have to asynchronously wait for a status callback from
        //      the WMF framework (on our StatusCallback method. We block
        //      on this event until then.
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
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4Result;
        TEvent          m_evStatus;
        TString         m_strSrcName;
        TString         m_strTmp;
        TString         m_strTypeName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDMetaExtrWMA,TCIDMetaExtractor)
};

#pragma CIDLIB_POPPACK



