//
// FILE NAME: MakeDocs_HtmlTranslator.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/10/1997
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
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
//  This is the header for the MakeDocs_HtmlTranslator.cpp file. This
//  module implements a derivative of the abstract TTranslator class. This
//  derivative handles translations for the HTML format.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// ----------------------------------------------------------------------------
//  CLASS: THtmlTranslator
// PREFIX: trans
// ----------------------------------------------------------------------------
class THtmlTranslator : public TTranslator
{
    public :
        // --------------------------------------------------------------------
        //  Constructors and Destructors.
        // --------------------------------------------------------------------
        THtmlTranslator();

        ~THtmlTranslator();


        // --------------------------------------------------------------------
        //  Public, inherited methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid OutputLink
        (
            const   TString&                strLinkText
            , const TString&                strLinkTarget
        );

        tCIDLib::TVoid OutputPageFooter();

        tCIDLib::TVoid OutputPageHeader
        (
            const   TString&                strTitle
        );

        tCIDLib::TVoid OutputTag
        (
            const   ETags                   eTag
        );

        const TString& strFileExtension() const;

        tCIDLib::TVoid TranslateSpecialChar
        (
            const   tCIDLib::TCh            chToXlat
        );


    private :
        // --------------------------------------------------------------------
        //  Private, static data members
        //
        //  s_strExtension
        //      This is the text for the extension we use "html". Its returned
        //      from strFileExtension().
        // --------------------------------------------------------------------
        static TString  s_strExtension;


        // --------------------------------------------------------------------
        //  Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(THtmlTranslator,TTranslator)
};


// ----------------------------------------------------------------------------
//  Constructors and destructors
// ----------------------------------------------------------------------------
inline THtmlTranslator::THtmlTranslator() :

    TTranslator(L"<>&")
{
}

inline THtmlTranslator::~THtmlTranslator()
{
}

// ----------------------------------------------------------------------------
//  Public, inherited methods
// ----------------------------------------------------------------------------
inline const TString& THtmlTranslator::strFileExtension() const
{
    return s_strExtension;
}

