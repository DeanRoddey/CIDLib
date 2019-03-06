//
// FILE NAME: TestMacroDbg_FileClassMgr.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/16/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the file file TestMacroDlg_FileClassMgr.cpp, which
//  implements a simple derivative of one of the pre-fab class managers
//  provided by the macro debugger engine. The one we use supports a simple
//  root directory based class management scheme, which is what we want, but
//  it doesn't do anything for the 'select class' request. We provide that
//  override and just invoke the file selection dialog.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

class TMEngDbgFrameWnd;

// ---------------------------------------------------------------------------
//  CLASS: TMEngFixedBaseClassMgr
// PREFIX: mecm
// ---------------------------------------------------------------------------
class TMacroDbgClassMgr : public TMEngFixedBaseClassMgr
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMacroDbgClassMgr
        (
                    TMEngDbgFrameWnd* const pwndOwner
        );

        TMacroDbgClassMgr
        (
            const   TString&                strBasePath
            ,       TMEngDbgFrameWnd* const pwndOwner

        );

        ~TMacroDbgClassMgr();


    protected :
        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::EOpenRes eDoSelect
        (
                    TString&                strToFill
            , const tCIDMacroEng::EResModes    eMode
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TMacroDbgClassMgr(const TMacroDbgClassMgr&);
        tCIDLib::TVoid operator=(const TMacroDbgClassMgr&);


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pwndOwner
        //      The main test frame is given to use so that we can correctly
        //      be modal to the frame when we pop up the file select dialog.
        // -------------------------------------------------------------------
        TMEngDbgFrameWnd*   m_pwndOwner;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMacroDbgClassMgr,TMEngFixedBaseClassMgr)
};


