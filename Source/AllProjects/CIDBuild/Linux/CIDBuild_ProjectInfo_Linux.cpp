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
//  This method is called to build up the names of any project relative
//  file names that might be platform specific.
//
tCIDLib::TVoid TProjectInfo::BuildOutputFileName()
{
    m_strOutBin = facCIDBuild.strOutDir();
    if (m_eType == tCIDBuild::EProjTypes::FileCopy)
    {
        //
        //  Nothing to do for this one. If there is any m_strCopyOutDir
        //  set, it'll get added to the output bin path to create the full
        //  output path for file copies.
        //
    }
     else
    {
        if (m_eType == tCIDBuild::EProjTypes::SharedLib)
        {
            m_strOutBin.Append(L"lib");
            m_strOutBin.Append(m_strProjectName);
            m_strOutBin.Append(kCIDBuild::pszDllExt);
            m_strOutBin.Append(facCIDBuild.strVersionSuffix());
        }
         else if (m_eType == tCIDBuild::EProjTypes::StaticLib)
        {
            m_strOutBin.Append(L".a");
        }
         else if ((m_eType == tCIDBuild::EProjTypes::Executable)
              ||  (m_eType == tCIDBuild::EProjTypes::Service))
        {
            // No extension for executables
            m_strOutBin.Append(m_strProjectName);
        }
         else
        {
            // This is bad, we don't know
            stdOut  << L"Unknown project type, can't set extension" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::Internal;
        }
    }
}
