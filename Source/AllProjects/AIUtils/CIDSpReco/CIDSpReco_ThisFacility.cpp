//
// FILE NAME: CIDSpReco_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/30/2016
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the facilty class for this facility.
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
#include    "CIDSpReco_.hpp"

#include    <Shlwapi.h>
#include    <sphelper.h>


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDSpReco,TFacility)



// ---------------------------------------------------------------------------
//   CLASS: TFacCIDSpReco
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDSpReco: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDSpReco::TFacCIDSpReco() :

    TFacility
    (
        L"CIDSpReco"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}

TFacCIDSpReco::~TFacCIDSpReco()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDSpReco: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TFacCIDSpReco::CompileGrammarTo(const   TString& strGrammarFile
                                , const TString& strTarFile)
{
    ISpStream*  pInStream;
    ISpStream*  pOutStream;
    HRESULT     hRes;

    // Try to open the input file
    hRes = ::SPBindToFile
    (
        strGrammarFile.pszBuffer()
        , SPFM_OPEN_READONLY
        , &pInStream
        , NULL
        , NULL
        , NULL
    );

    if (FAILED(hRes))
    {
        ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcGram_SrcFileFailed
            , strGrammarFile
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(hRes, tCIDLib::ERadices::Hex)
        );
    }
    TCOMJanitor<ISpStream> janInStream(&pInStream);

    // Try to create the output file
    hRes = ::SPBindToFile
    (
        strTarFile.pszBuffer()
        , SPFM_CREATE_ALWAYS
        , &pOutStream
        , NULL
        , NULL
        , NULL
    );

    if (FAILED(hRes))
    {
        ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcGram_TarFileFailed
            , strTarFile
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(hRes, tCIDLib::ERadices::Hex)
        );
    }
    TCOMJanitor<ISpStream> janOutStream(&pOutStream);

    // Try to create the compiler
    ISpGrammarCompiler* pCompiler = nullptr;
    hRes = ::CoCreateInstance
    (
        CLSID_SpW3CGrammarCompiler
        , NULL
        , CLSCTX_INPROC_SERVER
        , __uuidof(ISpGrammarCompiler)
        , (void**)&pCompiler
    );

    if (FAILED(hRes))
    {
        ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcGram_CreateCompiler
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(hRes, tCIDLib::ERadices::Hex)
        );
    }
    TCOMJanitor<ISpGrammarCompiler> janCompiler(&pCompiler);

    // And finally we can do the compile
    hRes = pCompiler->CompileStream
    (
        pInStream, pOutStream, nullptr, nullptr, nullptr, 0
    );

    if (FAILED(hRes))
    {
        ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcGram_CompileFailed
            , strGrammarFile
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(hRes, tCIDLib::ERadices::Hex)
        );
    }
}
