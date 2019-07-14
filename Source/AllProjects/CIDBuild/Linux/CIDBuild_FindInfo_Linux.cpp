//
// FILE NAME: CIDBuild_FindInfo_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 02/23/1999
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
#include    "../CIDBuild.hpp"
#include    <sys/stat.h>
#include    <glob.h>
#include    <stdio.h>
#include    <string.h>


namespace
{
    // ---------------------------------------------------------------------------
    //  Local types
    // ---------------------------------------------------------------------------
    struct TFileInfo
    {
        struct stat     StatBuf;
        tCIDLib::TSCh*  pszName;
    };
}


// ---------------------------------------------------------------------------
//  TFindInfo: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TFindInfo::bFindAFile(  const   TBldStr&                strToFind
                        ,       TFindInfo&              fndiToFill
                        , const tCIDBuild::EPathModes   eMode)
{
    TFileInfo FileInfo;
    FileInfo.pszName = TRawStr::pszTranscode(strToFind.pszBuffer());
    TArrayJanitor<tCIDLib::TSCh> janToFind(FileInfo.pszName);

    if (::stat(FileInfo.pszName, &FileInfo.StatBuf))
        return false;

    fndiToFill.SetFromHostInfo(&FileInfo, strToFind, eMode);

    return true;
}


tCIDLib::TCard4
TFindInfo::c4FindFiles( const   TBldStr&                strToFind
                        ,       TList<TFindInfo>&       listToFill
                        , const tCIDBuild::EPathModes   eMode)
{
    tCIDLib::TSCh* pszToFind = TRawStr::pszTranscode(strToFind.pszBuffer());
    TArrayJanitor<tCIDLib::TSCh> janToFind(pszToFind);

    glob_t GlobBuf;
    ::memset(&GlobBuf, 0, sizeof(GlobBuf));

    if (::glob(pszToFind, 0, 0, &GlobBuf))
        return 0;

    TFileInfo FileInfo;
    for (tCIDLib::TSInt i = 0; i < GlobBuf.gl_pathc; i++)
    {
        FileInfo.pszName = GlobBuf.gl_pathv[i];
        if (::stat(FileInfo.pszName, &FileInfo.StatBuf))
            continue;

        listToFill.Add
        (
            new TFindInfo(&FileInfo, TBldStr(FileInfo.pszName), eMode)
        );
    }

    ::globfree(&GlobBuf);

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
    const TFileInfo* pFileInfo = static_cast<const TFileInfo*>(pHostFindBuf);

    m_c4Size = pFileInfo->StatBuf.st_size;

    tCIDLib::TCard4 c4Name;
    tCIDLib::TCard4 c4Ext;
    TBldStr strName(pFileInfo->pszName);
    TUtils::FindPathParts(strName, c4Name, c4Ext);
    strName.Cut(c4Name);

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
            m_strFileName.Append(strName);
        }
         else
        {
            m_strFileName = pFileInfo->pszName;
        }
    }
     else if (eMode == tCIDBuild::EPathModes::Full)
    {
        // We have a utility helper for this
        TUtils::CompletePath(strSearchPath, m_strFileName);        
    }
     else
    {
        // They just want the name part
        m_strFileName = strName;
    }

    //
    //  Convert the time into a floating point value. This is used so that
    //  any platform can represent its time easily in a common format.
    //
    m_tmLastWrite = double(pFileInfo->StatBuf.st_mtime);
}
