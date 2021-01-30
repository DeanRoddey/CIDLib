//
// FILE NAME: CIDLib_BinaryFileStream.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/28/1997
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
//  This is the header for the CIDLib_BinaryFileStream.Cpp file, which
//  implements a couple very simple binary stream derivatives. These handle
//  the grunt work of setting up a file implementation object and installing
//  it on the input or output stream. You can do all of this yourself, but
//  these classes make it easy and safe.
//
//  These also provide a couple of file oriented methods, for when the user
//  knows he is dealing with a file oriented stream. They just turns around
//  and pass the commands on to the stream implementation object that controls
//  the file.
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
//   CLASS: TBinFileInStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDLIBEXP TBinFileInStream : public TBinInStream
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBinFileInStream();

        TBinFileInStream
        (
            const   tCIDLib::EStdFiles      eFile
        );

        TBinFileInStream
        (
            const   TString&                strFileName
            , const tCIDLib::ECreateActs    eAction
            , const tCIDLib::EFilePerms     ePerms
            , const tCIDLib::EFileFlags     eFlags
            , const tCIDLib::EAccessModes   eAccess = tCIDLib::EAccessModes::Read
        );

        TBinFileInStream(const TBinFileInStream&) = delete;
        TBinFileInStream(TBinFileInStream&&) = delete;

        ~TBinFileInStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBinFileInStream& operator=(const TBinFileInStream&) = delete;
        TBinFileInStream& operator=(TBinFileInStream&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Close();

        [[nodiscard]] tCIDLib::TCard8 c8CurSize() const;

        tCIDLib::TVoid Open
        (
            const   TString&                strFileName
            , const tCIDLib::ECreateActs    eAction
            , const tCIDLib::EFilePerms     ePerms
            , const tCIDLib::EFileFlags     eFlags
            , const tCIDLib::EAccessModes   eAccess = tCIDLib::EAccessModes::Read
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
        //  m_pstrmiFile
        //      We save a pointer to our impl object, in our specific derived
        //      type, in addition to giving it to our parent to own. This lets
        //      us avoid getting it from the parent and casting it.
        // -------------------------------------------------------------------
        TFileInStreamImpl*  m_pstrmiFile;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TBinFileInStream,TBinInStream)
};


// ---------------------------------------------------------------------------
//   CLASS: TBinFileOutStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDLIBEXP TBinFileOutStream : public TBinOutStream
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBinFileOutStream();

        TBinFileOutStream
        (
            const   tCIDLib::EStdFiles      eFile
        );

        TBinFileOutStream
        (
            const   TString&                strFileName
            , const tCIDLib::ECreateActs    eAction
            , const tCIDLib::EFilePerms     ePerms
            , const tCIDLib::EFileFlags     eFlags
            , const tCIDLib::EAccessModes   eAccess = tCIDLib::EAccessModes::Write
        );

        TBinFileOutStream(const TBinFileOutStream&) = delete;
        TBinFileOutStream(TBinFileOutStream&&) = delete;

        ~TBinFileOutStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBinFileOutStream& operator=(const TBinFileOutStream&) = delete;
        TBinFileOutStream& operator=(TBinFileOutStream&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Close();

        [[nodiscard]] tCIDLib::TCard8 c8CurSize() const;

        tCIDLib::TVoid Open
        (
            const   TString&                strFileName
            , const tCIDLib::ECreateActs    eAction
            , const tCIDLib::EFilePerms     ePerms
            , const tCIDLib::EFileFlags     eFlags
            , const tCIDLib::EAccessModes   eAccess = tCIDLib::EAccessModes::Write
        );

        tCIDLib::TVoid Open
        (
            const   tCIDLib::EStdFiles      eFile
        );

        const TString& strFileName() const;

        tCIDLib::TVoid SeekToEnd();

        tCIDLib::TVoid TruncateAt
        (
            const   tCIDLib::TCard8&        c8At
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pstrmiFile
        //      We save a pointer to our impl object, in our specific derived
        //      type, in addition to giving it to our parent to own. This lets
        //      us avoid getting it from the parent and casting it.
        // -------------------------------------------------------------------
        TFileOutStreamImpl*     m_pstrmiFile;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TBinFileOutStream,TBinOutStream)
};

#pragma CIDLIB_POPPACK

