//
// FILE NAME: MetaExtract.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/24/2007
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
//  This is the main module for a simple program that will do a metadata
//  extraction on a media file using the CIDMetaExtr facility. You give it
//  a file and it extracts the metadata and displays it.
//
//  Like most of these basic samples, it doesn't create facility object, it
//  just starts up a thread on a local function.
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
#include    "CIDMetaExtr.hpp"


// ---------------------------------------------------------------------------
//  Do the magic main module code to start up the main thread
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"MetaExtrMainThread", eMainThreadFunc))



// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
TOutConsole conOut;


// ---------------------------------------------------------------------------
//  Local helper methods
// ---------------------------------------------------------------------------

//
//  Shows the parameter usage for the program.
//
static tCIDLib::TVoid ShowUsage()
{
    conOut  << kCIDLib::NewLn
            << L"Usage: MetaExtract srcfile" << kCIDLib::DNewLn
            << L"       srcfile is the path to the file to extract from"
            << kCIDLib::NewEndLn;
}


// ---------------------------------------------------------------------------
//  Program entry point
// ---------------------------------------------------------------------------

tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    // Display the program blurb
    conOut  << kCIDLib::NewLn
            << L"MetaExtract.Exe, Version: "
            << facCIDLib().strVersion() << kCIDLib::NewLn
            << L"CIDLib Media Metadata Extractor Sample"
            << kCIDLib::NewEndLn;


    //
    //  Since this is a demo and testing program, we'd like to catch
    //  all exceptions cleanly and report them. So put the whole thing
    //  in a try.
    //
    try
    {
        // Check out the command line parms for valid count
        if (TSysInfo::c4CmdLineParmCount() != 1)
        {
            ShowUsage();
            return tCIDLib::EExitCodes::BadParameters;
        }

        tCIDLib::TCard4     c4ArtSize = 0;
        tCIDLib::TCard4     c4BitRate = 0;
        tCIDLib::TCard4     c4Duration = 0;
        tCIDLib::TCard4     c4Rating = 1;
        tCIDLib::TCard4     c4TrackNum = 0;
        tCIDLib::TCard4     c4Year = 0;
        THeapBuf            mbufArt(kCIDLib::c4Sz_64K, kCIDLib::c4Sz_16M);
        TString             strAlbum;
        TString             strAlbumArtist;
        TString             strArtist;
        TString             strDescr;
        TString             strGenre;
        TString             strTrack;

        // Look at the extension and figure out which extractor to use
        TPathStr pathSrcFile = TSysInfo::strCmdLineParmAt(0);
        TString strExt;
        if (!pathSrcFile.bQueryExt(strExt))
        {
            conOut  << L"The file has no extension to determine its type"
                    << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::Normal;
        }

        TCIDMetaExtractor* pmetexToUse = nullptr;
        strExt.ToUpper();
        if ((strExt == L"MP4") || (strExt == L"M4A"))
            pmetexToUse = new TCIDMetaExtrAAC;
        else if ((strExt == L"WMA") || (strExt == L"MP3"))
            pmetexToUse = new TCIDMetaExtrWMA;

        if (!pmetexToUse)
        {
            conOut  << L"This file is not supported for metadata extraction"
                    << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::Normal;
        }

        // We got an extractor so try it
        TJanitor<TCIDMetaExtractor> janExtract(pmetexToUse);
        pmetexToUse->bExtract
        (
            pathSrcFile
            , strGenre
            , strDescr
            , strAlbum
            , strAlbumArtist
            , strArtist
            , strTrack
            , c4TrackNum
            , c4Duration
            , c4Year
            , c4BitRate
            , c4Rating
            , kCIDLib::True
            , mbufArt
            , c4ArtSize
        );

        // Print out the info
        TStreamFmt strmfPrefix(16, 0, tCIDLib::EHJustify::Right, kCIDLib::chSpace);
        TStreamFmt strmfVal(52, 0, tCIDLib::EHJustify::Left, kCIDLib::chSpace);

        conOut  << strmfPrefix << L"Name"
                << TTextOutStream::Spaces(2)
                << strmfVal << L"Value" << kCIDLib::NewLn
                << strmfPrefix << TTextOutStream::RepChars(L'-', 14)
                << TTextOutStream::Spaces(2)
                << strmfVal << TTextOutStream::RepChars(L'-', 52) << kCIDLib::NewLn

                << strmfPrefix << L"Album:" << TTextOutStream::Spaces(2)
                << strmfVal << strAlbum << kCIDLib::NewLn

                << strmfPrefix << L"Genre:" << TTextOutStream::Spaces(2)
                << strmfVal << strGenre << kCIDLib::NewLn

                << strmfPrefix << L"Year:" << TTextOutStream::Spaces(2)
                << strmfVal << c4Year << kCIDLib::NewLn

                << strmfPrefix << L"Alb. Artist:" << TTextOutStream::Spaces(2)
                << strmfVal << strAlbumArtist << kCIDLib::NewLn

                << strmfPrefix << L"Trk. Artist:"
                << TTextOutStream::Spaces(2) << strmfVal << strArtist
                << kCIDLib::NewLn

                << strmfPrefix << L"Track:" << TTextOutStream::Spaces(2)
                << strmfVal << strTrack << kCIDLib::NewLn

                << strmfPrefix << L"Track #:" << TTextOutStream::Spaces(2)
                << strmfVal << c4TrackNum << kCIDLib::NewLn

                << strmfPrefix << L"Seconds:" << TTextOutStream::Spaces(2)
                << strmfVal << c4Duration << kCIDLib::NewLn

                << strmfPrefix << L"Bit Rate (b/s):" << TTextOutStream::Spaces(2)
                << strmfVal << c4BitRate << kCIDLib::NewLn

                << strmfPrefix << L"Rating (of 10):" << TTextOutStream::Spaces(2)
                << strmfVal << c4Rating << kCIDLib::NewLn

                << strmfPrefix << L"Art Size:"
                << TTextOutStream::Spaces(2) << strmfVal << c4ArtSize
                << kCIDLib::NewLn

                << strmfPrefix << L"Descr:" << TTextOutStream::Spaces(2)
                << strmfVal << strDescr << kCIDLib::NewLn

                << kCIDLib::NewEndLn;
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
        conOut <<  L"A CIDLib runtime error occured during processing. "
                 <<  L"Error: " << errToCatch.strErrText() << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but I test
    //  for them in my test programs so I can catch them if they do
    //  and fix them.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        conOut << L"A kernel error occured during processing.\n  Error="
                 << kerrToCatch.errcId() << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        conOut << L"A general exception occured during processing"
                 << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::SystemException;
    }
    return tCIDLib::EExitCodes::Normal;
}


