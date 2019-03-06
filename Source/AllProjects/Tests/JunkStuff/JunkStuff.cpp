#include "CIDRegX.hpp"

tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
        , tCIDLib::TVoid*   pData
);
CIDLib_MainModule(TThread(L"JunkStuffMainThread", eMainThreadFunc))

TTextOutStream& strmOut = TSysInfo::strmOut();
const TString strQuickTest(L"#if ");
TRegEx  regxGuard;
TString strCurLine;
TString strTmpFile;
TString strInclude;
TString strEndIf;
tCIDLib::TCard4 c4UniqueCnt = 1;

tCIDLib::TVoid ProcessFile(const TString& strCurFile)
{
    // Create a temp file name to write the new stuff to
//    TFileSys::CreateTmpFileNameIn(strCurFile, strTmpFile, c4UniqueCnt++);

    // Remember if we made changes since no need replace original if not
    tCIDLib::TBoolean bMadeChanges = kCIDLib::False;
    try
    {
        /*
        TTextFileInStream strmSrc
        (
            strCurFile
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
            , tCIDLib::EAccessModes::Read
        );

        TTextFileOutStream strmTar
        (
            strTmpFile
            , tCIDLib::ECreateActs::CreateIfNew
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
            , tCIDLib::EAccessModes::Write
        );

        while (!strmSrc.bEndOfStream())
        {
            strmSrc >> strCurLine;

            //
            //  If its a conditional and it matches the guard
            //  expression, then let's check it. Else just pass
            //  it on to the output.
            //
            if (strCurLine.bStartsWith(strQuickTest)
            &&  regxGuard.bFullyMatches(strCurLine))
            {
                // Read the next two lines
                strmSrc >> strInclude >> strEndIf;

                // Make sure it's what we expect
                if (strInclude.bStartsWith(L"#include ")
                &&  strEndIf.bStartsWith(L"#endif"))
                {
                    strmTar << strInclude << kCIDLib::NewLn;
                    bMadeChanges = kCIDLib::True;
                }
                 else
                {
                    // Assume the worst and keep the original stuff
                    strmTar << strCurLine << kCIDLib::NewLn
                            << strInclude << kCIDLib::NewLn
                            << strEndIf << kCIDLib::NewLn;
                }
            }
             else
            {
                strmTar << strCurLine << kCIDLib::NewLn;
            }
        }
        strmTar.Flush();
        */
    }

    catch(TError& errToCatch)
    {
        TStreamIndentJan janIndent(&strmOut, 4);
        strmOut  << L"An error occured on file: "
                << strCurFile << kCIDLib::NewLn;

        TStreamIndentJan janIndent2(&strmOut, 4);
        strmOut  << errToCatch << kCIDLib::EndLn;

        // Make sure this is off, keep the original
        bMadeChanges = kCIDLib::False;
    }

    //
    //  If we made changes, swap over the tmp file to replace the
    //  original. If not, the tmp file will be deleted if it got
    //  created.
    //
    try
    {
//        if (bMadeChanges)
//            TFileSys::ReplaceFile(strCurFile, strTmpFile);
//        else if (TFileSys::bExists(strTmpFile))
//            TFileSys::DeleteFile(strTmpFile);
    }

    catch(TError& errToCatch)
    {
        TStreamIndentJan janIndent(&strmOut, 4);
        strmOut  << L"\n\nCould not rename/delete temp file:\n";
        TStreamIndentJan janIndent2(&strmOut, 4);
        strmOut  << errToCatch << kCIDLib::EndLn;
    }
}


static tCIDLib::TVoid ProcessTree(const TString& strStartPath)
{
    TDirIterCB diterFind;
    diterFind.bIterate
    (
        strStartPath, L"*.?pp", kCIDLib::False,
        [](const TString& strCur, const tCIDLib::TBoolean bIsFile)
        {
            if (!bIsFile)
                strmOut << L"Processing Directory: " << strCur << kCIDLib::EndLn;
            ProcessFile(strCur);
            return kCIDLib::True;
        }
    );

}



tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    // Get the name of the facility to do
    TString strFacName = TSysInfo::strCmdLineParmAt(0);

    // Get the two top level source directories
    TPathStr pathCIDTop;
    TPathStr pathCQCTop;

    if (!TProcEnvironment::bFind(L"CIDLIB_SRCDIR", pathCIDTop))
    {
        strmOut  << L"Could not find CIDLib source dir in the environment"
                << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::Normal;
    }

    if (!TProcEnvironment::bFind(L"CID_SRCTREE", pathCQCTop))
    {
        strmOut  << L"Could not find CQC source dir in the environment"
                << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::Normal;
    }

    // Expand them both out to the right spot to start
    pathCIDTop.AddLevels(L"Source", L"AllProjects");
    pathCQCTop.AddLevels(L"Source", L"AllProjects");

    // Build the value of the guard token for this facility
    TString strGuard(TStrCat(strFacName, L"_HPP"));
    strGuard.ToUpper();

    //  Build up a regular expression for the start line we are looking for.
    TString strExpr(L"#if *!defined\\(", strGuard.c4Length() + 16);
    strExpr += strGuard;
    strExpr += L"\\).*$";
    regxGuard.SetExpression(strExpr);

    // Do both of the top level trees
    ProcessTree(pathCIDTop);
    ProcessTree(pathCQCTop);

    return tCIDLib::EExitCodes::Normal;
}

