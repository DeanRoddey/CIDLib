//
// FILE NAME: CIDDocComp_ParseCtx.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/02/2019
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  Implements the parser context class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "CIDDocComp.hpp"


// -----------------------------------------------------------------------------
//   CLASS: TParseCtx
//  PREFIX: ctx
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//  TParseCtx: Constructors
// -----------------------------------------------------------------------------
TParseCtx::TParseCtx() :

     m_bVerbose(TSysInfo::bFindCmdLineParm(L"/verbose"))
{
}


// -----------------------------------------------------------------------------
//  TParseCtx: Public, non-virtual methods
// -----------------------------------------------------------------------------

// Called by parsing code to add an error to our error list
tCIDLib::TVoid
TParseCtx::AddErrorMsg( const   TString&        strMsg
                        , const MFormattable&   mfmtblToken1
                        , const MFormattable&   mfmtblToken2)
{
    TString strErr = strMsg;
    if (!MFormattable::bIsNullObject(mfmtblToken1))
        strErr.eReplaceToken(mfmtblToken1, kCIDLib::chDigit1);
    if (!MFormattable::bIsNullObject(mfmtblToken2))
        strErr.eReplaceToken(mfmtblToken1, kCIDLib::chDigit2);

    m_colErrs.objAdd
    (
        TKeyValuePair(m_colSrcStack.objPeek(), strErr)
    );
}



tCIDLib::TBoolean TParseCtx::bParseXML(const TString& strFile)
{
    const tCIDLib::TBoolean bRes = m_xtprsToUse.bParseRootEntity
    (
        strFile
        , tCIDXML::EParseOpts::Validate
        , tCIDXML::EParseFlags::TagsNText
    );
    if (!bRes)
    {
        TTextStringOutStream strmFmt(1024UL);
        const TXMLTreeParser::TErrInfo& errCur = m_xtprsToUse.erriFirst();
        strmFmt << L"XML Error: "
                << errCur.strText() << kCIDLib::NewLn
                << L"    - " << errCur.c4Line()
                << kCIDLib::chPeriod << errCur.c4Column()
                << L" [" << errCur.strSystemId() << L"]";

        strmFmt.Flush();
        AddErrorMsg(strmFmt.strData());
    }
    return bRes;
}


//
//  Called to load our DTD text and add it to our parser as a preloaded entity,
//  so that the individual XML files don't have to deal with pointing themselves
//  at an actual DTD file (with all of the nesting going on it's a pain.) And it
//  will speed things up as well.
//
//  But it keeps the DTD external so we can quickly make changes and test them.
//
tCIDLib::TVoid TParseCtx::LoadDTD(const TString& strSrcPath)
{
    // Build up the full path name
    TPathStr pathDTD(strSrcPath);
    pathDTD.AddLevel(L"CIDDocs.DTD");

    TTextFileInStream strmDTD
    (
        pathDTD
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Excl_Read
        , facCIDEncode().ptcvtMakeNew(L"Latin1")
    );

    //
    //  Read in the lines and add them to a string. The file doesn't have the
    //  entity header line, so we add that, indicating our internal string
    //  format as the encoding.
    //
    TString strDTDText(32 * 1024UL);
    strDTDText = L"<?xml encoding='$NativeWideChar$'?>\n";

    TString strLine(128UL);
    while (!strmDTD.bEndOfStream())
    {
        strmDTD >> strLine;
        strDTDText.Append(strLine);
        strDTDText.Append(L"\n");
    }

    m_xtprsToUse.AddMapping
    (
        new TMemBufEntitySrc
        (
            L"http://www.charmedquark.com/CIDLib/CIDHelpDocs.DTD"
            , L"urn:charmedquark.com:CIDLib-Documentation.DTD"
            , strDTDText
        )
    );
}


// Format any errors we collected to the passed output stream
tCIDLib::TVoid TParseCtx::ShowErrors(TTextOutStream& strmOut)
{
    TString strLastSrc;
    m_colErrs.bForEach
    (
        [&strLastSrc, &strmOut](const TKeyValuePair& kvalCur)
        {
            if (kvalCur.strKey() != strLastSrc)
            {
                strmOut << L"  " << kvalCur.strKey() << kCIDLib::NewLn;
                strLastSrc = kvalCur.strKey();
            }
            strmOut << L"    " << kvalCur.strValue() << kCIDLib::NewLn;
            return kCIDLib::True;
        }
    );
    strmOut.Flush();
}
