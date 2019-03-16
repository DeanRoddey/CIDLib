//
// FILE NAME: CIDLib_FileStreamImpl.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/29/1995
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
//  This is the header for the CIDLib_MemoryStreamImpl.Cpp file. This file
//  implements the stream implementations that allow a binary stream to work
//  on top of a file. There is one for readable streams, TFileInStreamImpl,
//  and one for writeable streams, TFileOutStreamImpl.
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
//   CLASS: TFileInStreamImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------
class TFileInStreamImpl : public TInStreamImpl
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFileInStreamImpl() = delete;

        TFileInStreamImpl
        (
                    TBinaryFile* const      pflToUse
            , const tCIDLib::EAdoptOpts     eAdopt = tCIDLib::EAdoptOpts::NoAdopt
        );

        TFileInStreamImpl(const TFileInStreamImpl&) = delete;

        ~TFileInStreamImpl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFileInStreamImpl& operator=(const TFileInStreamImpl&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsOpen() const override;

        tCIDLib::TCard4 c4ReadBytes
        (
                    tCIDLib::TVoid* const   pBuffer
            , const tCIDLib::TCard4         c4BytesToRead
        )   override;

        tCIDLib::TCard8 c8CurPos() const override;

        tCIDLib::TVoid Reset() override;

        tCIDLib::TVoid SkipForwardBy
        (
            const   tCIDLib::TCard4         c4SkipBy
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AdoptFile();

        tCIDLib::TBoolean bFileIsAdopted() const;

        tCIDLib::TVoid Close();

        tCIDLib::TCard8 c8CurSize() const;

        tCIDLib::TVoid Open
        (
            const   tCIDLib::ECreateActs    eAction
            , const tCIDLib::EFilePerms     ePerms
            , const tCIDLib::EFileFlags     eFlags
        );

        tCIDLib::TVoid Open
        (
            const   tCIDLib::EStdFiles      eFile
        );

        const TString& strFileName() const;



    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eAdopted
        //      Indicates whether we have adopted the file object.
        //
        //  m_pflData
        //      This is the binary file object that provides the data.
        // -------------------------------------------------------------------
        tCIDLib::EAdoptOpts m_eAdopted;
        TBinaryFile*        m_pflData;



        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFileInStreamImpl,TInStreamImpl)
};



// ---------------------------------------------------------------------------
//   CLASS: TFileOutStreamImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------
class TFileOutStreamImpl : public TOutStreamImpl
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFileOutStreamImpl();

        TFileOutStreamImpl
        (
                    TBinaryFile* const      pflToUse
            , const tCIDLib::EAdoptOpts     eAdopt = tCIDLib::EAdoptOpts::NoAdopt
        );

        TFileOutStreamImpl(const TFileOutStreamImpl&);

        ~TFileOutStreamImpl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFileOutStreamImpl& operator=(const TFileOutStreamImpl&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsOpen() const override;

        tCIDLib::TCard4 c4WriteBytes
        (
            const   tCIDLib::TVoid* const   pBuffer
            , const tCIDLib::TCard4         c4BytesToWrite
        )   override;

        tCIDLib::TCard8 c8CurPos() const override;

        tCIDLib::TVoid Reset() override;

        tCIDLib::TVoid SeekToEnd() override;



        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AdoptFile();

        tCIDLib::TBoolean bFileIsAdopted() const;

        tCIDLib::TVoid Close();

        tCIDLib::TCard8 c8CurSize() const;

        tCIDLib::TVoid Open
        (
            const   tCIDLib::ECreateActs    eAction
            , const tCIDLib::EFilePerms     ePerms
            , const tCIDLib::EFileFlags     eFlags
        );

        tCIDLib::TVoid Open
        (
            const   tCIDLib::EStdFiles      eFile
        );

        const TString& strFileName() const;

        tCIDLib::TVoid TruncateAt
        (
            const   tCIDLib::TCard8&        c8Pos
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eAdopted
        //      Indicates whether we have adopted the file object.
        //
        //  m_pflData
        //      This is the binary file object that provides the data.
        // -------------------------------------------------------------------
        tCIDLib::EAdoptOpts m_eAdopted;
        TBinaryFile*        m_pflData;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFileOutStreamImpl,TOutStreamImpl)
};

#pragma CIDLIB_POPPACK


