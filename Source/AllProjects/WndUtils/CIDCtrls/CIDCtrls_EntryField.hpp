//
// FILE NAME: CIDCtrls_EntryField.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/27/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is a standard single line etry field control.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//   CLASS: TEFChangeInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TEFChangeInfo : public TCtrlNotify
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TEFChangeInfo
        (
            const   tCIDCtrls::EEFEvents    eEvent
            , const TWindow&                wndSrc

        );

        TEFChangeInfo
        (
            const   TEFChangeInfo&         wnotToCopy
        );

        ~TEFChangeInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TEFChangeInfo& operator=
        (
            const   TEFChangeInfo&         wnotToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDCtrls::EEFEvents eEvent() const;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TEFChangeInfo();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eEvent
        //      Indicates the particular type of event.
        // -------------------------------------------------------------------
        tCIDCtrls::EEFEvents    m_eEvent;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TEFChangeInfo,TCtrlNotify)
        DefPolyDup(TEFChangeInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TEntryField
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TEntryField : public TStdCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TNotificationId        nidChange;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TEntryField();

        ~TEntryField();


        // For internally use only
        TEntryField
        (
            const   tCIDCtrls::TWndHandle   hwndToUse
        );


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid InitFromDesc
        (
            const   TWindow&                wndParent
            , const TDlgItem&               dlgiSrc
            , const tCIDCtrls::EWndThemes   eTheme
        )  override;

        tCIDLib::TVoid QueryHints
        (
                    tCIDLib::TStrCollect&   colToFill
        )   const override;

        TSize szDefault() const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsEmpty() const;

        tCIDLib::TBoolean bGetSelectionIndices
        (
                    tCIDLib::TCard4&        c4StartInd
            ,       tCIDLib::TCard4&        c4EndInd
        );

        tCIDLib::TBoolean bGetSelectedText
        (
                    TString&                strToFill
        )   const;

        tCIDLib::TBoolean bReadOnly() const;

        tCIDLib::TBoolean bReadOnly
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TBoolean bSelectionToClipboard() const;

        tCIDLib::TVoid Create
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const TString&                strText
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::EEFStyles    eEFStyles = tCIDCtrls::EEFStyles::None
            , const tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None
        );

        tCIDCtrls::EEFStyles eEFStyles() const;

        tCIDLib::TVoid InsertText
        (
            const   TString&                strToInsert
            , const tCIDLib::TBoolean       bSelect = kCIDLib::False
        );

        tCIDLib::TVoid RemoveSelection();

        tCIDLib::TVoid SelectAll();

        tCIDLib::TVoid SetSelection
        (
            const   tCIDLib::TCard4         c4StartInd
            , const tCIDLib::TCard4         c4EndInd
        );

        tCIDLib::TVoid SetCueText
        (
            const   TString&                strToSet
        );


        // -------------------------------------------------------------------
        //  We provide a templatized method to allow for typesafe registration
        //  of event handlers.
        // -------------------------------------------------------------------
        template <class T> TNotHandlerBase*
        pnothRegisterHandler(       T* const pTarget
                            ,       tCIDCtrls::EEvResponses (T::*pfnTarget)(TEFChangeInfo&)
                            , const tCIDLib::EEnds  eEnd = tCIDLib::EEnds::Last)
        {
            CheckHandlerRegParms(this, pTarget);
            AddNotifyRequest
            (
                new TNotifyRequestFor<T, TEFChangeInfo>(pTarget, TEntryField::nidChange)
            );

            // Add a handler object to the target
            TNotHandlerBase* pnothNew = new TNotifyHandlerFor<T, TEFChangeInfo>
            (
                pTarget, TEntryField::nidChange, pfnTarget
            );
            pTarget->AddNotifyHandler(pnothNew, eEnd);
            return pnothNew;
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bNotReflect
        (
                    TWindow&                wndTar
            , const tCIDCtrls::TWndMsg      wmsgMsg
            , const tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      lParam
            ,       tCIDCtrls::TMsgResult&  mresRet
        )   override;

        tCIDCtrls::ECtxMenuOpts eShowContextMenu
        (
            const   TPoint&                 pntAt
            , const tCIDCtrls::TWndId       widSrc
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TEntryField(const TEntryField&);
        tCIDLib::TVoid operator=(const TEntryField&);


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eEFStyles
        //      The styles set on us, in our form.
        // -------------------------------------------------------------------
        tCIDCtrls::EEFStyles    m_eEFStyles;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TEntryField, TStdCtrlWnd)
};

#pragma CIDLIB_POPPACK


