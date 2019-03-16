//
// FILE NAME: CIDGraphDev_Device.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/27/1997
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
//  This file implements the TGraphicDevice class, which is a wrapper class
//  for the host OS' graphics output device handle.
//
//  A derivative is provided, TGraphDrawDev, that is the base for all devices
//  that can be drawn on.
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
#include    "CIDGraphDev_.hpp"


tCIDLib::TCard4
TGrDevPlatform::c4XlatBackMixMode(const tCIDGraphDev::EBackMixModes eMode)
{
    tCIDLib::TCard4 c4Ret = 0;
    if (eMode == tCIDGraphDev::EBackMixModes::Opaque)
        c4Ret = OPAQUE;
    else if (eMode == tCIDGraphDev::EBackMixModes::Transparent)
        c4Ret = TRANSPARENT;
    #if CID_DEBUG_ON
    else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(tCIDLib::c4EnumOrd(eMode))
            , TString(L"tCIDGraphDev::EBackMixModes")
        );
    }
    #endif
    return c4Ret;
}


tCIDLib::TCard4 TGrDevPlatform::c4XlatClipMode(const tCIDGraphDev::EClipModes eMode)
{
    tCIDLib::TCard4 c4Ret = 0;
    switch(eMode)
    {
        case tCIDGraphDev::EClipModes::And :
            c4Ret = RGN_AND;
            break;

        case tCIDGraphDev::EClipModes::Copy :
            c4Ret = RGN_COPY;
            break;

        case tCIDGraphDev::EClipModes::Diff :
            c4Ret = RGN_DIFF;
            break;

        case tCIDGraphDev::EClipModes::Or :
            c4Ret = RGN_OR;
            break;

        case tCIDGraphDev::EClipModes::Xor :
            c4Ret = RGN_XOR;
            break;

        default :
            #if CID_DEBUG_ON
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcGen_BadEnumValue
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::BadParms
                , TCardinal(tCIDLib::c4EnumOrd(eMode))
                , TString(L"tCIDGraphDev::EClipModes")
            );
            #endif
            break;
    }
    return c4Ret;
}


tCIDLib::TCard4 TGrDevPlatform::c4XlatEndStyle(const tCIDGraphDev::ELineEnds eEnd)
{
    tCIDLib::TCard4 c4Ret = 0;
    switch(eEnd)
    {
        case tCIDGraphDev::ELineEnds::Flat :
            c4Ret = PS_ENDCAP_FLAT;
            break;

        case tCIDGraphDev::ELineEnds::Square :
            c4Ret = PS_ENDCAP_SQUARE;
            break;

        case tCIDGraphDev::ELineEnds::Round :
            c4Ret = PS_ENDCAP_ROUND;
            break;

        default :
            #if CID_DEBUG_ON
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcGen_BadEnumValue
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::BadParms
                , TCardinal(tCIDLib::c4EnumOrd(eEnd))
                , TString(L"tCIDGraphDev::ELineEnds")
            );
            #endif
            break;
    }
    return c4Ret;
}


tCIDLib::TCard4 TGrDevPlatform::c4XlatLineStyle(const tCIDGraphDev::ELineStyles eStyle)
{
    tCIDLib::TCard4 c4Ret = 0;
    switch(eStyle)
    {
        case tCIDGraphDev::ELineStyles::Solid :
            c4Ret = PS_SOLID;
            break;

        case tCIDGraphDev::ELineStyles::Dot :
            c4Ret = PS_DOT;
            break;

        case tCIDGraphDev::ELineStyles::Dash :
            c4Ret = PS_DASH;
            break;

        case tCIDGraphDev::ELineStyles::DashDot :
            c4Ret = PS_DASHDOT;
            break;

        case tCIDGraphDev::ELineStyles::DashDoubleDot :
            c4Ret = PS_DASHDOTDOT;
            break;

        case tCIDGraphDev::ELineStyles::Invisible :
            c4Ret = PS_NULL;
            break;

        case tCIDGraphDev::ELineStyles::Alternate :
            c4Ret = PS_ALTERNATE;
            break;

        default :
            #if CID_DEBUG_ON
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcGen_BadEnumValue
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::BadParms
                , TCardinal(tCIDLib::c4EnumOrd(eStyle))
                , TString(L"tCIDGraphDev::ELineStyles")
            );
            #endif
            break;
    }
    return c4Ret;
}


tCIDLib::TCard4 TGrDevPlatform::c4XlatJoinStyle(const tCIDGraphDev::ELineJoins eJoin)
{
    tCIDLib::TCard4 c4Ret = 0;
    switch(eJoin)
    {
        case tCIDGraphDev::ELineJoins::Bevel :
            c4Ret = PS_JOIN_BEVEL;
            break;

        case tCIDGraphDev::ELineJoins::Round :
            c4Ret = PS_JOIN_ROUND;
            break;

        case tCIDGraphDev::ELineJoins::Mitre :
            c4Ret = PS_JOIN_MITER;
            break;

        default :
            #if CID_DEBUG_ON
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcGen_BadEnumValue
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::BadParms
                , TCardinal(tCIDLib::c4EnumOrd(eJoin))
                , TString(L"tCIDGraphDev::ELineJoins")
            );
            #endif
            break;
    }
    return c4Ret;
}


tCIDLib::TCard4 TGrDevPlatform::c4XlatBmpMode(const tCIDGraphDev::EBmpModes eMode)
{
    tCIDLib::TCard4 c4Ret = 0;
    switch(eMode)
    {
        case tCIDGraphDev::EBmpModes::Blackness :
            c4Ret = BLACKNESS;
            break;

        case tCIDGraphDev::EBmpModes::CaptureBlt :
            c4Ret = CAPTUREBLT;
            break;

        case tCIDGraphDev::EBmpModes::DstInvert :
            c4Ret = DSTINVERT;
            break;

        case tCIDGraphDev::EBmpModes::MergeCopy :
            c4Ret = MERGECOPY;
            break;

        case tCIDGraphDev::EBmpModes::MergePaint :
            c4Ret = MERGEPAINT;
            break;

        case tCIDGraphDev::EBmpModes::NoMirrorBitmap :
            c4Ret = NOMIRRORBITMAP;
            break;

        case tCIDGraphDev::EBmpModes::NotSrcCopy :
            c4Ret = NOTSRCCOPY;
            break;

        case tCIDGraphDev::EBmpModes::NotSrcErase :
            c4Ret = NOTSRCERASE;
            break;

        case tCIDGraphDev::EBmpModes::PatCopy :
            c4Ret = PATCOPY;
            break;

        case tCIDGraphDev::EBmpModes::PatInvert :
            c4Ret = PATINVERT;
            break;

        case tCIDGraphDev::EBmpModes::PatPaint :
            c4Ret = PATPAINT;
            break;

        case tCIDGraphDev::EBmpModes::SrcAnd :
            c4Ret = SRCAND;
            break;

        case tCIDGraphDev::EBmpModes::SrcCopy :
            c4Ret = SRCCOPY;
            break;

        case tCIDGraphDev::EBmpModes::SrcErase :
            c4Ret = SRCERASE;
            break;

        case tCIDGraphDev::EBmpModes::SrcInvert :
            c4Ret = SRCINVERT;
            break;

        case tCIDGraphDev::EBmpModes::SrcPaint :
            c4Ret = SRCPAINT;
            break;

        case tCIDGraphDev::EBmpModes::Whiteness :
            c4Ret = WHITENESS;
            break;

        default :
            #if CID_DEBUG_ON
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcGen_BadEnumValue
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::BadParms
                , TCardinal(tCIDLib::c4EnumOrd(eMode))
                , TString(L"tCIDGraphDev::EBmpModes")
            );
            #endif
            break;
    }
    return c4Ret;
}


tCIDLib::TCard4 TGrDevPlatform::c4XlatMixMode(const tCIDGraphDev::EMixModes eMode)
{
    tCIDLib::TCard4 c4Ret = 0;
    switch(eMode)
    {
        case tCIDGraphDev::EMixModes::Black :
            c4Ret = R2_BLACK;
            break;

        case tCIDGraphDev::EMixModes::CopyPen :
            c4Ret = R2_COPYPEN;
            break;

        case tCIDGraphDev::EMixModes::MaskNotPen :
            c4Ret = R2_MASKNOTPEN;
            break;

        case tCIDGraphDev::EMixModes::MaskPen :
            c4Ret = R2_MASKPEN;
            break;

        case tCIDGraphDev::EMixModes::MaskPenNot :
            c4Ret = R2_MASKPENNOT;
            break;

        case tCIDGraphDev::EMixModes::MergeNotPen :
            c4Ret = R2_MERGENOTPEN;
            break;

        case tCIDGraphDev::EMixModes::MergePen :
            c4Ret = R2_MERGEPEN;
            break;

        case tCIDGraphDev::EMixModes::MergePenNot :
            c4Ret = R2_MERGEPENNOT;
            break;

        case tCIDGraphDev::EMixModes::NOP :
            c4Ret = R2_NOP;
            break;

        case tCIDGraphDev::EMixModes::Not :
            c4Ret = R2_NOT;
            break;

        case tCIDGraphDev::EMixModes::NotCopyPen :
            c4Ret = R2_NOTCOPYPEN;
            break;

        case tCIDGraphDev::EMixModes::NotMaskPen :
            c4Ret = R2_NOTMASKPEN;
            break;

        case tCIDGraphDev::EMixModes::NotMergePen :
            c4Ret = R2_NOTMERGEPEN;
            break;

        case tCIDGraphDev::EMixModes::NotXorPen :
            c4Ret = R2_NOTXORPEN;
            break;

        case tCIDGraphDev::EMixModes::White :
            c4Ret = R2_WHITE;
            break;

         case tCIDGraphDev::EMixModes::XorPen :
            c4Ret = R2_XORPEN;
            break;

        default :
            #if CID_DEBUG_ON
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcGen_BadEnumValue
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::BadParms
                , TCardinal(tCIDLib::c4EnumOrd(eMode))
                , TString(L"tCIDGraphDev::EMixModes")
            );
            #endif
            break;
    }
    return c4Ret;
}


tCIDLib::TCard4 TGrDevPlatform::c4XlatPattern(const tCIDGraphDev::EPatterns ePattern)
{
    tCIDLib::TCard4 c4Ret = 0;
    switch(ePattern)
    {
        case tCIDGraphDev::EPatterns::BackDiagonal :
            c4Ret = HS_BDIAGONAL;
            break;

        case tCIDGraphDev::EPatterns::Cross :
            c4Ret = HS_CROSS;
            break;

        case tCIDGraphDev::EPatterns::DiagonalCross :
            c4Ret = HS_DIAGCROSS;
            break;

        case tCIDGraphDev::EPatterns::ForwardDiagonal :
            c4Ret = HS_FDIAGONAL;
            break;

        case tCIDGraphDev::EPatterns::Horizontal :
            c4Ret = HS_HORIZONTAL;
            break;

        case tCIDGraphDev::EPatterns::Vertical :
            c4Ret = HS_VERTICAL;
            break;

        case tCIDGraphDev::EPatterns::Solid :
            c4Ret = 0;
            break;

        default :
            #if CID_DEBUG_ON
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcGen_BadEnumValue
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::BadParms
                , TCardinal(tCIDLib::c4EnumOrd(ePattern))
                , TString(L"tCIDGraphDev::EPatterns")
            );
            #endif
            break;
    }
    return c4Ret;
}


tCIDGraphDev::EBackMixModes
TGrDevPlatform::eXlatBackMixMode(const tCIDLib::TCard4 c4Mode)
{
    tCIDGraphDev::EBackMixModes eMode = tCIDGraphDev::EBackMixModes::Opaque;
    if (c4Mode == OPAQUE)
        eMode = tCIDGraphDev::EBackMixModes::Opaque;
    else if (c4Mode == TRANSPARENT)
        eMode = tCIDGraphDev::EBackMixModes::Transparent;
    #if CID_DEBUG_ON
    else
    {
        facCIDGraphDev().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcPlat_CannotXlat
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(tCIDLib::c4EnumOrd(c4Mode))
            , TString(L"tCIDGraphDev::EBackMixModes")
        );
    }
    #endif
    return eMode;
}


tCIDGraphDev::EMixModes TGrDevPlatform::eXlatMixMode(const tCIDLib::TCard4 c4Mode)
{
    tCIDGraphDev::EMixModes eRet;
    switch(c4Mode)
    {
        case R2_BLACK :
            eRet = tCIDGraphDev::EMixModes::Black;
            break;

        case R2_COPYPEN :
            eRet = tCIDGraphDev::EMixModes::CopyPen;
            break;

        case R2_MASKNOTPEN :
            eRet = tCIDGraphDev::EMixModes::MaskNotPen;
            break;

        case R2_MASKPEN :
            eRet = tCIDGraphDev::EMixModes::MaskPen;
            break;

        case R2_MASKPENNOT :
            eRet = tCIDGraphDev::EMixModes::MaskPenNot;
            break;

        case R2_MERGENOTPEN :
            eRet = tCIDGraphDev::EMixModes::MergeNotPen;
            break;

        case R2_MERGEPEN :
            eRet = tCIDGraphDev::EMixModes::MergePen;
            break;

        case R2_MERGEPENNOT :
            eRet = tCIDGraphDev::EMixModes::MergePenNot;
            break;

        case R2_NOP :
            eRet = tCIDGraphDev::EMixModes::NOP;
            break;

        case R2_NOT :
            eRet = tCIDGraphDev::EMixModes::Not;
            break;

        case R2_NOTCOPYPEN :
            eRet = tCIDGraphDev::EMixModes::NotCopyPen;
            break;

        case R2_NOTMASKPEN :
            eRet = tCIDGraphDev::EMixModes::NotMaskPen;
            break;

        case R2_NOTMERGEPEN :
            eRet = tCIDGraphDev::EMixModes::NotMergePen;
            break;

        case R2_NOTXORPEN :
            eRet = tCIDGraphDev::EMixModes::NotXorPen;
            break;

        case R2_WHITE :
            eRet = tCIDGraphDev::EMixModes::White;
            break;

         case R2_XORPEN  :
            eRet = tCIDGraphDev::EMixModes::XorPen;
            break;

        default :
            // Make compiler happy
            eRet = tCIDGraphDev::EMixModes::XorPen;

            #if CID_DEBUG_ON
            facCIDGraphDev().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcPlat_CannotXlat
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::BadParms
                , TCardinal(tCIDLib::c4EnumOrd(c4Mode))
                , TString(L"tCIDGraphDev::EMixModes")
            );
            #endif
            break;
    };
    return eRet;
}

