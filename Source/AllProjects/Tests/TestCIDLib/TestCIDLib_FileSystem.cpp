//
// FILE NAME: TestCIDLib_FileSystem.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/28/1993
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This module is part of the TestCIDLib.Exe program. This module is called
//  from the program's main function. The functions in this module test the
//  file and file system classes to make sure that they are functioning
//  correctly.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// -----------------------------------------------------------------------------
//  Facility specific includes
// -----------------------------------------------------------------------------
#include    "TestCIDLib.hpp"


// -----------------------------------------------------------------------------
//  Local constant data
//
//  Note the extension "TestFile", which I've set up in my build shell's
//  scrub list. This way it will get cleaned up when I scrub this project.
// -----------------------------------------------------------------------------
static const TString    strBinFileName(L"BinaryFileTest.TestFile");


// -----------------------------------------------------------------------------
//  Local functions
// -----------------------------------------------------------------------------
static tCIDLib::TVoid TestBasicFileSystem(TTextOutStream& strmOut)
{
    //
    //  Get the current CID_SRCTREE environment variable, which should be set
    //  when doing development because its needed by the CIDBuild program.
    //
    TPathStr pathRoot;
    if (!TProcEnvironment::bFind(L"CID_SRCTREE", pathRoot))
    {
        strmOut << CUR_LN << L"The CID_SRCTREE environment variable was not set"
                << kCIDLib::EndLn;
        return;
    }

    //
    //  Now use it to build up a path to the directory that this test program
    //  is in. We'll use this as a base directory to test for some files
    //  in this directory.
    //
    pathRoot.AddLevel(L"Source");
    pathRoot.AddLevel(L"AllProjects");
    pathRoot.AddLevel(L"Tests");
    pathRoot.AddLevel(L"TestCIDLib");
    TPathStr pathTmp;

    //
    //  Do a simple existence test, with not particular test for the type
    //  of the entry.
    //
    pathTmp = pathRoot;
    pathTmp.AddLevel(L"TestCIDLib.hpp");
    if (!TFileSys::bExists(pathTmp))
        strmOut <<  CUR_LN << L"Failed to find " << pathTmp << kCIDLib::EndLn;

    //
    //  Do the same one again, indicating that we only want to see normal
    //  files, which should work fine also.
    //
    if (!TFileSys::bExists(pathTmp, tCIDLib::EDirSearchFlags::NormalFiles))
    {
        strmOut << CUR_LN
                << L"Failed to find normal file " << pathTmp
                << kCIDLib::EndLn;
    }

    // Make sure it does not show up as a directory
    if (TFileSys::bIsDirectory(pathTmp))
    {
        strmOut << CUR_LN
                << L"Non-directory showed up as a directory" << kCIDLib::EndLn;
    }

    // Try the version that concatenates the path for us
    if (!TFileSys::bExists(pathRoot, L"TestCIDLib_FileSystem.cpp"))
    {
        pathTmp = pathRoot;
        pathTmp.AddLevel(L"TestCIDLib_FileSystem.cpp");
        strmOut <<  CUR_LN << L"Did not find " << pathTmp << kCIDLib::EndLn;
    }

    //
    //  Check that the root directory we are using is found by the specialized
    //  directory existence check.
    //
    if (!TFileSys::bIsDirectory(pathRoot))
    {
        strmOut << CUR_LN
                << L"Directory " << pathRoot << L" not found." << kCIDLib::EndLn;
    }
}


tCIDLib::TVoid TestBinaryFiles(TTextOutStream& strmOut)
{
    tCIDLib::TCard4         ac4Test[100], c4Ind;
    tCIDLib::TCard4         c4Tmp;

    // Create a test binary file object
    TBinaryFile bflTest(strBinFileName);

    // Try to create it, truncating it if it exists
    bflTest.Open
    (
        tCIDLib::EAccessModes::Excl_ReadWrite
        , tCIDLib::ECreateActs::CreateAlways
        , tCIDLib::EFilePerms::AllAccess
        , tCIDLib::EFileFlags::RandomAccess
    );

    // Fill in the array with test data
    for (c4Ind = 0; c4Ind < 100; c4Ind++)
        ac4Test[c4Ind] = c4Ind;

    // Write out the data
    bflTest.c4WriteBuffer(ac4Test, sizeof(ac4Test));

    // Seek back to the start of the file
    bflTest.SetFilePos(0);

    //
    //  Read in the data again, but zero out the buffer first to make sure we
    //  really read it.
    //
    TRawMem::SetMemBuf(ac4Test, kCIDLib::c1MinCard, sizeof(ac4Test));
    bflTest.c4ReadBuffer(ac4Test, sizeof(ac4Test));

    // Test the contents
    for (c4Ind = 0; c4Ind < 100; c4Ind++)
    {
        if (ac4Test[c4Ind] != c4Ind)
        {
            strmOut << CUR_LN
                    << L"Buffer on disk (" << ac4Test[c4Ind]
                    << L" != data written (" << c4Ind << L")" << kCIDLib::EndLn;
            break;
        }
    }

    // Now close the file
    bflTest.Close();

    // Now do an open on the file now, since we know it exists
    bflTest.Open
    (
        tCIDLib::EAccessModes::Excl_ReadWrite
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::None
        , tCIDLib::EFileFlags::RandomAccess
    );

    //
    //  Read in the data again, but zero out the buffer first to make sure we
    //  really read it.
    //
    TRawMem::SetMemBuf(ac4Test, kCIDLib::c1MinCard, sizeof(ac4Test));
    bflTest.c4ReadBuffer(ac4Test, sizeof(ac4Test));

    // Test the contents
    for (c4Ind = 0; c4Ind < 100; c4Ind++)
    {
        if (ac4Test[c4Ind] != c4Ind)
        {
            strmOut << CUR_LN
                    << L"Buffer on disk (" << ac4Test[c4Ind]
                    << L" != data written (" << c4Ind << L")" << kCIDLib::EndLn;
            return;
        }
    }

    // Seek to the 10th item and read it in
    bflTest.SetFilePos(10 * sizeof(tCIDLib::TCard4));
    bflTest.c4ReadBuffer(&c4Tmp, sizeof(tCIDLib::TCard4));

    // Check the value
    if (c4Tmp != 10)
    {
        strmOut << CUR_LN
                << L"Read of 10th byte was not correct value failed. Value="
                << c4Tmp << kCIDLib::EndLn;
    }

    // Now close the file
    bflTest.Close();

    //
    //  Open it again and read it via the timeout'able version of the read,
    //  which should create the same results.
    //
    bflTest.Open
    (
        tCIDLib::EAccessModes::Excl_ReadWrite
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::None
        , tCIDLib::EFileFlags::Overlapped
    );

    //
    //  Read in the data again, but zero out the buffer first to make sure we
    //  really read it.
    //
    TEvent evWait;
    TRawMem::SetMemBuf(ac4Test, kCIDLib::c1MinCard, sizeof(ac4Test));
    bflTest.c4ReadBufferTO(ac4Test, sizeof(ac4Test), 100, evWait, kCIDLib::True);

    // Test the contents
    for (c4Ind = 0; c4Ind < 100; c4Ind++)
    {
        if (ac4Test[c4Ind] != c4Ind)
        {
            strmOut << CUR_LN
                    << L"Buffer on disk (" << ac4Test[c4Ind]
                    << L" != data written (" << c4Ind << L")" << kCIDLib::EndLn;
            return;
        }
    }

}


static tCIDLib::TVoid TestBinStreamIO(TTextOutStream& strmOut)
{
    TBinFileOutStream strmTestOut
    (
        strBinFileName
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::AllAccess
        , tCIDLib::EFileFlags::RandomAccess
    );

    TBinFileInStream strmTestIn
    (
        strBinFileName
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::AllAccess
        , tCIDLib::EFileFlags::RandomAccess
    );

    // Write out a find buffer and read it back in
    strmTestOut.Reset();
    TFindBuf fndbToWrite(L"%M");
    strmTestOut << fndbToWrite << kCIDLib::FlushIt;

    strmTestIn.Reset();
    TFindBuf fndbToRead;
    strmTestIn >> fndbToRead;

    // Make sure it came back the same
    if (fndbToWrite != fndbToRead)
    {
        strmOut << CUR_LN
                << L"Stream I/O of monomorphic TFindBuf object failed" << kCIDLib::EndLn;
    }

    //
    //  Now lets test polymorphic streaming. We seek back on the stream
    //  and write out a set of objects of various types.
    //
    strmTestOut.Reset();
    strmTestIn.Reset();

    //
    //  Create an object and stream it out polymorphically. We create the
    //  initial size of the string as less than the initialize string on
    //  purpose. It should expand up to the 16 and get it all!
    //
    TString strTest(L"String8-8", 8);
    ::PolymorphicWrite(&strTest, strmTestOut);

    // And do another object, this time an area object
    TArea   areaTest(10, 10, 100UL, 100UL);
    ::PolymorphicWrite(&areaTest, strmTestOut);

    // And now a date object
    TTime  tmTest;
    ::PolymorphicWrite(&tmTest, strmTestOut);

    // And flush it
    strmTestOut.Flush();

    //
    //  Seek it back now and read back in the objects, and make sure we
    //  get back the types we wrote.
    //
    TString*    pstrTest = 0;
    TArea*      pareaTest = 0;
    TTime*      ptmTest = 0;

    ::PolymorphicRead(pstrTest, strmTestIn);
    if (!pstrTest->bIsA(TString::clsThis()))
        strmOut << CUR_LN << L"Polymorphic read of TString failed." << kCIDLib::EndLn;
    if (*pstrTest != TString(L"String8-8", 8))
    {
        strmOut << CUR_LN
                << L"Polymorphic read of TString had wrong contents" << kCIDLib::EndLn;
    }
    delete pstrTest;

    ::PolymorphicRead(pareaTest, strmTestIn);
    if (!pareaTest->bIsA(TArea::clsThis()))
        strmOut << CUR_LN << L"Polymorphic read of TArea failed." << kCIDLib::EndLn;
    if (*pareaTest != TArea(10, 10, 100, 100))
    {
        strmOut << CUR_LN
                << L"Polymorphic read of TArea had wrong contents" << kCIDLib::EndLn;
    }
    delete pareaTest;

    ::PolymorphicRead(ptmTest, strmTestIn);
    if (!ptmTest->bIsA(TTime::clsThis()))
        strmOut << CUR_LN << L"Polymorphic read of TTime failed." << kCIDLib::EndLn;
    if (*ptmTest != tmTest)
    {
        strmOut << CUR_LN
                << L"Polymorphic read of TDate had wrong contents" << kCIDLib::EndLn;
    }

    delete ptmTest;
}


static
tCIDLib::TVoid TestComponents(          TTextOutStream&     strmOut
                                , const TPathStr&           pathTst
                                , const tCIDLib::TCh* const pszVolume
                                , const tCIDLib::TCh* const pszPath
                                , const tCIDLib::TCh* const pszName
                                , const tCIDLib::TCh* const pszExt)
{
    TString  strTmp(kCIDLib::pszEmptyZStr, kCIDLib::c4MaxPathLen);

    if (pszVolume)
    {
        if (!pathTst.bQueryVolume(strTmp))
            strmOut << CUR_LN << L"Volume was not found" << kCIDLib::EndLn;

        if (strTmp != pszVolume)
        {
            strmOut << CUR_LN << L"Volume did not match" << kCIDLib::EndLn;
            return;
        }
    }
     else
    {
        if (pathTst.bQueryVolume(strTmp))
            strmOut << CUR_LN << L"Bogus find of volume" << kCIDLib::EndLn;
    }

    if (pszPath)
    {
        if (!pathTst.bQueryPath(strTmp))
            strmOut << CUR_LN << L"Path was not found" << kCIDLib::EndLn;

        if (strTmp != pszPath)
        {
            strmOut << CUR_LN << L"Path did not match" << kCIDLib::EndLn;
            return;
        }
    }
     else
    {
        if (pathTst.bQueryPath(strTmp))
            strmOut << CUR_LN << L"Bogus find of path" << kCIDLib::EndLn;
    }

    if (pszName)
    {
        if (!pathTst.bQueryName(strTmp))
            strmOut << CUR_LN << L"Name was not found" << kCIDLib::EndLn;

        if (strTmp != pszName)
        {
            strmOut << CUR_LN << L"Name did not match" << kCIDLib::EndLn;
            return;
        }
    }
     else
    {
        if (pathTst.bQueryName(strTmp))
            strmOut << CUR_LN << L"Bogus find of name" << kCIDLib::EndLn;
    }

    if (pszExt)
    {
        if (!pathTst.bQueryExt(strTmp))
            strmOut << CUR_LN << L"Extension was not found" << kCIDLib::EndLn;

        if (strTmp != pszExt)
        {
            strmOut << CUR_LN << L"Extension did not match" << kCIDLib::EndLn;
            return;
        }
    }
     else
    {
        if (pathTst.bQueryExt(strTmp))
            strmOut << CUR_LN << L"Bogus find of extension" << kCIDLib::EndLn;
    }
}


static tCIDLib::TVoid TestDirectories(TTextOutStream& strmOut)
{
    //
    //  Try to create a directory underneath this directory. Start by
    //  querying the current directory and adding a subdirectory to it.
    //
    TPathStr    pathDir;
    TFileSys::QueryCurrentDir(pathDir);
    pathDir.AddLevel(L"Testing");

    //
    //  If the path exists, then delete it, so that we can create it again
    //  for the test.
    //
    if (TFileSys::bExists(pathDir))
        TFileSys::RemoveDirectory(pathDir);

    // Try to make this directory
    TFileSys::MakeDirectory(pathDir);

    // Now see if it exists
    if (!TFileSys::bIsDirectory(pathDir))
    {
        strmOut << CUR_LN << L"Just created directory, but couldn't find it"
                << kCIDLib::EndLn;
        return;
    }

    //
    //  Now delete the path and test to make sure that it does not exist
    //  any more.
    //
    TFileSys::RemoveDirectory(pathDir);

    if (TFileSys::bExists(pathDir))
    {
        strmOut << CUR_LN << L"Just deleted directory, buts its still there"
                << kCIDLib::EndLn;
        return;
    }
}


static tCIDLib::TVoid TestPathStr(TTextOutStream& strmOut)
{
    TPathStr  pathTst;
    TestComponents
    (
        strmOut
        , L"*.*"
        , nullptr
        , nullptr
        , L"*"
        , L"*"
    );

    TestComponents
    (
        strmOut
        , L"C:\\ABCD\\EFG\\123.456"
        , L"C:\\"
        , L"\\ABCD\\EFG\\"
        , L"123"
        , L"456"
    );

    TestComponents
    (
        strmOut
        , L"\\."
        , nullptr
        , L"\\"
        , L"."
        , nullptr
    );

    TestComponents
    (
        strmOut
        , L"\\*."
        , nullptr
        , L"\\"
        , L"*"
        , nullptr
    );

    TestComponents
    (
        strmOut
        , L"\\Test\\.name.ext"
        , nullptr
        , L"\\Test\\"
        , L".name"
        , L"ext"
    );

    TestComponents
    (
        strmOut
        , L"\\Test\\.name"
        , nullptr
        , L"\\Test\\"
        , L".name"
        , nullptr
    );

    TestComponents
    (
        strmOut
        , L"\\Test\\.name."
        , nullptr
        , L"\\Test\\"
        , L".name"
        , nullptr
    );

    TestComponents
    (
        strmOut
        , L"\\."
        , nullptr
        , L"\\"
        , L"."
        , nullptr
    );

    TestComponents
    (
        strmOut
        , L"."
        , nullptr
        , nullptr
        , L"."
        , nullptr
    );

    // These are only valid in the Win32 world
    #if defined(WIN32)
    TestComponents
    (
        strmOut
        , L"\\\\Wizard\\Develop\\QS2_Dev\\Result\\*.Dll"
        , L"\\\\Wizard\\Develop\\"
        , L"\\QS2_Dev\\Result\\"
        , L"*"
        , L"Dll"
    );
    #endif


    // Test the add to base path functionality
    struct TTestItem
    {
        const tCIDLib::TCh*     pszBase;
        const tCIDLib::TCh*     pszToAdd;
        const tCIDLib::TCh*     pszExpected;
    };

    TTestItem aItems[] =
    {
        { L"C:\\"           , L"Test1"              , L"C:\\Test1" }
      , { L"C:\\"           , L".\\Test1"           , L"C:\\Test1" }
      , { L"C:\\"           , L"..\\Test1"          , L"C:\\Test1" }
      , { L"C:\\Test1"      , L"..\\Test2"          , L"C:\\Test2" }
      , { L"C:\\Test1"      , L"\\Test2"            , L"C:\\Test2" }
    };
    const tCIDLib::TCard4 c4TestCount = tCIDLib::c4ArrayElems(aItems);

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TestCount; c4Index++)
    {
        TPathStr pathToAdd(aItems[c4Index].pszToAdd);
        TString  strBase(aItems[c4Index].pszBase);
        pathToAdd.AddToBasePath(strBase);

        if (pathToAdd != aItems[c4Index].pszExpected)
        {
            strmOut << CUR_LN << L"Add to base failed text #"
                    << c4Index << kCIDLib::EndLn;
        }
    }

    // Do a few other special case/regression type tests
    TPathStr pathUNC = L"\\\\Server\\Share\\File.Txt";
    pathUNC.bRemoveNameExt();
    pathUNC.AddLevel(L"NewFile.Txt");
    if (pathUNC != L"\\\\Server\\Share\\NewFile.Txt")
    {
        strmOut << CUR_LN << L"UNC path change failed" << kCIDLib::EndLn;
    }
}


static tCIDLib::TVoid TestDirTree(TTextOutStream& strmOut)
{
    // Get the buidl results directory as something we know exists
    TPathStr pathStartDir;
    if (!TProcEnvironment::bFind(L"CID_RESDIR", pathStartDir))
    {
        strmOut << CUR_LN << L"Could not find result directory in env" << kCIDLib::EndLn;
        return;
    }

    // Go to the include directory under that
    pathStartDir.AddLevel(L"Include");

    // And find that
    TFindBuf fndbTest;
    if (!TFileSys::bExists(pathStartDir, fndbTest))
    {
        strmOut << CUR_LN << L"Include directory doesn't exist" << kCIDLib::EndLn;
        return;
    }

    // And load the files under that
    TFileSys::c4BuildFileTree(L"*.hpp", fndbTest, tCIDLib::EDirSearchFlags::NormalFiles);

    // And let's iterate through the


    // Test copying with the tree built under it
    TFindBuf fndbCopy(fndbTest);

    if (fndbCopy != fndbTest)
    {
        strmOut << CUR_LN << L"Copy of find buffer tree != to original" << kCIDLib::EndLn;
        return;
    }
}


// -----------------------------------------------------------------------------
//  TFacTestCIDLib: Public, non-virtual methods
// -----------------------------------------------------------------------------

//
//  This method calls a number of local functions that test various
//  aspects of the file and file system classes.
//
tCIDLib::TVoid TFacTestCIDLib::TestFiles()
{
    const tCIDLib::TCh* pszCurTest = L"None";
    try
    {
        pszCurTest = L"path string";
        TestPathStr(strmOut());

        pszCurTest = L"basic file system";
        TestBasicFileSystem(strmOut());

        pszCurTest = L"binary file";
        TestBinaryFiles(strmOut());

        pszCurTest = L"directories";
        TestDirectories(strmOut());

        pszCurTest = L"binary stream";
        TestBinStreamIO(strmOut());

        pszCurTest = L"directory tree";
        TestDirTree(strmOut());

        // Clean up the temporary file
        pszCurTest = L"Temp file cleanup";
        if (TFileSys::bExists(strBinFileName))
            TFileSys::DeleteFile(strBinFileName);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        strmOut()   << L"Exception occured during the " << pszCurTest
                    << L" test" << kCIDLib::EndLn;
        throw;
    }
}
