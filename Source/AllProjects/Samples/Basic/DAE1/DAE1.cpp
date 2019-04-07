//
// FILE NAME: DAE1.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/20/2006
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
//  This is the main module for a program that demonstrates a basic digital
//  audio extraction support. It will display the available CDROMs that
//  have media ready and let you choose one. It will let you choose codec
//  and format to rip and a target path for the output.
//
//  Then it will show you the track info and ask if you want to rip the CD. If
//  so, it will rip the CD tracks to that path.
//
//  Like most of the basic samples it doesn't do the formal facility object
//  thing, and just starts a local function as the main thread.
//
// CAVEATS/GOTCHAS:
//
//  1)  This uses the kernel level CDROM object (there's no higher level class
//      for low level CDROM control) is used here. So this is a little unusual
//      a CIDLib based program, because normally kernel level objects are not
//      used directly.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes. The DAE (digital audio extraction) facility brings in everything
//  we need.
// ---------------------------------------------------------------------------
#include    "CIDDAE.hpp"


// ---------------------------------------------------------------------------
//  Do the magic main module code
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"DAE1MainThread", eMainThreadFunc))


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
TOutConsole         conOut;
TInConsole          conIn;
tCIDLib::TBoolean   bBreak;



// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------

//  This is the the function used as the main thread.
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    try
    {
        // Request the available audio WMA codecs
        TVector<TCIDDAEWMAEnc::TCodecInfo> colCodecs;
        const tCIDLib::TCard4 c4CodecCnt = TCIDDAEWMAEnc::c4EnumAudioCodecs(colCodecs);
        if (!c4CodecCnt)
        {
            conOut << L"No audio codecs were found" << kCIDLib::NewEndLn;
            return tCIDLib::EExitCodes::Normal;
        }

        //
        //  Iterate and display the available audio codecs. We want to display an
        //  index, so we just use an indexed loop.
        //
        conOut << L"\nAvailable Audio Codecs:\n--------------------------\n";
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4CodecCnt; c4Index++)
        {
            // Not working formal stream formatting, just handle up to 2 digits
            if (c4Index < 9)
                conOut << L' ';
            conOut  << (c4Index + 1) << L". " << colCodecs[c4Index].m_strName << L"\n";
        }
        conOut << kCIDLib::EndLn;

        // Ask them to select a codec
        TString strInput;
        conOut << L"\nEnter a codec number: " << kCIDLib::FlushIt;
        conIn >> strInput;
        conOut << kCIDLib::EndLn;

        //
        //  Should be a number, 1 based at this point, so check it, and it must
        //  be a valid one
        //
        tCIDLib::TCard4 c4CodecInd;
        if (!strInput.bToCard4(c4CodecInd, tCIDLib::ERadices::Dec)
        ||  !c4CodecInd
        ||  (c4CodecInd > c4CodecCnt))
        {
            conOut  << L"   The codec number was not valid\n"
                    << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::Normal;
        }

        // Convert back to an index
        c4CodecInd--;

        // It's a good one, so enumerate the formats for this codec
        TVector<TCIDDAEWMAEnc::TCodecInfo> colFormats;
        const tCIDLib::TCard4 c4FmtCnt = TCIDDAEWMAEnc::c4EnumCodecFmts
        (
            colCodecs[c4CodecInd], colFormats
        );
        if (!c4FmtCnt)
        {
            conOut << L"No codecs formats were found" << kCIDLib::NewEndLn;
            return tCIDLib::EExitCodes::Normal;
        }


        conOut << L"\nAvailable Codec Formats:\n--------------------------\n";
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4FmtCnt; c4Index++)
        {
            // Again, a simple, manual 2 column formatting deal
            if (c4Index < 9)
                conOut << L' ';
            conOut  << (c4Index + 1)<< L". " << colFormats[c4Index].m_strName<< L"\n";
        }
        conOut << kCIDLib::EndLn;

        // Ask them to select a format
        conOut << L"\nEnter a format number: " << kCIDLib::FlushIt;
        conIn >> strInput;
        conOut << kCIDLib::EndLn;

        // Should be a number, so check it, and it must be a valid one

        tCIDLib::TCard4 c4FmtInd;
        if (!strInput.bToCard4(c4FmtInd, tCIDLib::ERadices::Dec)
        ||  !c4FmtInd
        ||  (c4FmtInd > c4FmtCnt))
        {
            conOut  << L"   The format number was not valid" << kCIDLib::NewEndLn;
            return tCIDLib::EExitCodes::Normal;
        }
        c4FmtInd--;

        //
        //  We can ask for a list of device paths of available media driver
        //  are available. We have to use some kernel stuff for this.
        //
        TKrnlLList<TKrnlString> kllsDevs;
        if (!TKrnlRemMedia::bEnumDrvs(kllsDevs))
        {
            conOut << L"Could not enumerate CDROMs" << kCIDLib::NewEndLn;
            return tCIDLib::EExitCodes::FatalError;
        }

        //
        //  We'll go through and check each one to see if it's ready and a CD and
        //  get the goods ones out of the kernel level list and into a collection
        //  of key/value pairs holding the drive path and the drive id.
        //
        TKrnlRemMediaDrv    rmmdCur;
        tCIDLib::TKVPList   colDrvs;
        {
            TKrnlString* pkstrCur;
            while (kllsDevs.bNext(pkstrCur))
            {
                // Try to open it. If we can't, then skip this one
                if (!rmmdCur.bSetPath(pkstrCur->pszValue()) || !rmmdCur.bOpen())
                    break;

                // If read, and it has a media type and that typs is CD, take it
                TKrnlRemMedia::EMediaTypes eType;
                if (rmmdCur.bCheckIsReady()
                &&  rmmdCur.bMediaType(eType)
                &&  TKrnlRemMedia::bIsCDType(eType))
                {
                    colDrvs.objAdd
                    (
                        TKeyValuePair(pkstrCur->pszValue(), rmmdCur.pszDriveId())
                    );
                }
                rmmdCur.Close();
            }
        }

        // If there weren't any, then say so
        if (colDrvs.bIsEmpty())
        {
            conOut << L"No media was available" << kCIDLib::NewEndLn;
            return tCIDLib::EExitCodes::Normal;
        }

        // Display them and let the user select one. Again, we need to display the index.
        conOut  << L"Available Media:\n----------------------\n";
        const tCIDLib::TCard4 c4DrvCount = colDrvs.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4DrvCount; c4Index++)
        {
            conOut  << tCIDLib::TCard4(c4Index + 1)
                    << L". " << colDrvs[c4Index].strKey() << L"  "
                    << colDrvs[c4Index].strValue()
                    << kCIDLib::NewLn;
        }
        conOut << kCIDLib::EndLn;

        conOut << L"\nEnter Number: " << kCIDLib::FlushIt;
        conIn >> strInput;
        conOut << kCIDLib::EndLn;

        // Should be a number, so check it, and it must be a valid one
        tCIDLib::TCard4 c4SelNum;
        if (!strInput.bToCard4(c4SelNum, tCIDLib::ERadices::Dec)
        ||  !c4SelNum
        ||  (c4SelNum > c4DrvCount))
        {
            conOut  << L"   The driver number was not valid" << kCIDLib::NewEndLn;
            return tCIDLib::EExitCodes::Normal;
        }

        // Make it a zero based index now
        c4SelNum--;

        // Ok, we can set up the dive object now with the final target drive
        if (!rmmdCur.bSetPath(colDrvs[c4SelNum].strKey().pszBuffer()))
        {
            conOut  << L"  The drive path could not be set" << kCIDLib::NewEndLn;
            return tCIDLib::EExitCodes::Normal;
        }

        // Try to open the drive
        if (!rmmdCur.bOpen())
        {
            conOut  << L"   Could not open the drive" << kCIDLib::NewEndLn;
            return tCIDLib::EExitCodes::Normal;
        }

        // Try to query the TOCs
        TKrnlRemMedia::TCDTOCInfo TOCInfo;
        if (!rmmdCur.bQueryCDTOC(TOCInfo))
        {
            conOut  << L"  The table of contents could not be queried" << kCIDLib::NewEndLn;
            return tCIDLib::EExitCodes::Normal;
        }

        // Display the available tracks
        conOut  << L"\nAvailable Tracks\n-------------------------\n"
                << kCIDLib::EndLn;
        tCIDLib::TCard4 c4StartBlock;
        tCIDLib::TCard4 c4BlockCnt;
        tCIDLib::TCard4 c4Minutes;
        tCIDLib::TCard4 c4Seconds;
        tCIDLib::TCard4 c4TrackNum = TOCInfo.c1FirstTrack;
        for (; c4TrackNum <= TOCInfo.c1LastTrack; c4TrackNum++)
        {
            const tCIDLib::TBoolean bRes = TKrnlRemMedia::bExtractCDTrackInfo
            (
                TOCInfo
                , c4TrackNum
                , c4StartBlock
                , c4BlockCnt
                , c4Minutes
                , c4Seconds
            );

            if (bRes)
            {
                if (c4TrackNum < 9)
                    conOut << L' ';

                conOut  << (c4TrackNum + 1UL)
                        << L". Length=" << c4Minutes
                        << L":" << c4Seconds
                        << L"  (Start="
                        << c4StartBlock << L", Count=" << c4BlockCnt
                        << L")\n";
            }
        }
        conOut.Flush();

        // Ask if they want to rip this disc
        conOut << L"\nRip this disc(Y/N)?: " << kCIDLib::FlushIt;
        conIn >> strInput;
        conOut << kCIDLib::EndLn;
        if (!strInput.bCompareI(L"Y"))
            return tCIDLib::EExitCodes::Normal;

        // Get a target directory
        TPathStr pathOut;
        conOut << L"\nEnter path to store tracks: " << kCIDLib::FlushIt;
        conIn >> pathOut;
        conOut << kCIDLib::EndLn;

        // Ok, let's create a WMA encoder and a ripper
        TCIDDAEWMAEnc   daeeOut;
        TCIDDAERipper   daerOut;

        // Ask if they want jitter correction
        conOut << L"\nJitter Correction(Y/N)?: " << kCIDLib::FlushIt;
        conIn >> strInput;
        conOut << kCIDLib::EndLn;
        if (!strInput.bCompareI(L"Y"))
            daerOut.eJitterOpt(tCIDDAE::EJitterOpts::Always);

        // Kick off the ripping process now
        const tCIDLib::TCard4 c4TrackCnt((TOCInfo.c1LastTrack - TOCInfo.c1FirstTrack) + 1);
        TPathStr pathTrack;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TrackCnt; c4Index++)
        {
            pathTrack = pathOut;
            pathTrack.AddLevel(L"Track ");
            pathTrack.AppendFormatted(c4Index + 1);
            pathTrack.AppendExt(L"wma");

            conOut << L"Ripping Track #" << (c4Index + 1UL) << L"\n     "
                   << pathTrack << kCIDLib::EndLn;

            daerOut.RipTrack
            (
                rmmdCur
                , colCodecs[c4CodecInd].m_strName
                , colFormats[c4FmtInd].m_strName
                , c4Index
                , daeeOut
                , pathTrack
                , bBreak
            );
        }

        // Display stats
        conOut  << L"\n\n         Stitches: " << daerOut.c4Stitches()
                << L"\n  Stitch Failures: " << daerOut.c4StitchErrs()
                << L"\n      Read Errors: " << daerOut.c4ReadErrs()
                << kCIDLib::EndLn;

        // Open the CD door
        if (!rmmdCur.bOpenDoor(kCIDLib::True))
            conOut << L"  \nCould not open CD door" << kCIDLib::EndLn;

        // We can close the CDROM object now
        rmmdCur.Close();
    }

    catch(TError& errToCatch)
    {
        // If this hasn't been logged already, then log it
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        conOut  <<  L"A CIDLib runtime error occured during processing. "
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
        conOut << L"A kernel error occured during processing.\n  Error="
               << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        conOut << L"A general exception occured during processing\n"
               << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }

    return tCIDLib::EExitCodes::Normal;
}

