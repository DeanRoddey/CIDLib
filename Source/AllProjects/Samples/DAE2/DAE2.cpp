//
// FILE NAME: DAE2.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/20/2006
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
//  This is the main module for a program that demonstrates the decoding
//  aspects of the CIDDAE facility. It just takes a pointer to a WMA or
//  other compressed audio file format supported locally and decodes it
//  back to a WAV and spits it out.
//
// CAVEATS/GOTCHAS:
//
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes. Note that the DAE class is not much used so it is not normally
//  included.
// ---------------------------------------------------------------------------
#include    "CIDDAE.hpp"


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
CIDLib_MainModule(TThread(L"DAE2MainThread", eMainThreadFunc))


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
TOutConsole         conOut;
TInConsole          conIn;
tCIDLib::TBoolean   bBreak;



// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------


//
//  This is the the thread function for the main thread.
//
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    TString strTmp;

    // We have to let our calling thread go first
    thrThis.Sync();

    // We get a source file, and we need a chunk of memory for data reading
    TString     strSrc;
    THeapBuf    mbufChunk(16 * 1024UL);
    try
    {
        // Get the source file
        strSrc = strTmp = TSysInfo::strCmdLineParmAt(0);

        //
        //  Create a decoder. We assume WMA here (which can also handle
        //  other formats if codecs are installed.)
        //
        TCIDDAEWMADec daedTest;

        // Do the start phase. If this works, we should be ok
        daedTest.StartDecode(strSrc);

        // Ok, it worked, so let's create the output file
        TBinaryFile flTarget(L"Output.WAV");
        flTarget.Open
        (
            tCIDLib::EAccessModes::Excl_ReadWrite
            , tCIDLib::ECreateActs::CreateAlways
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::None
        );

        //
        //  Skip forward by the size of the wave header and the data chunk
        //  header. We'll come back later and write it out when we know how
        //  big the whole file is.
        //
        flTarget.c8OffsetFilePos(sizeof(tCIDMedia::TWaveHdr) + 8);

        //
        //  And loop until we get no samples back from the codec. For each
        //  chunk we get from the codec, we just write it to the WAV file
        //  as we go.
        //
        tCIDLib::TCard4 c4ThisTime;
        tCIDLib::TCard4 c4SoFar = 0;
        do
        {
            c4ThisTime = daedTest.c4LoadChunk(mbufChunk);
            if (c4ThisTime)
            {
                flTarget.c4WriteBuffer(mbufChunk, c4ThisTime);
                c4SoFar += c4ThisTime;
            }
        }   while (c4ThisTime);

        // Now we can fill in a WAV header and go back and write it in
        tCIDMedia::TWaveHdr Hdr = {0};
        TRawMem::CopyMemBuf(Hdr.szRiff, "RIFF", 4);
        Hdr.c4Len = c4SoFar + 44 - 8;
        TRawMem::CopyMemBuf(Hdr.szWavFmt, "WAVEfmt ", 8);
        Hdr.c4HdrLen = 16;
        Hdr.c2Format = 1;
        Hdr.c2NumChannels = 2;
        Hdr.c4SampleRate = 44100;
        Hdr.c4BytesPerSec = 44100 * 2 * 2;
        Hdr.c2BlockAlign = 4;
        Hdr.c2BitsPerSample = 16;
        flTarget.SetFilePos(0);
        flTarget.c4WriteBuffer(&Hdr, sizeof(tCIDMedia::TWaveHdr));

        // Write out the final data chunk info
        const tCIDLib::TCard4 c4Id = 0x61746164;
        flTarget.c4WriteBuffer(&c4Id, 4);
        flTarget.c4WriteBuffer(&c4SoFar, 4);

        flTarget.Close();
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
                <<  L"\nError: " << errToCatch
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

