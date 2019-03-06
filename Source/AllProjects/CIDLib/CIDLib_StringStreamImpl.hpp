//
// FILE NAME: CIDLib_StringStreamImpl.hpp
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
//  This is the header for the CIDLib_StringStreamImpl.Cpp file, which
//  implements a stream implementation classes.
//
// CAVEATS/GOTCHAS:
//
//  1)  This is a wierd one in some ways. Strings are ONLY Unicode format,
//      but we are allowing a binary stream to stream into it. The stream
//      classes make sure that its only ever used in UTF-16 encoding mode,
//      but it could be set up otherwise. If so, then junk data could be
//      streamed into it. It wouldn't fail, it would just place meaningless
//      Unicode characters into the string.
//
//      Since we provide convenience classes to create most combinations of
//      streams, the TTextStringInStream and TTextStringOutStream classes
//      will usually be used to create text streams on top of strings, and
//      they will insure that the encoding is set to UTF-16. They will not
//      provide any mechanism to set the encoding as others do, so that they
//      cannot set it to something else.
//
//      We cannot even check here that the right thing is being done upstream,
//      since the text stream buffers and writes out bunches of bytes at once.
//      The fact even that it wrote an odd number of bytes means nothing,
//      since the next bunch might contain the second byte of that final
//      character.
//
//  2)  The current position maintained by these classes are in terms of
//      Unicode chars into the string. However, the semantics of the binary
//      streams that use stream impl objects is in terms of bytes, so we have
//      to convert for any reporting of positions to the outside world.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TStringOutStreamImpl;


// ---------------------------------------------------------------------------
//   CLASS: TStringStreamImplInfo
//  PREFIX: sii
// ---------------------------------------------------------------------------
class CIDLIBEXP TStringStreamImplInfo
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStringStreamImplInfo() = delete;

        TStringStreamImplInfo
        (
            const   TString* const          pstrToUse
            , const tCIDLib::EAdoptOpts     eAdoptBuf
        );

        TStringStreamImplInfo(const TStringStreamImplInfo&) = delete;

        ~TStringStreamImplInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStringStreamImplInfo& operator=(const TStringStreamImplInfo&) = delete;


        // -------------------------------------------------------------------
        //  Public data members
        //
        //  The data is public because this object serves only to hold data
        //  for the impl objects and they are trusted to do the right thing.
        //  The outside world doesn't ever look inside here.
        //
        //  eAdopted
        //      This indicates whether we've adopted the string.
        //
        //  pstrData
        //      This is the pointer to the string that provides the data
        //      source for this implementation. Its const so that it can work
        //      for both in and out streams.
        // -------------------------------------------------------------------
        tCIDLib::EAdoptOpts eAdopted;
        const TString*      pstrData;
};



// ---------------------------------------------------------------------------
//   CLASS: TStringInStreamImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------
class CIDLIBEXP TStringInStreamImpl : public TInStreamImpl
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStringInStreamImpl() = delete;

        TStringInStreamImpl
        (
            const   TString* const          pstrData
            , const tCIDLib::EAdoptOpts     eAdopt = tCIDLib::EAdoptOpts::Adopt
        );

        TStringInStreamImpl
        (
            const   tCIDLib::TCard4         c4InitSize
        );

        TStringInStreamImpl
        (
            const   TStringOutStreamImpl&   strmiToSyncWith
        );

        TStringInStreamImpl(const TStringInStreamImpl&) = delete;

        ~TStringInStreamImpl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStringInStreamImpl& operator=(const TStringInStreamImpl&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsOpen() const override
        {
            // Always true for this type
            return kCIDLib::True;
        }

        tCIDLib::TCard4 c4ReadBytes
        (
                    tCIDLib::TVoid* const   pBuffer
            , const tCIDLib::TCard4         c4BytesToRead
        )   override;

        tCIDLib::TCard8 c8CurPos() const  override
        {
            // Convert our character index into a byte count
            return m_c4CurPos * kCIDLib::c4CharBytes;
        }

        tCIDLib::TVoid Reset() override
        {
            m_c4CurPos = 0;
        }

        tCIDLib::TVoid SkipForwardBy
        (
            const   tCIDLib::TCard4         c4SkipBy
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AdoptString()
        {
            m_cptrInfo->eAdopted = tCIDLib::EAdoptOpts::Adopt;
        }

        tCIDLib::TBoolean bStringIsAdopted() const
        {
            return (m_cptrInfo->eAdopted == tCIDLib::EAdoptOpts::Adopt);
        }

        const TString& strData() const
        {
            return *m_pstrIn;
        }


    private :
        // -------------------------------------------------------------------
        //  Protected data members
        //
        //  m_c4CurPos
        //      This is the current position that we are in the string. When
        //      it hits the null terminator of the string, then its at the
        //      end of stream.
        //
        //  m_cptrInfo
        //      This impl object's stream impl info object. It holds the part
        //      of the data that has to be shared among impls that are using
        //      the same string. Its managed via a counted pointer object so
        //      that we don't have to track its usage.
        //
        //  m_pstrIn
        //      Though we give the string to the info object, who owns it,
        //      it speeds things up a bit for us to keep our own view of it.
        // -------------------------------------------------------------------
        tCIDLib::TCard4                 m_c4CurPos;
        TCntPtr<TStringStreamImplInfo>  m_cptrInfo;
        const TString*                  m_pstrIn;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TStringInStreamImpl, TInStreamImpl)
};



// ---------------------------------------------------------------------------
//   CLASS: TStringOutStreamImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------
class CIDLIBEXP TStringOutStreamImpl : public TOutStreamImpl
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStringOutStreamImpl() = delete;

        TStringOutStreamImpl
        (
                    TString* const          pstrData
            , const tCIDLib::EAdoptOpts     eAdopt = tCIDLib::EAdoptOpts::Adopt
            , const   tCIDLib::TBoolean     bAppend = kCIDLib::True
        );

        TStringOutStreamImpl
        (
            const   tCIDLib::TCard4         c4InitSize
        );

        TStringOutStreamImpl(const TStringOutStreamImpl&) = delete;

        ~TStringOutStreamImpl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStringOutStreamImpl& operator==(const TStringOutStreamImpl&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsOpen() const override
        {
            // Always true for this type
            return kCIDLib::True;
        }

        tCIDLib::TCard4 c4WriteBytes
        (
            const   tCIDLib::TVoid* const   pBuffer
            , const tCIDLib::TCard4         c4BytesToWrite
        )   override;

        tCIDLib::TCard8 c8CurPos() const override
        {
            // Convert our character index into a byte count
            return m_c4CurPos * kCIDLib::c4CharBytes;
        }

        tCIDLib::TVoid Reset() override
        {
            m_c4CurPos = 0;
            m_pstrOut->Clear();
        }

        tCIDLib::TVoid SeekToEnd() override
        {
            // Reset our current position to the logical end
            m_c4CurPos = m_pstrOut->c4Length();
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AdoptString()
        {
            m_cptrInfo->eAdopted = tCIDLib::EAdoptOpts::Adopt;
        }

        tCIDLib::TBoolean bStringIsAdopted() const
        {
            return (m_cptrInfo->eAdopted == tCIDLib::EAdoptOpts::Adopt);
        }

        const TString& strData() const
        {
            return *m_cptrInfo->pstrData;
        }


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TStringInStreamImpl;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4CurPos
        //      This is the current position that we are in the string, in
        //      terms of characters (so we have to convert this to bytes for
        //      the outside world.)
        //
        //  m_cptrInfo
        //      This impl object's stream impl info object. It holds the part
        //      of the data that has to be shared among impls that are using
        //      the same string. Its managed via a counted pointer object so
        //      that we don't have to track its usage.
        //
        //  m_pstrOut
        //      Though we give the string to the stream impl info object, we
        //      also need to keep our own non-const version of it.
        // -------------------------------------------------------------------
        tCIDLib::TCard4                 m_c4CurPos;
        TCntPtr<TStringStreamImplInfo>  m_cptrInfo;
        TString*                        m_pstrOut;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TStringOutStreamImpl,TOutStreamImpl)
};

#pragma CIDLIB_POPPACK
