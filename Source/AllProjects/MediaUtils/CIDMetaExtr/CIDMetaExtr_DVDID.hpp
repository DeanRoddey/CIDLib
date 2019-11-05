//
// FILE NAME: CIDMetaExtr_DVDID.hpp
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
//  This file implements an algorithm used by various DVD based online
//  services and tools. It calculates a unique 64 bit CRC for a DVD, given
//  the path to the root of the DVD. It gives it back both as the binary
//  Card8 value, and in the form 12345678-12345678, i.e. formatted out to
//  hex pairs, with a dash in the middle.
//
//  This is used by the My Movies online web service to uniquely id discs
//  for metadata lookup.
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
//  CLASS: TCIDDVDID
// PREFIX: dvdid
// ---------------------------------------------------------------------------
class CIDMETAEXTREXP TCIDDVDID : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDDVDID();

        TCIDDVDID(const TCIDDVDID&) = delete;
        TCIDDVDID(TCIDDVDID&&) = delete;

        ~TCIDDVDID();


        // -------------------------------------------------------------------
        //  Public oeprators
        // -------------------------------------------------------------------
        TCIDDVDID& operator=(const TCIDDVDID&) = delete;
        TCIDDVDID& operator=( TCIDDVDID&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid GenDVDID
        (
            const   TString&                strDVDPath
            ,       tCIDLib::TCard8&        c8ID
            ,       TString&                strFmtID
        );


    private :
        // -------------------------------------------------------------------
        //  We need a small class to hold the per-file data. Unfortunately we
        //  have to sort the list based on UTF_8 encoded file names, so we
        //  can't just store the find buffers themselves. So we just go ahead
        //  and store just what we need.
        // -------------------------------------------------------------------
        class TPerFileInfo
        {
            public :
                TPerFileInfo
                (
                    const   TFindBuf&       fndbSrc
                    ,       TUTF8Converter& tcvtToUse
                    ,       TMemBuf&        mbufTmp
                );

                TPerFileInfo(const TPerFileInfo&) = delete;
                TPerFileInfo(TPerFileInfo&&) = delete;

                ~TPerFileInfo();

                TPerFileInfo& operator=(const TPerFileInfo&) = delete;
                TPerFileInfo& operator=(TPerFileInfo&&) = delete;

                tCIDLib::TCard4 m_c4NameBytes;
                tCIDLib::TCard4 m_c4Size;
                tCIDLib::TCard8 m_c8Time;
                tCIDLib::TSCh*  m_pschName;
        };


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ProcessBuf
        (
            const   tCIDLib::TCard1* const  pc1Data
            , const tCIDLib::TCard4         c4Size
        );

        tCIDLib::TVoid ProcessCard4
        (
            const   tCIDLib::TCard4         c4Val
        );

        tCIDLib::TVoid ProcessCard8
        (
            const   tCIDLib::TCard8         c8Val
        );

        tCIDLib::TVoid ProcessFile
        (
            const   TFindBuf&               fndbNext
        );

        tCIDLib::TVoid ProcessFiles();

        tCIDLib::TVoid LoadFiles();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c8Accum
        //      The accumulator for our checksum while we do the work.
        //
        //  m_colFiles
        //      We have to go through all of the files in the VIDEO_TS
        //      directory. For each one we store a find buffer which we'll
        //      later go back and use for the calcualtions.
        //
        //  m_mbufTmp
        //      A temporary buffer to use for reading in files and for doing
        //      text transcoding of files names to UTF8. We only read up to
        //      64K of the first two IFO files, so we just pre-size it to
        //      64K.
        //
        //  m_pathVTS
        //      The path to the VIDEO_TS directory.
        //
        //  m_tcvtUTF8
        //      We have to convert the file names to UTF8 before we process
        //      them.
        // -------------------------------------------------------------------
        tCIDLib::TCard8             m_c8Accum;
        TRefVector<TPerFileInfo>    m_colFiles;
        TSysBuf                     m_mbufTmp;
        TPathStr                    m_pathVTS;
        TUTF8Converter              m_tcvtUTF8;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDDVDID,TObject)
};

#pragma CIDLIB_POPPACK


