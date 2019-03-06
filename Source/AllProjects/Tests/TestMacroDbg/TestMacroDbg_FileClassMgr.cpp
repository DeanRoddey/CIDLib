//
// FILE NAME: TestMacroDbg_FileClassMgr.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/14/2003
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
// ---------------------------------------------------------------------------
#include    "TestMacroDbg.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TMacroDbgClassMgr,TMEngFixedBaseClassMgr)



// ---------------------------------------------------------------------------
//  CLASS: TMacroDbgClassMgr
// PREFIX: mecm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMacroDbgClassMgr: Constructors and Destructor
// ---------------------------------------------------------------------------
TMacroDbgClassMgr::TMacroDbgClassMgr(TMEngDbgFrameWnd* const pwndOwner) :

    m_pwndOwner(pwndOwner)
{
}

TMacroDbgClassMgr::TMacroDbgClassMgr(const  TString&                strBasePath
                                    ,       TMEngDbgFrameWnd* const pwndOwner) :

    TMEngFixedBaseClassMgr(strBasePath)
    , m_pwndOwner(pwndOwner)
{
}

TMacroDbgClassMgr::~TMacroDbgClassMgr()
{
}


// ---------------------------------------------------------------------------
//  TMacroDbgClassMgr: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::EOpenRes
TMacroDbgClassMgr::eDoSelect(       TString&                strToFill
                            , const tCIDMacroEng::EResModes    eMode)
{
    // Set up the single file type we want to select from
    tCIDLib::TKVPList colTypes;
    colTypes.objAdd
    (
        TKeyValuePair
        (
            facCIDMacroEng().strMacroFileDescr()
            , facCIDMacroEng().strMacroFileSelWC()
        )
    );

    //
    //  Get the current directory, which is what we want the dialog
    //  to start in.
    //
    const TString&      strStart = strBasePath();
    tCIDLib::TStrList   colSel;
    const tCIDLib::TBoolean bRes = facCIDCtrls().bOpenFileDlg
    (
        *m_pwndOwner
        , L"Select a MacroFile"
        , strStart
        , colSel
        , colTypes
        , tCIDLib::eOREnumBits
          (
            tCIDCtrls::EFOpenOpts::StrictTypes
            , tCIDCtrls::EFOpenOpts::PathMustExist
          )
    );

    tCIDLib::EOpenRes eRes = tCIDLib::EOpenRes::Cancel;
    if (bRes)
    {
        TPathStr pathTmp = colSel[0];
        pathTmp.bRemoveExt();
        pathTmp.Cut(0, strStart.c4Length() + 1);
        pathTmp.Prepend(L"MEng.");
        pathTmp.bReplaceChar(L'\\', kCIDLib::chPeriod);

        strToFill = pathTmp;

        if (TFileSys::bExists(colSel[0]))
            eRes = tCIDLib::EOpenRes::ExistingFile;
        else
            eRes = tCIDLib::EOpenRes::NewFile;
    }
    return eRes;
}


