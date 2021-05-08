//
// FILE NAME: CIDLib_Base64.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2005
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
//  This is the header for the CIDLib_Base64.cpp file, which provides support
//  for converting between binary and Base64 formats. It's stateless and just
//  converts what you pass it. It uses streams for input and output so it's
//  easy to do an ongoing sequence of operations that accumulate to a single
//  output.
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
//   CLASS: TBase64
//  PREFIX: b64
// ---------------------------------------------------------------------------
class CIDLIBEXP TBase64 : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBase64() = default;

        TBase64(const TBase64&) = default;
        TBase64(TBase64&&) = default;

        ~TBase64() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBase64& operator=(const TBase64&) = default;
        TBase64& operator=(TBase64&&) = default;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bEscapeForwardSlashes() const noexcept
        {
            return m_bEscapeForwardSlashes;
        }

        tCIDLib::TBoolean bEscapeForwardSlashes(const tCIDLib::TBoolean bToSet) noexcept
        {
            m_bEscapeForwardSlashes = bToSet;
            return m_bEscapeForwardSlashes;
        }

        tCIDLib::TCard4 c4Decode
        (
                    TBinInStream&           strmIn
            ,       TBinOutStream&          strmOut
        );

        tCIDLib::TCard4 c4Encode
        (
                    TBinInStream&           strmIn
            ,       TBinOutStream&          strmOut
        );

        tCIDLib::TCard4 c4LineWidth() const noexcept
        {
            return m_c4LineWidth;
        }

        tCIDLib::TCard4 c4LineWidth
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TVoid Decode
        (
                    TTextInStream&          strmIn
            ,       TBinOutStream&          strmOut
        );

        tCIDLib::TVoid Encode
        (
                    TBinInStream&           strmIn
            ,       TTextOutStream&         strmOut
        );

        tCIDLib::TVoid Encode
        (
                    TTextInStream&          strmIn
            ,       TTextOutStream&         strmOut
        );

        tCIDLib::TVoid Encode
        (
            const   TString&                strSrc
            ,       TBinOutStream&          strmOut
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4EncodeBlock
        (
            const   tCIDLib::TCard1* const  pc1In
            ,       tCIDLib::TCard1* const  pc1Out
            , const tCIDLib::TCard4         c4Len
        );

        tCIDLib::TVoid DecodeBlock
        (
            const   tCIDLib::TCard1* const  pc1In
            ,       tCIDLib::TCard1* const  pc1Out
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bEscapeForwardSlashes
        //      In some cases, such as transmitting over JSON, we need to escape
        //      forward slashes or recipients may choke on it. Default is not to.
        //
        //  m_c4LineWidth
        //      The maximum line width to generate when encoding. Defaults to
        //      the standard 72.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bEscapeForwardSlashes = kCIDLib::False;
        tCIDLib::TCard4     m_c4LineWidth = 72;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TBase64,TObject)
};

#pragma CIDLIB_POPPACK


