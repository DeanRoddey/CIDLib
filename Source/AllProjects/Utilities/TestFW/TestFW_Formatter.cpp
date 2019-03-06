//
// FILE NAME: TestFW_Formatter.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2007
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  Implements the output formatting classes for the test framework.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Include underlying headers.
// ---------------------------------------------------------------------------
#include    "TestFW.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTFWFormatter,TObject)
RTTIDecls(TTFWHTMLFormatter,TTFWFormatter)



// ---------------------------------------------------------------------------
//  CLASS: TTFWFormatter
// PREFIX: tfwfm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTFWFormatter: Constructors and Destructors
// ---------------------------------------------------------------------------
TTFWFormatter::TTFWFormatter()
{
}

TTFWFormatter::~TTFWFormatter()
{
}


// ---------------------------------------------------------------------------
//  TTFWFormatter: Protected, non-virtual methods
// ---------------------------------------------------------------------------
const TTFWGroupInfo&
TTFWFormatter::tfwgiFindGrp(const   TString&                strToFind
                            , const tTestFW::TGroupList&    colGroupList)
{
    const tCIDLib::TCard4 c4Count = colGroupList.c4ElemCount();
    tCIDLib::TCard4 c4Index = 0;
    for (; c4Index < c4Count; c4Index++)
    {
        if (colGroupList[c4Index].strName() == strToFind)
            break;
    }

    if (c4Index == c4Count)
    {
        facTestFW.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kTestFWErrs::errcGen_GroupNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
            , strToFind
        );
    }
    return colGroupList[c4Index];
}



// ---------------------------------------------------------------------------
//  CLASS: TTFWHTMLFormatter
// PREFIX: tfwfm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTFWHTMLFormatter: Constructors and Destructors
// ---------------------------------------------------------------------------
TTFWHTMLFormatter::TTFWHTMLFormatter() :

    m_strNewLine(L"<BR>")
    , m_strSpace(L"&nbsp;")
    , m_strTab(L"&nbsp;&nbsp;&nbsp;&nbsp;")
    , m_strErrEmph(L"ErrEmph")
    , m_strWarnEmph(L"WarnEmph")
{
}

TTFWHTMLFormatter::~TTFWHTMLFormatter()
{
}


// ---------------------------------------------------------------------------
//  TTFWHTMLFormatter: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TTFWHTMLFormatter::FormatResult(const   TString&                strOutPath
                                , const TString&                strCfgFile
                                , const TString&                strTestCfgName
                                , const tCIDLib::TCard4         c4MaxLevel
                                , const tTestFW::TStatList&     fcolStats
                                , const tTestFW::TGroupList&    colGroupList
                                , const tTestFW::TNameList&     colGroupsRun)
{
    // STore the out path so that all the methods can se it
    m_strOutPath = strOutPath;

    // First we put out a summary page
    {
        TPathStr pathOut = m_strOutPath;
        pathOut.AddLevel(L"index.html");
        TTextFileOutStream strmTar
        (
            pathOut
            , tCIDLib::ECreateActs::CreateAlways
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
            , tCIDLib::EAccessModes::Excl_Write
            , new TUTF8Converter
        );

        // Put out the header stuff
        TString strHeader(L"Test Results for '");
        strHeader.Append(strTestCfgName);
        strHeader.Append(L'\'');
        WriteHeader(strmTar, L"Test Output Summary", strHeader);

        // Put out a little overall information for the summary
        TTime tmNow(tCIDLib::ESpecialTimes::CurrentTime);
        tmNow.strDefaultFormat(TTime::strCTime());
        TString strOSInfo;
        TSysInfo::QueryOSInfoStr(strOSInfo);
        TString strVersion;
        TProcEnvironment::bFind(L"CID_VERSION", strVersion);
        strmTar << L"<BLOCKQUOTE><P><B>Date:</B> " << tmNow
                << L"<BR><B>Host:</B> " << TSysInfo::strIPHostName()
                << L"<BR><B>Test Cfg File:</B> " << strCfgFile
                << L"<BR><B>Max Test Level:</B> " << c4MaxLevel
                << L"<BR><B>Build Version:</B> " << strVersion
                << L"<BR><B>OS Version:</B> " << strOSInfo
                << L"</P></BLOCKQUOTE>";

        // Output the overall statistics
        WriteStats(strmTar, fcolStats);

        // Generate a table that indicates the overall result for each group
        strmTar << L"<H3>Group Summary</H3><P><BLOCKQUOTE>\n"
                << L"<TABLE class='ResTable' width='45%'>\n"
                   L"<TR>\n"
                   L"<TD class='TableHdr' width='65%'>Group Name</TD>\n"
                   L"<TD class='TableHdr'>Result</TD>\n"
                   L"</TR>";

        // For each group that was run, write out a row
        TString strClass;
        const tCIDLib::TCard4 c4Count = colGroupsRun.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            // Look up the current run group name in the group list
            const TTFWGroupInfo& tfwgiCur = tfwgiFindGrp
            (
                colGroupsRun[c4Index], colGroupList
            );

            strmTar << L"<TR>";
            const TString& strResText = facTestFWLib().strResText(tfwgiCur.eResult());

            if ((tfwgiCur.eResult() == tTestFWLib::ETestRes::NotRun)
            ||  tfwgiCur.bAnyTestsNotRun())
            {
                strClass = L"class='WarnEmph'";
            }
             else if (tfwgiCur.eResult() == tTestFWLib::ETestRes::Success)
            {
                strClass.Clear();
            }
             else
            {
                strClass = L"class='ErrEmph'";
            }

            strmTar << L"<TD " << strClass << L"><A HREF='.\\GrpRes"
                    << tfwgiCur.c4Id() << L".html'>"
                    << strEscape(tfwgiCur.strName()) << L"</A></TD>"
                    << L"<TD " << strClass << L">" << strResText << L"</TD>";

            strmTar << L"</TR>\n";
        }

        strmTar << L"</TABLE></BLOCKQUOTE></P>";


        // Make sure it's all flushed out
        strmTar.Flush();
    }

    //
    //  Generate the results pages for all the groups. The group result table
    //  method above generated a link in each row for the files we'll write
    //  out here.
    //
    //  Each group result page will contain links to the test program results
    //  page for all of the tests programs in that test.
    //
    WriteGroupResults(colGroupList);
}


// ---------------------------------------------------------------------------
//  TTFWHTMLFormatter: Private, inherited methods
// ---------------------------------------------------------------------------

//
//  Does a cheap HTML escaping on the passed string and returns the escaped
//  version. This is a very innefficient way to do it, but it's convenient
//  and performance is not a big concern here.
//
TString TTFWHTMLFormatter::strEscape(const TString& strToEscape)
{
    TString strRet(strToEscape.c4Length() + 64);
    const tCIDLib::TCh* pszCur = strToEscape.pszBuffer();
    const tCIDLib::TCh* pszEnd = pszCur + strToEscape.c4Length();
    while (pszCur < pszEnd)
    {
        const tCIDLib::TCh chCur = *pszCur++;

        switch(chCur)
        {
            case kCIDLib::chLessThan :
                strRet.Append(L"&lt;");
                break;

            case kCIDLib::chGreaterThan :
                strRet.Append(L"&gt;");
                break;

            case kCIDLib::chAmpersand :
                strRet.Append(L"&amp;");
                break;

            case kCIDLib::chTab :
                // Convert to a sequence of non-breaking spaces
                strRet.Append(m_strTab);
                break;

            case kCIDLib::chCR :
                // Eat it
                break;

            case kCIDLib::chLF :
                // Convert to an HTML break
                strRet.Append(m_strNewLine);
                break;

            case kCIDLib::chSpace :
                // Convert to an HTML non-breaking space
                strRet.Append(m_strSpace);
                break;

            default :
                // Nothing special, just take it as is
                strRet.Append(chCur);
                break;
        };
    }
    return strRet;
}


// Writes out a common footer for all pages
tCIDLib::TVoid TTFWHTMLFormatter::WriteFooter(TTextOutStream& strmTar)
{
    strmTar << L"</BODY></HTML>";
}


//
//  This is called after the main summary page has been generated. The
//  group summary table
//
tCIDLib::TVoid
TTFWHTMLFormatter::WriteGroupResults(const tTestFW::TGroupList& colGrpList)
{
    //
    //  For each group, we need to iterate the tests and output a details
    //  page.
    //
    TPathStr    pathCurFile;
    TString     strHeader;
    const tCIDLib::TCard4 c4GrpCount = colGrpList.c4ElemCount();
    for (tCIDLib::TCard4 c4GIndex = 0; c4GIndex < c4GrpCount; c4GIndex++)
    {
        // Get the current group and write the group results page for it
        const TTFWGroupInfo& tfwgiCur = colGrpList[c4GIndex];
        WriteOneGroupResult(tfwgiCur);
    }
}


// Writes out a common header for all files, with common styles
tCIDLib::TVoid
TTFWHTMLFormatter::WriteHeader(         TTextOutStream& strmTar
                                , const TString&        strTitle
                                , const TString&        strHeading)
{
    strmTar << L"<HTML><TITLE>" << strEscape(strTitle)
            << L"</TITLE><BODY><HEAD><STYLE>"
               L"<!-- \n"
               L"* { font-family: Verdana, sans-serif; font-size: 11;}\n"
               L"H2 { font-family: Verdana, sans-serif; font-size: 20;}\n"
               L"H3 { font-family: Verdana, sans-serif; font-size: 16;}\n"
               L"table.ResTable { border-color:gray; border-width:1px; border-collapse:collapse; border-style:solid; border-spacing:0; }\n"
               L"table.ResTable td { border-color:gray; border-width:1px; border-style:solid; padding:2; }\n"
               L".TableHdr { font-family: Verdana, sans-serif; font-weight:bolder; font-size:12; background-color:#99CCFF; }\n"
               L".GroupHdr { background-color:#C0C0C0; }\n"
               L".ErrEmph { font-family: Verdana, sans-serif; font-weight:bold; color:#FF0000; }\n"
               L".WarnEmph { font-family: Verdana, sans-serif; font-weight:bold; color:#0000FF; }\n"
               L" --></STYLE></HEAD>\n\n"
               L"<TABLE border='1' width='100%' cellspacing='0' bgcolor='#99CCFF'>\n"
               L"<TR><TD><H2>" << strEscape(strHeading)
            << L"</H2></TD></TR></TABLE>\n\n";
}


// Writes the table of links to the test program pages
tCIDLib::TVoid
TTFWHTMLFormatter::WriteLinksTable(         TTextOutStream&      strmTar
                                    , const tTestFW::TGroupList& colGroupList)
{
    strmTar << L"<P/><H3>Test Links</H3><P><BLOCKQUOTE>\n"
            << L"<TABLE class='ResTable' width='85%'>\n"
               L"<TR>\n"
               L"<TD class='TableHdr' width='80%'>Link</TD>\n"
               L"<TD class='TableHdr'>Status</TD>\n"
               L"</TR>";

    strmTar << L"</TABLE></BLOCKQUOTE></P>";
}


//
//  Writes out a results page for the passed group. This includes a table
//  with links to the results for all the test programs in the group.
//
tCIDLib::TVoid
TTFWHTMLFormatter::WriteOneGroupResult(const TTFWGroupInfo& tfwgiSrc)
{
    // Create the file and write out the header
    TPathStr pathCurFile = m_strOutPath;
    pathCurFile.AddLevel(L"GrpRes");
    pathCurFile.AppendFormatted(tfwgiSrc.c4Id());
    pathCurFile.AppendExt(L"html");
    TTextFileOutStream strmTar
    (
        pathCurFile
        , tCIDLib::ECreateActs::CreateAlways
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Excl_Write
        , new TUTF8Converter
    );

    TString strHeader = L"Results for Group '";
    strHeader.Append(tfwgiSrc.strName());
    strHeader.Append(L'\'');
    WriteHeader(strmTar, L"Group Results Report", strHeader);

    strmTar << L"<BLOCKQUOTE><P><B>Group Description:</B> "
            << strEscape(tfwgiSrc.strDescription())
            << L"</P></BLOCKQUOTE>";

    //
    //  First we write out a test program results table with one row
    //  per test program in the group.
    //
    strmTar << L"<H3>Test Program Summary</H3><P><BLOCKQUOTE>\n"
            << L"<TABLE class='ResTable' width='45%'>\n"
               L"<TR>\n"
               L"<TD class='TableHdr' width='65%'>Test Program Name</TD>\n"
               L"<TD class='TableHdr'>Result</TD>\n"
               L"</TR>";

    // For each test program, write out a row
    const tCIDLib::TCard4 c4Count = tfwgiSrc.c4TestCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        // Get the current test program info from the sourc group
        const TTFWTestPrgInfo& tfwtpiCur = tfwgiSrc.tfwtpiAt(c4Index);

        strmTar << L"<TR>";

        //
        //  Set the result text that we'll write out, which reflects the
        //  phase that the program got to. If it completed, then we want
        //  to check if any of the program failed and add to it to indicate
        //  the program ran to completion but some tests failed.
        //
        TString strPhaseText = facTestFWLib().strPhaseText(tfwtpiCur.ePhase());
        tCIDLib::TBoolean bFailures = kCIDLib::False;
        if (tfwtpiCur.ePhase() == tTestFWLib::EPhases::Completed)
        {
            bFailures = tfwtpiCur.tfwtrOutput().bAnyFailures();
            if (bFailures)
                strPhaseText.Append(L" with Errors");
        }

        if ((tfwtpiCur.ePhase() == tTestFWLib::EPhases::Completed) && !bFailures)
        {
            strmTar << L"<TD><A HREF='.\\TestPrgRes"
                    << tfwtpiCur.c4Id() << L".html'>"
                    << strEscape(tfwtpiCur.strName()) << L"</A></TD>";

            if (tfwtpiCur.tfwtrOutput().bAnyNotRun())
                strmTar << L"<TD class='WarnEmph'>";
            else
                strmTar << L"<TD>";
            strmTar << strPhaseText << L"</TD>";
        }
         else
        {
            strmTar << L"<TD class='ErrEmph'><A HREF='.\\TestPrgRes"
                    << tfwtpiCur.c4Id() << L".html'>"
                    << strEscape(tfwtpiCur.strName())
                    << L"</A></TD><TD class='ErrEmph'>"
                    << strPhaseText << L"</TD>";
        }
        strmTar << L"</TR>\n";

        //
        //  Generate the test program results page for this test program.
        //  It'll generate the file we just create the link to above.
        //
        WriteTestProgramResults(tfwtpiCur);
    }

    // Close off the table and write the standard footer
    strmTar << L"</TABLE></BLOCKQUOTE></P>";
    WriteFooter(strmTar);
}


//
//  Writes out the overall statistics table on the summary page. We show
//  failures in an emphasis style, and highlight the totals rows with the
//  things that go into those totals indented under them.
//
tCIDLib::TVoid
TTFWHTMLFormatter::WriteStats(          TTextOutStream&     strmTar
                                , const tTestFW::TStatList  fcolStats)
{
    strmTar << L"<H3>Overall Statistics</H3><P><BLOCKQUOTE>\n"
            << L"<TABLE class='ResTable' width='45%'>\n"
               L"<TR>\n"
               L"<TD class='TableHdr' width='85%'>Statistic</TD>\n"
               L"<TD class='TableHdr'>Value</TD>\n"
               L"</TR>";

    // For each of the statistics, put out a row
    WriteSTR
    (
        strmTar
        , L"Total Test Program Runs"
        , fcolStats[tTestFW::EStats::TotalTestPrgs]
        , TString::strEmpty()
        , kCIDLib::True
    );
    WriteSTR
    (
        strmTar
        , L"Failed Test Program Runs"
        , fcolStats[tTestFW::EStats::FailedTestPrgs]
        , m_strErrEmph
    );
    WriteSTR
    (
        strmTar
        , L"Exceptions"
        , fcolStats[tTestFW::EStats::Exceptions]
        , m_strErrEmph
    );

    WriteSTR
    (
        strmTar
        , L"Total Groups"
        , fcolStats[tTestFW::EStats::TotalGrps]
        , TString::strEmpty()
        , kCIDLib::True
    );
    WriteSTR
    (
        strmTar
        , L"Failed Groups"
        , fcolStats[tTestFW::EStats::FailedGrps]
        , m_strErrEmph
    );
    WriteSTR
    (
        strmTar
        , L"Passed Groups"
        , fcolStats[tTestFW::EStats::PassedGrps]
        , TString::strEmpty()
    );

    WriteSTR
    (
        strmTar
        , L"Total Tests"
        , fcolStats[tTestFW::EStats::TotalTests]
        , TString::strEmpty()
        , kCIDLib::True
    );
    WriteSTR
    (
        strmTar
        , L"Failed Tests"
        , fcolStats[tTestFW::EStats::FailedTests]
        , m_strErrEmph
    );
    WriteSTR
    (
        strmTar
        , L"Warnings"
        , fcolStats[tTestFW::EStats::Warnings]
        , m_strWarnEmph
    );
    WriteSTR
    (
        strmTar
        , L"Not Run"
        , fcolStats[tTestFW::EStats::NotRun]
        , m_strWarnEmph
    );
    WriteSTR
    (
        strmTar
        , L"Passed Tests"
        , fcolStats[tTestFW::EStats::PassedTests]
        , TString::strEmpty()
    );
    strmTar << L"</TABLE></BLOCKQUOTE></P>";
}


// Writes out a single name/value row in the overall statistics table
tCIDLib::TVoid TTFWHTMLFormatter
::WriteSTR(       TTextOutStream&     strmTar
                    , const TString&            strStatName
                    , const tCIDLib::TCard4     c4Value
                    , const TString&            strNZEmph
                    , const tCIDLib::TBoolean   bGroupHdr)
{
    TString strClass1;
    TString strClass2;
    if (!strNZEmph.bIsEmpty() && c4Value)
    {
        strClass1 = L"<FONT class='";
        strClass1.Append(strNZEmph);
        strClass1.Append(L"'>");
        strClass2 = L"</FONT>";
    }

    strmTar << L"<TR>";

    if (bGroupHdr)
        strmTar << L"<TD class='GroupHdr'>";
    else
        strmTar << L"<TD>";

    if (bGroupHdr)
        strmTar << L"<B>";
    else
        strmTar << L"&nbsp;&nbsp;";
    strmTar << strClass1 << strEscape(strStatName) << strClass2;
    if (bGroupHdr)
        strmTar << L"</B>";

    strmTar << L"</TD>";

    // Do the value column
    if (bGroupHdr)
        strmTar << L"<TD class='GroupHdr'>";
    else
        strmTar << L"<TD>";

    if (bGroupHdr)
        strmTar << L"<B>";
    strmTar << strClass1 << c4Value << strClass2;
    if (bGroupHdr)
        strmTar << L"</B>";

    strmTar << L"</TD>";

    strmTar << L"<TR>\n";
}


//
//  Writes out the page for a results of the passed test program. This is
//  called from the method that generates the results page for a group. For
//  each test program in the group it calls this method. It generates links
//  in its test program results table that refer to the files we generate
//  here.
//
tCIDLib::TVoid
TTFWHTMLFormatter::WriteTestProgramResults(const TTFWTestPrgInfo& tfwtpiSrc)
{
    // Create the file and write out the header
    TPathStr pathCurFile = m_strOutPath;
    pathCurFile.AddLevel(L"TestPrgRes");
    pathCurFile.AppendFormatted(tfwtpiSrc.c4Id());
    pathCurFile.AppendExt(L"html");
    TTextFileOutStream strmTar
    (
        pathCurFile
        , tCIDLib::ECreateActs::CreateAlways
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Excl_Write
        , new TUTF8Converter
    );

    TString strHeader = L"Results for Test Program '";
    strHeader.Append(tfwtpiSrc.strName());
    strHeader.Append(L'\'');
    WriteHeader(strmTar, L"Test Program Results Report", strHeader);

    strmTar << L"<BLOCKQUOTE><P><B>Program Description:</B> "
            << strEscape(tfwtpiSrc.strDescription())
            << L"</P></BLOCKQUOTE>";

    //
    //  If the phase doesn't indicate that it completed, then we generate
    //  an error page. Else, we output the results summary of the individual
    //  tests.
    //
    if (tfwtpiSrc.ePhase() == tTestFWLib::EPhases::Completed)
    {
        // Get a ref to the test results for this test program
        const TTFWTestRes& tfwtrCur = tfwtpiSrc.tfwtrOutput();

        // And generate a table of results for each test result
        strmTar << L"<H3>Test Summary</H3><P><BLOCKQUOTE>\n"
                << L"<TABLE class='ResTable' width='45%'>\n"
                   L"<TR>\n"
                   L"<TD class='TableHdr' width='65%'>Test Name</TD>\n"
                   L"<TD class='TableHdr'>Result</TD>\n"
                   L"</TR>";

        const tCIDLib::TCard4 c4TestCnt = tfwtrCur.c4ResCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestCnt; c4Index++)
        {
            // Get the current test result and do a row for it
            const TTFWOneTestRes& tfwotrCur = tfwtrCur.tfwotrAt(c4Index);

            strmTar << L"<TR>\n";
            const TString& strResText = facTestFWLib().strResText(tfwotrCur.eResult());
            if (tfwotrCur.eResult() >= tTestFWLib::ETestRes::NotRun)
            {
                //
                //  In this case, there's nothing to report, so we don't
                //  generate a link.
                //
                strmTar << L"<TD>" << strEscape(tfwotrCur.strSubName())
                        << L"</TD><TD>" << strResText << L"</TD>";
            }
             else
            {
                //
                //  It failed, so we need to generate a link in this case.
                //
                strmTar << L"<TD class='ErrEmph'><A HREF='.\\TestRes"
                        << tfwtpiSrc.c4Id() << L'_' << (c4Index + 1) << L".html'>"
                        << strEscape(tfwotrCur.strSubName())
                        << L"</A></TD><TD class='ErrEmph'>"
                        << strResText << L"</TD>";

                //
                //  Generate the test results page for this test. It'll generate
                //  the file we just created the link to above.
                //
                WriteTestResult(tfwtpiSrc, c4Index);
            }
            strmTar << L"</TR>\n";
        }

        // Close off the table
        strmTar << L"</TABLE></BLOCKQUOTE></P>";
    }
     else
    {
        // Format out the error information that was returned
        strmTar << L"<H3>Test Results</H3><P><BLOCKQUOTE><p>\n"
                   L"<FONT class='ErrEmph'>"
                   L"<B>Program failed to run to completion.<BR>Phase State="
                << strEscape(facTestFWLib().strPhaseText(tfwtpiSrc.ePhase()))
                << L"<BR>Error output=</FONT></B><BLOCKQUOTE>"
                << strEscape(tfwtpiSrc.strErrInfo())
                << L"</BLOCKQUOTE>";
    }

    // And write out the standard footer
    WriteFooter(strmTar);
}



//
//  Called from the test program results output method above. This is called
//  for each test result.
//
tCIDLib::TVoid
TTFWHTMLFormatter::WriteTestResult( const   TTFWTestPrgInfo&    tfwtpiSrc
                                    , const tCIDLib::TCard4     c4TestInd)
{
    // Create the file and write out the header
    TPathStr pathCurFile = m_strOutPath;
    pathCurFile.AddLevel(L"TestRes");
    pathCurFile.AppendFormatted(tfwtpiSrc.c4Id());
    pathCurFile.Append(L'_');
    pathCurFile.AppendFormatted(c4TestInd + 1);
    pathCurFile.AppendExt(L"html");
    TTextFileOutStream strmTar
    (
        pathCurFile
        , tCIDLib::ECreateActs::CreateAlways
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Excl_Write
        , new TUTF8Converter
    );

    // Get the indicated test result out
    const TTFWTestRes& tfwtrSrc = tfwtpiSrc.tfwtrOutput();
    const TTFWOneTestRes& tfwotrSrc = tfwtrSrc.tfwotrAt(c4TestInd);

    TString strHeader = L"Results for Test '";
    strHeader.Append(tfwtpiSrc.strName());
    strHeader.Append(L'/');
    strHeader.Append(tfwotrSrc.strSubName());
    strHeader.Append(L'\'');
    WriteHeader(strmTar, L"Test Results Report", strHeader);

    strmTar << L"<BLOCKQUOTE><P><B>Test Description:</B> "
            << strEscape(tfwotrSrc.strDescription())
            << L"</P></BLOCKQUOTE>";

    //
    //  We would not get called here if the result was success. We only
    //  get called if it fails in some way, else there's nothing to report.
    //
    strmTar << L"<H3>Test Results</H3><P><BLOCKQUOTE><p>\n"
               L"<FONT class='ErrEmph'>"
               L"<B>Test failed to run to completion.<BR>Result="
            << strEscape(facTestFWLib().strResText(tfwotrSrc.eResult()))
            << L"<BR>Error output=</FONT></B><BLOCKQUOTE>"
            << strEscape(tfwotrSrc.strOutput())
            << L"</BLOCKQUOTE>";
    WriteFooter(strmTar);
}

