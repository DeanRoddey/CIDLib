//
// FILE NAME: CIDCtrls_AccelTable.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/02/2015
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
//  This file implements the accelerator table class.
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
RTTIDecls(TAccelTable, TObject)


// ---------------------------------------------------------------------------
//  Local helpers
// ---------------------------------------------------------------------------

//
//  This is a helper that is called when setting up accelerator tables. We get
//  the text after any Ctrl-, Shift-, etc... We have to figure out what it is
//  and match it to a virtual key code. If we can't match it, we return c2MaxCard
//  and that accelerator entry will be ignored.
//
//  We can modify the text.
//
//  We don't support all possible virtual keys, just the ones that make reasonable
//  sense for hot keys.
//
static tCIDLib::TCard2 c2XlatShortCutKey(TString& strText)
{
    tCIDLib::TCard2 c2Ret = kCIDLib::c2MaxCard;

    // Get the length. If empty, give up now, to simplify things below
    const tCIDLib::TCard4 c4Len = strText.c4Length();
    if (!c4Len)
        return c2Ret;


    tCIDLib::TCh chFirst = strText[0];
    if (c4Len == 1)
    {
        // If it's a single character, it has to be a digit or character
        chFirst = TRawStr::chUpper(chFirst);
        if ((chFirst >= L'A') && (chFirst <= L'Z'))
            c2Ret = 0x41 + (chFirst - L'A');
        else if ((chFirst >= L'0') && (chFirst <= L'9'))
            c2Ret = 0x30 + (chFirst - L'0');
        else if (strText.bCompareI(L"+"))
            c2Ret = VK_OEM_PLUS;
        else if (strText.bCompareI(L"-"))
            c2Ret = VK_OEM_MINUS;
    }
     else if (((c4Len == 2) || (c4Len == 3)) && (chFirst == L'F'))
    {
        //
        //  It's got to be a function key, so after the F must be a decimal value.
        //  The value has to be 1 to 24.
        //
        strText.Cut(0, 1);
        tCIDLib::TCard4 c4FVal;
        if (!strText.bToCard4(c4FVal, tCIDLib::ERadices::Dec)
        ||  (c4FVal == 0)
        ||  (c4FVal > 24))
        {
            return c2Ret;
        }

        c2Ret = tCIDLib::TCard2(VK_F1 + (c4FVal - 1));
    }
     else if (strText.bStartsWithI(L"NumPad"))
    {
        if (!strText.bIsEmpty())
        {
            // There has to be a digit or name of a num pad key
            strText.Cut(0, 6);
            chFirst = strText[0];

            if ((chFirst >= L'0') && (chFirst <= L'9'))
                c2Ret = tCIDLib::TCard2(VK_NUMPAD0 + (chFirst - L'0'));
        }
    }
     else if (strText.bCompareI(L"Plus"))
    {
        c2Ret = VK_ADD;
    }
     else if (strText.bCompareI(L"Div"))
    {
        c2Ret = VK_DIVIDE;
    }
     else if (strText.bCompareI(L"Enter"))
    {
        c2Ret = VK_RETURN;
    }
     else if (strText.bCompareI(L"Mul"))
    {
        c2Ret = VK_MULTIPLY;
    }
     else if (strText.bCompareI(L"Minus"))
    {
        c2Ret = VK_SUBTRACT;
    }
     else if (strText.bStartsWithI(L"Delete") || strText.bStartsWithI(L"Del"))
    {
        c2Ret = VK_DELETE;
    }
     else if (strText.bStartsWithI(L"Down"))
    {
        c2Ret = VK_DOWN;
    }
     else if (strText.bStartsWithI(L"Insert"))
    {
        c2Ret = VK_INSERT;
    }
     else if (strText.bStartsWithI(L"Left"))
    {
        c2Ret = VK_LEFT;
    }
     else if (strText.bStartsWithI(L"PageDn"))
    {
        c2Ret = VK_NEXT;
    }
     else if (strText.bStartsWithI(L"PageUp"))
    {
        c2Ret = VK_PRIOR;
    }
     else if (strText.bStartsWithI(L"Play"))
    {
        c2Ret = VK_MEDIA_PLAY_PAUSE;
    }
     else if (strText.bStartsWithI(L"Right"))
    {
        c2Ret = VK_RIGHT;
    }
     else if (strText.bStartsWithI(L"Stop"))
    {
        c2Ret = VK_MEDIA_STOP;
    }
     else if (strText.bStartsWithI(L"Up"))
    {
        c2Ret = VK_UP;
    }
     else if (strText.bStartsWithI(L"VolDn"))
    {
        c2Ret = VK_VOLUME_DOWN;
    }
     else if (strText.bStartsWithI(L"VolMute"))
    {
        c2Ret = VK_VOLUME_MUTE;
    }
     else if (strText.bStartsWithI(L"VolUp"))
    {
        c2Ret = VK_VOLUME_UP;
    }


    return c2Ret;
}



// ---------------------------------------------------------------------------
//  CLASS: TAccelTable
// PREFIX: accel
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TAccelTable: Constructors and Destructor
// ---------------------------------------------------------------------------
TAccelTable::TAccelTable() :

    m_haccelThis(0)
{
}

TAccelTable::~TAccelTable()
{
    // If we created it, the close it
    Close();
}


// ---------------------------------------------------------------------------
// TAccelTable: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This is called to see if an incoming message matches one of our accelerators.
//  If so, we return true, else false.
//
tCIDLib::TBoolean
TAccelTable::bProcessAccel( const   TWindow&                wndTar
                            , const tCIDLib::TVoid* const   pMsgData) const
{
    // If no table handle,then obviously not
    if (m_haccelThis == kCIDCtrls::haccelInvalid)
        return kCIDLib::False;

    // Else let's try it
    const tCIDLib::TBoolean bRes
    (
        ::TranslateAccelerator(wndTar.hwndSafe(), m_haccelThis, (MSG*)pMsgData) != 0
    );
    return bRes;
}


// Close the table if we have one
tCIDLib::TVoid TAccelTable::Close()
{
    if (m_haccelThis)
    {
        ::DestroyAcceleratorTable(m_haccelThis);
        m_haccelThis = 0;
    }
}


//
//  Create a table from a list of key descriptions. See the header comments about
//  the format.
//
tCIDLib::TVoid
TAccelTable::Create(const   tCIDLib::TStrList&      colKeys
                    , const tCIDLib::TCardList&     fcolCmds)
{
    // Close any previous table
    Close();

    // Just in case, so we don't try to allocate an empty array
    const tCIDLib::TCard4 c4KeyCount = colKeys.c4ElemCount();
    if (!c4KeyCount)
        return;

    // They should be the same size
    CIDAssert
    (
        c4KeyCount == fcolCmds.c4ElemCount()
        , L"Key and command id lists are not the same size"
    );

    //
    //  Allocate an array of accelerator structures. We can't have more of them than
    //  keys we were provided with.
    //
    ACCEL* pAccels = new ACCEL[c4KeyCount];
    TArrayJanitor<ACCEL> janAccels(pAccels);

    tCIDLib::TCard4 c4TableSz = 0;
    TString strCur;

    for (tCIDLib::TCard4 c4KeyInd = 0; c4KeyInd < c4KeyCount; c4KeyInd++)
    {
        strCur = colKeys[c4KeyInd];

        // Get the current accelerator and provissionally fill it in
        ACCEL& CurAccel = pAccels[c4TableSz];

        // Deeal with the shift variations
        CurAccel.fVirt = 0;
        tCIDLib::TCard4 c4CutTo = 0;
        if (strCur.bStartsWithI(L"Ctrl-Shift-")
        ||  strCur.bStartsWithI(L"Shift-Ctrl-"))
        {
            CurAccel.fVirt = FSHIFT | FCONTROL;
            c4CutTo = 11;
        }
         else if (strCur.bStartsWithI(L"Alt-Ctrl-")
              ||  strCur.bStartsWithI(L"Ctrl-Alt-"))
        {
            CurAccel.fVirt = FALT | FCONTROL;
            c4CutTo = 9;
        }
         else if (strCur.bStartsWithI(L"Alt-Shift-")
              ||  strCur.bStartsWithI(L"Shift-Alt-"))
        {
             CurAccel.fVirt = FALT | FSHIFT;
             c4CutTo = 10;
        }
         else if (strCur.bStartsWithI(L"Alt-"))
        {
            CurAccel.fVirt = FALT;
            c4CutTo = 4;
        }
         else if (strCur.bStartsWithI(L"Ctrl-"))
        {
            CurAccel.fVirt = FCONTROL;
            c4CutTo = 5;
        }
         else if (strCur.bStartsWithI(L"Shift-"))
        {
            CurAccel.fVirt = FSHIFT;
            c4CutTo = 6;
        }

        // Cut that bit out and what's left should be the actual thing
        strCur.Cut(0, c4CutTo);

        //
        //  Now we have to do some work to figure out what virtual key code to
        //  use. We always do virtual key codes since it avoids a lot of wierdness.
        //  If it's one we can handle, let's add it.
        //
        CurAccel.key = c2XlatShortCutKey(strCur);
        if (CurAccel.key != kCIDLib::c2MaxCard)
        {
            // We are going to keep it so bump the table size
            c4TableSz++;
            CurAccel.fVirt |= FVIRTKEY;
            CurAccel.cmd = tCIDLib::TCard2(fcolCmds[c4KeyInd]);
        }
    }

    // If we kept any, create ourself
    if (c4TableSz)
    {
        m_haccelThis = ::CreateAcceleratorTable(pAccels, c4TableSz);
        if (!m_haccelThis)
        {
            TKrnlError::SetLastError(::GetLastError());
            facCIDCtrls().LogKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcMenu_CreateAccel
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}


//
//  We ask the menu to create a list of strings with all of the hot key indicators
//  in it. We pass this to the other version.
//
tCIDLib::TVoid TAccelTable::Create(const TMenu& menuSrc)
{
    tCIDLib::TStrList   colKeyStrs;
    tCIDLib::TCardList  fcolCmds;

    // Ask the menu for accelerator info, and call the other version
    menuSrc.QueryAccels(colKeyStrs, fcolCmds);
    Create(colKeyStrs, fcolCmds);
}

