//
// FILE NAME: CIDBuild_KCCDriver.Hpp
//
// AUTHOR: Will Mason
//
// CREATED: 02/24/99
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
//  This is the header for the CIDBuild_KCCDriver.Cpp file. This file
//  implements the TGCCDriver class, which is the concrete instantation
//  of the TToolsDriver interface for the KAI C++ environment.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//



// ---------------------------------------------------------------------------
//   CLASS: TGCCDriver
//  PREFIX: tdrv
// ---------------------------------------------------------------------------
class TGCCDriver : public TToolsDriver
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Operators
        // -------------------------------------------------------------------
        TGCCDriver();

        TGCCDriver(const TGCCDriver&) = delete;

        ~TGCCDriver();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TGCCDriver& operator=(const TGCCDriver&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bBuild
        (
            const   TProjectInfo&           projiTarget
        );

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
        tCIDLib::TBoolean bCompileCpps();

        tCIDLib::TBoolean bLibsNewer
        (
            const   TFindInfo&              fndiTarget
        )   const;

        tCIDLib::TBoolean bMakeRes();

        tCIDLib::TBoolean bObjsNewer
        (
            const   TFindInfo&              fndiTarget
        )   const;

        tCIDLib::TVoid Link();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bDebug
        //      We just get a quick access, TBoolean, flag for whether we are
        //      in debug or production mode. This makes things more convenient.
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

