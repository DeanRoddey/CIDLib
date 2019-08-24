//
// FILE NAME: CIDBuild_ProjectList.Cpp
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
    strProjFile.Append(L"Source", kCIDBuild::pszPathSep);
    strProjFile.Append(L"AllProjects", kCIDBuild::pszPathSep);
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
    TLineSpooler        lsplSource(strProjFile);
    TBldStr             strReadBuf;
    TBldStr             strProjName;
    tCIDBuild::TStrList listPlatfExcl;
    tCIDBuild::TStrList listPlatfIncl;
    while (kCIDLib::True)
    {
        // Get the next line. If at the end of file, then we are done
        if (!lsplSource.bReadLine(strReadBuf))
            break;

        if (strReadBuf.bStartsWith(L"PROJECT="))
        {
            // Cut off the prefix to leave the name and possibly platform incl/excl lists
            strReadBuf.Cut(8);
            strReadBuf.StripWhitespace();

            // Parse this initial line info
            ParseInitProjLine
            (
                strReadBuf, lsplSource.c4CurLine(), strProjName, listPlatfIncl, listPlatfExcl
            );

            //
            //  Create a new project info object and let it parse it's name and
            //  and include/exclude info if present.
            //
            TProjectInfo* pprojiCur = new TProjectInfo(strProjName, listPlatfIncl, listPlatfExcl);

            //
            //  If it doesn't support the current platform, then we just eat lines until
            //  the end of this project, then we can delete it and move on.
            //
            if (!pprojiCur->bSupportsThisPlatform())
            {
                // Remember the project name and delete this project
                const TBldStr strProjName = pprojiCur->strProjectName();
                delete pprojiCur;

                // Eat the rest of this project's content
                tCIDLib::TBoolean bGotEnd = kCIDLib::False;
                while (lsplSource.bReadLine(strReadBuf))
                {
                    if (strReadBuf.bIEquals(L"END PROJECT"))
                    {
                        bGotEnd = kCIDLib::True;
                        break;
                    }
                }

                if (!bGotEnd)
                {
                    stdOut  << L"(Line " << lsplSource.c4CurLine()
                            << L") Never found end of unsupported project ("
                            << strProjName << L")" << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::UnexpectedEOF;
                }
            }
             else
            {
                // This one is a keeper so ask it to parse out the rest of its content
                pprojiCur->ParseContent(lsplSource);

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


//
//  We get a line like:
//
//      projname [incl1 incl2, excl1 excl2]
//
//  So the project name and an optional list of platforms to include, followed by an optional
//  list of platforms to exclude. Each of the lists are space separated tokens.
//
tCIDLib::TVoid
TProjectList::ParseInitProjLine(const   TBldStr&                strLine
                                , const tCIDLib::TCard4         c4Line
                                ,       TBldStr&                strName
                                ,       tCIDBuild::TStrList&    listProjIncl
                                ,       tCIDBuild::TStrList&    listProjExcl)
{
    listProjIncl.RemoveAll();
    listProjExcl.RemoveAll();

    // Tokenize the line on our known separators. If we get none, then obviously bad
    tCIDBuild::TStrList listTokens;
    if (!TUtils::bTokenize(strLine, L"[,]", TBldStr(), listTokens))
    {
        stdOut  << L"(Line " << c4Line
                << L") At least a project name is required here" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::FileFormat;
    }

    tCIDBuild::TStrList::TCursor cursTokens(&listTokens);
    if (!cursTokens.bResetIter())
    {
        stdOut  << L"(Line " << c4Line
                << L") No tokens available even though some were reported" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::Internal;
    }

    // The first one has to be the project name
    strName = cursTokens.tCurElement();
    if (!TRawStr::bIsAlpha(strName.chFirst()))
    {
        stdOut  << L"(Line " << c4Line
                << L") Project names must start with an alpha character" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::FileFormat;
    }

    if (cursTokens.bNext())
    {
        // This has to be the open bracket
        if (cursTokens.tCurElement() != L"[")
        {
            stdOut  << L"(Line " << c4Line
                    << L") Expected open paren or end of line" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::Internal;
        }

        // Call the utils helper that parses the lists from a cursor of tokens
        TBldStr strError;
        if (!TUtils::bParseInclExclLists(cursTokens, listProjIncl, listProjExcl, strError))
        {
            stdOut  << L"(Line " << c4Line << L") " << strError << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }
    }
}
