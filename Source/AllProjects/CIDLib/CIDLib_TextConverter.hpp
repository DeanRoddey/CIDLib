//
// FILE NAME: CIDLib_TextConverter.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/17/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This header defines the abstract base class for a family of text format
//  converters.
//
// CAVEATS/GOTCHAS:
//
//  1)  Note that all derived classes must take into account the fact that
//      the native wide character type (tCIDLib::TCh) is not always a 16
//      bit value in memory. On some platforms it can be a 32 bit value.
//      The CIDLIB_WCISUTF16 token is defined if the native wide character is
//      a UTF-16 type.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TMemBuf;


// ---------------------------------------------------------------------------
//   CLASS: TTextConverter
//  PREFIX: tcvt
// ---------------------------------------------------------------------------
class CIDLIBEXP TTextConverter : public TObject, public MDuplicable
{
    public :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        TTextConverter() = delete;

        ~TTextConverter();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::EBaseTextFmts eBaseFmt() const = 0;

        virtual tCIDLib::TVoid Reset() = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4ConvertFrom
        (
            const   tCIDLib::TCard1* const  pc1Src
            , const tCIDLib::TCard4         c4SrcBytes
            ,       tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
            ,       tCIDLib::TCard4&        c4OutChars
        );

        tCIDLib::TCard4 c4ConvertFrom
        (
            const   tCIDLib::TCard1* const  pc1Src
            , const tCIDLib::TCard4         c4SrcBytes
            ,       TString&                strToFill
            , const tCIDLib::EAppendOver    eAppend = tCIDLib::EAppendOver::Overwrite
        );

        tCIDLib::TCard4 c4ConvertFrom
        (
            const   TMemBuf&                mbufSrc
            , const tCIDLib::TCard4         c4SrcBytes
            ,       TString&                strToFill
            , const tCIDLib::EAppendOver    eAppend = tCIDLib::EAppendOver::Overwrite
        );

        tCIDLib::TCard4 c4ConvertOneTo
        (
            const   tCIDLib::TCh            chSrc
            ,       tCIDLib::TCard1* const  pc1ToFill
            , const tCIDLib::TCard4         c4MaxBytes
        );

        tCIDLib::TCard4 c4ConvertTo
        (
            const   tCIDLib::TCh* const     pszSrc
            , const tCIDLib::TCard4         c4SrcChars
            ,       tCIDLib::TCard1* const  pc1ToFill
            , const tCIDLib::TCard4         c4MaxBytes
            ,       tCIDLib::TCard4&        c4OutBytes
        );

        tCIDLib::TCard4 c4ConvertTo
        (
            const   tCIDLib::TCh* const     pszSrc
            , const tCIDLib::TCard4         c4SrcChars
            ,       TExpByteBuf&            expbToFill
        );

        tCIDLib::TCard4 c4ConvertTo
        (
            const   tCIDLib::TCh* const     pszSrc
            , const tCIDLib::TCard4         c4SrcChars
            ,       TMemBuf&                mbufToFill
            ,       tCIDLib::TCard4&        c4OutBytes
        );

        tCIDLib::TCard4 c4ConvertTo
        (
            const   TString&                strSrc
            ,       TMemBuf&                mbufToFill
            ,       tCIDLib::TCard4&        c4OutBytes
        );

        tCIDLib::TCh chConvertOneFrom
        (
            const   tCIDLib::TCard1* const  pc1Src
            , const tCIDLib::TCard4         c4SrcBytes
            ,       tCIDLib::TCard4&        c4OutBytes
        );

        tCIDLib::TCh chRepChar() const;

        tCIDLib::TCh chRepChar
        (
            const   tCIDLib::TCh            chToSet
        );

        tCIDLib::ETCvtActions eErrorAction() const;

        tCIDLib::ETCvtActions eErrorAction
        (
            const   tCIDLib::ETCvtActions   eAction
        );

        const TString& strEncodingName() const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TTextConverter
        (
            const   TString&                strEncodingName
        );

        TTextConverter
        (
            const   TTextConverter&         tcvtSrc
        );

        tCIDLib::TVoid operator=
        (
            const   TTextConverter&         tcvtSrc
        );


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TCard4 c4BlockFrom
        (
            const   tCIDLib::TCard1* const  pc1Src
            , const tCIDLib::TCard4         c4SrcBytes
            ,       tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
            ,       tCIDLib::TCard4&        c4OutChars
            ,       tCIDLib::TBoolean&      bStop
        ) = 0;

        virtual tCIDLib::TCard4 c4BlockTo
        (
            const   tCIDLib::TCh* const     pszSrc
            , const tCIDLib::TCard4         c4SrcChars
            ,       tCIDLib::TCard1* const  pc1ToFill
            , const tCIDLib::TCard4         c4MaxBytes
            ,       tCIDLib::TCard4&        c4OutBytes
            ,       tCIDLib::TBoolean&      bStop
        ) = 0;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        const TString& strEncodingName
        (
            const   TString&                strNew
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_chRepChar
        //      The replacement character to use if we get an invalid char
        //      when internalizing text and they've set the error action to
        //      indicate they want to just use a replacement char.
        //
        //  m_eErrAction
        //      Indicates what action to take if we have trouble converting
        //      incoming data or outgoing data. We either throw immediately
        //      or we just stop with what's done and throw the next time.
        //
        //  m_strEncodingName
        //      The name of the encoding that the derived class handles. Its
        //      constructor will pass on this value to our constructor.
        // -------------------------------------------------------------------
        tCIDLib::TCh            m_chRepChar;
        tCIDLib::ETCvtActions   m_eErrAction;
        TString                 m_strEncodingName;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTextConverter,TObject)
};

#pragma CIDLIB_POPPACK



