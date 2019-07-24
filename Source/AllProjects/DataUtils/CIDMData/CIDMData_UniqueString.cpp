//
// FILE NAME: CIDMData_UniqueStr.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/29/2019
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
//  This file implements the unique string class
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
#include    "CIDMData_.hpp"


// ---------------------------------------------------------------------------
//  CLASS: TUniqueStr
// PREFIX: ustr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TUniqueString: Constructors and Destructor
// ---------------------------------------------------------------------------
TUniqueString::TUniqueString(const  tCIDLib::TCard4     c4MaxId
                            , const tCIDLib::TCard4     c4Modulus) :

    m_c4MaxId(c4MaxId)
    , m_c4NextId(1)
    , m_colUniqueList
      (
          c4Modulus
          , TStringKeyOps(kCIDLib::False)
          , [](const TAString& astrCur) -> const TString& { return astrCur.m_strVal; }
      )
    , m_colURLIdView(tCIDLib::EAdoptOpts::NoAdopt)
{
}


// ---------------------------------------------------------------------------
//  TUniqueString: Public operators
// ---------------------------------------------------------------------------
const TString& TUniqueString::operator[](const tCIDLib::TCard4 c4Id) const
{
    CheckId(c4Id);
    return m_colURLIdView[c4Id - 1]->m_strVal;
}


// ---------------------------------------------------------------------------
//  TUniqueString: Public, non-virtual methods
// ---------------------------------------------------------------------------
const tCIDLib::TBoolean TUniqueString::bHasString(const TString& strToCheck) const
{
    return m_colUniqueList.bKeyExists(strToCheck);
}


tCIDLib::TCard4 TUniqueString::c4StringToId(const TString& strToFind) const
{
    const TAString* pastrTest = m_colUniqueList.pobjFindByKey(strToFind);
    if (!pastrTest)
        return kCIDLib::c4MaxCard;
    return pastrTest->m_c4Id;
}


const tCIDLib::TCard4 TUniqueString::c4AddString(const TString& strToAdd)
{
    const TAString* pastrTest = m_colUniqueList.pobjFindByKey(strToAdd);
    if (!pastrTest)
    {
        // Don't got it, so add with next id and add to the id view
        pastrTest = &m_colUniqueList.objPlace(strToAdd, m_c4NextId++);
        m_colURLIdView.Add(pastrTest);
    }
    return pastrTest->m_c4Id;
}


// Just put us back to the start conditions
tCIDLib::TVoid TUniqueString::Reset()
{
    m_colURLIdView.RemoveAll();
    m_colUniqueList.RemoveAll();
    m_c4NextId = 1;
}



// ---------------------------------------------------------------------------
//  TUniqueString: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TUniqueString::CheckId(const tCIDLib::TCard4 c4ToCheck) const
{
    if (!c4ToCheck || (c4ToCheck > m_c4MaxId))
    {
        facCIDMData().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMDataErrs::errcUStr_InvalidId
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4ToCheck)
        );
    }
}
