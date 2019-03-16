//
// FILE NAME: MakeDocs_Translator.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/10/1997
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
//  This header defines the interface for the class that translates our
//  generic formatting tags to the desired target format. This is an
//  abstract base class from which actual translator classes are derived.
//
//  The supported formatting tags are defined by the ETags enum.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Forward references
// ----------------------------------------------------------------------------
class   TDocParser;


// ----------------------------------------------------------------------------
//  CLASS: TTranslator
// PREFIX: trans
// ----------------------------------------------------------------------------
class TTranslator : public TObject
{
    public:
        // --------------------------------------------------------------------
        //  Public types
        // --------------------------------------------------------------------
        enum class ETags
        {
            None
            , Bold
            , Break
            , Divider
            , EndBold
            , EndHeading1
            , EndHeading2
            , EndHeading3
            , EndHeading4
            , EndParagraph
            , EndItalic
            , EndList
            , EndSource
            , EndTable
            , EndTableCol1
            , EndTableCol2
            , Heading1
            , Heading2
            , Heading3
            , Heading4
            , Indent
            , Italic
            , List
            , ListItem
            , Outdent
            , Paragraph
            , Source
            , Table
            , TableCol1
            , TableCol2
        };


        // --------------------------------------------------------------------
        //  Constructors and Destructors.
        // --------------------------------------------------------------------
        TTranslator
        (
            const   TString&                strSpecialChars
        );

        virtual ~TTranslator();


        // --------------------------------------------------------------------
        //  Public, virtual methods
        // --------------------------------------------------------------------
        virtual tCIDLib::TVoid OutputLink
        (
            const   TString&                strLinkText
            , const TString&                strLinkTarget
        ) = 0;

        virtual tCIDLib::TVoid OutputPageFooter() = 0;

        virtual tCIDLib::TVoid OutputPageHeader
        (
            const   TString&                strTitle
        ) = 0;

        virtual tCIDLib::TVoid OutputTag
        (
            const   ETags                   eTag
        ) = 0;

        virtual const TString& strFileExtension() const = 0;

        virtual tCIDLib::TVoid TranslateSpecialChar
        (
            const   tCIDLib::TCh            chToXlat
        ) = 0;


        // --------------------------------------------------------------------
        //  Public, non-virtual methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid SetOutputStream
        (
                    TTextOutStream&         strmOut
        );

        tCIDLib::TVoid TranslateTagText
        (
                    TDocParser&             prsrSource
        );


    protected :
        // --------------------------------------------------------------------
        //  Protected, non-virtual methods
        // --------------------------------------------------------------------
        TTextOutStream& strmOut();


    private :
        // --------------------------------------------------------------------
        //  Private, non-virtual methods
        // --------------------------------------------------------------------
        TTranslator::ETags eMapToTag
        (
            const   TString&                strToken
        );


        // --------------------------------------------------------------------
        //  Private data members
        //
        //  m_pstrmTarget
        //      This is the output stream to output our translated text to.
        //
        //  m_strSpecialChars
        //      This is a list of special chars that the derived class can
        //      provide us. When the TranslateTagText() method sees one, it
        //      will let the derived class output by calling the method
        //      TranslateSpecialChar().
        // --------------------------------------------------------------------
        TTextOutStream* m_pstrmTarget;
        TString         m_strSpecialChars;


        // --------------------------------------------------------------------
        //  Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TTranslator,TObject)
};


// ----------------------------------------------------------------------------
//  Constructors and destructors
// ----------------------------------------------------------------------------
inline TTranslator::~TTranslator()
{
}


// ----------------------------------------------------------------------------
//  Public, non-virtual methods
// ----------------------------------------------------------------------------
inline tCIDLib::TVoid
TTranslator::SetOutputStream(TTextOutStream& strmOut)
{
    m_pstrmTarget = &strmOut;
}


// ----------------------------------------------------------------------------
//  Protected, non-virtual methods
// ----------------------------------------------------------------------------
inline TTextOutStream& TTranslator::strmOut()
{
    return *m_pstrmTarget;
}

