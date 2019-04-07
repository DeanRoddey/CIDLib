//
// FILE NAME: CIDBuild_ProjectInfo_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 01/24/2000
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
//  This file provides the Linux specific parts of the TProjectInfo class.
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
#include    "../CIDBuild.hpp"


// ---------------------------------------------------------------------------
//  TProjectInfo: Private platform methods
// ---------------------------------------------------------------------------


//
//  This method is called to build up the names of the files output from a
//  message file compilation for a particular project. Some platforms need
//  to do slightly different things here. The output files are the binary
//  CIDMsg file, the Exe or Lib/DLL file, and the error/message id headers.
//  The latter two are output to the project directory, while the former two
//  are output to the output path and might be versioned.
//
tCIDBuild::TVoid
TProjectInfo::__BuildOutputFileNames(const  TBldStr&        strOutPath
                                    , const TBldStr&        strVerSuffix
                                    , const TBldStr&        strLangSuffix
                                    ,       TBldStr&        strMsgFile
                                    ,       TBldStr&        strErrHpp
                                    ,       TBldStr&        strMsgHpp
                                    ,       TBldStr&        strBin)
{
    // Build the name to the error ids header
    strErrHpp = __strProjectDir;
    strErrHpp.Append(__strProjectName);
    strErrHpp.Append(L"_ErrorIds.hpp");

    // Build the name to the message ids header
    strMsgHpp = __strProjectDir;
    strMsgHpp.Append(__strProjectName);
    strMsgHpp.Append(L"_MessageIds.hpp");

    //
    //  Build up the output binary message file name. If this project
    //  is versioned, then append the version postfix.
    //
    strMsgFile = strOutPath;
    if (__eType == tCIDBuild::EProjTypes::Dll)
        strMsgFile.Append(L"lib");
    strMsgFile.Append(__strProjectName);
    if (__eType == tCIDBuild::EProjTypes::Dll)
        strMsgFile.Append(kCIDBuild::pszDllExt);
    if (__bVersioned)
        strMsgFile.Append(strVerSuffix);
    strMsgFile.Append(L"_");
    strMsgFile.Append(strLangSuffix);
    strMsgFile.Append(L".CIDMsg");

    // Build up the name of the DLL/Exe file
    strBin = strOutPath;
    if (__eType == tCIDBuild::EProjTypes::Dll)
    {
        strBin.Append(L"lib");
        strBin.Append(__strProjectName);
        strBin.Append(kCIDBuild::pszDllExt);
        strBin.Append(strVerSuffix);
    }
     else
    {
        strBin.Append(__strProjectName);
        if (__bVersioned)
            strBin.Append(strVerSuffix);
        strBin.Append(kCIDBuild::pszExeExt);
    }
}
