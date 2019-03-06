#include "VideoDemoSh_.hpp"

RTTIDecls(TDemoRecord,TObject)


TDemoRecord::TDemoRecord() :

    m_c4Age(0)
    , m_c4RecordNum(kCIDLib::c4MaxCard)
    , m_eStatus(tVideoDemoSh::EStatus::Count)
{
}

TDemoRecord::TDemoRecord(const  TString&                strFirst
                        , const TString&                strLast
                        , const tVideoDemoSh::EStatus   eStatus
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

TDemoRecord::TDemoRecord(const TDemoRecord& recSrc) :

    m_c4Age(recSrc.m_c4Age)
    , m_c4RecordNum(recSrc.m_c4RecordNum)
    , m_eStatus(recSrc.m_eStatus)
    , m_strFirst(recSrc.m_strFirst)
    , m_strFullName(recSrc.m_strFullName)
    , m_strLast(recSrc.m_strLast)
{
}

TDemoRecord::~TDemoRecord()
{
}

TDemoRecord& TDemoRecord::operator=(const TDemoRecord& recSrc)
{
    if (&recSrc != this)
    {
        m_c4Age        = recSrc.m_c4Age;
        m_c4RecordNum  = recSrc.m_c4RecordNum;
        m_eStatus      = recSrc.m_eStatus;
        m_strFirst     = recSrc.m_strFirst;
        m_strFullName  = recSrc.m_strFullName;
        m_strLast      = recSrc.m_strLast;
    }
    return *this;
}



tCIDLib::TVoid TDemoRecord::FormatTo(TTextOutStream& strmDest) const
{
    strmDest    << L"Name: " << m_strFirst << kCIDLib::chSpace << m_strLast
                << L", Status: " << m_eStatus << L", Age: " << m_c4Age
                << L", Record: " << m_c4RecordNum;
}


tCIDLib::TVoid TDemoRecord::StreamFrom(TBinInStream& strmToReadFrom)
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


tCIDLib::TVoid TDemoRecord::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << m_c4Age
                    << m_strFirst
                    << m_c4RecordNum
                    << m_strLast
                    << m_eStatus;
}
