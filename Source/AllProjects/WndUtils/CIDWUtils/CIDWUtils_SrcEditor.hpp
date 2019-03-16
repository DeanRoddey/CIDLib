//
// FILE NAME: CIDWUtils_SrcEditor.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/16/2015
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
//  This is a wrapper around the Scintilla editor engine. We just create one as
//  a child window of us, slightly intended for some margin.
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
//   CLASS: TSrcEdChangeInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------
class CIDWUTILSEXP TSrcEdChangeInfo : public TCtrlNotify
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSrcEdChangeInfo() = delete;

        TSrcEdChangeInfo
        (
            const   tCIDWUtils::ESEdEvents  eEvent
            , const TString&                strPath
            , const tCIDLib::TCard4         c4LineNum
            , const TWindow&                wndSrc
        );

        TSrcEdChangeInfo
        (
            const   TString&                strPath
            , const tCIDLib::TInt4          i4LineChanges
            , const TWindow&                wndSrc
        );

        TSrcEdChangeInfo(const TSrcEdChangeInfo&) = default;

        ~TSrcEdChangeInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSrcEdChangeInfo& operator=(const TSrcEdChangeInfo& wnotToAssign) = default;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4LineNum() const;

        tCIDWUtils::ESEdEvents eEvent() const;

        tCIDLib::TInt4 i4LineChanges() const;

        const TString& strPath() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4LineNum
        //      The line at which the event occurred. May be c4MaxCard if it
        //      applies to all break points. Not used for Modified.
        //
        //  m_eEvent
        //      Indicates the particular type of event.
        //
        //  m_i4LineChanges
        //      Only used by the _Modifed event. It indicates how many lines were
        //      added or removed (negative means removed.)
        //
        //  m_strPath
        //      The path that the event occurred in. This can be used by the
        //      receiver to correlate this change with any storage info he has.
        //      He has to give us a path when he loads text into us, or when
        //      setting us up.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4LineNum;
        tCIDWUtils::ESEdEvents  m_eEvent;
        tCIDLib::TInt4          m_i4LineChanges;
        TString                 m_strPath;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSrcEdChangeInfo,TCtrlNotify)
        DefPolyDup(TSrcEdChangeInfo)
};



// ---------------------------------------------------------------------------
//   CLASS: TSrcEditor
//  PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDWUTILSEXP TSrcEditor : public TStdCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TNotificationId        nidChange;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSrcEditor();

        TSrcEditor(const TSrcEditor&) = delete;

        ~TSrcEditor();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSrcEditor& operator=(const TSrcEditor&) = delete;


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
        tCIDLib::TVoid AddRemoveBP
        (
            const   tCIDLib::TCard4         c4Line
            , const tCIDLib::TBoolean       bState
            , const tCIDLib::TBoolean       bSendEvent
        );

        tCIDLib::TBoolean bCanRedo() const;

        tCIDLib::TBoolean bCanUndo() const;

        tCIDLib::TBoolean bChanges() const;

        tCIDLib::TBoolean bFind
        (
            const   TString&                strToFind
            , const tCIDLib::TBoolean       bNext
        );

        tCIDLib::TBoolean bHasDisabledBP
        (
            const   tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TBoolean bHasEnabledBP
        (
            const   tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TBoolean bHasSelection() const;

        tCIDLib::TBoolean bIsValidPos
        (
            const   tCIDLib::TCard4         c4Line
            , const tCIDLib::TCard4         c4Col
        )   const;

        tCIDLib::TBoolean bQuerySelectedText
        (
                    TString&                strToFill
        );

        tCIDLib::TBoolean bReadOnly() const;

        tCIDLib::TBoolean bReadOnly
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TCard4 c4CurLine() const;

        tCIDLib::TCard4 c4Replace
        (
            const   TString&                strToFind
            , const TString&                strReplaceWith
        );

        tCIDLib::TVoid ChangesSaved();

        tCIDLib::TVoid ClearSelection();

        tCIDLib::TVoid CopySelection
        (
            const   tCIDLib::TBoolean       bCut
        );

        tCIDLib::TVoid CreateSrcEditor
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widToUse
            , const TArea&                  areaInit
            , const tCIDCtrls::EWndStyles   eWndStyles
            , const TString&                strPath
        );

        tCIDLib::TVoid GoToLine
        (
            const   tCIDLib::TCard4         c4LineNum
        );

        tCIDLib::TVoid LoadText
        (
            const   TString&                strPath
            , const TString&                strToSet
        );

        tCIDLib::TVoid LoadText
        (
            const   TString&                strPath
            ,       TTextInStream&          strmSrc
        );

        tCIDLib::TVoid Paste();

        tCIDLib::TVoid QueryBPLines
        (
                    tCIDLib::TCardList&     fcolLines
            ,       tCIDLib::TBoolList&     fcolEnabled
        )   const;

        tCIDLib::TVoid QuerySrcText
        (
                    TString&                strToFill
            , const tCIDLib::TBoolean       bAppend
        )   const;

        tCIDLib::TVoid Redo();

        tCIDLib::TVoid RemoveAllBPs();

        tCIDLib::TVoid RemoveCurLineState();

        tCIDLib::TVoid RemoveLineState
        (
            const   tCIDLib::TCard4         c4LineIndex
            , const tCIDWUtils::ECEdLStates eToRemove
        );

        tCIDLib::TVoid SelectAll();

        tCIDLib::TVoid SelectCurLine();

        tCIDLib::TVoid SetBPEnable
        (
            const   tCIDLib::TCard4         c4Line
            , const tCIDLib::TBoolean       bState
            , const tCIDLib::TBoolean       bSendEvent
        );

        tCIDLib::TVoid SetPosition
        (
            const   tCIDLib::TCard4         c4Line
            , const tCIDLib::TCard4         c4Col
            , const tCIDLib::TBoolean       bSetCurLine
            , const tCIDLib::TBoolean       bKeepCentered
        );

        tCIDLib::TVoid SetSearchFlag
        (
            const   tCIDWUtils::EFindOpts   eOpt
            , const tCIDLib::TBoolean       bState
        );

        tCIDLib::TVoid ToggleBPoint();

        tCIDLib::TVoid ToggleBPEnable();

        tCIDLib::TVoid Undo();


        // -------------------------------------------------------------------
        //  We provide a templatized method to allow for typesafe registration
        //  of event handlers.
        // -------------------------------------------------------------------
        template <class T> TNotHandlerBase*
        pnothRegisterHandler(       T* const pTarget
                            ,       tCIDCtrls::EEvResponses (T::*pfnTarget)(TSrcEdChangeInfo&)
                            , const tCIDLib::EEnds  eEnd = tCIDLib::EEnds::Last)
        {
            CheckHandlerRegParms(this, pTarget);
            AddNotifyRequest
            (
                new TNotifyRequestFor<T, TSrcEdChangeInfo>(pTarget, TSrcEditor::nidChange)
            );

            // Add a handler object to the target
            TNotHandlerBase* pnothNew = new TNotifyHandlerFor<T, TSrcEdChangeInfo>
            (
                pTarget, TSrcEditor::nidChange, pfnTarget
            );
            pTarget->AddNotifyHandler(pnothNew, eEnd);
            return pnothNew;
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, inheritd methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bEatSubClassMsg
        (
            const   tCIDCtrls::TWndMsg      wmsgMsg
            , const tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      lParam
            ,       tCIDCtrls::TMsgResult&  mresRet
        )   override;

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
        //  Private data members
        //
        //  m_bSelOnly
        //      This is one of the flags set by the SetSearchFlags. The others
        //      are just sent to the editor. This one we have to manage ourself.
        //
        //  m_c4LastLine
        //      We watch for notifications that might cause the line to change. In many
        //      cases it won't have. So, to avoid send a deluge of notifications, we
        //      remember the last one reported and only notify if it changes. We set
        //      it initially to 1.
        //
        //  m_strPath
        //      The caller gives us some sort of 'path' to indentify the text
        //      he loads into us. It's used in notifications we send.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bSelOnly;
        tCIDLib::TCard4     m_c4LastLine;
        TString             m_strPath;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddLineState
        (
            const   tCIDLib::TCard4         c4LineIndex
            , const tCIDWUtils::ECEdLStates eToSet
        );

        tCIDLib::TCard4 c4XlatMarker
        (
            const   tCIDWUtils::ECEdLStates eToAdd
        );

        tCIDLib::TVoid DoBPToggle
        (
            const   tCIDLib::TCard4         c4LineNum
        );

        tCIDLib::TVoid ExpandTabs
        (
            const   TString&                strToExp
            ,       TString&                strToFill
        );

        tCIDLib::TVoid SendEdEvent
        (
            const   tCIDWUtils::ESEdEvents  eEvent
            , const tCIDLib::TCard4         c4LineNum
        );


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TSrcEditor, TStdCtrlWnd)
};

#pragma CIDLIB_POPPACK


