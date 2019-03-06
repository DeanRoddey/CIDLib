//
// FILE NAME: CIDCtrls_InPlaceEdit.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/06/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements a general purpose in place editor window that can be popped
//  up over list control rows/cells to get a new value.
//
//  We also define a simple mixin interface that lets the caller provide us with a
//  validation mechanism. Since it's a mixin, it can easily have access to their
//  data, without having to do anything tricky.
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


// ---------------------------------------------------------------------------
//  CLASS: MIPEIntf
// PREFIX: ipei
// ---------------------------------------------------------------------------
class CIDCTRLSEXP MIPEIntf
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bTestEnumVal
        (
            const   TAttrData&              adatTest
            ,       TString&                strCanonVal
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        MIPEIntf();

        MIPEIntf(const MIPEIntf&) = delete;

        virtual ~MIPEIntf();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        MIPEIntf& operator=(const MIPEIntf&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bIPEValidate
        (
            const   TString&                strSrc
            ,       TAttrData&              adatTar
            , const TString&                strNewVal
            ,       TString&                strErrMsg
            ,       tCIDLib::TCard8&        c8UserId
        )   const;

        virtual tCIDLib::TVoid IPEValChanged
        (
            const   tCIDCtrls::TWndId       widSrc
            , const TString&                strSrc
            , const TAttrData&              adatNew
            , const TAttrData&              adatOld
            , const tCIDLib::TCard8         c8UserId
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIPEHasFloatRange
        (
            const   TString&                strCheck
            ,       tCIDLib::TFloat8&       f8Min
            ,       tCIDLib::TFloat8&       f8Max
            , const tCIDLib::TBoolean       bPrefix
        )   const;

        tCIDLib::TBoolean bIPEHasRange
        (
            const   TString&                strCheck
            ,       tCIDLib::TInt8&         i8Min
            ,       tCIDLib::TInt8&         i8Max
            , const tCIDLib::TBoolean       bPrefix
        )   const;

        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4IPERow() const;

        tCIDLib::TCard4 c4IPERow
        (
            const   tCIDLib::TCard4         c4ToSet
        );


    private :
        // -------------------------------------------------------------------
        //  For those users of this interface that have a row based scheme, like
        //  the multi-column list box, they can set this before invoking the IPE
        //  and that will make the current row available. Otherwise, it'll be set
        //  to the default 0.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4IPECurRow;
};



// ---------------------------------------------------------------------------
//  CLASS: TInPlaceEdit
// PREFIX: ipe
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TInPlaceEdit : public TWindow
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TInPlaceEdit();

        TInPlaceEdit(const TInPlaceEdit&) = delete;

        ~TInPlaceEdit();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TInPlaceEdit& operator=(const TInPlaceEdit&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bProcess
        (
                    TWindow&                wndOwner
            , const TArea&                  areaEdit
            ,       TAttrData&              adatTar
            ,       MIPEIntf&               ipeiValidator
            , const tCIDLib::TCard8         c8UserId
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bEatSubClassMsg
        (
            const   tCIDCtrls::TWndMsg      wmsgMsg
            , const tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      lParam
            ,       tCIDCtrls::TMsgResult&  mresRet
        )   override;

        tCIDLib::TVoid Destroyed() override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bExit
        //      We set this in the subclass processing if we see anything that should
        //      make the modal loop exit. m_bSaveVal should be set if appropriate.
        //
        //  m_bFirstErr
        //      This is used to create the error tool tip upon the first need to
        //      display an error.
        //
        //  m_bSaveVal
        //      A flag to indicate, after we exit the modal loop, whether the value
        //      should be saved or not.
        //
        //  m_wndErrTip
        //      We use a tool tip to display any errors. It's not created until we
        //      actually need to display an error.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bExit;
        tCIDLib::TBoolean   m_bFirstErr;
        tCIDLib::TBoolean   m_bSaveVal;
        TToolTip            m_wndErrTip;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TInPlaceEdit, TWindow)
};

#pragma CIDLIB_POPPACK

