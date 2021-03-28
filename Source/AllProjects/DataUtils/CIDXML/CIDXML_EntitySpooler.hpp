//
// FILE NAME: CIDXML_EntitySpooler.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/20/1999
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
//  This is the header file for the CIDXML_EntitySpooler.Cpp file, which
//  implements the TXMLEntSpooler class. Each XML entity is represented
//  by a spooler which is responsible for internalizing that entity's text
//  and buffering it in reasonable chunks.
//
//  The spooler also has specialized methods for getting the next internalized
//  char from the entity, skipping the next char if it meets some criteria,
//  skipping over chars if they match some string, storing attributes of
//  the entity (such as external, parameter entity, referenced inside a
//  literal, etc...), and tracking the current line/col information for
//  error reporting.
//
//  Each entity is in some encoding. So the entity spooler must transcode
//  the text of the entity into Unicode for internal processing. We use the
//  CIDEncode facility's transcoding services to do this work.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


class TXMLEntityMgr;


// ---------------------------------------------------------------------------
//  CLASS: TXMLEntSpooler
// PREFIX: xesp
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLEntSpooler : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDXML::EBaseEncodings eProbeBaseEncoding
        (
            const   tCIDLib::TCard1* const  pc1Buffer
            , const tCIDLib::TCard4         c4Count
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TXMLEntSpooler
        (
            const   TString&                strSystemId
            ,       TBinInStream* const     pstrmToAdopt
            , const TString&                strEncoding
            , const tCIDXML::ERefFrom       eFrom
            , const tCIDXML::EEntityTypes   eType
            , const tCIDLib::TBoolean       bIgnoreBadChars
        );

        TXMLEntSpooler
        (
            const   TString&                strSystemId
            ,       TBinInStream* const     pstrmToAdopt
            ,       TTextConverter* const   ptcvtToAdopt
            , const tCIDXML::ERefFrom       eFrom
            , const tCIDXML::EEntityTypes   eType
            , const tCIDLib::TBoolean       bIgnoreBadChars
        );

        TXMLEntSpooler(const TXMLEntSpooler&) = delete;
        TXMLEntSpooler(TXMLEntSpooler&&) = delete;

        ~TXMLEntSpooler();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TXMLEntSpooler& operator=(const TXMLEntSpooler&) = delete;
        TXMLEntSpooler& operator=(TXMLEntSpooler&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCharBufEmpty() const;

        tCIDLib::TBoolean bEmpty() const;

        tCIDLib::TBoolean bFromLiteral() const;

        tCIDLib::TBoolean bFromLiteral
        (
            const   tCIDLib::TBoolean       bState
        );

        tCIDLib::TBoolean bGetName
        (
                    TString&                strToFill
            , const tCIDLib::TBoolean       bTokenStyle
        );

        tCIDLib::TBoolean bGetNext
        (
                    tCIDLib::TCh&           chToFill
        );

        tCIDLib::TBoolean bGetNextIfNot
        (
                    tCIDLib::TCh&           chToFill
            , const tCIDLib::TCh            chNotToGet
        );

        tCIDLib::TBoolean bGetSpaces
        (
                    TString&                strToFill
        );

        tCIDLib::TBoolean bInterned() const;

        tCIDLib::TBoolean bInterned
        (
            const   tCIDLib::TBoolean       bState
        );

        tCIDLib::TBoolean bPeekAhead
        (
                    tCIDLib::TCh&           chToFill
            , const tCIDLib::TCard4         c4AheadBy
        );

        tCIDLib::TBoolean bPeekNext
        (
                    tCIDLib::TCh&           chToFill
        );

        tCIDLib::TBoolean bSkippedChar
        (
            const   tCIDLib::TCh            chToSkip
        );

        tCIDLib::TBoolean bSkippedQuote
        (
            COP     tCIDLib::TCh&           chSkipped
        );

        tCIDLib::TBoolean bSkippedSpace();

        tCIDLib::TBoolean bSkippedString
        (
            const   tCIDLib::TCh* const     pszToSkip
        );

        tCIDLib::TBoolean bSkippedString
        (
            const   TString&                strToSkip
        );

        tCIDLib::TBoolean bSkipSpaces
        (
                    tCIDLib::TBoolean&      bSkippedSome
        );

        tCIDLib::TCard4 c4CurColumn() const;

        tCIDLib::TCard4 c4CurLine() const;

        tCIDLib::TCard4 c4SpoolerId() const;

        const TString& strSystemId() const;

        tCIDLib::TVoid SetDeclEncoding
        (
            const   TString&                strDeclEncoding
        );


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TXMLEntityMgr;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bReloadCharBuf();

        tCIDLib::TVoid DecodeDecl
        (
            const   tCIDXML::EBaseEncodings eBaseEncoding
        );

        tCIDLib::TVoid ReloadRawData();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_ac1RawBuf
        //      This is a pointer to the raw spool buffer. Large blocks of
        //      data are buffered into this member and transcoded in smaller
        //      blocks as required into m_ac1CharBuf. The number of bytes in
        //      this buffer is in m_c4RawBufCount and the current index into
        //      it is in m_c4RawBufInd;
        //
        //  m_achCharBuf
        //      This is the buffer of internalized data. The larger raw data
        //      buffer is block transcoded into this buffer in smaller blocks.
        //      The number of chars currently in the buffer is in the
        //      m_c4CharBufCount member, and the current offset into it is
        //      in m_c4CharBufInd.
        //
        //  m_bDataExhausted
        //      When we do a raw data buffer reload that fails to get any
        //      more data, this is set. This lets us short circuit some checks
        //      later when the next attempt is made to get more chars. We know
        //      then that there aren't any more and that this entity is done.
        //
        //  m_bEmpty
        //      During the initial decode, if we cannot read any bytes. This
        //      is set. This allows the higher level logic to short circuit
        //      a lot of work.
        //
        //  m_bEncodingForced
        //      This is set if the encoding was forced during the ctor. In
        //      that case, any autosensing is skipped.
        //
        //  m_bIgnoreBadChars
        //      We are told whether the entity spoolers we create should
        //      ignore bad chars (use the replacement character feature of
        //      the text transcoders) or not.
        //
        //  m_bInterned
        //      This entity has already been internalized, so the new line
        //      normalization has already been done. This is set by the
        //      outside world after creation.
        //
        //  m_bRefFromLiteral
        //      The entity that this spooler is working on was referenced
        //      from within a literal string.
        //
        //  m_bSpaceAdded
        //      This is used as a one shot flag to fault in the trailing
        //      space when a PE is referenced from outside a literal.
        //
        //  m_c4CharBufCount
        //  m_c4CharBufInd
        //      These are the current size and current index into the char
        //      buffer, m_ac1CharBuf.
        //
        //  m_c4CurColumn
        //  m_c4CurLine
        //      These are the current row and column values that are kept
        //      up to date as CR/LF chars are spooled out of the buffer (or
        //      when they are ungotten.)
        //
        //  m_c4SpoolerId
        //      This is the unique id given to this entity spooler. This is
        //      done during ctor.
        //
        //  m_c4RawBufCount
        //  m_c4RawBufInd
        //      These are the current size and current index into the raw
        //      data buffer, m_ac1RawBuf.
        //
        //  m_eType
        //      This is the type of entity. We are particular interested when
        //      its a parameter entity, since we have to fault in a leading
        //      and trailing space if its not referenced from within a literal.
        //
        //  m_pcvtDecode
        //      This is the CIDEncode transcoder that we use to decode the
        //      text.
        //
        //  m_pstrmSrc
        //      This is the source stream that has the raw entity data that
        //      we are to spool. It was adopted in the ctor, so we destroy it
        //      when we are destroyed.
        //
        //  m_strEncoding
        //      The name of the encoding that was either auto sensed, forced
        //      by client code, or found in the encoding="" string.
        //
        //  m_strSystemId
        //      The system id that was passed into the ctor. This is used for
        //      error reporting purposes, and its also used as the base for
        //      any relative system ids.
        // -------------------------------------------------------------------
        tCIDLib::TCh            m_achCharBuf[kCIDXML::c4CharBufSize];
        tCIDLib::TCard1         m_ac1RawBuf[kCIDXML::c4RawBufSize];
        tCIDLib::TBoolean       m_bDataExhausted;
        tCIDLib::TBoolean       m_bEmpty;
        tCIDLib::TBoolean       m_bEncodingForced;
        tCIDLib::TBoolean       m_bIgnoreBadChars;
        tCIDLib::TBoolean       m_bInterned;
        tCIDLib::TBoolean       m_bRefFromLiteral;
        tCIDLib::TBoolean       m_bSpaceAdded;
        tCIDLib::TCard4         m_c4CharBufCount;
        tCIDLib::TCard4         m_c4CharBufInd;
        tCIDLib::TCard4         m_c4CurColumn;
        tCIDLib::TCard4         m_c4CurLine;
        tCIDLib::TCard4         m_c4RawBufCount;
        tCIDLib::TCard4         m_c4RawBufInd;
        tCIDLib::TCard4         m_c4SpoolerId;
        tCIDXML::EEntityTypes   m_eType;
        TTextConverter*         m_ptcvtDecode;
        TBinInStream*           m_pstrmSrc;
        TString                 m_strEncoding;
        TString                 m_strSystemId;



        // -------------------------------------------------------------------
        //  Private static data members
        //
        //  s_scntNextSerialNum
        //      Each new entity spooler is given a unique id, as a means of
        //      tracking them for "partial markup" errors. This is a thread
        //      safe counter which is necessary because this one counter is
        //      used for all entities used by all parser instances (which
        //      could all be in separate threads.)
        // -------------------------------------------------------------------
        static TSafeCard4Counter    s_scntNextSerialNum;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXMLEntSpooler,TObject)
};

#pragma CIDLIB_POPPACK

