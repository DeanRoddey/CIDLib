//
// FILE NAME: CIDMData_ChunkedFile.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/29/2018
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
//  This file implements some classes that define a chunked file format that is very
//  generic and can be widely used within CIDLib and CQC where we need to store data
//  in a flexible way.
//
//  It is chunked so it is stored as a set of chunks of data, each of which has a
//  type and name. The chunks are predefined and currently there are only three types
//  of which two are always present and first in the file, and then there is a third for
//  extension chunks that can be defined by the client code.
//
//  The standard chunk ids are listed below, each of which is represented by a constant
//  defined in the IDL file. There can only be one instance of the first two types. They are
//  stored in this order, empty if not used but still present. Client defined extension
//  chunks come after that and are optional.

//  Any standard values will start with /CQSL/CF/, and of course any format specific ones
//  should start with the vendor specific path as outlined below. The built in meta keys
//  are listed below. Though some are standard, it's just so that there are some well
//  known keys. All of these values are for the application to use, none are set
//  automatically.
//
//      /CQSL/CF/Meta
//          Key/value string pairs for storing arbitrary metadata about the actual data.
//
//          This guy has to update the raw data and hash on each change, so use the
//          SetMetaValues() method where possible to set/update them all at once.
//
//      /CQSL/CF/Data
//          The main data being stored (image, action, etc...) flattened to a byte
//          array.
//
//      [client defined]
//          A blob of bytes that we know nothing about. As with the built in ones above,
//          this should be a short id string and in the form /[vendor]/[fltype]/[name], to
//          insure uniqueness. Each one is marked as to whether it represents 'file data'
//          which means that the serial number/last change stamp will be bumped if it
//          is modified. They all must be unique within the file.
//
//
//  Byte Order
//
//  We handle any big/little endian ordering for the binary housekeeping data in the
//  file. The format of the data chunk is the business of the application and we
//  don't get involved in that. They should handle byte order if necessary. We store
//  our housekeeping data in the standard CIDLib little endian format, so that means we
//  use our standard binary streams to read/write it since they handle byte swapping for
//  us based on the assumed little end on-disc format.
//
//
//  Standard Meta Keys
//
//  The standard meta chunk keys are list below. Any file format specific keys should be
//  in the form /[vendor]/[filetype]/[valname], to make sure that they are unique, so
//  for instance /Acme/Logs/PrevHistory or /ABHCo/Config/SrcServer and so forth.
//
//      /CQSL/CF/Application
//          A name for the application that created the data, if that is useful.
//
//      /CQSL/CF/Author
//          A name for the author who created the data, if that is useful.
//
//      /CQSL/CF/Notes
//          A string that can contain notes about the file. This should be kept to
//          some reasonably short length, it's not for a paper describing the file
//          format or anything long winded like that. Typically it would be user provided
//          notes about the specific data in this file, gotchas maybe.
//
//      /CQSL/CF/Version
//          A format version, which will be 3 numbers, separated by periods. If you only
//          need a single version number, it should be x.0.0, if only two, then x.y.0.
//          It is up the application to set this appropriately for its own needs.
//
//
//  Change Indicators
//
//  We maintain a serial number and last change time stamp. Any time any changes are made
//  that affect the actual file data, we bump the serial number and set the stamp to now
//  (UTC format.) This can be used by clients to know if they already have the most recent
//  copy of the file by just remembering the last serial number they got. The time stamp
//  is more for the generation of reports of what resources were last changed when. See
//  below for what constitutes data changes.
//
//
//  What is a Data Change?
//
//  If you have a chunked file object, you can always call bFiledChanged(). That will
//  compare the latest hashes on all of the chunks against the last ones we saved when
//  this file object was last stored. If any chunks have changed this will return
//  true.
//
//  But we also keep a serial number and last changed time stamp. These are only updated
//  when actual file data is changed. For programs that edit or renderer these files,
//  they only care about the actual data they edit, since they need to get the most recent
//  info if that changes. So these are only bumped for actual data changes. Changes to the
//  main data chunk always causes a bump of these values.
//
//  Meta keys are marked as to whether they are file change keys. If a file change key's
//  value is modified, that's treated as a file change. Else, even if the value changes
//  the serial number/stamp won't get bumped, though the overall bFiledChanged() will
//  return true.
//
//  Similarly extension chunks are marked as whether they represent file data or just extra
//  data. For instance, a thumbnail of the main image would only ever change when the main
//  data does, so it doesn't need to be thusly marked, that would only cause extra bumps.
//  Any that are marked will cause a bump of the serial number and last change stamp when
//  modified.
//
//
//  The actual low level format on disk is:
//
//      4 bytes     The file start magic id, defined in the IDL file
//      1 byte      The format version of this file (meaning this low level format in
//                  case we need to augment it over time, currently 1. This is only used
//                  when reading in, to convert to the latest format in memory, which
//                  will be used when writing it back out (and this value will be updated
//                  to the latest version.) So it is for backwards compatibility only,
//                  to allow old data to be upgraded upon read.
//      1 Byte      CIDLib stream frame marker
//      4 Bytes     A serial number that can be used to automatically keep up with
//                  whether new changes are available since the last time the file was
//                  read by a client. It is bumped any time the data chunk is set and
//                  the hash is different.
//      8 Bytes     A UTC time stamp (in our standard CIDLib format) of the last time the
//                  data block was modified.
//      1 Byte      The count of chunks to follow
//      1 Byte      The count XORd with 0xFF
//      1 Byte      CIDLib stream frame marker
//
//      X Bytes     - The data chunk (see below)
//      X Bytes     - The meta chunk (see below)
//      X Bytes     - Zero or more extension chunks (see below)
//
//      4 bytes     The file end magic id, defined in the IDL file
//
//  Each of the chunks has a header (for validation sake) plus the actual data, so each
//  chunk looks like this:
//
//      1 byte      Chunk start id (defined in the IDL file)
//      1 byte      Count of id characters without null (ASCII, so one byte each)
//      1 byte      Same as above but XOR'd with 0xFF
//      X bytes     The id characters (without terminating null)
//      1 byte      Flags (0x1 = Data change)
//      1 byte      Frame marker
//      4 bytes     The count of data bytes in the chunk
//      20 bytes    The SHA-1 hash for this chunk
//      1 byte      Frame marker
//      X bytes     The actual byte array content of this chunk
//      1 byte      Chunk end id (defined in the IDL file)
//
//
//  There is no forward index, so that this format can be streamed to a non-random
//  access target. You have to read through the file, skipping forward over chunks you
//  don't care about, using the chunk header info. Unless explicitly required, all
//  chunks must be maintained across a read/write.
//
//  Upon reading, chunks will be validated against the stored hash, on write the hash
//  of the written content is stored.
//
//
//  There is a base class that all the chunks derive from, which lets us treat them the
//  same for some basic operations. The base class handles the chunk id, the buffer for
//  the raw data and its size, and the hash of the raw data, and any flags. All of the
//  chunks have these things in common.
//
//  The data chunk and extensions chunks which are just a raw blob of data to us, are
//  just simple wrappers around the base class which does all the work. Others,
//  like the meta chunk manage their own data in memory but always update the raw data
//  in the parent class upon change.
//
// CAVEATS/GOTCHAS:
//
//  1)  Because of the chunked nature of the file, it can never just be updated in place
//      on disk. It must be written to a new file, then swapped with the old one. That's
//      the safe way anyway. So one makes all the required changes to the in-memory
//      representation, then store it out all at once.
//
//  2)  We can use the MStreamable interface for the file level read/write since this is
//      our own format and we define all multibyte binary values on disc to be in little
//      endian, as our streaming system assumes. That means it will handle byte swapping
//      for us if we are on a big endian system. The actual data chunks we just read
//      and write as blobs and byte order doesn't come into play. The format of that data
//      is something only the client code understands. It may be flattened objects of our
//      own or a third party format like a PNG image or some such.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TChunkedFileChunk
// PREFIX: chflch
// ---------------------------------------------------------------------------
class CIDMDATAEXP TChunkedFileChunk : public TObject, public MDuplicable
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TChunkedFileChunk() = delete;

        ~TChunkedFileChunk();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bClearData();

        virtual tCIDLib::TBoolean bSetData
        (
            const   tCIDLib::TCard4         c4Bytes
            , const TMemBuf&                mbufData
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsDataChange() const
        {
            return m_bIsDataChange;
        }

        tCIDLib::TCard4 c4Bytes() const
        {
            return m_c4Bytes;
        }

        const TMemBuf& mbufData() const
        {
            return m_mbufData;
        }

        const TSHA1Hash& mhashChunk() const
        {
            return m_mhashChunk;
        }

        const TString& strId() const
        {
            return m_strId;
        }


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TChunkedFileChunk
        (
            const   TString&                strId
            , const tCIDLib::TBoolean       bIsDataChange
        );

        TChunkedFileChunk
        (
            const   TString&                strId
            , const tCIDLib::TCard4         c4Bytes
            , const TMemBuf&                mbufData
            , const tCIDLib::TBoolean       bIsDataChange

        );

        TChunkedFileChunk
        (
            const   TChunkedFileChunk&      chflchSrc
        );

        TChunkedFileChunk
        (
                    TChunkedFileChunk&&     chflchSrc
        );

        TChunkedFileChunk& operator=
        (
            const   TChunkedFileChunk&      chflchSrc
        );

        TChunkedFileChunk& operator=
        (
                    TChunkedFileChunk&&     chflchSrc
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bIsDataChange
        //      Indicates if this chunk should cause an update of the serial number and
        //      last change stamp when modified.
        //
        //  m_c4Bytes
        //      The number of bytes in the data buffer.
        //
        //  m_mbufData
        //      The raw data of this chunk.
        //
        //  m_mhashChunk
        //      The hash of the data. Initially either read in from the file, or set
        //      upon initial set of the data. It is updated any time the data is
        //      changed.
        //
        //  m_strId
        //      A chunk id, which indicates what it holds, and is used in error msgs and
        //      such.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bIsDataChange;
        tCIDLib::TCard4     m_c4Bytes;
        THeapBuf            m_mbufData;
        TSHA1Hash           m_mhashChunk;
        TString             m_strId;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TChunkedFileChunk, TObject)
};


// ---------------------------------------------------------------------------
//  CLASS: TChunkedFileData
// PREFIX: chflch
//
//  We don't need to override virtuals since we add nothing to the info that our
//  base class provides. We are just for type safety basically. Our data setter
//  method returns true if the data changed, since the main file class has to bump
//  the serial number/stamp on a data chunk change.
// ---------------------------------------------------------------------------
class CIDMDATAEXP TChunkedFileData : public TChunkedFileChunk
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TChunkedFileData();

        TChunkedFileData
        (
            const   tCIDLib::TCard4         c4Bytes
            , const TMemBuf&                mbufData
        );

        TChunkedFileData
        (
            const   TChunkedFileData&      chflchSrc
        );

        TChunkedFileData
        (
                    TChunkedFileData&&     chflchSrc
        );

        ~TChunkedFileData();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TChunkedFileData& operator=
        (
            const   TChunkedFileData&      chflchSrc
        );

        TChunkedFileData& operator=
        (
                    TChunkedFileData&&      chflchSrc
        );


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TChunkedFileData, TChunkedFileChunk)
        DefPolyDup(TChunkedFileData)
};



// ---------------------------------------------------------------------------
//  CLASS: TChunkedFileExt
// PREFIX: chflch
//
//  We just add an extension name value to the underlying chunk cass.
// ---------------------------------------------------------------------------
class CIDMDATAEXP TChunkedFileExt : public TChunkedFileChunk
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TChunkedFileExt
        (
            const   TString&                strId
            , const tCIDLib::TBoolean       bIsDataChange
        );

        TChunkedFileExt
        (
            const   TString&                strId
            , const tCIDLib::TBoolean       bIsDataChange
            , const tCIDLib::TCard4         c4Bytes
            , const TMemBuf&                mbufData
        );

        TChunkedFileExt
        (
            const   TChunkedFileExt&      chflchSrc
        );

        TChunkedFileExt
        (
                    TChunkedFileExt&&     chflchSrc
        );

        ~TChunkedFileExt();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TChunkedFileExt& operator=
        (
            const   TChunkedFileExt&      chflchSrc
        );

        TChunkedFileExt& operator=
        (
                    TChunkedFileExt&&     chflchSrc
        );


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TChunkedFileExt, TChunkedFileChunk)
        DefPolyDup(TChunkedFileExt)
};



// ---------------------------------------------------------------------------
//  CLASS: TChunkedFileMeta
// PREFIX: chflch
// ---------------------------------------------------------------------------
class CIDMDATAEXP TChunkedFileMeta : public TChunkedFileChunk
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TChunkedFileMeta();

        TChunkedFileMeta
        (
            const   tCIDLib::TKVPFList&     colValues
        );

        TChunkedFileMeta
        (
            const   TChunkedFileMeta&      chflchSrc
        );

        TChunkedFileMeta
        (
                    TChunkedFileMeta&&     chflchSrc
        );

        ~TChunkedFileMeta();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TChunkedFileMeta& operator=
        (
            const   TChunkedFileMeta&      chflchSrc
        );

        TChunkedFileMeta& operator=
        (
                    TChunkedFileMeta&&     chflchSrc
        );


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bClearData() override;

        tCIDLib::TBoolean bSetData
        (
            const   tCIDLib::TCard4         c4Bytes
            , const TMemBuf&                mbufData
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddKey
        (
            const   TString&                strKey
            , const TString&                strInitVal
            , const tCIDLib::TBoolean       bIsFileChange
        );

        tCIDLib::TVoid AddKeys
        (
            const   tCIDLib::TKVPFList&     colToAdd
        );

        tCIDLib::TBoolean bFindValue
        (
            const   TString&                strKey
            ,       TString&                strToFill
        )   const;

        tCIDLib::TBoolean bTestValue
        (
            const   TString&                strKey
            , const TString&                strCompare
            , const tCIDLib::TBoolean       bIfNotFound
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bValueEquals
        (
            const   TString&                strKey
            , const TString&                strCompare
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bRemoveAllKeys
        (
                    tCIDLib::TBoolean&      bFileChange
        );

        tCIDLib::TBoolean bRemoveKey
        (
            const   TString&                strKey
            ,       tCIDLib::TBoolean&      bFileChange
        );

        tCIDLib::TBoolean bSetValue
        (
            const   TString&                strKey
            , const TString&                strToSet
            ,       tCIDLib::TBoolean&      bFileChange
        );

        tCIDLib::TBoolean bSetValues
        (
            const   tCIDLib::TKVPList&      colValues
            ,       tCIDLib::TBoolean&      bFileChange
        );

        tCIDLib::TBoolean bSetValues
        (
            const   tCIDLib::TKVPFList&     colValues
            ,       tCIDLib::TBoolean&      bFileChange
        );

        tCIDLib::TCard4 c4EstBytes() const;

        tCIDLib::TCard4 c4KeyCount() const
        {
            return m_colValues.c4ElemCount();
        }

        const tCIDLib::TKVPFList& colValues() const
        {
            return m_colValues;
        }

        tCIDLib::TVoid QueryValues
        (
                    tCIDLib::TKVPFList&     colValues
        )   const;

        tCIDLib::TVoid QueryValues
        (
                    tCIDLib::TKVPList&      colValues
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid RegenRawData();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colValues
        //      A keyed hash set of key=value pairs. This is runtime only, it is not
        //      written out. These are flattened to a raw data format. That is stored
        //      on the base class like all the chunk types. We regenerate it on any
        //      change. A little overhead but not too bad and it keeps thing much
        //      simpler.
        //
        //      It's actually a key/value/flag pair, and the flag indicates whether it
        //      should cause an update to the serial number/last changed stamp if the
        //      value changes.
        // -------------------------------------------------------------------
        tCIDLib::TKVPFList  m_colValues;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TChunkedFileMeta, TChunkedFileChunk)
        DefPolyDup(TChunkedFileMeta)
};


// ---------------------------------------------------------------------------
//  CLASS: TChunkedFile
// PREFIX: chfl
// ---------------------------------------------------------------------------
class CIDMDATAEXP TChunkedFile : public TObject, public MStreamable
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bFindMetaKey
        (
            const   tCIDLib::TKVPFList&     colMetaVals
            , const TString&                strToFind
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::False
        );

        static tCIDLib::TCard4 c4FindMetaKey
        (
            const   tCIDLib::TKVPFList&     colMetaVals
            , const TString&                strToFind
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::False
        );

        static tCIDLib::TVoid ExtractHdrInfo
        (
                    TBinInStream&           strmSrc
            ,       tCIDLib::TCard4&        c4SerialNum
            ,       tCIDLib::TEncodedTime&  enctLastChange
            ,       tCIDLib::TCard4&        c4ChunkCnt
        );

        static tCIDLib::TVoid ExtractInfo
        (
                    TBinInStream&           strmSrc
            ,       tCIDLib::TCard4&        c4SerialNum
            ,       tCIDLib::TEncodedTime&  enctLastChange
            ,       tCIDLib::TKVPFList&     colMetaVals
            ,       tCIDLib::TStrList&      colChunkIds
            ,       tCIDLib::TCardList&     fcolChunkSzs
        );

        static tCIDLib::TVoid ExtractMetaInfo
        (
                    TBinInStream&           strmSrc
            ,       tCIDLib::TCard4&        c4SerialNum
            ,       tCIDLib::TEncodedTime&  enctLastChange
            ,       tCIDLib::TKVPFList&     colToFill
            ,       tCIDLib::TCard4&        c4ChunkCnt
        );


        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TChunkedFile();

        TChunkedFile
        (
            const   tCIDLib::TKVPFList&     colMeta
            , const tCIDLib::TCard4         c4Bytes
            , const TMemBuf&                mbufData
        );

        TChunkedFile
        (
            const   tCIDLib::TKVPFList&     colMeta
            , const tCIDLib::TCard4         c4Bytes
            , const TMemBuf&                mbufData
            , const TString&                strExtChunkId
            , const tCIDLib::TCard4         c4ExtBytes
            , const TMemBuf&                mbufExtData
            , const tCIDLib::TBoolean       bIsDataChange
        );

        TChunkedFile
        (
            const   TChunkedFile&           chflSrc
        );

        TChunkedFile
        (
                    TChunkedFile&&          chflSrc
        );

        ~TChunkedFile();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TChunkedFile& operator=
        (
            const   TChunkedFile&           chflSrc
        );

        TChunkedFile& operator=
        (
                    TChunkedFile&&          chflSrc
        );

        tCIDLib::TBoolean operator==
        (
            const   TChunkedFile&           chflSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TChunkedFile&           chflSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddExtChunk
        (
            const   TString&                strId
            , const tCIDLib::TBoolean       bIsDataChange
        );

        tCIDLib::TVoid AddExtChunk
        (
            const   TString&                strId
            , const tCIDLib::TCard4         c4Bytes
            , const TMemBuf&                mbufData
            , const tCIDLib::TBoolean       bIsDataChange
        );

        tCIDLib::TVoid AddMetaKey
        (
            const   TString&                strKey
            , const TString&                strInitVal
            , const tCIDLib::TBoolean       bIsFileChange
        );

        tCIDLib::TVoid AddMetaKeys
        (
            const   tCIDLib::TKVPFList&     colToAdd
        );

        tCIDLib::TBoolean bChunkChanged
        (
            const   TString&                strId
            , const TSHA1Hash&              mhashCompare
        )   const;

        tCIDLib::TBoolean bChunkExists
        (
            const   TString&                strId
        )   const;

        tCIDLib::TBoolean bFileChanged() const;

        tCIDLib::TBoolean bFindMetaValue(const  TString&    strKey
                                        ,       TString&    strToFill) const
        {
            return m_pchflchMeta->bFindValue(strKey, strToFill);
        }

        tCIDLib::TBoolean bMetaValueEquals
        (
            const  TString&                 strKey
            , const TString&                strToFill
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bRemoveMetaKey
        (
            const   TString&                strKey
            ,       tCIDLib::TBoolean&      bFileChange
        );

        tCIDLib::TBoolean bSetDataChunk
        (
            const   tCIDLib::TCard4         c4Bytes
            , const TMemBuf&                mbufToSet
        );

        tCIDLib::TBoolean bSetChunkById
        (
            const   TString&                strId
            , const tCIDLib::TCard4         c4Bytes
            , const TMemBuf&                mbufToSet
        );

        tCIDLib::TBoolean bSetMetaValue
        (
            const   TString&                strKey
            , const TString&                strToSet
            ,       tCIDLib::TBoolean&      bFileChange
        );

        tCIDLib::TBoolean bSetMetaValues
        (
            const   tCIDLib::TKVPList&      colValues
            ,       tCIDLib::TBoolean&      bFileChange
        );

        tCIDLib::TBoolean bSetMetaValues
        (
            const   tCIDLib::TKVPFList&     colValues
            ,       tCIDLib::TBoolean&      bFileChange
        );

        tCIDLib::TBoolean bTestMetaValue
        (
            const   TString&                strKey
            , const TString&                strCompare
            , const tCIDLib::TBoolean       bIfNotFound
            , const tCIDLib::TBoolean       bCaseSensitive = kCIDLib::False
        )   const;

        tCIDLib::TCard4 c4ChunkCount() const
        {
            return m_colChunks.c4ElemCount();
        }

        tCIDLib::TCard4 c4DataBytes() const
        {
            return m_colChunks[1]->c4Bytes();
        }

        tCIDLib::TCard4 c4DataBytesById
        (
            const   TString&                strId
        )   const;

        tCIDLib::TCard4 c4FullSzEstimate() const;

        tCIDLib::TCard4 c4MetaKeyCount() const
        {
            return m_pchflchMeta->c4KeyCount();
        }

        tCIDLib::TCard4 c4QueryChunkById
        (
            const   TString&                strChunkId
            ,       TMemBuf&                mbufData
        )   const;

        tCIDLib::TCard4 c4SerialNum() const
        {
            return m_c4SerialNum;
        }

        // A special one we can directly return
        const TChunkedFileMeta& chflchMeta() const
        {
            return *m_pchflchMeta;
        }

        const tCIDLib::TKVPFList& colMetaValues() const
        {
            return m_pchflchMeta->colValues();
        }

        tCIDLib::TVoid ClearAllData();

        tCIDLib::TVoid ClearChunkById
        (
            const   TString&                strId
        );

        tCIDLib::TVoid ClearDataChunk();

        tCIDLib::TEncodedTime enctLastChange() const
        {
            return m_enctLastChange;
        }

        const TMemBuf& mbufChunkAt
        (
            const   tCIDLib::TCard4         c4Index
            ,       TString&                strId
            ,       tCIDLib::TCard4&        c4Bytes
        )   const;

        const TMemBuf& mbufChunkById
        (
            const   TString&                strId
            ,       tCIDLib::TCard4&        c4Bytes
        )   const;

        const TMemBuf& mbufData() const
        {
            return m_colChunks[1]->mbufData();
        }

        tCIDLib::TVoid QueryChunkAt
        (
            const   tCIDLib::TCard4&        c4At
            ,       TString&                strChunkId
            ,       TMemBuf&                mbufData
            ,       tCIDLib::TCard4&        c4Bytes
        )   const;

        tCIDLib::TVoid QueryChunkSizes
        (
                    tCIDLib::TStrList&      colChunkIds
            ,       tCIDLib::TCardList&     fcolChunkSizes
        )   const;

        tCIDLib::TVoid Reset
        (
            const   tCIDLib::TCard4         c4SerialNum = 1
        );

        tCIDLib::TVoid Reset
        (
            const   tCIDLib::TCard4         c4Bytes
            , const TMemBuf&                mbufData
            , const tCIDLib::TKVPFList&     colMeta
            , const tCIDLib::TCard4         c4SerialNum = 1
        );


    protected :
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
        //  Private data types and constants
        //
        //  We need a by ref list of chunks to store them polymorphically. ANd we need
        //  a list of hashes to match.
        // -------------------------------------------------------------------
        using TChunkList = TRefVector<TChunkedFileChunk>;
        using THashList = TVector<TSHA1Hash>;
        static const tCIDLib::TCard4        c4MaxChunks = 512;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid MakeDefChunks
        (
            const   tCIDLib::TCard4         c4Bytes
            , const TMemBuf&                mbufData
            , const tCIDLib::TKVPFList&     colMeta
        );

        TChunkedFileChunk* pchflchFindById
        (
            const   TString&                strToFind
            , const tCIDLib::TBoolean       bThrowIfNot
        );

        const TChunkedFileChunk* pchflchFindById
        (
            const   TString&                strToFind
            , const tCIDLib::TBoolean       bThrowIfNot
        )   const;


        // -------------------------------------------------------------------
        //  Private, static methods
        // -------------------------------------------------------------------
        static TChunkedFileChunk* pchflchStreamInChunk
        (
                    TBinInStream&           strmSrc
            ,       tCIDLib::TCard4&        c4ChunkSz
        );

        static tCIDLib::TVoid StreamInHdr
        (
                    TBinInStream&           strmSrc
            ,       tCIDLib::TCard4&        c4SerialNum
            ,       tCIDLib::TEncodedTime&  enctLastChange
            ,       tCIDLib::TCard4&        c4ChunkCnt
        );

        static tCIDLib::TVoid StreamOutChunk
        (
                    TBinOutStream&          strmTar
            , const TChunkedFileChunk&      chflchSrc
        );

        static tCIDLib::TVoid StreamOutHdr
        (
                    TBinOutStream&          strmTar
            , const tCIDLib::TCard4         c4SerialNum
            , const tCIDLib::TEncodedTime   enctLastChange
            , const tCIDLib::TCard1         c1ChunkCnt
        );

        static tCIDLib::TVoid ThrowFmtErr
        (
            const   tCIDLib::TCh* const     pszAltText
            , const tCIDLib::TCard4         c4Line
            , const MFormattable&           mfmtblToken1 = MFormattable::Nul_MFormattable()
            , const MFormattable&           mfmtblToken2 = MFormattable::Nul_MFormattable()
            , const MFormattable&           mfmtblToken3 = MFormattable::Nul_MFormattable()
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4SerialNum
        //      The serial number from the file header. It is initialized to 1 for a
        //      new object and persisted forward after that as it increments.
        //
        //  m_colChunks
        //      We keep a list of chunks as they are read from the file. The first one
        //      is always meta, then data, then any optional extension chunks.
        //
        //  m_colLastHashes
        //      The last stored hashes for each chunk in m_colChunkList (or the originals
        //      we read in if they have not changed since then.)
        //
        //  m_enctLastChange
        //      The time of the last change to the data chunk.
        //
        //  m_pchflchMeta
        //      For convenience we keep a separate typed pointer to the meta chunk, since
        //      it is specifically referenced in various methods.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4SerialNum;
        TChunkList              m_colChunks;
        mutable THashList       m_colLastHashes;
        tCIDLib::TEncodedTime   m_enctLastChange;
        TChunkedFileMeta*       m_pchflchMeta;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TChunkedFile, TObject)
};


#pragma CIDLIB_POPPACK

