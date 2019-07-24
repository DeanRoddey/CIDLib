//
// FILE NAME: CIDNet_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/05/2000
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
//  This file implements TFacCIDNet class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDNet_.hpp"


// ---------------------------------------------------------------------------
//  Local helpers
// ---------------------------------------------------------------------------

//
//  A helper for the multi-part MIME parsing method. This will pull out one line
//  of content from the source buffer.
//
//  This is only called if the current parsing state indicates that we are not inside
//  a part, which case it is appropriate to expect basic text content. If we are, then
//  we do a more elaborate scheme.
//
static tCIDLib::TBoolean
bGetString(         TString&            strToFill
            ,       tCIDLib::TCard4&    c4CurInd
            , const TMemBuf&            mbufSrc
            , const tCIDLib::TCard4     c4SrcBytes)
{
    if (c4CurInd == c4SrcBytes)
        return kCIDLib::False;

    // We just assume for now ASCII data
    strToFill.Clear();
    while (c4CurInd < c4SrcBytes)
    {
        const tCIDLib::TCard1 c1Cur = mbufSrc[c4CurInd++];

        // We should get CR/LF, but handle just CR or just LF
        if (c1Cur == 0x0D)
        {
            // There should be an LF but be careful
            if ((c4CurInd < c4SrcBytes) && (mbufSrc[c4CurInd] == 0x0A))
                c4CurInd++;
            break;
        }
         else if (c1Cur == 0x0A)
        {
            break;
        }
        strToFill.Append(tCIDLib::TCh(c1Cur));
    }

    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  TFacCIDNet: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDNet::TFacCIDNet() :

    TFacility
    (
        L"CIDNet"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}

TFacCIDNet::~TFacCIDNet()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDNet: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TFacCIDNet::ParseMultiPartMIME( const   TMemBuf&            mbufSrc
                                , const tCIDLib::TCard4     c4SrcBytes
                                , const TString&            strContType
                                ,       TVector<THeapBuf>&  colParts
                                ,       tCIDLib::TCardList& fcolSizes
                                ,       tCIDLib::TStrList&  colContTypes
                                ,       tCIDLib::TStrList&  colDispositions
                                ,       tCIDLib::TStrList&  colTransEncodings)
{
    // Make sure all of the return lists are empty
    colParts.RemoveAll();
    colContTypes.RemoveAll();
    colDispositions.RemoveAll();
    colTransEncodings.RemoveAll();
    fcolSizes.RemoveAll();

    // Make sure it is a multi-part which is the only scheme we understand
    TString strBoundary;
    if (!strContType.bStartsWithI(L"multipart/"))
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcMPMIME_NotMultiPart
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    // Check it for a boundary value and any others that we might care about
    {
        TStringTokenizer stokType(&strContType, L";");
        TString strName;
        TString strValue;
        while (stokType.bGetNextToken(strName))
        {
            // Split it on an equal sign
            strName.bSplit(strValue, kCIDLib::chEquals);

            // Be careful and remove leading/trailng white space
            strName.StripWhitespace();
            strValue.StripWhitespace();

            if (strName.bCompareI(L"boundary"))
            {
                strBoundary = strValue;
                break;
            }
        }
    }

    // Make sure we got the boundary
    if (strBoundary.bIsEmpty())
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcMPMIME_NoBoundary
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    // Build up the start/end versions of the boundary
    const TString strBStart(TStrCat(L"--", strBoundary));
    const TString strBEnd(TStrCat(strBStart, L"--"));

    //
    //  Create a simple binary version of the start and end boundary strings. We'll
    //  use this when we are in a part to test for the next boundary. There's no
    //  existing code to do this, since it's almost never needed, so we just copy
    //  over the bottom byte of each char.
    //
    const tCIDLib::TCard4 c4BStartLen = strBStart.c4Length();
    tCIDLib::TCard1* pac1BStart = new tCIDLib::TCard1[c4BStartLen];
    TArrayJanitor<tCIDLib::TCard1> janBStart(pac1BStart);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4BStartLen; c4Index++)
        pac1BStart[c4Index] = tCIDLib::TCard1(strBStart[c4Index]);

    const tCIDLib::TCard4 c4BEndLen = strBEnd.c4Length();
    tCIDLib::TCard1* pac1BEnd = new tCIDLib::TCard1[c4BEndLen];
    TArrayJanitor<tCIDLib::TCard1> janBEnd(pac1BEnd);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4BEndLen; c4Index++)
        pac1BEnd[c4Index] = tCIDLib::TCard1(strBEnd[c4Index]);

    //
    //  OK, we have to do this carefully because it has no real encoding, though we
    //  will assume that it's all 8 bit. It could be 7 bit, but we wouldn't really care
    //  for our purposes.
    //

    // Current tmp values until we can store them away
    tCIDLib::TCard4 c4PartBytes;
    THeapBuf mbufPartBuf(32 * 1024);
    TString strPartContType;
    TString strPartDisposition;
    TString strPartTransEncoding;

    // A simple state machine
    enum EStates
    {
        EState_FirstBoundary
        , EState_Boundary
        , EState_InHeaders
        , EState_InPart
        , EState_AtEnd
    };

    // Now we start in a part, waiting for the first boundary
    EStates eState = EState_FirstBoundary;
    tCIDLib::TCard4 c4SrcInd = 0;
    THeapBuf mbufCurLn(2048, 64 * 1024);
    tCIDLib::TCard4 c4CurBytes;
    TString strCurLn;
    TString strTmp;
    while ((c4SrcInd < c4SrcBytes) && (eState != EState_AtEnd))
    {
        if (eState == EState_FirstBoundary)
        {
            //
            //  Ignore lines till the first boundary. If we hit the end here
            //  We'll just assume empty content and return with all lists
            //  empty.
            //
            if (!bGetString(strCurLn, c4SrcInd, mbufSrc, c4SrcBytes))
            {
                eState = EState_AtEnd;
                break;
            }

            // If the start boundary, move to boundary state
            if (strCurLn.bCompareI(strBStart))
                eState = EState_Boundary;
        }
         else if (eState == EState_Boundary)
        {
            // This is just a way station state to reset for the next part
            c4PartBytes = 0;
            strPartContType.Clear();
            strPartDisposition.Clear();
            strPartTransEncoding.Clear();
            eState = EState_InHeaders;
        }
         else if (eState == EState_InHeaders)
        {
            // Shouldn't be empty, if so, break out with current state
            if (!bGetString(strCurLn, c4SrcInd, mbufSrc, c4SrcBytes))
                break;

            //
            //  If the new line separator, then we need to move forward. Ekse
            //  it has to be a header line, so break it apart and store if one
            //  we care about.
            //
            if (strCurLn.bIsEmpty())
            {
                eState = EState_InPart;
            }
             else
            {
                if (!strCurLn.bSplit(strTmp, kCIDLib::chColon))
                {
                    facCIDNet().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kNetErrs::errcMPMIME_BadHeader
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Format
                        , TCardinal(colParts.c4ElemCount() + 1)
                    );
                }

                strCurLn.StripWhitespace();
                strTmp.StripWhitespace();
                if (strCurLn.bCompareI(THTTPClient::strHdr_ContType))
                    strPartContType = strTmp;
                else if (strCurLn.bCompareI(THTTPClient::strHdr_ContDisposition))
                    strPartDisposition = strTmp;
                else if (strCurLn.bCompareI(THTTPClient::strHdr_ContTransferEncoding))
                    strPartTransEncoding = strTmp;
            }
        }
         else if (eState == EState_InPart)
        {
            c4CurBytes = 0;
            while (kCIDLib::True)
            {
                //
                //  Shouldn't happen in the middle of a part, so break out with
                //  current state.
                //
                if (c4SrcInd == c4SrcBytes)
                    break;

                // Let's process another line
                const tCIDLib::TCard1 c1Cur = mbufSrc[c4SrcInd++];
                if ((c1Cur == 0x0D) || (c1Cur == 0x0A))
                {
                    // If a CR, check for following LF
                    if (c1Cur == 0x0D)
                    {
                        if ((c4SrcInd < c4SrcBytes) && (mbufSrc[c4SrcInd] == 0x0A))
                            c4SrcInd++;
                    }

                    //
                    //  Let's see if it's the start or end boundary. Check the end
                    //  first.
                    //
                    if (c4CurBytes == c4BEndLen)
                    {
                        tCIDLib::TCard4 c4TestInd = 0;
                        while (c4TestInd < c4CurBytes)
                        {
                            if (mbufCurLn[c4TestInd] != pac1BEnd[c4TestInd])
                                break;
                            c4TestInd++;
                        }

                        // Force us to end if we matched
                        if (c4TestInd == c4CurBytes)
                            eState = EState_AtEnd;
                    }
                     else if (c4CurBytes == c4BStartLen)
                    {
                        tCIDLib::TCard4 c4TestInd = 0;
                        while (c4TestInd < c4CurBytes)
                        {
                            if (mbufCurLn[c4TestInd] != pac1BStart[c4TestInd])
                                break;
                            c4TestInd++;
                        }

                        // Force us back to a boundary reset if we matched
                        if (c4TestInd == c4CurBytes)
                            eState = EState_Boundary;
                    }

                    //
                    //  If still in part state, accumulate these bytes. If not, then
                    //  we are at the end of a part, so store the data.
                    //
                    if (eState == EState_InPart)
                    {
                        // We want to keep the new lines in this case
                        mbufCurLn[c4CurBytes++] = 0xD;
                        mbufCurLn[c4CurBytes++] = 0xA;

                        mbufPartBuf.CopyIn(mbufCurLn, c4CurBytes, c4PartBytes);
                        c4PartBytes += c4CurBytes;

                        // Reset for the next line
                        c4CurBytes = 0;
                    }
                     else
                    {
                        fcolSizes.c4AddElement(c4PartBytes);
                        colContTypes.objAdd(strPartContType);
                        colDispositions.objAdd(strPartDisposition);
                        colTransEncodings.objAdd(strPartTransEncoding);

                        // To only store the actual bytes for this round
                        THeapBuf& mbufTar = colParts.objPlace(c4PartBytes, c4PartBytes + 1);
                        mbufTar.CopyIn(mbufPartBuf, c4PartBytes);

                        // And go back out to the outer loop
                        break;
                    }
                }
                 else
                {
                    // Nothing special so just accumulate
                    mbufCurLn[c4CurBytes++] = c1Cur;
                }
            }
        }
    }

    // If we didn't make it to the end state, can't be good
    if (eState != EState_AtEnd)
    {
        facCIDNet().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kNetErrs::errcMPMIME_Truncated
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }
}

