//
// FILE NAME: CIDMacroEng_ClassManager.cpp
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
//  This file implements the little bit of out of line stuff for the path
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
RTTIDecls(TMEngFixedBaseClassMgr,TObject)


// ---------------------------------------------------------------------------
//  CLASS: MMEngClassMgr
// PREFIX: mecm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  MMEngClassMgr: Constructors and Destructor
// ---------------------------------------------------------------------------
MMEngClassMgr::~MMEngClassMgr()
{
}


// ---------------------------------------------------------------------------
//  MMEngClassMgr: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean MMEngClassMgr::bMacroExists(const TString& strToCheck)
{
    return bCheckIfExists(strToCheck);
}


tCIDLib::EOpenRes
MMEngClassMgr::eSelectClass(        TString&                strToFill
                            , const tCIDMacroEng::EResModes    eMode)
{
    //
    //  This method is provided for future flexibility, but for now it just
    //  delegates to the protected virtual.
    //
    return eDoSelect(strToFill, eMode);
}


TTextInStream*
MMEngClassMgr::pstrmLoadClass(  const   TString&                strClassPath
                                , const tCIDMacroEng::EResModes    eMode)
{
    //
    //  This method is provided for future flexibility, but for now it just
    //  delegates to the protected virtual.
    //
    return pstrmDoLoad(strClassPath, eMode);
}


tCIDLib::TVoid
MMEngClassMgr::StoreClass(  const   TString&                strClassPath
                            , const TString&                strText)
{
    //
    //  This method is provided for future flexibility, but for now it just
    //  delegates to the protected virtual.
    //
    DoStore(strClassPath, strText);
}


tCIDLib::TVoid MMEngClassMgr::UndoWriteMode(const TString& strClassPath)
{
    //
    //  This method is provided for future flexibility, but for now it just
    //  delegates to the protected virtual.
    //
    DoUndoWriteMode(strClassPath);
}



// ---------------------------------------------------------------------------
//  MMEngClassMgr: Hidden constructors
// ---------------------------------------------------------------------------
MMEngClassMgr::MMEngClassMgr()
{
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngFixedBaseClassMgr
// PREFIX: mecm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngFixedBaseClassMgr: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngFixedBaseClassMgr::TMEngFixedBaseClassMgr()
{
}

TMEngFixedBaseClassMgr::TMEngFixedBaseClassMgr(const TString& strBasePath) :

    m_strBasePath(strBasePath)
{
}

TMEngFixedBaseClassMgr::~TMEngFixedBaseClassMgr()
{
}


// ---------------------------------------------------------------------------
//  TMEngFixedBaseClassMgr: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TString& TMEngFixedBaseClassMgr::strBasePath() const
{
    return m_strBasePath;
}

const TString& TMEngFixedBaseClassMgr::strBasePath(const TString& strToSet)
{
    m_strBasePath = strToSet;
    return m_strBasePath;
}


// ---------------------------------------------------------------------------
//  TMEngFixedBaseClassMgr: Private, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngFixedBaseClassMgr::bCheckIfExists(const TString& strToCheck)
{
    m_pathTmp1 = m_strBasePath;

    // Get the path, cut off MEng, replace periods, and add an extension
    m_pathTmp2 = strToCheck;
    m_pathTmp2.Cut(0, 4);
    m_pathTmp2.bReplaceChar(kCIDLib::chPeriod, L'\\');
    m_pathTmp1.AddLevel(m_pathTmp2);
    m_pathTmp1.AppendExt(L".mengc");
    return TFileSys::bExists(m_pathTmp1);
}


tCIDLib::EOpenRes
TMEngFixedBaseClassMgr::eDoSelect(TString&, const tCIDMacroEng::EResModes)
{
    // We can't provide this at this level, so we just return cancel
    return tCIDLib::EOpenRes::Cancel;
}


tCIDLib::TVoid
TMEngFixedBaseClassMgr::DoStore(const   TString&    strClassPath
                                , const TString&    strText)
{
    // Build up the path to the classes hierarchy
    m_pathTmp1 = m_strBasePath;

    // Get the path, cut off MEng, replace periods, and add an extension
    m_pathTmp2 = strClassPath;
    m_pathTmp2.Cut(0, 4);
    m_pathTmp2.bReplaceChar(kCIDLib::chPeriod, L'\\');
    m_pathTmp1.AddLevel(m_pathTmp2);
    m_pathTmp1.AppendExt(L".mengc");

    //
    //  Create an output file stream for this file, always creating so that
    //  we always overwrite.
    //
    TTextFileOutStream strmOut
    (
        m_pathTmp1
        , tCIDLib::ECreateActs::CreateAlways
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Excl_OutStream
        , new TUTF8Converter
    );

    // Create an input text stream for the source text
    TTextStringInStream strmIn(&strText);

    // Now read lines from the source and write them to the output
    while (!strmIn.bEndOfStream())
    {
        strmIn.c4GetLine(m_pathTmp1);
        strmOut << m_pathTmp1 << kCIDLib::NewLn;
    }
    strmOut.Flush();
    strmOut.Close();
}


tCIDLib::TVoid
TMEngFixedBaseClassMgr::DoUndoWriteMode(const TString&)
{
    // For us, this is a no-op, since we don't do formal checkouts
}


TTextInStream*
TMEngFixedBaseClassMgr::pstrmDoLoad(const   TString&             strClassPath
                                    , const tCIDMacroEng::EResModes )
{
    // Build up the path to the classes hierarchy
    m_pathTmp1 = m_strBasePath;

    // Get the path, cut off MEng, replace periods, and add an extension
    m_pathTmp2 = strClassPath;
    m_pathTmp2.Cut(0, 4);
    m_pathTmp2.bReplaceChar(kCIDLib::chPeriod, L'\\');
    m_pathTmp1.AddLevel(m_pathTmp2);
    m_pathTmp1.AppendExt(L".mengc");

    return new TTextFileInStream
    (
        m_pathTmp1
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Excl_Read
        , new TUTF8Converter
    );
}



// ---------------------------------------------------------------------------
//  CLASS: MMEngExtClassLoader
// PREFIX: mecl
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  MMEngExtClassLoader: Constructors and Destructor
// ---------------------------------------------------------------------------
MMEngExtClassLoader::~MMEngExtClassLoader()
{
}


// ---------------------------------------------------------------------------
//  MMEngExtClassLoader: Hidden constructors
// ---------------------------------------------------------------------------
MMEngExtClassLoader::MMEngExtClassLoader()
{
}


