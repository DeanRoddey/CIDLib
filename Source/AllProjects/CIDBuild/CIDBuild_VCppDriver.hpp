//
// FILE NAME: CIDBuild_VisualCppDriver.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/29/1998
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
//  This is the header for the CIDBuild_VisualCppDriver.Cpp file. This file
//  implements the TVCppDriver class, which is the concrete instantation
//  of the TToolsDriver interface for the MS Visual C++ environment.
//
// CAVEATS/GOTCHAS:
//
//  1)  Minimal rebuild support was dropped here in favor of parallel compilation
//      mode.
//
// LOG:
//
//  $_CIDLib_Log_$
//



// ---------------------------------------------------------------------------
//   CLASS: TVCppDriver
//  PREFIX: tdrv
// ---------------------------------------------------------------------------
class TVCppDriver : public TToolsDriver
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TVCppDriver();

        TVCppDriver(const TVCppDriver&) = delete;
        TVCppDriver(TVCppDriver&&) = delete;

        ~TVCppDriver();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TVCppDriver& operator=(const TVCppDriver&) = delete;
        TVCppDriver& operator=(TVCppDriver&&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bBuild
        (
            const   TProjectInfo&           projiTarget
        )   override;

        tCIDLib::TBoolean bInvokeDebugger
        (
            const   TProjectInfo&           projiTarget
        )   override;

        tCIDLib::TVoid ResetDebugInfo
        (
            const   TProjectInfo&           projiToReset
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCompileOne
        (
            const   tCIDLib::TCh**          apszParms
            , const tCIDLib::TCard4         c4ParmCnt
            ,       tCIDLib::TCard4&        c4ErrCode
        );

        tCIDLib::TBoolean bCompileCpps();

        tCIDLib::TBoolean bCompileShaders();

        tCIDLib::TBoolean bLibsNewer
        (
            const   TFindInfo&              fndiTarget
        )   const;

        tCIDLib::TBoolean bObjsNewer
        (
            const   TFindInfo&              fndiTarget
        )   const;

        tCIDLib::TVoid Link
        (
            const   tCIDLib::TBoolean       bHaveRCFile
        );

        tCIDLib::TVoid LinkStatic();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bDebug
        //      We just get a quick access, TBoolean, flag for whether we are
        //      in debug or production mode. This makes things more
        //      convenient.
        //
        //  m_pprojiTarget
        //      We store the target project here so that is available during
        //      all of the internal calls we make during the build.
        //
        //  m_strTargetFile
        //      The path to the output file (Exe or Dll), which we have to
        //      use in a couple places, so we build it once and keep it
        //      around. Keep in mind that it has no path part since we link
        //      in the output directory.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bDebug;
        const TProjectInfo* m_pprojiTarget;
        TBldStr             m_strTargetFile;
};
