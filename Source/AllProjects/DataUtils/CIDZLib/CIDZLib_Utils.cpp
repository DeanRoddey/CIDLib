//
// FILE NAME: CIDZLib_Utils.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/19/2004
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements some grunt work methods of the deflate/inflate
//  algorithms, to keep the files that implemement them from getting too
//  large.
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
#include    "CIDZLib_.hpp"


// ---------------------------------------------------------------------------
//  TZLibCompImpl: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  After the literal and distance trees are built, then a bitlength tree
//  is built from them. We return the max bit length.
//
tCIDLib::TCard4
TZLibCompImpl::c4BuildBLTree(tCIDLib::TCard4&   c4OptLen
                            , tCIDLib::TCard4&  c4StaticLen)
{
    tCIDLib::TCard4 c4MaxBL;

    // Determine the bit length frequencies for literal and distance trees
    ScanTree(m_tdDynLens);
    ScanTree(m_tdDynDist);

    //
    //  Build the bit length tree for the two other trees
    //
    //  c4OptLen now includes the length of the tree representations, except
    //  the lengths of the bit lengths codes and the 5+5+4 bits for the counts.
    //
    m_tdDynBitLen.Build(c4OptLen, c4StaticLen);

    //
    //  Determine the number of bit length codes to send. The pkzip format
    //  requires that at least 4 bit length codes be sent.
    //
    TZTree& treeBL = m_tdDynBitLen.treeData();
    for (c4MaxBL = kCIDZLib_::c4BitLenCodes - 1; c4MaxBL >= 3; c4MaxBL--)
    {
        if (treeBL.c2Len(kCIDZLib_::ac4BitLenOrder[c4MaxBL]))
            break;
    }

    // Update opt_len to include the bit length tree and counts
    c4OptLen += 3 * (c4MaxBL + 1) + (5 + 5 + 4);
    return c4MaxBL;
}


tCIDLib::TCard4
TZLibCompImpl::c4LongestMatch(  const   tCIDLib::TCard2     c2Start
                                , const tCIDLib::TCard4     c4PrevMatch
                                , const tCIDLib::TCard4     c4PrevLen
                                ,       tCIDLib::TCard4&    c4MatchOfs)
{
    tCIDLib::TCard2  c2CurMatch = c2Start;
    tCIDLib::TCard4  c4ChainLen = m_c4MaxChainLen;
    tCIDLib::TCard1* pc1Scan = m_pc1WndBuf + m_c4CurOfs;
    tCIDLib::TCard1* pc1Match;
    tCIDLib::TCard4  c4Len;
    tCIDLib::TCard4  c4BestLen = c4PrevLen;
    tCIDLib::TCard4  c4NiceMatch = m_c4NiceLen;
    tCIDLib::TCard4  c4Limit = (m_c4CurOfs > kCIDZLib_::c4MaxDist)
                               ? m_c4CurOfs - kCIDZLib_::c4MaxDist : 0;
    tCIDLib::TCard4  c4Mask = kCIDZLib_::c4WndMask;
    tCIDLib::TCard1* pc1StrEnd = m_pc1WndBuf + m_c4CurOfs + kCIDZLib_::c4MaxMatch;
    tCIDLib::TCard1  c1ScanEnd1 = pc1Scan[c4BestLen - 1];
    tCIDLib::TCard1  c1ScanEnd = pc1Scan[c4BestLen];

    // If we already have a pretty good match, don't put in too much effort
    if (c4PrevLen >= m_c4GoodLen)
        c4ChainLen >>= 2;

    // Clip back matching to what we actually have available
    if (c4NiceMatch > m_c4BytesAvail)
        c4NiceMatch = m_c4BytesAvail;

    do
    {
        pc1Match = m_pc1WndBuf + c2CurMatch;

        if ((pc1Match[c4BestLen] != c1ScanEnd)
        ||  (pc1Match[c4BestLen - 1] != c1ScanEnd1)
        ||  (*pc1Match != *pc1Scan)
        ||  (*++pc1Match != pc1Scan[1]))
        {
            continue;
        }

        pc1Scan += 2;
        pc1Match++;

        // Do 8 bytes at a time, and check for going over the end
        do
        {
        }   while ((*++pc1Scan == *++pc1Match) && (*++pc1Scan == *++pc1Match)
            &&     (*++pc1Scan == *++pc1Match) && (*++pc1Scan == *++pc1Match)
            &&     (*++pc1Scan == *++pc1Match) && (*++pc1Scan == *++pc1Match)
            &&     (*++pc1Scan == *++pc1Match) && (*++pc1Scan == *++pc1Match)
            &&     (pc1Scan < pc1StrEnd));


        c4Len = kCIDZLib_::c4MaxMatch - tCIDLib::TCard4(pc1StrEnd - pc1Scan);
        pc1Scan = pc1StrEnd - kCIDZLib_::c4MaxMatch;

        // If we got a better match, take that one
        if (c4Len > c4BestLen)
        {
            c4MatchOfs = c2CurMatch;
            c4BestLen = c4Len;

            // If it's good enough, get out
            if (c4Len >= c4NiceMatch)
                break;

            c1ScanEnd1 = pc1Scan[c4BestLen - 1];
            c1ScanEnd = pc1Scan[c4BestLen];
        }

    }   while (((c2CurMatch = m_pc2HashPrev[c2CurMatch & kCIDZLib_::c4WndMask]) > c4Limit)
              && (--c4ChainLen != 0));

    if (c4BestLen <= m_c4BytesAvail)
        return c4BestLen;
    return m_c4BytesAvail;
}



//
//  Checks the values in the passed tree (which should be the literals tree)
//  to see what values are showing up the most.
//
tCIDZLib_::EDataTypes TZLibCompImpl::eCheckDataType()
{
    tCIDLib::TCard4 c4Ind = 0;
    tCIDLib::TCard4 c4ASCIIFreq = 0;
    tCIDLib::TCard4 c4BinFreq = 0;

    const TZTree& treeLits = m_tdDynLens.treeData();

    while (c4Ind < 7)
        c4BinFreq += treeLits.c2Freq(c4Ind++);
    while (c4Ind < 128)
        c4ASCIIFreq += treeLits.c2Freq(c4Ind++);
    while (c4Ind < kCIDZLib_::c4Literals)
        c4BinFreq += treeLits.c2Freq(c4Ind++);

    if (c4BinFreq > (c4ASCIIFreq / 4))
        return tCIDZLib_::EDataTypes::Binary;

    return tCIDZLib_::EDataTypes::ASCII;
}


//
//  Flushes out any remaining bits in the bit buffer, zero padding up to
//  the next byte boundary.
//
tCIDLib::TVoid TZLibCompImpl::FlushBitBuf()
{
    if (m_c4BitCount > 8)
        PutShortLSB(m_c2BitBuf);
    else if (m_c4BitCount > 0)
        PutByte(tCIDLib::TCard1(m_c2BitBuf));

    m_c2BitBuf = 0;
    m_c4BitCount = 0;
}


tCIDLib::TVoid TZLibCompImpl::PerBlockReset()
{
    TRawMem::SetMemBuf(m_pc2HashPrev, tCIDLib::TCard2(0), kCIDZLib_::c4HashSz);
    TRawMem::SetMemBuf(m_pc2HashTbl, tCIDLib::TCard2(0), kCIDZLib_::c4HashSz);
    TRawMem::SetMemBuf(m_pc1LLAccum, tCIDLib::TCard1(0), kCIDZLib_::c4WndSz);
    TRawMem::SetMemBuf(m_pc2DistAccum, tCIDLib::TCard2(0), kCIDZLib_::c4WndSz);

    // Set up the dynamic trees
    m_tdDynBitLen.Reset();
    m_tdDynDist.Reset();
    m_tdDynLens.Reset();
    m_tdDynLens.treeData().c2Freq(kCIDZLib_::c2EndBlock, 1);
}


// Dumps a byte to the output stream and bumps the out count
tCIDLib::TVoid TZLibCompImpl::PutByte(const tCIDLib::TCard1 c1ToPut)
{
    *m_pstrmOut << c1ToPut;
    m_c4TotalOut++;
}


//
//  Dumps a short into the pending buffer, most significant byte first, then
//  the least significant byte.
//
tCIDLib::TVoid TZLibCompImpl::PutShortLSB(const tCIDLib::TCard2 c2ToPut)
{
    *m_pstrmOut << tCIDLib::TCard1(c2ToPut & 0xFF);
    *m_pstrmOut << tCIDLib::TCard1(c2ToPut >> 8);
    m_c4TotalOut += 2;
}

tCIDLib::TVoid TZLibCompImpl::PutShortMSB(const tCIDLib::TCard2 c2ToPut)
{
    *m_pstrmOut << tCIDLib::TCard1(c2ToPut >> 8);
    *m_pstrmOut << tCIDLib::TCard1(c2ToPut & 0xFF);
    m_c4TotalOut += 2;
}


tCIDLib::TVoid TZLibCompImpl::Reset()
{
    m_bEndOfInput = kCIDLib::False;
    m_c2LastEOBLen = 8;
    m_c4Adler = TRawMem::hshHashBufferAdler32(0, 0, 0);
    m_c4BitCount = 0;
    m_c4BytesAvail = 0;
    m_c4CurOfs = 0;
    m_c4InflBuf = 0;
    m_c4TotalIn = 0;
    m_c4TotalOut = 0;
    m_eDataType = tCIDZLib_::EDataTypes::Unknown;

    // And do an initial reset of per-block data
    PerBlockReset();
}


tCIDLib::TVoid TZLibCompImpl::ScanTree(TZTreeDescr& tdToScan)
{
    TZTree& treeScan = tdToScan.treeData();
    const tCIDLib::TCard4 c4MaxCode = tdToScan.c4MaxCode();
    TZTree& treeBL = m_tdDynBitLen.treeData();

    tCIDLib::TCard2 c2NextLen = treeScan.c2Len(0);
    tCIDLib::TCard4 c4MaxCount = 7;
    tCIDLib::TCard4 c4MinCount = 4;
    tCIDLib::TCard2 c2PrevLen= kCIDLib::c2MaxCard;

    if (!c2NextLen)
    {
        c4MaxCount = 138;
        c4MinCount = 3;
    }

    // Add a guard value
    treeScan.c2Len(c4MaxCode + 1, kCIDLib::c2MaxCard);

    tCIDLib::TCard4 c4Count = 0;
    for (tCIDLib::TCard4 c4Index = 0; c4Index <= c4MaxCode; c4Index++)
    {
        const tCIDLib::TCard2 c2CurLen = c2NextLen;
        c2NextLen = treeScan.c2Len(c4Index + 1);
        if ((++c4Count < c4MaxCount) && (c2CurLen == c2NextLen))
            continue;

        if (c4Count < c4MinCount)
        {
            treeBL.IncFreq(c2CurLen, tCIDLib::TCard2(c4Count));
        }
         else if (c2CurLen != 0)
        {
            if (c2CurLen != c2PrevLen)
                treeBL.BumpFreq(c2CurLen);
            treeBL.BumpFreq(kCIDZLib_::c2Rep_3_6);
        }
         else if (c4Count <= 10)
        {
            treeBL.BumpFreq(kCIDZLib_::c2RepZ_3_10);
        }
         else
        {
            treeBL.BumpFreq(kCIDZLib_::c2RepZ_11_138);
        }

        c4Count = 0;
        c2PrevLen = c2CurLen;
        if (!c2NextLen)
        {
            c4MaxCount = 138;
            c4MinCount = 3;
        }
         else if (c2CurLen == c2NextLen)
        {
            c4MaxCount = 6;
            c4MinCount = 3;
        }
         else
        {
            c4MaxCount = 7;
            c4MinCount = 4;
        }
    }
}


tCIDLib::TVoid TZLibCompImpl::SendAllTrees(const tCIDLib::TCard4 c4MaxBLInd)
{
    SendBits(m_tdDynLens.c4MaxCode() - 256, 5);
    SendBits(m_tdDynDist.c4MaxCode(), 5);
    SendBits(c4MaxBLInd - 3, 4);

    const TZTree& treeBL = m_tdDynBitLen.treeData();
    for (tCIDLib::TCard4 c4Index = 0; c4Index <= c4MaxBLInd; c4Index++)
        SendBits(treeBL.c2Len(kCIDZLib_::ac4BitLenOrder[c4Index]), 3);

    SendTree(m_tdDynLens);
    SendTree(m_tdDynDist);
}


tCIDLib::TVoid
TZLibCompImpl::SendBits(const   tCIDLib::TCard4 c4ToSend
                        , const tCIDLib::TCard4 c4Bits)
{
    if (m_c4BitCount > 16 - c4Bits)
    {
        tCIDLib::TCard2 c2Val = tCIDLib::TCard2(c4ToSend);
        m_c2BitBuf |= (c2Val << m_c4BitCount);
        PutShortLSB(m_c2BitBuf);
        m_c2BitBuf = c2Val >> (16 - m_c4BitCount);
        m_c4BitCount += tCIDLib::TInt4(c4Bits) - 16;
    }
     else
    {
        m_c2BitBuf |= tCIDLib::TCard2(c4ToSend << m_c4BitCount);
        m_c4BitCount += c4Bits;
    }
}


tCIDLib::TVoid
TZLibCompImpl::SendCode(const   tCIDLib::TCard2 c2Code
                        , const TZTree&         treeData)
{
    SendBits(treeData.c2Code(c2Code), treeData.c2Len(c2Code));
}


tCIDLib::TVoid TZLibCompImpl::SendTree(const TZTreeDescr& tdToSend)
{
    const TZTree& treeSend = tdToSend.treeData();
    tCIDLib::TCard2 c2NextLen = treeSend.c2Len(0);
    tCIDLib::TCard4 c4MaxCount = 7;
    tCIDLib::TCard4 c4MinCount = 4;
    tCIDLib::TCard4 c2PrevLen= kCIDLib::c2MaxCard;
    tCIDLib::TCard4 c4MaxCode= tdToSend.c4MaxCode();

    if (!c2NextLen)
    {
        c4MaxCount = 138;
        c4MinCount = 3;
    }

    tCIDLib::TCard4 c4Count = 0;
    for (tCIDLib::TCard4 c4Index = 0; c4Index <= c4MaxCode; c4Index++)
    {
        const tCIDLib::TCard2 c2CurLen = c2NextLen;
        c2NextLen = treeSend.c2Len(c4Index + 1);
        if ((++c4Count < c4MaxCount) && (c2CurLen == c2NextLen))
            continue;

        if (c4Count < c4MinCount)
        {
            do
            {
                SendCode(c2CurLen, m_tdDynBitLen.treeData());
            } while (--c4Count);
        }
         else if (c2CurLen)
        {
            if (c2CurLen != c2PrevLen)
            {
                SendCode(c2CurLen, m_tdDynBitLen.treeData());
                c4Count--;
            }

            SendCode(kCIDZLib_::c2Rep_3_6, m_tdDynBitLen.treeData());
            SendBits(c4Count - 3, 2);
        }
         else if (c4Count <= 10)
        {
            SendCode(kCIDZLib_::c2RepZ_3_10, m_tdDynBitLen.treeData());
            SendBits(c4Count - 3, 3);
        }
         else
        {
            SendCode(kCIDZLib_::c2RepZ_11_138, m_tdDynBitLen.treeData());
            SendBits(c4Count - 11, 7);
        }

        c4Count = 0;
        c2PrevLen = c2CurLen;
        if (!c2NextLen)
        {
            c4MaxCount = 138;
            c4MinCount = 3;
        }
         else if (c2CurLen == c2NextLen)
        {
            c4MaxCount = 6;
            c4MinCount = 3;
        }
         else
        {
            c4MaxCount = 7;
            c4MinCount = 4;
        }
    }
}


