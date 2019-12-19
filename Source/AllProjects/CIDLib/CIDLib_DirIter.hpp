//
// FILE NAME: CIDLib_DirIter.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/12/1993
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
//  This is the header for the directory iteration oriented classes.
//  TFileSys itself doesn't provide any iteration calls. It does provide
//  some calls to grab all the files in a directory into a list, but no
//  actual iteration.
//
//  This guy provides two iteration mechanims. TDirIter and TDirIterCB.
//
//  The former allows for iteration of a single directory. It provides find
//  first/next methods to work through the contents of a directory. If you
//  want to do a tree recursively you have to create one of these at each
//  level of recursion. You can reuse it as many times as you want. Calling
//  find first implicitly resets it.
//
//  TDirIterCB uses a callback, usually a lambda, that takes a string and
//  a boolean. For each directory and file it calls the callback, passing
//  the full path and true if its a file, else false to indicate it is a
//  directory.
//
//  The callback returns true to continue the search or false to stop it at
//  any point.
//
//      [](const TString& strCur, const tCIDLib::TBoolean bIsFile)
//      {
//          // Do something with strCur
//          return kCIDLib::True;
//      }
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once



#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TDirIter
//  PREFIX: diter
// ---------------------------------------------------------------------------
class CIDLIBEXP TDirIter : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDirIter();

        TDirIter(const TDirIter&) = delete;
        TDirIter(TDirIter&&) = delete;

        ~TDirIter();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDirIter& operator=(const TDirIter&) = delete;
        TDirIter& operator=(TDirIter&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bFindFirst
        (
            const   TString&                strSpec
            ,       TFindBuf&               fndbToFill
            , const tCIDLib::EDirSearchFlags eFlags = tCIDLib::EDirSearchFlags::All
        );

        tCIDLib::TBoolean bFindFirst
        (
            const   TString&                strPath
            , const TString&                strWildCard
            ,       TFindBuf&               fndbToFill
            , const tCIDLib::EDirSearchFlags eFlags = tCIDLib::EDirSearchFlags::All
        );

        tCIDLib::TBoolean bFindNext
        (
                    TFindBuf&               fndbToFill
        );

        tCIDLib::TVoid Reset();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_kdsSearch
        //      This is the kernel search object for this search.
        //
        //  m_pathSearchSpec
        //      This is the passed search spec. It is saved for logging errors
        //      and so it can be prepended to the file name.
        // -------------------------------------------------------------------
        TKrnlDirSearch  m_kdsSearch;
        TPathStr        m_pathSearchSpec;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDirIter,TObject)
};


// ---------------------------------------------------------------------------
//   CLASS: TDirIterCB
//  PREFIX: diter
// ---------------------------------------------------------------------------
class CIDLIBEXP TDirIterCB : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDirIterCB();

        TDirIterCB(const TDirIterCB&) = delete;
        TDirIterCB(TDirIterCB&&) = delete;

        ~TDirIterCB();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDirIterCB& operator=(const TDirIterCB&) = delete;
        TDirIterCB& operator=(TDirIterCB&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        template <typename IterCB>
        tCIDLib::TBoolean bIterate( const   TString&            strPath
                                    , const TString&            strWildCard
                                    , const tCIDLib::TBoolean   bRecurse
                                    ,       IterCB              iterCB                                    )
        {
            // Get the stack ready. This throws if initial diretory is bad
            Reset(strPath, strWildCard, bRecurse);

            // Special case callback the initial directory
            if (!iterCB(strPath, kCIDLib::False))
                return kCIDLib::False;

            tCIDLib::TBoolean bRet = kCIDLib::True;
            while (bRet)
            {
                // If we go all the way to the end, then break out with true
                if (!bNext())
                    break;

                //
                //  Else call the callback. Store their return into the
                //  return flag, which will cause the loop to exit if
                //  they return false (which means I want to stop.)
                //
                const TFindBuf& fndbRet = fndbCurrent();
                bRet = iterCB(fndbRet.pathFileName(), fndbRet.bIsFile());
            }
            return bRet;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data types
        //
        //  What we need to track for each level, used in a stack. Each level
        //  finds files first, then recurses if that is allowed.
        // -------------------------------------------------------------------
        enum class EStates
        {
            FirstFile
            , NextFile
            , FirstDir
            , NextDir
        };
        class TDepthInfo
        {
            public :
                TDepthInfo() = delete;
                TDepthInfo(const TFindBuf& fndbCur);
                TDepthInfo(const TDepthInfo&) = delete;
                TDepthInfo(TDepthInfo&&) = delete;
                ~TDepthInfo() {}

                TDepthInfo& operator=(const TDepthInfo&) = delete;
                TDepthInfo& operator=(TDepthInfo&&) = delete;

                tCIDLib::TBoolean bDoingDirs() const;
                tCIDLib::TBoolean bDoingFirst() const;

                EStates         m_eState;
                TFindBuf        m_fndbCur;
                TKrnlDirSearch  m_kdsSearch;

                //
                //  The incoming initial path is copied here to keep around
                //  since m_fndbCur will be updated for each new child at
                //  this level and we'd lose this. We need to keep it around
                //  for errors and to create the search spec for files and
                //  then directories.
                //
                TString         m_strCurPath;
        };
        using TIterStack = TRefStack<TDepthInfo>;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bNext();

        const TFindBuf& fndbCurrent() const;

        tCIDLib::TVoid Reset
        (
            const   TString&                strPath
            , const TString&                strWildCard
            , const tCIDLib::TBoolean       bRecurse
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bRecurse
        //      The caller tells us if we should recurse or not. If not, we just
        //      don't and that means we'll give up as soon as the initial level
        //      is used up.
        //
        //  m_colStack
        //      This is our faux recursion stack. The one on top is the one
        //      currently being searched.
        //
        //  m_pathTmp
        //      A temp to build up the search spec for each find first and some
        //      other places where we need to manipulate the path temporarily.
        //
        //  m_RawFindBuf
        //      For making calls to the kernel level finder.
        //
        //  m_strWildCard
        //      The wild card spec we are looking for. This needs to be stored
        //      separately since we need it for each round.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean           m_bRecurse;
        TIterStack                  m_colStack;
        TPathStr                    m_pathTmp;
        TKrnlFileSys::TRawFileFind  m_RawFindBuf;
        TString                     m_strWildCard;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TDirIterCB,TObject)
};

#pragma CIDLIB_POPPACK


