//
// FILE NAME: CIDBuild_FindInfo_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/02/1998
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
//  This file implements the platform specific parts of the TFindInfo class.
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
#include    "..\CIDBuild.hpp"

#define     WIN32_LEAN_AND_MEAN
#include    <windows.h>


// ---------------------------------------------------------------------------
//  TFindInfo: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TFindInfo::bFindAFile(  const   TBldStr&                strToFind
                        ,       TFindInfo&              fndiToFill
                        , const tCIDBuild::EPathModes   eMode)
{
    WIN32_FIND_DATA FindInfo;
    tCIDLib::TVoid* hTmp = FindFirstFile(strToFind.pszBuffer(), &FindInfo);
    if (hTmp == INVALID_HANDLE_VALUE)
        return kCIDLib::False;

    // Go ahead and close the handle now, since we don't need it anymore
    FindClose(hTmp);

    // Let the find info object fill itself in with the info
    fndiToFill.SetFromHostInfo(&FindInfo, strToFind, eMode);

    return kCIDLib::True;
}


tCIDLib::TCard4
TFindInfo::c4FindFiles( const   TBldStr&                strToFind
                        ,       TList<TFindInfo>&       listToFill
                        , const tCIDBuild::EPathModes   eMode)
{
    WIN32_FIND_DATA FindInfo;
    tCIDLib::TVoid* hTmp = ::FindFirstFile(strToFind.pszBuffer(), &FindInfo);
    if (hTmp == INVALID_HANDLE_VALUE)
        return 0;

    // We got at least one, so find the rest
    while (kCIDLib::True)
    {
        // Add a new object to the list
        listToFill.Add(new TFindInfo(&FindInfo, strToFind, eMode));

        // Try to find another match
        if (!::FindNextFile(hTmp, &FindInfo))
            break;
    }

    // Close the find handle and return the number of files in the list
    FindClose(hTmp);
    return listToFill.c4ElemCount();
}


// ---------------------------------------------------------------------------
//  TFindInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TFindInfo::bIsSpecialDirectory() const
{
    return ((m_strFileName == L".") || (m_strFileName == L".."));
}


// ---------------------------------------------------------------------------
//  TFindInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TFindInfo::SetFromHostInfo( const   tCIDLib::TVoid* const   pHostFindBuf
                            , const TBldStr&                strSearchPath
                            , const tCIDBuild::EPathModes   eMode)
{
    // Cast the TVoid parameter to its real type
    const WIN32_FIND_DATA* pFindInfo
                    = reinterpret_cast<const WIN32_FIND_DATA*>(pHostFindBuf);

    //
    //  Copy out the file size. For portability reasons we only do the low
    //  32 bits, but we are dealing with source code here so a Cpp file over
    //  4 Gig is a little unlikely.
    //
    m_c4Size = pFindInfo->nFileSizeLow;

    // And handle the file name, taking into account the requested path mode
    if (eMode == tCIDBuild::EPathModes::Relative)
    {
        //
        //  In this case we take any relative path component in the wildcard
        //  spec, yank the name part off, and graft the actual found file name
        //  back onto it.
        //
        tCIDLib::TCard4 c4NameOfs;
        tCIDLib::TCard4 c4ExtOfs;
        m_strFileName = strSearchPath;
        TUtils::FindPathParts(m_strFileName, c4NameOfs, c4ExtOfs);

        //
        //  If the name offset is zero, then just copy over the name as is since
        //  there was no relative part, else take the real name in place of the
        //  wild card spec.
        //
        if (c4NameOfs)
        {
            m_strFileName.CapAt(c4NameOfs);
            m_strFileName.Append(pFindInfo->cFileName);
        }
         else
        {
            m_strFileName = pFindInfo->cFileName;
        }
    }
     else if (eMode == tCIDBuild::EPathModes::Full)
    {
        //
        //  We have a utility method to basically complete the path of the
        //  wildcard spec, cut the name part off, and graft the actual found
        //  file name onto it. Then just copy that back to the Win32 find
        //  buffer.
        //
        TUtils::CompletePath(strSearchPath, pFindInfo->cFileName, m_strFileName);
    }
     else
    {
        // Just the base name mode, so just copy the file name over
        m_strFileName = pFindInfo->cFileName;
    }

    //
    //  Convert the time into a floating point value. This is used so that
    //  any platform can represent its time easily in a common format.
    //
    m_tmLastWrite = double(pFindInfo->ftLastWriteTime.dwHighDateTime);
    m_tmLastWrite *= double(0xFFFFFFFF);
    m_tmLastWrite += double(pFindInfo->ftLastWriteTime.dwLowDateTime);

    if (pFindInfo->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        m_bIsDir = kCIDLib::True;
    else
        m_bIsDir = kCIDLib::False;
}

