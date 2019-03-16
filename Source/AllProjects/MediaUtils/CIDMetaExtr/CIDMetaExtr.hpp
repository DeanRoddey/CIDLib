//
// FILE NAME: CIDMetaExtr.hpp
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
//  This is the main public header for the facility. It is the single point
//  of contact for the outside world. By including it, they get what we
//  have to offer.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Set up our import/export attributes
// ---------------------------------------------------------------------------
#if     defined(PROJ_CIDMETAEXTR)
#define CIDMETAEXTREXP  CID_DLLEXPORT
#else
#define CIDMETAEXTREXP  CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"

// ---------------------------------------------------------------------------
//  This one isn't normally included, so we have to do it manually
// ---------------------------------------------------------------------------
#if         !defined(CIDKERNEL_REMMEDIA_HPP)
#include    "CIDKernel_RemMedia.hpp"
#endif



// ---------------------------------------------------------------------------
//  Non-class facility constants
// ---------------------------------------------------------------------------
namespace kCIDMetaExtr
{
    // -----------------------------------------------------------------------
    //  The MP3 ID3 genres, which are stored in the file as numbers that
    //  index into this list.
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const apszID3Genres[] =
    {
        L""
        , L"Blues"
        , L"Classic Rock"
        , L"Country"
        , L"Dance"
        , L"Disco"
        , L"Funk"
        , L"Grunge"
        , L"Hip-Hop"
        , L"Jazz"
        , L"Metal"
        , L"New Age"
        , L"Oldies"
        , L"Other"
        , L"Pop"
        , L"R&B"
        , L"Rap"
        , L"Reggae"
        , L"Rock"
        , L"Techno"
        , L"Industrial"
        , L"Alternative"
        , L"Ska"
        , L"Death Metal"
        , L"Pranks"
        , L"Soundtrack"
        , L"Euro-Techno"
        , L"Ambient"
        , L"Trip-Hop"
        , L"Vocal"
        , L"Jazz+Funk"
        , L"Fusion"
        , L"Trance"
        , L"Classical"
        , L"Instrumental"
        , L"Acid"
        , L"House"
        , L"Game"
        , L"Sound Clip"
        , L"Gospel"
        , L"Noise"
        , L"AlternRock"
        , L"Bass"
        , L"Soul"
        , L"Punk"
        , L"Space"
        , L"Meditative"
        , L"Instrumental Pop"
        , L"Instrumental Rock"
        , L"Ethnic"
        , L"Gothic"
        , L"Darkwave"
        , L"Techno-Industrial"
        , L"Electronic"
        , L"Pop-Folk"
        , L"Eurodance"
        , L"Dream"
        , L"Southern Rock"
        , L"Comedy"
        , L"Cult"
        , L"Gangsta"
        , L"Top 40"
        , L"Christian Rap"
        , L"Pop/Funk"
        , L"Jungle"
        , L"Native American"
        , L"Cabaret"
        , L"New Wave"
        , L"Psychadelic"
        , L"Rave"
        , L"Showtunes"
        , L"Trailer"
        , L"Lo-Fi"
        , L"Tribal"
        , L"Acid Punk"
        , L"Acid Jazz"
        , L"Polka"
        , L"Retro"
        , L"Musical"
        , L"Rock & Roll"
        , L"Hard Rock"

        // Win-amp extension
        , L"Folk"
        , L"Folk-Rock"
        , L"National Folk"
        , L"Swing"
        , L"Fast Fusion"
        , L"Bebob"
        , L"Latin"
        , L"Revival"
        , L"Celtic"
        , L"Bluegrass"
        , L"Avantgarde"
        , L"Gothic Rock"
        , L"Progressive Rock"
        , L"Psychedelic Rock"
        , L"Symphonic Rock"
        , L"Slow Rock"
        , L"Big Band"
        , L"Chorus"
        , L"Easy Listening"
        , L"Acoustic"
        , L"Humour"
        , L"Speech"
        , L"Chanson"
        , L"Opera"
        , L"Chamber Music"
        , L"Sonata"
        , L"Symphony"
        , L"Booty Bass"
        , L"Primus"
        , L"Porn Groove"
        , L"Satire"
        , L"Slow Jam"
        , L"Club"
        , L"Tango"
        , L"Samba"
        , L"Folklore"
        , L"Ballad"
        , L"Power Ballad"
        , L"Rhythmic Soul"
        , L"Freestyle"
        , L"Duet"
        , L"Punk Rock"
        , L"Drum Solo"
        , L"A capella"
        , L"Euro-House"
        , L"Dance Hall"
        , L"Goa"
        , L"Drum & Bass"
        , L"Club-House"
        , L"Hardcore"
        , L"Terror"
        , L"Indie"
        , L"Britpop"
        , L"Negerpunk"
        , L"Polsk Punk"
        , L"Beat"
        , L"Christian Gangsta Rap"
        , L"Heavy Metal"
        , L"Black Metal"
        , L"Crossover"
        , L"Contemporary Christian"
        , L"Christian Rock "
        , L"Merengue"
        , L"Salsa"
        , L"Trash Metal"
        , L"Anime"
        , L"JPop"
        , L"Synthpop"
    };
    const tCIDLib::TCard4 c4ID3GenreCnt = tCIDLib::c4ArrayElems(apszID3Genres);
}


// ---------------------------------------------------------------------------
//  Subinclude our other headers, in the needed order.
// ---------------------------------------------------------------------------
#include    "CIDMetaExtr_ErrorIds.hpp"
#include    "CIDMetaExtr_Shared.hpp"
#include    "CIDMetaExtr_Type.hpp"

#include    "CIDMetaExtr_MetaExtractor.hpp"
#include    "CIDMetaExtr_AAC.hpp"
#include    "CIDMetaExtr_WMA.hpp"

#include    "CIDMetaExtr_BlurayID.hpp"
#include    "CIDMetaExtr_DVDID.hpp"
#include    "CIDMetaExtr_Sony777DVDID.hpp"

#include    "CIDMetaExtr_ThisFacility.hpp"



// ---------------------------------------------------------------------------
//  Export the facility object lazy evaluator function. This guy is in the
//  main module, as usual.
// ---------------------------------------------------------------------------
extern CIDMETAEXTREXP TFacCIDMetaExtr& facCIDMetaExtr();


