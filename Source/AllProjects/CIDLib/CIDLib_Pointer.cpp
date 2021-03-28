//
// FILE NAME: CIDLib_Pointer.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/02/1997
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
//  This file implements the TPointer class. Most of it is inline because
//  its very simple.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard RTTI macros
// ---------------------------------------------------------------------------
AdvRTTIDecls(TPointer,TObject)


// ---------------------------------------------------------------------------
//   CLASS: TPointer
//  PREFIX: ptr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TPointer: Constructors and Destructor
// ---------------------------------------------------------------------------
TPointer::TPointer(const tCIDLib::TVoid* const pPointer) :

    m_pData(pPointer)
{
}


// ---------------------------------------------------------------------------
//  TPointer: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TPointer::operator==(const TPointer& ptrToCompare) const
{
    return (m_pData == ptrToCompare.m_pData);
}

tCIDLib::TBoolean TPointer::operator!=(const TPointer& ptrToCompare) const
{
    return (m_pData != ptrToCompare.m_pData);
}


// ---------------------------------------------------------------------------
//  TPointer: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TPointer::FormatTo(CIOP TTextOutStream& strmToWriteTo) const
{
    //
    //  Do the conversion into a temporary string. Make it way overly big
    //  to account for other architctures in the future. We use a low level
    //  converion here just because its the only thing this class does so
    //  its no big burden, and makes it faster.
    //
    tCIDLib::TZStr32 szTmp;
    if (!TRawStr::bFormatVal(tCIDLib::TCard4(m_pData), szTmp, 32, tCIDLib::ERadices::Hex))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcLoc_NumericFormat
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // And append the formatted string to the passed stream
    strmToWriteTo << szTmp;
}


