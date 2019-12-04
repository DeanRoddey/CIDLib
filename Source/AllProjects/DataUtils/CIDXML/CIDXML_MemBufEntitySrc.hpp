//
// FILE NAME: CIDXML_MemBufEntitySrc.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/28/1999
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
//  This is the header file for the CIDXML_MemBufEntitySrc.Cpp file, which
//  implements the TMemBufEntitySrc class. This is a derivative of the base
//  entity source class which provides support for memory buffer based
//  sources.
//
// CAVEATS/GOTCHAS:
//
//  1)  We can't know here how many bytes of the passed buffer are the
//      important ones (since the buffer might be generic and just being used
//      to load buffer after buffer.) So you must indicate how many of them
//      are valid via the c4SrcBytes parameter.
//
//  2)  The version that takes a reference to a mem buf object just copies
//      the buffer. The other version adopts the buffer.
//
//  3)  The version that takes the counter pointer to a memory buffer will
//      of course just reference the buffer, which will be dropped when the
//      last reference to it is dropped.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TMemBufEntitySrc
// PREFIX: xsrc
// ---------------------------------------------------------------------------
class CIDXMLEXP TMemBufEntitySrc : public TXMLEntitySrc
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMemBufEntitySrc() = delete;

        TMemBufEntitySrc
        (
            const   TString&                strSystemId
            , const TMemBuf* const          pmbufToAdopt
            , const tCIDLib::TCard4         c4SrcBytes
        );

        TMemBufEntitySrc
        (
            const   TString&                strSystemId
            , const TMemBuf&                mbufToCopy
            , const tCIDLib::TCard4         c4SrcBytes
        );

        TMemBufEntitySrc
        (
            const   TString&                strSystemId
            ,       THeapBuf&&              mbufToTake
            , const tCIDLib::TCard4         c4SrcBytes
        );

        TMemBufEntitySrc
        (
            const   TString&                strSystemId
            , const TString&                strPublicId
            , const TMemBuf&                mbufToCopy
            , const tCIDLib::TCard4         c4SrcBytes
        );

        TMemBufEntitySrc
        (
            const   TString&                strSystemId
            , const TString&                strPublicId
            ,       THeapBuf&&              mbufToTake
            , const tCIDLib::TCard4         c4SrcBytes
        );

        TMemBufEntitySrc
        (
            const   TString&                strSystemId
            , const TString&                strXMLText
        );

        TMemBufEntitySrc
        (
            const   TString&                strSystemId
            , const TString&                strPublicId
            , const TString&                strXMLText
        );

        TMemBufEntitySrc
        (
            const   TString&                strSystemId
            , const TCntPtr<const TMemBuf>& cptrBuffer
            , const tCIDLib::TCard4         c4SrcBytes
        );

        TMemBufEntitySrc
        (
            const   TString&                strSystemId
            , const tCIDLib::TVoid* const   pDataToCopy
            , const tCIDLib::TCard4         c4SrcBytes
        );

        TMemBufEntitySrc
        (
            const   TString&                strSystemId
            , const tCIDLib::TSCh* const    pschDataToCopy
            , const tCIDLib::TCard4         c4SrcBytes
        );

        TMemBufEntitySrc
        (
            const   TString&                strSystemId
            , const TString&                strPublicId
            , const tCIDLib::TSCh* const    pschDataToCopy
            , const tCIDLib::TCard4         c4SrcBytes
        );

        TMemBufEntitySrc
        (
            const   TMemBufEntitySrc&       xsrcSrc
        );

        TMemBufEntitySrc(TMemBufEntitySrc&&) = delete;

        ~TMemBufEntitySrc();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMemBufEntitySrc& operator=
        (
            const   TMemBufEntitySrc&       xsrcSrc
        );

        TMemBufEntitySrc& operator=(TMemBufEntitySrc&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        [[nodiscard]] TBinInStream* pstrmMakeNew() const final;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4SrcBytes() const;

        tCIDLib::TCard4 c4SrcBytes
        (
            const   tCIDLib::TCard4         c4ToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4SrcBytes
        //      The number of bytes of the source memory buffer that are data
        //      for us to parse.
        //
        //  m_cptrBuffer
        //      This is a counted pointer that's used to ref count the
        //      memory buffer. Since many copies of an entity source might be
        //      returned from a cache or catalog, we cannot afford to copy it
        //      every time. Since the data is never changed, just read, it
        //      makes imminent sense to reference count a single buffer.
        //
        //      We never modify the original contents, so its a counted pointer
        //      to a const memory buffer pointer.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4SrcBytes;
        TCntPtr<const TMemBuf>  m_cptrBuffer;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMemBufEntitySrc,TXMLEntitySrc)
        DefPolyDup(TMemBufEntitySrc)
};

#pragma CIDLIB_POPPACK


