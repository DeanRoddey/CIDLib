//
// FILE NAME: CIDMData_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/03/2015
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
//  This file implements the facility class for this facility.
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
#include    "CIDMData_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard members macro
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDMData,TFacility)




// ---------------------------------------------------------------------------
//   CLASS: TFacCIDMData
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDMData: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDMData::TFacCIDMData() :

    TFacility
    (
        L"CIDMData"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}

TFacCIDMData::~TFacCIDMData()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDMData: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Find the attribute with the indicated id and remove it
tCIDLib::TVoid
TFacCIDMData::RemoveAttr(       tCIDMData::TAttrList&   colList
                        , const TString&                strId)
{
    const tCIDLib::TCard4 c4Count = colList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TAttrData& attrCur = colList[c4Index];
        if (attrCur.strId().bCompareI(strId))
        {
            colList.RemoveAt(c4Index);
            break;
        }
    }
}


// Return the attribute data version of true/false strings
const TString& TFacCIDMData::strBoolAttr(const tCIDLib::TBoolean bValue) const
{
    return bValue ? kCIDMData::strAttrVal_True : kCIDMData::strAttrVal_False;
}



//
//  Find the attribute in the list that has the indicated id, and set various
//  attribute values.
//
tCIDLib::TVoid
TFacCIDMData::SetAttrEditType(          tCIDMData::TAttrList&   colList
                                , const TString&                strId
                                , const tCIDMData::EAttrEdTypes eToSet) const
{
    const tCIDLib::TCard4 c4Count = colList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TAttrData& attrCur = colList[c4Index];
        if (attrCur.strId().bCompareI(strId))
        {
            attrCur.eEditType(eToSet);
            break;
        }
    }
}

tCIDLib::TVoid
TFacCIDMData::SetAttrLimits(        tCIDMData::TAttrList&   colList
                            , const TString&                strId
                            , const TString&                strToSet) const
{
    const tCIDLib::TCard4 c4Count = colList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TAttrData& attrCur = colList[c4Index];
        if (attrCur.strId().bCompareI(strId))
        {
            attrCur.strLimits(strToSet);
            break;
        }
    }
}

tCIDLib::TVoid
TFacCIDMData::SetAttrName(          tCIDMData::TAttrList&   colList
                            , const TString&                strId
                            , const TString&                strToSet) const
{
    const tCIDLib::TCard4 c4Count = colList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TAttrData& attrCur = colList[c4Index];
        if (attrCur.strId().bCompareI(strId))
        {
            attrCur.strName(strToSet);
            break;
        }
    }
}


