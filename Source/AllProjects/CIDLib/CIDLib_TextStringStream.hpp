//
// FILE NAME: CIDLib_TextStringStream.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/28/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDLib_StringStream.Cpp file, which
//  implements the TTextStringInStream and TTextStringOutStream classes. These
//  are convenience classes that make it quick to set up a text stream over
//  a TString object sink/source. This is often done to format text directly
//  to a string.
//
//  It basically just does a couple of steps that you could do yourself, but
//  which makes it safe and convenient to have done. Since string streams are
//  very common, this provides a good payback.
//
// CAVEATS/GOTCHAS:
//
//  1)  These classes do now allow you to provide a text converter. Instead,
//      they install a special, internal only, converter that just passes
//      the text through in native 'in memory' wide character format. This
//      insures that the data streamed out is valid string data.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class TStreamFmt;
class TTextStringOutStream;


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//   CLASS: TTextStringInStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDLIBEXP TTextStringInStream : public TTextInStream
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTextStringInStream() = delete;

        TTextStringInStream
        (
            const   TString* const          pstrData
            , const tCIDLib::EAdoptOpts     eAdopt = tCIDLib::EAdoptOpts::NoAdopt
        );

        explicit TTextStringInStream
        (
            const   TTextStringOutStream&   strmToSyncWith
        );

        TTextStringInStream(const TTextStringInStream&) = delete;
        ~TTextStringInStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        operator const TString&() const;

        TTextStringInStream& operator=(const TTextStringInStream&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TString& strData() const;


    private     :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pstrmiIn
        //      A pointer to the implementation object that we put into the
        //      underlying binary stream that we create. This gives us access
        //      to the string text and current position info in a more
        //      convenient TCard4 format (for those folks who know that they
        //      specifically dealing with this class.)
        // -------------------------------------------------------------------
        TStringInStreamImpl*    m_pstrmiIn;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTextStringInStream,TTextInStream)
};



// ---------------------------------------------------------------------------
//   CLASS: TTextStringOutStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDLIBEXP TTextStringOutStream : public TTextOutStream
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTextStringOutStream() = delete;

        TTextStringOutStream
        (
                    TString* const          pstrData
            , const TStreamFmt&             strmfToUse
            , const tCIDLib::EAdoptOpts     eAdopt = tCIDLib::EAdoptOpts::NoAdopt
            , const tCIDLib::TBoolean       bAppend = kCIDLib::False
        );

        TTextStringOutStream
        (
                    TString* const          pstrData
            , const tCIDLib::EAdoptOpts     eAdopt = tCIDLib::EAdoptOpts::NoAdopt
            , const tCIDLib::TBoolean       bAppend = kCIDLib::False
        );

        explicit TTextStringOutStream
        (
            const   tCIDLib::TCard4         c4InitChars
        );

        TTextStringOutStream
        (
            const   tCIDLib::TCard4         c4InitChars
            , const TStreamFmt&             strmfToUse
        );

        TTextStringOutStream(const TTextStringOutStream&) = delete;

        ~TTextStringOutStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        operator const TString&() const;

        TTextStringOutStream& operator=(const TTextStringOutStream&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TString& strData() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TTextStringInStream;


    private     :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pstrmiOut
        //      In order to support synchronizing input streams to output
        //      streams, we keep a pointer to our impl object. The binary
        //      stream that underlies this stream owns the impl object but
        //      we cannot get back to it once we give it away (and it would
        //      have to be down cast anyway.)
        // -------------------------------------------------------------------
        TStringOutStreamImpl*   m_pstrmiOut;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTextStringOutStream,TTextOutStream)
};


#pragma CIDLIB_POPPACK



