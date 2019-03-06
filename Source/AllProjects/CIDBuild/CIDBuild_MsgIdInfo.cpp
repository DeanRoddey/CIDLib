//
// FILE NAME: CIDBuild_MsgIdInfo.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/01/2000
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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

