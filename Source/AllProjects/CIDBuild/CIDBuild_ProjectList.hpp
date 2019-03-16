//
// FILE NAME: CIDBuild_TProjectList.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/21/1998
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
//  This is the header for the CIDBuild_TProjectList.Cpp file. This file
//  implements the TTProjectList class. This class handles parsing the .Projects
//  file, building the list of TProjSettings objects, and building the
//  dependency graph of projects.
//
//  Once all that is done, the build process can easily figure out what
//  it needs to do in the build just by asking questions of the project list
//  object about what needs to be rebuilt and what the settings are for a
//  project, etc...
//
//  And, very importantly, the outside world can provide an iterator call
//  back object to the bIterate() method to have the projects iterated in
//  a number of different ways that are needed during processing. It will
//  iterate them bottom up (as they would be built) or top down (as when
//  displaying a dependency tree). This iteration mechanism drives most of
//  the operations that the program does.
//
//  The iteration can be minimal (a particular dependent project is only
//  ever called back on once, then its marked as done, such as would be
//  done when building from the bottom up. Or it can be non-minimal, which
//  means that the literal tree of dependencies is called back on, as in the
//  case of displaying the depenency tree. Any combination of these modes
//  is allowed.
//
//  TProjIterator and TMethProjIterator provide the callback mechanism
//  by which the project list calls back during iteration. TProjIterator
//  is the abstract base class interface, and TMethProjIterator is a very
//  nice template class that allows the callback to be on a method of an
//  object, which is how its generally used here (though you could do your
//  own derivative of TProjIterator for very special case jobs.)
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class TProjIterator;


// ---------------------------------------------------------------------------
//   CLASS: TProjectList
//  PREFIX: plist
// ---------------------------------------------------------------------------
class TProjectList
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TProjectList();

        ~TProjectList();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDependsOn
        (
            const   TBldStr&                strSrcProj
            , const TBldStr&                strToCheck
            , const tCIDLib::TBoolean       bMustExist
        )   const;

        tCIDLib::TBoolean bIterate
        (
            const   TProjIterator&          projiCallBack
            , const TBldStr&                strStartProject
            , const tCIDBuild::EIterModes   eMode
        )   const;

        tCIDLib::TBoolean bProjectExists
        (
            const   TBldStr&                strToFind
        )   const;

        tCIDLib::TVoid DumpProjectSettings
        (
            const   TBldStr&                strTarget
        )   const;

        tCIDLib::TCard4 c4ProjectCount() const;

        const TList<TProjectInfo>& listProjects() const;

        tCIDLib::TVoid LoadFileLists();

        const TProjectInfo& projiByName
        (
            const   TBldStr&                strToFind
        )   const;

        TProjectInfo& projiByName
        (
            const   TBldStr&                strToFind
        );

        tCIDLib::TVoid ParseProjectFile();

        tCIDLib::TVoid RemoveAll();



    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TProjectList(const TProjectList&);
        tCIDLib::TVoid operator=(const TProjectList&);


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid LoadFileLists
        (
                    TProjectInfo&           projiToLoad
        );

        const TProjectInfo* pprojiFindProject
        (
            const   TBldStr&                strToFind
        )   const;

        TProjectInfo* pprojiFindProject
        (
            const   TBldStr&                strToFind
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_listProjects;
        //      A list of TProjectInfo objects that is used to track all
        //      of the projects that are defined in the .Projects file.
        //
        //  m_depgList
        //      The dependency graph for the system. After parsing the
        //      .Projects file, all of the dependencies are fed into the
        //      graph to build the dependencies.
        // -------------------------------------------------------------------
        TList<TProjectInfo> m_listProjects;
        TDependGraph        m_depgList;
};



// ---------------------------------------------------------------------------
//   CLASS: TProjIterator
//  PREFIX: piter
// ---------------------------------------------------------------------------
class TProjIterator
{
    public :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        ~TProjIterator();


    protected :
        // -------------------------------------------------------------------
        //  Declare our friend. The dependency graph needs to be able to
        //  call the bNextProject callback method.
        // -------------------------------------------------------------------
        friend class TDependGraph;


        // -------------------------------------------------------------------
        //  Constructor
        // -------------------------------------------------------------------
        TProjIterator();


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bNextProject
        (
            const   TBldStr&                strProjName
            , const tCIDLib::TCard4         c4Level
        )   const = 0;
};



// ---------------------------------------------------------------------------
//   CLASS: TMethProjIterator
//  PREFIX: piter
// ---------------------------------------------------------------------------
template <class T> class TMethProjIterator : public TProjIterator
{
    public :
        // -------------------------------------------------------------------
        //  The prototype for the callback methods
        // -------------------------------------------------------------------
        typedef tCIDLib::TBoolean (T::*TIterMethod)(const TBldStr&, const tCIDLib::TCard4);


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMethProjIterator(T* const pobjTarget, TIterMethod pfnCallBack) :

            m_pobjTarget(pobjTarget)
            , m_pfnCallBack(pfnCallBack)
        {
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bNextProject( const   TBldStr&        strProjName
                                        , const tCIDLib::TCard4 c4Level) const
        {
            return (m_pobjTarget->*m_pfnCallBack)(strProjName, c4Level);
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pobjTarget
        //      The object on which the callback is done.
        //
        //  m_pfnCallBack
        //      This is the function we call back on.
        // -------------------------------------------------------------------
        T*              m_pobjTarget;
        TIterMethod     m_pfnCallBack;
};


// ---------------------------------------------------------------------------
//  TProjectList: Public, non-virtual methods
// ---------------------------------------------------------------------------
inline tCIDLib::TBoolean
TProjectList::bIterate( const   TProjIterator&          projiCallBack
                        , const TBldStr&                strStartAt
                        , const tCIDBuild::EIterModes   eMode) const
{
    // Just pass it on to our dependency graph
    return m_depgList.bIterate(projiCallBack, strStartAt, eMode);
}


inline const TList<TProjectInfo>& TProjectList::listProjects() const
{
    return m_listProjects;
}


inline tCIDLib::TCard4 TProjectList::c4ProjectCount() const
{
    return m_listProjects.c4ElemCount();
}


// ---------------------------------------------------------------------------
//  TProjIterator: Destructor
// ---------------------------------------------------------------------------
inline TProjIterator::~TProjIterator()
{
}


// ---------------------------------------------------------------------------
//  TProjIterator: Hidden constructors
// ---------------------------------------------------------------------------
inline TProjIterator::TProjIterator()
{
}

