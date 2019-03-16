//
// FILE NAME: CIDLib_FindBuf.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/12/1993
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
//  This file implements the TFindBuf class, which is basically a wrapper
//  for the TKrnlFileSys::TRawFileFind kernel structure. So one TFindBuf
//  object represents one found file. TFileSys provides methods to build
//  them easily.
//
//  When formatting find buffers, you use a set of standard replacement
//  tokens (the same way that TTime and others are formatted".) This is
//  the most flexible way possible to do it. Each one has a default
//  format of just the file name. A couple of common ones are provided as
//  static members.
//
//  The token characters are:
//
//      'P'     - The path to the file
//      'N'     - The root part of the file name (with extension)
//      'S'     - The file size
//      'L'     - The last access time
//      'l'     - The last access date
//      'M'     - The last modify time
//      'm'     - The last modify date
//      'F'     - The full path and name together
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
//   CLASS: TFindBuf
//  PREFIX: fndb
// ---------------------------------------------------------------------------
class CIDLIBEXP TFindBuf :

    public TObject, public MDuplicable, public MStreamable, public MFormattable
{
    public  :
        // -------------------------------------------------------------------
        //  Class types
        // -------------------------------------------------------------------
        using TCursor   = TBag<TFindBuf>::TCursor;
        using TNCCursor = TBag<TFindBuf>::TNCCursor;


        // -------------------------------------------------------------------
        //  Public static methods
        // -------------------------------------------------------------------
        static tCIDLib::ESortComps eCompByPath
        (
            const   TFindBuf&               fndb1
            , const TFindBuf&               fndb2
        );

        static tCIDLib::ESortComps eCompByPathI
        (
            const   TFindBuf&               fndb1
            , const TFindBuf&               fndb2
        );

        static tCIDLib::ESortComps eCompByLastMod
        (
            const   TFindBuf&               fndb1
            , const TFindBuf&               fndb2
        );

        static const TString& strFull();

        static const TString& strGetKey
        (
            const   TFindBuf&               fndbSrc
        );

        static const TString& strNameAndSize();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFindBuf();

        TFindBuf
        (
            const   TString&                strDefFormat
        );

        TFindBuf
        (
            const   TFindBuf&               fndbToCopy
        );

        ~TFindBuf();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TBoolean operator<
        (
            const   TFindBuf&               fndbToTest
        )   const;

        tCIDLib::TBoolean operator>
        (
            const   TFindBuf&               fndbToTest
        )   const;

        tCIDLib::TBoolean operator==
        (
            const   TFindBuf&               fndbToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TFindBuf&               fndbToTest
        )   const;

        TFindBuf& operator=
        (
            const   TFindBuf&               fndbToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCanExecute() const;

        tCIDLib::TBoolean bCanRead() const;

        tCIDLib::TBoolean bCanWrite() const;

        tCIDLib::TBoolean bIsDirectory() const;

        tCIDLib::TBoolean bIsFile() const;

        tCIDLib::TBoolean bIsNormal() const;

        tCIDLib::TBoolean bIsNormalDir() const;

        tCIDLib::TBoolean bIsNormalFile() const;

        tCIDLib::TCard8 c8Size() const;

        TBag<TFindBuf>& colChildren();

        TCursor cursChildren() const;

        TNCCursor cursChildrenNC();

        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmToWriteTo
            , const TString&                strFormat
        )   const;

        const TPathStr& pathFileName() const;

        TPathStr& pathFileName();

        TPathStr& pathFileName
        (
            const   TString&                strToSet
        );

        tCIDLib::TVoid RemoveAllChildren();

        const TString& strDateFormat() const;

        const TString& strDateFormat
        (
            const   TString&                strNewDef
        );

        const TString& strDefFormat() const;

        const TString& strDefFormat
        (
            const   TString&                strNewDef
        );

        const TString& strTimeFormat() const;

        const TString& strTimeFormat
        (
            const   TString&                strNewDef
        );

        const TTime& tmCreated() const;

        const TTime& tmLastAccess() const;

        const TTime& tmLastModify() const;


    protected   :
        // -------------------------------------------------------------------
        //  Declare the directory iterator classes as friends
        // -------------------------------------------------------------------
        friend  class   TDirIter;
        friend  class   TDirIterCB;


        // -------------------------------------------------------------------
        //  Hidden constructors/operators
        // -------------------------------------------------------------------
        TFindBuf
        (
            const   TKrnlFileSys::TRawFileFind& rfndSrc
            , const TString&                strPath
        );


        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmToWriteTo
        )   const;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        );

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FromFindBuf
        (
            const   TKrnlFileSys::TRawFileFind& rfndSrc
            , const TString&                strPath
        );


    private         :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ReplicateChildren
        (
            const   TFindBuf&               fndSrc
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c8Size
        //      This is the size of the file in bytes.
        //
        //  m_eFlags
        //      These are the standard info flags of the found file. They
        //      drive most of the bIsXXX() methods.
        //
        //  m_pathFileName
        //      The path to the file or directory found.
        //
        //  m_pcolChildren
        //      When doing deep searches, if this node is a directory, this
        //      collection will contain all of the nodes for this directory's
        //      children that matched the search spec. Its a pointer so it
        //      has no cost unless its used. It is faulted in as needed.
        //
        //  m_tmCreated
        //  m_tmLastAccess
        //  m_tmLastModify
        //      The last access, last modification, and creation times of this
        //      entry.
        //
        //  m_strDefFormat
        //      This is the default formatting string. Used if one is not
        //      provided.
        //
        //  m_pstrDateFormat
        //  m_pstrDefFormat
        //  m_pstrTimeFormat
        //      These are format strings for the time/date members. They are
        //      faulted in only when accessed, since the default is usually
        //      fine.
        // -------------------------------------------------------------------
        tCIDLib::TCard8         m_c8Size;
        tCIDLib::EFileInfoFlags m_eFlags;
        TPathStr                m_pathFileName;
        mutable TBag<TFindBuf>* m_pcolChildren;
        TTime                   m_tmCreated;
        TTime                   m_tmLastAccess;
        TTime                   m_tmLastModify;

        // Faulted in upon access
        mutable TString*        m_pstrDateFormat;
        mutable TString*        m_pstrDefFormat;
        mutable TString*        m_pstrTimeFormat;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFindBuf, TObject)
        DefPolyDup(TFindBuf)
};

#pragma CIDLIB_POPPACK


