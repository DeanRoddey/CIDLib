//
// FILE NAME: CIDKernel_File.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/10/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDKernel_File.Cpp file. This file
//  implements the TKrnlFile class, which encapsulates the host's file
//  oriented services (not file system, just file.)
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class   TKrnlTimeStamp;


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TKrnlFile
//  PREFIX: kfl
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlFile
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bOpenStdFile
        (
            const   tCIDLib::EStdFiles      eFile
            ,       TFileHandle&            hflToFill
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlFile();

        TKrnlFile
        (
            const   tCIDLib::TCh* const     pszName
        );

        TKrnlFile(const TKrnlFile&) = delete;

        ~TKrnlFile();


        // -------------------------------------------------------------------
        //  Public opeators
        // -------------------------------------------------------------------
        TKrnlFile& operator=(const TKrnlFile&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bClose();

        tCIDLib::TBoolean bFlush();

        tCIDLib::TBoolean bIsValid() const;

        tCIDLib::TBoolean bOffsetFilePointer
        (
            const   tCIDLib::TInt8&         i8OffsetBy
            ,       tCIDLib::TCard8&        c8ToFill
        );

        tCIDLib::TBoolean bOpen
        (
            const   tCIDLib::EAccessModes   eAccess
            , const tCIDLib::ECreateActs    eAction
            , const tCIDLib::EFilePerms     ePerms
            , const tCIDLib::EFileFlags     eFlags
        );

        tCIDLib::TBoolean bOpen
        (
            const   tCIDLib::EStdFiles      eStdFile
        );

        tCIDLib::TBoolean bQueryCurSize
        (
                    tCIDLib::TCard8&        c8ToFill
        )   const;

        tCIDLib::TBoolean bQueryFileInfo
        (
                    TKrnlFileSys::TRawFileFind& fndbToFill
        );

        tCIDLib::TBoolean bQueryFilePtr
        (
                    tCIDLib::TCard8&        c8ToFill
        )   const;

        tCIDLib::TBoolean bQueryFileTimes
        (
                    tCIDLib::TEncodedTime&  enctLastAccess
            ,       tCIDLib::TEncodedTime&  enctLastWrite
        );

        tCIDLib::TBoolean bQueryLastAccessTime
        (
                    tCIDLib::TEncodedTime&  enctToFill
        )   const;

        tCIDLib::TBoolean bQueryLastWriteTime
        (
                    tCIDLib::TEncodedTime&  enctToFill
        )   const;

        tCIDLib::TBoolean bReadBuffer
        (
                    tCIDLib::TVoid* const   pBuffer
            , const tCIDLib::TCard4         c4ToRead
            ,       tCIDLib::TCard4&        c4BytesRead
        );

        tCIDLib::TBoolean bReadBufferTO
        (
                    tCIDLib::TVoid* const   pBuffer
            , const tCIDLib::TCard4         c4ToRead
            ,       tCIDLib::TCard4&        c4BytesRead
            , const tCIDLib::TCard4         c4MaxWait
            ,       TKrnlEvent&             kevToPost
        );

        tCIDLib::TBoolean bSetFilePointer
        (
            const   tCIDLib::TCard8&        c8ToSet
        );

        tCIDLib::TBoolean bSetFileTimes
        (
            const   tCIDLib::TEncodedTime&  enctLastAccess
            , const tCIDLib::TEncodedTime&  enctLastWrite
        );

        tCIDLib::TBoolean bSetLastAccessTime(const tCIDLib::TEncodedTime& enctLastAccess)
        {
            return bSetFileTimes(enctLastAccess, tCIDLib::TEncodedTime(-1));
        }

        tCIDLib::TBoolean bSetLastWriteTime(const tCIDLib::TEncodedTime& enctLastWrite)
        {
            return bSetFileTimes(tCIDLib::TEncodedTime(-1), enctLastWrite);
        }

        tCIDLib::TBoolean bSetName
        (
            const   tCIDLib::TCh* const     pszNewName
        );

        tCIDLib::TBoolean bTruncateAt
        (
            const   tCIDLib::TCard8&        c8Position
        );

        tCIDLib::TBoolean bTruncateAt();

        tCIDLib::TBoolean bWriteBuffer
        (
            const   tCIDLib::TVoid* const   pBuffer
            , const tCIDLib::TCard4         c4ToWrite
            ,       tCIDLib::TCard4&        c4BytesWritten
        );

        tCIDLib::TBoolean bWriteBufferTO
        (
            const   tCIDLib::TVoid* const   pBuffer
            , const tCIDLib::TCard4         c4ToRead
            ,       tCIDLib::TCard4&        c4BytesRead
            , const tCIDLib::TCard4         c4MaxWait
            ,       TKrnlEvent&             kevToPost
        );

        const TFileHandle& hflThis() const
        {
            return m_hflThis;
        }

        const tCIDLib::TCh* pszName() const
        {
            return m_pszName;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_hflThis
        //      The handle to the file.
        //
        //  m_pszName
        //      The name of the file. Set during the construction and never
        //      changed. A buffer is allocated to hold it.
        // -------------------------------------------------------------------
        TFileHandle     m_hflThis;
        tCIDLib::TCh*   m_pszName;
};

#pragma CIDLIB_POPPACK

