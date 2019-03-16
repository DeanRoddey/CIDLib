//
// FILE NAME: CIDLib_FileBase.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/22/1993
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
//  This file implements the TFileBase class. TFileBase is an abstract class
//  that is the basis of all file classes. There are a good many information
//  and control methods that can be done to any file so they are included
//  here. Some methods are protected and are only exposed by derivatives for
//  which they are legal. This class uses the TKrnlFile class as its mechanism
//  for accessing host file services.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TEvent;

// ---------------------------------------------------------------------------
//   CLASS: TFileBase
//  PREFIX: fbase
// ---------------------------------------------------------------------------
class CIDLIBEXP TFileBase : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFileBase();

        TFileBase
        (
            const   TString&                strFileName
        );

        TFileBase
        (
            const   tCIDLib::TCh* const     pszFileName
        );

        TFileBase(const TFileBase&) = delete;

        ~TFileBase();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFileBase& operator=(const TFileBase&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bEndOfFile();

        tCIDLib::TBoolean bExists
        (
            const   tCIDLib::EDirSearchFlags eFlags = tCIDLib::EDirSearchFlags::All
        )   const;

        tCIDLib::TBoolean bExists
        (
            const   tCIDLib::EDirSearchFlags eFlags
            ,       tCIDLib::TBoolean&      bUnique
        )   const;

        tCIDLib::TVoid Flush();

        tCIDLib::TBoolean bIsOpen() const;

        tCIDLib::TVoid Close();

        tCIDLib::TCard8 c8CurPos();

        tCIDLib::TCard8 c8CurSize();

        const TKrnlFile& kflThis() const;

        TKrnlFile& kflThis();

        tCIDLib::TVoid Open
        (
            const   tCIDLib::EAccessModes   eAccess
            , const tCIDLib::ECreateActs    eAction
            , const tCIDLib::EFilePerms     ePerms
            , const tCIDLib::EFileFlags     eFlags
        );

        tCIDLib::TVoid Open
        (
            const   tCIDLib::EStdFiles      eStdFile
        );

        tCIDLib::TVoid QueryFileTimes
        (
                    TTime&                  tmLastAccess
            ,       TTime&                  tmLastWrite
        );

        const TString& strName() const;

        const TString& strName
        (
            const   TString&                strNew
        );

        tCIDLib::TVoid SetFileTimes
        (
            const   TTime&                  tmLastAccess
            , const TTime&                  tmLastWrite
        );

        tCIDLib::TVoid SetLastAccessTime
        (
            const   TTime&                  tmLastAccess
        );

        tCIDLib::TVoid SetLastWriteTime
        (
            const   TTime&                  tmLastWrite
        );

        TTime tmLastAccess() const;

        TTime tmLastWrite() const;

        tCIDLib::TVoid TruncateAt
        (
            const   tCIDLib::TCard8&        c8Position
        );

        tCIDLib::TVoid TruncateAt();


    protected :
        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4ReadRawBuf
        (
                    tCIDLib::TVoid* const   pBuf
            , const tCIDLib::TCard4         c4BufSz
        );

        tCIDLib::TCard4 c4ReadRawBufTO
        (
                    tCIDLib::TVoid* const   pBuf
            , const tCIDLib::TCard4         c4BufSz
            , const tCIDLib::TCard4         c4MaxWait
            ,       TEvent&                 evWait
            , const tCIDLib::TBoolean       bThrowIfTO
        );

        tCIDLib::TCard4 c4WriteRawBuf
        (
            const   tCIDLib::TVoid* const   pBuf
            , const tCIDLib::TCard4         c4BufSz
        );

        tCIDLib::TCard4 c4WriteRawBufTO
        (
            const   tCIDLib::TVoid* const   pBuf
            , const tCIDLib::TCard4         c4BufSz
            , const tCIDLib::TCard4         c4MaxWait
            ,       TEvent&                 evWait
        );

        tCIDLib::TCard8 c8OffsetPos
        (
            const   tCIDLib::TInt8&         i8ToOffset
        );

        tCIDLib::TVoid SetPos
        (
            const   tCIDLib::TCard8&        c8ToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Protected data members
        //
        //  m_kflThis
        //      This the kernel file object that represents the file in a
        //      platform independent way.
        //
        //  m_strName
        //      This is used to store the name of the file. It is stored just
        //      as it is provided by the creator, which may mean a relative
        //      path. The caller is responsible for making sure that the
        //      current directory is the correct base for the relative path
        //      given.
        // -------------------------------------------------------------------
        TKrnlFile   m_kflThis;
        TString     m_strName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFileBase,TObject)
};

#pragma CIDLIB_POPPACK


