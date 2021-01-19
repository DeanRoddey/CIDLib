//
// FILE NAME: CIDMacroEng_OpCode.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/15/2003
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
//  This file implements the TMEngOpCode class, which represents an opcode
//  in the virtual machine assembly code of the engine.
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
#include    "CIDMacroEng_.hpp"



// ---------------------------------------------------------------------------
//  CLASS: TMEngOpCode
// PREFIX: meop
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngOpCode: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngOpCode::TMEngOpCode() :

    m_eOpCode(tCIDMacroEng::EOpCodes::None)
{
    TRawMem::SetMemBuf(&m_uStorage, tCIDLib::TCard1(0), sizeof(m_uStorage));
}

TMEngOpCode::TMEngOpCode(const TMEngOpCode& meopToCopy) :

    m_eOpCode(meopToCopy.m_eOpCode)
    , m_uStorage(meopToCopy.m_uStorage)
{
}

TMEngOpCode::~TMEngOpCode()
{
}


// ---------------------------------------------------------------------------
//  TMEngOpCode: Public operators
// ---------------------------------------------------------------------------
TMEngOpCode& TMEngOpCode::operator=(const TMEngOpCode& meopToAssign)
{
    if (this != &meopToAssign)
    {
        m_eOpCode   = meopToAssign.m_eOpCode;
        m_uStorage  = meopToAssign.m_uStorage;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TMEngOpCode: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngOpCode::bConvertNumeric(const tCIDLib::TCard2 c2TargetId)
{
    //  Based on the opcode, which will be one of the immediate value types,
    //  see if we can convert it in place to fit the target numeric type.
    //
    tCIDLib::TBoolean   bRet = kCIDLib::True;
    switch(m_eOpCode)
    {
        case tCIDMacroEng::EOpCodes::PushImCard1 :
        case tCIDMacroEng::EOpCodes::PushImCard2 :
        case tCIDMacroEng::EOpCodes::PushImCard4 :
        {
            tCIDLib::TCard4 c4Val = m_uStorage.c4Immediate;
            if (m_eOpCode == tCIDMacroEng::EOpCodes::PushImCard1)
                c4Val = m_uStorage.c1Immediate;
            else if (m_eOpCode == tCIDMacroEng::EOpCodes::PushImCard2)
                c4Val = m_uStorage.c2Immediate;

            if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card1))
            &&  (c4Val <= kCIDLib::c1MaxCard))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImCard1;
                m_uStorage.c1Immediate = tCIDLib::TCard1(c4Val);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card2))
                  &&  (c4Val <= kCIDLib::c2MaxCard))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImCard2;
                m_uStorage.c2Immediate = tCIDLib::TCard2(c4Val);
            }
             else if (c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card4))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImCard4;
                m_uStorage.c4Immediate = c4Val;
            }
             else if (c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card8))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImCard8;
                m_uStorage.c8Immediate = c4Val;
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int1))
                  &&  (c4Val <= kCIDLib::i1MaxInt))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImInt1;
                m_uStorage.i1Immediate = tCIDLib::TInt1(c4Val);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int2))
                  &&  (c4Val <= kCIDLib::i2MaxInt))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImInt2;
                m_uStorage.i2Immediate = tCIDLib::TInt2(c4Val);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int4))
                  &&  (c4Val <= kCIDLib::i4MaxInt))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImInt4;
                m_uStorage.i4Immediate = tCIDLib::TInt4(c4Val);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Float4))
                  &&  (c4Val <= kCIDLib::f4MaxFloat))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImFloat4;
                m_uStorage.f4Immediate = tCIDLib::TFloat4(c4Val);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Float8))
                  &&  (c4Val <= kCIDLib::f8MaxFloat))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImFloat8;
                m_uStorage.f8Immediate = tCIDLib::TFloat8(c4Val);
            }
             else
            {
                bRet = kCIDLib::False;
            }
            break;
        }

        case tCIDMacroEng::EOpCodes::PushImCard8 :
        {
            const tCIDLib::TCard8 c8Val = m_uStorage.c8Immediate;

            if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card1))
            &&  (c8Val <= kCIDLib::c1MaxCard))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImCard1;
                m_uStorage.c1Immediate = tCIDLib::TCard1(c8Val);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card2))
                  &&  (c8Val <= kCIDLib::c2MaxCard))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImCard2;
                m_uStorage.c2Immediate = tCIDLib::TCard2(c8Val);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card4))
                  &&  (c8Val <= kCIDLib::c4MaxCard))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImCard4;
                m_uStorage.c4Immediate = tCIDLib::TCard4(c8Val);
            }
             else if (c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card8))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImCard8;
                m_uStorage.c8Immediate = c8Val;
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int1))
                  &&  (c8Val <= kCIDLib::i1MaxInt))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImInt1;
                m_uStorage.i1Immediate = tCIDLib::TInt1(c8Val);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int2))
                  &&  (c8Val <= kCIDLib::i2MaxInt))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImInt2;
                m_uStorage.i2Immediate = tCIDLib::TInt2(c8Val);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int4))
                  &&  (c8Val <= kCIDLib::i4MaxInt))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImInt4;
                m_uStorage.i4Immediate = tCIDLib::TInt4(c8Val);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Float4))
                  &&  (c8Val <= kCIDLib::f4MaxFloat))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImFloat4;
                m_uStorage.f4Immediate = tCIDLib::TFloat4(c8Val);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Float8))
                  &&  (c8Val <= kCIDLib::f8MaxFloat))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImFloat8;
                m_uStorage.f8Immediate = tCIDLib::TFloat8(c8Val);
            }
             else
            {
                bRet = kCIDLib::False;
            }
            break;
        }

        case tCIDMacroEng::EOpCodes::PushImChar :
            bRet = kCIDLib::False;
            break;

        case tCIDMacroEng::EOpCodes::PushImFloat4 :
        {
            //
            //  If we are converting to any of the cardinal/integral types,
            //  we just throw away the decimal part.
            //
            tCIDLib::TFloat4 f4Integral = 0;
            tCIDLib::TFloat4 f4Fract = 0;

            const tCIDLib::TFloat4 f4Val = m_uStorage.f4Immediate;
            f4Fract = TMathLib::f4Split(f4Val, f4Integral);

            if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card1))
            &&  (f4Integral >= 0)
            &&  (f4Integral <= tCIDLib::TFloat4(kCIDLib::c1MaxCard)))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImCard1;
                m_uStorage.c1Immediate = tCIDLib::TCard1(f4Integral);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card2))
                  &&  (f4Integral >= 0)
                  &&  (f4Integral <= tCIDLib::TFloat4(kCIDLib::c2MaxCard)))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImCard2;
                m_uStorage.c2Immediate = tCIDLib::TCard2(f4Integral);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card4))
                  &&  (f4Integral >= 0)
                  &&  (f4Integral <= tCIDLib::TFloat4(kCIDLib::c4MaxCard)))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImCard4;
                m_uStorage.c4Immediate = tCIDLib::TCard4(f4Integral);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card8))
                  &&  (f4Integral >= 0))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImCard8;
                m_uStorage.c8Immediate = tCIDLib::TCard8(f4Integral);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int1))
                  &&  (f4Integral >= tCIDLib::TFloat4(kCIDLib::i1MinInt))
                  &&  (f4Integral <= tCIDLib::TFloat4(kCIDLib::i1MaxInt)))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImInt1;
                m_uStorage.i1Immediate = tCIDLib::TInt1(f4Integral);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int2))
                  &&  (f4Integral >= tCIDLib::TFloat4(kCIDLib::i2MinInt))
                  &&  (f4Integral <= tCIDLib::TFloat4(kCIDLib::i2MaxInt)))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImInt2;
                m_uStorage.i2Immediate = tCIDLib::TInt2(f4Integral);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int4))
                  &&  (f4Integral >= tCIDLib::TFloat4(kCIDLib::i4MinInt))
                  &&  (f4Integral <= tCIDLib::TFloat4(kCIDLib::i4MaxInt)))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImInt4;
                m_uStorage.i4Immediate = tCIDLib::TInt4(f4Integral);
            }
             else if (c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Float8))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImFloat8;
                m_uStorage.f8Immediate = tCIDLib::TFloat8(f4Val);
            }
             else
            {
                bRet = kCIDLib::False;
            }
            break;
        }

        case tCIDMacroEng::EOpCodes::PushImFloat8 :
        {
            //
            //  If we are converting to any of the cardinal/integral types,
            //  we just throw away the decimal part.
            //
            tCIDLib::TFloat8 f8Integral = 0;
            tCIDLib::TFloat8 f8Fract = 0;

            const tCIDLib::TFloat8 f8Val = m_uStorage.f8Immediate;
            f8Fract = TMathLib::f8Split(f8Val, f8Integral);

            if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card1))
            &&  (f8Integral >= 0)
            &&  (f8Integral <= tCIDLib::TFloat8(kCIDLib::c1MaxCard)))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImCard1;
                m_uStorage.c1Immediate = tCIDLib::TCard1(f8Integral);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card2))
                  &&  (f8Integral >= 0)
                  &&  (f8Integral <= tCIDLib::TFloat8(kCIDLib::c2MaxCard)))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImCard2;
                m_uStorage.c2Immediate = tCIDLib::TCard2(f8Integral);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card4))
                  &&  (f8Integral >= 0)
                  &&  (f8Integral <= tCIDLib::TFloat8(kCIDLib::c4MaxCard)))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImCard4;
                m_uStorage.c4Immediate = tCIDLib::TCard4(f8Integral);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card8))
                  &&  (f8Integral >= 0)
                  &&  (f8Integral <= tCIDLib::TFloat8(kCIDLib::c8MaxCard)))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImCard8;
                m_uStorage.c8Immediate = tCIDLib::TCard8(f8Integral);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int1))
                  &&  (f8Integral >= tCIDLib::TFloat8(kCIDLib::i1MinInt))
                  &&  (f8Integral <= tCIDLib::TFloat8(kCIDLib::i1MaxInt)))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImInt1;
                m_uStorage.i1Immediate = tCIDLib::TInt1(f8Integral);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int2))
                  &&  (f8Integral >= tCIDLib::TFloat8(kCIDLib::i2MinInt))
                  &&  (f8Integral <= tCIDLib::TFloat8(kCIDLib::i2MaxInt)))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImInt2;
                m_uStorage.i2Immediate = tCIDLib::TInt2(f8Integral);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int4))
                  &&  (f8Integral >= tCIDLib::TFloat8(kCIDLib::i4MinInt))
                  &&  (f8Integral <= tCIDLib::TFloat8(kCIDLib::i4MaxInt)))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImInt4;
                m_uStorage.i4Immediate = tCIDLib::TInt4(f8Integral);
            }
             else if (c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Float4))
            {
                if ((f8Val >= -kCIDLib::f4MaxFloat)
                ||  (f8Val <= kCIDLib::f4MaxFloat))
                {
                    m_eOpCode = tCIDMacroEng::EOpCodes::PushImFloat4;
                    m_uStorage.f4Immediate = tCIDLib::TFloat4(f8Val);
                }
                 else
                {
                    bRet = kCIDLib::False;
                }
            }
             else
            {
                bRet = kCIDLib::False;
            }
            break;
        }

        case tCIDMacroEng::EOpCodes::PushImInt1 :
        case tCIDMacroEng::EOpCodes::PushImInt2 :
        case tCIDMacroEng::EOpCodes::PushImInt4 :
        {
            tCIDLib::TInt4 i4Val = m_uStorage.i4Immediate;
            if (m_eOpCode == tCIDMacroEng::EOpCodes::PushImInt1)
                i4Val = m_uStorage.i1Immediate;
            else if (m_eOpCode == tCIDMacroEng::EOpCodes::PushImInt2)
                i4Val = m_uStorage.i2Immediate;

            if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card1))
            &&  (i4Val >= 0) && (i4Val <= kCIDLib::c1MaxCard))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImCard1;
                m_uStorage.c1Immediate = tCIDLib::TCard1(i4Val);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card2))
                  && (i4Val >= 0) && (i4Val < kCIDLib::c2MaxCard))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImCard2;
                m_uStorage.c2Immediate = tCIDLib::TCard2(i4Val);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card4))
                  && (i4Val >= 0))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImCard4;
                m_uStorage.c4Immediate = tCIDLib::TCard4(i4Val);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card8))
                  &&  (i4Val >= 0))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImCard8;
                m_uStorage.c8Immediate = tCIDLib::TCard8(i4Val);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int1))
                  &&  (i4Val >= kCIDLib::i1MinInt)
                  &&  (i4Val <= kCIDLib::i1MaxInt))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImInt1;
                m_uStorage.i1Immediate = tCIDLib::TInt1(i4Val);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int2))
                  &&  (i4Val >= kCIDLib::i2MinInt)
                  &&  (i4Val <= kCIDLib::i2MaxInt))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImInt2;
                m_uStorage.i2Immediate = tCIDLib::TInt2(i4Val);
            }
             else if (c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int4))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImInt4;
                m_uStorage.i4Immediate = i4Val;
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Float4))
                  &&  (i4Val >= -kCIDLib::f4MaxFloat)
                  &&  (i4Val <= kCIDLib::f4MaxFloat))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImFloat4;
                m_uStorage.f4Immediate = tCIDLib::TFloat4(i4Val);
            }
             else if ((c2TargetId == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Float8))
                  &&  (i4Val >= -kCIDLib::f8MaxFloat)
                  &&  (i4Val <= kCIDLib::f8MaxFloat))
            {
                m_eOpCode = tCIDMacroEng::EOpCodes::PushImFloat8;
                m_uStorage.f8Immediate = tCIDLib::TFloat8(i4Val);
            }
             else
            {
                bRet = kCIDLib::False;
            }
            break;
        }

        default :
            // This shouldn't happen
            facCIDMacroEng().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcEng_NotImNumOpCode
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
                , TInteger(tCIDLib::i4EnumOrd(eOpCode()))
            );
            break;
    }
    return bRet;
}


tCIDLib::TCard4 TMEngOpCode::c4Immediate(const tCIDLib::TCard4 c4ToSet)
{
    m_uStorage.c4Immediate = c4ToSet;
    return m_uStorage.c4Immediate;
}


tCIDLib::TVoid
TMEngOpCode::Format(TTextOutStream& strmTarget, const TCIDMacroEngine& meOwner) const
{
    //
    //  Write out the opcode itself. We want each one to be left justified
    //  in a field big enough to keep them corectly aligned.
    //
    static const TStreamFmt sfmtOpCode
    (
        16
        , 0
        , tCIDLib::EHJustify::Left
        , kCIDLib::chSpace
    );
    static const TStreamFmt sfmtData
    (
        8
        , 4
        , tCIDLib::EHJustify::Right
        , kCIDLib::chSpace
    );

    static const TStreamFmt sfmtText
    (
        0
        , 0
        , tCIDLib::EHJustify::Left
        , kCIDLib::chSpace
    );

    // Retain the original stream formatting
    TStreamJanitor janStream(&strmTarget);

    // Put put the opcode with the opcode format
    strmTarget << sfmtOpCode << m_eOpCode;

    // Group the one's that share common data, and show it
    switch(m_eOpCode)
    {
        case tCIDMacroEng::EOpCodes::CallLocal :
        case tCIDMacroEng::EOpCodes::CallMember :
        case tCIDMacroEng::EOpCodes::CallParm :
        case tCIDMacroEng::EOpCodes::CallStack :
        case tCIDMacroEng::EOpCodes::PushEnum :
        {
            strmTarget  << sfmtData
                        << m_uStorage.ac2Indices[0]
                        << m_uStorage.ac2Indices[1];
            break;
        }

        case tCIDMacroEng::EOpCodes::CondJump :
        case tCIDMacroEng::EOpCodes::CondJumpNP :
        case tCIDMacroEng::EOpCodes::CurLine :
        case tCIDMacroEng::EOpCodes::MultiPop :
        case tCIDMacroEng::EOpCodes::NotCondJump :
        case tCIDMacroEng::EOpCodes::NotCondJumpNP :
        case tCIDMacroEng::EOpCodes::PushImCard4 :
        case tCIDMacroEng::EOpCodes::Repush :
        case tCIDMacroEng::EOpCodes::ThrowFmt :
        case tCIDMacroEng::EOpCodes::Try :
            strmTarget << sfmtData << m_uStorage.c4Immediate;
            break;

        case tCIDMacroEng::EOpCodes::Jump :
        case tCIDMacroEng::EOpCodes::PushImCard2 :
            strmTarget << sfmtData << m_uStorage.c2Immediate;
            break;

        case tCIDMacroEng::EOpCodes::Throw :
        case tCIDMacroEng::EOpCodes::PushImBoolean :
            strmTarget << sfmtData << m_uStorage.bImmediate;
            break;

        case tCIDMacroEng::EOpCodes::PushImCard1 :
            strmTarget << sfmtData << m_uStorage.c1Immediate;
            break;

        case tCIDMacroEng::EOpCodes::PushImCard8 :
            strmTarget << sfmtData << m_uStorage.c8Immediate;
            break;

        case tCIDMacroEng::EOpCodes::PushImChar :
            strmTarget << sfmtData << m_uStorage.chImmediate;
            break;

        case tCIDMacroEng::EOpCodes::PushImFloat4 :
            strmTarget << sfmtData << m_uStorage.f4Immediate;
            break;

        case tCIDMacroEng::EOpCodes::PushImFloat8 :
            strmTarget << sfmtData << m_uStorage.f8Immediate;
            break;

        case tCIDMacroEng::EOpCodes::PushImInt1 :
            strmTarget << sfmtData << m_uStorage.i1Immediate;
            break;

        case tCIDMacroEng::EOpCodes::PushImInt2 :
            strmTarget << sfmtData << m_uStorage.i2Immediate;
            break;

        case tCIDMacroEng::EOpCodes::PushImInt4 :
            strmTarget << sfmtData << m_uStorage.i4Immediate;
            break;

        case tCIDMacroEng::EOpCodes::CallExcept :
        case tCIDMacroEng::EOpCodes::CallParent :
        case tCIDMacroEng::EOpCodes::CallThis :
        case tCIDMacroEng::EOpCodes::PushLocal :
        case tCIDMacroEng::EOpCodes::PushMember :
        case tCIDMacroEng::EOpCodes::PushParm :
        case tCIDMacroEng::EOpCodes::PushStrPoolItem :
        case tCIDMacroEng::EOpCodes::PushTempConst :
        case tCIDMacroEng::EOpCodes::PushTempVar :
        case tCIDMacroEng::EOpCodes::TableJump :
        case tCIDMacroEng::EOpCodes::TypeCast :
            strmTarget << sfmtData << m_uStorage.ac2Indices[0];
            break;

        case tCIDMacroEng::EOpCodes::ColIndex :
        case tCIDMacroEng::EOpCodes::Copy :
        case tCIDMacroEng::EOpCodes::CondEnumInc :
        case tCIDMacroEng::EOpCodes::EndTry :
        case tCIDMacroEng::EOpCodes::FlipTop :
        case tCIDMacroEng::EOpCodes::LogicalAnd :
        case tCIDMacroEng::EOpCodes::LogicalOr :
        case tCIDMacroEng::EOpCodes::LogicalXor :
        case tCIDMacroEng::EOpCodes::Negate :
        case tCIDMacroEng::EOpCodes::NoOp :
        case tCIDMacroEng::EOpCodes::PopTop :
        case tCIDMacroEng::EOpCodes::PopToReturn :
        case tCIDMacroEng::EOpCodes::PushCurLine :
        case tCIDMacroEng::EOpCodes::PushException :
        case tCIDMacroEng::EOpCodes::PushThis :
        case tCIDMacroEng::EOpCodes::ResetEnum :
        case tCIDMacroEng::EOpCodes::Return :
            // No data
            break;

        default :
            strmTarget << L"???";
            break;
    }
}


tCIDLib::TVoid
TMEngOpCode::SetOpCode(const tCIDMacroEng::EOpCodes eOpCode)
{
    TRawMem::SetMemBuf(&m_uStorage, tCIDLib::TCard1(0), sizeof(m_uStorage));
    m_eOpCode = eOpCode;
}


tCIDLib::TVoid
TMEngOpCode::SetSingleIndex(const   tCIDMacroEng::EOpCodes eOpCode
                                , const tCIDLib::TCard2     c2Index)
{
    TRawMem::SetMemBuf(&m_uStorage, tCIDLib::TCard1(0), sizeof(m_uStorage));
    m_eOpCode = eOpCode;
    m_uStorage.ac2Indices[0] = c2Index;
}


tCIDLib::TVoid
TMEngOpCode::SetDoubleIndex(const   tCIDMacroEng::EOpCodes eOpCode
                                , const tCIDLib::TCard2     c2Index1
                                , const tCIDLib::TCard2     c2Index2)
{
    TRawMem::SetMemBuf(&m_uStorage, tCIDLib::TCard1(0), sizeof(m_uStorage));
    m_eOpCode = eOpCode;
    m_uStorage.ac2Indices[0] = c2Index1;
    m_uStorage.ac2Indices[1] = c2Index2;
}


tCIDLib::TVoid
TMEngOpCode::SetImmediate(  const   tCIDMacroEng::EOpCodes eOpCode
                            , const tCIDLib::TBoolean   bImmediate)
{
    TRawMem::SetMemBuf(&m_uStorage, tCIDLib::TCard1(0), sizeof(m_uStorage));
    m_eOpCode = eOpCode;
    m_uStorage.bImmediate = bImmediate;
}


tCIDLib::TVoid
TMEngOpCode::SetImmediate(  const   tCIDMacroEng::EOpCodes eOpCode
                            , const tCIDLib::TCard1     c1Immediate)
{
    TRawMem::SetMemBuf(&m_uStorage, tCIDLib::TCard1(0), sizeof(m_uStorage));
    m_eOpCode = eOpCode;
    m_uStorage.c1Immediate = c1Immediate;
}

tCIDLib::TVoid
TMEngOpCode::SetImmediate(  const   tCIDMacroEng::EOpCodes eOpCode
                            , const tCIDLib::TCard2     c2Immediate)
{
    TRawMem::SetMemBuf(&m_uStorage, tCIDLib::TCard1(0), sizeof(m_uStorage));
    m_eOpCode = eOpCode;
    m_uStorage.c2Immediate = c2Immediate;
}

tCIDLib::TVoid
TMEngOpCode::SetImmediate(  const   tCIDMacroEng::EOpCodes eOpCode
                            , const tCIDLib::TCard4     c4Immediate)
{
    TRawMem::SetMemBuf(&m_uStorage, tCIDLib::TCard1(0), sizeof(m_uStorage));
    m_eOpCode = eOpCode;
    m_uStorage.c4Immediate = c4Immediate;
}

tCIDLib::TVoid
TMEngOpCode::SetImmediate(  const   tCIDMacroEng::EOpCodes eOpCode
                            , const tCIDLib::TCard8     c8Immediate)
{
    TRawMem::SetMemBuf(&m_uStorage, tCIDLib::TCard1(0), sizeof(m_uStorage));
    m_eOpCode = eOpCode;
    m_uStorage.c8Immediate = c8Immediate;
}


tCIDLib::TVoid
TMEngOpCode::SetImmediate(  const   tCIDMacroEng::EOpCodes eOpCode
                            , const tCIDLib::TCh        chImmediate)
{
    TRawMem::SetMemBuf(&m_uStorage, tCIDLib::TCard1(0), sizeof(m_uStorage));
    m_eOpCode = eOpCode;
    m_uStorage.chImmediate = chImmediate;
}


tCIDLib::TVoid
TMEngOpCode::SetImmediate(  const   tCIDMacroEng::EOpCodes eOpCode
                            , const tCIDLib::TFloat4    f4Immediate)
{
    TRawMem::SetMemBuf(&m_uStorage, tCIDLib::TCard1(0), sizeof(m_uStorage));
    m_eOpCode = eOpCode;
    m_uStorage.f4Immediate = f4Immediate;
}

tCIDLib::TVoid
TMEngOpCode::SetImmediate(  const   tCIDMacroEng::EOpCodes eOpCode
                            , const tCIDLib::TFloat8    f8Immediate)
{
    TRawMem::SetMemBuf(&m_uStorage, tCIDLib::TCard1(0), sizeof(m_uStorage));
    m_eOpCode = eOpCode;
    m_uStorage.f8Immediate = f8Immediate;
}

tCIDLib::TVoid
TMEngOpCode::SetImmediate(  const   tCIDMacroEng::EOpCodes eOpCode
                            , const tCIDLib::TInt1      i1Immediate)
{
    TRawMem::SetMemBuf(&m_uStorage, tCIDLib::TCard1(0), sizeof(m_uStorage));
    m_eOpCode = eOpCode;
    m_uStorage.i1Immediate = i1Immediate;
}

tCIDLib::TVoid
TMEngOpCode::SetImmediate(  const   tCIDMacroEng::EOpCodes eOpCode
                            , const tCIDLib::TInt2      i2Immediate)
{
    TRawMem::SetMemBuf(&m_uStorage, tCIDLib::TCard1(0), sizeof(m_uStorage));
    m_eOpCode = eOpCode;
    m_uStorage.i2Immediate = i2Immediate;
}

tCIDLib::TVoid
TMEngOpCode::SetImmediate(  const   tCIDMacroEng::EOpCodes eOpCode
                            , const tCIDLib::TInt4      i4Immediate)
{
    TRawMem::SetMemBuf(&m_uStorage, tCIDLib::TCard1(0), sizeof(m_uStorage));
    m_eOpCode = eOpCode;
    m_uStorage.i4Immediate = i4Immediate;
}


// ---------------------------------------------------------------------------
//  TMEngOpCode: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngOpCode::BadIndex(const tCIDLib::TCard4 c4Index) const
{
    facCIDMacroEng().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kMEngErrs::errcMeth_BadOpcodeIndex
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Index
        , TCardinal(c4Index)
    );
}


