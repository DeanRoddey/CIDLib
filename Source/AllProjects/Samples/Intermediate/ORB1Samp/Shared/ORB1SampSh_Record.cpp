//
// FILE NAME: ORB1SampSh.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/17/2019
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
//  This is the main header for the facility that is shared between the client and
//  server sides. It defines the types and constants and such that they both need
//  to see.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// -------------------------------------------------------------------
//  Bring in our own private header which gets us what we need
// -------------------------------------------------------------------
#include "ORB1SampSh_.hpp"


// -------------------------------------------------------------------
//  Magic macros
// -------------------------------------------------------------------
RTTIDecls(TORB1SampRec,TObject)


// -------------------------------------------------------------------
//   CLASS: TORB1SampRec
//  PREFIX: rec
// -------------------------------------------------------------------
TORB1SampRec::TORB1SampRec() :

    m_c4Age(0)
    , m_c4RecordNum(kCIDLib::c4MaxCard)
    , m_eStatus(tORB1SampSh::EStatus::Count)
{
}

TORB1SampRec::TORB1SampRec( const   TString&                strFirst
                            , const TString&                strLast
                            , const tORB1SampSh::EStatus    eStatus
                            , const tCIDLib::TCard4         c4Age
                            , const tCIDLib::TCard4         c4RecordNum) :

    m_c4Age(c4Age)
    , m_c4RecordNum(c4RecordNum)
    , m_eStatus(eStatus)
    , m_strFirst(strFirst)
    , m_strFullName(strFirst.c4Length() + strLast.c4Length() + 1)
    , m_strLast(strLast)
{
    m_strFullName = m_strFirst;
    m_strFullName.Append(kCIDLib::chSpace);
    m_strFullName.Append(m_strLast);
}


TORB1SampRec::~TORB1SampRec()
{
}


// -------------------------------------------------------------------
//  TORB1SampRec: Protected, inherited methods
// -------------------------------------------------------------------
tCIDLib::TVoid TORB1SampRec::FormatTo(TTextOutStream& strmDest) const
{
    strmDest    << L"Name: " << m_strFirst << kCIDLib::chSpace << m_strLast
                << L", Status: " << m_eStatus << L", Age: " << m_c4Age
                << L", Record: " << m_c4RecordNum;
}


tCIDLib::TVoid TORB1SampRec::StreamFrom(TBinInStream& strmToReadFrom)
{
    strmToReadFrom  >> m_c4Age
                    >> m_strFirst
                    >> m_c4RecordNum
                    >> m_strLast
                    >> m_eStatus;

    // We don't stream the full name, so re-build it
    m_strFullName = m_strFirst;
    m_strFullName.Append(kCIDLib::chSpace);
    m_strFullName.Append(m_strLast);
}


tCIDLib::TVoid TORB1SampRec::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << m_c4Age
                    << m_strFirst
                    << m_c4RecordNum
                    << m_strLast
                    << m_eStatus;
}
