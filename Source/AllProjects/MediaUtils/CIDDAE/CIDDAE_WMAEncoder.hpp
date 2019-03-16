//
// FILE NAME: CIDDAE_WMAEncoder.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/21/2006
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
//  This file implements the TCIDDAEWMAEnc class. This class implements the
//  base encoder interface and supports ripping to the WMA format.
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
//  CLASS: TCIDDAEWMADec
// PREFIX: daed
// ---------------------------------------------------------------------------
class CIDDAEEXP TCIDDAEWMADec : public TCIDDAEDecoder
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strCodecName();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDDAEWMADec();

        TCIDDAEWMADec(const TCIDDAEWMADec&) = delete;

        ~TCIDDAEWMADec();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDDAEWMADec& operator=(const TCIDDAEWMADec&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4LoadChunkImpl
        (
                    TMemBuf&                mbufToFill
            , const tCIDLib::TCard4         c4StartInd = 0
        )   override;

        tCIDLib::TVoid CleanupImpl() override;

        tCIDLib::TVoid EndDecodeImpl() override;

        tCIDLib::TVoid StartDecodeImpl
        (
            const   TString&                strSrcFile
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pc1Data
        //      We don't expose all the WMF SDK mumbo jumbo, so we just
        //      provide an opaque pointer that this guy allocates as required
        //      internally.
        // -------------------------------------------------------------------
        tCIDLib::TCard1*    m_pc1Data;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDDAEWMADec,TCIDDAEDecoder)
};




// ---------------------------------------------------------------------------
//  CLASS: TCIDDAEWMAEnc
// PREFIX: daee
// ---------------------------------------------------------------------------
class CIDDAEEXP TCIDDAEWMAEnc : public TCIDDAEEncoder
{
    public  :
        // -------------------------------------------------------------------
        //   CLASS: TCodecInfo
        //  PREFIX: cdeci
        //
        //  A simple class used to return codec and format info. We have to
        //  return a name and an index for each one, plus some basic format
        //  info required to access the format info later. The VBR/passes
        //  ones are only used for formats, not for codecs. And the passes
        //  only matters for VBR formats.
        // -------------------------------------------------------------------
        class TCodecInfo
        {
            public :
                TCodecInfo() :
                    m_bIsVBR(kCIDLib::False)
                    , m_c4Index(0)
                    , m_c4Passes(0)
                {
                }

                TCodecInfo( const   TString&            strName
                            , const tCIDLib::TCard4     c4Index
                            , const tCIDLib::TBoolean   bIsVBR
                            , const tCIDLib::TCard4     c4Passes) :

                    m_bIsVBR(bIsVBR)
                    , m_c4Index(c4Index)
                    , m_c4Passes(c4Passes)
                    , m_strName(strName)
                {
                }

                TCodecInfo( const   TString&            strName
                            , const tCIDLib::TCard4     c4Index) :

                    m_bIsVBR(kCIDLib::False)
                    , m_c4Index(c4Index)
                    , m_c4Passes(0)
                    , m_strName(strName)
                {
                }

                ~TCodecInfo() {}

                tCIDLib::TBoolean   m_bIsVBR;
                tCIDLib::TCard4     m_c4Index;
                tCIDLib::TCard4     m_c4Passes;
                TString             m_strName;
        };


        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bFindCodecInfo
        (
            const   TString&                strCodecDescr
            , const TString&                strFmtDescr
            ,       TCodecInfo&             cdeciCodec
            ,       TCodecInfo&             cdeciFmt
        );

        static tCIDLib::TCard4 c4EnumAudioCodecs
        (
                    TCollection<TCodecInfo>& colToFill
        );

        static tCIDLib::TCard4 c4EnumCodecFmts
        (
            const   TCodecInfo&              cdeciSrc
            ,       TCollection<TCodecInfo>& colToFill
        );

        static const TString& strCodecName();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDDAEWMAEnc();

        TCIDDAEWMAEnc(const TCIDDAEWMAEnc&) = delete;

        ~TCIDDAEWMAEnc();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDDAEWMAEnc& operator=(const TCIDDAEWMAEnc&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCodecInfoValid
        (
            const   TString&                strCodec
            , const TString&                strFormat
        )   const;

        tCIDLib::TCard4 c4QueryCodecNames
        (
                    tCIDLib::TStrList&      colToFill
        );

        tCIDLib::TCard4 c4QueryCodecFmtNames
        (
            const   TString&                strCodec
            ,       tCIDLib::TStrList&      colToFill
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CleanupImpl() override;

        tCIDLib::TVoid EndEncodeImpl() override;

        tCIDLib::TVoid StartEncodeImpl
        (
            const   TString&                strTargetFile
            , const TString&                strCodecName
            , const TString&                strFmtName
            , const tCIDLib::TCard4         c4TrackNum
            , const TString&                strAlbumTitle
            , const TString&                strTrackTitle
            , const TString&                strArtist
            , const TString&                strLabel
            , const TString&                strGenre
            , const tCIDLib::TCard4         c4Year
        )   override;

        tCIDLib::TVoid StoreChunkImpl
        (
            const   tCIDLib::TCard1* const  pc1Data
            , const tCIDLib::TCard4         c4Bytes
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddCardTag
        (
                    IWMHeaderInfo3* const   pHdrInfo
            , const tCIDLib::TCh* const     pszAttr
            , const tCIDLib::TCard4         c4Value
        );

        tCIDLib::TVoid AddCard8Tag
        (
                    IWMHeaderInfo3* const   pHdrInfo
            , const tCIDLib::TCh* const     pszAttr
            , const tCIDLib::TCard8         c8Value
        );

        tCIDLib::TVoid AddStringTag
        (
                    IWMHeaderInfo3* const   pHdrInfo
            , const tCIDLib::TCh* const     pszAttr
            , const TString&                strValue
        );

        tCIDLib::TVoid ThrowEncError
        (
            const   tCIDLib::TErrCode       errcToThrow
            , const tCIDLib::TCard4         c4HRes
            , const tCIDLib::TCard4         c4Line
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pc1Data
        //      We don't expose all the WMF SDK mumbo jumbo, so we just
        //      provide an opaque pointer that this guy allocates as required
        //      internally.
        // -------------------------------------------------------------------
        tCIDLib::TCard1*    m_pc1Data;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDDAEWMAEnc,TCIDDAEEncoder)
};

#pragma CIDLIB_POPPACK


