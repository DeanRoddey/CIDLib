//
// FILE NAME: CDROM1.cpp
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
//  This is the main module for a program that demonstrates some of the low
//  level CDROM digital audio extraction support. It will dump out info
//  about a CD in a drive. It shows all the CD drives, and for any that
//  have a CD in them, it displays the table of contents info.
//
// CAVEATS/GOTCHAS:
//
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"

#include    "CIDJPEG.hpp"

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
CIDLib_MainModule(TThread(L"CDROM1MainThread", eMainThreadFunc))



// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------


//
//  This is the the thread function for the main thread.
//
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    TOutConsole strmOut;
    try
    {
        // We can ask for a list of device paths of CDROMs available
        const tCIDLib::TCard4 c4EnumBufSz = 2048;
        tCIDLib::TCh szBuf[c4EnumBufSz + 1];
        tCIDLib::TCard4 c4DevsFound;
        if (!TKrnlRemMedia::bEnumDrvs(szBuf, c4EnumBufSz, c4DevsFound))
        {
            strmOut << L"CDROM enum failed" << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::FatalError;
        }

        // Create an input text stream over it
        TTextStringInStream strmEnum(new TString(szBuf), tCIDLib::EAdoptOpts::Adopt);

        // And now go through all the objects and dump info on them
        TKrnlRemMediaDrv rmmdCur;

        TKrnlRemMedia::TCDTOCInfo TOCInfo;
        const tCIDLib::TCard4 c4BufSz = 20 * TKrnlRemMedia::c4CDRawSectorSz;
        THeapBuf mbufData(c4BufSz, c4BufSz);
        tCIDLib::TBoolean bRes;
        TString strCurDev;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4DevsFound; c4Index++)
        {
            // Read in the next line from the enumeration buffer
            strmEnum >> strCurDev;

            // Set up the CDROM object with this new device path
            if (!rmmdCur.bSetPath(strCurDev.pszBuffer()))
            {
                strmOut << L"   Could not set new device path\n" << kCIDLib::EndLn;
                continue;
            }

            // Open the drive. If we can't, then not that and continue
            if (!rmmdCur.bOpen())
            {
                strmOut << L"   Could not open drive\n" << kCIDLib::EndLn;
                continue;
            }

            strmOut << L"\nDRIVE: " << strCurDev << L", "
                    << rmmdCur.pszDriveId()
                    << kCIDLib::EndLn;

            // Do the media type query
            TKrnlRemMedia::EMediaTypes eMediaType;
            if (!rmmdCur.bMediaType(eMediaType)
            ||  (eMediaType == TKrnlRemMedia::EMediaTypes::None)
            ||  (eMediaType == TKrnlRemMedia::EMediaTypes::Unknown))
            {
                strmOut << L"Could not determine media type"
                        << kCIDLib::NewLn;
            }
             else
            {
                strmOut << L"  Media type is: "
                        << TKrnlRemMedia::apszMediaTypes[tCIDLib::c4EnumOrd(eMediaType)]
                        << kCIDLib::NewLn;
            }

            //
            //  Get TOC info based on the media type
            //
            if (TKrnlRemMedia::bIsCDType(eMediaType))
            {
                if (rmmdCur.bQueryCDTOC(TOCInfo))
                {
                    tCIDLib::TCard4 c4StartBlock;
                    tCIDLib::TCard4 c4BlockCnt;
                    tCIDLib::TCard4 c4Minutes;
                    tCIDLib::TCard4 c4Seconds;
                    tCIDLib::TCard4 c4TrackNum = TOCInfo.c1FirstTrack;
                    for (; c4TrackNum <= TOCInfo.c1LastTrack; c4TrackNum++)
                    {
                        bRes = TKrnlRemMedia::bExtractCDTrackInfo
                        (
                            TOCInfo
                            , c4TrackNum
                            , c4StartBlock
                            , c4BlockCnt
                            , c4Minutes
                            , c4Seconds
                        );

                        strmOut << L"    Track #" << c4TrackNum
                                << L", M:S=" << c4Minutes << L":" << c4Seconds
                                << L", Block Start/Cnt=" << c4StartBlock
                                << L"/" << c4BlockCnt << kCIDLib::EndLn;
                    }
                    strmOut << kCIDLib::EndLn;
                }
                 else
                {
                    strmOut << L"  (No TOC Available)\n";
                }
            }
             else if (TKrnlRemMedia::bIsDVDType(eMediaType))
            {
                // We don't have any TOC extraction stuff at this level for DVDs
            }
            strmOut << kCIDLib::EndLn;
        }
    }

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
