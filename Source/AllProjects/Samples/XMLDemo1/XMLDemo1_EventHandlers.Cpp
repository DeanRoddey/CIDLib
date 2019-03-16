//
// FILE NAME: XMLDemo1_EventHandlers.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/21/99
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file provides the implementations of the event callback methods.
//  These are all actually methods of the facility class, but we don't want
//  to clutter its Cpp file up, so we put them here.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Include our own main header, which brings in all we need
// ---------------------------------------------------------------------------
#include    "XMLDemo1.Hpp"



// ---------------------------------------------------------------------------
//   CLASS: TFacXMLDemo1
//  PREFIX: xprs
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacXMLDemo1: Protected, inherited methods (from MXMLDocEvents)
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TFacXMLDemo1::DocCharacters(const   TString&            strChars
                            , const tCIDLib::TBoolean   bIsCDATA
                            , const tCIDLib::TBoolean   bIsIgnorable
                            , const tCIDXML::ELocations eLocation)
{
    // If no display mode, then nothing to do
    if (m_eDisplayMode == EDisplayModes::None)
        return;

    //
    //  Call a private to do processing. It has to handle embedded XML type
    //  newlines, which wouldn't cause the right results if just dumped
    //  to the out put.
    //
    if (bIsCDATA && (m_eDisplayMode != EDisplayModes::Canonical))
        *m_pstrmOut << L"<![CDATA[";

    ShowChars(strChars);

    if (bIsCDATA && (m_eDisplayMode != EDisplayModes::Canonical))
        *m_pstrmOut << L"]]>";
}


tCIDLib::TVoid
TFacXMLDemo1::DocComment(const  TString&        strCommentText
                        , const tCIDXML::ELocations)
{
    // If no display mode, then nothing to do
    if (m_eDisplayMode == EDisplayModes::None)
        return;

    if (m_eDisplayMode != EDisplayModes::Canonical)
    {
        *m_pstrmOut << L"<!--";
        ShowChars(strCommentText);
        *m_pstrmOut << L"-->";
    }
}


tCIDLib::TVoid TFacXMLDemo1::DocPI( const   TString&    strTarget
                                    , const TString&    strValue
                                    , const tCIDXML::ELocations)
{
    // If no display mode, then nothing to do
    if (m_eDisplayMode == EDisplayModes::None)
        return;

    *m_pstrmOut << L"<?" << strTarget << L" ";
    if (strValue.c4Length())
        ShowChars(strValue, kCIDLib::False);
    *m_pstrmOut << L"?>";
}


tCIDLib::TVoid TFacXMLDemo1::EndDocument(const TXMLEntitySrc& xsrcOfRoot)
{
    // Flush the output stream
    *m_pstrmOut << kCIDLib::FlushIt;
}


tCIDLib::TVoid TFacXMLDemo1::EndTag(const TXMLElemDecl& xdeclElem)
{

    if (m_c4ElemDepth)
        m_c4ElemDepth--;

    // If no display mode, then nothing to do
    if (m_eDisplayMode == EDisplayModes::None)
        return;

    *m_pstrmOut << L"</" << xdeclElem.strFullName() << L">";
}


tCIDLib::TVoid TFacXMLDemo1::ResetDocument()
{
}


tCIDLib::TVoid TFacXMLDemo1::StartDocument(const TXMLEntitySrc& xsrcOfRoot)
{
}


tCIDLib::TVoid
TFacXMLDemo1::StartTag(         TXMLParserCore&     xprsSrc
                        , const TXMLElemDecl&       xdeclElem
                        , const tCIDLib::TBoolean   bEmpty
                        , const TVector<TXMLAttr>&  colAttrList
                        , const tCIDLib::TCard4     c4AttrListSize)
{
    // Bump up the element depth
    m_c4ElemDepth++;

    // If no display mode, then nothing to do
    if (m_eDisplayMode == EDisplayModes::None)
        return;

    // Output the opening part with the bracket and name
    *m_pstrmOut << L"<" << xdeclElem.strFullName();

    //
    //  If any attributes, then output them. If we are doing canonical output
    //  we have to sort them.
    //
    if (c4AttrListSize)
    {
        if (m_eDisplayMode == EDisplayModes::Canonical)
        {
            //
            //  We want to display them sorted. So lets create a ref vector,
            //  which does not adopt. We will then put references to each
            //  element into the vector, sort it, and use it for display. We
            //  can then just toss the vector.
            //
            TRefVector<const TXMLAttr> colSorted
            (
                tCIDLib::EAdoptOpts::NoAdopt, c4AttrListSize
            );

            // Sort them into the vector
            tCIDLib::TCard4 c4Index;
            for (c4Index = 0; c4Index < c4AttrListSize; c4Index++)
                colSorted.Add(&colAttrList[c4Index]);

            //
            //  And now sort and display display them. This class has a
            //  default comparator method that works fine for us.
            //
            colSorted.Sort(TXMLAttr::eComp);
            for (c4Index = 0; c4Index < c4AttrListSize; c4Index++)
            {
                const TXMLAttr& xattrCur = *colSorted[c4Index];
                *m_pstrmOut << L" " << xattrCur.strQName() << L"=\"";
                ShowChars(xattrCur.strValue());
                *m_pstrmOut << L"\"";
            }
        }
         else
        {
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4AttrListSize; c4Index++)
            {
                const TXMLAttr& xattrCur = colAttrList[c4Index];
                *m_pstrmOut << L" " << xattrCur.strQName() << L"=\"";
                ShowChars(xattrCur.strValue());
                *m_pstrmOut << L"\"";
            }
        }
    }

    // Cap it off differently if empty or non-empty
    if (bEmpty)
    {
        //
        //  If canonical format, we can't do empty elements, we have to do a
        //  real end tag.
        //
        if (m_eDisplayMode == EDisplayModes::Canonical)
            *m_pstrmOut << L"></" << xdeclElem.strFullName() << L">";
        else
            *m_pstrmOut << L"/>";
    }
     else
    {
        *m_pstrmOut << L">";
    }
}


tCIDLib::TVoid
TFacXMLDemo1::XMLDecl(  const   TString&    strVersion
                        , const TString&    strEncoding
                        , const TString&    strStandalone)
{
    // Only do this on the primary level
    if (m_c4EntityNesting > 1)
        return;

    // If no display mode, then nothing to do
    if (m_eDisplayMode == EDisplayModes::None)
        return;

    *m_pstrmOut  << L"<?xml";

    if (strVersion.c4Length())
        *m_pstrmOut << L" version=\"" << strVersion << L"\"";

    if (strEncoding.c4Length())
        *m_pstrmOut << L" encoding=\"" << strEncoding << L"\"";

    if (strStandalone.c4Length())
        *m_pstrmOut << L" standalone=\"" << strStandalone << L"\"";

    *m_pstrmOut << L" ?>";
}



// ---------------------------------------------------------------------------
//  TFacXMLDemo1: Protected, inherited methods (from MXMLErrorEvents)
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TFacXMLDemo1::HandleXMLError(const  tCIDLib::TErrCode   errcToPost
                            , const tCIDXML::EErrTypes  eType
                            , const TString&            strText
                            , const tCIDLib::TCard4     c4CurColumn
                            , const tCIDLib::TCard4     c4CurLine
                            , const TString&            strSystemId)
{
    // Only show warnings if we are explicitly told to do so
    if ((eType == tCIDXML::EErrTypes::Warning) && !m_bShowWarnings)
        return;

    //
    //  If we are doing the error location type output, then we put out
    //  a format that is compact and used for regression testing. Else we
    //  put out the more human readable version.
    //
    if (m_eDisplayMode == EDisplayModes::ErrLoc)
    {
        //
        //  This mode looks like this:
        //
        //  errcode, lastextsysid, idoferr::line/col
        //
        //  We strip off thep ath of the leading exteral system id, since its
        //  just to give a hint and it takes up to much space otherwise.
        //
//        TPathStr pathExtId(m_xprsThis.xemThis().strLastExtId());
//        pathExtId.bRemovePath();
//        *m_pstrmOut << errcToPost << L", " << pathExtId << L", "
//                    << strSystemId << L"::" << c4CurLine
//                    << L"/" << c4CurColumn
//                    << kCIDLib::EndLn;
    }
     else if (m_eDisplayMode == EDisplayModes::ErrLoc2)
    {
        //
        //  This is a simpler error report format that is useful in those
        //  scenarios where you want to compile within an editor and it wants
        //  a standard format for the editor to parse for next/previous error
        //  support.
        //
        *m_pstrmErr << strSystemId << L" (" << c4CurLine << L","
                    << c4CurColumn << L") : ";

        if (eType == tCIDXML::EErrTypes::Warning)
        {
            *m_pstrmErr  << L"warning - ";
        }
        else if ((eType == tCIDXML::EErrTypes::Error)
             ||  (eType == tCIDXML::EErrTypes::Validation))
        {
            *m_pstrmErr  << L"error - ";
        }

        *m_pstrmErr << strText << kCIDLib::EndLn;
    }
     else
    {
        if (eType == tCIDXML::EErrTypes::Warning)
            *m_pstrmOut  << L"\nWARNING ";
        else if (eType == tCIDXML::EErrTypes::Error)
            *m_pstrmOut  << L"\nERROR ";
        else if (eType == tCIDXML::EErrTypes::Validation)
            *m_pstrmOut  << L"\nINVALID ";

        *m_pstrmOut << L"(\""
                    << strSystemId << L"\" Line/Col:"
                    << c4CurLine << L"/" << c4CurColumn
                    << L")\n    "
                    << strText
                    << kCIDLib::EndLn;
    }
}


tCIDLib::TVoid TFacXMLDemo1::ResetErrors()
{
}



// ---------------------------------------------------------------------------
//  TFacXMLDemo1: Protected, inherited methods (from MXMLDocTypeEvents)
// ---------------------------------------------------------------------------
tCIDLib::TVoid TFacXMLDemo1::AttrDef(const  TDTDAttrDef&        xadAttr
                                    , const tCIDLib::TBoolean   bIgnored)
{
    // If no display mode, then nothing to do
    if (m_eDisplayMode == EDisplayModes::None)
        return;

    // Just use the default formatting
    xadAttr.FormatTo(*m_pstrmOut, *m_pxvalTest);
}


tCIDLib::TVoid
TFacXMLDemo1::DocType(  const   TDTDElemDecl&   xdeclRoot
                        , const TString&        strPublicId
                        , const TString&        strSystemId)
{
    // If no display mode, then nothing to do
    if (m_eDisplayMode == EDisplayModes::None)
        return;

    *m_pstrmOut << L"<!DOCTYPE " << xdeclRoot.strFullName();

    if (!strSystemId.bIsEmpty())
    {
        if (!strPublicId.bIsEmpty())
        {
            *m_pstrmOut << L" PUBLIC \""
                        << strPublicId << L"\" \""
                        << strSystemId << L"\"";
        }
         else
        {
            *m_pstrmOut << L" SYSTEM \"" << strSystemId << L"\"";
        }
    }
}


tCIDLib::TVoid TFacXMLDemo1::DocTypeComment(const TString& strCommentText)
{
    // If no display mode, then nothing to do
    if (m_eDisplayMode == EDisplayModes::None)
        return;

    *m_pstrmOut << L"<!--";
    ShowChars(strCommentText);
    *m_pstrmOut << L"-->";
}


tCIDLib::TVoid TFacXMLDemo1::DocTypePI( const   TString&    strTarget
                                        , const TString&    strValue
                                        , const tCIDXML::ELocations)
{
    // If no display mode, then nothing to do
    if (m_eDisplayMode == EDisplayModes::None)
        return;

    *m_pstrmOut << L"<?" << strTarget << L" ";
    if (strValue.c4Length())
        ShowChars(strValue, kCIDLib::False);
    *m_pstrmOut << L"?>";
}


tCIDLib::TVoid TFacXMLDemo1::DocTypeWS(const TString& strChars)
{
    // If no display mode, then nothing to do
    if (m_eDisplayMode == EDisplayModes::None)
        return;

    //
    //  Call a private to do processing. It has to handle embedded XML type
    //  newlines, which wouldn't cause the right results if just dumped
    //  to the output.
    //
    ShowChars(strChars);
}


tCIDLib::TVoid
TFacXMLDemo1::ElementDecl(  const   TDTDElemDecl&       xdeclElement
                            , const tCIDLib::TBoolean   bIgnored)
{
    // If no display mode, then nothing to do
    if (m_eDisplayMode == EDisplayModes::None)
        return;

    // Just use the default formatting
    xdeclElement.FormatTo(*m_pstrmOut, *m_pxvalTest);
}


tCIDLib::TVoid
TFacXMLDemo1::EndAttList(const TDTDElemDecl& xdeclParentElement)
{
    // If no display mode, then nothing to do
    if (m_eDisplayMode == EDisplayModes::None)
        return;

    *m_pstrmOut << L">";
}


tCIDLib::TVoid TFacXMLDemo1::EndDocType()
{
    // If no display mode, then nothing to do
    if (m_eDisplayMode == EDisplayModes::None)
        return;

    *m_pstrmOut << L">";
}


tCIDLib::TVoid TFacXMLDemo1::EndExtSubset()
{
}

tCIDLib::TVoid TFacXMLDemo1::EndIntSubset()
{
    // If no display mode, then nothing to do
    if (m_eDisplayMode == EDisplayModes::None)
        return;

    *m_pstrmOut << L"]";
}


tCIDLib::TVoid
TFacXMLDemo1::EntityDecl(const  TDTDEntityDecl&     xdeclEntity
                        , const tCIDLib::TBoolean   bIgnored)
{
    // If no display mode, then nothing to do
    if (m_eDisplayMode == EDisplayModes::None)
        return;

    // Just do the standard formatting
    xdeclEntity.FormatTo(*m_pstrmOut, *m_pxvalTest);
}


tCIDLib::TVoid
TFacXMLDemo1::NotationDecl( const   TDTDNotationDecl&   xdeclNotation
                            , const tCIDLib::TBoolean   bIgnored)
{
    // If no display mode, then nothing to do
    if (m_eDisplayMode == EDisplayModes::None)
        return;

    // Just use the default formatting
    xdeclNotation.FormatTo(*m_pstrmOut, *m_pxvalTest);
}


tCIDLib::TVoid TFacXMLDemo1::ResetDocType()
{
}


tCIDLib::TVoid
TFacXMLDemo1::StartAttList(const TDTDElemDecl& xdeclParentElement)
{
    // If no display mode, then nothing to do
    if (m_eDisplayMode == EDisplayModes::None)
        return;

    *m_pstrmOut << L"<!ATTLIST " << xdeclParentElement.strFullName();
}


tCIDLib::TVoid TFacXMLDemo1::StartExtSubset()
{
}


tCIDLib::TVoid TFacXMLDemo1::StartIntSubset()
{
    // If no display mode, then nothing to do
    if (m_eDisplayMode == EDisplayModes::None)
        return;

    *m_pstrmOut << L"\n[";
}


tCIDLib::TVoid
TFacXMLDemo1::TextDecl( const   TString&    strVersion
                        , const TString&    strEncoding)
{
    // If no display mode, then nothing to do
    if (m_eDisplayMode == EDisplayModes::None)
        return;

    *m_pstrmOut  << L"\n<?xml";

    if (strVersion.c4Length())
        *m_pstrmOut << L" version=\"" << strVersion << L"\"";

    if (strEncoding.c4Length())
        *m_pstrmOut << L" encoding=\"" << strEncoding << L"\"";

    *m_pstrmOut << L" ?>";
}



// ---------------------------------------------------------------------------
//  TFacXMLDemo1: Protected, inherited methods (from MXMLEntityEvents)
// ---------------------------------------------------------------------------
tCIDLib::TVoid TFacXMLDemo1::EndEntity(const TXMLEntityDecl& xdeclEnding)
{
    // Bump up the entity nesting level
    m_c4EntityNesting++;
}

tCIDLib::TVoid TFacXMLDemo1::ResetEntities()
{
}

tCIDLib::TVoid TFacXMLDemo1::StartEntity(const TXMLEntityDecl& xdeclStarting)
{
    // Bump down the entity nesting level
    if (m_c4EntityNesting)
        m_c4EntityNesting--;
}


// ---------------------------------------------------------------------------
//  TXMLTreeParser: Protected, virtual methods (entity resolver)
// ---------------------------------------------------------------------------
tCIDXML::TEntitySrcRef
TFacXMLDemo1::esrResolve(const  TString&                strPublicId
                        , const TString&                strSystemId
                        , const TString&
                        , const TString&
                        , const tCIDXML::EResolveTypes  eResType)
{
    // If we don't have a catalog, then obviously not
    if (!m_xcatMappings.c4MapCount())
        return tCIDXML::TEntitySrcRef(0);

    //
    //  Lets try to look up the public id. If not found, it will return a
    //  zero, which is what we want to return. If it does, it will return a
    //  pointer to the entity source, which is also what we want to return.
    //
    return m_xcatMappings.esrMapId(strPublicId, strSystemId);
}


tCIDLib::TVoid TFacXMLDemo1::ResetResolver()
{
    // We don't need to do anything in this case
}



// ---------------------------------------------------------------------------
//  Local methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TFacXMLDemo1::ShowChars(const   TString&            strChars
                        , const tCIDLib::TBoolean   bDoEntReplacement)
{
    if (m_eDisplayMode == EDisplayModes::Canonical)
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < strChars.c4Length(); c4Index++)
        {
            if (bDoEntReplacement)
            {
                if (strChars[c4Index] == kCIDLib::chLF)
                    *m_pstrmOut << L"&#10;";
                else if (strChars[c4Index] == kCIDLib::chCR)
                    *m_pstrmOut << L"&#13;";
                else if (strChars[c4Index] == kCIDLib::chTab)
                    *m_pstrmOut << L"&#9;";
                else if (strChars[c4Index] == kCIDLib::chApostrophe)
                    *m_pstrmOut << L"&apos;";
                else if (strChars[c4Index] == kCIDLib::chAmpersand)
                    *m_pstrmOut << L"&amp;";
                else if (strChars[c4Index] == kCIDLib::chQuotation)
                    *m_pstrmOut << L"&quot;";
                else if (strChars[c4Index] == kCIDLib::chLessThan)
                    *m_pstrmOut << L"&lt;";
                else if (strChars[c4Index] == kCIDLib::chGreaterThan)
                    *m_pstrmOut << L"&gt;";
                else
                    *m_pstrmOut << strChars[c4Index];
            }
             else
            {
                *m_pstrmOut << strChars[c4Index];
            }
        }
    }
     else
    {
        //
        //  We are not doing canonical format, so all we have to do is to
        //  translate XML newline chars into output new lines.
        //
        for (tCIDLib::TCard4 c4Index = 0; c4Index < strChars.c4Length(); c4Index++)
        {
            if (strChars[c4Index] == kCIDLib::chLF)
                *m_pstrmOut << kCIDLib::NewLn;
            else if (strChars[c4Index] == kCIDLib::chCR)
                *m_pstrmOut << kCIDLib::NewLn;
            else
                *m_pstrmOut << strChars[c4Index];
        }
    }
}


