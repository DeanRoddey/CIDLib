//
// FILE NAME: CIDImage_Type.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/21/1998
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
//  This file provides formatting for the facility specific enums from
//  CIDImage_Type.hpp.
//
// CAVEATS/GOTCHAS:
//
//  1)  Some enums are very commonly understood and therefore will be used
//      in very non-software senses, such as boolean values. Therefore, the
//      text for those types are loaded dyanmically in order to support
//      translation for them. Others are very tied to the source code and
//      anyone expected to understand the code (which is in english) would
//      understand them in their english form.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDImage_.hpp"



// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDImage::EBmpTypes eType)
{
    if (eType == tCIDImage::EBmpTypes::Win_1x)
        strmToWriteTo << L"Win 1.x";
    else if (eType == tCIDImage::EBmpTypes::Win_2x)
        strmToWriteTo << L"Win 2.x";
    else if (eType == tCIDImage::EBmpTypes::Win_3x)
        strmToWriteTo << L"Win 3.x";
    else if (eType == tCIDImage::EBmpTypes::Win_4x)
        strmToWriteTo << L"Win 4.x";
    else if (eType == tCIDImage::EBmpTypes::OS2_2x)
        strmToWriteTo << L"OS/2 2.x";
    else
    {
        // We are throwing a CIDLib error here, not one of ours!
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eType))
            , TString(L"tCIDImage::EBmpTypes")
        );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDImage::EBmpComp eCompress)
{
    if (eCompress == tCIDImage::EBmpComp::None)
        strmToWriteTo << L"None";
    else if (eCompress == tCIDImage::EBmpComp::RLE8)
        strmToWriteTo << L"RLE8";
    else if (eCompress == tCIDImage::EBmpComp::RLE4)
        strmToWriteTo << L"RLE4";
    else if (eCompress == tCIDImage::EBmpComp::RLE24)
        strmToWriteTo << L"RLE24";
    else if (eCompress == tCIDImage::EBmpComp::Huffman1D)
        strmToWriteTo << L"Huffman1D";
    else if (eCompress == tCIDImage::EBmpComp::BitFields)
        strmToWriteTo << L"Bitfields";
    else
    {
        // We are throwing a CIDLib error here, not one of ours!
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eCompress))
            , TString(L"tCIDImage::EBmpComp")
        );
    }
    return strmToWriteTo;
}
