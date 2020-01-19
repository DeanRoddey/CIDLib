//
// FILE NAME: CIDAI_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/30/2016
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
#include    "CIDAI_.hpp"
#include    "CIDAI_BTDefNodeFactory_.hpp"
#include    "CIDAI_BTParser_.hpp"



// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDAI,TFacility)



// ---------------------------------------------------------------------------
//   CLASS: TFacCIDAI
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDAI: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDAI::TFacCIDAI() :

    TFacility
    (
        L"CIDAI"
        , tCIDLib::EModTypes::SharedLib
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
    , m_colFactList
      (
        tCIDLib::EAdoptOpts::Adopt
        , 109
        , TStringKeyOps(kCIDLib::False)
        , &TAIBTNodeFact::strKey
      )
{
    // Register our default node factory
    RegisterFactory(new TAIBTDefNodeFact);
}

TFacCIDAI::~TFacCIDAI()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDAI: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  See if we have a factory registered under the passed factory type. If so, return
//  a pointer to it.
//
TAIBTNodeFact& TFacCIDAI::nfactFind(const TString& strFactType)
{
    // Let's find the factory
    TAIBTNodeFact* pnfactTar = m_colFactList.pobjFindByKey(strFactType, kCIDLib::False);
    if (!pnfactTar)
    {
        facCIDAI().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kAIErrs::errcBT_FactNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strFactType
        );
    }

    return *pnfactTar;
}


tCIDLib::TVoid TFacCIDAI::RegisterFactory(TAIBTNodeFact* const pnfactToAdopt)
{
    m_colFactList.Add(pnfactToAdopt);
}

