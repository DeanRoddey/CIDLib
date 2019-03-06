//
// FILE NAME: MakePreviews.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/08/2005
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main module for this program. This one is very simple and
//  just has a single cpp file that contains everything. This program is
//  a utility that is used to generate preview versions of a directory of
//  images.
//
//  You point this guy at a directory and it will find all the image files
//  in formats we support. Any found will be read in, reduced to an indicated
//  horz size (maintaining AR), and spit back out in JPEG format (regardless
//  of input format) in a fairly low quality.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDJPEG.hpp"

#include    "CIDPNG.hpp"



// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
        , tCIDLib::TVoid*   pData
);


// ---------------------------------------------------------------------------
//  Do the magic main module code
// ---------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"CMakePreviwsMainThread", eMainThreadFunc))


// ---------------------------------------------------------------------------
//  Local data
//
//  imgXXX
//      Some image objects that we use to stream in/out the image data. We
//      don't want to create these large objects over and over so we make
//      them global.
// ---------------------------------------------------------------------------
TBitmapImage    imgBMP;
TJPEGImage      imgJPEG;
TPNGImage       imgPNG;



// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------

//
//  Shows the parameter usage for the program.
//
static tCIDLib::TVoid ShowUsage()
{
    TSysInfo::strmOut()
            << kCIDLib::NewLn
            << L"Usage: MakePreviews path [/MaxH=XX /Suffix=SSS]\n\n"
               L"       If an image is larger in the horizontal direction than\n"
               L"       the indicated /MaxH then the image is scaled down so that\n"
               L"       the offending horzizontal fits. The aspect ratio is retained.\n\n"
               L"       Default MaxH is 480\n\n"
               L"       If the input file is foo.png, the output file will be\n"
               L"       fooSSS.jpg, so the passed suffix is added to each\n"
               L"       generated preview file name. If no suffix is provided,\n"
               L"       then _Preview is used\n"
            << kCIDLib::EndLn;
}


static tCIDLib::TBoolean bParseParms(tCIDLib::TCard4&   c4MaxH
                                    , TString&          strStartPath
                                    , TString&          strSuffix)
{
    TString strCur;

    // Check out the command line parms for valid count
    tCIDLib::TCard4 c4Args = TSysInfo::c4CmdLineParmCount();
    if ((c4Args < 1) || (c4Args > 3))
        return kCIDLib::False;

    // The first one should be the path
    TSysInfo::TCmdLineCursor cursParms = TSysInfo::cursCmdLineParms();
    strCur = *cursParms++;
    TFileSys::QueryFullPath(strCur, strStartPath);
    if (!TFileSys::bIsDirectory(strStartPath))
    {
        TSysInfo::strmOut() << L"The starting path was not found\n\n"
                            << kCIDLib::FlushIt;
        return kCIDLib::False;
    }

    // If there are any more parms, check them out
    tCIDLib::TBoolean   bOk;
    for (; cursParms; ++cursParms)
    {
        strCur = *cursParms;

        if (strCur.bStartsWithI(L"/MaxH="))
        {
            strCur.Cut(0, 6);
            c4MaxH = TRawStr::c4AsBinary(strCur.pszBuffer(), bOk, tCIDLib::ERadices::Dec);
            if (!bOk || (c4MaxH < 64))
            {
                TSysInfo::strmOut() << L"MaxH must be >= 64\n\n"
                                    << kCIDLib::FlushIt;
                return kCIDLib::False;
            }
        }
         else if (strCur.bStartsWithI(L"/Suffix="))
        {
            strCur.Cut(0, 8);
            strSuffix = strCur;
        }
         else
        {
            TSysInfo::strmOut() << L"'" << strCur
                                << L"' is not a valid parameter\n\n"
                                << kCIDLib::FlushIt;
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}


static tCIDLib::TVoid DoAFile(  const   TString&        strPath
                                , const TString&        strExt
                                , const tCIDLib::TCard4 c4MaxH
                                , const TString&        strSuffix)
{
    try
    {
        tCIDLib::TCard4 c4OrgSz;

        // Open a binary input file stream over this file
        TCIDImage* pimgTar = 0;
        {
            TBinFileInStream strmSrc
            (
                strPath
                , tCIDLib::ECreateActs::OpenIfExists
                , tCIDLib::EFilePerms::Default
                , tCIDLib::EFileFlags::SequentialScan
            );
            c4OrgSz = tCIDLib::TCard4(strmSrc.c8CurSize());

            if (strExt == L"BMP")
            {
                strmSrc >> imgBMP;
                pimgTar = &imgBMP;
            }
            else if ((strExt == L"JPEG") || (strExt == L"JPG"))
            {
                strmSrc >> imgJPEG;
                pimgTar = &imgJPEG;
            }
            else if (strExt == L"PNG")
            {
                strmSrc >> imgPNG;
                pimgTar = &imgPNG;
            }
        }

        if (pimgTar && (pimgTar->c4Width() > c4MaxH))
        {
            // Scale the image to fit thw MaxH, keeping AR
            TSize szNew(pimgTar->szImage());
            szNew.ScaleToWidthAR(c4MaxH);
            pimgTar->Scale(szNew, 2);

            //
            //  Ok, now we can now stream it back to its new file. So we
            //  create the preview file name based on the inputname.
            //
            TPathStr pathNewPath(strPath);
            pathNewPath.bRemoveExt();
            pathNewPath.Append(strSuffix);

            //
            //  If it wasn't a JPEG file, then we need to convert it to a
            //  JPEG, since that's what we always write out.
            //
            if (!pimgTar->bIsDescendantOf(TJPEGImage::clsThis()))
            {
                imgJPEG = *pimgTar;
                pimgTar = &imgJPEG;
            }
            pathNewPath.AppendExt(L"jpg");

//            if (!pimgTar->bIsDescendantOf(TPNGImage::clsThis()))
//            {
//                imgPNG = *pimgTar;
//                pimgTar = &imgPNG;
//            }
//            pathNewPath.AppendExt(L"png");

            // Stream it out and remember the bytes we output
            tCIDLib::TCard4 c4OutBytes;
            {
                TBinFileOutStream strmOut
                (
                    pathNewPath
                    , tCIDLib::ECreateActs::CreateAlways
                    , tCIDLib::EFilePerms::Default
                    , tCIDLib::EFileFlags::SequentialScan
                );

                strmOut << *pimgTar << kCIDLib::FlushIt;
                c4OutBytes = strmOut.c4CurPos();
            }

            //
            //  Display the percent we reduced the file and the path of
            //  the file we just did. We cut off the leading part if it
            //  is more than 60 chars long, so that they don't wrap.
            //
            tCIDLib::TCard4 c4Per = 0;
            if (c4OutBytes < c4OrgSz)
            {
                c4Per = 100 - tCIDLib::TCard4
                (
                    tCIDLib::TFloat8(c4OutBytes) / tCIDLib::TFloat8(c4OrgSz)
                    * 100
                );
            }

            const tCIDLib::TCard4 c4OrgLen = strPath.c4Length();
            pathNewPath = strPath;
            pathNewPath.CapAt(c4OrgLen);
            if (c4OrgLen > 60)
            {
                pathNewPath.Cut(0, c4OrgLen - 60);
                pathNewPath.Prepend(L"...");
            }
            TSysInfo::strmOut() << L"(Reduced ";
            if (c4Per < 10)
                TSysInfo::strmOut() << L"0";
            TSysInfo::strmOut() << c4Per;
            TSysInfo::strmOut() << L"%) " << pathNewPath << kCIDLib::EndLn;
        }
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
        TSysInfo::strmOut() << L"Failed to convert file: " << strPath
                            << kCIDLib::EndLn;
    }
}


static tCIDLib::TVoid DoADir(const  TString&            strCurPath
                            , const tCIDLib::TCard4     c4MaxH
                            , const TString&            strSuffix)
{
    // If no files at this level, then we are done
    TDirIter diterLevel;
    TFindBuf fndbSearch;
    if (!diterLevel.bFindFirst( strCurPath
                                , kCIDLib::pszAllFilesSpec
                                , fndbSearch
                                , tCIDLib::EDirSearchFlags::All))
    {
        return;
    }

    TString strTmp;
    do
    {
        //
        //  If it's a file, then pull the extension and see if it's an image
        //  file type that we care about.
        //
        if (fndbSearch.bIsNormalFile())
        {
            const TPathStr& pathCur = fndbSearch.pathFileName();
            pathCur.bQueryExt(strTmp);
            strTmp.ToUpper();


            if ((strTmp == L"BMP")
            ||  (strTmp == L"JPEG")
            ||  (strTmp == L"JPG")
            ||  (strTmp == L"PNG"))
            {
                DoAFile(pathCur, strTmp, c4MaxH, strSuffix);
            }
        }
    }   while (diterLevel.bFindNext(fndbSearch));
}


//
//  This is the the thread function for the main thread.
//
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    //
    //  Since this is a demo and testing program, we'd like to catch
    //  all exceptions cleanly and report them. So put the whole thing
    //  in a try.
    //
    try
    {

        // Get the start path out and make sure it exists
        tCIDLib::TCard4     c4MaxH = 640;
        TString             strStartPath;
        TString             strSuffix(L"_Preview");

        if (!bParseParms(c4MaxH, strStartPath, strSuffix))
        {
            ShowUsage();
            return tCIDLib::EExitCodes::BadParameters;
        }

        // Set up the JPEG object for the output quality we want
        imgJPEG.c4CompQuality(85);
        imgJPEG.eOutSample(tCIDJPEG::EOutSamples::F4_2_0);
        DoADir(strStartPath, c4MaxH, strSuffix);
    }

    //
    //  Catch any unhandled CIDLib runtime errors
    //
    catch(TError& errToCatch)
    {
        // If this hasn't been logged already, then log it
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        TSysInfo::strmOut()
                    <<  L"A CIDLib runtime error occured during processing. "
                    <<  L"\nError: " << errToCatch.strErrText()
                    << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but I test
    //  for them in my test programs so I can catch them if they do
    //  and fix them.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        TSysInfo::strmOut()
                    << L"A kernel error occured during processing.\n  Error="
                    << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        TSysInfo::strmOut()
                    << L"A general exception occured during processing\n"
                    << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }

    return tCIDLib::EExitCodes::Normal;
}


