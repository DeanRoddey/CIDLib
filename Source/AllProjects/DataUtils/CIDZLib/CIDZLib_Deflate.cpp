//
// FILE NAME: CIDZLib_Deflate.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/19/2004
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
//  This file implements the deflate algorithm. This is a private method of
//  the compressor impl class, and we've just broken this code out separately
//  to keep the class' main file from getting too big.
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
//  Local helpers
// ---------------------------------------------------------------------------
static tCIDLib::TVoid CheckIndex(const  tCIDLib::TCard4     c4Line
                                , const tCIDLib::TCh* const pszData
                                , const tCIDLib::TCard4     c4Index
                                , const tCIDLib::TCard4     c4Count)
{
    if (c4Index >= c4Count)
    {
        facCIDZLib().ThrowErr
        (
            CID_FILE
            , c4Line
            , kZLibErrs::errcDbg_DeflIndexErr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TString(pszData)
            , TCardinal(c4Index)
            , TCardinal(c4Count)
        );
    }
}



// ---------------------------------------------------------------------------
//  TZLibCompImpl: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Used in decompression mode, it will load up more input (if available)
//  into the sliding window buffer, moving any trailing part down to the
//  bottom if needed, to make room for more.
//
//  It updates the end of data pointer, and if it has to move the data
//  down, it updates any member pointers/indices into the buffer
//  (potentially throwing away those that now refer to data in the now
//  lost lower half), and will return true so that the caller can adjust
//  locals.
//
tCIDLib::TBoolean TZLibCompImpl::bFillWindow()
{
    //
    //  If the read position is within the minimum lookahead of the end of
    //  the buffer, then we want to move the top half (32K) to the bottom.
    //  Note that this maintains our 32K lookback requirement.
    //
    tCIDLib::TBoolean bShifted = kCIDLib::False;
    if (m_c4CurOfs >= kCIDZLib_::c4WndBufSz - kCIDZLib_::c4MinLookAhead)
    {
        TRawMem::CopyMemBuf
        (
            m_pc1WndBuf
            , m_pc1WndBuf + kCIDZLib_::c4WndSz
            , kCIDZLib_::c4WndSz
        );

        // Adjust members that reference into the buffer
        m_c4CurOfs -= kCIDZLib_::c4WndSz;

        // Slide the hash table and previous link tables back
        tCIDLib::TCard2* pc2T = m_pc2HashTbl;
        tCIDLib::TCard2* pc2P = m_pc2HashPrev;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < kCIDZLib_::c4HashSz; c4Index++)
        {
            tCIDLib::TCard2 c2T = *pc2T;
            tCIDLib::TCard2 c2P = *pc2P;

            if (c2T >= kCIDZLib_::c4WndSz)
                c2T -= kCIDZLib_::c4WndSz;
            else
                c2T = 0;

            if (c2P >= kCIDZLib_::c4WndSz)
                c2P -= kCIDZLib_::c4WndSz;
            else
                c2P = 0;

            *pc2T++ = c2T;
            *pc2P++ = c2P;
        }
        bShifted = kCIDLib::True;
    }

    // Calculate now many bytes we could get into the buffer now
    const tCIDLib::TCard4 c4CurEnd = m_c4CurOfs + m_c4BytesAvail;
    const tCIDLib::TCard4 c4ToRead = kCIDZLib_::c4WndBufSz - c4CurEnd;

    // And try to read up to that much, putting it in after any current
    const tCIDLib::TCard4 c4Got = c4ReadBuf(m_pc1WndBuf + c4CurEnd, c4ToRead);

    // Bump up the bytes avoid pointer by the bytes we got
    m_c4BytesAvail += c4Got;

    return bShifted;
}



tCIDLib::TVoid
TZLibCompImpl::CompressBlock(const  TZTree&             treeLits
                            , const TZTree&             treeDist
                            , const tCIDLib::TCard4     c4LastLit)
{
    if (c4LastLit)
    {
        tCIDLib::TCard4 c4Index = 0;
        do
        {
            #if CID_DEBUG_ON
            CheckIndex(CID_LINE, L"Accums", c4Index, kCIDZLib_::c4WndSz);
            #endif

            tCIDLib::TCard2 c2Dist = m_pc2DistAccum[c4Index];
            tCIDLib::TCard2 c2LenCode = m_pc1LLAccum[c4Index++];

            if (!c2Dist)
            {
                SendCode(c2LenCode, treeLits);
            }
             else
            {
                #if CID_DEBUG_ON
                CheckIndex(CID_LINE, L"LenCodes", c2LenCode, tCIDLib::c4ArrayElems(ac1LengthCode));
                #endif

                tCIDLib::TCard2 c2Code = ac1LengthCode[c2LenCode];
                SendCode(c2Code + kCIDZLib_::c4Literals + 1, treeLits);
                tCIDLib::TCard2 c2Extra = kCIDZLib_::ac1ExtraLBits[c2Code];

                if (c2Extra)
                {
                    c2LenCode -= asBaseLength[c2Code];
                    SendBits(c2LenCode, c2Extra);
                }

                c2Dist--;
                c2Code = c1DistCode(c2Dist);

                SendCode(c2Code, treeDist);

                #if CID_DEBUG_ON
                CheckIndex(CID_LINE, L"ExtraBits", c2Code, tCIDLib::c4ArrayElems(kCIDZLib_::ac1ExtraDBits));
                #endif
                c2Extra = kCIDZLib_::ac1ExtraDBits[c2Code];
                if (c2Extra)
                {
                    c2Dist -= asBaseDist[c2Code];
                    SendBits(c2Dist, c2Extra);
                }
            }
        } while (c4Index < c4LastLit);
    }

    SendCode(kCIDZLib_::c2EndBlock, treeLits);
    m_c2LastEOBLen = treeLits.c2Len(kCIDZLib_::c2EndBlock);
}


//
//  The main deflation entry point. The pass us the compression method to
//  use, which controls some performance flags and the particular compression
//  scheme we use.
//
tCIDLib::TVoid
TZLibCompImpl::Deflate(const tCIDZLib_::ECompMethods eMethod)
{
    // Write out the header
    tCIDLib::TCard2 c2Header = tCIDLib::TCard2
    (
        (tCIDLib::TCard2(eMethod) + ((kCIDZLib_::c4WndBits - 8) << 4)) << 8
    );

    tCIDLib::TCard2 c2LvlFlags;
    if ((m_eStrategy >= tCIDZLib_::EStrategies::HuffmanOnly)
    ||  (m_eCompLevel < tCIDZLib::ECompLevels::L2))
    {
        c2LvlFlags = 0;
    }
     else if (m_eCompLevel < tCIDZLib::ECompLevels::L6)
    {
        c2LvlFlags = 1;
    }
     else if (m_eCompLevel == tCIDZLib::ECompLevels::L6)
    {
        c2LvlFlags = 2;
    }
     else
    {
        c2LvlFlags = 3;
    }

    c2Header |= (c2LvlFlags << 6);
    c2Header += 31 - (c2Header % 31);
    PutShortMSB(c2Header);

    // Reset the adler hash value now
    m_c4Adler = TRawMem::hshHashBufferAdler32(0, 0, 0);

    // Call the correct deflation method based on compression level
    tCIDZLib_::ECompFuncs eFunc
    (
        kCIDZLib_::aStratTable[tCIDLib::c4EnumOrd(m_eCompLevel)].eFunc
    );
    if (eFunc == tCIDZLib_::ECompFuncs::Store)
        DeflateStore();
    else if (eFunc == tCIDZLib_::ECompFuncs::Fast)
        DeflateFast();
    else
        DeflateSlow();

    // Get any remain bits out and align on byte boundary
    FlushBitBuf();

    // Write out the trailer info
    PutShortMSB(TRawBits::c2High16From32(m_c4Adler));
    PutShortMSB(TRawBits::c2Low16From32(m_c4Adler));
}


tCIDLib::TVoid TZLibCompImpl::DeflateFast()
{
    // For now, we don't support this, so throw...
    facCIDZLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kZLibErrs::errcDbg_NoFastDeflate
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotSupported
    );
}


tCIDLib::TVoid TZLibCompImpl::DeflateSlow()
{
    //
    //  Loop till done. On each round we see if we need to update the
    //  sliding window, and then look for matches.
    //
    tCIDLib::TBoolean   bMatchAvail  = kCIDLib::False;
    tCIDLib::TCard2     c2CurHash    = 0;
    tCIDLib::TCard2     c2InsHash    = 0;
    tCIDLib::TCard4     c4LitInd     = 0;
    tCIDLib::TCard4     c4PrevLen    = kCIDZLib_::c4MinMatch - 1;
    tCIDLib::TCard4     c4PrevMatch  = 0;
    tCIDLib::TCard4     c4Matches    = 0;
    tCIDLib::TCard4     c4MatchLen   = kCIDZLib_::c4MinMatch - 1;
    tCIDLib::TCard4     c4MatchStart = 0;
    tCIDLib::TInt4      i4LastBlock  = tCIDLib::TInt4(m_c4CurOfs);
    while (kCIDLib::True)
    {
        if (m_c4BytesAvail < kCIDZLib_::c4MinLookAhead)
        {
            // REmember if this is the first read
            const tCIDLib::TBoolean bFirst = m_c4TotalIn == 0;
            if (bFillWindow())
            {
                //
                //  It swapped the top half to the bottom, so we have to
                //  adjust some things that are offsets into the window
                //  buffer. bFillWindow() adjusted any members that needed
                //  adustment.
                //
                c4MatchStart -= kCIDZLib_::c4WndSz;

                // This can go negative
                i4LastBlock -= tCIDLib::TInt4(kCIDZLib_::c4WndSz);
            }

            // If no bytes and none were gotten, then we are done
            if (!m_c4BytesAvail)
                break;

            // If the first read, then init the hash with the first two bytes
            if (bFirst && (m_c4BytesAvail > kCIDZLib_::c4MinMatch))
            {
                c2InsHash = m_pc1WndBuf[0];
                c2InsHash = ((c2InsHash << kCIDZLib_::c4HashShift)
                            ^ m_pc1WndBuf[m_c4CurOfs + 1])
                            & kCIDZLib_::c4HashMask;
            }
        }

        //
        //  Kick start this round by putting the current minimum match sized
        //  string into the hash table, adn remember it's hash as the current
        //  hash head.
        //
        if (m_c4BytesAvail >= kCIDZLib_::c4MinMatch)
        {
            c2InsHash = ((c2InsHash << kCIDZLib_::c4HashShift)
                        ^ m_pc1WndBuf[m_c4CurOfs + (kCIDZLib_::c4MinMatch - 1)])
                        & kCIDZLib_::c4HashMask;

            c2CurHash = m_pc2HashTbl[c2InsHash];
            m_pc2HashPrev[m_c4CurOfs & kCIDZLib_::c4WndMask] = c2CurHash;
            m_pc2HashTbl[c2InsHash] = tCIDLib::TCard2(m_c4CurOfs);
        }

        //
        //  Find the longest match with a previous string we've added to the
        //  hash table. If we don't find one, put out the current byte as a
        //  literal byte and move up and start over.
        //
        c4PrevLen = c4MatchLen;
        c4PrevMatch = c4MatchStart;
        c4MatchLen = kCIDZLib_::c4MinMatch - 1;

        //
        //  If we got a hit this time, and the last match we got is less
        //  than the max laxy match length, and the distance between our
        //  current position and the previous string with this hash, then
        //  lets look for a longer match.
        //
        if (c2CurHash
        &&  (c4PrevLen < m_c4MaxLazyLen)
        &&  ((m_c4CurOfs - c2CurHash) < kCIDZLib_::c4MaxDist))
        {
            if (m_eStrategy < tCIDZLib_::EStrategies::HuffmanOnly)
            {
                c4MatchLen = c4LongestMatch
                (
                    c2CurHash
                    , c4PrevMatch
                    , c4PrevLen
                    , c4MatchStart
                );
            }
             else if ((m_eStrategy < tCIDZLib_::EStrategies::HuffmanOnly)
                  &&  ((m_c4CurOfs - c2CurHash) == 1))
            {
                // <TBD>
                // For now, we don't support this, so throw...
                facCIDZLib().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kZLibErrs::errcDbg_NoFastLongest
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::NotSupported
                );
#if 0
                c4MatchLen = c4LongestMatchFast
                (
                    c2CurHash
                    , c4PrevMatch
                    , c4PrevLen
                    , c4MatchStart
                );
#endif
            }

            if ((c4MatchLen <= 5)
            &&  ((m_eStrategy == tCIDZLib_::EStrategies::Filtered)
            ||   ((c4MatchLen == kCIDZLib_::c4MinMatch)
            &&    (m_c4CurOfs - c4MatchStart > kCIDZLib_::c4TooFar))))
            {
                c4MatchLen = kCIDZLib_::c4MinMatch - 1;
            }
        }

        //
        //  If there was a match last time, and the current match isn't
        //  better than that, then output the previous match. If no previous
        //  match, output the current start byte as a literal. If the new
        //  one is better, then truncate the old one to a single byte.
        //
        if ((c4PrevLen >= kCIDZLib_::c4MinMatch) && (c4MatchLen <= c4PrevLen))
        {
            //
            //  Calc the point at which we want to give up looking for
            //  matches because of lookahead problems.
            //
            const tCIDLib::TCard4 c4InsertLim
            (
                m_c4CurOfs + m_c4BytesAvail - kCIDZLib_::c4MinMatch
            );

            const tCIDLib::TCard1 c1LenCode = tCIDLib::TCard1
            (
                c4PrevLen - kCIDZLib_::c4MinMatch
            );

            const tCIDLib::TCard2 c2Dist = tCIDLib::TCard2
            (
                (m_c4CurOfs - 1) - c4PrevMatch
            );

            // Update the stats for later tree building
            #if CID_DEBUG_ON
            CheckIndex(CID_LINE, L"Accums", c4LitInd, kCIDZLib_::c4WndSz);
            #endif
            m_pc2DistAccum[c4LitInd] = c2Dist;
            m_pc1LLAccum[c4LitInd] = c1LenCode;
            c4LitInd++;

            m_tdDynLens.treeData().BumpFreq
            (
                ac1LengthCode[c1LenCode] + kCIDZLib_::c4Literals + 1
            );
            m_tdDynDist.treeData().BumpFreq(c1DistCode(c2Dist - 1));

            // Bump the count of matches
            c4Matches++;

            // Bump down the count of bytes left to read in window
            m_c4BytesAvail -= c4PrevLen - 1;

            //
            //  Insert all the strings (after the first two which where done
            //  above.
            //
            c4PrevLen -= 2;
            do
            {
                if (++m_c4CurOfs <= c4InsertLim)
                {
                    c2InsHash = ((c2InsHash << kCIDZLib_::c4HashShift)
                                ^ m_pc1WndBuf[m_c4CurOfs + (kCIDZLib_::c4MinMatch - 1)])
                                & kCIDZLib_::c4HashMask;

                    c2CurHash = m_pc2HashTbl[c2InsHash];
                    m_pc2HashPrev[m_c4CurOfs & kCIDZLib_::c4WndMask] = c2CurHash;
                    m_pc2HashTbl[c2InsHash] = tCIDLib::TCard2(m_c4CurOfs);
                }
            }   while (--c4PrevLen);

            //
            //  Ok, we are going to start over now since we've taken the
            //  match.
            //
            bMatchAvail = kCIDLib::False;
            c4MatchLen = kCIDZLib_::c4MinMatch - 1;
            m_c4CurOfs++;

            // Flush if we need to
            if (c4LitInd == kCIDZLib_::c4WndSz - 1)
            {
                FlushBlock(i4LastBlock, c4LitInd, kCIDLib::False);
                i4LastBlock = tCIDLib::TInt4(m_c4CurOfs);
                c4LitInd = 0;
            }
        }
         else if (bMatchAvail)
        {
            //
            //  There was a match last time, but the new one is better, so
            //  we truncate the previous one. So add the previous character
            //  to the stats for literals.
            //
            #if CID_DEBUG_ON
            CheckIndex(CID_LINE, L"Accums", c4LitInd, kCIDZLib_::c4WndSz);
            #endif
            m_pc2DistAccum[c4LitInd] = 0;
            m_pc1LLAccum[c4LitInd] = m_pc1WndBuf[m_c4CurOfs - 1];
            c4LitInd++;
            m_tdDynLens.treeData().BumpFreq(m_pc1WndBuf[m_c4CurOfs - 1]);

            // Flush if we need to
            if (c4LitInd == kCIDZLib_::c4WndSz - 1)
            {
                FlushBlock(i4LastBlock, c4LitInd, kCIDLib::False);
                i4LastBlock = tCIDLib::TInt4(m_c4CurOfs);
                c4LitInd = 0;
            }

            m_c4CurOfs++;
            m_c4BytesAvail--;
        }
         else
        {
            //
            //  There was no previous match, so we just remember we now
            //  have one, move up, and go around again.
            //
            bMatchAvail = kCIDLib::True;
            m_c4CurOfs++;
            m_c4BytesAvail--;
        }
    }

    // Handle any last match by just putting in a literal
    if (bMatchAvail)
    {
        #if CID_DEBUG_ON
        CheckIndex(CID_LINE, L"Accums", c4LitInd, kCIDZLib_::c4WndSz);
        #endif
        m_pc2DistAccum[c4LitInd] = 0;
        m_pc1LLAccum[c4LitInd] = m_pc1WndBuf[m_c4CurOfs - 1];
        c4LitInd++;
        m_tdDynLens.treeData().BumpFreq(m_pc1WndBuf[m_c4CurOfs - 1]);
    }

    // Flush any remaining dta into a last block
    FlushBlock(i4LastBlock, c4LitInd, kCIDLib::True);
    i4LastBlock = tCIDLib::TInt4(m_c4CurOfs);
    c4LitInd = 0;
}


//
//  This guy just copies the input to the output.
//
tCIDLib::TVoid TZLibCompImpl::DeflateStore()
{
    // For now, we don't support this, so throw...
    facCIDZLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kZLibErrs::errcDbg_NoStoredBlocks
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotSupported
    );
}


//
//  Flushes the current block out to the output stream, and resets the per-
//  block stuff to get them ready for the next round.
//
tCIDLib::TVoid
TZLibCompImpl::FlushBlock(  const   tCIDLib::TInt4      i4LastBlock
                            , const tCIDLib::TCard4     c4LastLit
                            , const tCIDLib::TBoolean   bEOF)
{
    // Calc the length since the last block
    const tCIDLib::TCard4 c4InBytes = tCIDLib::TCard4
    (
        tCIDLib::TInt4(m_c4CurOfs) - i4LastBlock
    );

    //
    //  We need to determine if the resulting block would be bigger with
    //  dynamnic or static trees, unless the level is zero, in which case
    //  we do a stored block.
    //
    tCIDLib::TCard4 c4MaxBitLen = 0;
    tCIDLib::TCard4 c4OptLen2;
    tCIDLib::TCard4 c4StaticLen2;
    if (m_eCompLevel > tCIDZLib::ECompLevels::L0)
    {
        //
        //  If this is our first non-stored block, then take a whack at
        //  figuring out if it is ASCII or binary.
        //
        if (m_eDataType == tCIDZLib_::EDataTypes::Unknown)
            m_eDataType = eCheckDataType();

        // Build the literals/lengths and distance trees
        tCIDLib::TCard4 c4OptLen = 0;
        tCIDLib::TCard4 c4StaticLen = 0;
        m_tdDynLens.Build(c4OptLen, c4StaticLen);
        m_tdDynDist.Build(c4OptLen, c4StaticLen);

        // Build up the bit length tree
        c4MaxBitLen = c4BuildBLTree(c4OptLen, c4StaticLen);

        c4OptLen2 = (c4OptLen + 3 + 7) >> 3;
        c4StaticLen2 = (c4StaticLen + 3 + 7) >> 3;
        if (c4StaticLen2 <= c4OptLen2)
            c4OptLen2 = c4StaticLen2;
    }
     else
    {
        //
        //  Force a stored block by setting both lengths to the input bytes
        //  plus the minimal stored block overhead.
        //
        c4OptLen2 = c4InBytes + 5;
        c4StaticLen2 = c4OptLen2;
    }

    // And now, based on what we calculated, do one of the block flush types
    if (c4InBytes + 4 <= c4OptLen2)
    {
        // Nothing gained, so do a stored block

        // Send the block header type
        tCIDLib::TCard2 c2Flag = kCIDZLib_::c1NoComp << 1;
        if (bEOF)
            c2Flag |= 1;
        SendBits(c2Flag, 3);

        // Sync the output stream to byte boundaries
        FlushBitBuf();

        // Put out the header bytes (size and XORd size)
        PutShortLSB(tCIDLib::TCard2(c4InBytes));
        PutShortLSB(tCIDLib::TCard2(~c4InBytes));

        //
        //  Copy the indicated bytes from the window buffer to the output
        //  stream.
        //
        if (i4LastBlock)
        {
            facCIDZLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kZLibErrs::errcDbg_NegBlockStart
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
            );
        }
        m_pstrmOut->c4WriteRawBuffer(&m_pc1WndBuf[i4LastBlock], c4InBytes);
    }
     else if (c4StaticLen2 == c4OptLen2)
    {
        // Use the static trees
        tCIDLib::TCard2 c2Flag = kCIDZLib_::c1Fixed << 1;
        if (bEOF)
            c2Flag |= 1;
        SendBits(c2Flag, 3);
        CompressBlock(*s_stdLens.ptreeData(), *s_stdDist.ptreeData(), c4LastLit);
    }
     else
    {
        // Use the optimal trees
        tCIDLib::TCard2 c2Flag = kCIDZLib_::c1Dynamic << 1;
        if (bEOF)
            c2Flag |= 1;
        SendBits(c2Flag, 3);
        SendAllTrees(c4MaxBitLen);
        CompressBlock(m_tdDynLens.treeData(), m_tdDynDist.treeData(), c4LastLit);
    }

    // And now do per-block init to get ready for the next round
    PerBlockReset();

    // If end of file, then get any trailng bits out
    if (bEOF)
        FlushBitBuf();
}

