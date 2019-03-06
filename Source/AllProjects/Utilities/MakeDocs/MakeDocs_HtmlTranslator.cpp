//
// FILE NAME: MakeDocs_HtmlTranslator.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/10/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
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
//  This module implements the Html derivative of the abstract translator
//  class. This guy outputs HTML tags to the provided output text stream.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// -----------------------------------------------------------------------------
//  Include underlying headers.
// -----------------------------------------------------------------------------
#include    "MakeDocs.hpp"


// ----------------------------------------------------------------------------
//  Do our RTTI macros
// ----------------------------------------------------------------------------
RTTIDecls(THtmlTranslator,TTranslator)


// ----------------------------------------------------------------------------
//  CLASS: THtmlTranslator
// PREFIX: trans
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  THtmlTranslator: Public, inherited methods
// ----------------------------------------------------------------------------

//
//  This method outputs an hyperlink in the Html format. The link text is
//  what the user sees. The link target is the target file, just the base
//  name so we add the extension as required.
//
tCIDLib::TVoid
THtmlTranslator::OutputLink(const   TString&    strLinkText
                            , const TString&    strLinkTarget)
{
    // Build the file name
    TPathStr pathTarget(strLinkTarget);
    pathTarget.AppendExt(s_strExtension);

    strmOut() << L"<A HREF=\"" << pathTarget << L"\">"
              << strLinkText
              << L"</A>";
}


//
//  These methods output any needed header and footer information for each
//  page. This includes the standard visual stuff plus any book keeping
//  stuff required by the output format.
//
tCIDLib::TVoid
THtmlTranslator::OutputPageFooter()
{
    strmOut() <<
        L"<P><P>"
        L"<HR WIDTH=85%>"
        L"<FONT SIZE=\"-1\">"
        L"Copyright &#169; 1996..2002, <A HREF=\"http://www.charmedquark.com\">"
        L"Charmed Quark Software</A>"
        L"<BR>"
        L"Mountain View, CA"
        L"<P><center>(<A HREF=\"mailto:CIDLib@charmedquark.com\">"
        L"CIDLib@charmedquark.com</A>)</center>"
        L"</FONT>"
        L"</BODY>"
        L"</HTML>" << kCIDLib::EndLn;
}

tCIDLib::TVoid THtmlTranslator::OutputPageHeader(const TString& strTitle)
{
    strmOut() <<
        L"<HMTL>\n"
        L"<HEAD>\n"
        L"<TITLE> " << strTitle << L"</TITLE>\n"
        L"</HEAD>\n"
        L"<BODY BACKGROUND=\"Background.Jpg\"><br>\n"
        L"<left><img src=\"MainHeader.Jpg\"></left>\n\n";
}


//
//  This method outputs the requested tag in the Html format.
//
tCIDLib::TVoid THtmlTranslator::OutputTag(const ETags eTag)
{
    if (eTag == TTranslator::ETags::Bold)
        strmOut() << L"<B>";
    else if (eTag == TTranslator::ETags::Break)
        strmOut() << L"<BR>";
    else if (eTag == TTranslator::ETags::Divider)
        strmOut() << L"<HR WIDTH=85%>";
    else if (eTag == TTranslator::ETags::EndBold)
        strmOut() << L"</B>";
    else if (eTag == TTranslator::ETags::EndHeading1)
        strmOut() << L"</H2>";
    else if (eTag == TTranslator::ETags::EndHeading2)
        strmOut() << L"</H3>";
    else if (eTag == TTranslator::ETags::EndHeading3)
        strmOut() << L"</H4>";
    else if (eTag == TTranslator::ETags::EndHeading4)
        strmOut() << L"</H5>";
    else if (eTag == TTranslator::ETags::EndItalic)
        strmOut() << L"</I>";
    else if (eTag == TTranslator::ETags::EndList)
        strmOut() << L"</UL>";
    else if (eTag == TTranslator::ETags::EndSource)
        strmOut() << L"</BLOCKQUOTE></B></PRE>";
    else if (eTag == TTranslator::ETags::EndTable)
        strmOut() << L"</TABLE>";
    else if (eTag == TTranslator::ETags::EndTableCol1)
        strmOut() << L"</TD>";
    else if (eTag == TTranslator::ETags::EndTableCol2)
        strmOut() << L"</TD></TR>";
    else if (eTag == TTranslator::ETags::Heading1)
        strmOut() << L"<H2>";
    else if (eTag == TTranslator::ETags::Heading2)
        strmOut() << L"<H3>";
    else if (eTag == TTranslator::ETags::Heading3)
        strmOut() << L"<H4>";
    else if (eTag == TTranslator::ETags::Heading4)
        strmOut() << L"<H5>";
    else if (eTag == TTranslator::ETags::Indent)
        strmOut() << L"<BLOCKQUOTE>";
    else if (eTag == TTranslator::ETags::Italic)
        strmOut() << L"<I>";
    else if (eTag == TTranslator::ETags::List)
        strmOut() << L"<UL>";
    else if (eTag == TTranslator::ETags::ListItem)
        strmOut() << L"<LI>";
    else if (eTag == TTranslator::ETags::Outdent)
        strmOut() << L"</BLOCKQUOTE>";
    else if (eTag == TTranslator::ETags::Paragraph)
        strmOut() << L"<P>";
    else if (eTag == TTranslator::ETags::Source)
        strmOut() << L"<PRE><B><BLOCKQUOTE>";
    else if (eTag == TTranslator::ETags::Table)
        strmOut() << L"<TABLE CELLPADDING=10>";
    else if (eTag == TTranslator::ETags::TableCol1)
        strmOut() << L"<TR><TD VALIGN=\"TOP\">";
    else if (eTag == TTranslator::ETags::TableCol2)
        strmOut() << L"<TD VALIGN=\"TOP\" COLSPAN=5>";
    else
    {
        facMakeDocs.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kDocErrs::errcTransBadTag
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
            , TInteger(tCIDLib::i4EnumOrd(eTag))
        );
    }
}


//
//  This method will output the translated special character.
//
tCIDLib::TVoid
THtmlTranslator::TranslateSpecialChar(const tCIDLib::TCh chToXlat)
{
    if (chToXlat == L'>')
        strmOut() << L"&gt;";
    else if (chToXlat == L'<')
        strmOut() << L"&lt;";
    else if (chToXlat == L'&')
        strmOut() << L"&amp;";
    else
    {
        TString strChar(L" ");
        strChar[0] = chToXlat;
        facMakeDocs.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kDocErrs::errcTransBadSpecialChar
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
            , strChar
            , clsIsA()
        );
    }
}


// ----------------------------------------------------------------------------
//  THtmlTranslator: Private, static data members
// ----------------------------------------------------------------------------
TString  THtmlTranslator::s_strExtension = L"html";

