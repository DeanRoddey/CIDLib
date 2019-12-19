//
// FILE NAME: CIDCtrls_AttrEdit.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/23/2015
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
//  This file implements a standardized interface for editing the attributes of
//  whatever. It's the standard thing, with a multi-column display with the name
//  of the attribute on the left and the value to edit on the right.
//
//  The user can click on a selected item to edit in place. There is also a third
//  column with an ellipsis to bring up an interactive editor, where that is supported,
//  which is mostly for custom types, though we provide one for colors (a color palette
//  selector.) And, for boolean or enumerated string types, it can be a down area to
//  drop down a menu to select the value.
//
//  The attributes are loaded into us via a list of attr data objects. It has an id
//  that's the unique id the attribute, plus a name, type and value. There's an
//  optional limits value to provide limit checking here (a derived class can do more
//  if it wants.)
//
//  The built in types are indicated in CIDCtrls, in the TAttrData class. All
//  attributes must be one of these types, and these are the types that we support
//  here. However, there is also a 'special type' value in the attributes, which
//  is just a string that client code can use to indicate more specific type
//  semantics above and beyond the base type. They can compltely take over validation
//  and editing of the attribute based on that, or they can build on top of the
//  stuff we provide here.
//
//  Each attribute can be read only, or it can support either visual editing or
//  in-place editing, or both. Visual editing means generally some sort of popup
//  dialog. For boolean and enumerated string attributes, we provide built in visual
//  editing (via a drop down menu.) For colors we provide support for visual editing
//  via the standard color palette dialog. Otherwise it's typically only in place
//  editing that we provide here, and/or some application specific visual editing.
//  If they don't want to have to deal with formatting/parsing text formatted values,
//  they can disable in-place and only support visual editing.
//
//  We use the standard in place editor from CIDCtrls, however we don't use the
//  'auto-IPE' scheme of the parent class, which is more for ad-hoc IPE. We deal
//  with in place editing ourself, since we do far more fancy stuff.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TAttrEditWnd;


// ---------------------------------------------------------------------------
//   CLASS: TAttrEditInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------
class CIDWUTILSEXP TAttrEditInfo : public TCtrlNotify
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TAttrEditInfo() = delete;

        TAttrEditInfo
        (
            const   TAttrData&              adatNew
            , const TAttrData&              adatOld
            , const tCIDLib::TCard8         c8UserId
            , const TWindow&                wndSrc
            , const tCIDLib::TBoolean       bFromUser
        );

        TAttrEditInfo(const TAttrEditInfo&) = default;

        ~TAttrEditInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TAttrEditInfo& operator=(const TAttrEditInfo&) = default;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bFromUser() const;

        const TAttrData& adatNew() const;

        const TAttrData& adatOld() const;

        tCIDLib::TCard8 c8UserId() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_adatNew
        //      The new attribute data, which provides all of the type info and
        //      value and such that might be needed.
        //
        //  m_adatOld
        //      The previous value before editing, which they may need in order to
        //      do optimal updating, i.e. it didn't just changed from changed from
        //      this to that. Everything besides the value is redundant of course,
        //      i.e. the same as m_adatNew.
        //
        //  m_bFromUser
        //      Indicates if this came from the user actually editing the value in place
        //      or from the attribute being set from the outside.
        //
        //  m_c8UserId
        //      The user id that the client code provided. It's passed back in IPE
        //      callbacks and we send it along as well.
        // -------------------------------------------------------------------
        TAttrData           m_adatNew;
        TAttrData           m_adatOld;
        tCIDLib::TBoolean   m_bFromUser;
        tCIDLib::TCard8     m_c8UserId;
        TString             m_strOldValue;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TAttrEditInfo,TCtrlNotify)
        DefPolyDup(TAttrEditInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TAttrEditWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDWUTILSEXP TAttrEditWnd : public TMultiColListBox
{
    public :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TNotificationId        nidChange;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TAttrEditWnd();

        TAttrEditWnd(const TAttrEditWnd&) = delete;

        ~TAttrEditWnd();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TAttrEditWnd& operator=(const TAttrEditWnd&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid InitFromDesc
        (
            const   TWindow&                wndParent
            , const TDlgItem&               dlgiSrc
            , const tCIDCtrls::EWndThemes   eTheme
        )   override;

        tCIDLib::TVoid QueryHints
        (
                    tCIDLib::TStrCollect&   colToFill
        )   const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TAttrData& adatFindById
        (
            const   TString&                strIdToFind
        )   const;

        tCIDLib::TBoolean bAttrExists
        (
            const   TString&                strIdToFind
        );

        tCIDLib::TVoid ChangeAttr
        (
            const   TString&                strIdToChange
            , const TString&                strName
            , const TString&                strLimits
            , const tCIDMData::EAttrEdTypes eEdType = tCIDMData::EAttrEdTypes::Count
        );

        tCIDLib::TVoid ChangeEditType
        (
            const   TString&                strIdToChange
            , const tCIDMData::EAttrEdTypes eToSet
        );

        tCIDLib::TVoid ChangeAttrLimits
        (
            const   TString&                strIdToChange
            , const TString&                strLims
        );

        tCIDLib::TVoid ChangeAttrType
        (
            const   TString&                strIdToChange
            , const tCIDMData::EAttrTypes   eType
        );

        tCIDLib::TVoid ChangeAttrUserData
        (
            const   TString&                strIdToChange
            , const TString&                strToSet
        );

        tCIDLib::TVoid CreateAttrEd
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None
        );

        tCIDLib::TVoid LoadValues
        (
            const   tCIDMData::TAttrList&   colToLoad
            , const tCIDLib::TCard8         c8UserId
        );

        tCIDLib::TVoid SetIPEHandler
        (
                    MIPEIntf* const         pipeiToUse
        );

        tCIDLib::TVoid RefreshValues
        (
            const   tCIDMData::TAttrList&   colToLoad
        );

        tCIDLib::TVoid UpdateAttr
        (
            const   TAttrData&              adatToSet
            , const tCIDLib::TBoolean       bReport = kCIDLib::False
        );

        tCIDLib::TVoid UpdateArea
        (
            const   TString&                strId
            , const TArea&                  areaToSet
            , const tCIDLib::TBoolean       bReport = kCIDLib::False
        );

        tCIDLib::TVoid UpdateBool
        (
            const   TString&                strId
            , const tCIDLib::TBoolean       bToSet
            , const tCIDLib::TBoolean       bReport = kCIDLib::False
        );

        tCIDLib::TVoid UpdateCard
        (
            const   TString&                strId
            , const tCIDLib::TCard4         c4ToSet
            , const tCIDLib::TBoolean       bReport = kCIDLib::False
        );

        tCIDLib::TVoid UpdateCardRange
        (
            const   TString&                strId
            , const tCIDLib::TCard4         c4MinToSet
            , const tCIDLib::TCard4         c4MaxToSet
            , const tCIDLib::TBoolean       bReport = kCIDLib::False
        );

        tCIDLib::TVoid UpdateCard64
        (
            const   TString&                strId
            , const tCIDLib::TCard8         c8ToSet
            , const tCIDLib::TBoolean       bReport = kCIDLib::False
        );

        tCIDLib::TVoid UpdateChar
        (
            const   TString&                strId
            , const tCIDLib::TCh            chToSet
            , const tCIDLib::TBoolean       bReport = kCIDLib::False
        );

        tCIDLib::TVoid UpdateFloat
        (
            const   TString&                strId
            , const tCIDLib::TFloat8        f8ToSet
            , const tCIDLib::TBoolean       bReport = kCIDLib::False
        );

        tCIDLib::TVoid UpdateInt
        (
            const   TString&                strId
            , const tCIDLib::TInt4          i4ToSet
            , const tCIDLib::TBoolean       bReport = kCIDLib::False
        );

        tCIDLib::TVoid UpdateIntRange
        (
            const   TString&                strId
            , const tCIDLib::TInt4          i4MinToSet
            , const tCIDLib::TInt4          i4MaxToSet
            , const tCIDLib::TBoolean       bReport = kCIDLib::False
        );

        tCIDLib::TVoid UpdateMemBuf
        (
            const   TString&                strId
            , const tCIDLib::TCard4         c4Bytes
            , const TMemBuf&                mbufToSet
            , const tCIDLib::TBoolean       bReport = kCIDLib::False
        );

        tCIDLib::TVoid UpdatePoint
        (
            const   TString&                strId
            , const TPoint&                 pntToSet
            , const tCIDLib::TBoolean       bReport = kCIDLib::False
        );

        tCIDLib::TVoid UpdateRGBColor
        (
            const   TString&                strId
            , const TRGBClr&                clrToSet
            , const tCIDLib::TBoolean       bReport = kCIDLib::False
        );

        tCIDLib::TVoid UpdateSize
        (
            const   TString&                strId
            , const TSize&                  szToSet
            , const tCIDLib::TBoolean       bReport = kCIDLib::False
        );

        tCIDLib::TVoid UpdateString
        (
            const   TString&                strId
            , const TString&                strToSet
            , const tCIDLib::TBoolean       bReport = kCIDLib::False
        );

        tCIDLib::TVoid UpdateTime
        (
            const   TString&                strId
            , const tCIDLib::TEncodedTime   enctToSet
            , const tCIDLib::TBoolean       bReport = kCIDLib::False
        );

        // -------------------------------------------------------------------
        //  We provide a templatized method to allow for typesafe registration
        //  of event handlers.
        // -------------------------------------------------------------------
        template <typename T> TNotHandlerBase*
        pnothRegisterHandler(       T* const pTarget
                            ,       tCIDCtrls::EEvResponses (T::*pfnTarget)(TAttrEditInfo&)
                            , const tCIDLib::EEnds  eEnd = tCIDLib::EEnds::Last)
        {
            CheckHandlerRegParms(this, pTarget);
            AddNotifyRequest
            (
                new TNotifyRequestFor<T, TEFChangeInfo>(pTarget, TAttrEditWnd::nidChange)
            );

            // Add a handler object to the target
            TNotHandlerBase* pnothNew = new TNotifyHandlerFor<T, TAttrEditInfo>
            (
                pTarget, TAttrEditWnd::nidChange, pfnTarget
            );
            pTarget->AddNotifyHandler(pnothNew, eEnd);
            return pnothNew;
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bEditValue
        (
                    TAttrData&              adataEdit
            , const TArea&                  areaCell
            , const tCIDLib::TCard8         c8UserId
        );

        virtual tCIDLib::TBoolean bVisEdit
        (
                    TAttrData&              adatEdit
            , const TArea&                  areaCellScr
            , const tCIDLib::TCard8         c8UserId
        );


        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreated() override;

        tCIDLib::TBoolean bExtKey
        (
            const   tCIDCtrls::EExtKeys     eKey
            , const tCIDLib::TBoolean       bAlt
            , const tCIDLib::TBoolean       bCtrl
            , const tCIDLib::TBoolean       bShift
            , const TWindow* const          pwndChild = nullptr
        )   override;

        tCIDLib::TVoid CellClicked
        (
            const   tCIDLib::TCard4         c4Row
            , const tCIDLib::TCard4         c4Column
            , const tCIDLib::TBoolean       bLeftButton
        )   override;

        tCIDCtrls::EMCLBCustRets eCustomDraw
        (
                    TGraphDrawDev&          gdevTar
            , const tCIDLib::TCard4         c4Row
            , const tCIDLib::TCard4         c4Column
            , const tCIDLib::TBoolean       bPost
            , const TArea&                  areaAt
            ,       TRGBClr&                rgbBgn
            ,       TRGBClr&                rgbText
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        TAttrData& adatById(const TString& strId);

        tCIDLib::TBoolean bInPlaceEdit
        (
                    TAttrData&              adatEdit
            , const TArea&                  areaCell
        );

        tCIDLib::TVoid DoIncDecCXYKey
        (
                    TAttrData&              adatTar
            , const tCIDCtrls::EExtKeys     eKey
        );

        tCIDLib::TVoid DoIncDecXYKey
        (
                    TAttrData&              adatTar
            , const tCIDCtrls::EExtKeys     eKey
        );

        tCIDLib::TVoid DoPrevNextKey
        (
                    TAttrData&              adatTar
            , const tCIDLib::TBoolean       bPrevious
        );

        tCIDLib::TVoid EditValue
        (
            const   tCIDLib::TCard4         c4Item
        );

        tCIDLib::TVoid SendEditEvent
        (
            const   TAttrData&              adatNew
            , const TAttrData&              adatOld
            , const tCIDLib::TBoolean       bFromUser
        );

        tCIDLib::TVoid VisEdit
        (
            const   tCIDLib::TCard4         c4Item
        );



        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bmpVisEdit
        //      For those attrs that support visual editing, we display a bitmap in
        //      the third column. We pre-load it here and use it during custom draw
        //      of the column.
        //
        //  m_c8UserId
        //      The caller can pass an id to LoadValues(). We'll store that and
        //      pass it back to the IPE callback interface, so that they can use it
        //      to vector changed attributes to the right place.
        //
        //  m_colOrgVals
        //  m_colVals
        //      We make a copy of the list passed to LoadValues. And we make another
        //      copy as well. This way we can know which ones have changed.
        //
        //  m_pipeiToUse
        //      The IPE interface to use for interfacing us to the standard in place
        //      editor control. We pass it on to the IPE control when editing is
        //      invoked. We don't own it. Usually it's another window that has
        //      imlemented this mixin interface. The IPE uses it to interact with the
        //      client code.
        //
        //      NOTE THAT we do not implement the parent class' 'auto-IPE' scheme,
        //      so this is not set on the parent class as the IPE object. We handle
        //      our own IPE.
        //
        //  m_strTmpFmt
        //      For temporary formatting of attribute values
        // -------------------------------------------------------------------
        tCIDLib::TCard8         m_c8UserId;
        tCIDMData::TAttrList    m_colOrgVals;
        tCIDMData::TAttrList    m_colVals;
        MIPEIntf*               m_pipeiToUse;
        TString                 m_strTmpFmt;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TAttrEdWnd, TMultiColListBox)
};

#pragma CIDLIB_POPPACK


