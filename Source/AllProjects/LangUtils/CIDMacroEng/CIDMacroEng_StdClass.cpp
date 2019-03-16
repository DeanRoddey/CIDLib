//
// FILE NAME: CIDMacroEng_StdClass.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/24/2003
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
//  This file implements the derivatives of the class and value classes that
//  are used to represent the non-built in classes, i.e. the ones compiled
//  from user macros.
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
#include    "CIDMacroEng_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TMEngStdClassVal,TMEngClassVal)
RTTIDecls(TMEngStdClassInfo,TMEngClassInfo)



// ---------------------------------------------------------------------------
//  CLASS: TMEngStdClassVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngStdClassVal: Constuctors and Destructor
// ---------------------------------------------------------------------------
TMEngStdClassVal::TMEngStdClassVal( const   TString&                strName
                                    , const tCIDLib::TCard2         c2ClassId
                                    , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2ClassId, eConst)
{
}

TMEngStdClassVal::~TMEngStdClassVal()
{
}


// ---------------------------------------------------------------------------
//  TMEngStdClassVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngStdClassVal::bDbgFormat(       TTextOutStream&     strmTarget
                            , const TMEngClassInfo&
                            , const tCIDMacroEng::EDbgFmts eFormat
                            , const tCIDLib::ERadices   eRadix
                            , const TCIDMacroEngine&    meOwner) const
{
    //
    //  If they ask for the long format, we will just format the name=value
    //  pairs for all our members.
    //
    if (eFormat == tCIDMacroEng::EDbgFmts::Short)
        return kCIDLib::False;

    const tCIDLib::TCard4 c4ThisCount = c4MemberCount();
    for (tCIDLib::TCard2 c2Id = 0; c2Id < c4ThisCount; c2Id++)
    {
        const TMEngClassVal& mecvMember = mecvFind(c2Id, meOwner);
        const TMEngClassInfo& meciMember = meOwner.meciFind(mecvMember.c2ClassId());
        strmTarget << mecvMember.strName() << kCIDLib::chEquals;
        mecvMember.bDbgFormat
        (
            strmTarget
            , meciMember
            , tCIDMacroEng::EDbgFmts::Short
            , eRadix
            , meOwner
        );
        strmTarget << kCIDLib::NewLn;
    }
    return kCIDLib::True;
}


tCIDLib::TVoid
TMEngStdClassVal::CopyFrom( const   TMEngClassVal&      mecvToCopy
                            ,       TCIDMacroEngine&    meOwner)
{
    //
    //  Loop through the members at this level and recursively call the
    //  copy method on all of our members, who will do the same to theirs
    //  and so on.
    //
    //  If validating, then make sure that both instances are of the same
    //  type, that this class is copyable, and that both instances have the
    //  same number of members. We check that they are of the same class, so
    //  they better.
    //
    const tCIDLib::TCard4 c4ThisCount = c4MemberCount();
    if (meOwner.bValidation())
    {
        // Make sure that they are the same type
        meOwner.CheckSameClasses(*this, mecvToCopy);

        const TMEngClassInfo& meciToCopy = meOwner.meciFind(mecvToCopy.c2ClassId());
        if (!meciToCopy.bIsCopyable())
        {
            facCIDMacroEng().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcPrs_NotCopyable
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
                , meciToCopy.strClassPath()
            );
        }

        if (c4ThisCount != mecvToCopy.c4MemberCount())
        {
            facCIDMacroEng().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcClass_MemberMismatch
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
                , meciToCopy.strClassPath()
            );
        }
    }

    for (tCIDLib::TCard2 c2Id = 0; c2Id < c4ThisCount; c2Id++)
    {
        const TMEngClassVal& mecvSrc = mecvToCopy.mecvFind(c2Id, meOwner);
        TMEngClassVal& mecvTarget = mecvFind(c2Id, meOwner);
        mecvTarget.CopyFrom(mecvSrc, meOwner);
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngStdClassInfo
// PREFIX: eeti
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngStdClassInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngStdClassInfo::TMEngStdClassInfo(const  TString&         strName
                                    , const TString&         strBasePath
                                    ,       TCIDMacroEngine& meOwner
                                    , const TString&         strParentClassPath) :
    TMEngClassInfo
    (
        strName
        , strBasePath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::NonFinal
        , strParentClassPath
    )
{
}

TMEngStdClassInfo::~TMEngStdClassInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngStdClassInfo: Public, inherited methods
// ---------------------------------------------------------------------------
TMEngClassVal*
TMEngStdClassInfo::pmecvMakeStorage(const   TString&                strName
                                    ,       TCIDMacroEngine&        meOwner
                                    , const tCIDMacroEng::EConstTypes  eConst) const
{
    TMEngStdClassVal* pmecvNew = new TMEngStdClassVal(strName, c2Id(), eConst);
    TJanitor<TMEngStdClassVal> janNew(pmecvNew);
    LoadMemberVals(*pmecvNew, meOwner);
    return janNew.pobjOrphan();
}



// ---------------------------------------------------------------------------
//  TMEngStdClassInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngStdClassInfo::bInvokeMethod(       TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    //
    //  See if we have a method implementation at this level. If we don't,
    //  then return false and let the search down the inheritance chain
    //  continue.
    //
    TMEngMethodImpl* pmethCalled = pmethFind(methiTarget.c2Id());
    if (!pmethCalled)
        return kCIDLib::False;

    //
    //  Invoke the method now, passing along the relevant information. We
    //  pass the class instance as a parameter instead of on the stack.
    //
    pmethCalled->Invoke(mecvInstance, *this, methiTarget, meOwner);
    return kCIDLib::True;
}


