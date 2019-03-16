//
// FILE NAME: CIDBuild_DependGraph.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/22/1998
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
//  This is the header for the CIDBuild_DependGraph.Cpp file. This file
//  implements the TDependGraph class, which is used to maintain a dependency
//  graph among all of the projects. That way, the outside world can ask
//  what projects depend upon any others, we can check for circular
//  dependencies, etc...
//
//  Its a pretty simple data structure, but keeping it hidden makes things
//  simpler. Its basically done as a two way adjacency graph in a 2D array.
//
//  A magic 0th element is added which is the 'all' project. This one depends
//  upon all other projects, so its a way to orthogonally kick off iterations
//  of all projects.
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
class   TProjIterator;


// ---------------------------------------------------------------------------
//   CLASS: TDependGraph
//  PREFIX: depg
// ---------------------------------------------------------------------------
class TDependGraph
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDependGraph();

        ~TDependGraph();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddDependency
        (
            const   TBldStr&                strTarget
            , const TBldStr&                strDependent
        );

        tCIDLib::TVoid AddDependency
        (
            const   tCIDLib::TCard4         c4Target
            , const TBldStr&                strDependent
        );

        tCIDLib::TBoolean bDependsOn
        (
            const   TBldStr&                strToCheck
            , const TBldStr&                strDependent
            , const tCIDLib::TBoolean       bMustExist
        )   const;

        tCIDLib::TBoolean bDependsOn
        (
            const   TBldStr&                strToCheck
            , const tCIDLib::TCard4         c4DepIndex
        )   const;

        tCIDLib::TBoolean bDependsOn
        (
            const   tCIDLib::TCard4         c4ToCheck
            , const tCIDLib::TCard4         c4DepIndex
        )   const;

        tCIDLib::TBoolean bElementExists
        (
            const   TBldStr&                strToCheck
        )   const;

        tCIDLib::TBoolean bHasDependencies
        (
            const   TBldStr&                strToCheck
        )   const;

        tCIDLib::TBoolean bHasDependencies
        (
            const   tCIDLib::TCard4         c4ToCheck
        )   const;

        tCIDLib::TBoolean bIterate
        (
            const   TProjIterator&          projiCallBack
            , const TBldStr&                strStartProject
            , const tCIDBuild::EIterModes   eMode
        )   const;

        tCIDLib::TBoolean bTestCircular() const;

        tCIDLib::TVoid Clear();

        const TBldStr& strElementAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        tCIDLib::TCard4 c4AddNewElement
        (
            const   TBldStr&                strNewName
        );

        tCIDLib::TCard4 c4CurElements() const;


    private :
        // -------------------------------------------------------------------
        //  Private class constants
        // -------------------------------------------------------------------
        static const tCIDLib::TCard4 c4MaxEntries = 256;


        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TDependGraph(const TDependGraph&);
        tCIDLib::TVoid operator=(const TDependGraph&);


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckDependencies
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        tCIDLib::TBoolean bIterateBottomUp
        (
            const   tCIDLib::TCard4         c4Cur
            ,       tCIDLib::TCard1* const  pc1Done
            , const TProjIterator&          projiCallBack
            , const tCIDLib::TCard4         c4Level
            , const tCIDLib::TBoolean       bMinimal
            , const tCIDLib::TCard4         c4Skip
        )   const;

        tCIDLib::TBoolean bIterateTopDown
        (
            const   tCIDLib::TCard4         c4Cur
            ,       tCIDLib::TCard1* const  pabDone
            , const TProjIterator&          projiCallBack
            , const tCIDLib::TCard4         c4Level
            , const tCIDLib::TBoolean       bMinimal
            , const tCIDLib::TCard4         c4Skip
        )   const;

        tCIDLib::TCard4 c4AddElement
        (
            const   TBldStr&                strNewName
        );

        tCIDLib::TCard4 c4IndexOfElement
        (
            const   TBldStr&                strNewName
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_abList
        //      The two dimensional array of bytes used to implement the
        //      graph. The first dimension is the map of facilities. The
        //      second is the set of facilities each first dimension depends
        //      on. A zero or one is stored to indicate a dependency.
        //
        //  m_astrNames
        //      The names of the elements that have been added. These map
        //      by position to the same indexes into the m_abList array.
        //
        //  m_c4CurElements
        //      The current number of elements that have been added. So that
        //      means the number of elements in both dimensions that are
        //      valid.
        // -------------------------------------------------------------------
        tCIDLib::TCard1     m_ac1List[c4MaxEntries][c4MaxEntries];
        TBldStr             m_astrNames[c4MaxEntries];
        tCIDLib::TCard4     m_c4CurElements;
};


// ---------------------------------------------------------------------------
//  TDependGraph: Public, non-virtual methods
// ---------------------------------------------------------------------------
inline tCIDLib::TCard4 TDependGraph::c4CurElements() const
{
    return m_c4CurElements;
}

