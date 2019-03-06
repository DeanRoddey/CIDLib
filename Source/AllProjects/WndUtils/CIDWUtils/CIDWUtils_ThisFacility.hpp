//
// FILE NAME: CIDWUtils_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/23/2015
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
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TFacCIDWUtils
//  PREFIX: fac
// ---------------------------------------------------------------------------
class CIDWUTILSEXP TFacCIDWUtils : public TGUIFacility
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDWUtils();

        TFacCIDWUtils(const TFacCIDWUtils&) = delete;

        ~TFacCIDWUtils();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDWUtils& operator=(const TFacCIDWUtils&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCfgSerialPort
        (
            const   TWindow&                wndOwner
            ,       TCommPortCfg&           cpcfgToEdit
        );

        tCIDLib::TBoolean bEditText
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const TString&                strOrgText
            ,       TString&                strNewText
            , const tCIDLib::TBoolean       bTakeCRs
        );

        tCIDLib::TBoolean bEditTime
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            ,       tCIDLib::TEncodedTime&  enctEdit
        );

        tCIDLib::TBoolean bGetReplaceOpts
        (
            const   TWindow&                wndOwner
            ,       TString&                strToFind
            ,       TString&                strRepWith
            ,       tCIDLib::TStrList&      colFindRecall
            ,       tCIDLib::TStrList&      colRepRecall
            ,       tCIDLib::TBoolean&      bFullMatchOnly
            ,       tCIDLib::TBoolean&      bSelectionOnly
            , const tCIDWUtils::EFindFlags  eFlags
        )   const;

        tCIDLib::TBoolean bGetSearchOpts
        (
            const   TWindow&                wndOwner
            ,       TString&                strToFind
            ,       tCIDLib::TStrList&      colRecallList
            ,       tCIDLib::TBoolean&      bFullMatchOnly
            ,       tCIDLib::TBoolean&      bSelectionOnly
            , const tCIDWUtils::EFindFlags  eFlags
        )   const;

        tCIDLib::TBoolean bGetSize
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            ,       TSize&                  szToFill
            , const TSize&                  szInit
            , const TSize&                  szMin
            , const TSize&                  szMax
            , const tCIDLib::TBoolean       bForceAR
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        )   const;

        tCIDLib::TBoolean bGetSize
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            ,       TSize&                  szToFill
            , const TSize&                  szMin
            , const TSize&                  szMax
            , const tCIDLib::TBoolean       bForceAR
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        )   const;

        tCIDLib::TBoolean bGetText
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const TString&                strOrgText
            ,       TString&                strNewText
        )   const;

        tCIDLib::TBoolean bGetText2
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const TString&                strInstruct
            , const TString&                strOrgText
            ,       TString&                strNewText
            , const TString&                strRegEx
            , const tCIDLib::TBoolean       bSecret = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bListSelect
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const tCIDLib::TStrCollect&   colList
            ,       TString&                strSelText
        )   const;

        tCIDLib::TBoolean bListMultiSelect
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const tCIDLib::TStrCollect&   colList
            ,       tCIDLib::TStrCollect&   colSelections
        )   const;

        tCIDLib::TBoolean bListMultiSelect2
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const TString&                strDataTitle
            , const tCIDLib::TStrList&      colAvail
            ,       tCIDLib::TStrList&      colSelected
        )   const;

        tCIDLib::TBoolean bKVPListSelect
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const tCIDLib::TKVPCollect&   colList
            ,       TKeyValuePair&          kvalToFill
            , const tCIDLib::TBoolean       bShowValue
        )   const;

        tCIDLib::TBoolean bKVPListMulitiSelect
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const TString&                strDataTitle
            , const tCIDLib::TKVPList&      colAvail
            ,       tCIDLib::TKVPList&      colSelected
        );

        tCIDLib::TBoolean bListSelect2
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            ,       tCIDLib::TStrList&      colList
            ,       TString&                strSelText
            , const tCIDWUtils::EListValFmts eFmt
        )   const;

        tCIDLib::TBoolean bSelectColor
        (
            const   TWindow&                wndOwner
            ,       TRGBClr&                rgbSelected
        );

        tCIDLib::TBoolean bSelectFont
        (
            const   TWindow&                wndOwner
            ,       TString&                strFaceName
            ,       tCIDLib::TCard4&        c4Height
            ,       tCIDLib::TBoolean&      bBold
            ,       tCIDLib::TBoolean&      bItalic
        );

        tCIDLib::TBoolean bSelectFontFace
        (
            const   TWindow&                wndOwner
            ,       TString&                strFaceName
            , const tCIDLib::TCard4         c4Height
            , const tCIDLib::TBoolean       bBold
            , const tCIDLib::TBoolean       bItalic
        );

        tCIDLib::TBoolean bSelectImage
        (
            const   TWindow&                wndOwner
            , const TRefVector<TCIDImage>&  colImgs
            , const TString&                strTitle
            ,       tCIDLib::TCard4&        c4SelIndex
        );

        tCIDLib::TCard4 c4ListSelect
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const tCIDLib::TStrList&      colList
            , const tCIDLib::TCard4         c4InitSel
            , const tCIDLib::TBoolean       bSort
        )   const;

        tCIDLib::TCard4 c4TriChoiceDlg
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const TString&                strInstruct
            , const TString&                strOpt1
            , const TString&                strOpt2
            , const TString&                strOpt3
        )   const;

        tCIDWUtils::EErrOpts eGetErrOpt
        (
            const   TWindow&                wndOwner
            ,       tCIDLib::TBoolean&      bDoAll
            , const TString&                strOpName
            , const tCIDLib::TBoolean       bMultiMode
            , const tCIDLib::TBoolean       bDiscardMode
        )   const;

        tCIDWUtils::EErrOpts eGetErrOpt2
        (
            const   TWindow&                wndOwner
            ,       tCIDLib::TBoolean&      bDoAll
            , const TString&                strTarget
            , const TString&                strErrMsg
            , const tCIDLib::TBoolean       bMultiMode
            , const tCIDLib::TBoolean       bDiscardMode
        )   const;

        tCIDWUtils::EOverOpts eGetOverOpt
        (
            const   TWindow&                wndOwner
            ,       tCIDLib::TBoolean&      bDoAll
            , const TString&                strTarget
            , const tCIDLib::TBoolean       bMultiMode
        )   const;

        tCIDWUtils::ESaveOpts eGetSaveOpt
        (
            const   TWindow&                wndOwner
            ,       tCIDLib::TBoolean&      bDoAll
            , const TString&                strSource
            , const tCIDLib::TBoolean       bMultiMode
        )   const;

        tCIDLib::TVoid ShowKVPList
        (
            const   TWindow&                wndOwner
            , const tCIDLib::TKVPCollect&   colList
            , const TString&                strTitle
            , const TString&                strKeyTitle
            , const TString&                strValueTitle
        )   const;

        tCIDLib::TVoid ShowList
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const tCIDLib::TStrCollect&   colList
        )   const;

        tCIDLib::TVoid ShowText
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const TString&                strToShow
        )   const;


    private :

        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDWUtils,TGUIFacility)
};

#pragma CIDLIB_POPPACK

