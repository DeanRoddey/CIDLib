//
// FILE NAME: CIDBuild_DependGraph.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/21/1998
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
//  This file implements the TDependGraph class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDBuild.hpp"
#include    <memory.h>



// ---------------------------------------------------------------------------
//   CLASS: TDependGraph
//  PREFIX: depg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDependGraph: Constructors and Destructor
// ---------------------------------------------------------------------------
TDependGraph::TDependGraph() :

    m_c4CurElements(0)
{
    // Initialize all of the data
    Clear();
}

TDependGraph::~TDependGraph()
{
}


// ---------------------------------------------------------------------------
//  TDependGraph: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDependGraph::AddDependency(const   TBldStr&    strTarget
                            , const TBldStr&    strDependent)
{
    // The target must be present already
    const tCIDLib::TCard4 c4Target = c4IndexOfElement(strTarget);
    if (c4Target == kCIDBuild::c4NotFound)
    {
        stdOut  << L"Target project '"
                << strTarget << L"' was not found in dependency graph"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::NotFound;
    }

    // The dependent must be present already
    const tCIDLib::TCard4 c4Depend = c4IndexOfElement(strDependent);
    if (c4Depend == kCIDBuild::c4NotFound)
    {
        stdOut  << L"Dependent project '"
                << strDependent << L"' was not found in dependency graph (for "
                << strTarget << L")"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::NotFound;
    }

    //
    //  And now we just mark the element. If its already marked, that's
    //  fine. We don't track number of dependencies, just a boolean
    //  dependency flag.
    //
    m_ac1List[c4Target][c4Depend] = 1;
}

tCIDLib::TVoid
TDependGraph::AddDependency(const   tCIDLib::TCard4 c4Target
                            , const TBldStr&        strDependent)
{
    if (c4Target >= m_c4CurElements)
    {
        stdOut << L"Illegal target index for dependency graph" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::IndexError;
    }

    // The dependent must be present already
    const tCIDLib::TCard4 c4Depend = c4IndexOfElement(strDependent);
    if (c4Depend == kCIDBuild::c4NotFound)
    {
        stdOut  << L"Dependent project '" << strDependent
                << L"' was not found (for "
                << m_astrNames[c4Target] << L')'
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::NotFound;
    }

    //
    //  And now we just mark the element. If its already marked, that's
    //  fine. We don't track number of dependencies, just a boolean
    //  dependency flag.
    //
    m_ac1List[c4Target][c4Depend] = 1;
}


tCIDLib::TBoolean
TDependGraph::bDependsOn(const  TBldStr&            strToCheck
                        , const TBldStr&            strDependent
                        , const tCIDLib::TBoolean   bMustExist) const
{
    // Find both of the elements
    const tCIDLib::TCard4 c4Target = c4IndexOfElement(strToCheck);
    if (c4Target == kCIDBuild::c4NotFound)
    {
        stdOut  << L"Target project '" << strToCheck
                << L"' was not found in dependency graph" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::NotFound;
    }

    const tCIDLib::TCard4 c4Depend = c4IndexOfElement(strDependent);
    if (c4Depend == kCIDBuild::c4NotFound)
    {
        // This one doesn't necessarily have to exist
        if (bMustExist)
        {
            stdOut  << L"Dependent project '" << strDependent
                    << L"' was not found in dependency graph" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::NotFound;
        }

        // Just say we don't depend on it
        return kCIDLib::False;
    }

    // And now its just a lookup
    return (m_ac1List[c4Target][c4Depend] != 0);
}

tCIDLib::TBoolean
TDependGraph::bDependsOn(const  TBldStr&        strToCheck
                        , const tCIDLib::TCard4 c4DepIndex) const
{
    // Find both of the elements
    const tCIDLib::TCard4 c4Target = c4IndexOfElement(strToCheck);
    if (c4Target == kCIDBuild::c4NotFound)
    {
        stdOut  << L"Target project '" << strToCheck
                << L"' was not found in dependency graph" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::NotFound;
    }

    if (c4DepIndex >= m_c4CurElements)
    {
        stdOut  << L"Illegal dependent index for dependency graph"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::IndexError;
    }
    return (m_ac1List[c4Target][c4DepIndex] != 0);
}

tCIDLib::TBoolean
TDependGraph::bDependsOn(const  tCIDLib::TCard4 c4ToCheck
                        , const tCIDLib::TCard4 c4DepIndex) const
{
    if (c4ToCheck >= m_c4CurElements)
    {
        stdOut << L"Illegal target index for dependency graph" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::IndexError;
    }

    if (c4DepIndex >= m_c4CurElements)
    {
        stdOut  << L"Illegal dependent index for dependency graph"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::IndexError;
    }
    return (m_ac1List[c4ToCheck][c4DepIndex] != 0);
}


tCIDLib::TBoolean TDependGraph::bElementExists(const TBldStr& strNewName) const
{
    tCIDLib::TCard4 c4Index = c4IndexOfElement(strNewName);
    return (c4Index != kCIDBuild::c4NotFound);
}


tCIDLib::TBoolean
TDependGraph::bHasDependencies(const TBldStr& strToCheck) const
{
    const tCIDLib::TCard4 c4Target = c4IndexOfElement(strToCheck);
    if (c4Target == kCIDBuild::c4NotFound)
    {
        stdOut  << L"Project '" << strToCheck
                << L"' was not found in dependency graph" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::NotFound;
    }
    return bCheckDependencies(c4Target);
}


tCIDLib::TBoolean
TDependGraph::bHasDependencies(const tCIDLib::TCard4 c4ToCheck) const
{
    if (c4ToCheck >= m_c4CurElements)
    {
        stdOut  << L"Illegal element index for dependency graph"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::IndexError;
    }
    return bCheckDependencies(c4ToCheck);
}


tCIDLib::TBoolean TDependGraph::bTestCircular() const
{
    //
    //  Do a nested loop and see any two elements point at each other,
    //  Being sure to check that they aren't the same of coures.
    //
    tCIDLib::TBoolean bFailed = kCIDLib::False;
    for (tCIDLib::TCard4 c4Outer = 0; c4Outer < m_c4CurElements; c4Outer++)
    {
        for (tCIDLib::TCard4 c4Inner = 0; c4Inner < m_c4CurElements; c4Inner++)
        {
            if (m_ac1List[c4Outer][c4Inner] && m_ac1List[c4Inner][c4Outer])
            {
                bFailed = kCIDLib::True;
                if (c4Outer == c4Inner)
                {
                    stdOut  << L"Project '" << m_astrNames[c4Outer]
                            << L"' depends upon itself" << kCIDBuild::EndLn;
                }
                 else
                {
                    stdOut  << L"Circular dependency between projects '"
                            << m_astrNames[c4Outer] << L"' and '"
                            << m_astrNames[c4Inner] << L"'" << kCIDBuild::EndLn;
                }
            }
        }
    }
    return bFailed;
}


tCIDLib::TBoolean
TDependGraph::bIterate( const   TProjIterator&          projiCallBack
                        , const TBldStr&                strStartProject
                        , const tCIDBuild::EIterModes   eMode) const
{
    //
    //  This is a recursive operation. We know that we don't have any
    //  cycles, so we can safely just kick off the operation. So lets
    //  look up the starting project first.
    //
    const tCIDLib::TCard4 c4At = c4IndexOfElement(strStartProject);
    if (c4At == kCIDBuild::c4NotFound)
    {
        stdOut  << L"Project '" << strStartProject
                << L"' was not found in dependency graph" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::NotFound;
    }

    //
    //  Create a 'done it' array which is used to check off projects done.
    //  It will only be used if in minimum mode.
    //
    tCIDLib::TCard1 abDone[c4MaxEntries];
    memset(abDone, 0, sizeof(abDone));

    //
    //  If the target is the implicit 0th project, then set the initial
    //  level to 0, so that the real top level that gets reported to the
    //  callbacks is 1. Otherwise, set it to one.
    //
    const tCIDLib::TCard4 c4InitLevel = c4At ? 1 : 0;

    //
    //  If the user wants to skip the top level target, then lets set
    //  up that. If the initial level is 0, then the target is the magic
    //  "all" project and the top level gets skipped anyway.
    //
    tCIDLib::TCard4 c4Skip = 0;
    if (bTestBits(eMode, tCIDBuild::EIterModes::SkipTarget) && (c4At != 0))
        c4Skip = c4At;

    // Get the minimal state from the mode
    const tCIDLib::TBoolean bMinimal
    (
        bTestBits(eMode, tCIDBuild::EIterModes::Minimal) ? kCIDLib::True : kCIDLib::False
    );

    // And now lets kick off the recursive operation
    if (bTestBits(eMode, tCIDBuild::EIterModes::BottomUp))
        return bIterateBottomUp(c4At, abDone, projiCallBack, c4InitLevel, bMinimal, c4Skip);

    return bIterateTopDown(c4At, abDone, projiCallBack, c4InitLevel, bMinimal, c4Skip);
}


tCIDLib::TVoid TDependGraph::Clear()
{
    // Zero out the list
    memset(m_ac1List, 0, sizeof(m_ac1List));

    //
    //  Create the special 'all' project which is at the top of the
    //  pyramid. Its not a real project, but is used to deal with all
    //  projects. By filling the 0th row, we make it dependent upon all
    //  other projects, but don't set [0][0] or it will appear to depend
    //  upon itself.
    //
    m_astrNames[0] = kCIDBuild::pszAllProjects;
    memset(&(m_ac1List[0][1]), 1, c4MaxEntries - 1);
    m_c4CurElements = 1;
}


const TBldStr& TDependGraph::strElementAt(const tCIDLib::TCard4 c4At) const
{
    if (c4At >= m_c4CurElements)
    {
        stdOut << L"Illegal element index for dependency graph" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::IndexError;
    }
    return m_astrNames[c4At];
}


tCIDLib::TCard4 TDependGraph::c4AddNewElement(const TBldStr& strNewName)
{
    //
    //  See if the element is already in the list. If so, then return a
    //  kCIDLib::False to indicate it failed.
    //
    tCIDLib::TCard4 c4Index = c4IndexOfElement(strNewName);
    if (c4Index != kCIDBuild::c4NotFound)
    {
        stdOut  << L"New element '" << strNewName
                << L"' already exists in dependency graph" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::AlreadyExists;
    }
    return c4AddElement(strNewName);
}



// ---------------------------------------------------------------------------
//  TDependGraph: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TDependGraph::bCheckDependencies(const tCIDLib::TCard4 c4At) const
{
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurElements; c4Index++)
    {
        if (m_ac1List[c4At][c4Index] != 0)
            return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean
TDependGraph::bIterateBottomUp( const   tCIDLib::TCard4         c4Cur
                                ,       tCIDLib::TCard1* const  pc1Done
                                , const TProjIterator&          projiCallBack
                                , const tCIDLib::TCard4         c4Level
                                , const tCIDLib::TBoolean       bMinimal
                                , const tCIDLib::TCard4         c4Skip) const
{
    // Mark the current project as done
    pc1Done[c4Cur] = 1;

    //
    //  Does the current project have any dependents? If so, then lets
    //  recurse on those before we do anything here.
    //
    if (bHasDependencies(c4Cur))
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurElements; c4Index++)
        {
            //
            //  If this one has already been done and we are doing a minimal
            //  iteration, then we just move on.
            //
            if (bMinimal && pc1Done[c4Index])
                continue;

            // If this one depends on the current one, then recurse
            if (bDependsOn(c4Cur, c4Index))
            {
                if (!bIterateBottomUp
                (
                    c4Index
                    , pc1Done
                    , projiCallBack
                    , c4Level+1
                    , bMinimal
                    , c4Skip))
                {
                    return kCIDLib::False;
                }
            }
        }
    }

    //
    //  Ok, now we can do the call out for this project, since any dependents
    //  have been dealt with.
    //
    //  Check for the special "All" top level project and don't send it since
    //  its not a real project.
    //
    if (!c4Cur)
        return kCIDLib::True;

    // Make sure its not the skip level
    if (c4Cur == c4Skip)
        return kCIDLib::True;

    return projiCallBack.bNextProject(m_astrNames[c4Cur], c4Level);
}


tCIDLib::TBoolean
TDependGraph::bIterateTopDown(  const   tCIDLib::TCard4         c4Cur
                                ,       tCIDLib::TCard1* const  pc1Done
                                , const TProjIterator&          projiCallBack
                                , const tCIDLib::TCard4         c4Level
                                , const tCIDLib::TBoolean       bMinimal
                                , const tCIDLib::TCard4         c4Skip) const
{
    // Mark the current project as done
    pc1Done[c4Cur] = 1;

    //
    //  If this is not the implicit 'all' project, then lets do the callback
    //  for this level before we do its children.
    //
    if (c4Cur && (c4Cur != c4Skip))
    {
        if (!projiCallBack.bNextProject(m_astrNames[c4Cur], c4Level))
            return kCIDLib::False;
    }

    //
    //  Does the current project have any dependents? If so, then lets
    //  recurse on those before we do anything here.
    //
    if (bHasDependencies(c4Cur))
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurElements; c4Index++)
        {
            //
            //  If this one has already been done and we are doing a minimal
            //  iteration, then we just move on.
            //
            if (bMinimal && pc1Done[c4Index])
                continue;

            // If this one depends on the current one, then recurse
            if (bDependsOn(c4Cur, c4Index))
            {
                if (!bIterateTopDown
                (
                    c4Index
                    , pc1Done
                    , projiCallBack
                    , c4Level+1
                    , bMinimal
                    , c4Skip))
                {
                    return kCIDLib::False;
                }
            }
        }
    }
    return kCIDLib::True;
}


tCIDLib::TCard4 TDependGraph::c4AddElement(const TBldStr& strNewName)
{
    // We assume that it does not already exist
    const tCIDLib::TCard4 c4Index = m_c4CurElements++;
    m_astrNames[c4Index] = strNewName;
    return c4Index;
}


tCIDLib::TCard4
TDependGraph::c4IndexOfElement(const TBldStr& strNewName) const
{
    // Search for the element
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurElements; c4Index++)
    {
        if (strNewName.bIEquals(m_astrNames[c4Index]))
        {
            // Just to make sure...
            if (c4Index >= m_c4CurElements)
            {
                stdOut  << L"Found a project at an illegal graph index"
                        << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::Internal;
            }
            return c4Index;
        }
    }
    return kCIDBuild::c4NotFound;
}

