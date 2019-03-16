//
// FILE NAME: CIDBuild_FindInfo.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/02/1998
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
//  This file implements the TFindInfo class, which represents the results
//  of a directory search.
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
//   CLASS: TFindInfo
//  PREFIX: fndi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFindInfo: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TFindInfo::bIsDirectory(const TBldStr& strToCheck)
{
    TFindInfo fndiToFill;
    if (!bFindAFile(strToCheck, fndiToFill))
        return kCIDLib::False;
    return fndiToFill.bIsDirectory();
}


tCIDLib::TBoolean
TFindInfo::bFindAFile(  const   TBldStr&                strDirectory
                        , const TBldStr&                strFile
                        ,       TFindInfo&              fndiToFill
                        , const tCIDBuild::EPathModes   eMode)
{
    // Just build the search spec and call the other version
    TBldStr strTmp(strDirectory);
    if (strTmp.chLast() != L'\\')
        strTmp.Append(L"\\");
    strTmp.Append(strFile);

    return bFindAFile(strTmp, fndiToFill, eMode);
}


tCIDLib::TCard4
TFindInfo::c4FindFiles( const   TBldStr&                strDirectory
                        , const TBldStr&                strFile
                        ,       TList<TFindInfo>&       listToFill
                        , const tCIDBuild::EPathModes   eMode)
{
    // Just build the search spec and call the other version
    TBldStr strTmp(strDirectory);
    if (strTmp.chLast() != L'\\')
        strTmp.Append(L"\\");
    strTmp.Append(strFile);

    return c4FindFiles(strTmp, listToFill, eMode);
}


// ---------------------------------------------------------------------------
//  TFindInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TFindInfo::TFindInfo() :

    m_bIsDir(kCIDLib::False)
    , m_tmLastWrite(0)
    , m_c4Size(0)
{
}

TFindInfo::TFindInfo(const  TBldStr&            strFileName
                    , const tCIDLib::TBoolean   bIsDir) :

    m_bIsDir(bIsDir)
    , m_strFileName(strFileName)
    , m_tmLastWrite(0)
    , m_c4Size(0)
{
}

TFindInfo::TFindInfo(const  TBldStr&            strFileName
                    , const tCIDLib::TBoolean   bIsDir
                    , const tCIDLib::TFloat8    tmLastWrite
                    , const tCIDLib::TCard4     ulSize) :

    m_bIsDir(bIsDir)
    , m_strFileName(strFileName)
    , m_tmLastWrite(tmLastWrite)
    , m_c4Size(ulSize)
{
}

TFindInfo::TFindInfo(const  tCIDLib::TVoid* const   pHostFindBuf
                    , const TBldStr&                strSearchPath
                    , const tCIDBuild::EPathModes   eMode)
{
    SetFromHostInfo(pHostFindBuf, strSearchPath, eMode);
}

TFindInfo::~TFindInfo()
{
}


// ---------------------------------------------------------------------------
//  TFindInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
#if 0
tCIDLib::TBoolean
TFindInfo::bUpdateInfo(const tCIDBuild::EPathModes eMode)
{
    //
    //  Get a copy of the file name field and use it in the call, because
    //  this call will attempt to reset it.
    //
    TBldStr strOldName(m_strFileName);

    // Now call our own static and return its return
    return TFindInfo::bFindAFile(strOldName, *this, eMode);
}
#endif

tCIDLib::TVoid TFindInfo::Set(  const   TBldStr&            strFileName
                                , const tCIDLib::TBoolean   bIsDir
                                , const tCIDLib::TFloat8    tmLastWrite
                                , const tCIDLib::TCard4     c4Size)
{
    m_bIsDir = bIsDir;
    m_strFileName = strFileName;
    m_tmLastWrite = tmLastWrite;
    m_c4Size = c4Size;
}

