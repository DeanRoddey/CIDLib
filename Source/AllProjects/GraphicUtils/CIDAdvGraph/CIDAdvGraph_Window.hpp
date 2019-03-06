//
// FILE NAME: CIDAdvGraph_Window.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/19/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDAdvGraph_Window.cpp file. This file
//  implements a derivative of the TWindow class that is designed to support
//  advanced graphics output. It wraps the graphics output and display to a
//  window all together.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


#pragma CIDLIB_PACK(CIDLIBPACK)

// Forward reference our internal data structure
struct TAdvGrWndData;


// ---------------------------------------------------------------------------
//  CLASS: TCIDAdvGraphWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDADVGREXP TCIDAdvGraphWnd : public TWindow
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDAdvGraphWnd();

        ~TCIDAdvGraphWnd();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Create
        (
            const   TWindow&                wndParent
            , const TArea&                  areaInit
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::EExWndStyles eExStyles
            , const tCIDCtrls::TWndId       widThis
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreated() override;

        tCIDLib::TVoid Destroyed() override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pIntData
        //      This is an opaque data type publically, and we define it
        //      internally.
        // -------------------------------------------------------------------
        TAdvGrWndData*  m_pData;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDAdvGraphWnd,TWindow)
};

#pragma CIDLIB_POPPACK

