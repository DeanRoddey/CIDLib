//
// FILE NAME: TestCIDZLib.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/20/2004
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
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
#if         !defined(CIDCIDZLIB_HPP)
#include    "CIDZLib.hpp"
#endif


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
    ,   tCIDLib::TVoid*     pData
);


// ---------------------------------------------------------------------------
//  Do the magic main module code
// ---------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"TestCIDZLib", eMainThreadFunc))



// ----------------------------------------------------------------------------
//  Local data
//
//  conOut
//      Our output console for showing info to the user
// ----------------------------------------------------------------------------
TTextOutStream& conOut = TSysInfo::strmOut();



tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    try
    {
        //
        //  Do a text file
        //
        {
            //
            //  Create a source buffer and read the text file contents into
            //  it.
            //
            THeapBuf mbufSrc(0x20000);
            TBinaryFile flSrc(L"Src.Txt");
            flSrc.Open
            (
                tCIDLib::EAccessModes::Excl_Read
                , tCIDLib::ECreateActs::OpenIfExists
                , tCIDLib::EFilePerms::Default
                , tCIDLib::EFileFlags::SequentialScan
            );

            const tCIDLib::TCard4 c4InBytes = flSrc.c4ReadBuffer
            (
                mbufSrc, 0x20000
            );


            //
            //  Create a compresed data buffer and temporarily put streams over
            //  the source and compressed (target) buffers, and od a compression
            //  pass.
            //
            tCIDLib::TCard4 c4CompSz;
            THeapBuf mbufComp(0x20000);
            {
                TBinMBufInStream strmIn(&mbufSrc, c4InBytes);
                TBinMBufOutStream strmOut(&mbufComp);
                TZLibCompressor zlibTest;
                c4CompSz = zlibTest.c4Compress(strmIn, strmOut);

    #if 0
                const TMemBuf& mbufRes = strmOut.mbufData();
                conOut  << L"const unsigned byte_count = " << c4CompSz << L";\n";
                conOut  << L"const unsigned char abData[" << c4CompSz << L"] = \n{\n    ";
                conOut  << TTextOutStream::Fill(kCIDLib::chDigit0);
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4CompSz; c4Index++)
                {
                    if (c4Index)
                    {
                        if (!(c4Index % 10))
                            conOut << kCIDLib::NewLn << L"  , ";
                        else
                            conOut << L", ";
                    }

                    conOut  << L"0x"
                            << TTextOutStream::Width(2)
                            << TTextOutStream::Justify(tCIDLib::EHJustify::Right)
                            << TCardinal(mbufRes[c4Index], tCIDLib::ERadices::Hex)
                            << TTextOutStream::Width(0)
                            << TTextOutStream::Justify(tCIDLib::EHJustify::Left);
                }
                conOut << L"\n};\n";
                conOut.Flush();
    #endif
            }

            //
            //  Now let's put an input stream around the compressed data, and
            //  create a new decompressed data buffer (don't reuse the original
            //  so that we can do a comparison.
            //
            tCIDLib::TCard4 c4DecompSz;
            THeapBuf mbufDecomp(0x20000);
            {
                TBinMBufInStream strmIn(&mbufComp, c4CompSz);
                TBinMBufOutStream strmOut(&mbufDecomp);
                TZLibCompressor zlibTest;
                c4DecompSz = zlibTest.c4Decompress(strmIn, strmOut);
            }


            TBinaryFile flTar(L"out.txt");
            flTar.Open
            (
                tCIDLib::EAccessModes::Excl_Write
                , tCIDLib::ECreateActs::CreateAlways
                , tCIDLib::EFilePerms::Default
                , tCIDLib::EFileFlags::SequentialScan
            );
            flTar.c4WriteBuffer(mbufDecomp, c4DecompSz);
        }


        //
        //  And do a binary file
        //
        {
            TBinaryFile flSrc(L"208.png");
            flSrc.Open
            (
                tCIDLib::EAccessModes::Excl_Read
                , tCIDLib::ECreateActs::OpenIfExists
                , tCIDLib::EFilePerms::Default
                , tCIDLib::EFileFlags::SequentialScan
            );

            const tCIDLib::TCard4 c4OrgSz(tCIDLib::TCard4(flSrc.c8CurSize()));
            THeapBuf mbufSrc(c4OrgSz, c4OrgSz);
            flSrc.c4ReadBuffer(mbufSrc, c4OrgSz);

            tCIDLib::TCard4 c4CompSz;
            THeapBuf mbufComp(c4OrgSz + 64, c4OrgSz + 64);
            {
                TBinMBufInStream strmIn(&mbufSrc, c4OrgSz);
                TBinMBufOutStream strmOut(&mbufComp);
                TZLibCompressor zlibTest;
                c4CompSz = zlibTest.c4Compress(strmIn, strmOut);
            }

            tCIDLib::TCard4 c4DecompSz;
            THeapBuf mbufDecomp(c4OrgSz, c4OrgSz);
            {
                TBinMBufInStream strmIn(&mbufComp, c4CompSz);
                TBinMBufOutStream strmOut(&mbufDecomp);
                TZLibCompressor zlibTest;
                c4DecompSz = zlibTest.c4Decompress(strmIn, strmOut);
            }

            TBinaryFile flTar(L"out.png");
            flTar.Open
            (
                tCIDLib::EAccessModes::Excl_Write
                , tCIDLib::ECreateActs::CreateAlways
                , tCIDLib::EFilePerms::Default
                , tCIDLib::EFileFlags::SequentialScan
            );

            flTar.c4WriteBuffer(mbufDecomp, c4DecompSz);
        }
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
        conOut  << L"A CIDLib runtime error occured during processing.\n"
                << L"Error: " << errToCatch.strErrText() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but test for
    //  them here just in case.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        conOut  << L"A kernel error occured during processing.\nError="
                << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        conOut  << L"A general exception occured during processing"
                << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }
    return tCIDLib::EExitCodes::Normal;
}


