//
// FILE NAME: CIDLib_TextFileStream.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/28/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDLib_TextFileStream.Cpp file, which implements
//  the TTextFileInStream and TTextFileOutStream classes. These are both
//  convenience classes that does the grunt work of setting up a TTextInStream
//  or TTextOutStream to work on top of a binary stream that is streaming
//  to/from a file object.
//
//  You could do all of this yourself, but these classes make it much more
//  convient and safe.
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
//   CLASS: TTextFileInStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDLIBEXP TTextFileInStream : public TTextInStream
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        explicit TTextFileInStream
        (
                    TTextConverter* const   ptcvtToAdopt = 0
        );

        TTextFileInStream
        (
            const   TString&                strFileName
            , const tCIDLib::ECreateActs    eAction
            , const tCIDLib::EFilePerms     ePerms
            , const tCIDLib::EFileFlags     eFlags
            , const tCIDLib::EAccessModes   eAccess
            ,       TTextConverter* const   ptcvtToAdopt = 0
        );

        TTextFileInStream
        (
            const   tCIDLib::EStdFiles      eFile
            ,       TTextConverter* const   ptcvtToAdopt = 0
        );

        TTextFileInStream(const TTextFileInStream&) = delete;

        ~TTextFileInStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTextFileInStream& operator=(const TTextFileInStream&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsOpen() const;

        tCIDLib::TVoid Close();

        tCIDLib::TCard8 c8CurSize() const;

        tCIDLib::TVoid Open
        (
            const   TString&                strFileName
            , const tCIDLib::ECreateActs    eAction
            , const tCIDLib::EFilePerms     ePerms
            , const tCIDLib::EFileFlags     eFlags
            , const tCIDLib::EAccessModes   eAccess
        );

        tCIDLib::TVoid Open
        (
            const   tCIDLib::EStdFiles      eFile
        );

        const TString& strFileName() const;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ThrowNotReady
        (
            const   tCIDLib::TCard4         c4Line
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pstrmIn
        //      This is our type specific view of the binary stream that
        //      underlies this text stream. We don't own it though, since our
        //      parent class does the cleanup.
        // -------------------------------------------------------------------
        TBinFileInStream*   m_pstrmIn;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTextFileInStream,TTextInStream)
};



// ---------------------------------------------------------------------------
//   CLASS: TTextFileOutStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDLIBEXP TTextFileOutStream : public TTextOutStream
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        explicit TTextFileOutStream
        (
                    TTextConverter* const   ptcvtToAdopt = 0
        );

        TTextFileOutStream
        (
            const   TString&                strFileName
            , const tCIDLib::ECreateActs    eAction
            , const tCIDLib::EFilePerms     ePerms
            , const tCIDLib::EFileFlags     eFlags
            , const tCIDLib::EAccessModes   eAccess
            ,       TTextConverter* const   ptcvtToAdopt = 0
        );

        TTextFileOutStream
        (
            const   tCIDLib::EStdFiles      eFile
            ,       TTextConverter* const   ptcvtToAdopt = 0
        );

        TTextFileOutStream(const TTextFileOutStream&) = delete;

        ~TTextFileOutStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTextFileOutStream& operator=(const TTextFileOutStream&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsOpen() const;

        tCIDLib::TVoid Close();

        tCIDLib::TCard8 c8CurSize() const;

        tCIDLib::TVoid Open
        (
            const   TString&                strFileName
            , const tCIDLib::ECreateActs    eAction
            , const tCIDLib::EFilePerms     ePerms
            , const tCIDLib::EFileFlags     eFlags
            , const tCIDLib::EAccessModes   eAccess
        );

        tCIDLib::TVoid Open
        (
            const   tCIDLib::EStdFiles      eFile
        );

        tCIDLib::TVoid SeekToEnd();

        const TString& strFileName() const;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ThrowNotReady
        (
            const   tCIDLib::TCard4         c4Line
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pstrmOut
        //      This is our type specific view of the binary stream that
        //      underlies this text stream. We don't own it though, since our
        //      parent class does the cleanup.
        // -------------------------------------------------------------------
        TBinFileOutStream*  m_pstrmOut;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTextFileOutStream,TTextOutStream)
};

#pragma CIDLIB_POPPACK


