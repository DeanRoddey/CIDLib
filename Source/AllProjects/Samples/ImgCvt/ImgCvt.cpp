//
// FILE NAME: ImgCvt.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/23/2007
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
//  This is the main (only) module of the ImgCvt sample program. This program
//  takes a source and target directory. In the source it will find any PNG
//  files there and convert them to JPG files in the target directory, with
//  the same base file name (overwriting any existing files in the target.)
//  It just displays each file name as it is converts it.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes. This program is so simple that we don't even have a header of
//  our own. So just include some image facility headers, which bring in all
//  we need.
// ----------------------------------------------------------------------------
#include    "CIDJPEG.hpp"
#include    "CIDPNG.hpp"



// ----------------------------------------------------------------------------
//  Forward references
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
        , tCIDLib::TVoid*   pData
);


// ----------------------------------------------------------------------------
//  Local data
//
//  bToJPG
//      Set from the last cmd line parameter, to indicate the type of
//      conversion, either from JPEG to PNG or from PNG to JPEG conversion.
//
//  strmOut
//      We don't need any interactivity for this program so we just set up
//      an output text stream, which writes to the standard output. Use the
//      default text encoding for this platform.
//
//  strSrcFiles
//      Set from the first parameter, to indicate which type of files we are
//      searching for to convert.
// ----------------------------------------------------------------------------
static tCIDLib::TBoolean    bToJPG;
static TTextFileOutStream   strmOut(tCIDLib::EStdFiles::StdOut);
static TString              strSrcFiles;



// ----------------------------------------------------------------------------
//  Do the magic main module code
//
//  This tells CIDLib what the main thread of the program is. This is the
//  only thread object that is run automatically. All others are started
//  manually when required or desired.
// ----------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"ImgCvtMainThread", eMainThreadFunc))



// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    //
    //  Get the standard output stream. We don't have any interactive needs,
    //  so we can just use this standard one, so that we support redirection.
    //
    TTextOutStream& strmOut = TSysInfo::strmOut();


    //
    //  Since this is a demo and partly a testing program, we'd like to
    //  catch all exceptions cleanly and report them. So put the whole thing
    //  in a try.
    //
    try
    {
        // Output a little program blurb
        strmOut << L"\nImgCvt.Exe\n"
                << L"CIDLib Image Conversion Demo\n\n" << kCIDLib::EndLn;

        // We need three params so get those
        tCIDLib::TCard4 c4Args = TSysInfo::c4CmdLineParmCount();
        if (c4Args != 3)
        {
            strmOut << L"Usage:\n    ImgCvt srcdir tardir cvttype\n"
                       L"            cvttype = PNG2JPG or JPG2PNG\n\n"
                    << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::BadParameters;
        }

        TPathStr pathSrc = TSysInfo::strCmdLineParmAt(0);
        TPathStr pathTar = TSysInfo::strCmdLineParmAt(1);

        // Make sure the source exists
        if (!TFileSys::bExists(pathSrc))
        {
            strmOut << L"    The source path does not exist\n\n"
                    << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::BadParameters;
        }

        // If the target doesn't exist, then create it
        if (!TFileSys::bExists(pathTar))
        {
            try
            {
                TFileSys::MakeDirectory(pathTar);
            }

            catch(...)
            {
                strmOut << L"    The target path did not exist and could not be"
                           L" created.\n\n"
                        << kCIDLib::EndLn;
                return tCIDLib::EExitCodes::BadParameters;
            }
        }

        //
        //  Set up JPEG and PNG image objects. Set up the output quality
        //  on the jPEG in case we are going in that direction. We set a
        //  pretty high quality level.
        //
        TJPEGImage imgJPEG;
        imgJPEG.c4CompQuality(90);
        imgJPEG.eOutSample(tCIDJPEG::EOutSamples::F4_4_4);
        TPNGImage imgPNG;

        // Get the type of conversion to do
        TString strCvtType = TSysInfo::strCmdLineParmAt(2);
        if (strCvtType == L"PNG2JPG")
        {
            bToJPG = kCIDLib::True;
            strSrcFiles = L"*.png";
        }
         else if (strCvtType == L"JPG2PNG")
        {
            bToJPG = kCIDLib::False;
            strSrcFiles = L"*.jpg";
        }
         else
        {
            strmOut << L"    The conversion type was not recognized\n\n"
                    << kCIDLib::EndLn;
        }

        // And now do a search of the src dir for the source files
        TDirIter diterSrc;
        TFindBuf fndbCur;
        if (!diterSrc.bFindFirst(pathSrc, strSrcFiles, fndbCur))
        {
            strmOut << L"No matching files were found in the source directory\n\n"
                    << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::BadParameters;
        }

        TString  strImgName;
        do
        {
            // We create an input file stream and stream in the src image
            {
                TBinFileInStream strmSrc
                (
                    fndbCur.pathFileName()
                    , tCIDLib::ECreateActs::OpenIfExists
                    , tCIDLib::EFilePerms::Default
                    , tCIDLib::EFileFlags::SequentialScan
                );

                if (bToJPG)
                    strmSrc >> imgPNG;
                else
                    strmSrc >> imgJPEG;
            }

            // Assign it to the target image to copy over the data
            if (bToJPG)
                imgJPEG = imgPNG;
            else
                imgPNG = imgJPEG;

            //
            //  Now create an output file and stream out the target object.
            //  We have to build up the new file name.
            //
            {
                fndbCur.pathFileName().bExtractNameExt(strImgName);

                TPathStr pathOutImg = pathTar;
                pathOutImg.AddLevel(strImgName);
                pathOutImg.bRemoveExt();
                if (bToJPG)
                    pathOutImg.AppendExt(L"jpg");
                else
                    pathOutImg.AppendExt(L"png");
                strmOut << L"Generating: " << pathOutImg << kCIDLib::EndLn;

                TBinFileOutStream strmTar
                (
                    pathOutImg
                    , tCIDLib::ECreateActs::CreateAlways
                    , tCIDLib::EFilePerms::Default
                    , tCIDLib::EFileFlags::SequentialScan
                );

                if (bToJPG)
                   strmTar << imgJPEG << kCIDLib::FlushIt;
                else
                   strmTar << imgPNG << kCIDLib::FlushIt;
            }
        }   while (diterSrc.bFindNext(fndbCur));
    }

    // Catch any CIDLib runtime errors
    catch(TError& errToCatch)
    {
        // If this hasn't been logged already, then log it
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        strmOut << L"A CIDLib runtime error occured during processing.\nError:\n\n"
                << errToCatch << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but I test
    //  for them in my test programs so I can catch them if they do
    //  and fix them.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        strmOut << L"A kernel error occured during processing.\nError="
                << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        strmOut << L"A general exception occured during processing"
                << kCIDLib::DNewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }

    return tCIDLib::EExitCodes::Normal;
}

