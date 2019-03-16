//
// FILE NAME: CIDCtrls_InPlaceEdit.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/06/2015
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
//  This file implements the
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
#include    "CIDCtrls_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TInPlaceEdit, TWindow)



// ---------------------------------------------------------------------------
//  CLASS: MIPEIntf
// PREFIX: ipev
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  MEPIntf: Public, static method
// ---------------------------------------------------------------------------

tCIDLib::TBoolean MIPEIntf::bTestEnumVal(const  TAttrData&  adatTest
                                        ,       TString&    strCanonVal)
{
    TString strEnum = adatTest.strLimits();
    if (!strEnum.bStartsWithI(kCIDMData::strAttrLim_EnumPref))
        return kCIDLib::True;

    strEnum.Cut(0, kCIDMData::strAttrLim_EnumPref.c4Length());

    const TString& strTestVal = adatTest.strValue();
    TStringTokenizer stokVals(&strEnum, L",");
    TString strToken;
    TString strFirstToken;
    tCIDLib::TBoolean bMatch = kCIDLib::False;
    while (stokVals.bGetNextToken(strToken))
    {
        strToken.StripWhitespace();

        // Remember the first token in case we have to provide a default
        if (strFirstToken.bIsEmpty())
            strFirstToken = strToken;

        if (strTestVal.bCompareI(strToken))
        {
            // Give back the canonical form
            strCanonVal = strToken;
            bMatch = kCIDLib::True;
            break;
        }
    }

    // If no match, return the first token as a new value
    if (!bMatch)
        strCanonVal = strFirstToken;

    return bMatch;
}


// ---------------------------------------------------------------------------
//  MEPIntf: Constructors and Destructor
// ---------------------------------------------------------------------------
MIPEIntf::MIPEIntf() :

    m_c4IPECurRow(0)
{
}

MIPEIntf::~MIPEIntf()
{
}


// ---------------------------------------------------------------------------
//  MEPIntf: Public, virtual methods
// ---------------------------------------------------------------------------

//
//  At this level we handle all of the core type validation. The derived class can
//  add more. It can call us and see if we are happy with it or not, then do further
//  validation or handle its own types.
//
//  Note that, this is for in place editing, so the value is always a text string
//  in his case, not in the type specific format within the attribute data object.
//  We have to parse the values out.
//
//  If we can parse a valid value out, we store it on the attribute object and return
//  true.
//
//  The user id can be updated for the final call to the value changed callback, but
//  we don't do that here. That's something for the actual application to do if he
//  wants to.
//
tCIDLib::TBoolean
MIPEIntf::bIPEValidate( const   TString&            strSrc
                        ,       TAttrData&          adatVal
                        , const TString&            strNewVal
                        ,       TString&            strErrMsg
                        ,       tCIDLib::TCard8&    c8UserId) const
{
    if (adatVal.eType() == tCIDMData::EAttrTypes::AppImage)
    {
        // It can be empty (to be loaded dynmically later)
        if (!strNewVal.bIsEmpty())
        {
            TString strToLoad;
            const tCIDCtrls::EAppImgSz eSz = TDlgItem::eParseAppImage
            (
                strNewVal, strToLoad
            );

            if (!facCIDCtrls().bAppImageExists(strToLoad, eSz))
            {
                strErrMsg = L"The named app image does not exist";
                return kCIDLib::False;
            }
        }

        adatVal.SetString(strNewVal);
    }
     else if (adatVal.eType() == tCIDMData::EAttrTypes::Area)
    {
        TArea areaTest;
        if (!areaTest.bParseFromText(strNewVal, tCIDLib::ERadices::Auto, L','))
        {
            strErrMsg = L"Areas must be in the form: x,y,width,height";
            return kCIDLib::False;
        }

        // Make sure that it's within any limit area
        if (adatVal.strLimits().bStartsWith(kCIDMData::strAttrLim_AreaPref))
        {
            TArea areaLim;
            if (!TAttrData::bParseAreaLim(adatVal.strLimits(), areaLim, strErrMsg))
                return kCIDLib::False;

            if (!areaLim.bContainsArea(areaTest))
            {
                strErrMsg = L"The area is not within the limit area of ";
                areaLim.FormatToText(strErrMsg, kCIDLib::chComma, kCIDLib::True);
                return kCIDLib::False;
            }
        }
         else if (adatVal.strLimits().bStartsWith(kCIDMData::strAttrLim_SizePref))
        {
            // The origin must be zero in this case
            if (areaTest.pntOrg() != TPoint::pntOrigin)
            {
                strErrMsg = L"This area must remain positioned at 0,0";
                return kCIDLib::False;
            }

            TSize szMin, szMax;
            if (!TAttrData::bParseSizeLim(adatVal.strLimits(), szMin, szMax, strErrMsg))
                return kCIDLib::False;

            if ((areaTest.c4Width() > szMax.c4Width())
            ||  (areaTest.c4Height() > szMax.c4Height()))
            {
                strErrMsg = L"The area cannot be larger than ";
                szMax.FormatToText(strErrMsg, tCIDLib::ERadices::Dec, L'x', kCIDLib::True);
                return kCIDLib::False;
            }

            if ((areaTest.c4Width() < szMin.c4Width())
            ||  (areaTest.c4Height() < szMin.c4Height()))
            {
                strErrMsg = L"The area cannot be smaller than ";
                szMin.FormatToText(strErrMsg, tCIDLib::ERadices::Dec, L'x', kCIDLib::True);
                return kCIDLib::False;
            }
        }
        adatVal.SetArea(areaTest);
    }
     else if (adatVal.eType() == tCIDMData::EAttrTypes::Bool)
    {
        if (strNewVal.bCompareI(kCIDMData::strAttrVal_False)
        ||  strNewVal.bCompareI(kCIDMData::strAttrVal_AltFalse1)
        ||  strNewVal.bCompareI(kCIDMData::strAttrVal_AltFalse2))
        {
            adatVal.SetBool(kCIDLib::False);
        }
         else if (strNewVal.bCompareI(kCIDMData::strAttrVal_True)
              ||  strNewVal.bCompareI(kCIDMData::strAttrVal_AltTrue1)
              ||  strNewVal.bCompareI(kCIDMData::strAttrVal_AltTrue2))
        {
            adatVal.SetBool(kCIDLib::True);
        }
         else
        {
            strErrMsg = L"The value must be False or True (0 or 1 are also accepted)";
            return kCIDLib::False;
        }
    }
     else if (adatVal.eType() == tCIDMData::EAttrTypes::Card)
    {
        // Make sure we can convert it
        tCIDLib::TCard4 c4Val;
        if (!strNewVal.bToCard4(c4Val, tCIDLib::ERadices::Auto))
        {
            strErrMsg = L"The value must be an unsigned number";
            return kCIDLib::False;
        }

        // If there is range info, getit and compare the value
        tCIDLib::TInt8 i8Min, i8Max;
        if (bIPEHasRange(adatVal.strLimits(), i8Min, i8Max, kCIDLib::True))
        {
            if ((tCIDLib::TInt8(c4Val) < i8Min)
            ||  (tCIDLib::TInt8(c4Val) > i8Max))
            {
                strErrMsg = L"The value must be from ";
                strErrMsg.AppendFormatted(i8Min);
                strErrMsg.Append(L" to ");
                strErrMsg.AppendFormatted(i8Max);
                return kCIDLib::False;
            }
        }
        adatVal.SetCard(c4Val);
    }
     else if (adatVal.eType() == tCIDMData::EAttrTypes::Card64)
    {
        // Make sure we can convert it
        tCIDLib::TCard8 c8Val;
        if (!strNewVal.bToCard8(c8Val, tCIDLib::ERadices::Auto))
        {
            strErrMsg = L"The value must be an unsigned 64 bit number";
            return kCIDLib::False;
        }
        adatVal.SetCard64(c8Val);
    }
     else if (adatVal.eType() == tCIDMData::EAttrTypes::Char)
    {
        //
        //  See if there's a limit. If not, then any character is fine. If required,
        //  then can't be empty.
        //
        if (strNewVal.bIsEmpty())
        {
            if (adatVal.strLimits().bCompareI(kCIDMData::strAttrLim_Required)
            ||  adatVal.strLimits().bCompareI(kCIDMData::strAttrLim_SingleReq))
            {
                strErrMsg = L"This value cannot be empty";
                return kCIDLib::False;
            }

            // We can't set a non-char, so set it via the text, which is always legal
            adatVal.SetString(strNewVal);
        }
         else if (strNewVal.c4Length() > 1)
        {
            // Has to be a single character
            strErrMsg = L"The value must be a single character";
            return kCIDLib::False;
        }
         else if (adatVal.strLimits().bStartsWithI(kCIDMData::strAttrLim_CharPref))
        {
            TString strTmpLim = adatVal.strLimits();
            strTmpLim.Cut(0, kCIDMData::strAttrLim_CharPref.c4Length());
            if (!strTmpLim.bContainsChar(strNewVal[0]))
            {
                strErrMsg = L"You must enter one of the allowed character values";
                return kCIDLib::False;
            }
            adatVal.SetChar(strNewVal[0]);
        }
    }
     else if ((adatVal.eType() == tCIDMData::EAttrTypes::CardRange)
          ||  (adatVal.eType() == tCIDMData::EAttrTypes::IntRange))
    {
        // Parse out the actual values, no prefix in this case
        tCIDLib::TInt8 i8Low, i8High;
        if (bIPEHasRange(strNewVal, i8Low, i8High, kCIDLib::False))
        {
            strErrMsg = L"Range values must be in the form: min,max";
            return kCIDLib::False;
        }

        // Make sure that they are within the valid ranges, including implicit ones
        tCIDLib::TInt8 i8Min;
        tCIDLib::TInt8 i8Max;

        if (!bIPEHasRange(adatVal.strLimits(), i8Min, i8Max, kCIDLib::True))
        {
            if (adatVal.eType() == tCIDMData::EAttrTypes::CardRange)
            {
                i8Min = 0;
                i8Max = kCIDLib::c4MaxCard;
            }
             else
            {
                i8Min = kCIDLib::i4MinInt;
                i8Max = kCIDLib::i4MaxInt;
            }
        }

        if ((i8Low < i8Min) || (i8High > i8Max))
        {
            strErrMsg = L"The maximum range is ";
            strErrMsg.AppendFormatted(i8Low);
            strErrMsg.Append(L" to ");
            strErrMsg.AppendFormatted(i8High);
            return kCIDLib::False;
        }

        if (adatVal.eType() == tCIDMData::EAttrTypes::CardRange)
            adatVal.SetCardRange(tCIDLib::TCard4(i8Min), tCIDLib::TCard4(i8Max));
        else
            adatVal.SetIntRange(tCIDLib::TInt4(i8Min), tCIDLib::TInt4(i8Max));
    }
     else if (adatVal.eType() == tCIDMData::EAttrTypes::Float)
    {
        // Make sure we can convert it
        tCIDLib::TFloat8 f8Val;
        if (!strNewVal.bToFloat8(f8Val))
        {
            strErrMsg = L"The value must be an floating point number";
            return kCIDLib::False;
        }

        // If there is range info, getit and compare the value
        tCIDLib::TFloat8 f8Min, f8Max;
        if (bIPEHasFloatRange(adatVal.strLimits(), f8Min, f8Max, kCIDLib::True))
        {
            if ((f8Val < f8Min) || (f8Val > f8Max))
            {
                strErrMsg = L"The value must be from ";
                strErrMsg.AppendFormatted(f8Min, 4);
                strErrMsg.Append(L" to ");
                strErrMsg.AppendFormatted(f8Max, 4);
                return kCIDLib::False;
            }
        }
        adatVal.SetFloat(f8Val);
    }
     else if (adatVal.eType() == tCIDMData::EAttrTypes::Int)
    {
        // Make sure we can convert it
        tCIDLib::TInt4 i4Val;
        if (!strNewVal.bToInt4(i4Val, tCIDLib::ERadices::Dec))
        {
            strErrMsg = L"The value must be an signed number";
            return kCIDLib::False;
        }

        // If there is range info, getit and compare the value
        tCIDLib::TInt8 i8Min, i8Max;
        if (bIPEHasRange(adatVal.strLimits(), i8Min, i8Max, kCIDLib::True))
        {
            if ((i4Val < i8Min) || (i4Val > i8Max))
            {
                strErrMsg = L"The value must be from ";
                strErrMsg.AppendFormatted(i8Min);
                strErrMsg.Append(L" to ");
                strErrMsg.AppendFormatted(i8Max);
                return kCIDLib::False;
            }
        }
        adatVal.SetInt(i4Val);
    }
     else if (adatVal.eType() == tCIDMData::EAttrTypes::Point)
    {
        TPoint pntTest;
        if (!pntTest.bParseFromText(strNewVal))
        {
            strErrMsg = L"Points must be in the form: x,y";
            return kCIDLib::False;
        }

        // Make sure that it's within any limit area
        if (adatVal.strLimits().bStartsWith(kCIDMData::strAttrLim_AreaPref))
        {
            TArea areaLim;
            if (!TAttrData::bParseAreaLim(adatVal.strLimits(), areaLim, strErrMsg))
                return kCIDLib::False;

            if (!areaLim.bContainsPoint(pntTest))
            {
                strErrMsg = L"The point is not within the limit area of ";
                areaLim.FormatToText(strErrMsg, kCIDLib::chComma, kCIDLib::True);
                return kCIDLib::False;
            }
        }
        adatVal.SetPoint(pntTest);
    }
     else if (adatVal.eType() == tCIDMData::EAttrTypes::RGBColor)
    {
        TRGBClr rgbTest;
        if (!rgbTest.bParseFromText(strNewVal))
        {
            strErrMsg = L"Colors must be in the form: r,g,b";
            return kCIDLib::False;
        }

        //
        //  Format back out for canonical format. Try to be nice and use the same
        //  radix it was entered in. So search for an x. If we find one, then assume
        //  hex, else decimal.
        //
        tCIDLib::ERadices eRadix = tCIDLib::ERadices::Dec;
        if (strNewVal.bContainsChar(L'x'))
            eRadix = tCIDLib::ERadices::Hex;
        adatVal.SetRGBColor(rgbTest);
    }
     else if ((adatVal.eType() == tCIDMData::EAttrTypes::String)
          ||  (adatVal.eType() == tCIDMData::EAttrTypes::File))
    {
        //
        //  If it's empty, see if the limit indicates that not valid. If not empty,
        //  see if the limit is an enumerated type and if so that the value is one
        //  of those enumerated values.
        //
        if (strNewVal.bIsEmpty())
        {
            if (adatVal.strLimits().bCompareI(kCIDMData::strAttrLim_Required)
            ||  adatVal.strLimits().bCompareI(kCIDMData::strAttrLim_SingleReq))
            {
                strErrMsg = L"This value cannot be empty";
                return kCIDLib::False;
            }
            adatVal.SetString(strNewVal);
        }
         else if (adatVal.strLimits().bStartsWithI(kCIDMData::strAttrLim_EnumPref))
        {
            //
            //  We have to force the new value into the attribute to pass to the enum
            //  value tester.
            //
            TString strCanonVal;
            adatVal.SetString(strNewVal);
            if (bTestEnumVal(adatVal, strCanonVal))
            {
                adatVal.SetString(strCanonVal);
            }
             else
            {
                strErrMsg = L"You must enter one of the possible enumerated values";
                return kCIDLib::False;
            }
        }
         else
        {
            // Nothing special, so just store the raw text
            adatVal.SetString(strNewVal);
        }
    }
     else if (adatVal.eType() == tCIDMData::EAttrTypes::Size)
    {
        TSize szTest;
        if (!szTest.bParseFromText(strNewVal, tCIDLib::ERadices::Auto, L','))
        {
            strErrMsg = L"Sizes must be in the form: width,height";
            return kCIDLib::False;
        }

        // Make sure that it's within any limit area
        if (adatVal.strLimits().bStartsWith(kCIDMData::strAttrLim_SizePref))
        {
            TSize szMin, szMax;
            if (!TAttrData::bParseSizeLim(adatVal.strLimits(), szMin, szMax, strErrMsg))
                return kCIDLib::False;

            if ((szTest.c4Width() > szMax.c4Width())
            ||  (szTest.c4Height() > szMax.c4Height()))
            {
                strErrMsg = L"The size cannot be larger than ";
                szMax.FormatToText
                (
                    strErrMsg, tCIDLib::ERadices::Dec, kCIDLib::chComma, kCIDLib::True
                );
                return kCIDLib::False;
            }

            if ((szTest.c4Width() < szMin.c4Width())
            ||  (szTest.c4Height() < szMin.c4Height()))
            {
                strErrMsg = L"The size cannot be smaller than ";
                szMin.FormatToText
                (
                    strErrMsg, tCIDLib::ERadices::Dec, kCIDLib::chComma, kCIDLib::True
                );
                return kCIDLib::False;
            }
        }
        adatVal.SetSize(szTest);
    }
     else if (adatVal.eType() == tCIDMData::EAttrTypes::Time)
    {
        tCIDLib::TCard8 c8Val;
        if (!strNewVal.bToCard8(c8Val, tCIDLib::ERadices::Auto))
        {
            strErrMsg = L"The value must be an unsigned 64 bit number";
            return kCIDLib::False;
        }
        adatVal.SetTime(c8Val);
    }

    return kCIDLib::True;
}


//
//  If not overridden, do nothing. THis is called when an edited value passes
//  validation, and needs to be stored back to whever it came from. The value has
//  already been set back on the attribute object, and they get the previous value
//  as well, in case that's important.
//
tCIDLib::TVoid
MIPEIntf::IPEValChanged(const   tCIDCtrls::TWndId
                        , const TString&
                        , const TAttrData&
                        , const TAttrData&
                        , const tCIDLib::TCard8)
{
}


// ---------------------------------------------------------------------------
//  MEPVal: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  We check to see if the item has numeric range limits, which are in the form.
//  It's used for both card/int, and cardrange/intrange.
//
//      Range:min, max
//
//  We can optionally only check for min,max, i.e. parse out the value itself,
//  so we can get the most out of this code.
//
//  We get them as 8 byte ints, so that we can return both signed and unsigned
//  32 bits values from the same method. We have a separate one for floating point.
//
tCIDLib::TBoolean
MIPEIntf::bIPEHasFloatRange(const   TString&            strCheck
                            ,       tCIDLib::TFloat8&   f8Min
                            ,       tCIDLib::TFloat8&   f8Max
                            , const tCIDLib::TBoolean   bPrefix) const
{
    if (bPrefix && !strCheck.bStartsWith(kCIDMData::strAttrLim_RangePref))
        return kCIDLib::False;

    TString strMin = strCheck;
    if (bPrefix)
        strMin.Cut(0, kCIDMData::strAttrLim_RangePref.c4Length());

    TString strMax;
    if (!strMin.bSplit(strMax, kCIDLib::chSpace))
    {

        return kCIDLib::False;
    }

    strMin.StripWhitespace();
    strMax.StripWhitespace();
    if (!strMin.bToFloat8(f8Min) || !strMax.bToFloat8(f8Max))
        return kCIDLib::False;
    return kCIDLib::True;
}


tCIDLib::TBoolean
MIPEIntf::bIPEHasRange( const   TString&            strCheck
                        ,       tCIDLib::TInt8&     i8Min
                        ,       tCIDLib::TInt8&     i8Max
                        , const tCIDLib::TBoolean   bPrefix) const
{
    if (bPrefix && !strCheck.bStartsWith(kCIDMData::strAttrLim_RangePref))
        return kCIDLib::False;

    TString strMin = strCheck;
    if (bPrefix)
        strMin.Cut(0, kCIDMData::strAttrLim_RangePref.c4Length());

    TString strMax;
    if (!strMin.bSplit(strMax, kCIDLib::chSpace))
    {

        return kCIDLib::False;
    }

    strMin.StripWhitespace();
    strMax.StripWhitespace();
    if (!strMin.bToInt8(i8Min, tCIDLib::ERadices::Dec)
    ||  !strMax.bToInt8(i8Max, tCIDLib::ERadices::Dec))
    {
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TCard4 MIPEIntf::c4IPERow() const
{
    return m_c4IPECurRow;
}

tCIDLib::TCard4 MIPEIntf::c4IPERow(const tCIDLib::TCard4 c4ToSet)
{
    m_c4IPECurRow = c4ToSet;
    return m_c4IPECurRow;
}




// ---------------------------------------------------------------------------
//  CLASS: TInPlaceEdit
// PREFIX: ipe
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TInPlaceEdit: Constructors and Destructor
// ---------------------------------------------------------------------------
TInPlaceEdit::TInPlaceEdit() :

    TWindow(kCIDLib::False)
    , m_bExit(kCIDLib::False)
    , m_bFirstErr(kCIDLib::True)
    , m_bSaveVal(kCIDLib::False)
{
}

TInPlaceEdit::~TInPlaceEdit()
{
}


// ---------------------------------------------------------------------------
//  TInPlaceEdit: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This is called to invoke the in-place edit process. We return true if the
//  user has committed to the change.
//
tCIDLib::TBoolean
TInPlaceEdit::bProcess(         TWindow&        wndParent
                        , const TArea&          areaEdit
                        ,       TAttrData&      adatTar
                        ,       MIPEIntf&       ipeiValidator
                        , const tCIDLib::TCard8 c8UserId)
{
    //
    //  Create us with the correct styles, but not visible. We can set some styles
    //  based on data type at some point if that's desirable.
    //
    TString strCurVal;
    adatTar.FormatToText(strCurVal);
    tCIDLib::TCard4 c4Styles = ES_AUTOHSCROLL | WS_CHILD | WS_BORDER;
    CreateWnd
    (
        wndParent.hwndSafe()
        , WC_EDIT
        , strCurVal.pszBuffer()
        , areaEdit
        , tCIDCtrls::EWndStyles(c4Styles)
        , tCIDCtrls::EExWndStyles::None
        , 5000
    );

    try
    {
        //
        //  Get our area, so we can watch for clicks outside of us. We have to get
        //  this in screen coordinates since the msg point is that type.
        //
        TArea areaUs;
        ToScreenCoordinates(areaWndSize(), areaUs);

        // Subclass it so we can spy on user input
        SetSubclass();

        // Set us to use the passed font
        ::SendMessage
        (
            hwndThis()
            , WM_SETFONT
            , tCIDCtrls::TWParam(TGUIFacility::gfontDefNorm().hfontThis())
            , 0
        );

        // Now we can show it, bring it to the top, and activate it
        ::SetWindowPos
        (
            hwndThis()
            , HWND_TOP
            , 0
            , 0
            , 0
            , 0
            , SWP_NOMOVE | SWP_NOSIZE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER | SWP_SHOWWINDOW
        );

        // Take focus
        ::SetFocus(hwndThis());

        // If any initial content, select it all
        if (!strCurVal.bIsEmpty())
            ::SendMessage(hwndThis(), EM_SETSEL, 0, -1);

        //
        //  Sometimes it won't show up, paritcularly in dialog boxes, so we have to
        //  force a redraw of the underlying parent window.
        //
        wndParent.Redraw(tCIDCtrls::ERedrawFlags::Full);

        // Capture the mouse so that we can watch for clicks outside of us
        TMouseCapJanitor janMouse(this);

        //
        //  Now do a modal loop, so we can watch for anything that wuld make us
        //  exit.
        //
        MSG msgCur;
        TPoint pntMsg;
        while (::GetMessage(&msgCur, 0, 0, 0))
        {
            pntMsg.Set(msgCur.pt.x, msgCur.pt.y);

            if (((msgCur.message == WM_LBUTTONDOWN)
            ||   (msgCur.message == WM_MBUTTONDOWN)
            ||   (msgCur.message == WM_RBUTTONDOWN)
            ||   (msgCur.message == WM_LBUTTONUP)
            ||   (msgCur.message == WM_MBUTTONUP)
            ||   (msgCur.message == WM_RBUTTONUP)
            ||   (msgCur.message == WM_KILLFOCUS))
            &&   !areaUs.bContainsPoint(pntMsg))
            {
                // Any mouse button outside us, dismiss us without saving
                m_bExit = kCIDLib::True;
            }
             else if ((msgCur.message == WM_CHAR)
                  &&  (msgCur.wParam == VK_ESCAPE))
            {
                //
                //  On escape, if our toolk tip is visible, then hide it. Else,
                //  we exit since the user is giving up. Tell it we don't care if
                //  if it's valid or not, invalid means not visible.
                //
                if (m_wndErrTip.bIsVisible(kCIDLib::False))
                    m_wndErrTip.Hide();
                else
                    m_bExit = kCIDLib::True;
            }
             else
            {
                ::TranslateMessage(&msgCur);
                ::DispatchMessage(&msgCur);
            }

            if (m_bExit)
            {
                if (m_bSaveVal)
                {
                    // Get the current text of the entry field
                    tCIDCtrls::TMsgResult mresLen = ::SendMessage
                    (
                        hwndThis(), WM_GETTEXTLENGTH, 0, 0
                    );

                    if (mresLen)
                    {
                        tCIDLib::TCh* pchBuf = new tCIDLib::TCh[mresLen + 1];
                        TArrayJanitor<tCIDLib::TCh> janBuf(pchBuf);

                        ::SendMessage
                        (
                            hwndThis(), WM_GETTEXT, mresLen + 1, tCIDLib::TInt4(pchBuf)
                        );
                        strCurVal.ImportChars(pchBuf, mresLen);
                    }
                     else
                    {
                        strCurVal.Clear();
                    }

                    //
                    //  Validate this value. We only break out if it's valid. We need
                    //  to save a copy of the attribute to pass in as the old value
                    //  if they modify this one. If they accept it, they must update
                    //  the adatTest object's value. This also allows them to put it into
                    //  a cononical format if needed.
                    //
                    //  They can also modify the user data to be passed to the changed
                    //  callback. They can only change a copy. If this fails we get
                    //  back here again, we'll pass the original again.
                    //
                    tCIDLib::TCard8 c8NewUserId = c8UserId;
                    TAttrData adatTest(adatTar);
                    TString strErrMsg;
                    const tCIDLib::TBoolean bValid = ipeiValidator.bIPEValidate
                    (
                        wndParent.strName(), adatTest, strCurVal, strErrMsg, c8NewUserId
                    );

                    if (bValid)
                    {
                        //
                        //  Store the changes, remembering the previous value first so
                        //  that we can pass it in below.
                        ///
                        const TAttrData adatOld(adatTar);
                        adatTar = adatTest;

                        //
                        //  It was accepted (and possibly modified) in the process of
                        //  putting it back into the attribute object. So call the
                        //  value changed callback to let the client code store it.
                        //
                        ipeiValidator.IPEValChanged
                        (
                            wndParent.widThis()
                            , wndParent.strName()
                            , adatTar
                            , adatOld
                            , c8NewUserId
                        );
                    }
                     else
                    {
                        //
                        //  Show a tool tip that indicates the error. If this is the
                        //  first error, then create it.
                        //
                        if (m_bFirstErr)
                        {
                            m_bFirstErr = kCIDLib::False;
                            m_wndErrTip.Create
                            (
                                *this, L"Invalid value", tCIDCtrls::ETTipIcons::Error
                            );
                        }

                        // And display it
                        TPoint pntTool;
                        ToScreenCoordinates(areaWndSize().pntCenter(), pntTool);
                        m_wndErrTip.Display(*this, pntTool, strErrMsg);

                        // Reset the flags and we'll keep going
                        m_bExit = kCIDLib::False;
                        m_bSaveVal = kCIDLib::False;
                    }
                }

                // If the exit flag stayed on, let's break out
                if (m_bExit)
                    break;
            }
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);

        // Fall though now, we need to get the window destroyed
        m_bSaveVal = kCIDLib::False;
    }

    // Destroy our window now before we exit
    Destroy();

    return m_bSaveVal;
}


// ---------------------------------------------------------------------------
//  TInPlaceEdit: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  We implement this so that we can watch for keyboard input and close if we see
//  an enter. We stil pass everything through.
//
tCIDLib::TBoolean
TInPlaceEdit::bEatSubClassMsg(  const   tCIDCtrls::TWndMsg      wmsgMsg
                                , const tCIDCtrls::TWParam      wParam
                                , const tCIDCtrls::TLParam      lParam
                                ,       tCIDCtrls::TMsgResult&  mresRet)
{
    if (wmsgMsg == WM_CHAR)
    {
        if ((wParam == VK_RETURN) || (wParam == VK_TAB))
        {
            m_bSaveVal = kCIDLib::True;
            m_bExit = kCIDLib::True;

            // Don't let it propogate or we get an annoying ding from the system
            return kCIDLib::True;
        }
    }

    // Let it go through
    return kCIDLib::False;
}


// We make sure our tool tip is destroyed
tCIDLib::TVoid TInPlaceEdit::Destroyed()
{
    m_wndErrTip.Destroy();
    TParent::Destroyed();
}


