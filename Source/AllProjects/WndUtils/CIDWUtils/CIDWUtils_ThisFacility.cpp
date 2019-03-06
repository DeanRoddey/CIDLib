//
// FILE NAME: CIDWUtils_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/24/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
//
//  We bring in the headers of all of the dialogs that we encapsulate here so as
//  not to expose their headers. Each of them will include their own header. So only
//  this file and the class file will see any given dialog's header file.
// ---------------------------------------------------------------------------
#include    "CIDWUtils_.hpp"
#include    "CIDWUtils_24BitPaletteDlg_.hpp"
#include    "CIDWUtils_CfgSerialPort_.hpp"
#include    "CIDWUtils_GetSizeDlg_.hpp"
#include    "CIDWUtils_GetTextDlg_.hpp"
#include    "CIDWUtils_EditTextDlg_.hpp"
#include    "CIDWUtils_ErrOptsDlg_.hpp"
#include    "CIDWUtils_FindTextDlg_.hpp"
#include    "CIDWUtils_FontSelDlg_.hpp"
#include    "CIDWUtils_ImgSelDlg_.hpp"
#include    "CIDWUtils_ListSelDlg_.hpp"
#include    "CIDWUtils_OverOptsDlg_.hpp"
#include    "CIDWUtils_SaveOptsDlg_.hpp"
#include    "CIDWUtils_SetTimeDlg_.hpp"
#include    "CIDWUtils_ShowKVPListDlg_.hpp"
#include    "CIDWUtils_ShowListDlg_.hpp"
#include    "CIDWUtils_ShowTextDlg_.hpp"
#include    "CIDWUtils_TriChoiceDlg_.hpp"



// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDWUtils,TGUIFacility)



// ---------------------------------------------------------------------------
//   CLASS: TFacCIDWUtils
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDWUtils: Constructors and operators
// ---------------------------------------------------------------------------
TFacCIDWUtils::TFacCIDWUtils() :

    TGUIFacility
    (
        L"CIDWUtils"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgsAndRes
    )
{
}

TFacCIDWUtils::~TFacCIDWUtils()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDWUtils: Public, non-virtual methods
// ---------------------------------------------------------------------------

// A wrapper around the serial port config dialog
tCIDLib::TBoolean
TFacCIDWUtils::bCfgSerialPort(const TWindow& wndOwner, TCommPortCfg& cpcfgToEdit)
{
    TCfgSerialPortDlg dlgCfg;
    return dlgCfg.bRunDlg(wndOwner, cpcfgToEdit);
}


// A wrapper around the simple text editor dialog
tCIDLib::TBoolean
TFacCIDWUtils::bEditText(const  TWindow&            wndOwner
                        , const TString&            strTitle
                        , const TString&            strOrgText
                        ,       TString&            strNewText
                        , const tCIDLib::TBoolean   bTakeCRs)
{
    TEditTextDlg dlgEdit;
    return dlgEdit.bRun(wndOwner, strTitle, strOrgText, strNewText, bTakeCRs);
}


// A wrapper around the little time interval editing dialog
tCIDLib::TBoolean
TFacCIDWUtils::bEditTime(const  TWindow&                wndOwner
                        , const TString&                strTitle
                        ,       tCIDLib::TEncodedTime&  enctEdit)
{
    TSetTimeDlg dlgSet;
    return dlgSet.bRun(wndOwner, strTitle, enctEdit);
}


tCIDLib::TBoolean
TFacCIDWUtils::bGetReplaceOpts( const   TWindow&                wndOwner
                                ,       TString&                strToFind
                                ,       TString&                strRepWith
                                ,       tCIDLib::TStrList&      colFindRecall
                                ,       tCIDLib::TStrList&      colRepRecall
                                ,       tCIDLib::TBoolean&      bFullMatch
                                ,       tCIDLib::TBoolean&      bSelection
                                , const tCIDWUtils::EFindFlags  eFlags) const
{
    TFindTextDlg dlgSearch;
    return dlgSearch.bRun
    (
        wndOwner
        , kCIDLib::True
        , strToFind
        , strRepWith
        , colFindRecall
        , colRepRecall
        , bFullMatch
        , bSelection
        , eFlags
    );
}

tCIDLib::TBoolean
TFacCIDWUtils::bGetSearchOpts(  const   TWindow&                wndOwner
                                ,       TString&                strToFind
                                ,       tCIDLib::TStrList&      colFindRecall
                                ,       tCIDLib::TBoolean&      bFullMatch
                                ,       tCIDLib::TBoolean&      bSelection
                                , const tCIDWUtils::EFindFlags  eFlags) const
{
    // In search mode we eat some of the options
    TString strRepWith;
    tCIDLib::TStrList colRepRecall;

    TFindTextDlg dlgSearch;
    return dlgSearch.bRun
    (
        wndOwner
        , kCIDLib::False
        , strToFind
        , strRepWith
        , colFindRecall
        , colRepRecall
        , bFullMatch
        , bSelection
        , eFlags
    );
}


//
//  Wrappers around the get size dialog, which gets a horz/vertical size
//  from the user, with various options.
//
tCIDLib::TBoolean
TFacCIDWUtils::bGetSize(const   TWindow&            wndOwner
                        , const TString&            strTitle
                        ,       TSize&              szToFill
                        , const TSize&              szInit
                        , const TSize&              szMin
                        , const TSize&              szMax
                        , const tCIDLib::TBoolean   bForceAR
                        , const tCIDLib::ERadices   eRadix) const
{
    TGetTSizeDlg dlgGetSz;
    return dlgGetSz.bRunDlg(wndOwner, strTitle, szToFill, szInit, szMin, szMax, bForceAR, eRadix);
}

tCIDLib::TBoolean
TFacCIDWUtils::bGetSize(const   TWindow&            wndOwner
                        , const TString&            strTitle
                        ,       TSize&              szToFill
                        , const TSize&              szMin
                        , const TSize&              szMax
                        , const tCIDLib::TBoolean   bForceAR
                        , const tCIDLib::ERadices   eRadix) const
{
    TGetTSizeDlg dlgGetSz;
    return dlgGetSz.bRunDlg(wndOwner, strTitle, szToFill, szMin, szMax, bForceAR, eRadix);
}


// A wrapper around the single line text getter dialog
tCIDLib::TBoolean
TFacCIDWUtils::bGetText(const   TWindow&            wndOwner
                        , const TString&            strTitle
                        , const TString&            strOrgText
                        ,       TString&            strNewText) const
{
    TGetTextDlg dlgGet;
    return dlgGet.bRun(wndOwner, strTitle, strOrgText, strNewText);
}


// A wrapper around the 2nd single line text getter dialog
tCIDLib::TBoolean
TFacCIDWUtils::bGetText2(const  TWindow&            wndOwner
                        , const TString&            strTitle
                        , const TString&            strInstruct
                        , const TString&            strOrgText
                        ,       TString&            strNewText
                        , const TString&            strRegEx
                        , const tCIDLib::TBoolean   bSecret) const
{
    TGetTextDlg2 dlgGet;
    return dlgGet.bRun
    (
        wndOwner, strTitle, strInstruct, strOrgText, strNewText, strRegEx, bSecret
    );
}


//
//  Wrappers around the simple list item selection dialogs. The first one gets a single
//  selection.
//
tCIDLib::TBoolean
TFacCIDWUtils::bListSelect( const   TWindow&                wndOwner
                            , const TString&                strTitle
                            , const tCIDLib::TStrCollect&   colList
                            ,       TString&                strSelText) const
{
    TCIDListSelDlg dlgSel;
    tCIDLib::TStrList colSels;
    if (dlgSel.bRun(wndOwner, strTitle, colList, colSels, kCIDLib::False))
    {
        strSelText = colSels[0];
        return kCIDLib::True;
    }
    return kCIDLib::False;
}

tCIDLib::TBoolean
TFacCIDWUtils::bListSelect2(const   TWindow&                    wndOwner
                            , const TString&                    strTitle
                            ,       tCIDLib::TStrList&          colList
                            ,       TString&                    strSelText
                            , const tCIDWUtils::EListValFmts    eFmt) const
{
    TCIDListSelDlg2 dlgSel;
    if (dlgSel.bRun(wndOwner, strTitle, colList, strSelText, eFmt))
    {
        strSelText = colList[0];
        return kCIDLib::True;
    }
    return kCIDLib::False;
}

tCIDLib::TBoolean
TFacCIDWUtils::bKVPListSelect(  const   TWindow&                wndOwner
                                , const TString&                strTitle
                                , const tCIDLib::TKVPCollect&   colList
                                ,       TKeyValuePair&          kvalSelected
                                , const tCIDLib::TBoolean       bShowValue) const
{
    TCIDKVPListSelDlg dlgSel;
    return dlgSel.bRun(wndOwner, strTitle, colList, kvalSelected, bShowValue);
}


tCIDLib::TBoolean
TFacCIDWUtils::bKVPListMulitiSelect(const   TWindow&            wndOwner
                                    , const TString&            strTitle
                                    , const TString&            strDataTitle
                                    , const tCIDLib::TKVPList&  colAvail
                                    ,       tCIDLib::TKVPList&  colSelected)
{
    TCIDKVPMListSelDlg dlgSel;
    return dlgSel.bRun(wndOwner, strTitle, strDataTitle, colAvail, colSelected);
}


//
//  A wrapper around the list selection dialog, in multi-select mode in this case.
//
tCIDLib::TBoolean
TFacCIDWUtils::bListMultiSelect(const   TWindow&                wndOwner
                                , const TString&                strTitle
                                , const tCIDLib::TStrCollect&   colList
                                ,       tCIDLib::TStrCollect&   colSelections) const
{
    TCIDListSelDlg dlgSel;
    return dlgSel.bRun(wndOwner, strTitle, colList, colSelections, kCIDLib::True);
}


//
//  A wrapper around the checked style multi-list selection dialog. It is basically
//  the same as the one above but uses a checked list box, which is sometimes more
//  convenient for the user. This one also allows for a separate dialog title and
//  a title for the name column of the list box.
//
tCIDLib::TBoolean
TFacCIDWUtils::bListMultiSelect2(const  TWindow&            wndOwner
                                , const TString&            strTitle
                                , const TString&            strDataTitle
                                , const tCIDLib::TStrList&  colAvail
                                ,       tCIDLib::TStrList&  colSelected) const
{
    TCIDMListSelDlg dlgSel;
    return dlgSel.bRun(wndOwner, strTitle, strDataTitle, colAvail, colSelected);
}


// A wrapper around the palette dialog to avoid exposing the header
tCIDLib::TBoolean
TFacCIDWUtils::bSelectColor(const TWindow& wndOwner, TRGBClr& rgbSelected)
{
    T24BitPalDlg dlgPal;
    return dlgPal.bRun(wndOwner, rgbSelected);
}


// Wrapper around the font selection dialogs
tCIDLib::TBoolean
TFacCIDWUtils::bSelectFont( const   TWindow&            wndOwner
                            ,       TString&            strFaceName
                            ,       tCIDLib::TCard4&    c4Height
                            ,       tCIDLib::TBoolean&  bBold
                            ,       tCIDLib::TBoolean&  bItalic)
{
    TFontSelDlg dlgSel;
    return dlgSel.bRun(wndOwner, strFaceName, c4Height, bBold, bItalic);
    return kCIDLib::True;
}

tCIDLib::TBoolean
TFacCIDWUtils::bSelectFontFace( const   TWindow&            wndOwner
                                ,       TString&            strFaceName
                                , const tCIDLib::TCard4     c4Height
                                , const tCIDLib::TBoolean   bBold
                                , const tCIDLib::TBoolean   bItalic)
{
    TFontFaceSelDlg dlgSel;
    return dlgSel.bRun(wndOwner, strFaceName, c4Height, bBold, bItalic);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TFacCIDWUtils::bSelectImage(const   TWindow&                wndOwner
                            , const TRefVector<TCIDImage>&  colImgs
                            , const TString&                strTitle
                            ,       tCIDLib::TCard4&        c4SelIndex)
{
    TImgSelDlg dlgSel;
    return dlgSel.bRun(wndOwner, colImgs, strTitle, c4SelIndex);
}


//
//  A wrapper around the simple single list selection dialog. Returns maxcard if not selection
//  was made.
//
tCIDLib::TCard4
TFacCIDWUtils::c4ListSelect(const   TWindow&            wndOwner
                            , const TString&            strTitle
                            , const tCIDLib::TStrList&  colList
                            , const tCIDLib::TCard4     c4InitSel
                            , const tCIDLib::TBoolean   bSort) const
{
    TCIDListSelDlg3 dlgSel;
    return dlgSel.c4Run(wndOwner, strTitle, colList, c4InitSel, bSort);
}


// A wrapper around the tri-choice dialog
tCIDLib::TCard4
TFacCIDWUtils::c4TriChoiceDlg(  const   TWindow&    wndOwner
                                , const TString&    strTitle
                                , const TString&    strInstruct
                                , const TString&    strOpt1
                                , const TString&    strOpt2
                                , const TString&    strOpt3) const
{
    TTriChoiceDlg dlgChoice;
    return dlgChoice.c4Run
    (
        wndOwner
        , strTitle
        , strInstruct
        , strOpt1
        , strOpt2
        , strOpt3
    );
}


// Wrappers around the error options dialog
tCIDWUtils::EErrOpts
TFacCIDWUtils::eGetErrOpt(  const   TWindow&            wndOwner
                            ,       tCIDLib::TBoolean&  bDoAll
                            , const TString&            strOpName
                            , const tCIDLib::TBoolean   bMulti
                            , const tCIDLib::TBoolean   bDiscard) const
{
    TErrOptsDlg dlgOpts;
    return dlgOpts.eRun(wndOwner, bDoAll, strOpName, bMulti, bDiscard);
}

tCIDWUtils::EErrOpts
TFacCIDWUtils::eGetErrOpt2( const   TWindow&            wndOwner
                            ,       tCIDLib::TBoolean&  bDoAll
                            , const TString&            strTarget
                            , const TString&            strErrMsg
                            , const tCIDLib::TBoolean   bMulti
                            , const tCIDLib::TBoolean   bDiscard) const
{
    TErrOptsDlg2 dlgOpts;
    return dlgOpts.eRun(wndOwner, bDoAll, strTarget, strErrMsg, bMulti, bDiscard);
}


// A wrapper around the overwrite opts
tCIDWUtils::EOverOpts
TFacCIDWUtils::eGetOverOpt( const   TWindow&            wndOwner
                            ,       tCIDLib::TBoolean&  bDoAll
                            , const TString&            strTarget
                            , const tCIDLib::TBoolean   bMultiMode) const
{
    TOverOptsDlg dlgOpts;
    return dlgOpts.eRun(wndOwner, bDoAll, strTarget, bMultiMode);
}


// A wrapper around the save options dialog
tCIDWUtils::ESaveOpts
TFacCIDWUtils::eGetSaveOpt( const   TWindow&            wndOwner
                            ,       tCIDLib::TBoolean&  bDoAgain
                            , const TString&            strSource
                            , const tCIDLib::TBoolean   bMultiMode) const
{
    TSaveOptsDlg dlgOpts;
    return dlgOpts.eRun(wndOwner, bDoAgain, strSource, bMultiMode);
}


// Show a list of key/value pairs
tCIDLib::TVoid
TFacCIDWUtils::ShowKVPList( const   TWindow&                wndOwner
                            , const tCIDLib::TKVPCollect&   colList
                            , const TString&                strTitle
                            , const TString&                strKeyTitle
                            , const TString&                strValueTitle) const
{
    TShowKVPListDlg dlgShowList;
    dlgShowList.Run(wndOwner, colList, strTitle, strKeyTitle, strValueTitle);
}


// Show a list of text values
tCIDLib::TVoid
TFacCIDWUtils::ShowList(const   TWindow&                wndOwner
                        , const TString&                strTitle
                        , const tCIDLib::TStrCollect&   colList) const
{
    TShowListDlg dlgShowList;
    dlgShowList.Run(wndOwner, colList, strTitle);
}


tCIDLib::TVoid
TFacCIDWUtils::ShowText(const   TWindow&    wndOwner
                        , const TString&    strTitle
                        , const TString&    strToShow) const
{
    TShowTextDlg dlgShow;
    dlgShow.Run(wndOwner, strTitle, strToShow);
}

