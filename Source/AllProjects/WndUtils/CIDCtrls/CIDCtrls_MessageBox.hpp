//
// FILE NAME: CIDCtrl_MsgBox.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/16/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This class implements the message box interface. We don't even bother to treat
//  these like windows. They are just self contained, run and close, and we don't
//  need to know what goes inside or deal with their internals.
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
//  CLASS: TMsgBox
// PREFIX: msgb
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TMsgBox : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMsgBox
        (
            const   TString&                strTitle
            , const TString&                strText
        );

        TMsgBox
        (
            const   TString&                strText
        );

        ~TMsgBox();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDCtrls::EMsgBoxRets eShow
        (
            const   TWindow&                wndOwner
            , const tCIDCtrls::EMsgBoxTypes eType
            , const tCIDCtrls::EMsgBoxIcons eIcon
            , const tCIDLib::TBoolean       bAppModal = kCIDLib::True
            , const tCIDLib::TBoolean       bTopMost = kCIDLib::False
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TMsgBox(const TMsgBox&);
        tCIDLib::TVoid operator=(const TMsgBox&);


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strText
        //  m_strTitle
        //      We store the title and text here until we can get it displayed. If
        //      the title is empty upon display, we will use the owner window's
        //      title text.
        // -------------------------------------------------------------------
        TString     m_strText;
        TString     m_strTitle;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMsgBox, TObject)
};


// ---------------------------------------------------------------------------
//  CLASS: TErrBox
// PREFIX: msgb
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TErrBox : public TMsgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TErrBox
        (
            const   TString&                strTitle
            , const TString&                strText
        );

        TErrBox
        (
            const   TString&                strText
        );

        ~TErrBox();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ShowIt
        (
            const   TWindow&                wndOwner
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TErrBox(const TErrBox&);
        tCIDLib::TVoid operator=(const TErrBox&);


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TErrBox, TMsgBox)
};



// ---------------------------------------------------------------------------
//  CLASS: TOkBox
// PREFIX: msgb
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TOkBox : public TMsgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TOkBox
        (
            const   TString&                strTitle
            , const TString&                strText
        );

        TOkBox
        (
            const   TString&                strText
        );

        ~TOkBox();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ShowIt
        (
            const   TWindow&                wndOwner
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TOkBox(const TOkBox&);
        tCIDLib::TVoid operator=(const TOkBox&);


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TOkBox, TMsgBox)
};


// ---------------------------------------------------------------------------
//  CLASS: TRetryCancelBox
// PREFIX: msgb
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TRetryCancelBox : public TMsgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TRetryCancelBox
        (
            const   TString&                strTitle
            , const TString&                strText
        );

        TRetryCancelBox
        (
            const   TString&                strText
        );

        ~TRetryCancelBox();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDCtrls::EMsgBoxRets eShowIt
        (
            const   TWindow&                wndOwner
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TRetryCancelBox(const TRetryCancelBox&);
        tCIDLib::TVoid operator=(const TRetryCancelBox&);


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TRetryCancelBox, TMsgBox)
};



// ---------------------------------------------------------------------------
//  CLASS: TYesNoBox
// PREFIX: msgb
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TYesNoBox : public TMsgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TYesNoBox
        (
            const   TString&                strTitle
            , const TString&                strText
        );

        TYesNoBox
        (
            const   TString&                strText
        );

        ~TYesNoBox();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bShowIt
        (
            const   TWindow&                wndOwner
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TYesNoBox(const TYesNoBox&);
        tCIDLib::TVoid operator=(const TYesNoBox&);


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TYesNoBox, TMsgBox)
};


// ---------------------------------------------------------------------------
//  CLASS: TYesNoCancelBox
// PREFIX: msgb
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TYesNoCancelBox : public TMsgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TYesNoCancelBox
        (
            const   TString&                strTitle
            , const TString&                strText
        );

        TYesNoCancelBox
        (
            const   TString&                strText
        );

        ~TYesNoCancelBox();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDCtrls::EMsgBoxRets eShowIt
        (
            const   TWindow&                wndOwner
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TYesNoCancelBox(const TYesNoCancelBox&);
        tCIDLib::TVoid operator=(const TYesNoCancelBox&);


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TYesNoCancelBox, TMsgBox)
};


#pragma CIDLIB_POPPACK


