//
// FILE NAME: CIDPack_Ctrl_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/21/2015
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
//  This is the header for the CIDPack_Ctrl.cpp file. This file defines the classes
//  that represent the control structures within the file. There is one for the
//  overall file header, and one that represents the per-file entries in the map
//  that goes after the overall header (and before the per-file data.)
//
//  These guys read and write their data in the format required for the packing
//  file format definition. This is convenient since we can just read them directly
//  into objects that we can manipulate easily, or set up the objects and write
//  them out easily.
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
//  CLASS: TCIDPackFlHdr
// PREFIX: pkfhdr
// ---------------------------------------------------------------------------
class CIDPACKEXP TCIDPackFlHdr : public TObject, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TCIDPackFlHdr() = default;

        TCIDPackFlHdr
        (
            const   tCIDLib::TCard4         c4CompBytes
            , const tCIDLib::TCard4         c4OrgBytes
            , const TMD5Hash&               mhashOrg
            , const TString&                strRelPath
        );

        TCIDPackFlHdr(const TCIDPackFlHdr&) = default;
        TCIDPackFlHdr(TCIDPackFlHdr&&) = default;

        ~TCIDPackFlHdr() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDPackFlHdr& operator=(const TCIDPackFlHdr&) = default;
        TCIDPackFlHdr& operator=(TCIDPackFlHdr&&) = default;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4CompBytes() const;

        tCIDLib::TCard4 c4OrgBytes() const;

        const TMD5Hash& mhashOrg() const;

        const TString& strRelPath() const;


    protected   :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   final;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const final;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4CompBytes
        //  m_c4OrgBytes
        //      The size of the compressed data, which will follow this header
        //      in the file, and the number of original bytes (for validation
        //      up decompression.)
        //
        //  m_mhashOrg
        //      The MD5 has of the original, uncompressed data. What is actually
        //      written out to the file header info is just the raw hash bytes.
        //
        //  m_strRelPath
        //      The relative path of this file, relative to the original top
        //      level directory.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4CompBytes = 0;
        tCIDLib::TCard4     m_c4OrgBytes = 0;
        TMD5Hash            m_mhashOrg;
        TString             m_strRelPath;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDPackFlHdr,TObject)
};


// ---------------------------------------------------------------------------
//  CLASS: TCIDPackHdr
// PREFIX: pkhdr
// ---------------------------------------------------------------------------
class CIDPACKEXP TCIDPackHdr : public TObject, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TCIDPackHdr() = default;

        TCIDPackHdr
        (
            const   tCIDLib::TCard4         c4FileCount
            , const tCIDLib::TCard8         c8Version
            , const tCIDLib::TEncodedTime   enctStamp
        );

        TCIDPackHdr(const TCIDPackHdr&) = default;
        TCIDPackHdr(TCIDPackHdr&&) = default;

        ~TCIDPackHdr() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDPackHdr& operator=(const  TCIDPackHdr&) = default;
        TCIDPackHdr& operator=(TCIDPackHdr&&) = default;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4FileCount() const;

        tCIDLib::TCard4 c4User() const;

        tCIDLib::TCard4 c4User
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TCard8 c8User() const;

        tCIDLib::TCard8 c8User
        (
            const   tCIDLib::TCard8         c8ToSet
        );

        tCIDLib::TCard8 c8Version() const;

        tCIDLib::TEncodedTime enctStamp() const;

        const TString& strUser() const;

        const TString& strUser
        (
            const   TString&                strToSet
        );


    protected   :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   final;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const final;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4FileCount
        //      The total count of files in this package. This is for validation,
        //      since we can count how many files we've read as we go during
        //      unpack an compare.
        //
        //  m_c4User
        //      A 32 bit value for client code use to store some useful info into
        //      the file.
        //
        //  m_c8User
        //      A 64 bit value for client code use to store some useful info into
        //      the file.
        //
        //  m_c48ersion
        //      This is for client code usage. It's not the file format version
        //      of the package, but some version related to the data that is in
        //      this package.
        //
        //  m_enctStamp
        //      A time stamp provided by client code to mark this package with.
        //
        //  m_strUser
        //      Some open ended info that can be stored in the file for the use of
        //      client code. It's not compressed, so it shouldn't be really big.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4FileCount = 0;
        tCIDLib::TCard4         m_c4User = 0;
        tCIDLib::TCard8         m_c8User = 0;
        tCIDLib::TCard8         m_c8Version = 0;
        tCIDLib::TEncodedTime   m_enctStamp = 0;
        TString                 m_strUser;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDPackHdr,TObject)
};


#pragma CIDLIB_POPPACK


