//
// FILE NAME: CIDBuild_ResCompiler_Dlg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/02/2000
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
//  This file implements the dialog compilation parts of the resource compiler
//  class.
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
#include    "CIDBuild.hpp"



// ---------------------------------------------------------------------------
//  Local types
// ---------------------------------------------------------------------------
enum class EDlgFlds
{
    Symbol
    , Area
    , TextSym
    , Flags
    , Theme
    , Count
    , Min      = 0
};
EnumIncMacro(EDlgFlds)

enum class EWidgetFlds
{
    Symbol
    , Area
    , TextSym
    , FlyOverSym
    , CueSym
    , Hints
    , Flags
    , Image
    , Anchor
    , Count
    , Min      = 0
};
EnumIncMacro(EWidgetFlds)



// ---------------------------------------------------------------------------
//  TResCompiler: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TResCompiler::ProcessDialog(TBinFile& bflBinOut, TTextFile& tflResHpp)
{
    static tCIDLib::TBoolean bInitDone = kCIDLib::False;
    static TBlkField ablkfDlgInfo[tCIDLib::TCard4(EDlgFlds::Count)] =
    {
        TBlkField(L"SYMBOL"         , kCIDLib::True     , 2, 2 )
        , TBlkField(L"AREA"         , kCIDLib::True     , 4, 4 )
        , TBlkField(L"TEXTSYM"      , kCIDLib::False    , 1, 1 )
        , TBlkField(L"FLAGS"        , kCIDLib::False    , 1, 4 )
        , TBlkField(L"THEME"        , kCIDLib::True     , 1, 1 )
    };

    //
    //  If this is the first time through, then do some more init. We have
    //  to setup up the types of the various values
    //
    if (!bInitDone)
    {
        ablkfDlgInfo[tCIDLib::TCard4(EDlgFlds::Symbol)].eValType(0, TBlkField::EValTypes::CppName);
        ablkfDlgInfo[tCIDLib::TCard4(EDlgFlds::Symbol)].eValType(1, TBlkField::EValTypes::Cardinal);

        ablkfDlgInfo[tCIDLib::TCard4(EDlgFlds::Area)].eValType(0, TBlkField::EValTypes::Integer);
        ablkfDlgInfo[tCIDLib::TCard4(EDlgFlds::Area)].eValType(1, TBlkField::EValTypes::Integer);
        ablkfDlgInfo[tCIDLib::TCard4(EDlgFlds::Area)].eValType(2, TBlkField::EValTypes::Cardinal);
        ablkfDlgInfo[tCIDLib::TCard4(EDlgFlds::Area)].eValType(3, TBlkField::EValTypes::Cardinal);

        ablkfDlgInfo[tCIDLib::TCard4(EDlgFlds::Theme)].eValType(0, TBlkField::EValTypes::Alpha);

        // And indicate that we've done this init
        bInitDone = kCIDLib::True;
    }

    TBldStr strReadBuf;

    //
    //  This is our map item, which we'll fill in as we get the info we need
    //  and then write out at the end.
    //
    tCIDLib::TResItemMap rimCur = {0};
    rimCur.eType = tCIDLib::EResTypes::Dialog;
    rimCur.c4At = bflBinOut.c4CurPos();

    //
    //  We'll also need to fill in a dialog description. We'll partially
    //  fill it in now and write it out. We'll seek back and update it after
    //  we've gotten all the information, which includes the count of children
    //  which we cannot know until we've parsed and written out all of the
    //  the children. The place to seek back to was stored above as the offset
    //  for this resource.
    //
    tCIDCtrls::TRawDlgDesc rdlgdTmp = {0};
    rdlgdTmp.c4FormatVersion = kCIDCtrls::c4ResFmtVersion;
    rdlgdTmp.midTitle = kCIDLib::c4MaxCard;
    rdlgdTmp.eFlags = tCIDCtrls::EDlgFlags::None;
    rdlgdTmp.eTheme = tCIDCtrls::EWndThemes::None;
    bflBinOut.c4Write(&rdlgdTmp, sizeof(rdlgdTmp));

    // The first thing we see must be the INFO block
    if (!m_plsplInput->bReadLine(strReadBuf))
    {
        stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                << L") Expected INFO="
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::UnexpectedEOF;
    }

    TBldStr strDlgName;
    if (strReadBuf != L"INFO=")
    {
        stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                << L") Expected INFO="
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::FileFormat;
    }
     else
    {
        // Use the block parser class to do the grunt work parsing
        TBlkParser blkpInfo
        (
            ablkfDlgInfo, tCIDLib::TCard4(EDlgFlds::Count), m_plsplInput, L"END INFO"
        );
        blkpInfo.Parse();

        //
        //  Ok, we got all the required fields, so lets get the ones
        //  with provided values and process those values.
        //
        for (EDlgFlds eFld = EDlgFlds::Min; eFld < EDlgFlds::Count; eFld++)
        {
            const TBlkField& blkfCur = ablkfDlgInfo[tCIDLib::TCard4(eFld)];

            // If this one wasn't provided, then try again
            if (!blkfCur.bFound())
                continue;

            if (eFld == EDlgFlds::Symbol)
            {
                // Store the resource id
                rimCur.ridRes = blkfCur.strField(1).c4AsCard();

                //
                //  We also store it in the dialog descr, so that apps can
                //  know what the resource id is without having to remember
                //  what id they used to load it.
                //
                rdlgdTmp.ridDlg = rimCur.ridRes;

                // Write out the the symbol to the header, if needed
                if (m_bGenHeaders)
                {
                    tflResHpp   << L"    constexpr tCIDLib::TResId  "
                                << blkfCur.strField(0)
                                << L" = " << blkfCur.strField(1) << L";\n";
                }

                strDlgName = blkfCur.strField(0);
            }
             else if (eFld == EDlgFlds::Area)
            {
                // Update the area
                rdlgdTmp.i4X = blkfCur.strField(0).i4AsInt();
                rdlgdTmp.i4Y = blkfCur.strField(1).i4AsInt();
                rdlgdTmp.c4CX = blkfCur.strField(2).c4AsCard();
                rdlgdTmp.c4CY = blkfCur.strField(3).c4AsCard();
            }
             else if (eFld == EDlgFlds::Flags)
            {
                // Parse out any flags to their associated bits
                for (tCIDLib::TCard4 c4Index = 0; c4Index < 4; c4Index++)
                {
                    const TBldStr& strCur = blkfCur.strField(c4Index);
                    if (!strCur.c4Length())
                        break;

                    if (strCur.bIEquals(L"Sizeable"))
                    {
                        rdlgdTmp.eFlags = tCIDCtrls::EDlgFlags
                        (
                            tCIDLib::TEnumMaskType(rdlgdTmp.eFlags)
                            | tCIDLib::TEnumMaskType(tCIDCtrls::EDlgFlags::Sizeable)
                        );
                    }
                     else if (strCur.bIEquals(L"UseOrigin"))
                    {
                        rdlgdTmp.eFlags = tCIDCtrls::EDlgFlags
                        (
                            tCIDLib::TEnumMaskType(rdlgdTmp.eFlags)
                            | tCIDLib::TEnumMaskType(tCIDCtrls::EDlgFlags::UseOrigin)
                        );
                    }
                     else if (strCur.bIEquals(L"ScreenOrigin"))
                    {
                        rdlgdTmp.eFlags = tCIDCtrls::EDlgFlags
                        (
                            tCIDLib::TEnumMaskType(rdlgdTmp.eFlags)
                            | tCIDLib::TEnumMaskType(tCIDCtrls::EDlgFlags::ScreenOrigin)
                        );
                    }
                     else if (strCur.bIEquals(L"SetFgn"))
                    {
                        rdlgdTmp.eFlags = tCIDCtrls::EDlgFlags
                        (
                            tCIDLib::TEnumMaskType(rdlgdTmp.eFlags)
                            | tCIDLib::TEnumMaskType(tCIDCtrls::EDlgFlags::SetFgn)
                        );
                    }
                     else
                    {
                        stdOut  << L"(Line " << blkfCur.c4LineAt()
                                << L") '"
                                << strCur
                                << L"' is not a known dialog flag"
                                << kCIDBuild::EndLn;
                        throw tCIDBuild::EErrors::FileFormat;
                    }
                }
            }
             else if (eFld == EDlgFlds::TextSym)
            {
                //
                //  Its the message id for the text of the title bar. So
                //  get the symbol name out and look it up in the message
                //  id info. If its there, then save the message id for
                //  later use at runtime.
                //
                if (!bFindMsgSymbol(blkfCur.strField(0).pszBuffer(), rdlgdTmp.midTitle))
                {
                    stdOut  << L"(Line " << blkfCur.c4LineAt()
                            << L") Text id '" << blkfCur.strField(0)
                            << L"' is not defined for project "
                            << m_projiSrc.strProjectName() << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::FileFormat;
                }
            }
             else if (eFld == EDlgFlds::Theme)
            {
                if (blkfCur.strField(0).bIEquals(L"MainWnd"))
                    rdlgdTmp.eTheme = tCIDCtrls::EWndThemes::MainWnd;
                else if (blkfCur.strField(0).bIEquals(L"DialogBox"))
                    rdlgdTmp.eTheme = tCIDCtrls::EWndThemes::DialogBox;
                else
                    rdlgdTmp.eTheme = tCIDCtrls::EWndThemes::None;
            }
             else
            {
                stdOut  << tCIDLib::TInt4(eFld)
                        << L" is not a valid dialog field type" << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::Internal;
            }
        }
    }

    // Create a blank item to use as a reset mechanism for each round
    tCIDCtrls::TRawDlgItem  dlgiBlank = {0};
    dlgiBlank.midCue = kCIDLib::c4MaxCard;
    dlgiBlank.midFlyOver = kCIDLib::c4MaxCard;
    dlgiBlank.midText = kCIDLib::c4MaxCard;

    //
    //  Now the rest can be child widget definitions until we hit the end
    //  of the dialog description.
    //
    tCIDCtrls::TRawDlgItem  dlgiCur;
    TBldStr                 strHints;
    TBldStr                 strImage;
    while (kCIDLib::True)
    {
        if (!m_plsplInput->bReadLine(strReadBuf))
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") Expected resource or END DIALOG"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        // Watch for the end of this dialog description
        if (strReadBuf == L"END DIALOG")
            break;

        // Reset the item data for this round
        dlgiCur = dlgiBlank;
        strHints.Clear();

        //
        //  Call another method to parse the various types of child widgets
        //  that we support. They will fill in the dialog item for us.
        //
        if ((strReadBuf == L"2COLSECTLIST=")
        ||  (strReadBuf == L"ACTIVITYBAR=")
        ||  (strReadBuf == L"ATTREDIT=")
        ||  (strReadBuf == L"CALENDAR=")
        ||  (strReadBuf == L"CHECKBOX=")
        ||  (strReadBuf == L"CHECKEDMCLIST=")
        ||  (strReadBuf == L"COLOREDLIST=")
        ||  (strReadBuf == L"GENERICWND=")
        ||  (strReadBuf == L"CLRPALETTE=")
        ||  (strReadBuf == L"COMBOBOX=")
        ||  (strReadBuf == L"ENTRYFLD=")
        ||  (strReadBuf == L"ENUMSPINBOX=")
        ||  (strReadBuf == L"IMGPREVIEW=")
        ||  (strReadBuf == L"LISTBOX=")
        ||  (strReadBuf == L"MCLISTBOX=")
        ||  (strReadBuf == L"MENU=")
        ||  (strReadBuf == L"OBJVIEW=")
        ||  (strReadBuf == L"MLEDIT=")
        ||  (strReadBuf == L"MLSTATICTEXT=")
        ||  (strReadBuf == L"NUMSPINBOX=")
        ||  (strReadBuf == L"PROGRESSBAR=")
        ||  (strReadBuf == L"PUSHBUTTON=")
        ||  (strReadBuf == L"RADIOBUTTON=")
        ||  (strReadBuf == L"SCROLLAREA=")
        ||  (strReadBuf == L"SECTIONALLIST=")
        ||  (strReadBuf == L"SEPARATOR=")
        ||  (strReadBuf == L"SLIDER=")
        ||  (strReadBuf == L"STATICCLR=")
        ||  (strReadBuf == L"STATICIMG=")
        ||  (strReadBuf == L"STATICTEXT=")
        ||  (strReadBuf == L"TABBEDWND=")
        ||  (strReadBuf == L"TEXTOUTWND=")
        ||  (strReadBuf == L"TEXTOUTWND2=")
        ||  (strReadBuf == L"TREEVIEW=")
        ||  (strReadBuf == L"VSEPARATOR="))
        {
            strReadBuf.DeleteLast();
            ProcessDialogWidget
            (
                strDlgName
                , strReadBuf
                , dlgiCur
                , bflBinOut
                , tflResHpp
                , strHints
                , strImage
            );
        }
         else
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") Expected a child widget block, e.g. PUSHBUTTON="
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }

        //
        //  Write out this item. We write out the item first, then the text values
        //  after it.
        //
        dlgiCur.c4HintsLen = strHints.c4Length();
        dlgiCur.c4ItemTypeLen = strReadBuf.c4Length();
        dlgiCur.c4ImageLen = strImage.c4Length();
        bflBinOut.c4Write(&dlgiCur, sizeof(dlgiCur));

        // Do the hints string
        bflBinOut.c4Write
        (
            strHints.pszBuffer(), (dlgiCur.c4HintsLen + 1) * kCIDLib::c4CharBytes
        );

        // Write out the item type
        bflBinOut.c4Write
        (
            strReadBuf.pszBuffer(), (dlgiCur.c4ItemTypeLen + 1) * kCIDLib::c4CharBytes
        );

        // Write out the image name
        bflBinOut.c4Write
        (
            strImage.pszBuffer(), (dlgiCur.c4ImageLen + 1) * kCIDLib::c4CharBytes
        );

        // Bump up the count of children
        rdlgdTmp.c4ChildCount++;
    }

    //
    //  We can go back and fill in the resource item map for this resource
    //  now. The size is all we are missing, which is the difference between
    //  our current end of file and where we started.
    //
    rimCur.c4Size = bflBinOut.c4CurPos() - rimCur.c4At;
    bflBinOut.SeekTo
    (
        sizeof(tCIDLib::TResHeader)
        + (sizeof(tCIDLib::TResItemMap) * m_reshOut.c4ResCount)
    );
    bflBinOut.c4Write(&rimCur, sizeof(rimCur));
    bflBinOut.c4SeekToEnd();

    //
    //  Ok, we have the child count set in the dialog description header,
    //  so lets seek back to where we first wrote the header and update it,
    //  then seek back to the end again.
    //
    bflBinOut.SeekTo(rimCur.c4At);
    bflBinOut.c4Write(&rdlgdTmp, sizeof(rdlgdTmp));
    bflBinOut.c4SeekToEnd();

    // Bump up the resource index
    m_reshOut.c4ResCount++;
}


tCIDLib::TVoid TResCompiler::
ProcessDialogWidget(const   TBldStr&                    strDlgName
                    , const TBldStr&                    strStartText
                    ,       tCIDCtrls::TRawDlgItem&     rdlgiToFill
                    ,       TBinFile&                   bflBinOut
                    ,       TTextFile&                  tflResHpp
                    ,       TBldStr&                    strHints
                    ,       TBldStr&                    strImage)
{
    static tCIDLib::TBoolean bInitDone = kCIDLib::False;
    static TBlkField ablkfWidgetInfo[tCIDLib::TCard4(EWidgetFlds::Count)] =
    {
        TBlkField(L"SYMBOL"         , kCIDLib::False    , 2, 2 )
        , TBlkField(L"AREA"         , kCIDLib::True     , 4, 4 )
        , TBlkField(L"TEXTSYM"      , kCIDLib::False    , 1, 1 )
        , TBlkField(L"FLYOVERSYM"   , kCIDLib::False    , 1, 1 )
        , TBlkField(L"CUESYM"       , kCIDLib::False    , 1, 1 )
        , TBlkField(L"HINTS"        , kCIDLib::False    , 1, 1 )
        , TBlkField(L"FLAGS"        , kCIDLib::False    , 1, 4 )
        , TBlkField(L"IMAGE"        , kCIDLib::False    , 1, 1 )
        , TBlkField(L"ANCHOR"       , kCIDLib::False    , 1, 1 )
    };

    //
    //  If this is the first time through, then do some more init. We have
    //  to setup up the types of the various values
    //
    if (!bInitDone)
    {
        ablkfWidgetInfo[tCIDLib::TCard4(EDlgFlds::Symbol)].eValType(0, TBlkField::EValTypes::CppName);
        ablkfWidgetInfo[tCIDLib::TCard4(EDlgFlds::Symbol)].eValType(1, TBlkField::EValTypes::AlphNum);

        ablkfWidgetInfo[tCIDLib::TCard4(EWidgetFlds::Area)].eValType(0, TBlkField::EValTypes::Integer);
        ablkfWidgetInfo[tCIDLib::TCard4(EWidgetFlds::Area)].eValType(1, TBlkField::EValTypes::Integer);
        ablkfWidgetInfo[tCIDLib::TCard4(EWidgetFlds::Area)].eValType(2, TBlkField::EValTypes::Cardinal);
        ablkfWidgetInfo[tCIDLib::TCard4(EWidgetFlds::Area)].eValType(3, TBlkField::EValTypes::Cardinal);

        ablkfWidgetInfo[tCIDLib::TCard4(EWidgetFlds::Flags)].eValType(0, TBlkField::EValTypes::Alpha);
        ablkfWidgetInfo[tCIDLib::TCard4(EWidgetFlds::Flags)].eValType(1, TBlkField::EValTypes::Alpha);
        ablkfWidgetInfo[tCIDLib::TCard4(EWidgetFlds::Flags)].eValType(2, TBlkField::EValTypes::Alpha);
        ablkfWidgetInfo[tCIDLib::TCard4(EWidgetFlds::Flags)].eValType(3, TBlkField::EValTypes::Alpha);

        // And indicate that we've done this init
        bInitDone = kCIDLib::True;
    }

    // Build up the end text for this block
    TBldStr strEndText(L"END ");
    strEndText.Append(strStartText);

    // Use the block parser class to do the grunt work parsing
    TBlkParser blkpInfo
    (
        ablkfWidgetInfo, tCIDLib::TCard4(EWidgetFlds::Count), m_plsplInput, strEndText
    );
    blkpInfo.Parse();

    //
    //  Ok, we got all the required fields, so lets get the ones
    //  with provided values and process those values.
    //
    tCIDLib::TCard4 c4FocusCnt = 0;
    for (EWidgetFlds eFld = EWidgetFlds::Min; eFld < EWidgetFlds::Count; eFld++)
    {
        const TBlkField& blkfCur = ablkfWidgetInfo[tCIDLib::TCard4(eFld)];

        // If this one wasn't provided, then try again
        if (!blkfCur.bFound())
            continue;

        if (eFld == EWidgetFlds::Area)
        {
            // Update the area
            rdlgiToFill.i4X = blkfCur.strField(0).i4AsInt();
            rdlgiToFill.i4Y = blkfCur.strField(1).i4AsInt();
            rdlgiToFill.c4CX = blkfCur.strField(2).c4AsCard();
            rdlgiToFill.c4CY = blkfCur.strField(3).c4AsCard();
        }
         else if (eFld == EWidgetFlds::CueSym)
        {
            //
            //  Its the message id for the prompt text of the widget. So get
            //  the symbol name out and look it up in the message id info. If
            //  its there, then save the message id for later use at runtime.
            //
            if (!bFindMsgSymbol(blkfCur.strField(0).pszBuffer()
                                , rdlgiToFill.midCue))
            {
                stdOut  << L"(Line " << blkfCur.c4LineAt()
                        << L") Text id '" << blkfCur.strField(0)
                        << L"' is not defined for project "
                        << m_projiSrc.strProjectName() << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::FileFormat;
            }
        }
         else if (eFld == EWidgetFlds::Flags)
        {
            // Look up any of the values and set the appropriate flags
            for (tCIDLib::TCard4 c4Index = 0; c4Index < 4; c4Index++)
            {
                const TBldStr& strCur = blkfCur.strField(c4Index);
                if (!strCur.c4Length())
                    break;

                if (strCur.bIEquals(L"InitFocus"))
                {
                    rdlgiToFill.eFlags = tCIDCtrls::EDlgItemFlags
                    (
                        tCIDLib::TEnumMaskType(rdlgiToFill.eFlags)
                        | tCIDLib::TEnumMaskType(tCIDCtrls::EDlgItemFlags::InitFocus)
                    );
                    c4FocusCnt++;
                }
                 else if (strCur.bIEquals(L"Disabled"))
                {
                    rdlgiToFill.eFlags = tCIDCtrls::EDlgItemFlags
                    (
                        tCIDLib::TEnumMaskType(rdlgiToFill.eFlags)
                        | tCIDLib::TEnumMaskType(tCIDCtrls::EDlgItemFlags::Disabled)
                    );
                }
                 else
                {
                    stdOut  << L"(Line " << blkfCur.c4LineAt()
                            << L") '"
                            << strCur
                            << L"' is not a known dialog item flag"
                            << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::FileFormat;
                }
            }
        }
         else if (eFld == EWidgetFlds::FlyOverSym)
        {
            //
            //  Its the message id for the flyover text of the widget. So get
            //  the symbol name out and look it up in the message id info. If
            //  its there, then save the message id for later use at runtime.
            //
            if (!bFindMsgSymbol(blkfCur.strField(0).pszBuffer()
                                , rdlgiToFill.midFlyOver))
            {
                stdOut  << L"(Line " << blkfCur.c4LineAt()
                        << L") Text id '" << blkfCur.strField(0)
                        << L"' is not defined for project "
                        << m_projiSrc.strProjectName() << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::FileFormat;
            }
        }
         else if (eFld == EWidgetFlds::Symbol)
        {
            //
            //  Store the resource id. We allow for a small number of special
            //  values, so check those first. Else it has to be a number.
            //
            //      IdCancel
            //      IdOK
            //
            if (blkfCur.strField(1).bIEquals(L"IdCancel"))
                rdlgiToFill.ridThis = 2;
            else if (blkfCur.strField(1).bIEquals(L"IdOk"))
                rdlgiToFill.ridThis = 1;
            else
                rdlgiToFill.ridThis = blkfCur.strField(1).c4AsCard();

            // Write out the the symbol to the header, if needed
            if (m_bGenHeaders)
            {
                tflResHpp   << L"    constexpr tCIDLib::TResId  "
                            << blkfCur.strField(0)
                            << L" = " << rdlgiToFill.ridThis << L";\n";
            }
        }
         else if (eFld == EWidgetFlds::TextSym)
        {
            //
            //  Its the message id for the text of the widget. So get the
            //  symbol name out and look it up in the message id info. If
            //  its there, then save the message id for later use at runtime.
            //
            //  But first check for a special symbol. We allow symbols in
            //  the form [xxx] to represent a set of messages in the
            //  CIDKernel loadable message file that define commonly used
            //  message text.
            //
            if (!bFindSpecialMsgSymbol( blkfCur.strField(0).pszBuffer()
                                        , rdlgiToFill.midText)
            &&  !bFindMsgSymbol(blkfCur.strField(0).pszBuffer()
                                , rdlgiToFill.midText))
            {
                stdOut  << L"(Line " << blkfCur.c4LineAt()
                        << L") Text id '" << blkfCur.strField(0)
                        << L"' is not defined for project "
                        << m_projiSrc.strProjectName() << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::FileFormat;
            }
        }
         else if (eFld == EWidgetFlds::Hints)
        {
            // We can't really do any validation here
            strHints = blkfCur.strField(0);
        }
         else if (eFld == EWidgetFlds::Image)
        {
            strImage = blkfCur.strField(0);
        }
         else if (eFld == EWidgetFlds::Anchor)
        {
            // Map it to one of the valid anchor values
            if (blkfCur.strField(0).bIEquals(L"MoveBottom"))
                rdlgiToFill.eAnchor = tCIDCtrls::EEdgeAnchors::MoveBottom;
            else if (blkfCur.strField(0).bIEquals(L"MoveRight"))
                rdlgiToFill.eAnchor = tCIDCtrls::EEdgeAnchors::MoveRight;
            else if (blkfCur.strField(0).bIEquals(L"MoveBottomRight"))
                rdlgiToFill.eAnchor = tCIDCtrls::EEdgeAnchors::MoveBottomRight;
            else if (blkfCur.strField(0).bIEquals(L"SizeBottom"))
                rdlgiToFill.eAnchor = tCIDCtrls::EEdgeAnchors::SizeBottom;
            else if (blkfCur.strField(0).bIEquals(L"SizeRight"))
                rdlgiToFill.eAnchor = tCIDCtrls::EEdgeAnchors::SizeRight;
            else if (blkfCur.strField(0).bIEquals(L"SizeBottomRight"))
                rdlgiToFill.eAnchor = tCIDCtrls::EEdgeAnchors::SizeBottomRight;
            else if (blkfCur.strField(0).bIEquals(L"MoveBottomSizeRight"))
                rdlgiToFill.eAnchor = tCIDCtrls::EEdgeAnchors::MoveBottomSizeRight;
            else if (blkfCur.strField(0).bIEquals(L"SizeBottomMoveRight"))
                rdlgiToFill.eAnchor = tCIDCtrls::EEdgeAnchors::SizeBottomMoveRight;
        }
         else
        {
            stdOut  << L"(Line " << blkfCur.c4LineAt()
                    << L") " << tCIDLib::TInt4(eFld)
                    << L" is not a valid widget field type" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::Internal;
        }
    }

    if (c4FocusCnt > 1)
    {
        stdOut  << L"Line " << m_plsplInput->c4CurLine()
                << L") More than one item in dialog '"
                << strDlgName << L"' claimed initial focus"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::FileFormat;
    }
}

