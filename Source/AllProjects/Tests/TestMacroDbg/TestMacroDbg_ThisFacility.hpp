//
// FILE NAME: TestMacroDbg_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/14/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the facility class of this facility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TFacTestMacroDbg
//  PREFIX: fac
// ---------------------------------------------------------------------------
class TFacTestMacroDbg : public TGUIFacility
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacTestMacroDbg();

        ~TFacTestMacroDbg();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eMainThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        const TString& strClassPathRoot() const;

        const TString& strFileSysRoot() const;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TFacTestMacroDbg(const TFacTestMacroDbg&);
        tCIDLib::TVoid operator=(const TFacTestMacroDbg&);


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strClassPathRoot
        //      All classes are loaded from a hierarchy underneath a given
        //      directory. This is set by a command line parm.
        //
        //  m_strFileSysRoot
        //      We us a simple means of expanding macro paths. We just make
        //      them all exist below a particular directory. This is set by
        //      a command line parm.
        //
        //  m_wndMainFrame
        //      Our main frame window. This guy lets the user select a macro
        //      to debug, and then loads up the macro debugger for it.
        // -------------------------------------------------------------------
        TString             m_strClassPathRoot;
        TString             m_strFileSysRoot;
        TMEngDbgFrameWnd    m_wndMainFrame;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacTestMacroDbg,TGUIFacility)
};


