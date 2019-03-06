//
// FILE NAME: CIDBuild_LineSpooler.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/22/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDBuild_LineSpooler.Cpp file. This file
//  implements the TLineSpooler class, which is a simple wrapper for a
//  file stream that deals with tracking line numbers, stripping leading
//  whitespace, etc...
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TLineSpooler
//  PREFIX: lspl
// ---------------------------------------------------------------------------
class TLineSpooler
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TLineSpooler
        (
            const   TBldStr&                strFileName
            , const tCIDLib::TBoolean       bStripSpace = kCIDLib::True
        );

        ~TLineSpooler();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDisableMacros
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TBoolean bEndOfFile() const;

        tCIDLib::TBoolean bReadLine
        (
                    TBldStr&                strToFill
        );

        tCIDLib::TCh chGetNext();

        tCIDLib::TVoid Reset();

        const TBldStr& strFileName() const;

        tCIDLib::TCard4 c4CurLine() const;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ExpandKeywords
        (
                    TBldStr&                strToFill
        );

        tCIDLib::TVoid GetLine
        (
                    TBldStr&                strToFill
        );

        tCIDLib::TCh* pchAllocReadBuf
        (
                    tCIDLib::TCard4&        c4CharsRead
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bDisableMacros
        //      When we use the spooler to do things like scan headers for
        //      dependencies, we don't want to interpret $(x) type patterns as
        //      our macros, so we have to be able to turn off macro expansion.
        //
        //  m_bStripSpace
        //      When used to read in .Projects files, its nice to have this
        //      guy handle stripping out all whitespace.
        //
        //  m_pchReadBuf
        //      The buffer into which the file is read and spooled from. The
        //      text is converted to Unicode format before being stored here
        //      so we always are dealing with Unicode. The actual storage
        //      format for each character is in the native wide character
        //      format.
        //
        //  m_strFileName
        //      This is the name of the file that is being spooled. Just
        //      kept for error reporting reasons.
        //
        //  m_c4BufSize
        //      The size of the read buffer, i.e. the number of bytes in
        //      the file that is being spooled.
        //
        //  m_c4CurLine
        //      The current line that just got extracted from the file.
        //
        //  m_c4CurOfs
        //      The current offset into the read buffer.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bDisableMacros;
        tCIDLib::TBoolean   m_bStripSpace;
        tCIDLib::TCh*       m_pchReadBuf;
        TBldStr             m_strFileName;
        tCIDLib::TCard4     m_c4BufSize;
        tCIDLib::TCard4     m_c4CurLine;
        tCIDLib::TCard4     m_c4CurOfs;
};


// ---------------------------------------------------------------------------
//  TLineSpooler: Public, non-virtual methods
// ---------------------------------------------------------------------------
inline tCIDLib::TBoolean
TLineSpooler::bDisableMacros(const tCIDLib::TBoolean bToSet)
{
    m_bDisableMacros = bToSet;
    return m_bDisableMacros;
}

inline const TBldStr& TLineSpooler::strFileName() const
{
    return m_strFileName;
}

inline tCIDLib::TCard4 TLineSpooler::c4CurLine() const
{
    return m_c4CurLine;
}

