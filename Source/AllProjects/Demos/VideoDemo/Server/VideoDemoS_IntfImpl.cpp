#include    "VideoDemoS.hpp"
RTTIDecls(TVideoDemoIntfImpl,TVideoDemoServerBase)


tCIDLib::TBoolean
TVideoDemoIntfImpl::bQueryByName(const TString& strToFind
                                , TDemoRecord& recFound)
{
    tCIDLib::TBoolean bRet = kCIDLib::False;
    m_colPatients.ForEach
    (
        [&bRet, &recFound, &strToFind](const TDemoRecord& recCur)
        {
            if (bRet = strToFind.bCompareI(recCur.m_strFullName))
                recFound = recCur;
            return !bRet;
        }
    );
    return bRet;
}

tCIDLib::TBoolean
TVideoDemoIntfImpl::bQueryAllStatus(const   tVideoDemoSh::EStatus   eToFind
                                    ,       TVector<TDemoRecord>&   colFound)
{
    colFound.RemoveAll();
    m_colPatients.ForEach
    (
        [eToFind, &colFound](const TDemoRecord& recCur)
        {
            if (recCur.m_eStatus == eToFind)
                colFound.objAdd(recCur);
            return kCIDLib::True;
        }
    );
    return !colFound.bIsEmpty();
}


tCIDLib::TVoid TVideoDemoIntfImpl::Initialize()
{
    // Load up some dummy records
    m_colPatients.objAdd(TDemoRecord(L"Angie", L"Night", tVideoDemoSh::EStatus::Married, 34, 1));
    m_colPatients.objAdd(TDemoRecord(L"Tom", L"Jones", tVideoDemoSh::EStatus::Single, 28, 2));
    m_colPatients.objAdd(TDemoRecord(L"Nancy", L"Sinatra", tVideoDemoSh::EStatus::Single, 24, 3));
    m_colPatients.objAdd(TDemoRecord(L"Robin", L"Ford", tVideoDemoSh::EStatus::Widowed, 29, 4));
    m_colPatients.objAdd(TDemoRecord(L"Reggie", L"Singleton", tVideoDemoSh::EStatus::Divorced, 54, 5));
    m_colPatients.objAdd(TDemoRecord(L"Jack", L"Anthony", tVideoDemoSh::EStatus::Married, 56, 6));
    m_colPatients.objAdd(TDemoRecord(L"Scott", L"Lassiter", tVideoDemoSh::EStatus::Married, 54, 7));
    m_colPatients.objAdd(TDemoRecord(L"Jane", L"Doe", tVideoDemoSh::EStatus::Single, 16, 8));
}

tCIDLib::TVoid TVideoDemoIntfImpl::Terminate()
{
}

