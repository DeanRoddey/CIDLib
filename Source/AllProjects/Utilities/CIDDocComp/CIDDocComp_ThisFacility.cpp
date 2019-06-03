//
// FILE NAME: CIDDocComp_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/10/1997
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
//  This module implements the facility class for this program. This program's
//  facility class provides the centralized access to program functionality.
//  Everything goes through it, making it easy and convenient to coordinate
//  the program's work.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// -----------------------------------------------------------------------------
//  Include underlying headers.
// -----------------------------------------------------------------------------
#include    "CIDDocComp.hpp"


// ----------------------------------------------------------------------------
//  Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TFacCIDDocComp, TFacility)


// ---------------------------------------------------------------------------
//   CLASS: TFacCIDDocComp
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDDocComp: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDDocComp::TFacCIDDocComp() :

    TFacility
    (
        L"CIDDocComp"
        , tCIDLib::EModTypes::Exe
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}

TFacCIDDocComp::~TFacCIDDocComp()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDDocComp: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::EExitCodes
TFacCIDDocComp::eMainThread(TThread& thrThis, tCIDLib::TVoid* pData)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    if (m_ctxParse.bVerbose())
        strmOut() << L"CIDLib Documentation Compiler" << kCIDLib::NewEndLn;

    // We output into a directory under the result directory
    TPathStr pathTmp;
    if (!TProcEnvironment::bFind(L"CID_RESDIR", pathTmp))
    {
        strmOut() << L"   Could not find CIDLib result path" << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::BadEnvironment;
    }

    // Create the target path if it doesn't exist
    pathTmp.AddLevel(L"CIDDocs.Out");
    TFileSys::MakePath(pathTmp);

    // Store it away now that it's good
    m_strTarPath = pathTmp;


    // Create the top level source directory
    if (!TProcEnvironment::bFind(L"CID_SRCTREE", pathTmp))
    {
        strmOut() << L"   Could not find CIDLib Source path" << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::BadEnvironment;
    }
    pathTmp.AddLevel(L"Source");
    pathTmp.AddLevel(L"Docs");
    pathTmp.AddLevel(L"CIDDocs");

    // Make sure it exists
    if (!TFileSys::bIsDirectory(pathTmp))
    {
        strmOut()   << L"   Source path doesn't exists Path="
                    << pathTmp << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::BadParameters;
    }

    // Store it away now
    m_strSrcPath = pathTmp;

    try
    {
        // Tell the context object to load the DTD into his XML parser
        m_ctxParse.LoadDTD(m_strSrcPath);

        // We have to create a special case root topic to start the process.
        m_cptrRoot.SetPointer
        (
            new TTopic(L"Root", L"/", TString::strEmpty(), m_strSrcPath)
        );

        // And kick off the parse of the whole hierarchy
        m_cptrRoot->Parse(m_ctxParse);

        // If we got errors, then show them, else do the output
        if (m_ctxParse.bGotErrors())
        {
            strmOut() << L"\nErrors occurred during the parse:\n\n";
            m_ctxParse.ShowErrors(strmOut());
        }
         else
        {
            //
            //  Copy any files to the top level target directory that we just use
            //  as is. They will be in the top level source directory.
            //
            //  1.  The main HTML file which is hand done and includes the main
            //      header with menu and such. It includes a DIV into which all
            //      of the actual content is dynamically loaded.
            //  2.  The CSS file that contains the formatting info
            //  3.  Our own javascript file
            //  4.  The jquery javascript file
            //
            TPathStr pathCopySrc;
            TPathStr pathCopyTar;

            pathCopySrc = m_strSrcPath;
            pathCopySrc.AddLevel(L"CIDDocs.html");
            pathCopyTar = m_strTarPath;
            pathCopyTar.AddLevel(L"CIDDocs.html");
            TFileSys::CopyFile(pathCopySrc, pathCopyTar);

            pathCopySrc = m_strSrcPath;
            pathCopySrc.AddLevel(L"CIDDocs.css");
            pathCopyTar = m_strTarPath;
            pathCopyTar.AddLevel(L"CIDDocs.css");
            TFileSys::CopyFile(pathCopySrc, pathCopyTar);

            pathCopySrc = m_strSrcPath;
            pathCopySrc.AddLevel(L"CIDDocs.js");
            pathCopyTar = m_strTarPath;
            pathCopyTar.AddLevel(L"CIDDocs.js");
            TFileSys::CopyFile(pathCopySrc, pathCopyTar);

            // Copy over all of the images
            pathCopySrc = m_strSrcPath;
            pathCopySrc.AddLevel(L"Images");
            pathCopyTar = m_strTarPath;
            pathCopyTar.AddLevel(L"Images");
            if (!TFileSys::bIsDirectory(pathCopyTar))
                TFileSys::MakeSubDirectory(L"Images", m_strTarPath);
            CopyDir(pathCopySrc, pathCopyTar);

            //
            //  And now invoke the output generation on the root topic. For this one
            //  there is no parent, so pass an empty pointer. We also have to pass
            //  him our own pointer since it won't be available inside the method
            //  otherwise.
            //
            m_cptrRoot->GenerateOutput(m_strTarPath, TTopic::TTopicPtr(), m_cptrRoot);
        }

        // Now let's drop the root topic to clean everything up
        m_cptrRoot.DropRef();
    }

    catch(const TError& errToCatch)
    {
        strmOut()   << L"        An exception occurred. Error=\n"
                    << errToCatch<< kCIDLib::NewEndLn;
        strmOut().Flush();
        return tCIDLib::EExitCodes::RuntimeError;
    }

    strmOut() << kCIDLib::FlushIt;
    return tCIDLib::EExitCodes::Normal;
}


// ---------------------------------------------------------------------------
//  TFacCIDDocComp: Private, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TFacCIDDocComp::CopyDir(const TString& strSrc, const TString& strTar)
{
    TDirIter    diterSrc;
    TFindBuf    fndbCur;

    // Copy any files first
    TPathStr pathTar;
    TString  strNameExt;
    if (diterSrc.bFindFirst(strSrc, L"*.*", fndbCur, tCIDLib::EDirSearchFlags::NormalFiles))
    {
        do
        {
            // Build up the target path for this file
            fndbCur.pathFileName().bQueryNameExt(strNameExt);
            pathTar = strTar;
            pathTar.AddLevel(strNameExt);

            TFileSys::CopyFile(fndbCur.pathFileName(), pathTar);
        }   while (diterSrc.bFindNext(fndbCur));
    }

    // And now do any child directories
    if (diterSrc.bFindFirst(strSrc, L"*", fndbCur, tCIDLib::EDirSearchFlags::NormalDirs))
    {
        do
        {
            //
            //  Build up the target path for this file. And make sure it
            //  exists.
            //
            fndbCur.pathFileName().bQueryNameExt(strNameExt);
            pathTar = strTar;
            pathTar.AddLevel(strNameExt);

            if (!TFileSys::bIsDirectory(pathTar))
                TFileSys::MakeSubDirectory(strNameExt, strTar);

            CopyDir(fndbCur.pathFileName(), pathTar);

        }   while (diterSrc.bFindNext(fndbCur));
    }
}


