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

namespace
{
    // ---------------------------------------------------------------------------
    //  Local types
    // ---------------------------------------------------------------------------
    struct TFileInfo
    {
        struct stat         StatBuf;
        tCIDBuild::TSCh*    pszName;
    };
}


// ---------------------------------------------------------------------------
//  TFindInfo: Public, static methods
// ---------------------------------------------------------------------------
tCIDBuild::TBoolean
TFindInfo::bFindAFile(  const   TBldStr&            strToFind
                        ,       TFindInfo&          fndiToFill
                        , const tCIDBuild::TBoolean bReturnFullPath)
{
    TFileInfo FileInfo;
    FileInfo.pszName = TRawStr::pszTranscode(strToFind.pszBuffer());
    TArrayJanitor<tCIDBuild::TNatCh> janToFind(FileInfo.pszName);

    if (::stat(FileInfo.pszName, &FileInfo.StatBuf))
        return false;

    fndiToFill.__SetFromHostInfo(&FileInfo, strToFind, bReturnFullPath);

    return true;
}


tCIDBuild::TUInt
TFindInfo::uiFindFiles( const   TBldStr&            strToFind
                        ,       TList<TFindInfo>&   listToFill
                        , const tCIDBuild::TBoolean bReturnFullPath)
{
    tCIDBuild::TNatCh* pszToFind = TRawStr::pszTranscode(strToFind.pszBuffer());
    TArrayJanitor<tCIDBuild::TNatCh> janToFind(pszToFind);

    glob_t GlobBuf;
    ::memset(&GlobBuf, 0, sizeof(GlobBuf));

    if (::glob(pszToFind, 0, 0, &GlobBuf))
        return 0;

    TFileInfo FileInfo;
    for (tCIDBuild::TInt i = 0; i < GlobBuf.gl_pathc; i++)
    {
        FileInfo.pszName = GlobBuf.gl_pathv[i];
        if (::stat(FileInfo.pszName, &FileInfo.StatBuf))
            continue;

        listToFill.Add(new TFindInfo(&FileInfo, pszToFind, bReturnFullPath));
    }

    ::globfree(&GlobBuf);

    return listToFill.uiElemCount();
}


// ---------------------------------------------------------------------------
//  TFindInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDBuild::TBoolean TFindInfo::bIsSpecialDirectory() const
{
    return ((__strFileName == NStr(".")) || (__strFileName == NStr("..")));
}



// ---------------------------------------------------------------------------
//  TFindInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDBuild::TVoid
TFindInfo::__SetFromHostInfo(   const   tCIDBuild::TVoid* const pHostFindBuf
                                , const TBldStr&                strSearchPath
                                , const tCIDBuild::TBoolean     bFullPath)
{
    const TFileInfo* pFileInfo = static_cast<const TFileInfo*>(pHostFindBuf);

    __uiSize = pFileInfo->StatBuf.st_size;

    //
    //  Now deal with the path. We optionally have to support returning
    //  just the base name or the full path to the found file.
    //
    tCIDBuild::TUInt uiName;
    tCIDBuild::TUInt uiExt;
    TBldStr strName(pFileInfo->pszName);
    TUtils::FindPathParts(strName, uiName, uiExt);
    strName.Cut(uiName);

    if (bFullPath)
    {
        __strFileName = strSearchPath;

        TUtils::FindPathParts(__strFileName, uiName, uiExt);

        if (uiName != kCIDBuild::uiNotFound)
            __strFileName.CapAt(uiName);

        if (__strFileName.uiLength() &&  __strFileName.chLast() != kCIDBuild::chPathSep)
            __strFileName.Append(kCIDBuild::pszPathSep);

        __strFileName.Append(strName);
    }
    else
    {
        __strFileName = strName;
    }

    //
    //  Convert the time into a floating point value. This is used so that
    //  any platform can represent its time easily in a common format.
    //
    __tmLastWrite = double(pFileInfo->StatBuf.st_mtime);
}
