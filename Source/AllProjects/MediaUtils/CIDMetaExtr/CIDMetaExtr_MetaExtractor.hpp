//
// FILE NAME: CIDMetaExtr_MetaExtractor.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/24/2007
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
//  This file implements the base extractor interface from which we create
//  derivatives for each file type we want to extract from.
//
// CAVEATS/GOTCHAS:
//
//  1)  The track number should be returned zero based, so adjust if the
//      embedded metadata has it 1 based.
//
//  2)  Durations are in seconds.
//
//  3)  If more than one genre is supported, return it as a comma separated
//      list. Genre names cannot therefore have commas in them, so replace
//      them.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TCIDMetaExtractor
// PREFIX: metex
// ---------------------------------------------------------------------------
class CIDMETAEXTREXP TCIDMetaExtractor : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDMetaExtractor();

        ~TCIDMetaExtractor();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bExtract
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
        ) = 0;

        virtual tCIDLib::TBoolean bExtractImage
        (
            const   TString&                strFilePath
            ,       TMemBuf&                mbufArt
            ,       tCIDLib::TCard4&        c4ArtSize
        ) = 0;

        virtual tCIDLib::TBoolean bProbe
        (
            const   TString&                strFilePath
        ) = 0;

        virtual const TString& strTypeName() const = 0;


    protected :
        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard2 c2Extract
        (
                    TBinInStream&           strmSrc
        );

        tCIDLib::TCard4 c4Extract
        (
                    TBinInStream&           strmSrc
        );

        tCIDLib::TCard8 c8Extract
        (
                    TBinInStream&           strmSrc
        );

        tCIDLib::TVoid ThrowNotOfType();


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDMetaExtractor,TObject)
};

#pragma CIDLIB_POPPACK


