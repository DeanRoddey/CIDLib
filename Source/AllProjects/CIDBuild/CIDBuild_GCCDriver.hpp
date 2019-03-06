//
// FILE NAME: CIDBuild_KCCDriver.Hpp
//
// AUTHOR: Will Mason
//
// CREATED: 02/24/99
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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

        ~TGCCDriver();


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDBuild::TBoolean bBuild
        (
            const   TProjectInfo&           projiTarget
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TGCCDriver(const TGCCDriver&);
        tCIDBuild::TVoid operator=(const TGCCDriver&);


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDBuild::TBoolean bCompileCpps();

        tCIDBuild::TBoolean bLibsNewer
        (
            const   TFindInfo&              fndiTarget
        )   const;

        tCIDBuild::TBoolean bMakeRes();

        tCIDBuild::TBoolean bObjsNewer
        (
            const   TFindInfo&              fndiTarget
        )   const;

        tCIDBuild::TVoid Link();


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
        tCIDBuild::TBoolean m_bDebug;
        const TProjectInfo* m_pprojiTarget;
        TBldStr             m_strTargetFile;
};

