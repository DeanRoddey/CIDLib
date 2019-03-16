//
// FILE NAME: CIDBuild_MsgIdInfo.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/01/2000
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file implements the TIdInfo class.
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
//   CLASS: TIdInfo
//  PREFIX: idi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TIdInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TIdInfo::TIdInfo() :

    m_eType(ETypes::Msg)
    , m_c4Id(0)
{
}

TIdInfo::TIdInfo(const  TBldStr&        strName
                , const ETypes          eType
                , const tCIDLib::TCard4 c4Id
                , const TBldStr&        strText) :

    m_eType(eType)
    , m_strText(strText)
    , m_strName(strName)
    , m_c4Id(c4Id)
{
}

TIdInfo::~TIdInfo()
{
}

