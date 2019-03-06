//
// FILE NAME: CIDBuild_ToolsDriver.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/29/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDBuild_ToolsDriver.Cpp file. This file
//  provides the basic abstract interface for the pluggable tools driver
//  subsection. Each development enivonment (Visual C++, Visual Age, etc...)
//  has its own build environment issues which need to be abstracted. This
//  is the base class for all of those environment specific classes.
//
//  The basic interface is simple because the driver is allowed to do the
//  whole build of a project in a single shot. So its just given the project
//  info object and told to build. However, this class provides some concrete
//  implementation code that does common operations, such as parsing the
//  .Depend file to create the file dependency list.
//
// CAVEATS/GOTCHAS:
//
//  1)  The static methods tdrvBuilder() handles the lazy creation of the
//      the actual concrete derivative for the target build environment.
//      This requires a little conditional code for each platform and target
//      platform. In order to make things cleaner, he directly includes all
//      of the headers for the concrete derivatives of this class.
//
//      If the same compiler can be used on multiple platforms, then either
//      then separate drivers can be created (and the lazy eval method updated
//      to use the platform define to separate them), or a single one can
//      be used (and the platform define used to implement slightly different
//      functionality internally.)
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TDepInfo
//  PREFIX: depi
// ---------------------------------------------------------------------------
class TDepInfo
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDepInfo
        (
            const   TBldStr&                strFileName
            , const TProjectInfo&           projiParent
        );

        ~TDepInfo();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddHppDependent
        (
                    TFindInfo* const        pfndiHpp
        );

        const TFindInfo& fndiThis() const;

        const TList<TFindInfo>& listHpps() const;

        const TBldStr& strFileName() const;

        const TBldStr& strObjFileName() const;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TDepInfo();
        tCIDLib::TVoid operator==(const TDepInfo&);


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_fndiCpp
        //      The directory search info for the Cpp file that this object
        //      represents.
        //
        //  m_listHpps
        //      A list of directory search info objects for the Hpps that
        //      this Cpp file depends on.
        //
        //  m_strObjFile
        //      The name of the object file for this Cpp file. Its the full
        //      path to the object file.
        // -------------------------------------------------------------------
        TFindInfo           m_fndiCpp;
        TList<TFindInfo>    m_listHpps;
        TBldStr             m_strObjFile;
};




// ---------------------------------------------------------------------------
//   CLASS: TToolsDriver
//  PREFIX: tdrv
// ---------------------------------------------------------------------------
class TToolsDriver
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static TToolsDriver& tdrvBuilder();


        // -------------------------------------------------------------------
        //  Public Constructors and Destructor
        // -------------------------------------------------------------------
        ~TToolsDriver();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bBuildProject
        (
            const   TProjectInfo&           projiToBuild
        );


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TToolsDriver();


        // -------------------------------------------------------------------
        //  Local types
        // -------------------------------------------------------------------
        static const tCIDLib::TCard4 c4MaxHppDepsPerProj = 4096;


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bBuild
        (
            const   TProjectInfo&           projiTarget
        ) = 0;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bMustCompile
        (
            const   TProjectInfo&           projiTarget
            , const TDepInfo&               depiToCheck
        );

        TDepInfo* pdepiFindCpp
        (
            const   TBldStr&                strCppName
        );

        const TList<TDepInfo>& listCpps() const;

        const TList<TFindInfo>& listHpps() const;

        const TList<TFindInfo>& listLibs() const;

        tCIDLib::TVoid PostLink
        (
            const   TProjectInfo&           projiTarget
            , const TBldStr&                strTarget
        );



    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TToolsDriver(const TToolsDriver&);
        tCIDLib::TVoid operator=(const TToolsDriver&);


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddToCppsList
        (
                    TDepInfo&               depiTarget
            , const TBldStr&                strHeader
        );

        tCIDLib::TBoolean bLibListCallBack
        (
            const   TBldStr&                strTarget
            , const tCIDLib::TCard4         c4Level
        );

        tCIDLib::TVoid BuildLibName
        (
            const   TBldStr&                strTargetProject
            ,       TBldStr&                strToFill
        );

        TDepInfo& depiAddCpp
        (
            const   TBldStr&                strCppName
            , const TProjectInfo&           projiTarget
        );

        tCIDLib::TVoid ParseDepFile
        (
            const   TProjectInfo&           projiTarget
        );

        tCIDLib::TVoid ParseDependents
        (
                    TDepInfo&               depiCpp
            ,       TLineSpooler&           lsplSrc
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  NOTE: These lists are created by reading the .Depend file, which
        //        could be out of date with the .Projects file. So that's
        //        why we have our own list of Cpp files instead of using
        //        the one from the project info.
        //
        //        Depend files exist per-project, so the Cpp list is flushed
        //        and rebuilt on every call to bBuildProject(). However, the
        //        Hpp list is allowed to just build up because the same Hpps
        //        are seen again and again as we build higher levels of code.
        //
        //        The Cpp list is of TDepInfo objects, so its has a find info
        //        for the Cpp file and a list of find infos for all of the
        //        Hpp files that the Depend files says the Cpp file depends
        //        on.
        //
        //        The Libs list is built up by doing a standard bottom up
        //        iteration of the project dependencies of the target project
        //        and adding each one to the list of dependent libs.
        //
        //  m_listCpps
        //      The list of Cpp files. This gives us time stamp info about
        //      each Cpp file, and a list of references into the m_listiHpps
        //      file for all of the Hpps it depends upon. This is built as
        //      the .Depend file is read. It also contains the name of the
        //      object file for that Cpp file, so it serves as the object
        //      file list as well.
        //
        //  m_listHpps
        //      The list of all the headers that this project depends on.
        //      This gives us the time stamps of all of them. This is filled
        //      in as the .Depend file is read.
        //
        //  m_listLibs
        //      A list of all of the libs that the target project depends
        //      on. It is recursively built up, so it is all of them.
        //
        //  m_pprojiTar
        //      bBuild will store a pointer to the target so that our callbacks
        //      (from the iterator) can access the project info. We don't
        //      own it.
        // -------------------------------------------------------------------
        TList<TDepInfo>         m_listCpps;
        TList<TFindInfo>        m_listHpps;
        TList<TFindInfo>        m_listLibs;
        const TProjectInfo*     m_pprojiTar;

};


// ---------------------------------------------------------------------------
//  TDepInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
inline tCIDLib::TVoid TDepInfo::AddHppDependent(TFindInfo* const pfndiHpp)
{
    //
    //  Note that we set up this list not to adopt, so this is just
    //  referencing the Hpp find info that is really owned by the ToolsDriver
    //  object.
    //
    m_listHpps.Add(pfndiHpp);
}

inline const TFindInfo& TDepInfo::fndiThis() const
{
    return m_fndiCpp;
}

inline const TList<TFindInfo>& TDepInfo::listHpps() const
{
    return m_listHpps;
}

inline const TBldStr& TDepInfo::strFileName() const
{
    return m_fndiCpp.strFileName();
}

inline const TBldStr& TDepInfo::strObjFileName() const
{
    return m_strObjFile;
}



// ---------------------------------------------------------------------------
//  TToolsDriver: Protected, non-virtual methods
// ---------------------------------------------------------------------------
inline const TList<TDepInfo>& TToolsDriver::listCpps() const
{
    return m_listCpps;
}

inline const TList<TFindInfo>& TToolsDriver::listHpps() const
{
    return m_listHpps;
}

inline const TList<TFindInfo>& TToolsDriver::listLibs() const
{
    return m_listLibs;
}
