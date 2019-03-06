//
// FILE NAME: TestFW_ThisFacility2.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/11/2007
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file contains grunt work methods of the facilty class, to keep
//  them out of the main facilty implementation file. These are mostly
//  related to the parsing of the parameters and the test configuration
//  file.
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
//  TFacTestFW: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This is called to parse the configuration file.
//
tCIDLib::TBoolean TFacTestFW::bParseCfgFile()
{
    // We had to get a config file, and it must exist
    if (m_strCfgFile.bIsEmpty())
    {
        *m_pstrmErr << L"No configuration file was provided\n"
                    << kCIDLib::EndLn;
        return kCIDLib::False;
    }
     else if (!TFileSys::bExists(m_strCfgFile))
    {
        *m_pstrmErr << L"The input config file was not found\n"
                    << kCIDLib::EndLn;
        return kCIDLib::False;
    }

    // Fully qualify the path
    const TString strTmp(m_strCfgFile);
    TFileSys::QueryFullPath(strTmp, m_strCfgFile);

    //
    //  It's there, so let's parse it. It's all line oriented so we can
    //  do a pretty simple line at a time parse. Comments are in the form
    //  that C++ comments are.
    //
    try
    {
        TTextFileInStream strmSrc
        (
            m_strCfgFile
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::None
            , tCIDLib::EFileFlags::SequentialScan
            , tCIDLib::EAccessModes::Excl_Read
        );
        m_c4LineNum = 0;

        //
        //  At this level, we should see the definitions of tests or
        //  groups. The tests have to come first, so we loop while we
        //  see tests.
        //
        ParseTestPrgs(strmSrc);

        //
        //  And now we can see some group definitions, but they are not
        //  required.
        //
        ParseGroups(strmSrc);
    }

    catch(const TString& strError)
    {
        *m_pstrmErr << L"(Line=" << m_c4LineNum
                    << L") " << strError << kCIDLib::EndLn;
        return kCIDLib::False;
    }

    catch(const TError& errToCatch)
    {
        *m_pstrmErr << L"(Line=" << m_c4LineNum
                    << L") Error while parsing config file:\n"
                    << errToCatch << kCIDLib::EndLn;
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


//
//  Parses out the next line. It skips comment lines, until it finds a none
//  empty line. It then strips the lead/trailing whitespace and returns
//  the line.
//
tCIDLib::TBoolean
TFacTestFW::bParseLine(         TTextInStream&      strmSrc
                        ,       TString&            strToFill
                        , const tCIDLib::TBoolean   bEOFOK)
{
    while (!strmSrc.bEndOfStream())
    {
        strmSrc >> strToFill;
        m_c4LineNum++;
        strToFill.StripWhitespace();

        if (!strToFill.bIsEmpty() && !strToFill.bStartsWith(L"//"))
            return kCIDLib::True;
    }

    if (strmSrc.bEndOfStream() && !bEOFOK)
    {
        TString strErr(L"Unexpected end of file.");
        throw strErr;
    }

    return !strmSrc.bEndOfStream();
}



//
//  Get the parameters. See the main program header for details.
//
tCIDLib::TBoolean TFacTestFW::bParseParms()
{
    TString strGroups;
    TString strCurParm;
    TSysInfo::TCmdLineCursor cursParms = TSysInfo::cursCmdLineParms();
    for (; cursParms; ++cursParms)
    {
        strCurParm = *cursParms;

        if (strCurParm.bStartsWithI(L"/Verbosity="))
        {
            strCurParm.Cut(0, 11);

            // Set the verbosity, validing the value
            if (strCurParm.bCompareI(L"low"))
                m_eVerbosity = tTestFWLib::EVerbosity::Low;
            else if (strCurParm.bCompareI(L"med"))
                m_eVerbosity = tTestFWLib::EVerbosity::Medium;
            else if (strCurParm.bCompareI(L"high"))
                m_eVerbosity = tTestFWLib::EVerbosity::High;
            else
            {
                *m_pstrmOut << L"The verbosity level was invalid. Use Low, Med, or High\n"
                            << kCIDLib::EndLn;
                return kCIDLib::False;
            }
        }
         else if (strCurParm.bStartsWithI(L"/CfgFile="))
        {
            strCurParm.Cut(0, 9);
            m_strCfgFile = strCurParm;
        }
         else if (strCurParm.bStartsWithI(L"/MaxLevel="))
        {
            strCurParm.Cut(0, 10);
            try
            {
                m_c4MaxLevel = strCurParm.c4Val(tCIDLib::ERadices::Dec);
            }

            catch(...)
            {
                *m_pstrmErr << L"Invalid /MaxLevel value, ignoring!"
                            << kCIDLib::EndLn;
            }
        }
         else if (strCurParm.bStartsWithI(L"/Groups="))
        {
            strCurParm.Cut(0, 8);
            strGroups = strCurParm;
        }
         else if (strCurParm.bStartsWithI(L"/OutFmt="))
        {
            strCurParm.Cut(0, 8);
            m_strOutFmt = strCurParm;
        }
         else if (strCurParm.bStartsWithI(L"/TestRoot="))
        {
            strCurParm.Cut(0, 10);
            m_strTestRoot = strCurParm;
        }
         else
        {
            *m_pstrmErr << L"Unknown parameter: " << strCurParm
                        << kCIDLib::EndLn;
            return kCIDLib::False;
        }
    }

    // Make sure that the output format is something valid
    if (!bCheckFmtType(m_strOutFmt, 0))
    {
        *m_pstrmErr << L"The output format is not a known type"
                    << kCIDLib::EndLn;
        return kCIDLib::False;
    }

    //
    //  If no test root was set, then set it to the current CIDBuild
    //  result directory.
    //
    if (m_strTestRoot.bIsEmpty())
    {
        if (!TProcEnvironment::bFind(L"CID_RESDIR", m_strTestRoot))
        {
            *m_pstrmErr << L"No test root directory is available"
                        << kCIDLib::EndLn;
            return kCIDLib::False;
        }
    }

    // If no result path, then set it to the CIDBuild result directory.
    if (m_strResPath.bIsEmpty())
    {
        if (!TProcEnvironment::bFind(L"CID_RESDIR", m_strResPath))
        {
            *m_pstrmErr << L"No test root directory is available"
                        << kCIDLib::EndLn;
            return kCIDLib::False;
        }
    }
    // Parse the configuration file. If it fails, return false
    if (!bParseCfgFile())
        return kCIDLib::False;

    //
    //  The base path for relative working directories indicated by test
    //  programs in the config file are relative to the config file path.
    //  So normalize the config file path and remove the name.ext, and
    //  this is that path base path.
    //
    m_pathTestBaseDir = m_strCfgFile;
    m_pathTestBaseDir.Normalize();
    m_pathTestBaseDir.bRemoveNameExt();

    //
    //  If we got specific groups to invoke, then parse them out and store
    //  them. If not, then we'll just create a bogus group and add all
    //  the defined tests to it.
    //
    if (strGroups.bIsEmpty())
    {
        TTFWGroupInfo& tfwgiNew = m_colGroupList.objAdd(TTFWGroupInfo());
        tfwgiNew.Set(L"<AllTests>", L"Automatically generated group for all tests");
        tfwgiNew.c4Id(m_c4NextGroupId++);

        const tCIDLib::TCard4 c4TestPrgCnt = m_colTestPrgList.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestPrgCnt; c4Index++)
        {
            TTFWTestPrgInfo& tfwtpiNew = m_colTestPrgList[c4Index];
            tfwgiNew.AddTestPrg(tfwtpiNew);
        }

        // And add this as the one group to do
        m_colGroupsToDo.objAdd(tfwgiNew.strName());
    }
     else
    {
        TString strTok;
        TStringTokenizer stokGroups(&strGroups, L",");
        while (stokGroups.bGetNextToken(strTok))
        {
            strTok.StripWhitespace();

            // Make sure the group exists
            if (c4FindGroup(strTok) == kCIDLib::c4MaxCard)
            {
                *m_pstrmErr << L"Group '" << strTok
                            << L"' was not found in the config file"
                            << kCIDLib::EndLn;
                return kCIDLib::False;
            }
            m_colGroupsToDo.objAdd(strTok);
        }
    }

    return kCIDLib::True;
}



//
//  Tries to find the indicated group in our list of groups. If not found
//  it returns kCIDLib::c4MaxCard.
//
tCIDLib::TCard4 TFacTestFW::c4FindGroup(const TString& strToFind) const
{
    const tCIDLib::TCard4 c4Count = m_colGroupList.c4ElemCount();
    tCIDLib::TCard4 c4Ret = 0;
    for (; c4Ret < c4Count; c4Ret++)
    {
        if (m_colGroupList[c4Ret].strName() == strToFind)
            break;
    }
    if (c4Ret == c4Count)
        c4Ret = kCIDLib::c4MaxCard;
    return c4Ret;
}


//
//  Tries to find the indicated test in our list of tests. If not found, it
//  returns kCIDLib::c4MaxCard.
//
tCIDLib::TCard4 TFacTestFW::c4FindTestPrg(const TString& strToFind) const
{
    const tCIDLib::TCard4 c4Count = m_colTestPrgList.c4ElemCount();
    tCIDLib::TCard4 c4Ret = 0;
    for (; c4Ret < c4Count; c4Ret++)
    {
        if (m_colTestPrgList[c4Ret].strName() == strToFind)
            break;
    }
    if (c4Ret == c4Count)
        c4Ret = kCIDLib::c4MaxCard;
    return c4Ret;
}


//
//  Gets the next line and makes sure it equals the passed expected value,
//  and another one that checks whether it starts with a paritcular prefix,
//  and if so it strips off the prefix, leaving the value.
//
tCIDLib::TVoid
TFacTestFW::CheckLine(          TTextInStream&      strmSrc
                        ,       TString&            strTmp
                        , const TString&            strExpected)
{
    if (!bParseLine(strmSrc, strTmp, kCIDLib::False) || (strTmp != strExpected))
    {
        TString strErr(L"Expected to see token '");
        strErr.Append(strExpected);
        strErr.Append(L"'");
        throw strErr;
    }
}

tCIDLib::TVoid
TFacTestFW::CheckLine(  const   TString&    strToCheck
                        , const TString&    strExpected)
{
    if (strToCheck != strExpected)
    {
        TString strErr(L"Expected to see token '");
        strErr.Append(strExpected);
        strErr.Append(L"'");
        throw strErr;
    }
}

tCIDLib::TVoid
TFacTestFW::CheckLinePref(          TTextInStream&      strmSrc
                            ,       TString&            strToFill
                            , const TString&            strPrefix)
{
    if (!bParseLine(strmSrc, strToFill, kCIDLib::False)
    ||  !strToFill.bStartsWith(strPrefix))
    {
        TString strErr(L"Expected to see token '");
        strErr.Append(strPrefix);
        strErr.Append(L"'");
        throw strErr;
    }
    strToFill.Cut(0, strPrefix.c4Length());
}


//
//  Parses out group info objects until we get to the end of them.
//
tCIDLib::TVoid TFacTestFW::ParseGroups(TTextInStream& strmSrc)
{
    TString strCurLn;

    // The next line should be the Groups= line
    CheckLine(strmSrc, strCurLn, L"Groups=");

    // Now we should see Group= blocks until we see the EndTests line
    TString strDescr;
    TString strName;
    while (kCIDLib::True)
    {
        //
        //  Get the next line end of line is ok here. If we get one,
        //  just break out.
        //
        if (!bParseLine(strmSrc, strCurLn, kCIDLib::True))
            break;

        // If it's the end of groups, then break out
        if (strCurLn == L"EndGroups;")
            break;

        // Else it's got to be a group
        if (!strCurLn.bStartsWith(L"Group="))
        {
            strCurLn = L"Expected to see a Group block";
            throw strCurLn;
        }

        // Get the name out
        strCurLn.Cut(0, 6);
        strName = strCurLn;

        // It can't be empty
        if (strName.bIsEmpty())
        {
            strCurLn = L"Groups must have a name";
            throw strCurLn;
        }

        // See if this name already exists
        if (c4FindGroup(strName) != kCIDLib::c4MaxCard)
        {
            strCurLn = L"Group '";
            strCurLn.Append(strName);
            strCurLn.Append(L"' already exists.");
            throw strCurLn;
        }

        // The next line should be the description
        CheckLinePref(strmSrc, strDescr, L"Description=");

        // Next we have to see a TestPrgs= line
        CheckLine(strmSrc, strCurLn, L"TestPrgs=");

        // Add a new group to our group list, giving it the next group id
        TTFWGroupInfo& tfwgiNew = m_colGroupList.objAdd(TTFWGroupInfo());
        tfwgiNew.Set(strName, strDescr);
        tfwgiNew.c4Id(m_c4NextGroupId++);

        // Loop until we see the EndTests line, storing the referenced tests
        while (kCIDLib::True)
        {
            bParseLine(strmSrc, strCurLn, kCIDLib::False);

            if (strCurLn == L"EndTestPrgs;")
                break;

            // Look it up as a test name
            const tCIDLib::TCard4 c4TestPrgInd = c4FindTestPrg(strCurLn);
            if (c4TestPrgInd == kCIDLib::c4MaxCard)
            {
                strCurLn = L"The referenced test program does not exist";
                throw strCurLn;
            }

            // Looks ok, so store it
            tfwgiNew.AddTestPrg(m_colTestPrgList[c4TestPrgInd]);
        }

        // And we must see the end group
        CheckLine(strmSrc, strCurLn, L"EndGroup;");
    }
}


//
//  Parses out test program info objects until we get to the end of them.
//
tCIDLib::TVoid TFacTestFW::ParseTestPrgs(TTextInStream& strmSrc)
{
    TString strCurLn;

    //
    //  The next line should be the TestPrgs= line. The name that comes out
    //  this is what we use as the overall test configuration name.
    //
    CheckLinePref(strmSrc, strCurLn, L"TestPrgs=");
    m_strTestCfgName = strCurLn;

    // Now we should see Test= blocks until we see the EndTests line
    TString strDescr;
    TString strName;
    TString strPath;
    TString strWorkDir;
    while (kCIDLib::True)
    {
        //
        //  Get the next line end of line is ok here. If we get one,
        //  just break out.
        //
        if (!bParseLine(strmSrc, strCurLn, kCIDLib::True))
            break;

        // If it's the end of tests, then break out
        if (strCurLn == L"EndTestPrgs;")
            break;

        // Else it's got to be a test program
        if (!strCurLn.bStartsWith(L"TestPrg="))
        {
            TString strErr(L"Expected to see a Test block");
            throw strErr;
        }

        // Get the name out
        strCurLn.Cut(0, 8);
        strName = strCurLn;

        // See if this name already exists
        if (c4FindTestPrg(strName) != kCIDLib::c4MaxCard)
        {
            strCurLn = L"Test program'";
            strCurLn.Append(strName);
            strCurLn.Append(L"' already exists.");
        }

        // The next lines should be the path and description
        CheckLinePref(strmSrc, strPath, L"TestPath=");
        CheckLinePref(strmSrc, strDescr, L"Description=");

        // Next we might see the working directory
        bParseLine(strmSrc, strCurLn, kCIDLib::False);
        if (strCurLn.bStartsWith(L"WorkDir="))
        {
            strCurLn.Cut(0, 8);
            strWorkDir = strCurLn;

            // And get another line
            bParseLine(strmSrc, strCurLn, kCIDLib::False);
        }

        // And we must see the end test program
        CheckLine(strCurLn, L"EndTestPrg;");

        //
        //  Make sure that the indicated program exists. It can start
        //  with <Root>, which we will replace with the value provided
        //  on the /TestRoot= parameter.
        //
        if (strPath.bStartsWith(L"<Root>"))
        {
            strPath.Cut(0, 6);
            strPath.Prepend(m_strTestRoot);
        }

        if (!TFileSys::bExists(strPath))
        {
            TString strFl(L"Test program '");
            strFl.Append(strPath);
            strFl.Append(L"' was not found");
            throw strFl;
        }

        // Add a new test, giving it the next available test id
        TTFWTestPrgInfo& tfwtpiNew = m_colTestPrgList.objAdd(TTFWTestPrgInfo());
        tfwtpiNew.c4Id(m_c4NextTestPrgId++);
        tfwtpiNew.Set(strName, strDescr, strPath, strWorkDir);
    }
}


// Shows the program blurb
tCIDLib::TVoid TFacTestFW::ShowBlurb()
{
    *m_pstrmOut << L"\nCIDLib Test Framework\n   Version: "
                << facCIDLib().strVersion() << kCIDLib::NewLn
                << kCIDLib::EndLn;
}



