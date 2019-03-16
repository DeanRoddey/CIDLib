//
// FILE NAME: CIDMacroEng_FileResolver.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/15/2003
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
//  This file implements the little bit of out of line stuff for the file
//  resolver abstract mixin class, and a simple implementation that does
//  resolution via simple base path.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDMacroEng_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TMEngFixedBaseFileResolver,TObject)


// ---------------------------------------------------------------------------
//  CLASS: MMEngFileResolver
// PREFIX: meres
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  MMEngFileResolver: Constructors and Destructor
// ---------------------------------------------------------------------------
MMEngFileResolver::~MMEngFileResolver()
{
}


// ---------------------------------------------------------------------------
//  MMEngFileResolver: Hidden constructors
// ---------------------------------------------------------------------------
MMEngFileResolver::MMEngFileResolver()
{
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngFixedBaseFileResolver
// PREFIX: meres
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngFixedBaseFileResolver: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngFixedBaseFileResolver::TMEngFixedBaseFileResolver()
{
}

TMEngFixedBaseFileResolver::TMEngFixedBaseFileResolver(const TString& strBasePath) :

    m_strBasePath(strBasePath)
{
}

TMEngFixedBaseFileResolver::~TMEngFixedBaseFileResolver()
{
}


// ---------------------------------------------------------------------------
//  TMEngFixedBaseFileResolver: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TString& TMEngFixedBaseFileResolver::strBasePath() const
{
    return m_strBasePath;
}

const TString& TMEngFixedBaseFileResolver::strBasePath(const TString& strToSet)
{
    m_strBasePath = strToSet;
    return m_strBasePath;
}


// ---------------------------------------------------------------------------
//  TMEngFixedBaseFileResolver: Private, inherited methods
// ---------------------------------------------------------------------------

//
//  Contract an OS level path to a macro engine level path. We make sure it
//  starts with the base path and then remove it.
//
tCIDLib::TVoid
TMEngFixedBaseFileResolver::ContractPath(const  TString&    strOSFilePath
                                        ,       TPathStr&   pathToFill)
{
    // It must start with the base path
    if (!strOSFilePath.bStartsWithI(m_strBasePath))
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_CantConvertPath
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , strOSFilePath
        );
    }

    //
    //  According to whether the base path ends with a slash or not, we
    //  have to copy from different index.
    //
    tCIDLib::TCard4 c4Index = m_strBasePath.c4Length();
    if (m_strBasePath.chLast() == L'\\')
        c4Index--;

    pathToFill.CopyInSubStr(strOSFilePath, c4Index);
}


//
//  Expand a macro level path to an OS level path. We just append it to the
//  base path.
//
tCIDLib::TVoid
TMEngFixedBaseFileResolver::ExpandPath( const   TString&    strMacroFilePath
                                        ,       TPathStr&   pathToFill)
{
    TPathStr pathTmp = m_strBasePath;
    pathTmp.AddLevel(strMacroFilePath);

    // Normalize the path
    TFileSys::NormalizePath(pathTmp, pathToFill);

    // It must start with the base path
    if (!pathToFill.bStartsWithI(m_strBasePath))
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_BadExpPath
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , strMacroFilePath
        );
    }
}

