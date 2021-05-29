//
// FILE NAME: CIDZLib_Inflate.cpp
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
//  This file implements the inflate algorithm. This is a private method of
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
//  Local types and constants
// ---------------------------------------------------------------------------
namespace
{
    namespace CIDZLib_Inflate
    {
        constexpr tCIDLib::TCard4   c4Enough = 1440;
        constexpr tCIDLib::TCard4   c4MaxD = 154;
        constexpr tCIDLib::TCard4   c4Lens = 320;

        // -----------------------------------------------------------------------
        //  Tables for fixed table decoding
        // -----------------------------------------------------------------------
        constexpr tCIDZLib_::TCode acdLenFix[512] =
        {
              {96,7,0}, {0,8,80}, {0,8,16}, {20,8,115}, {18,7,31}, {0,8,112}, {0,8,48}
            , {0,9,192}, {16,7,10}, {0,8,96}, {0,8,32}, {0,9,160}, {0,8,0}, {0,8,128}
            , {0,8,64}, {0,9,224}, {16,7,6}, {0,8,88}, {0,8,24}, {0,9,144}, {19,7,59}
            , {0,8,120}, {0,8,56}, {0,9,208}, {17,7,17}, {0,8,104}, {0,8,40}, {0,9,176}
            , {0,8,8}, {0,8,136}, {0,8,72}, {0,9,240}, {16,7,4}, {0,8,84}, {0,8,20}
            , {21,8,227}, {19,7,43}, {0,8,116}, {0,8,52}, {0,9,200}, {17,7,13}, {0,8,100}
            , {0,8,36}, {0,9,168}, {0,8,4}, {0,8,132}, {0,8,68}, {0,9,232}, {16,7,8}
            , {0,8,92}, {0,8,28}, {0,9,152}, {20,7,83}, {0,8,124}, {0,8,60}, {0,9,216}
            , {18,7,23}, {0,8,108}, {0,8,44}, {0,9,184}, {0,8,12}, {0,8,140}, {0,8,76}
            , {0,9,248}, {16,7,3}, {0,8,82}, {0,8,18}, {21,8,163}, {19,7,35}, {0,8,114}
            , {0,8,50}, {0,9,196}, {17,7,11}, {0,8,98}, {0,8,34}, {0,9,164}, {0,8,2}
            , {0,8,130}, {0,8,66}, {0,9,228}, {16,7,7}, {0,8,90}, {0,8,26}, {0,9,148}
            , {20,7,67}, {0,8,122}, {0,8,58}, {0,9,212}, {18,7,19}, {0,8,106}, {0,8,42}
            , {0,9,180}, {0,8,10}, {0,8,138}, {0,8,74}, {0,9,244}, {16,7,5}, {0,8,86}
            , {0,8,22}, {64,8,0}, {19,7,51}, {0,8,118}, {0,8,54}, {0,9,204}, {17,7,15}
            , {0,8,102}, {0,8,38}, {0,9,172}, {0,8,6}, {0,8,134}, {0,8,70}, {0,9,236}
            , {16,7,9}, {0,8,94}, {0,8,30}, {0,9,156}, {20,7,99}, {0,8,126}, {0,8,62}
            , {0,9,220}, {18,7,27}, {0,8,110}, {0,8,46}, {0,9,188}, {0,8,14}, {0,8,142}
            , {0,8,78}, {0,9,252}, {96,7,0}, {0,8,81}, {0,8,17}, {21,8,131}, {18,7,31}
            , {0,8,113}, {0,8,49}, {0,9,194}, {16,7,10}, {0,8,97}, {0,8,33}, {0,9,162}
            , {0,8,1}, {0,8,129}, {0,8,65}, {0,9,226}, {16,7,6}, {0,8,89}, {0,8,25}
            , {0,9,146}, {19,7,59}, {0,8,121}, {0,8,57}, {0,9,210}, {17,7,17}, {0,8,105}
            , {0,8,41}, {0,9,178}, {0,8,9}, {0,8,137}, {0,8,73}, {0,9,242}, {16,7,4}
            , {0,8,85}, {0,8,21}, {16,8,258}, {19,7,43}, {0,8,117}, {0,8,53}, {0,9,202}
            , {17,7,13}, {0,8,101}, {0,8,37}, {0,9,170}, {0,8,5}, {0,8,133}, {0,8,69}
            , {0,9,234}, {16,7,8}, {0,8,93}, {0,8,29}, {0,9,154}, {20,7,83}, {0,8,125}
            , {0,8,61}, {0,9,218}, {18,7,23}, {0,8,109}, {0,8,45}, {0,9,186}, {0,8,13}
            , {0,8,141}, {0,8,77}, {0,9,250}, {16,7,3}, {0,8,83}, {0,8,19}, {21,8,195}
            , {19,7,35}, {0,8,115}, {0,8,51}, {0,9,198}, {17,7,11}, {0,8,99}, {0,8,35}
            , {0,9,166}, {0,8,3}, {0,8,131}, {0,8,67}, {0,9,230}, {16,7,7}, {0,8,91}
            , {0,8,27}, {0,9,150}, {20,7,67}, {0,8,123}, {0,8,59}, {0,9,214}, {18,7,19}
            , {0,8,107}, {0,8,43}, {0,9,182}, {0,8,11}, {0,8,139}, {0,8,75}, {0,9,246}
            , {16,7,5}, {0,8,87}, {0,8,23}, {64,8,0}, {19,7,51}, {0,8,119}, {0,8,55}
            , {0,9,206}, {17,7,15}, {0,8,103}, {0,8,39}, {0,9,174}, {0,8,7}, {0,8,135}
            , {0,8,71}, {0,9,238}, {16,7,9}, {0,8,95}, {0,8,31}, {0,9,158}, {20,7,99}
            , {0,8,127}, {0,8,63}, {0,9,222}, {18,7,27}, {0,8,111}, {0,8,47}, {0,9,190}
            , {0,8,15}, {0,8,143}, {0,8,79}, {0,9,254}, {96,7,0}, {0,8,80}, {0,8,16}
            , {20,8,115}, {18,7,31}, {0,8,112}, {0,8,48}, {0,9,193}, {16,7,10}, {0,8,96}
            , {0,8,32}, {0,9,161}, {0,8,0}, {0,8,128}, {0,8,64}, {0,9,225}, {16,7,6}
            , {0,8,88}, {0,8,24}, {0,9,145}, {19,7,59}, {0,8,120}, {0,8,56}, {0,9,209}
            , {17,7,17}, {0,8,104}, {0,8,40}, {0,9,177}, {0,8,8}, {0,8,136}, {0,8,72}
            , {0,9,241}, {16,7,4}, {0,8,84}, {0,8,20}, {21,8,227}, {19,7,43}, {0,8,116}
            , {0,8,52}, {0,9,201}, {17,7,13}, {0,8,100}, {0,8,36}, {0,9,169}, {0,8,4}
            , {0,8,132}, {0,8,68}, {0,9,233}, {16,7,8}, {0,8,92}, {0,8,28}, {0,9,153}
            , {20,7,83}, {0,8,124}, {0,8,60}, {0,9,217}, {18,7,23}, {0,8,108}, {0,8,44}
            , {0,9,185}, {0,8,12}, {0,8,140}, {0,8,76}, {0,9,249}, {16,7,3}, {0,8,82}
            , {0,8,18}, {21,8,163}, {19,7,35}, {0,8,114}, {0,8,50}, {0,9,197}, {17,7,11}
            , {0,8,98}, {0,8,34}, {0,9,165}, {0,8,2}, {0,8,130}, {0,8,66}, {0,9,229}
            , {16,7,7}, {0,8,90}, {0,8,26}, {0,9,149}, {20,7,67}, {0,8,122}, {0,8,58}
            , {0,9,213}, {18,7,19}, {0,8,106}, {0,8,42}, {0,9,181}, {0,8,10}, {0,8,138}
            , {0,8,74}, {0,9,245}, {16,7,5}, {0,8,86}, {0,8,22}, {64,8,0}, {19,7,51}
            , {0,8,118}, {0,8,54}, {0,9,205}, {17,7,15}, {0,8,102}, {0,8,38}, {0,9,173}
            , {0,8,6}, {0,8,134}, {0,8,70}, {0,9,237}, {16,7,9}, {0,8,94}, {0,8,30}
            , {0,9,157}, {20,7,99}, {0,8,126}, {0,8,62}, {0,9,221}, {18,7,27}, {0,8,110}
            , {0,8,46}, {0,9,189}, {0,8,14}, {0,8,142}, {0,8,78}, {0,9,253}, {96,7,0}
            , {0,8,81}, {0,8,17}, {21,8,131}, {18,7,31}, {0,8,113}, {0,8,49}, {0,9,195}
            , {16,7,10}, {0,8,97}, {0,8,33}, {0,9,163}, {0,8,1}, {0,8,129}, {0,8,65}
            , {0,9,227}, {16,7,6}, {0,8,89}, {0,8,25}, {0,9,147}, {19,7,59}, {0,8,121}
            , {0,8,57}, {0,9,211}, {17,7,17}, {0,8,105}, {0,8,41}, {0,9,179}, {0,8,9}
            , {0,8,137}, {0,8,73}, {0,9,243}, {16,7,4}, {0,8,85}, {0,8,21}, {16,8,258}
            , {19,7,43}, {0,8,117}, {0,8,53}, {0,9,203}, {17,7,13}, {0,8,101}, {0,8,37}
            , {0,9,171}, {0,8,5}, {0,8,133}, {0,8,69}, {0,9,235}, {16,7,8}, {0,8,93}
            , {0,8,29}, {0,9,155}, {20,7,83}, {0,8,125}, {0,8,61}, {0,9,219}, {18,7,23}
            , {0,8,109}, {0,8,45}, {0,9,187}, {0,8,13}, {0,8,141}, {0,8,77}, {0,9,251}
            , {16,7,3}, {0,8,83}, {0,8,19}, {21,8,195}, {19,7,35}, {0,8,115}, {0,8,51}
            , {0,9,199}, {17,7,11}, {0,8,99}, {0,8,35}, {0,9,167}, {0,8,3}, {0,8,131}
            , {0,8,67}, {0,9,231}, {16,7,7}, {0,8,91}, {0,8,27}, {0,9,151}, {20,7,67}
            , {0,8,123}, {0,8,59}, {0,9,215}, {18,7,19}, {0,8,107}, {0,8,43}, {0,9,183}
            , {0,8,11}, {0,8,139}, {0,8,75}, {0,9,247}, {16,7,5}, {0,8,87}, {0,8,23}
            , {64,8,0}, {19,7,51}, {0,8,119}, {0,8,55}, {0,9,207}, {17,7,15}, {0,8,103}
            , {0,8,39}, {0,9,175}, {0,8,7}, {0,8,135}, {0,8,71}, {0,9,239}, {16,7,9}
            , {0,8,95}, {0,8,31}, {0,9,159}, {20,7,99}, {0,8,127}, {0,8,63}, {0,9,223}
            , {18,7,27}, {0,8,111}, {0,8,47}, {0,9,191}, {0,8,15}, {0,8,143}, {0,8,79}
            , {0,9,255}
        };

        constexpr tCIDZLib_::TCode acdDistFix[32] =
        {
              {16,5,1}, {23,5,257}, {19,5,17}, {27,5,4097}, {17,5,5}, {25,5,1025}
            , {21,5,65}, {29,5,16385}, {16,5,3}, {24,5,513}, {20,5,33}, {28,5,8193}
            , {18,5,9}, {26,5,2049}, {22,5,129}, {64,5,0}, {16,5,2}, {23,5,385}
            , {19,5,25}, {27,5,6145}, {17,5,7}, {25,5,1537}, {21,5,97}, {29,5,24577}
            , {16,5,4}, {24,5,769}, {20,5,49}, {28,5,12289}, {18,5,13}, {26,5,3073}
            , {22,5,193}, {64,5,0}
        };
    }
}


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
//  Pulls out bits of the decompressed data in the accumulator. We use the
//  same valid bit counter as the compression side, but we have to use a
//  different (32) bit buffer. The number of bits requested must have already
//  been reserved via ReserveInflBits().
//
tCIDLib::TCard2 TZLibCompImpl::c2GetInflBits(const tCIDLib::TCard4 c4ToGet)
{
    #if CID_DEBUG_ON
    if ((c4ToGet > m_c4BitCount) || (m_c4BitCount > 32))
    {
        facCIDZLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kZLibErrs::errcDbg_NotEnoughBits
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
            , TCardinal(c4ToGet)
            , TCardinal(m_c4BitCount)
        );
    }
    #endif

    tCIDLib::TCard4 c4Ret = (m_c4InflBuf & ((1UL << c4ToGet) - 1));
    m_c4InflBuf >>= c4ToGet;
    m_c4BitCount -= c4ToGet;
    return tCIDLib::TCard2(c4Ret);
}

tCIDLib::TCard4
TZLibCompImpl::c4GetInflBits(const tCIDLib::TCard4 c4ToGet)
{
    #if CID_DEBUG_ON
    if ((c4ToGet > m_c4BitCount) || (m_c4BitCount > 32))
    {
        facCIDZLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kZLibErrs::errcDbg_NotEnoughBits
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
            , TCardinal(c4ToGet)
            , TCardinal(m_c4BitCount)
        );
    }
    #endif

    tCIDLib::TCard4 c4Ret = (m_c4InflBuf & ((1UL << c4ToGet) - 1));
    m_c4InflBuf >>= c4ToGet;
    m_c4BitCount -= c4ToGet;
    return c4Ret;
}


//
//  Peeks at bits of the decompressed data in the accumulator. We use the
//  same valid bit counter as the compression side, but we have to use a
//  different (32) bit buffer.
//
tCIDLib::TCard4 TZLibCompImpl::c4PeekInflBits(const tCIDLib::TCard4 c4ToPeek)
{
    #if CID_DEBUG_ON
    if ((c4ToPeek > m_c4BitCount) || (m_c4BitCount > 32))
    {
        facCIDZLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kZLibErrs::errcDbg_NotEnoughBits
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
            , TCardinal(c4ToPeek)
            , TCardinal(m_c4BitCount)
        );
    }
    #endif
    return (m_c4InflBuf & ((1UL << c4ToPeek) - 1));
}


//
//  Same as above but doesn't check that the number of bits requested are
//  actually there.
//
tCIDLib::TCard4
TZLibCompImpl::c4PeekInflBitsNC(const tCIDLib::TCard4 c4ToGet)
{
    // At least check that they don't ask for more than we could hold
    #if CID_DEBUG_ON
    if (c4ToGet > 32)
    {
        facCIDZLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kZLibErrs::errcDbg_NotEnoughBits
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
            , TCardinal(c4ToGet)
            , TCardinal(m_c4BitCount)
        );
    }
    #endif
    return (m_c4InflBuf & ((1UL << c4ToGet) - 1));
}


tCIDLib::TVoid TZLibCompImpl::CopyInflBlock(const tCIDLib::TCard4 c4Length)
{
    //
    //  Start blasting in bytes from the input stream, flushing the window
    //  buffer as required.
    //
    tCIDLib::TCard1 c1Tmp;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Length; c4Index++)
    {
        if (m_c4BytesAvail + 1 >= kCIDZLib_::c4WndBufSz)
            UpdateDecompBuf();

        *m_pstrmIn >> c1Tmp;

        #if CID_DEBUG_ON
        CheckIndex(CID_LINE, L"WndBuf", m_c4BytesAvail, kCIDZLib_::c4WndBufSz);
        #endif
        m_pc1WndBuf[m_c4BytesAvail++] = c1Tmp;
    }
}


tCIDLib::TVoid
TZLibCompImpl::CopyInflBytes(const  tCIDLib::TCard4 c4FromOfs
                            , const tCIDLib::TCard4 c4Count)
{
    //
    //  Make sure that we have enough space to copy forward. If not, then
    //  update the window buffer to move down. Note that this cannot
    //  invalidate the from offset since a valid back link cannot be more
    //  than 32K back and the max match is only 258 bytes. So at worst we
    //  have to be within 258 bytes of the end of our 64K sliding window,
    //  and will end up with a full 32K of data to work with.
    //
    if (m_c4BytesAvail + c4Count >= kCIDZLib_::c4WndBufSz)
        UpdateDecompBuf();

    //
    //  Now get a pointer that c4FromOfs back from the current end of
    //  buffer, and one at the end of the buffer, and start copying bytes
    //  forward.
    //
    tCIDLib::TCard1* pc1Tar = m_pc1WndBuf + m_c4BytesAvail;
    tCIDLib::TCard1* pc1Src = pc1Tar - c4FromOfs;

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        *pc1Tar++ = *pc1Src++;
}


// Toss out the indicated number of bits from the bit buffer
tCIDLib::TVoid
TZLibCompImpl::DropInflBits(const tCIDLib::TCard4 c4ToDrop)
{
    #if CID_DEBUG_ON
    if ((c4ToDrop > m_c4BitCount) || (m_c4BitCount > 32))
    {
        facCIDZLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kZLibErrs::errcDbg_NotEnoughBits
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
            , TCardinal(c4ToDrop)
            , TCardinal(m_c4BitCount)
        );
    }
    #endif

    m_c4InflBuf >>= c4ToDrop;
    m_c4BitCount -= c4ToDrop;
}


//
//  This guy deflates a stream of compressed data. We have to read in the
//  header info, then start reading out blocks. We must maintain in memory
//  32K of output because references to previously matched strings can go
//  back that far.
//
tCIDLib::TVoid TZLibCompImpl::Inflate(const tCIDLib::TBoolean bHaveDict)
{
    // A permutation of the code lengths
    static constexpr tCIDLib::TCard2 ac2Order[19] =
    {
        16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
    };

    // We use a state machine to do the decoding
    tCIDZLib_::EInfModes eState = tCIDZLib_::EInfModes::Head;

    // Some tables to hold decoding info
    tCIDZLib_::TCode        acdTable[CIDZLib_Inflate::c4Enough];
    tCIDLib::TCard2         ac2Lens[CIDZLib_Inflate::c4Lens];

    tCIDLib::TBoolean       bLastBlock = kCIDLib::False;
    tCIDLib::TCard4         c4DistBits;
    tCIDLib::TCard4         c4Extra;
    tCIDLib::TCard4         c4Length;
    tCIDLib::TCard4         c4LenBits;
    tCIDLib::TCard4         c4LensCount;
    tCIDLib::TCard4         c4NumCodes;
    tCIDLib::TCard4         c4NumLens;
    tCIDLib::TCard4         c4NumDists;
    tCIDLib::TCard4         c4Offset;
    tCIDZLib_::TCode        cdLast;
    tCIDZLib_::TCode        cdThis;

    const tCIDZLib_::TCode* pcdDist = nullptr;
    const tCIDZLib_::TCode* pcdLen = nullptr;
    tCIDZLib_::TCode*       pcdNext = nullptr;

    tCIDLib::TBoolean bDone = kCIDLib::False;
    while (!bDone)
    {
        switch(eState)
        {
            case tCIDZLib_::EInfModes::Head :
            {
                ReserveInflBits(16);
                if (((c4PeekInflBits(8) << 8) | (m_c4InflBuf >> 8)) % 31)
                {
                    facCIDZLib().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kZLibErrs::errcInfl_BadHeaderChk
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Format
                    );
                }

                if (c4PeekInflBits(4) != tCIDLib::c4EnumOrd(tCIDZLib_::ECompMethods::Deflated))
                {
                    facCIDZLib().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kZLibErrs::errcInfl_BadCompType
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Format
                    );
                }
                DropInflBits(4);

                if (c4PeekInflBits(4) + 8 > kCIDZLib_::c4WndBits)
                {
                    facCIDZLib().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kZLibErrs::errcInfl_BadWndSz
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Format
                    );
                }

                if (m_c4InflBuf & 0x200)
                    eState = tCIDZLib_::EInfModes::DictId;
                else
                    eState = tCIDZLib_::EInfModes::Type;

                // Reset the bit buffer now
                m_c4BitCount = 0;
                m_c4InflBuf = 0;
                break;
            }

            case tCIDZLib_::EInfModes::DictId :
            {
                // Eat the next 32 bits and reset buffer
                ReserveInflBits(32);
                m_c4BitCount = 0;
                m_c4InflBuf = 0;

                // Set the new state and fall through
                eState = tCIDZLib_::EInfModes::Dict;
            }

            case tCIDZLib_::EInfModes::Dict :
            {
                if (!bHaveDict)
                {
                    // Why are we here?
                    facCIDZLib().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kZLibErrs::errcInfl_NeedDictionary
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Format
                    );
                }

                eState = tCIDZLib_::EInfModes::Type;
                break;
            }

            case tCIDZLib_::EInfModes::Type :
            {
                if (bLastBlock)
                {
                    SyncInflStream();
                    eState = tCIDZLib_::EInfModes::Check;
                    break;
                }
                ReserveInflBits(3);
                bLastBlock = c4GetInflBits(1) != 0;

                switch(c4GetInflBits(2))
                {
                    case 0 :
                        eState = tCIDZLib_::EInfModes::Stored;
                        break;

                    case 1 :
                        pcdLen = CIDZLib_Inflate::acdLenFix;
                        c4LenBits = 9;
                        pcdDist = CIDZLib_Inflate::acdDistFix;
                        c4DistBits = 5;
                        eState = tCIDZLib_::EInfModes::Len;
                        break;

                    case 2 :
                        eState = tCIDZLib_::EInfModes::Table;
                        break;

                    default :
                        facCIDZLib().ThrowErr
                        (
                            CID_FILE
                            , CID_LINE
                            , kZLibErrs::errcInfl_BadBlockType
                            , tCIDLib::ESeverities::Failed
                            , tCIDLib::EErrClasses::Format
                            , TCardinal(m_c4TotalIn)
                        );
                        break;
                };
                break;
            }

            case tCIDZLib_::EInfModes::Stored :
            {
                // Move up to the next byte boundary
                SyncInflStream();

                // We need a full 32 bits loaded up
                ReserveInflBits(32);

                if ((m_c4InflBuf & 0xFFFF) != ((m_c4InflBuf >> 16) ^ 0xFFFF))
                {
                    facCIDZLib().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kZLibErrs::errcInfl_BadStoredBlkLen
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Format
                    );
                }

                // Store the low 16 as the length, and clear the bit buffer
                c4Length = m_c4InflBuf & 0xFFFF;
                ResetInflBitBuf();

                // Set the new state and fall through
                eState = tCIDZLib_::EInfModes::Copy;
                [[fallthrough]];
            }

            case tCIDZLib_::EInfModes::Copy :
            {
                // Copy bytes from the input to the output
                CopyInflBlock(c4Length);

                // And go back to looking for a block type
                eState = tCIDZLib_::EInfModes::Type;
                break;
            }

            case tCIDZLib_::EInfModes::Table :
            {
                ReserveInflBits(14);
                c4NumLens = c4GetInflBits(5) + 257;
                c4NumDists = c4GetInflBits(5) + 1;
                c4NumCodes = c4GetInflBits(4) + 4;

                // Get ready to start counting len codes
                c4LensCount = 0;

                // Set next state and just fall through
                eState = tCIDZLib_::EInfModes::LenLens;
                [[fallthrough]];
            }

            case tCIDZLib_::EInfModes::LenLens :
            {
                while (c4LensCount < c4NumCodes)
                {
                    ReserveInflBits(3);

                    #if CID_DEBUG_ON
                    CheckIndex(CID_LINE, L"Orders", c4LensCount, tCIDLib::c4ArrayElems(ac2Order));
                    #endif
                    ac2Lens[ac2Order[c4LensCount++]] = c2GetInflBits(3);
                }

                while (c4LensCount < 19)
                    ac2Lens[ac2Order[c4LensCount++]] = 0;

                pcdNext = acdTable;
                pcdLen = acdTable;
                c4LenBits = 7;

                InflateTable
                (
                    tCIDZLib_::EInflTbls::Codes, ac2Lens, pcdNext, 19, c4LenBits
                );
                c4LensCount = 0;

                // Set next state and just fall through
                eState = tCIDZLib_::EInfModes::CodeLens;
                [[fallthrough]];
            }

            case tCIDZLib_::EInfModes::CodeLens :
            {
                while (c4LensCount < c4NumLens + c4NumDists)
                {
                    while(kCIDLib::True)
                    {
                        //
                        //  <TBD> This seems to be a bug in the reference impl,
                        //  in that it asks for more bits than might be there,
                        //  because it's not reserving them before it calls.
                        //  So we call a special non-checking version here for
                        //  now till we validate this.
                        //
                        cdThis = pcdLen[c4PeekInflBitsNC(c4LenBits)];
                        if (cdThis.c1Bits <= m_c4BitCount)
                            break;
                        PullCompByte();
                    }

                    if (cdThis.c2Val < 16)
                    {
                        // Throw away these bits
                        c4GetInflBits(cdThis.c1Bits);

                        #if CID_DEBUG_ON
                        CheckIndex(CID_LINE, L"Lens", c4LensCount, tCIDLib::c4ArrayElems(ac2Lens));
                        #endif
                        ac2Lens[c4LensCount++] = cdThis.c2Val;
                    }
                     else
                    {
                        tCIDLib::TCard4 c4Copy;
                        tCIDLib::TCard4 c4Len;
                        if (cdThis.c2Val == 16)
                        {
                            ReserveInflBits(cdThis.c1Bits + 2);
                            DropInflBits(cdThis.c1Bits);
                            if (!c4LensCount)
                            {
                                facCIDZLib().ThrowErr
                                (
                                    CID_FILE
                                    , CID_LINE
                                    , kZLibErrs::errcInfl_BadBitLenRep
                                    , tCIDLib::ESeverities::Failed
                                    , tCIDLib::EErrClasses::Format
                                );
                            }

                            #if CID_DEBUG_ON
                            CheckIndex(CID_LINE, L"Lens", c4LensCount - 1, tCIDLib::c4ArrayElems(ac2Lens));
                            #endif
                            c4Len = ac2Lens[c4LensCount - 1];
                            c4Copy = 3 + c4GetInflBits(2);
                        }
                         else if (cdThis.c2Val == 17)
                        {
                            ReserveInflBits(cdThis.c1Bits + 3);
                            DropInflBits(cdThis.c1Bits);
                            c4Len = 0;
                            c4Copy = 3 + c4GetInflBits(3);
                        }
                         else
                        {
                            ReserveInflBits(cdThis.c1Bits + 7);
                            DropInflBits(cdThis.c1Bits);
                            c4Len = 0;
                            c4Copy = 11 + c4GetInflBits(7);
                        }

                        if (c4LensCount + c4Copy > c4NumLens + c4NumDists)
                        {
                            facCIDZLib().ThrowErr
                            (
                                CID_FILE
                                , CID_LINE
                                , kZLibErrs::errcInfl_BadBitLenRep
                                , tCIDLib::ESeverities::Failed
                                , tCIDLib::EErrClasses::Format
                            );
                        }

                        while (c4Copy--)
                            ac2Lens[c4LensCount++] = tCIDLib::TCard2(c4Len);
                    }
                }

                // Build code tables
                pcdNext = acdTable;
                pcdLen = pcdNext;
                c4LenBits = 9;
                InflateTable
                (
                    tCIDZLib_::EInflTbls::Lens, ac2Lens, pcdNext, c4NumLens, c4LenBits
                );

                pcdDist = pcdNext;
                c4DistBits = 6;
                InflateTable
                (
                    tCIDZLib_::EInflTbls::Dists
                    , ac2Lens + c4NumLens
                    , pcdNext
                    , c4NumDists
                    , c4DistBits
                );

                // Set the new state and just fall through
                eState = tCIDZLib_::EInfModes::Len;
                [[fallthrough]];
            }

            case tCIDZLib_::EInfModes::Len :
            {
                //
                //  This isn't required, but will provide a substantial
                //  speedup, so we should do it eventually. Because of our
                //  stream based I/O, the vast bulk of the work would be
                //  done in here.
                //
//                if (c4LensCount >= 6)
//                {
//                    InflateFast();
//                    break;
//                }

                while(kCIDLib::True)
                {
                    cdThis = pcdLen[c4PeekInflBitsNC(c4LenBits)];
                    if (cdThis.c1Bits <= m_c4BitCount)
                        break;
                    PullCompByte();
                }
                if (cdThis.c1Op && ((cdThis.c1Op & 0xF0) == 0))
                {
                    cdLast = cdThis;
                    while (kCIDLib::True)
                    {
                        cdThis = pcdLen
                        [
                            cdLast.c2Val
                            + (c4PeekInflBitsNC(cdLast.c1Bits + cdLast.c1Op)
                            >> cdLast.c1Bits)
                        ];

                        if (tCIDLib::TCard4(cdLast.c1Bits + cdThis.c1Bits) <= m_c4BitCount)
                            break;
                        PullCompByte();
                    }
                    DropInflBits(cdLast.c1Bits);
                }
                DropInflBits(cdThis.c1Bits);
                c4Length = cdThis.c2Val;
                if (!cdThis.c1Op)
                {
                    eState = tCIDZLib_::EInfModes::Lit;
                    break;
                }

                if (cdThis.c1Op & 32)
                {
                    eState = tCIDZLib_::EInfModes::Type;
                    break;
                }

                if (cdThis.c1Op & 64)
                {
                    facCIDZLib().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kZLibErrs::errcInfl_BadLLCode
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Format
                    );
                }
                c4Extra = tCIDLib::TCard4((cdThis.c1Op) & 15);

                // Set the new state and just fall through
                eState = tCIDZLib_::EInfModes::LenExt;
                [[fallthrough]];
            }

            case tCIDZLib_::EInfModes::LenExt :
            {
                if (c4Extra)
                {
                    ReserveInflBits(c4Extra);
                    c4Length += c4GetInflBits(c4Extra);
                }

                // Set the new state and just fall through
                eState = tCIDZLib_::EInfModes::Dist;
                [[fallthrough]];
            }

            case tCIDZLib_::EInfModes::Dist :
            {
                while(kCIDLib::True)
                {
                    cdThis = pcdDist[c4PeekInflBitsNC(c4DistBits)];
                    if (cdThis.c1Bits <= m_c4BitCount)
                        break;
                    PullCompByte();
                }

                if (!(cdThis.c1Op & 0xF0))
                {
                    cdLast = cdThis;
                    while(kCIDLib::True)
                    {
                        cdThis = pcdDist
                        [
                            cdLast.c2Val +
                            (
                                c4PeekInflBitsNC(cdLast.c1Bits + cdLast.c1Op)
                                >> cdLast.c1Bits
                            )
                        ];
                        if (tCIDLib::TCard4(cdLast.c1Bits + cdThis.c1Bits) <= m_c4BitCount)
                            break;
                        PullCompByte();
                    }
                    DropInflBits(cdLast.c1Bits);
                }
                DropInflBits(cdThis.c1Bits);
                if (cdThis.c1Op & 64)
                {
                    facCIDZLib().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kZLibErrs::errcInfl_BadDistCode
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Format
                    );
                }
                c4Offset = cdThis.c2Val;
                c4Extra = cdThis.c1Op & 15;

                // Set the new state and just fall through
                eState = tCIDZLib_::EInfModes::DistExt;
                [[fallthrough]];
            }

            case tCIDZLib_::EInfModes::DistExt :
            {
                if (c4Extra)
                {
                    ReserveInflBits(c4Extra);
                    c4Offset += c4GetInflBits(c4Extra);
                }

                if (c4Offset > m_c4BytesAvail)
                {
                    facCIDZLib().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kZLibErrs::errcInfl_DistTooBig
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Format
                    );
                }

                // Set the new state and just fall through
                eState = tCIDZLib_::EInfModes::Match;
                [[fallthrough]];
            }

            case tCIDZLib_::EInfModes::Match :
            {
                //
                //  Copy the matched range from it's position in the buffer
                //  to the end of the buffer.
                //
                CopyInflBytes(c4Offset, c4Length);

                // And adjust our count of bytes in the buffer by the length
                m_c4BytesAvail += c4Length;

                // Reset the match info and go back to length state
                c4Offset = 0;
                c4Length = 0;

                eState = tCIDZLib_::EInfModes::Len;
                break;
            }

            case tCIDZLib_::EInfModes::Lit :
            {
                // Put the length (which is a literal in this case) out
                PutInflByte(tCIDLib::TCard1(c4Length));
                eState = tCIDZLib_::EInfModes::Len;
                break;
            }

            case tCIDZLib_::EInfModes::Check :
            {
                // Flush any remaining window data to the output stream
                if (m_c4BytesAvail)
                {
                    m_pstrmOut->c4WriteRawBuffer(m_pc1WndBuf, m_c4BytesAvail);
                    m_c4Adler = TRawMem::hshHashBufferAdler32
                    (
                        m_c4Adler
                        , m_pc1WndBuf
                        , m_c4BytesAvail
                    );
                    m_c4TotalOut += m_c4BytesAvail;
                    m_c4BytesAvail = 0;
                }

                //
                //  Get the next 32 bits (which should be the Adler sum). We
                //  synced the input stream before getting here, so this
                //  should get a clean 32 aligned bits.
                //
                ReserveInflBits(32);

                if (TRawBits::c4SwapBytes(m_c4InflBuf) != m_c4Adler)
                {
                    facCIDZLib().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kZLibErrs::errcInfl_BadCheckSum
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Format
                    );
                }

                eState = tCIDZLib_::EInfModes::Done;
                break;
            }

            case tCIDZLib_::EInfModes::Done :
            {
                bDone = kCIDLib::True;
                break;
            }

            default :
                // <TBD> Throw here if debugging
                break;
        };
    }
}


tCIDLib::TVoid
TZLibCompImpl::InflateTable(const   tCIDZLib_::EInflTbls    eTable
                            ,       tCIDLib::TCard2* const  pc2Lens
                            ,       tCIDZLib_::TCode*&      pcdTable
                            , const tCIDLib::TCard4         c4TblSz
                            ,       tCIDLib::TCard4&        c4Bits)
{
    // Some const tables that we only use here locally
    static constexpr tCIDLib::TCard2 ac2LBase[31] =
    {
        3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31
        , 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0
    };

    static constexpr tCIDLib::TCard2 ac2LExt[31] =
    {
          16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18
        , 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 16, 76, 66
    };

    static constexpr tCIDLib::TCard2 ac2DBase[32] =
    {
        1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257
        , 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289
        , 16385, 24577, 0, 0
    };

    static constexpr tCIDLib::TCard2 ac2DExt[32] =
    {
        16, 16, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22,
        23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 64, 64
    };

    tCIDLib::TCard2 ac2Count[kCIDZLib_::c4MaxBits + 1];
    tCIDLib::TCard2 ac2Offs[kCIDZLib_::c4MaxBits + 1];
    tCIDLib::TCard2 ac2Work[288];

    // Accumulate lengths for codes
    TRawMem::SetMemBuf(ac2Count, tCIDLib::TCard2(0), kCIDZLib_::c4MaxBits + 1);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TblSz; c4Index++)
        ac2Count[pc2Lens[c4Index]]++;

    // Nound code lengths, force root to be within code lengths
    tCIDLib::TCard4 c4Root = c4Bits;
    tCIDLib::TCard4 c4Max;
    tCIDLib::TCard4 c4Min;
    for (c4Max = kCIDZLib_::c4MaxBits; c4Max >= 1; c4Max--)
    {
        if (ac2Count[c4Max])
            break;
    }

    if (c4Root > c4Max)
        c4Root = c4Max;

    if (!c4Max)
    {
        facCIDZLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kZLibErrs::errcInfl_NoCodes
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    for (c4Min = 1; c4Min <= kCIDZLib_::c4MaxBits; c4Min++)
    {
        if (ac2Count[c4Min])
            break;
    }

    if (c4Root < c4Min)
        c4Root = c4Min;

    // Check for an over-subscribed or incomplete set of lengths
    tCIDLib::TInt4 i4Left = 1;
    for (tCIDLib::TCard4 c4Index = 1; c4Index <= kCIDZLib_::c4MaxBits; c4Index++)
    {
        i4Left <<= 1;
        i4Left -= ac2Count[c4Index];
        if (i4Left < 0)
        {
            facCIDZLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kZLibErrs::errcInfl_OverSubTbl
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
            );
        }
    }
    if (i4Left
    &&  ((eTable == tCIDZLib_::EInflTbls::Codes)
    ||   ((c4TblSz - ac2Count[0]) != 1)))
    {
        facCIDZLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kZLibErrs::errcInfl_IncompleteTbl
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    // Generate offsets into symbol table for each length for sorting
    ac2Offs[1] = 0;
    for (tCIDLib::TCard4 c4Index = 1; c4Index < kCIDZLib_::c4MaxBits; c4Index++)
        ac2Offs[c4Index + 1] = ac2Offs[c4Index] + ac2Count[c4Index];

    // Sort symbols by length, by symbol order within each length
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TblSz; c4Index++)
    {
        if (pc2Lens[c4Index] != 0)
            ac2Work[ac2Offs[pc2Lens[c4Index]]++] = tCIDLib::TCard2(c4Index);
    }

    //
    //  Create an fill in decoding tables. Start by setting up the code
    //  info for the type of table.
    //
    tCIDLib::TInt4   i4End;
    const tCIDLib::TCard2* pc2Base;
    const tCIDLib::TCard2* pc2Extra;
    switch (eTable)
    {
        case tCIDZLib_::EInflTbls::Codes :
            i4End = 19;

            // Dummy values, not used
            pc2Base = ac2Work;
            pc2Extra = ac2Work;
            break;

        case tCIDZLib_::EInflTbls::Lens :
            pc2Base = ac2LBase;
            pc2Base -= 257;
            pc2Extra = ac2LExt;
            pc2Extra -= 257;
            i4End = 256;
            break;

        case tCIDZLib_::EInflTbls::Dists :
            pc2Base = ac2DBase;
            pc2Extra = ac2DExt;
            i4End = -1;
            break;

        default :
            // <TBD> Throw something if debugging
            break;
    };


    // Initialize state for loop
    tCIDLib::TCard4 c4Cur = c4Root;
    tCIDLib::TCard4 c4Drop = 0;
    tCIDLib::TCard4 c4Huff = 0;
    tCIDLib::TCard4 c4Len = c4Min;
    tCIDLib::TCard4 c4Sym = 0;
    tCIDLib::TCard4 c4Low = kCIDLib::c4MaxCard;
    tCIDLib::TCard4 c4Used = 1UL << c4Root;
    tCIDLib::TCard4 c4Mask = c4Used - 1;
    tCIDZLib_::TCode* pcdNext = pcdTable;

    // Check available table space
    if ((eTable == tCIDZLib_::EInflTbls::Lens)
    &&  (c4Used >= CIDZLib_Inflate::c4Enough - CIDZLib_Inflate::c4MaxD))
    {
        facCIDZLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kZLibErrs::errcInfl_NoTblSpace
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    // Process all the codes and make the table entries
    tCIDZLib_::TCode cdThis;
    while (kCIDLib::True)
    {
        // Create table entry
        cdThis.c1Bits = tCIDLib::TCard1(c4Len - c4Drop);
        if (tCIDLib::TInt4(ac2Work[c4Sym]) < i4End)
        {
            cdThis.c1Op = (unsigned char)0;
            cdThis.c2Val = ac2Work[c4Sym];
        }
         else if ((int)(ac2Work[c4Sym]) > i4End)
        {
            cdThis.c1Op = tCIDLib::TCard1(pc2Extra[ac2Work[c4Sym]]);
            cdThis.c2Val = pc2Base[ac2Work[c4Sym]];
        }
         else
        {
            // End of block
            cdThis.c1Op = tCIDLib::TCard1(32 + 64);
            cdThis.c2Val = 0;
        }

        // Replicate for those indices with low len bits equal to huff
        tCIDLib::TCard4 c4Incr = 1UL << tCIDLib::TCard4(c4Len - c4Drop);
        tCIDLib::TCard4 c4Fill = 1UL << c4Cur;
        do
        {
            c4Fill -= c4Incr;
            pcdNext[(c4Huff >> c4Drop) + c4Fill] = cdThis;
        } while (c4Fill);

        // Backwards increment the len-bit code huff
        c4Incr = 1U << (c4Len - 1);
        while (c4Huff & c4Incr)
            c4Incr >>= 1;

        if (c4Incr)
        {
            c4Huff &= (c4Incr - 1);
            c4Huff += c4Incr;
        }
         else
        {
            c4Huff = 0;
        }

        // Go to next symbol, update count and len
        #if CID_DEBUG_ON
        CheckIndex(CID_LINE, L"Counts", c4Len, tCIDLib::c4ArrayElems(ac2Count));
        #endif
        c4Sym++;
        if (--(ac2Count[c4Len]) == 0)
        {
            if (c4Len == c4Max)
                break;
            c4Len = pc2Lens[ac2Work[c4Sym]];
        }

        // Create new sub-table if needed
        if ((c4Len > c4Root) && ((c4Huff & c4Mask) != c4Low))
        {
            // If first time, transition to sub-tables
            if (c4Drop == 0)
                c4Drop = c4Root;

            // Increment past last table
            pcdNext += 1U << c4Cur;

            // Determine c4Length of next table
            c4Cur = c4Len - c4Drop;
            i4Left = tCIDLib::TInt4(1 << c4Cur);
            while ((c4Cur + c4Drop) < c4Max)
            {
                i4Left -= tCIDLib::TInt4(ac2Count[c4Cur + c4Drop]);
                if (i4Left <= 0)
                    break;
                c4Cur++;
                i4Left <<= 1;
            }

            // Check for enough space
            c4Used += 1U << c4Cur;
            if ((eTable == tCIDZLib_::EInflTbls::Lens)
            &&  (c4Used >= CIDZLib_Inflate::c4Enough - CIDZLib_Inflate::c4MaxD))
            {
                facCIDZLib().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kZLibErrs::errcInfl_NoTblSpace
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                );
            }

            // Point entry in root table to sub-table
            c4Low = c4Huff & c4Mask;
            pcdTable[c4Low].c1Op = tCIDLib::TCard1(c4Cur);
            pcdTable[c4Low].c1Bits = tCIDLib::TCard1(c4Root);
            pcdTable[c4Low].c2Val = tCIDLib::TCard2(pcdNext - pcdTable);
        }
    }

    // Fill in the rest of the table for incomplete codes
    cdThis.c1Op = 64;
    cdThis.c1Bits = tCIDLib::TCard1(c4Len - c4Drop);
    cdThis.c2Val = 0;

    while (c4Huff != 0)
    {
        // When done with sub-table, drop back to root table
        if ((c4Drop != 0) && ((c4Huff & c4Mask) != c4Low))
        {
            c4Drop = 0;
            c4Len = c4Root;
            pcdNext = pcdTable;
            c4Cur = c4Root;
            cdThis.c1Bits = tCIDLib::TCard1(c4Len);
        }

        // put invalid code marker in table
        pcdNext[c4Huff >> c4Drop] = cdThis;

        // backwards c4Increment the len-bit code c4Huff
        tCIDLib::TCard4 c4Incr(1UL << (c4Len - 1));
        while (c4Huff & c4Incr)
            c4Incr >>= 1;

        if (c4Incr != 0)
        {
            c4Huff &= c4Incr - 1;
            c4Huff += c4Incr;
        }
         else
        {
            c4Huff = 0;
        }
    }

    // Set return parameters
    pcdTable += c4Used;
    c4Bits = c4Root;
}


tCIDLib::TVoid TZLibCompImpl::PutInflByte(const tCIDLib::TCard1 c1ToPut)
{
    //
    //  If this wouldn't fit, then update the buffer by dumping the low
    //  part to the output stream and moving the rest down.
    //
    if (m_c4BytesAvail + 1 >= kCIDZLib_::c4WndBufSz)
        UpdateDecompBuf();

    m_pc1WndBuf[m_c4BytesAvail++] = c1ToPut;
}


tCIDLib::TVoid
TZLibCompImpl::PutInflBytes(const   tCIDLib::TCard1* const  pc1From
                            , const tCIDLib::TCard4         c4Count)
{
    //
    //  If this wouldn't fit, then update the buffer by dumping the low
    //  part to the output stream and moving the rest down.
    //
    if (m_c4BytesAvail + c4Count >= kCIDZLib_::c4WndBufSz)
        UpdateDecompBuf();

    TRawMem::CopyMemBuf(&m_pc1WndBuf[m_c4BytesAvail], pc1From, c4Count);
    m_c4BytesAvail += c4Count;
}


// Just pulls another byte of input into the buffer
tCIDLib::TVoid TZLibCompImpl::PullCompByte()
{
    if (m_c4BitCount <= 24)
    {
        tCIDLib::TCard1 c1Tmp;
        *m_pstrmIn >> c1Tmp;
        m_c4TotalIn++;
        m_c4InflBuf |= tCIDLib::TCard4(c1Tmp) << m_c4BitCount;
        m_c4BitCount += 8;
    }
}


//
//  Insures that there are at least the indicated number of bits in the
//  inflate read buffer.
//
tCIDLib::TVoid
TZLibCompImpl::ReserveInflBits(const tCIDLib::TCard4 c4ToRes)
{
    while (m_c4BitCount < c4ToRes)
    {
        tCIDLib::TCard1 c1Tmp;
        *m_pstrmIn >> c1Tmp;
        m_c4TotalIn++;
        m_c4InflBuf |= tCIDLib::TCard4(c1Tmp) << m_c4BitCount;
        m_c4BitCount += 8;
    }
}


tCIDLib::TVoid TZLibCompImpl::ResetInflBitBuf()
{
    m_c4BitCount = 0;
    m_c4InflBuf = 0;
}


// Moves up to the next byte boundary in the inflate stream
tCIDLib::TVoid TZLibCompImpl::SyncInflStream()
{
    m_c4InflBuf >>= (m_c4BitCount & 7);
    m_c4BitCount -= (m_c4BitCount & 7);
}


//
//  If we've got more than a window of data in the sliding window, we can
//  write out the low bytes to the output stream and move the rest down
//  to the bottom of the buffer again, insuring that we keep 32K in there
//  to handle any back references.
//
//  m_c4BytesAvail contains the number of valid bytes in the buffer.
//
tCIDLib::TVoid TZLibCompImpl::UpdateDecompBuf()
{
    if (m_c4BytesAvail > kCIDZLib_::c4WndSz)
    {
        const tCIDLib::TCard4 c4ToWrite = m_c4BytesAvail - kCIDZLib_::c4WndSz;
        m_pstrmOut->c4WriteRawBuffer(m_pc1WndBuf, c4ToWrite);

        //
        //  Before we overwrite the stuff we wrote out, add their bytes
        //  to the Adler CRC so we can test at the end.
        //
        m_c4Adler = TRawMem::hshHashBufferAdler32(m_c4Adler, m_pc1WndBuf, c4ToWrite);

        // Move the rest down
        TRawMem::MoveMemBuf(m_pc1WndBuf, &m_pc1WndBuf[c4ToWrite], m_c4BytesAvail - c4ToWrite);

        //
        //  Reduce the amount of bytes available in the buffer by the bytes
        //  we wrote, and bump up the current offset by that much, because
        //  it represents the index (into the output stream) of the base of
        //  the buffer.
        //
        m_c4BytesAvail -= c4ToWrite;
        m_c4TotalOut += c4ToWrite;
    }
}

