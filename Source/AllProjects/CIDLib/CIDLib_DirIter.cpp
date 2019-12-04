//
// FILE NAME: CIDLib_DirIter.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/13/1993
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
//  This file implements the TDirIter class, which allows for iteration of the
//  files in a directory.
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
#include    "CIDLib_.hpp"

// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TDirIter,TObject)
RTTIDecls(TDirIterCB,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TDirIter
//  PREFIX: diter
//
//  This class provides directory search services
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDirIter: Constructors and Destructor
// ---------------------------------------------------------------------------
TDirIter::TDirIter()
{
}

TDirIter::~TDirIter()
{
}


// ---------------------------------------------------------------------------
//  TDirIter: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TDirIter::bFindNext(TFindBuf& fndBuf)
{
    TKrnlFileSys::TRawFileFind  FindBuf = {};

    // If there is no open search, then log an error and abort
    if (!m_kdsSearch.bSearchIsOpen())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_NoOpenSearch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    // Try to find another match
    tCIDLib::TBoolean bFound;
    if (!m_kdsSearch.bFindNext(FindBuf, bFound))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcDIter_FindFailure
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , m_pathSearchSpec
        );
    }

    if (!bFound)
        return kCIDLib::False;

    // We got a match, so copy the needed stuff to this object
    fndBuf.FromFindBuf(FindBuf, m_pathSearchSpec);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TDirIter::bFindFirst(   const   TString&                    strSpec
                        ,       TFindBuf&                   fndBuf
                        , const tCIDLib::EDirSearchFlags    eFlags)
{
    TKrnlFileSys::TRawFileFind  FindBuf;

    // Store away the spec and attributes
    m_pathSearchSpec    = strSpec;

    // Strip the name and extension off of the search spec
    m_pathSearchSpec.bRemoveNameExt();

    // Try to start the search
    tCIDLib::TBoolean bFound;
    if (!m_kdsSearch.bFindFirst(strSpec.pszBuffer()
                                , FindBuf
                                , bFound
                                , eFlags))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcDIter_FindFailure
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strSpec
        );
    }

    if (!bFound)
        return kCIDLib::False;

    // Copy the needed stuff back
    fndBuf.FromFindBuf(FindBuf, m_pathSearchSpec);

    return kCIDLib::True;
}

tCIDLib::TBoolean
TDirIter::bFindFirst(   const   TString&                    strPath
                        , const TString&                    strWildCard
                        ,       TFindBuf&                   fndBuf
                        , const tCIDLib::EDirSearchFlags    eFlags)
{
    //
    //  Just build a full spec out of the passed path and wildcards, then
    //  call the other version
    //
    TPathStr pathTmp(strPath);

    // Add the wild cards to the path in the temp string
    pathTmp.AddLevel(strWildCard);

    // Pass along our data and get the respose
    return bFindFirst(pathTmp, fndBuf, eFlags);
}


tCIDLib::TVoid TDirIter::Reset()
{
    if (!m_kdsSearch.bClose())
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_CloseDirSearch
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}



// ---------------------------------------------------------------------------
//   CLASS: TDirIterCB
//  PREFIX: diter
//
//  This class provides a callback based directory tree search.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDirIterCB::TDepthInfo
// ---------------------------------------------------------------------------

//
//  When we start a new directory, this is called with the find buffer for that
//  guy. WE store away his path separately, but we want the find buffer because
//  we need to do the callback for the directory as well, and this makes it
//  consistent, that the thing being called back for is always in the find buffer
//  member.
//
TDirIterCB::TDepthInfo::TDepthInfo(const TFindBuf& fndbCur) :

    m_eState(EStates::FirstFile)
    , m_fndbCur(fndbCur)
    , m_strCurPath(fndbCur.pathFileName())
{
}

tCIDLib::TBoolean TDirIterCB::TDepthInfo::bDoingDirs() const
{
    return ((m_eState == EStates::FirstDir) || (m_eState == EStates::NextDir));
}

tCIDLib::TBoolean TDirIterCB::TDepthInfo::bDoingFirst() const
{
    return ((m_eState == EStates::FirstDir) || (m_eState == EStates::FirstFile));
}



// ---------------------------------------------------------------------------
//  TDirIterCB: Constructors and Destructor
// ---------------------------------------------------------------------------
TDirIterCB::TDirIterCB() :

    m_bRecurse(kCIDLib::False)
    , m_colStack(tCIDLib::EAdoptOpts::Adopt)
    , m_RawFindBuf()
{
}

TDirIterCB::~TDirIterCB()
{
}


// ---------------------------------------------------------------------------
//  TDirIterCB: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This is called to drive the process. On each round we move forward to a new
//  file, pushing and popping as required.
//
tCIDLib::TBoolean TDirIterCB::bNext()
{
    tCIDLib::TBoolean bFound = kCIDLib::False;

    while (kCIDLib::True)
    {
        TDepthInfo& diCur = *m_colStack.pobjPeek();

        // Either do a find first or find next, depending on the state
        tCIDLib::TBoolean bRes = kCIDLib::False;
        if (diCur.bDoingFirst())
        {
            tCIDLib::EDirSearchFlags eFlags = tCIDLib::EDirSearchFlags::NormalFiles;
            m_pathTmp = diCur.m_strCurPath;
            if (diCur.m_eState == EStates::FirstFile)
            {
                eFlags = tCIDLib::EDirSearchFlags::NormalFiles;
                diCur.m_eState = EStates::NextFile;
                m_pathTmp.AddLevel(m_strWildCard);
            }
             else
            {
                eFlags = tCIDLib::EDirSearchFlags::NormalDirs;
                diCur.m_eState = EStates::NextDir;
                m_pathTmp.AddLevel(kCIDLib::pszAllDirsSpec);
            }

            bRes = diCur.m_kdsSearch.bFindFirst
            (
                m_pathTmp.pszBuffer(), m_RawFindBuf, bFound, eFlags
            );
        }
         else
        {
            bRes = diCur.m_kdsSearch.bFindNext(m_RawFindBuf, bFound);
        }

        // It it failed we throw
        if (!bRes)
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcDIter_FindFailure
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , diCur.m_strCurPath
            );
        }


        if (bFound)
        {
            // Get the info to our CIDLib level find buffer
            diCur.m_fndbCur.FromFindBuf(m_RawFindBuf, diCur.m_strCurPath);

            //
            //  We found something. If it's a directory, add to the stack. Else, it's a
            //  file. Else way, break out with true result to let the callback be done
            //  on this guy.
            //
            if (diCur.bDoingDirs())
                m_colStack.Add(new TDepthInfo(diCur.m_fndbCur));
            break;
        }
         else
        {
            if (diCur.bDoingDirs())
            {
                //
                //  We are out of directories, so we are done at this level. So
                //  pop the stack. If empty, fall out with false result. Else we
                //  go back around to pick up where we left off.
                //
                m_colStack.TrashTop();
                if (m_colStack.bIsEmpty())
                    break;
            }
             else
            {
                //
                //  We are out of files. If recursion is allowed, then set the state
                //  to start doing directories at this level. Else, just break out
                //  with false state.
                //
                if (!m_bRecurse)
                    break;
                diCur.m_eState = EStates::FirstDir;
            }
        }
    }

    return bFound;
}


// To avoid some verbosity in the inlined template method
const TFindBuf& TDirIterCB::fndbCurrent() const
{
    return m_colStack.pobjPeek()->m_fndbCur;
}



// This is called to reset us for another round
tCIDLib::TVoid TDirIterCB::Reset(const  TString&            strPath
                                , const TString&            strWildCard
                                , const tCIDLib::TBoolean   bRecurse)
{
    TFindBuf fndbInit;
    if (!TFileSys::bExists(strPath, fndbInit, tCIDLib::EDirSearchFlags::NormalDirs))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_MustBeADir
            , strPath
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
        );
    }

    m_colStack.RemoveAll();
    m_strWildCard = strWildCard;
    m_bRecurse = bRecurse;

    // Push the initial path onto the stack
    m_colStack.Add(new TDepthInfo(fndbInit));
}
