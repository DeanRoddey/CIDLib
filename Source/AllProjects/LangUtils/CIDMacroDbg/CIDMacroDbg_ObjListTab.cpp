//
// FILE NAME: CIDMacroDbg_ObjListTab.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/23/2015
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
//  This file implements the object display tab window.
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
#include    "CIDMacroDbg_.hpp"



// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TObjListTab, TTabWindow)


// ---------------------------------------------------------------------------
//   CLASS: TObjListTab
//  PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TObjListTab: Constructors and Destructor
// ---------------------------------------------------------------------------
TObjListTab::TObjListTab(const  TString&    strName
                        , const EModes      eMode) :

    TTabWindow(strName, L"", kCIDLib::False, kCIDLib::True)
    , m_bEditOK(kCIDLib::False)
    , m_c4Col_Name(0)
    , m_c4Col_Type(1)
    , m_c4Col_ValExplore(2)
    , m_c4Col_Value(3)
    , m_eMode(eMode)
    , m_eRadix(tCIDLib::ERadices::Dec)
    , m_pmeOwner(nullptr)
    , m_pmecvSrc(nullptr)
    , m_pmethSrc(nullptr)
    , m_pmethiSrc(nullptr)
    , m_pwndObjList(nullptr)
    , m_strmFmt(1024UL)
{
    //
    //  Pre-expand the values vector with empty strings. We will use this to
    //  load up the columns values with.
    //
    m_colVals.objAdd(TString::strEmpty());
    m_colVals.objAdd(TString::strEmpty());
    m_colVals.objAdd(L"...");
    m_colVals.objAdd(TString::strEmpty());
}

TObjListTab::~TObjListTab()
{
}


// ---------------------------------------------------------------------------
//  TObjListTab: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TObjListTab::IPEValChanged( const   tCIDCtrls::TWndId   widSrc
                            , const TString&            strSrc
                            , const TAttrData&          adatNew
                            , const TAttrData&          adatOld
                            , const tCIDLib::TCard8     )
{
    // Find the class based on the stored class path.
    const TMEngClassInfo* pmeciType = m_pmeOwner->pmeciFind(adatNew.strSpecType());
    CIDAssert(pmeciType != nullptr, L"Couldn't look up saved class path on store");

    //
    //  Get it back to the intrinsic type enum, dealing with the special case of
    //  enums.
    //
    tCIDMacroEng::EIntrinsics eType = tCIDMacroEng::EIntrinsics(pmeciType->c2Id());
    if (eType >= tCIDMacroEng::EIntrinsics::Count)
    {
        if (m_pmeOwner->bIsDerivedFrom(pmeciType->c2Id(), tCIDMacroEng::EIntrinsics::Enum))
            eType = tCIDMacroEng::EIntrinsics::Enum;
    }

    CIDAssert
    (
        eType < tCIDMacroEng::EIntrinsics::Count, L"Stored type was not intrinsic"
    );

    //
    //  Get the id back out of the key value we stored it in, and look up the row
    //  index.
    //
    const tCIDLib::TCard4 c4RowId = adatNew.strId().c4Val();
    const tCIDLib::TCard4 c4RowInd = m_pwndObjList->c4IdToIndex(c4RowId);

    // Find the class value object, which is mode specific
    TMEngClassVal* pmecvVal = nullptr;
    if (m_eMode == EModes::Members)
    {
        // The row index is the id of the member
        pmecvVal = &m_pmecvSrc->mecvFind(tCIDLib::TCard2(c4RowInd), *m_pmeOwner);
    }
     else if (m_eMode == EModes::Locals)
    {
        //
        //  The row index is the id of the local. We look it up using the stored
        //  method impl pointer that we were given in the Update() method.
        //
        CIDAssert(m_pmethSrc != nullptr, L"The method impl pointer was not set");
        const tCIDLib::TCard2 c2LocalId = tCIDLib::TCard2(c4RowInd);
        const TMEngLocalInfo& meliCur = m_pmethSrc->meliFind(c2LocalId);

        tCIDLib::TCard4 c4CallInd;
        TMEngCallStackItem* pmecsiCall = m_pmeOwner->pmecsiMostRecentCall(c4CallInd);
        CIDAssert
        (
            pmecsiCall && pmecsiCall->bIsMethod()
            , L"Missing or non-method call stack Item"
        );
        pmecvVal = &m_pmeOwner->mecvStackAt(c4CallInd + c2LocalId + 1);
    }
     else if (m_eMode == EModes::Parms)
    {
        // Get the top call on the stack. If none, we are done
        tCIDLib::TCard4 c4CallInd;
        TMEngCallStackItem* pmecsiCall = m_pmeOwner->pmecsiMostRecentCall(c4CallInd);
        if (pmecsiCall)
        {
            // Get the called method info and get the parameter count
            const TMEngMethodInfo& methiTarget = pmecsiCall->methiCalled();
            const tCIDLib::TCard2 c2Count = tCIDLib::TCard2(methiTarget.c4ParmCount());

            // Make absolutely sure the row is within the count of parameters
            if (c4IPERow() < c2Count)
            {
                //
                //  The row index should be the id of the parameter. Add it to
                //  the call index on the stack, minus the parm count plus 1.
                //  This gets us to the method call. Then add the row count plus
                //  one to get to the current parameter.
                //
                pmecvVal = &m_pmeOwner->mecvStackAt
                (
                    (c4CallInd - (1 + c2Count)) + tCIDLib::TCard2(c4IPERow()) + 1
                );
            }
        }
    }
     else
    {
        CIDAssert2(L"Unknown object list mode");
    }


    switch(eType)
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
        {
            TMEngBooleanVal* pmecvBool = static_cast<TMEngBooleanVal*>(pmecvVal);
            pmecvBool->bValue(adatNew.bVal());
            break;
        }

        case tCIDMacroEng::EIntrinsics::Card1 :
        {
            TMEngCard1Val* pmecvC1 = static_cast<TMEngCard1Val*>(pmecvVal);
            pmecvC1->c1Value(tCIDLib::TCard1(adatNew.c4Val()));
            break;
        }

        case tCIDMacroEng::EIntrinsics::Card2 :
        {
            TMEngCard2Val* pmecvC2 = static_cast<TMEngCard2Val*>(pmecvVal);
            pmecvC2->c2Value(tCIDLib::TCard2(adatNew.c4Val()));
            break;
        }

        case tCIDMacroEng::EIntrinsics::Card4 :
        {
            TMEngCard4Val* pmecvC4 = static_cast<TMEngCard4Val*>(pmecvVal);
            pmecvC4->c4Value(adatNew.c4Val());
            break;
        }

        case tCIDMacroEng::EIntrinsics::Card8 :
        {
            TMEngCard8Val* pmecvC8 = static_cast<TMEngCard8Val*>(pmecvVal);
            pmecvC8->c8Value(adatNew.c8Val());
            break;
        }

        case tCIDMacroEng::EIntrinsics::Enum :
        {
            TMEngEnumVal* pmecvEnum = static_cast<TMEngEnumVal*>(pmecvVal);

            // Find the enum class for this particular type
            TMEngEnumInfo& meciEnum = static_cast<TMEngEnumInfo&>
            (
                m_pmeOwner->meciFind(pmecvEnum->c2ClassId())
            );
            pmecvEnum->c4Ordinal(meciEnum.c4FindOrdinal(adatNew.strValue()));
            break;
        }

        case tCIDMacroEng::EIntrinsics::Float4 :
        {
            TMEngFloat4Val* pmecvF4 = static_cast<TMEngFloat4Val*>(pmecvVal);
            pmecvF4->f4Value(tCIDLib::TFloat4(adatNew.f8Val()));
            break;
        }

        case tCIDMacroEng::EIntrinsics::Float8 :
        {
            TMEngFloat8Val* pmecvF8 = static_cast<TMEngFloat8Val*>(pmecvVal);
            pmecvF8->f8Value(adatNew.f8Val());
            break;
        }

        case tCIDMacroEng::EIntrinsics::Int1 :
        {
            TMEngInt1Val* pmecvI1 = static_cast<TMEngInt1Val*>(pmecvVal);
            pmecvI1->i1Value(tCIDLib::TInt1(adatNew.i4Val()));
            break;
        }

        case tCIDMacroEng::EIntrinsics::Int2 :
        {
            TMEngInt2Val* pmecvI2 = static_cast<TMEngInt2Val*>(pmecvVal);
            pmecvI2->i2Value(tCIDLib::TInt2(adatNew.i4Val()));
            break;
        }

        case tCIDMacroEng::EIntrinsics::Int4 :
        {
            TMEngInt4Val* pmecvI4 = static_cast<TMEngInt4Val*>(pmecvVal);
            pmecvI4->i4Value(adatNew.i4Val());
            break;
        }

        case tCIDMacroEng::EIntrinsics::String :
        {
            TMEngStringVal* pmecvStr = static_cast<TMEngStringVal*>(pmecvVal);
            pmecvStr->strValue(adatNew.strValue());
            break;
        }

        case tCIDMacroEng::EIntrinsics::Time :
        {
            TMEngTimeVal* pmecvTime = static_cast<TMEngTimeVal*>(pmecvVal);
            pmecvTime->SetTime(adatNew.enctVal());
            break;
        }

        default :
            break;
    };
}


// ---------------------------------------------------------------------------
//  TObjListTab: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TObjListTab::CreateObjListTab(          TTabbedWnd&             wndParent
                                , const TString&                strTabText
                                ,       TCIDMacroEngine* const  pmeToUse)
{
    // Store the macro engine we are to use
    m_pmeOwner = pmeToUse;

    // And ask our parent to create us as a tabbed window
    wndParent.c4CreateTab(this, strTabText);
}


tCIDLib::TVoid TObjListTab::Reset()
{
    // Clear the list if it exists
    if (m_pwndObjList)
        m_pwndObjList->RemoveAll();

    m_bEditOK = kCIDLib::False;
    m_pmecvSrc = nullptr;
    m_pmethSrc = nullptr;
    m_pmethiSrc = nullptr;
}


//
//  Set or clear the flag that let's us allow the user to in-place edit values.
//
tCIDLib::TVoid TObjListTab::SetEditOK(const tCIDLib::TBoolean bToSet)
{
    m_bEditOK = bToSet;
}


tCIDLib::TVoid TObjListTab::SetRadix(const tCIDLib::ERadices eToSet)
{
    // If the radix is changing, then update our values
    if (eToSet != m_eRadix)
    {
        m_eRadix = eToSet;

        if (m_eMode == EModes::Members)
            Update(m_pmecvSrc);
        else if (m_eMode == EModes::Locals)
            Update(m_pmethSrc);
        else
            Update();
    }
}


//
//  We are a members display winodw and are being given a class object to display
//  or update the members for.
//
tCIDLib::TVoid TObjListTab::Update(TMEngClassVal* const pmecvInstance)
{
    CIDAssert(m_eMode == EModes::Members, L"This object list is not set for members");
    CIDAssert(m_pmeOwner != nullptr, L"The object list has no macro engine");

    //
    //  Sometimes we can be called with a null, such as at startup of a new
    //  debugging session, but before we get to the point where the class value
    //  object is found. If so, just do nothing. We should already be cleared
    //  out at that point.
    //
    if (!pmecvInstance)
    {
        m_pmecvSrc = nullptr;
        m_pwndObjList->RemoveAll();
        return;
    }

    TWndPaintJanitor janDraw(m_pwndObjList);
    tCIDLib::TBoolean bAvail;
    if (m_pmecvSrc == pmecvInstance)
    {
        //
        //  Refresh all of the items. We just have to find them by their ids
        //  and update their values.
        //
        const tCIDLib::TCard4 c4Count = pmecvInstance->c4MemberCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            const TMEngClassVal& mecvMember = m_pmecvSrc->mecvFind
            (
                tCIDLib::TCard2(c4Index), *m_pmeOwner
            );

            m_strmFmt.Reset();
            bAvail = mecvMember.bDbgFormat
            (
                m_strmFmt
                , m_pmeOwner->meciFind(mecvMember.c2ClassId())
                , tCIDMacroEng::EDbgFmts::Short
                , m_eRadix
                , *m_pmeOwner
            );

            // Update the value column
            if (bAvail)
            {
                m_strmFmt.Flush();
                m_colVals[2] = m_strmFmt.strData();
            }
             else
            {
                m_colVals[2].Clear();
            }
            m_pwndObjList->SetColText(c4Index, 2, m_colVals[2]);
        }
    }
     else
    {
        // Either no current content, or we have a new object, so full update
        m_pmecvSrc = pmecvInstance;

        //
        //  The members have ids from 0 to count-1, so we can just use the index
        //  to find each member.
        //
        const tCIDLib::TCard4 c4Count = pmecvInstance->c4MemberCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            const TMEngClassVal& mecvMember = m_pmecvSrc->mecvFind
            (
                tCIDLib::TCard2(c4Index), *m_pmeOwner
            );

            // Get our values out
            m_colVals[m_c4Col_Name] = mecvMember.strName();
            m_colVals[m_c4Col_Type] = m_pmeOwner->meciFind(mecvMember.c2ClassId()).strClassPath();

            m_strmFmt.Reset();
            bAvail = mecvMember.bDbgFormat
            (
                m_strmFmt
                , m_pmeOwner->meciFind(mecvMember.c2ClassId())
                , tCIDMacroEng::EDbgFmts::Short
                , m_eRadix
                , *m_pmeOwner
            );

            if (bAvail)
            {
                m_strmFmt.Flush();
                m_colVals[m_c4Col_Value] = m_strmFmt.strData();
            }
             else
            {
                m_colVals[m_c4Col_Value].Clear();
            }

            // Add it, setting it's list id to the index for later correlateion
            m_pwndObjList->c4AddItem(m_colVals, c4Index);
        }
    }
}


//
//  we are a method locals display window and are being given a method to
//  display the locals of.
//
tCIDLib::TVoid TObjListTab::Update(const TMEngMethodImpl* const pmethSrc)
{
    CIDAssert(m_eMode == EModes::Locals, L"This object list is not set for members");
    CIDAssert(m_pmeOwner != nullptr, L"The object list has no macro engine");

    TWndPaintJanitor janDraw(m_pwndObjList);

    // Clear out any current items
    m_pwndObjList->RemoveAll();

    // If this method has no locals, we are done
    const tCIDLib::TCard2 c2Count = tCIDLib::TCard2(pmethSrc->c4LocalCount());
    if (!c2Count)
        return;

    //
    //  Get the most recent call on the stack. It should always be non-null,
    //  and it cleraly should be a method since we asked for one, but just in
    //  case, check it.
    //
    tCIDLib::TCard4 c4CallInd;
    TMEngCallStackItem* pmecsiCall = m_pmeOwner->pmecsiMostRecentCall(c4CallInd);
    if (!pmecsiCall || !pmecsiCall->bIsMethod())
        return;

    //
    //  Get the method info and impl objects. We only need them here in order
    //  to looad them, but we keep them around in case of in-place editing.
    //
    m_pmethSrc = pmethSrc;
    m_pmethiSrc = &pmecsiCall->methiCalled();

    tCIDLib::TBoolean bAvail;
    for (tCIDLib::TCard2 c2Id = 0; c2Id < c2Count; c2Id++)
    {
        const TMEngLocalInfo& meliCur = m_pmethSrc->meliFind(c2Id);
        TMEngClassVal& mecvCur = m_pmeOwner->mecvStackAt(c4CallInd + c2Id + 1);

        // Get our values out
        m_colVals[m_c4Col_Name] = mecvCur.strName();
        m_colVals[m_c4Col_Type] = m_pmeOwner->meciFind(mecvCur.c2ClassId()).strClassPath();

        m_strmFmt.Reset();
        bAvail = mecvCur.bDbgFormat
        (
            m_strmFmt
            , m_pmeOwner->meciFind(mecvCur.c2ClassId())
            , tCIDMacroEng::EDbgFmts::Short
            , m_eRadix
            , *m_pmeOwner
        );

        if (bAvail)
        {
            m_strmFmt.Flush();
            m_colVals[m_c4Col_Value] = m_strmFmt.strData();
        }
         else
        {
            m_colVals[m_c4Col_Value].Clear();
        }

        //
        //  Add it, setting it's list id to the
        m_pwndObjList->c4AddItem(m_colVals, c2Id);
    }
}


tCIDLib::TVoid TObjListTab::Update()
{
    CIDAssert(m_eMode == EModes::Parms, L"This object list is not set for params");
    CIDAssert(m_pmeOwner != nullptr, L"The object list has no macro engine");

    TWndPaintJanitor janDraw(m_pwndObjList);

    // Clear out any current items
    m_pwndObjList->RemoveAll();

    //
    //  Search backwards down the stack until we come to the first call
    //  item. If we find one, then update our list. Else, just fall out with
    //  the list empty.
    //
    tCIDLib::TCard4 c4CallInd;
    TMEngCallStackItem* pmecsiCall = m_pmeOwner->pmecsiMostRecentCall(c4CallInd);
    if (pmecsiCall)
    {
        // Get the called method info and get the parameter count.
        const TMEngMethodInfo& methiTarget = pmecsiCall->methiCalled();
        const tCIDLib::TCard2 c2Count = tCIDLib::TCard2(methiTarget.c4ParmCount());

        tCIDLib::TBoolean bAvail;
        const tCIDLib::TCard4 c4Start = c4CallInd - (1 + c2Count);
        for (tCIDLib::TCard2 c2Id = 0; c2Id < c2Count; c2Id++)
        {
            //
            //  Get the current value at the indicated place on the call stack,
            //  and get the parameter info from the method item.
            //
            //  NOTE:   The reason we need to have the parm info is that the
            //          values pushed on the stack for parameters don't have
            //          meaningful names, since they don't normally need them.
            //          So we have to get the correct names from the parm
            //          info objects.
            //
            TMEngClassVal& mecvCur = m_pmeOwner->mecvStackAt(c4Start + c2Id + 1);
            const TMEngParmInfo& mepiCur = methiTarget.mepiFind(c2Id);

            // Get our values out
            m_colVals[m_c4Col_Name] = mepiCur.strName();
            m_colVals[m_c4Col_Type] = m_pmeOwner->meciFind(mecvCur.c2ClassId()).strClassPath();

            m_strmFmt.Reset();
            bAvail = mecvCur.bDbgFormat
            (
                m_strmFmt
                , m_pmeOwner->meciFind(mecvCur.c2ClassId())
                , tCIDMacroEng::EDbgFmts::Short
                , m_eRadix
                , *m_pmeOwner
            );

            if (bAvail)
            {
                m_strmFmt.Flush();
                m_colVals[m_c4Col_Value] = m_strmFmt.strData();
            }
             else
            {
                m_colVals[m_c4Col_Value].Clear();
            }

            // Add it, setting it's list id to the index
            m_pwndObjList->c4AddItem(m_colVals, c2Id);
        }
    }

}


// ---------------------------------------------------------------------------
//  TObjListTab: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TObjListTab::AreaChanged(const  TArea&                  areaPrev
                        , const TArea&                  areaNew
                        , const tCIDCtrls::EPosStates   ePosState
                        , const tCIDLib::TBoolean       bOrgChanged
                        , const tCIDLib::TBoolean       bSizeChanged
                        , const tCIDLib::TBoolean       bStateChanged)
{
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    // Just keep the list window sized to fit our client area
    if (m_pwndObjList && bSizeChanged)
        m_pwndObjList->SetSize(areaClient().szArea(), kCIDLib::True);
}


tCIDLib::TBoolean TObjListTab::bCreated()
{
    TParent::bCreated();

    TArea areaClient;
    QueryClientArea(areaClient, kCIDLib::False);

    tCIDLib::TStrList colTitles(4);
    colTitles.objAdd(facCIDMacroDbg().strMsg(kMDbgMsgs::midCol_Name));
    colTitles.objAdd(facCIDMacroDbg().strMsg(kMDbgMsgs::midCol_Type));
    colTitles.objAdd(L"...");
    colTitles.objAdd(facCIDMacroDbg().strMsg(kMDbgMsgs::midCol_Value));

    // Create our mc list box
    m_pwndObjList = new TMultiColListBox();
    m_pwndObjList->CreateMCLB
    (
        *this
        , kCIDCtrls::widFirstCtrl
        , areaClient
        , colTitles
        , 0
        , tCIDCtrls::EWndStyles::VisTabChild
        , tCIDLib::eOREnumBits
          (
            tCIDCtrls::EMCLBStyles::FullRowSel, tCIDCtrls::EMCLBStyles::AutoLastCol
          )
    );

    // Size the ... column to fit the column header
    m_pwndObjList->AutoSizeCol(m_c4Col_ValExplore, kCIDLib::True);

    // Set up an event handler for it
    m_pwndObjList->pnothRegisterHandler(this, &TObjListTab::eLBHandler);

    return kCIDLib::True;
}


//
//  If the user clicks on the value explore column of the already selected row, this is
//  called to give us a chance to show the user more info if it's not just a simple
//  value type.
//
tCIDLib::TVoid
TObjListTab::ExploreValue(const tCIDLib::TCard4 c4RowInd, const TMEngClassVal& mecvVal)
{
    //
    //  Else we need to figure out the type and see if it's a type we handle.
    //  Regardless of mode, we can get the class type from column 1 since it was
    //  stored there.
    //
    TString strClassPath;
    m_pwndObjList->QueryColText(c4RowInd, m_c4Col_Type, strClassPath);

    // If by some strange bug we can't find the type, don't blow up
    const TMEngClassInfo* pmeciType = m_pmeOwner->pmeciFind(strClassPath);
    if (!pmeciType)
        return;

    // Ask the object for format itself out in long form
    TTextStringOutStream strmOut(2048UL);
    const tCIDLib::TBoolean bRes = mecvVal.bDbgFormat
    (
        strmOut, *pmeciType, tCIDMacroEng::EDbgFmts::Long, m_eRadix, *m_pmeOwner
    );

    if (bRes)
    {
        strmOut.Flush();
        facCIDWUtils().ShowText(*this, L"Object Information", strmOut.strData());
    }
     else
    {
        TErrBox msgbErr(L"Information could not be obtained for this object");
        msgbErr.ShowIt(*this);
    }
}


//
//  If the user clicks on the value column of the already selected row, this is
//  called to give us a chance to invoke the inline editor.
//
tCIDLib::TVoid
TObjListTab::InlineEdit(const tCIDLib::TCard4 c4RowInd, const TMEngClassVal& mecvVal)
{
    //
    //  Else we need to figure out the type and see if it's a type we can
    //  handle.  Regardless of mode, we can get the class type from column 1
    //  since it was stored there.
    //
    TString strClassPath;
    m_pwndObjList->QueryColText(c4RowInd, m_c4Col_Type, strClassPath);

    // If by some strange bug we can't find the type, don't blow up
    const TMEngClassInfo* pmeciType = m_pmeOwner->pmeciFind(strClassPath);
    if (!pmeciType)
        return;

    // See if it's an intrinsic type and one we can handle if so
    tCIDMacroEng::EIntrinsics eType = tCIDMacroEng::EIntrinsics::Count;
    if (pmeciType->c2Id() < tCIDLib::TCard2(eType))
        eType = tCIDMacroEng::EIntrinsics(pmeciType->c2Id());

    //
    //  If not an intrinsic, check the special case of an enumerated derivative.
    //  If so, treat it as the base enum class.
    //
    if (eType == tCIDMacroEng::EIntrinsics::Count)
    {
        if (m_pmeOwner->bIsDerivedFrom(pmeciType->c2Id(), tCIDMacroEng::EIntrinsics::Enum))
            eType = tCIDMacroEng::EIntrinsics::Enum;
    }

    // If still non-intrinsic, we we can't edit it
    if (eType == tCIDMacroEng::EIntrinsics::Count)
        return;

    // As we are going, we set up the in place editor type and limits as well
    tCIDMData::EAttrTypes eIPEType = tCIDMData::EAttrTypes::Count;
    TString strLimits;
    switch(eType)
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
            eIPEType = tCIDMData::EAttrTypes::Bool;
            break;

        case tCIDMacroEng::EIntrinsics::Card1 :
            eIPEType = tCIDMData::EAttrTypes::Card;
            strLimits = L"Range:0, 0xFF";
            break;

        case tCIDMacroEng::EIntrinsics::Card2 :
            eIPEType = tCIDMData::EAttrTypes::Card;
            strLimits = L"Range:0, 0xFFFF";
            break;

        case tCIDMacroEng::EIntrinsics::Card4 :
            eIPEType = tCIDMData::EAttrTypes::Card;
            break;

        case tCIDMacroEng::EIntrinsics::Card8 :
            eIPEType = tCIDMData::EAttrTypes::Card64;
            break;

        case tCIDMacroEng::EIntrinsics::Enum :
            // We'll figure out the limits below
            eIPEType = tCIDMData::EAttrTypes::String;
            break;

        // Can't handle these yet
        case tCIDMacroEng::EIntrinsics::Float4 :
            eType = tCIDMacroEng::EIntrinsics::Count;
            break;

        case tCIDMacroEng::EIntrinsics::Float8 :
            eType = tCIDMacroEng::EIntrinsics::Count;
            break;

        case tCIDMacroEng::EIntrinsics::Int1 :
            eIPEType = tCIDMData::EAttrTypes::Int;
            strLimits = L"Range:-128, 127";
            break;

        case tCIDMacroEng::EIntrinsics::Int2 :
            eIPEType = tCIDMData::EAttrTypes::Int;
            strLimits = L"Range:-32768, 32767";
            break;

        case tCIDMacroEng::EIntrinsics::Int4 :
            eIPEType = tCIDMData::EAttrTypes::Int;
            break;

        case tCIDMacroEng::EIntrinsics::Time :
            eIPEType = tCIDMData::EAttrTypes::Time;
            break;

        case tCIDMacroEng::EIntrinsics::String :
        default :
            eIPEType = tCIDMData::EAttrTypes::String;
            break;
    };

    // If not a type we can handle, then give up
    if (eType == tCIDMacroEng::EIntrinsics::Count)
        return;

    //
    //  Looks ok. We set the attribute value's object to our formatted row id
    //  so that we know how to get back to this one when a value comes back.
    //
    TString strId;
    strId.AppendFormatted(m_pwndObjList->c4IndexToId(c4RowInd));

    //
    //  If it's an enumerated type, that's a special case for figuring out the
    //  limits so deal with that.
    //
    if (eType == tCIDMacroEng::EIntrinsics::Enum)
    {
        const TMEngEnumInfo* pmeciEnum = static_cast<const TMEngEnumInfo*>(pmeciType);
        strLimits = L"Enum: ";

        const tCIDLib::TCard4 c4EnumCnt = pmeciEnum->c4ValueCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4EnumCnt; c4Index++)
        {
            if (c4Index)
                strLimits.Append(kCIDLib::chComma);
            strLimits.Append(pmeciEnum->strItemName(c4Index));
        }
    }

    // Get the name of this one
    TString strName;
    m_pwndObjList->QueryColText(c4RowInd, m_c4Col_Name, strName);

    // And set up the attribute value object
    TAttrData adatEdit;
    adatEdit.Set(strName, strId, strLimits, eIPEType);
    switch(eType)
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
            adatEdit.SetBool(static_cast<const TMEngBooleanVal&>(mecvVal).bValue());
            break;

        case tCIDMacroEng::EIntrinsics::Card1 :
            adatEdit.SetCard
            (
                tCIDLib::TCard4(static_cast<const TMEngCard1Val&>(mecvVal).c1Value())
            );
            break;

        case tCIDMacroEng::EIntrinsics::Card2 :
            adatEdit.SetCard
            (
                tCIDLib::TCard4(static_cast<const TMEngCard2Val&>(mecvVal).c2Value())
            );
            break;

        case tCIDMacroEng::EIntrinsics::Card4 :
            adatEdit.SetCard(static_cast<const TMEngCard4Val&>(mecvVal).c4Value());
            break;

        case tCIDMacroEng::EIntrinsics::Card8 :
            adatEdit.SetCard64(static_cast<const TMEngCard8Val&>(mecvVal).c8Value());
            break;

        case tCIDMacroEng::EIntrinsics::Enum :
        {
            const TMEngEnumInfo& meciEnum = static_cast<const TMEngEnumInfo&>
            (
                m_pmeOwner->meciFind(mecvVal.c2ClassId())
            );
            adatEdit.SetString
            (
                meciEnum.strItemName(static_cast<const TMEngEnumVal&>(mecvVal))
            );
            break;
        }

        case tCIDMacroEng::EIntrinsics::Float4 :
        {
            const TMEngFloat4Info& meciEnum = static_cast<const TMEngFloat4Info&>
            (
                m_pmeOwner->meciFind(mecvVal.c2ClassId())
            );
            adatEdit.SetFloat((static_cast<const TMEngFloat4Val&>(mecvVal)).f4Value());
            break;
        }

        case tCIDMacroEng::EIntrinsics::Float8 :
        {
            const TMEngFloat8Info& meciEnum = static_cast<const TMEngFloat8Info&>
            (
                m_pmeOwner->meciFind(mecvVal.c2ClassId())
            );
            adatEdit.SetFloat((static_cast<const TMEngFloat8Val&>(mecvVal)).f8Value());
            break;
        }

        case tCIDMacroEng::EIntrinsics::Int1 :
            adatEdit.SetInt
            (
                tCIDLib::TInt4(static_cast<const TMEngInt1Val&>(mecvVal).i1Value())
            );
            break;

        case tCIDMacroEng::EIntrinsics::Int2 :
            adatEdit.SetInt
            (
                tCIDLib::TInt4(static_cast<const TMEngInt2Val&>(mecvVal).i2Value())
            );
            break;

        case tCIDMacroEng::EIntrinsics::Int4 :
            adatEdit.SetInt(static_cast<const TMEngInt4Val&>(mecvVal).i4Value());
            break;

        case tCIDMacroEng::EIntrinsics::String :
            adatEdit.SetString(static_cast<const TMEngStringVal&>(mecvVal).strValue());
            break;

        case tCIDMacroEng::EIntrinsics::Time :
            adatEdit.SetTime(static_cast<const TMEngTimeVal&>(mecvVal).enctValue());
            break;

        default :
            // Have to use generic formatting
            adatEdit.SetString(m_strmFmt.strData());
            break;
    };

    //
    //  Set the special type to the class path we got out above. In the store
    //  call above we can look that back up.
    //
    adatEdit.SetSpecType(strClassPath);

    // Get the area of the cell we care about
    TArea areaCell;
    m_pwndObjList->QueryColArea(c4RowInd, m_c4Col_Value, areaCell);

    //
    //  Invoke the in place editor. Set the list index on the IPE interface. Don't care
    //  about the return. If they committed and it validated, then we will have stored it
    //  in the IPEValChanged() callback. If taken, we update the cell.
    //
    c4IPERow(c4RowInd);
    TInPlaceEdit ipeVal;
    if (ipeVal.bProcess(*m_pwndObjList, areaCell, adatEdit, *this, 0))
    {
        TString strNewVal;
        adatEdit.FormatToText(strNewVal);
        m_pwndObjList->SetColText(c4RowInd, m_c4Col_Value, strNewVal);
    }
}


tCIDCtrls::EEvResponses TObjListTab::eLBHandler(TListChangeInfo& wnotEvent)
{
    if (wnotEvent.eEvent() == tCIDCtrls::EListEvents::ColClicked)
    {
        // If not edit ok mode, then ignore this
        if (!m_bEditOK)
            return tCIDCtrls::EEvResponses::Handled;

        // In either case here they have to be clicking on the already selected row
        const tCIDLib::TCard4 c4ListInd = wnotEvent.c4Index();
        if (c4ListInd != m_pwndObjList->c4CurItem())
            return tCIDCtrls::EEvResponses::Handled;

        // Make sure it's one of the columns we care about
        const tCIDLib::TCard4 c4ColInd  = wnotEvent.c4ColIndex();
        if ((c4ColInd != m_c4Col_ValExplore)
        &&  (c4ColInd != m_c4Col_Value))
        {
            return tCIDCtrls::EEvResponses::Handled;
        }

        // Get the value. Finding the value object is mode dependent.
        const TMEngClassVal* pmecvVal = nullptr;
        if (m_eMode == EModes::Members)
        {
            // The row index is the id of the member
            CIDAssert(m_pmecvSrc != nullptr, L"The class value pointer was not set");
            pmecvVal = &m_pmecvSrc->mecvFind
            (
                tCIDLib::TCard2(c4ListInd), *m_pmeOwner
            );
            CIDAssert(pmecvVal != nullptr, L"The class member was not found");
        }
         else if (m_eMode == EModes::Locals)
        {
            //
            //  The row index is the id of the local. We look it up using the stored
            //  method impl pointer that we were given in the Update() method.
            //
            CIDAssert(m_pmethSrc != nullptr, L"The method impl pointer was not set");
            const tCIDLib::TCard2 c2LocalId = tCIDLib::TCard2(c4ListInd);
            const TMEngLocalInfo& meliCur = m_pmethSrc->meliFind(c2LocalId);

            tCIDLib::TCard4 c4CallInd;
            TMEngCallStackItem* pmecsiCall = m_pmeOwner->pmecsiMostRecentCall(c4CallInd);

            CIDAssert
            (
                pmecsiCall && pmecsiCall->bIsMethod()
                , L"Missing or non-method call stack Item"
            );
            pmecvVal = &m_pmeOwner->mecvStackAt(c4CallInd + c2LocalId + 1);
        }
         else if (m_eMode == EModes::Parms)
        {
            // Get the top call on the stack. If none, we are done
            tCIDLib::TCard4 c4CallInd;
            TMEngCallStackItem* pmecsiCall = m_pmeOwner->pmecsiMostRecentCall(c4CallInd);
            if (pmecsiCall)
            {
                // Get the called method info and get the parameter count
                const TMEngMethodInfo& methiTarget = pmecsiCall->methiCalled();
                const tCIDLib::TCard2 c2Count = tCIDLib::TCard2(methiTarget.c4ParmCount());

                // Make absolutely sure the row is within the count of parameters
                if (c4ListInd < c2Count)
                {
                    //
                    //  The row index should be the id of the parameter. Add it to
                    //  the call index on the stack, minus the parm count plus 1.
                    //  This gets us to the method call. Then add the row count plus
                    //  one to get to the current parameter.
                    //
                    pmecvVal = &m_pmeOwner->mecvStackAt
                    (
                        (c4CallInd - (1 + c2Count)) + tCIDLib::TCard2(c4ListInd) + 1
                    );
                }
            }
        }
         else
        {
            CIDAssert2(L"Unknown object list mode");
        }

        // Shouldn't happen, but just in case
        if (!pmecvVal)
            return tCIDCtrls::EEvResponses::Handled;

        //
        //  We either want to allow it to be in place edited, if the value column and
        //  it's a type that can handle that. Or to allow for a visual value explorer
        //  type dialog, if it's the ... column. So call a helper for each.
        //
        if (c4ColInd == m_c4Col_ValExplore)
        {
            // Call the value selection/explorer
            ExploreValue(c4ListInd, *pmecvVal);
        }
         else if (c4ColInd == m_c4Col_Value)
        {
            // Call the in place editor helper
            InlineEdit(c4ListInd, *pmecvVal);
        }
    }
    return tCIDCtrls::EEvResponses::Handled;
}
