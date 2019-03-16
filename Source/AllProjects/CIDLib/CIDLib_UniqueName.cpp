//
// FILE NAME: CIDLib_UniqueName.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/10/2001
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TUniqueName,TObject)



// ---------------------------------------------------------------------------
//  TUniqueName: Constructors and Destructor
// ---------------------------------------------------------------------------
TUniqueName::TUniqueName() :

    m_c4Number(1)
{
}

TUniqueName::TUniqueName(const  TString&        strText
                        , const tCIDLib::TCard4 c4Value) :

    m_c4Number(c4Value)
    , m_crsLock()
    , m_strText(strText)
{
    // Validate that it has the required replacement parm
    ValidateText(strText);
}

TUniqueName::TUniqueName(const  tCIDLib::TCh* const pszText
                        , const tCIDLib::TCard4     c4Value) :

    m_c4Number(c4Value)
    , m_crsLock()
    , m_strText(pszText)
{
    // Validate that it has the required replacement parm
    ValidateText(m_strText);
}

TUniqueName::~TUniqueName()
{
}


// ---------------------------------------------------------------------------
//  TUniqueName: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TUniqueName::c4SetSequenceNum(const tCIDLib::TCard4 c4NewNum)
{
    TCritSecLocker crslSync(&m_crsLock);
    m_c4Number = c4NewNum;
    return c4NewNum;
}

tCIDLib::TVoid TUniqueName::QueryNewName(TString& strToFill)
{
    // If debugging, make sure the text was set
    CIDAssert(!m_strText.bIsEmpty(), L"The unique name object has not be set up yet");

    // Get the current value out and add 1 for next time
    tCIDLib::TCard4 c4New;
    {
        TCritSecLocker crslSync(&m_crsLock);
        c4New = m_c4Number++;
    }

    // Get a copy of the text and replace the parameter
    strToFill = m_strText;
    strToFill.eReplaceToken(TCardinal(c4New), L'1');
}

TString TUniqueName::strQueryNewName()
{
    // If debugging, make sure the text was set
    CIDAssert(!m_strText.bIsEmpty(), L"The unique name object has not be set up yet");

    // Get the current value and add 1 for the next time
    tCIDLib::TCard4 c4New;
    {
        TCritSecLocker crslSync(&m_crsLock);
        c4New = m_c4Number++;
    }

    // Get a copy of the text and replace the parameter
    TString strRet = m_strText;
    strRet.eReplaceToken(TCardinal(c4New), L'1');

    return strRet;
}


const TString& TUniqueName::strText() const
{
    return m_strText;
}

const TString& TUniqueName::strText(const TString& strNew)
{
    // Validate that it has the required replacement parm
    ValidateText(strNew);

    TCritSecLocker crslSync(&m_crsLock);
    m_strText = strNew;

    return m_strText;
}


// ---------------------------------------------------------------------------
//  TUniqueName: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TUniqueName::ValidateText(const TString& strToValidate)
{
    if (!strToValidate.bTokenExists(L'1'))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcUName_BadText
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , strToValidate
        );
    }
}

