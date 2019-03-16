//
// FILE NAME: CIDLib_BinFile.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/28/1993
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
//  This is the header for the CIDLib_BinFile.Cpp file, which implements
//  the TBinaryFile class. TBinaryFile supports random access, binary file
//  support. It is based on the fundamental file class, TFileBase. TFileBase
//  provides the open/close calls, as well as the generic buffer read and
//  buffer write methods that are the basis of all file I/O done by this
//  class. It also provides other protected interfaces that specific
//  derivatives can make public via passthrough methods. We do this for the
//  read, write, and seek methods.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TMemBuf;


// ---------------------------------------------------------------------------
//   CLASS: TBinaryFile
//  PREFIX: bfl
// ---------------------------------------------------------------------------
class CIDLIBEXP TBinaryFile : public TFileBase
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBinaryFile();

        TBinaryFile
        (
            const   TString&                strFileName
        );

        explicit TBinaryFile
        (
            const   tCIDLib::TCh* const     pszFileName
        );

        TBinaryFile(const TBinaryFile&) = delete;

        ~TBinaryFile();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBinaryFile& operator=(const TBinaryFile&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4ReadBuffer
        (
                    tCIDLib::TVoid* const   pBufTarget
            , const tCIDLib::TCard4         c4BytesToRead
            , const tCIDLib::EAllData       eAllData = tCIDLib::EAllData::OkIfNotAll
        );

        tCIDLib::TCard4 c4ReadBuffer
        (
                    TMemBuf&                mbufTarget
            , const tCIDLib::TCard4         c4BytesToRead
            , const tCIDLib::EAllData       eAllData = tCIDLib::EAllData::OkIfNotAll
        );

        tCIDLib::TCard4 c4ReadBufferTO
        (
                    tCIDLib::TVoid* const   pBufTarget
            , const tCIDLib::TCard4         c4BytesToRead
            , const tCIDLib::TCard4         c4WaitFor
            ,       TEvent&                 evWait
            , const tCIDLib::TBoolean       bThrowIfTO
        );

        tCIDLib::TCard4 c4ReadBufferTO
        (
                    TMemBuf&                mbufTarget
            , const tCIDLib::TCard4         c4BytesToRead
            , const tCIDLib::TCard4         c4WaitFor
            ,       TEvent&                 evWait
            , const tCIDLib::TBoolean       bThrowIfTO
        );

        tCIDLib::TCard4 c4WriteBuffer
        (
            const   tCIDLib::TVoid* const   pBufSource
            , const tCIDLib::TCard4         c4BytesToWrite
        );

        tCIDLib::TCard4 c4WriteBuffer
        (
            const   TMemBuf&                mbufSource
            , const tCIDLib::TCard4         c4BytesToWrite = kCIDLib::c4MaxCard
        );

        tCIDLib::TCard4 c4WriteBufferTO
        (
            const   tCIDLib::TVoid* const   pBufTarget
            , const tCIDLib::TCard4         c4BytesToWrite
            , const tCIDLib::TCard4         c4WaitFor
            ,       TEvent&                 evWait
        );

        tCIDLib::TCard4 c4WriteBufferTO
        (
            const   TMemBuf&                mbufTarget
            , const tCIDLib::TCard4         c4BytesToWrite
            , const tCIDLib::TCard4         c4WaitFor
            ,       TEvent&                 evWait
        );

        tCIDLib::TCard8 c8OffsetFilePos
        (
            const   tCIDLib::TInt8&         i8OffsetBy
        );

        TBinInStream* pstrmMakeReadable
        (
            const   tCIDLib::EAdoptOpts     eAdopt = tCIDLib::EAdoptOpts::NoAdopt
        );

        TBinOutStream* pstrmMakeWriteable
        (
            const   tCIDLib::EAdoptOpts     eAdopt = tCIDLib::EAdoptOpts::NoAdopt
        );

        tCIDLib::TVoid SeekToEnd();

        tCIDLib::TVoid SetFilePos
        (
            const   tCIDLib::TCard8&        c8ToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TBinaryFile,TFileBase)
};

#pragma CIDLIB_POPPACK

