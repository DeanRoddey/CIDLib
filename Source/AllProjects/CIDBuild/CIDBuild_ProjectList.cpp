//
// FILE NAME: CIDBuild_ProjectList.Cpp
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
//  This file implements the TProjectList class, which manages the list of
//  projects, their settings, their dependencies, etc...
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



// ---------------------------------------------------------------------------
//   CLASS: TProjectList
//  PREFIX: plist
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TProjectList: Constructors and Destructor
// ---------------------------------------------------------------------------
TProjectList::TProjectList()
{
}

TProjectList::~TProjectList()
{
}


// ---------------------------------------------------------------------------
//  TProjectList: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  A quick check to see if one project depends on another. They can indicate
//  if it must exist, or if not existing just returns false.
//
tCIDLib::TBoolean
TProjectList::bDependsOn(const  TBldStr&            strSrcProj
                        , const TBldStr&            strToCheck
                        , const tCIDLib::TBoolean   bMustExist) const
{
    // Just pass it on to the dependenency graphi
    return m_depgList.bDependsOn(strSrcProj, strToCheck, bMustExist);
}


tCIDLib::TBoolean TProjectList::bProjectExists(const TBldStr& strToFind) const
{
    const TProjectInfo* const pprojiTarget = pprojiFindProject(strToFind);
    return (pprojiTarget != 0);
}


tCIDLib::TVoid
TProjectList::DumpProjectSettings(const TBldStr& strTarget) const
{
    // Find the project and ask it to dump its settings
    const TProjectInfo* const pprojiTarget = pprojiFindProject(strTarget);
    if (!pprojiTarget)
    {
        stdOut  << L"Project '" << strTarget
                << L"' was not found" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::NotFound;
    }
    pprojiTarget->DumpSettings();
}


tCIDLib::TVoid TProjectList::LoadFileLists()
{
    //
    //  Just pass this request on to each of the projects in our list. The
    //  order does not matter here so we just do a straight iteration of
    //  the list.
    //
    TList<TProjectInfo>::TCursor cursProjs(&m_listProjects);

    // If no entries, that's wierd, but nothing to do
    if (!cursProjs.bResetIter())
        return;

    do
    {
        cursProjs.tCurElement().LoadFileLists();
    }   while (cursProjs.bNext());
}


const TProjectInfo& TProjectList::projiByName(const TBldStr& strToFind) const
{
    // Look up the project name and return its project info object
    const TProjectInfo* const pprojiTarget = pprojiFindProject(strToFind);
    if (!pprojiTarget)
    {
        stdOut  << L"Project '" << strToFind
                << L"' was not found" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::NotFound;
    }
    return *pprojiTarget;
}


TProjectInfo& TProjectList::projiByName(const TBldStr& strToFind)
{
    // Look up the project name and return its project info object
    TProjectInfo* const pprojiTarget = pprojiFindProject(strToFind);
    if (!pprojiTarget)
    {
        stdOut  << L"Project '" << strToFind
                << L"' was not found" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::NotFound;
    }
    return *pprojiTarget;
}


tCIDLib::TVoid TProjectList::ParseProjectFile()
{
    // Build up the path to the project file
    TBldStr strProjFile(facCIDBuild.strRootDir());
    strProjFile.Append(L"Source", L"\\");
    strProjFile.Append(L"AllProjects", L"\\");
    strProjFile.Append(L"CIDBuild.Projects");

    // Make sure it exists
    if (!TUtils::bExists(strProjFile))
    {
        stdOut  << L"Project file " << strProjFile
                << L" was not found" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::FileNotFound;
    }

    if (facCIDBuild.bVerbose())
    {
        stdOut  << L"\n----------------------------------------------\n"
                << L"Parsing project file: " << strProjFile << L"\n"
                << L"----------------------------------------------"
                << kCIDBuild::EndLn;
    }

    //
    //  Lets create a line spooler for the project file and go into the
    //  line reading loop. The file format is all line oriented so we
    //  don't need any fancy lexing here.
    //
    TLineSpooler    lsplSource(strProjFile);
    TBldStr         strReadBuf;
    while (kCIDLib::True)
    {
        // Get the next line. If at the end of file, then we are done
        if (!lsplSource.bReadLine(strReadBuf))
            break;

        if (strReadBuf.bStartsWith(L"PROJECT="))
        {
            // Pull out the name, which should be the rest of the line
            strReadBuf.Cut(8);
            strReadBuf.StripWhitespace();

            // Create a new project info object and let it parse itself
            TProjectInfo* pprojiCur = new TProjectInfo(strReadBuf);

            if (facCIDBuild.bVerbose())
                stdOut << L"    Parsing project: " << strReadBuf << kCIDBuild::EndLn;

            // Ask the object to parse out its info
            pprojiCur->ParseContent(lsplSource);

            //
            //  See if this project is targeted towards the current platform.
            //  It will say it is if either the current platform was explicitly
            //  given, or if no explicit platform support was given. We'll check
            //  the base platform and the full one, it can match either for
            //  more or less filtering.
            //
            if (!pprojiCur->bSupportsPlatform(facCIDBuild.strBasePlatform())
            &&  !pprojiCur->bSupportsPlatform(facCIDBuild.strFullPlatform()))
            {
                delete pprojiCur;
                continue;
            }

            //
            //  Let this new project add him/herself to the dependency graph.
            //  This will catch any duplicate names. He will cache the index
            //  at which he was added to the graph, which will much speed up
            //  later work.
            //
            pprojiCur->AddToDepGraph(m_depgList);

            // And then add this new project to our list of projects
            m_listProjects.Add(pprojiCur);
        }
         else if (strReadBuf == L"ALLPROJECTS=")
        {
            //
            //  The settings in this block are system wide, i.e. they apply
            //  to all projects.
            //
            facCIDBuild.ParseAllProjects(lsplSource);
        }
         else
        {
            // Dunno what this is
            stdOut  << L"(Line " << lsplSource.c4CurLine()
                    << L") Expected ALLPROJECTS=, or PROJECT=XXX"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }
    }

    //
    //  Ok, so now lets feed all of the dependencies of each project into
    //  the dependency graph. This will set up all of the info required to
    //  do the actions in the correct order of dependency.
    //
    //  Note that we added all of the projects themselves into the graph
    //  during parsing above, so here we just need to add their dependencies
    //  to the graph. They each have already cached their offset into the
    //  graph, so we can use that index for speed.
    //
    TList<TProjectInfo>::TCursor cursProjs(&m_listProjects);

    if (cursProjs.bResetIter())
    {
        do
        {
            // Get a quick ref to the next project info object
            const TProjectInfo& projiCur = cursProjs.tCurElement();

            // Do the regular dependencies
            TList<TBldStr>::TCursor cursDeps(&projiCur.listDeps());
            if (cursDeps.bResetIter())
            {
                do
                {
                    //
                    //  Add a dependency for this new dependent. Note that we
                    //  use the cached index into the dependency graph instead
                    //  of doing an insert by project name.
                    //
                    m_depgList.AddDependency
                    (
                        projiCur.c4DepIndex()
                        , cursDeps.tCurElement()
                    );
                }   while (cursDeps.bNext());
            }
        }   while (cursProjs.bNext());
    }

    // And ask the dependency graph to check for circular depenendies
    if (m_depgList.bTestCircular())
    {
        stdOut  << L"Circular dependencies not allowed in the project list"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::DependError;
    }
}


tCIDLib::TVoid TProjectList::RemoveAll()
{
    // Just flush our project info list and clear the dependency graph
    m_listProjects.RemoveAll();
    m_depgList.Clear();
}



// ---------------------------------------------------------------------------
//  TProjectList: Private, non-virtual methods
// ---------------------------------------------------------------------------
const TProjectInfo*
TProjectList::pprojiFindProject(const TBldStr& strName) const
{
    // Get a cursor for the project list
    TList<TProjectInfo>::TCursor cursProjs(&m_listProjects);

    // If no entries, that's wierd but nothing to do
    if (!cursProjs.bResetIter())
        return 0;

    do
    {
        // If the project name matches the passed name, this is it
        if (cursProjs.tCurElement().strProjectName().bIEquals(strName))
            return &cursProjs.tCurElement();
    }   while (cursProjs.bNext());
    return 0;
}

TProjectInfo* TProjectList::pprojiFindProject(const TBldStr& strName)
{
    // Get a cursor for the project list
    TList<TProjectInfo>::TCursor cursProjs(&m_listProjects);

    // If no entries, that's wierd but nothing to do
    if (!cursProjs.bResetIter())
        return 0;

    do
    {
        // If the project name matches the passed name, this is it
        if (cursProjs.tCurElement().strProjectName().bIEquals(strName))
            return &cursProjs.tCurElement();
    }   while (cursProjs.bNext());
    return 0;
}
