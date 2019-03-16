//
// FILE NAME: CIDCtrls_DlgDesc.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/26/2000
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
//  This is the header for the CIDCtrls_DlgDesc.cpp file, which implements
//  the TDlgDesc class. This class describes a dialog and its widgets. It can
//  be used at runtime by client code, but its main purpose is as the in file
//  representation of dialog resources.
//
// CAVEATS/GOTCHAS:
//
//  1)  The positions and sizes of dialog items are not pixels. Instead,
//      they are in terms of dialog units.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TDlgItem
// PREFIX: dlgi
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TDlgItem : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bFindHint
        (
            const   tCIDCtrls::THintsList&  colHints
            , const TString&                strToFind
            ,       tCIDLib::TCard4&        c4At
        );

        static tCIDLib::TBoolean bParseResImage
        (
            const   TString&                strText
            ,       tCIDLib::TResId&        ridImg
            ,       TString&                strFacName
            ,       tCIDImage::EImgTypes&   eType
        );

        static tCIDLib::TVoid ParseHints
        (
            const   TString&                strText
            ,       tCIDLib::TStrList&      colToFill
        );

        static tCIDCtrls::EAppImgSz eParseAppImage
        (
            const   TString&                strText
            ,       TString&                strImgName
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDlgItem();

        TDlgItem
        (
            const   tCIDLib::TResId         ridChild
            , const TString&                strText
            , const TString&                strCueText
            , const TString&                strFlyText
            , const TArea&                  areaPos
            , const TString&                strType
            , const tCIDCtrls::THintsList   colHints
            , const TString&                strImage
            , const tCIDCtrls::EEdgeAnchors eAnchor
        );

        TDlgItem
        (
            const   tCIDCtrls::TRawDlgItem& rdlgiSrc
            , const TFacility&              facTextSrc
        );

        TDlgItem
        (
            const   TDlgItem&               dlgiToCopy
        );

        ~TDlgItem();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDlgItem& operator=
        (
            const   TDlgItem&               dlgiToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual
        // -------------------------------------------------------------------
        const TArea& areaPos() const;

        const TArea& areaPos
        (
            const   TArea&                  areaToSet
        );

        tCIDLib::TVoid AddHint
        (
            const   TString&                strToAdd
        );

        tCIDLib::TBoolean bHasHint
        (
            const   TString&                strToCheck
        )   const;

        const tCIDCtrls::THintsList& colHints() const;

        tCIDCtrls::EEdgeAnchors eAnchor() const;

        tCIDCtrls::EEdgeAnchors eAnchor
        (
            const   tCIDCtrls::EEdgeAnchors eToSet
        );

        tCIDCtrls::EDlgItemFlags eFlags() const;

        tCIDLib::TVoid FromRaw
        (
            const   tCIDCtrls::TRawDlgItem& rdlgiSrc
            , const TFacility&              facTextSrc
        );

        tCIDLib::TResId ridChild() const;

        const TString& strCppType() const;

        const TString& strCppType
        (
            const   TString&                strToSet
        );

        const TString& strCueText() const;

        const TString& strFlyoverText() const;

        const TString& strImage() const;

        const TString& strText() const;

        const TString& strType() const;



    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_areaPos
        //      The position of this widget within the dialog. As stored on disk it is in
        //      dialog units, but when we load up the raw stuff into us, we convert to
        //      screen units at that point, so these are always screen units.
        //
        //  m_colHints
        //      A list of hints that tells the control about special circumstances it should
        //      take into account. Combined with the parent window's theme, this gives them
        //      the info they need to set themselves up. We parse themout into separate
        //      tokens for efficiency. We can't use a hash set since we can't really
        //      guarantee they'll always be in a consistent case. But there are seldom more
        //      than a handful.
        //
        //  m_eAnchor
        //      Indicates whether this window should be affected by an auto-size/pos operation
        //      of the parent.
        //
        //  m_eFlags
        //      The per-item flags set for this item in the resource text.
        //
        //  m_ridThis
        //      This resource id that this particular item should have, which becomes the
        //      window id of the control.
        //
        //  m_strCppType
        //      Usually this is empty and m_strType is used to figure out a default C++
        //      class. But, sometimes applications will load the dialog resource then manually
        //      set some custom class to be used (generally a derivative of the original.)
        //
        //  m_strCueText
        //      The cue text for the widget, if any. This is for things like entry
        //      fields or combo boxes, to prompt the user for what to enter.
        //
        //  m_strFlyoverText
        //      The flyover text for the widget, if any.
        //
        //  m_strImage
        //      If the control can load an app image, then this can be set to the name of
        //      the image to load.
        //
        //  m_strText
        //      The text of the control, if any.
        //
        //  m_strType
        //      The item type, which is the name used to identify it in the CIDRC file. We
        //      translate this to a real C++ class name, if the type hasn't been fixed by
        //      he application before creating the dialog. If so, then m_strCppClass is set.
        // -------------------------------------------------------------------
        TArea                       m_areaPos;
        tCIDCtrls::THintsList       m_colHints;
        tCIDCtrls::EEdgeAnchors     m_eAnchor;
        tCIDCtrls::EDlgItemFlags    m_eFlags;
        tCIDLib::TResId             m_ridThis;
        TString                     m_strCppType;
        TString                     m_strCueText;
        TString                     m_strFlyoverText;
        TString                     m_strImage;
        TString                     m_strText;
        TString                     m_strType;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDlgItem,TObject)
};


// ---------------------------------------------------------------------------
//  CLASS: TDlgDesc
// PREFIX: dlgd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TDlgDesc : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public types
        // -------------------------------------------------------------------
        typedef TVector<TDlgItem>::TCursor      TCursor;
        typedef TVector<TDlgItem>::TNCCursor    TNCCursor;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDlgDesc();

        TDlgDesc
        (
            const   TString&                strTitle
            , const tCIDCtrls::EWndThemes   eTheme
            , const TArea&                  areaPos
        );

        TDlgDesc
        (
            const   TDlgDesc&               dlgdToCoyp
        );

        TDlgDesc
        (
            const   tCIDCtrls::TRawDlgDesc& rdlgdSrc
            , const TFacility&              facTextSrc
        );

        ~TDlgDesc();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDlgDesc& operator=
        (
            const   TDlgDesc&               dlgdToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TArea& areaPos() const;

        const TArea& areaPos
        (
            const   TArea&                  areaToSet
        );

        tCIDLib::TVoid AddItem
        (
            const   TDlgItem&               dlgiToAdd
        );

        TCursor cursItems() const;

        TDlgItem& dlgiFindById
        (
            const   tCIDLib::TResId         ridToFind
        );

        const TDlgItem& dlgiFindById
        (
            const   tCIDLib::TResId         ridToFind
        )   const;

        tCIDCtrls::EDlgFlags eFlags() const;

        tCIDCtrls::EWndThemes eTheme() const;

        tCIDCtrls::EWndThemes eTheme
        (
            const   tCIDCtrls::EWndThemes   eToSet
        );

        tCIDLib::TVoid FromRaw
        (
            const   tCIDCtrls::TRawDlgDesc& rdlgdSrc
            , const TFacility&              facTextSrc
        );

        const TDlgItem* pdlgiFindById
        (
            const   tCIDLib::TResId         ridToFind
        )   const;

        TDlgItem* pdlgiFindById
        (
            const   tCIDLib::TResId         ridToFind
        );

        tCIDLib::TResId ridThis() const;

        tCIDLib::TVoid RemoveItem
        (
            const   tCIDLib::TResId         ridToRemove
        );

        const TString& strTitle() const;

        tCIDLib::TVoid SetOrigin
        (
            const   TPoint&                 pntToSet
        );

        tCIDLib::TVoid SetCppType
        (
            const   tCIDLib::TResId         ridToSet
            , const TString&                strType
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ChildrenFromRaw
        (
            const   tCIDCtrls::TRawDlgDesc& rdlgdSrc
            , const TFacility&              facTextSrc
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_areaPos
        //      The size/pos to place the dialog. If the flags indicate that we should
        //      center it, then the origin is overridden. This is in Window units,
        //      see the header comments.
        //
        //  m_eFlags
        //      Some optional flags to control various aspects of the dialog.
        //
        //  m_eTheme
        //      The window theme to apply to this dialog and its children.
        //
        //  m_pcolItems
        //      A collection of dialog items. The order is significant since
        //      order of creation controls Z order, which controls tabbing.
        //
        //  m_ridThis
        //      The resource id of this dialog description
        //
        //  m_strTitle
        //      The title text for the dialog, i.e. what goes into the title
        //      bar.
        // -------------------------------------------------------------------
        TArea                   m_areaPos;
        tCIDCtrls::EDlgFlags    m_eFlags;
        tCIDCtrls::EWndThemes   m_eTheme;
        TVector<TDlgItem>*      m_pcolItems;
        tCIDLib::TResId         m_ridThis;
        TString                 m_strTitle;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDlgDesc,TObject)
};

#pragma CIDLIB_POPPACK


