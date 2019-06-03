//
// FILE NAME: ORB1SampS_IntfImpl.hpp
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
//  This is the header for our implementation of the server side ORB1 sample
//  interface. We just have to override the virtual methods that define the
//  interface.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// ---------------------------------------------------------------------------
//  Bring in our own public header
// ---------------------------------------------------------------------------
#include    "ORB1SampS.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TORB1SampIntfImpl,TORB1SampServerBase)


// ---------------------------------------------------------------------------
//   TORB1SampIntfImpl: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TORB1SampIntfImpl::bQueryByName(const   TString&        strToFind
                                ,       TORB1SampRec&   recFound)
{
    tCIDLib::TBoolean bRet = kCIDLib::False;
    m_colPatients.bForEach
    (
        [&bRet, &recFound, &strToFind](const TORB1SampRec& recCur)
        {
            if (bRet = strToFind.bCompareI(recCur.m_strFullName))
                recFound = recCur;
            return !bRet;
        }
    );
    return bRet;
}

tCIDLib::TBoolean
TORB1SampIntfImpl::bQueryAllStatus(const    tORB1SampSh::EStatus   eToFind
                                    ,       TVector<TORB1SampRec>&   colFound)
{
    colFound.RemoveAll();
    m_colPatients.bForEach
    (
        [eToFind, &colFound](const TORB1SampRec& recCur)
        {
            if (recCur.m_eStatus == eToFind)
                colFound.objAdd(recCur);
            return kCIDLib::True;
        }
    );
    return !colFound.bIsEmpty();
}


// ---------------------------------------------------------------------------
//   TORB1SampIntfImpl: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  The ORB calls these. We use the init to load up some dummy records
tCIDLib::TVoid TORB1SampIntfImpl::Initialize()
{
    // Load up some dummy records
    m_colPatients.objAdd(TORB1SampRec(L"Angie", L"Night", tORB1SampSh::EStatus::Married, 34, 1));
    m_colPatients.objAdd(TORB1SampRec(L"Tom", L"Jones", tORB1SampSh::EStatus::Single, 28, 2));
    m_colPatients.objAdd(TORB1SampRec(L"Nancy", L"Sinatra", tORB1SampSh::EStatus::Single, 24, 3));
    m_colPatients.objAdd(TORB1SampRec(L"Robin", L"Ford", tORB1SampSh::EStatus::Widowed, 29, 4));
    m_colPatients.objAdd(TORB1SampRec(L"Reggie", L"Singleton", tORB1SampSh::EStatus::Divorced, 54, 5));
    m_colPatients.objAdd(TORB1SampRec(L"Jack", L"Anthony", tORB1SampSh::EStatus::Married, 56, 6));
    m_colPatients.objAdd(TORB1SampRec(L"Scott", L"Lassiter", tORB1SampSh::EStatus::Married, 54, 7));
    m_colPatients.objAdd(TORB1SampRec(L"Jane", L"Doe", tORB1SampSh::EStatus::Single, 16, 8));
}

tCIDLib::TVoid TORB1SampIntfImpl::Terminate()
{
}

