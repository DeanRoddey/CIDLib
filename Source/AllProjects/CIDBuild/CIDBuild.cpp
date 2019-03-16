//
// FILE NAME: CIDBuild.Cpp
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
//  This is the main module for the CIDBuild.Exe utility program. This is
//  the portable build framework for the system. It is a simple, straight
//  C++ program that drives the whole build process. Each platform provides
//  its own version of the TBuildDriver class, and arranges to have that
//  one built and linked in on its platform. That driver knows how to invoke
//  all of the tools required to do the builds on that platform.
//
//  Each facility has a text format .Project file with the build settings
//  for that facility (expressed in a simple, platform independent format.)
//  Plus there is some global stuff that comes in on the command line which
//  affects everyone (such as Prod vs. Dev build, ect...) All of that stuff
//  is used by the build driver class to know how to build up the command
//  lines for each tool required.
//
//  The main infrastructure here just creates a facility object and calls
//  its main entry point, passing along the arguments.
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
//  Global data
// ---------------------------------------------------------------------------
TFacCIDBuild    facCIDBuild;
TTextFile       stdOut(tCIDLib::EStdFiles::StdOut);



// ---------------------------------------------------------------------------
//  Program entry point
// ---------------------------------------------------------------------------

//
//  This guy just processes the command line parms
//
//  RETURN: 0 if successful, else 1.
//
#if defined(CIDBUILD_SHORTMAIN)
tCIDLib::TSInt main(tCIDLib::SIn4 iArgs, tCIDLib::TSCh** apszArgs)
{
    //
    //  We are all Unicode internally, so we have to create a transcoded
    //  set of parameters to pass in.
    //
    tCIDLib::TCh** apszIntArgs = new tCIDLib::TCh*[iArgs];
    for (tCIDLib::TSInt iIndex = 0; iIndex < iArgs; iIndex++)
        apszIntArgs[iIndex] = TRawStr::pszTranscode(apszArgs[iIndex]);
    return facCIDBuild.iMain(iArgs, apszIntArgs);
}
#else
extern "C" tCIDLib::TSInt wmain(tCIDLib::TSInt iArgs, tCIDLib::TCh**apszArgs)
{
    return facCIDBuild.iMain(iArgs, apszArgs);
}
#endif



// ---------------------------------------------------------------------------
//  Global insertion operators
// ---------------------------------------------------------------------------
TTextFile& operator<<(TTextFile& strmOut, const tCIDBuild::EActions eAction)
{
    if (eAction == tCIDBuild::EActions::Build)
        strmOut << L"Build";
    else if (eAction == tCIDBuild::EActions::MakeDeps)
        strmOut << L"Dependencies";
    else if (eAction == tCIDBuild::EActions::ShowProjDeps)
        strmOut << L"Show Project Dependencies";
    else if (eAction == tCIDBuild::EActions::ShowProjSettings)
        strmOut << L"Show Project Settings";
    else if (eAction == tCIDBuild::EActions::CopyHeaders)
        strmOut << L"Copy Headers";
    else if (eAction == tCIDBuild::EActions::MakeRes)
        strmOut << L"Make Resources";
    else if (eAction == tCIDBuild::EActions::MakeBinRelease)
        strmOut << L"Make Binary Release";
    else if (eAction == tCIDBuild::EActions::MakeDevRelease)
        strmOut << L"Make Dev Release";
    else if (eAction == tCIDBuild::EActions::IDLGen)
        strmOut << L"IDL Generation";
    else if (eAction == tCIDBuild::EActions::Bootstrap)
        strmOut << L"Bootstrap Build";
    else
        strmOut << L"[Unknown action value]";

    return strmOut;
}

TTextFile& operator<<(TTextFile& strmOut, const tCIDBuild::EErrors eError)
{
    if (eError == tCIDBuild::EErrors::BadParams)
        strmOut << L"Bad Parameters";
    else if (eError == tCIDBuild::EErrors::CopyFailed)
        strmOut << L"Copy Failed";
    else if (eError == tCIDBuild::EErrors::FileNotFound)
        strmOut << L"File Not Found";
    else if (eError == tCIDBuild::EErrors::FileFormat)
        strmOut << L"File Format";
    else if (eError == tCIDBuild::EErrors::UnexpectedEOF)
        strmOut << L"Unexpected EOF";
    else if (eError == tCIDBuild::EErrors::Internal)
        strmOut << L"Internal";
    else if (eError == tCIDBuild::EErrors::NotFound)
        strmOut << L"Not Found";
    else if (eError == tCIDBuild::EErrors::AlreadyExists)
        strmOut << L"Already Exists";
    else if (eError == tCIDBuild::EErrors::IndexError)
        strmOut << L"Index Error";
    else if (eError == tCIDBuild::EErrors::Full)
        strmOut << L"Full";
    else if (eError == tCIDBuild::EErrors::NotImplemented)
        strmOut << L"Not Implemented";
    else if (eError == tCIDBuild::EErrors::DependError)
        strmOut << L"Depend Error";
    else if (eError == tCIDBuild::EErrors::CreateError)
        strmOut << L"Create Error";
    else if (eError == tCIDBuild::EErrors::BuildError)
        strmOut << L"Build Error";
    else if (eError == tCIDBuild::EErrors::NotSupported)
        strmOut << L"Not Supported";
    else if (eError == tCIDBuild::EErrors::WriteError)
        strmOut << L"Write Error";
    else if (eError == tCIDBuild::EErrors::ReadError)
        strmOut << L"Read Errror";
    else if (eError == tCIDBuild::EErrors::OpenError)
        strmOut << L"Open Errror";
    else if (eError == tCIDBuild::EErrors::QueryError)
        strmOut << L"Query Errror";
    else if (eError == tCIDBuild::EErrors::SeekError)
        strmOut << L"Seek Errror";
    else if (eError == tCIDBuild::EErrors::QueryCurDir)
        strmOut << L"Get Current Directory";
    else if (eError == tCIDBuild::EErrors::MakeTmpFile)
        strmOut << L"Make temp file name";
    else if (eError == tCIDBuild::EErrors::NotADir)
        strmOut << L"Indicated path was not a directory";
    else
        strmOut << L"[Unknown Error]";

    return strmOut;
}

TTextFile& operator<<(TTextFile& strmOut, const tCIDBuild::EBldModes eMode)
{
    if (eMode == tCIDBuild::EBldModes::Develop)
        strmOut << L"Develop";
    else if (eMode == tCIDBuild::EBldModes::Production)
        strmOut << L"Production";
    else
        strmOut << L"[Unknown build mode value]";

    return strmOut;
}

TTextFile& operator<<(TTextFile& strmOut, const tCIDBuild::EProjTypes eType)
{
    if (eType == tCIDBuild::EProjTypes::SharedObj)
        strmOut << L"SharedObj/DLL";
    else if (eType == tCIDBuild::EProjTypes::SharedLib)
        strmOut << L"SharedLib/DLL";
    else if (eType == tCIDBuild::EProjTypes::StaticLib)
        strmOut << L"StaticLib";
    else if (eType == tCIDBuild::EProjTypes::Executable)
        strmOut << L"Executable";
    else if (eType == tCIDBuild::EProjTypes::Service)
        strmOut << L"Service";
    else if (eType == tCIDBuild::EProjTypes::FileCopy)
        strmOut << L"FileCopy";
    else if (eType == tCIDBuild::EProjTypes::Group)
        strmOut << L"Group";
    else
        strmOut << L"[Unknown project type value]";
    return strmOut;
}

TTextFile& operator<<(TTextFile& strmOut, const tCIDBuild::ERTLModes eMode)
{
    if (eMode == tCIDBuild::ERTLModes::SingleStatic)
        strmOut << L"Single/Static";
    else if (eMode == tCIDBuild::ERTLModes::SingleDynamic)
        strmOut << L"Single/Dynamic";
    else if (eMode == tCIDBuild::ERTLModes::MultiStatic)
        strmOut << L"Multi/Static";
    else if (eMode == tCIDBuild::ERTLModes::MultiDynamic)
        strmOut << L"Multi/Dynamic";
    else
        strmOut << L"[Unknown RTL mode value]";

    return strmOut;
}

