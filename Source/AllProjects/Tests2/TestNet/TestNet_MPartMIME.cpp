//
// FILE NAME: TestNet_MPartMIME.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/01/2017
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
//  This file does some multi-part MIME encoding/decoding oriented tests
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Include underlying headers
// ---------------------------------------------------------------------------
#include    "TestNet.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_MPMIMEDecode1,TTestFWTest)




// ---------------------------------------------------------------------------
//  CLASS: TTest_MPMIMEDecode1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_MPMIMEDecode1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_MPMIMEDecode1::TTest_MPMIMEDecode1() :

    TTestFWTest
    (
        L"MultiPart MIME Decode 1", L"Multi-part MIME decoding tests 1", 4
    )
{
}

TTest_MPMIMEDecode1::~TTest_MPMIMEDecode1()
{
}


// ---------------------------------------------------------------------------
//  TTest_MPMIMEDecode1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_MPMIMEDecode1::eRunTest(  TTextStringOutStream&   strmOut
                                , tCIDLib::TBoolean&    bWarning)
{
    // Build up a message that we can then decode
    TString strBoundary(L"FJAKF9388339");
    tCIDLib::TCard4 c4SrcBytes;
    THeapBuf mbufTest(32 * 1024);
    {
        TTextMBufOutStream strmTar
        (
            &mbufTest, tCIDLib::EAdoptOpts::NoAdopt, new TUSASCIIConverter
        );

        // Make sure we use the MIME standard line end
        strmTar.eNewLineType(tCIDLib::ENewLineTypes::CRLF);

        strmTar << L"This is a multipart MIME msg" << kCIDLib::DNewLn

                << L"--" << strBoundary << kCIDLib::NewLn
                << L"Content-Type: text/plain; charset=US-ASCII" << kCIDLib::DNewLn

                << L"This is the first part text" << kCIDLib::DNewLn

                << L"--" << strBoundary << kCIDLib::NewLn
                << L"Content-Type: application/json" << kCIDLib::NewLn
                << L"Content-Disposition: attachment; filename=\"test.txt\"" << kCIDLib::NewLn
                << L"Content-Transfer-Encoding: base64" << kCIDLib::DNewLn

                << L"This is the second part text" << kCIDLib::DNewLn
                << L"--" << strBoundary << L"--" << kCIDLib::NewLn;

        strmTar.Flush();
        c4SrcBytes = strmTar.c4CurSize();
    }

    TVector<THeapBuf>   colParts;
    tCIDLib::TCardList  fcolSizes;
    tCIDLib::TStrList   colContTypes;
    tCIDLib::TStrList   colDispositions;
    tCIDLib::TStrList   colTransEncodings;

    facCIDNet().ParseMultiPartMIME
    (
        mbufTest
        , c4SrcBytes
        , TString::strConcat(L"multipart/mixed; boundary=", strBoundary)
        , colParts
        , fcolSizes
        , colContTypes
        , colDispositions
        , colTransEncodings
    );

    // We should have two parts
    const tCIDLib::TCard4 c4PartCnt = colParts.c4ElemCount();
    if (c4PartCnt != 2)
    {
        strmOut << L"Expected 2 MIME parts but got " << c4PartCnt << L"\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // All the lists should be the same size
    if ((fcolSizes.c4ElemCount() != 2)
    ||  (colContTypes.c4ElemCount() != 2)
    ||  (colDispositions.c4ElemCount() != 2)
    ||  (colTransEncodings.c4ElemCount() != 2))
    {
        strmOut << L"All part lists should be the same size\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // Check that the content types came back correctly
    if (colContTypes[0] != L"text/plain; charset=US-ASCII")
    {
        strmOut << L"Part 1 type was wrong: " << colContTypes[0] << L"\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if (colContTypes[1] != L"application/json")
    {
        strmOut << L"Part 1 type was wrong: " << colContTypes[0] << L"\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    // The second should have disposition and transfer encoding, the first neither
    if (!colDispositions[0].bIsEmpty()
    ||  !colTransEncodings[0].bIsEmpty())
    {
        strmOut << L"Part 1 should have no disp or trans encoding\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if (colDispositions[1] != L"attachment; filename=\"test.txt\"")
    {
        strmOut << L"Part 1 disposition was wrong\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    if (colTransEncodings[1] != L"base64")
    {
        strmOut << L"Part 1 trans encoding was wrong\n\n";
        return tTestFWLib::ETestRes::Failed;
    }

    return tTestFWLib::ETestRes::Success;
}

