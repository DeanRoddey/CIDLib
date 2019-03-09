//
// FILE NAME: FileSys2.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/24/1997
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
//  This is the main module for the second of the file system oriented demo
//  programs. This one queries TVolumeInfo objects for all of the available
//  volumes. It then prints out a nice report on all of them. It demonstrates
//  the use of collections, since we query a collection of volume info objects,
//  collection iteration, and formatted text stream output.
//
//  Note that this program will only get info for those volumes that are
//  ready. Floppies without disks or CD-ROMs without a CD in them or unsuppored
//  file system partitions are reported but no information can be gotten about
//  them.
//
// CAVEATS/GOTCHAS:
//
//  1)  This program is so simple that it does not create a facility object
//      for itself, or have a main facility header.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ----------------------------------------------------------------------------
//  Local static data
//
//  conOut
//      This is a console object which we use in this program for our standard
//      output. Its a specialized text stream class.
// ----------------------------------------------------------------------------
static TOutConsole  conOut;


// ----------------------------------------------------------------------------
//  Do the magic main module code to start the main thread
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"FileSys2MainThread", eMainThreadFunc))


// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------

//
//  We need a key extraction method for TVolumeInfo and TVolFailureInfo
//  objects.
//
static const TString& strVolInfoKey(const TVolumeInfo& voliSrc)
{
    return voliSrc.strVolume();
}

static const TString& strVolFailureKey(const TVolFailureInfo& volfiSrc)
{
    return volfiSrc.strVolume();
}



// This is the the thread function for the main thread
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
        //
        //  We need a collection of TVolumeInfo objects to fill in with the
        //  available volumes. And, we need a collection for the ones that
        //  fail. Not all volumes will be available. They can be not ready
        //  or an unsupported file system type.
        //
        //  Because of tricks we'll do below, we use a hash table because
        //  we'll need to do a quick search by volume name.
        //
        using TVolInfoCol = TKeyedHashSet<TVolumeInfo, TString, TStringKeyOps>;
        using TVolFailureCol = TKeyedHashSet<TVolFailureInfo, TString, TStringKeyOps>;

        TVolInfoCol colVols(29, new TStringKeyOps, strVolInfoKey);
        TVolFailureCol  colFails(29, new TStringKeyOps, strVolFailureKey);

        // Ask the file system class for a list of all of the  available volumes.
        if (!TFileSys::c4QueryAvailableVolumes(colVols, colFails, kCIDLib::True, kCIDLib::False))
        {
            conOut << kCIDLib::NewLn << L"Could not find any volumes to report on"
                     << kCIDLib::NewLn << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::Normal;
        }

        // Indicate what we are displaying
        conOut << L"\nInformation on available volumes:" << kCIDLib::NewEndLn;

        //
        //  We need to create stream format objects for each of the columns
        //  that we want to deal with.
        //
        //  The columns are:
        //
        //  1) Volume path. A string, left justified in a width of 22
        //  2) Volume label. A string. Left justified in a width of 14
        //  3) File system type. A string. Left justified in a width of 8
        //  4) MBs used. Float, right justified
        //  5) Total MBs. Right justified
        //  6) Max path length. Numeric, right justified in a width of 8
        //
        TStreamFmt strmfPath(22, 0, tCIDLib::EHJustify::Left, kCIDLib::chSpace);
        TStreamFmt strmfLabel(14, 0, tCIDLib::EHJustify::Left, kCIDLib::chSpace);
        TStreamFmt strmfType(8, 0, tCIDLib::EHJustify::Left, kCIDLib::chSpace);
        TStreamFmt strmfUsed(12, 0, tCIDLib::EHJustify::Right, kCIDLib::chSpace);
        TStreamFmt strmfTotal(12, 0, tCIDLib::EHJustify::Right, kCIDLib::chSpace);
        TStreamFmt strmfPathLen(8, 0, tCIDLib::EHJustify::Right, kCIDLib::chSpace);

        //
        //  Output the headers for the columns with dashed underlines under
        //  each one.
        //
        //  The RepChars() mechanism does not honor the formatting system. It
        //  just puts out the requested number of the passed character. The
        //  ESpaces() scheme, which is built on the other mechanism, is the
        //  same. If we just dumped "  ", it would be formatted and justified
        //  and turn into either 32 or 8 spaces according to which of the
        //  formatting schemes was on the stream at the time.
        //
        conOut  << strmfPath << L"Volume" << TTextOutStream::Spaces(2)
                << strmfLabel << L"Label" << TTextOutStream::Spaces(2)
                << strmfType << L"Type" << TTextOutStream::Spaces(2)
                << strmfUsed << L"KB Avail" << TTextOutStream::Spaces(2)
                << strmfTotal << L"KB Total" << TTextOutStream::Spaces(2)
                << strmfPathLen << L"Max Path" << TTextOutStream::Spaces(2)
                << kCIDLib::NewLn

                << TTextOutStream::RepChars(L'-', 22) << TTextOutStream::Spaces(2)
                << TTextOutStream::RepChars(L'-', 14) << TTextOutStream::Spaces(2)
                << TTextOutStream::RepChars(L'-', 8) << TTextOutStream::Spaces(2)
                << TTextOutStream::RepChars(L'-', 12) << TTextOutStream::Spaces(2)
                << TTextOutStream::RepChars(L'-', 12) << TTextOutStream::Spaces(2)
                << TTextOutStream::RepChars(L'-', 8) << TTextOutStream::Spaces(2)
                << kCIDLib::EndLn;

        //
        //  We want to display the volumes in alphabetical order. This includes
        //  the ones that failed. So we have to put the names of the volumes
        //  and failed volumes into a sort bag. Then we'll iterate it and
        //  find the names in one or other of the original collections. It needs a
        //  comparitor, which the string class provides.
        //
        TSortedBag<TString> colNames(TString::eComp);

        //
        //  Create a cursor for both our source collections and add them to
        //  the sorted bag.
        //
        TVolInfoCol::TCursor cursVolumes(&colVols);
        for (; cursVolumes; ++cursVolumes)
            colNames.objAdd(cursVolumes->strVolume());

        TVolFailureCol::TCursor cursFailures(&colFails);
        for (; cursFailures; ++cursFailures)
            colNames.objAdd(cursFailures->strVolume());

        //
        //  Now, we can iterate the sorted bag. For each name we get out, we
        //  look for in the original collections and print out the info.
        //
        TSortedBag<TString>::TCursor cursNames(&colNames);
        for (; cursNames; ++cursNames)
        {
            if (colVols.bKeyExists(*cursNames))
            {
                // Get a quicker, shorter name to the current volume object
                const TVolumeInfo& volCur = colVols.objFindByKey(*cursNames);

                // Query the space info
                tCIDLib::TCard8 c8TotalBytes = 0;
                tCIDLib::TCard8 c8VolFreeBytes = 0;
                tCIDLib::TCard8 c8UserAvailBytes = 0;
                TFileSys::QueryVolumeInfo
                (
                    volCur.strVolume(), c8TotalBytes, c8VolFreeBytes, c8UserAvailBytes
                );

                //
                //  Output the info for this volume. For each string we output
                //  the stream format object for that field first. This makes
                //  the formatting come out correctly.
                //
                conOut << strmfPath << volCur.strVolume()
                         << TTextOutStream::Spaces(2)
                         << strmfLabel << volCur.strVolumeLabel()
                         << TTextOutStream::Spaces(2)
                         << strmfType << volCur.strFileSysType()
                         << TTextOutStream::Spaces(2)
                         << strmfUsed
                         << TLocCardinal64(c8VolFreeBytes / 1024)
                         << TTextOutStream::Spaces(2)
                         << strmfTotal
                         << TLocCardinal64(c8TotalBytes / 1024)
                         << TTextOutStream::Spaces(2)
                         << strmfPathLen << volCur.c4MaxPathLenComp()
                         << TTextOutStream::Spaces(2)
                         << kCIDLib::EndLn;
            }
             else
            {
                const TVolFailureInfo& volfiCur = colFails.objFindByKey(*cursNames);

                conOut.SetDefaultFormat();
                conOut << volfiCur.strVolume() << L" - ";
                if (volfiCur.errcReason() == kKrnlErrs::errcFl_UnsupportedFileSystem)
                    conOut << L"[Unknown type]" << kCIDLib::EndLn;
                else if (volfiCur.errcReason() == kKrnlErrs::errcGen_NotReady)
                    conOut << L"[Not ready]" << kCIDLib::EndLn;
                else
                    conOut << L"[Unknown State]" << kCIDLib::EndLn;
            }
        }
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
        conOut << L"A CIDLib runtime error occured during processing. "
                 << L"Error: " << errToCatch.strErrText() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but I test
    //  for them in my demo programs so I can catch them if they do
    //  and fix them.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        conOut << L"A kernel error occured during processing. Error="
                 << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        conOut << L"A general exception occured during processing"
                 << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }

    return tCIDLib::EExitCodes::Normal;
}

