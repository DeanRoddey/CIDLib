//
// FILE NAME: CIDMacroEng_MethodInfo.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/27/2003
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
//  This file implements TMEngMethodInfo class. This class describes the
//  immutable characteristics of a method, e.g. its name, return type,
//  parameters, etc... It doesn't contain the actual implementation info,
//  which can exist in multiple derived classes, and which includes the
//  compiled opcodes, locals info, and such. That is in TMEngMethodImpl.
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
#include    "CIDMacroEng_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TMEngMethodInfo,TMEngNamedItem)


// ---------------------------------------------------------------------------
//  CLASS: TMEngMethodInfo
// PREFIX: methi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngMethodInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngMethodInfo::strKey(const TMEngMethodInfo& methiSrc)
{
    return methiSrc.strName();
}


// ---------------------------------------------------------------------------
//  TMEngMethodInfo: Destructor
// ---------------------------------------------------------------------------
TMEngMethodInfo::TMEngMethodInfo() :

    TMEngNamedItem()
    , m_bCtor(kCIDLib::False)
    , m_c2RetClassId(0)
    , m_colParmList(4UL)
    , m_eConst(tCIDMacroEng::EConstTypes::NonConst)
    , m_eExtend(tCIDMacroEng::EMethExt::Final)
    , m_eVisibility(tCIDMacroEng::EVisTypes::Private)
{
}

TMEngMethodInfo::TMEngMethodInfo(const  TString&                strName
                                , const tCIDLib::TCard2         c2RetClassId
                                , const tCIDMacroEng::EVisTypes    eVisibility
                                , const tCIDMacroEng::EMethExt     eExtend
                                , const tCIDMacroEng::EConstTypes  eConst) :
    TMEngNamedItem(strName)
    , m_bCtor(kCIDLib::False)
    , m_c2RetClassId(c2RetClassId)
    , m_colParmList(4UL)
    , m_eConst(eConst)
    , m_eExtend(eExtend)
    , m_eVisibility(eVisibility)
{
}

TMEngMethodInfo::TMEngMethodInfo(const  TString&                strName
                                , const tCIDMacroEng::EIntrinsics  eRetType
                                , const tCIDMacroEng::EVisTypes    eVisibility
                                , const tCIDMacroEng::EMethExt     eExtend
                                , const tCIDMacroEng::EConstTypes  eConst) :
    TMEngNamedItem(strName)
    , m_bCtor(kCIDLib::False)
    , m_c2RetClassId(tCIDLib::TCard2(eRetType))
    , m_colParmList(4UL)
    , m_eConst(eConst)
    , m_eExtend(eExtend)
    , m_eVisibility(eVisibility)
{
}

TMEngMethodInfo::TMEngMethodInfo(const TMEngMethodInfo& methiToCopy) :

    TMEngNamedItem(methiToCopy)
    , m_bCtor(methiToCopy.m_bCtor)
    , m_c2RetClassId(methiToCopy.m_c2RetClassId)
    , m_colParmList(methiToCopy.m_colParmList)
    , m_eConst(methiToCopy.m_eConst)
    , m_eExtend(methiToCopy.m_eExtend)
    , m_eVisibility(methiToCopy.m_eVisibility)
{
}

TMEngMethodInfo::~TMEngMethodInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngMethodInfo: Public operators
// ---------------------------------------------------------------------------
TMEngMethodInfo&
TMEngMethodInfo::operator=(const TMEngMethodInfo& methiToAssign)
{
    if (this != &methiToAssign)
    {
        TParent::operator=(methiToAssign);
        m_bCtor         = methiToAssign.m_bCtor;
        m_c2RetClassId  = methiToAssign.m_c2RetClassId;
        m_colParmList   = methiToAssign.m_colParmList;
        m_eConst        = methiToAssign.m_eConst;
        m_eExtend       = methiToAssign.m_eExtend;
        m_eVisibility   = methiToAssign.m_eVisibility;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TMEngMethodInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngMethodInfo::bHasParm(const TString& strToCheck) const
{
    const tCIDLib::TCard4 c4Count = m_colParmList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colParmList[c4Index].strName() == strToCheck)
            return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean TMEngMethodInfo::bIsConst() const
{
    return (m_eConst == tCIDMacroEng::EConstTypes::Const);
}


tCIDLib::TBoolean TMEngMethodInfo::bIsCtor() const
{
    return m_bCtor;
}

tCIDLib::TBoolean TMEngMethodInfo::bIsCtor(const tCIDLib::TBoolean bToSet)
{
    m_bCtor = bToSet;
    return m_bCtor;
}


tCIDLib::TBoolean
TMEngMethodInfo::bIsLegalOverride(const TMEngMethodInfo& methiToCheck)
{
    // The name, visibility, constness, and return type must be identical
    if ((strName() != methiToCheck.strName())
    ||  (m_eConst != methiToCheck.m_eConst)
    ||  (m_eVisibility != methiToCheck.m_eVisibility)
    ||  (m_c2RetClassId != methiToCheck.m_c2RetClassId))
    {
        return kCIDLib::False;
    }

    //
    //  The extend type can mostly only go forward in the override. So it
    //  must be equal to or greater than the old one. But, the one exception
    //  is required to non-final.
    //
    if (methiToCheck.m_eExtend < m_eExtend)
    {
        if ((methiToCheck.m_eExtend != tCIDMacroEng::EMethExt::NonFinal)
        ||  (m_eExtend != tCIDMacroEng::EMethExt::Required))
        {
            return kCIDLib::False;
        }
    }

    // It's legal, so update ours to match
    m_eExtend = methiToCheck.m_eExtend;

    const tCIDLib::TCard4 c4Count = m_colParmList.c4ElemCount();
    if (c4Count != methiToCheck.m_colParmList.c4ElemCount())
        return kCIDLib::False;

    //
    //  They are the same, so check the direction and type of each parameter.
    //  They must be the same and in the same order. We we take the parm
    //  names of the source object's parms, so that our parms match the
    //  actual names used. Our current names are those used by the base
    //  class.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TMEngParmInfo& mepiCur = m_colParmList[c4Index];
        if (!mepiCur.bIsLegalOverride(methiToCheck.m_colParmList[c4Index]))
            return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TCard2 TMEngMethodInfo::c2AddParm(const TMEngParmInfo& mepiToAdd)
{
    // Make sure we don't max out on parms
    if (m_colParmList.c4ElemCount() == kMacroEng::c4MaxMethodParms)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcMeth_TooManyParms
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
            , strName()
            , TCardinal(kMacroEng::c4MaxMethodParms)
        );
    }

    facCIDMacroEng().CheckIdOverflow(m_colParmList.c4ElemCount(), L"method parameters");

    TMEngParmInfo& mepiNew = m_colParmList.objAdd(mepiToAdd);
    mepiNew.c2Id(tCIDLib::TCard2(m_colParmList.c4ElemCount() - 1));
    return mepiNew.c2Id();
}


tCIDLib::TCard2
TMEngMethodInfo::c2AddInParm(const  TString&        strName
                            , const tCIDLib::TCard2 c2ClassId)
{
    // Make sure we don't max out on parms
    if (m_colParmList.c4ElemCount() == kMacroEng::c4MaxMethodParms)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcMeth_TooManyParms
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
            , strName
            , TCardinal(kMacroEng::c4MaxMethodParms)
        );
    }

    facCIDMacroEng().CheckIdOverflow(m_colParmList.c4ElemCount(), L"method parameters");

    TMEngParmInfo& mepiNew = m_colParmList.objAdd
    (
        TMEngParmInfo(strName, c2ClassId, tCIDMacroEng::EParmDirs::In)
    );
    mepiNew.c2Id(tCIDLib::TCard2(m_colParmList.c4ElemCount() - 1));
    return mepiNew.c2Id();
}

tCIDLib::TCard2
TMEngMethodInfo::c2AddInParm(const  TString&                strName
                            , const tCIDMacroEng::EIntrinsics  eType)
{
    return c2AddInParm(strName, tCIDLib::TCard2(eType));
}

tCIDLib::TCard2
TMEngMethodInfo::c2AddInOutParm(const   TString&        strName
                                , const tCIDLib::TCard2 c2ClassId)
{
    // Make sure we don't max out on parms
    if (m_colParmList.c4ElemCount() == kMacroEng::c4MaxMethodParms)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcMeth_TooManyParms
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
            , strName
            , TCardinal(kMacroEng::c4MaxMethodParms)
        );
    }

    facCIDMacroEng().CheckIdOverflow(m_colParmList.c4ElemCount(), L"method parameters");

    TMEngParmInfo& mepiNew = m_colParmList.objAdd
    (
        TMEngParmInfo(strName, c2ClassId, tCIDMacroEng::EParmDirs::InOut)
    );
    mepiNew.c2Id(tCIDLib::TCard2(m_colParmList.c4ElemCount() - 1));
    return mepiNew.c2Id();
}

tCIDLib::TCard2
TMEngMethodInfo::c2AddInOutParm(const  TString&                 strName
                                , const tCIDMacroEng::EIntrinsics  eType)
{
    return c2AddInOutParm(strName, tCIDLib::TCard2(eType));
}


tCIDLib::TCard2
TMEngMethodInfo::c2AddOutParm(  const   TString&        strName
                                , const tCIDLib::TCard2 c2ClassId)
{
    // Make sure we don't max out on parms
    if (m_colParmList.c4ElemCount() == kMacroEng::c4MaxMethodParms)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcMeth_TooManyParms
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
            , strName
            , TCardinal(kMacroEng::c4MaxMethodParms)
        );
    }

    facCIDMacroEng().CheckIdOverflow(m_colParmList.c4ElemCount(), L"method parameters");

    TMEngParmInfo& mepiNew = m_colParmList.objAdd
    (
        TMEngParmInfo(strName, c2ClassId, tCIDMacroEng::EParmDirs::Out)
    );
    mepiNew.c2Id(tCIDLib::TCard2(m_colParmList.c4ElemCount() - 1));
    return mepiNew.c2Id();
}

tCIDLib::TCard2
TMEngMethodInfo::c2AddOutParm(  const   TString&                strName
                                , const tCIDMacroEng::EIntrinsics  eType)
{
    return c2AddOutParm(strName, tCIDLib::TCard2(eType));
}


tCIDLib::TCard2 TMEngMethodInfo::c2FindParm(const TString& strName) const
{
    const tCIDLib::TCard4 c4Count = m_colParmList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colParmList[c4Index].strName() == strName)
            return tCIDLib::TCard2(c4Index);
    }
    return kMacroEng::c2BadId;
}


tCIDLib::TCard2 TMEngMethodInfo::c2RetClassId() const
{
    return m_c2RetClassId;
}

tCIDLib::TCard2 TMEngMethodInfo::c2RetClassId(const tCIDLib::TCard2 c2ToSet)
{
    m_c2RetClassId = c2ToSet;
    return m_c2RetClassId;
}


tCIDLib::TCard4 TMEngMethodInfo::c4ParmCount() const
{
    return m_colParmList.c4ElemCount();
}


tCIDMacroEng::EConstTypes TMEngMethodInfo::eConst() const
{
    return m_eConst;
}

tCIDMacroEng::EConstTypes
TMEngMethodInfo::eConst(const tCIDMacroEng::EConstTypes eToSet)
{
    m_eConst = eToSet;
    return m_eConst;
}


tCIDMacroEng::EMethExt TMEngMethodInfo::eExtend() const
{
    return m_eExtend;
}

tCIDMacroEng::EMethExt TMEngMethodInfo::eExtend(const tCIDMacroEng::EMethExt eToSet)
{
    m_eExtend = eToSet;
    return m_eExtend;
}


tCIDMacroEng::EVisTypes TMEngMethodInfo::eVisibility() const
{
    return m_eVisibility;
}


const TMEngParmInfo&
TMEngMethodInfo::mepiFind(const tCIDLib::TCard2 c2Id) const
{
    if (c2Id >= m_colParmList.c4ElemCount())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcMeth_BadParmId
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c2Id)
            , strName()
        );
    }
    return m_colParmList[c2Id];
}

TMEngParmInfo& TMEngMethodInfo::mepiFind(const tCIDLib::TCard2 c2Id)
{
    if (c2Id >= m_colParmList.c4ElemCount())
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcMeth_BadParmId
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c2Id)
            , strName()
        );
    }
    return m_colParmList[c2Id];
}



const TMEngParmInfo* TMEngMethodInfo::pmepiFind(const TString& strName) const
{
    const tCIDLib::TCard4 c4Count = m_colParmList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colParmList[c4Index].strName() == strName)
            return &m_colParmList[c4Index];
    }
    return 0;
}

TMEngParmInfo* TMEngMethodInfo::pmepiFind(const TString& strName)
{
    const tCIDLib::TCard4 c4Count = m_colParmList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colParmList[c4Index].strName() == strName)
            return &m_colParmList[c4Index];
    }
    return 0;
}


