//
// FILE NAME: CIDLib_Console.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/10/1996
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
//  This file implements the TConsole class, a specialized stream for
//  getting interactive input from the console, though it can be redirectable
//  also.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TConsoleInImpl, TInStreamImpl)
RTTIDecls(TConsoleOutImpl, TOutStreamImpl)
RTTIDecls(TInConsole, TInTextStream)
RTTIDecls(TOutConsole, TOutTextStream)
RTTIDecls(TConPager, TObject)



// ---------------------------------------------------------------------------
//   CLASS: TConPager
//  PREFIX: conp
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TConPager: Constuctors and Destructor
// ---------------------------------------------------------------------------
TConPager::TConPager(           TInConsole* const   pconIn
                        ,       TOutConsole* const  pconOut
                        , const TString&            strPrompt) :

    m_bBreakSeen(kCIDLib::False)
    , m_c4CurLine(0)
    , m_c4WndSize(0)
    , m_pconIn(pconIn)
    , m_pconOut(pconOut)
    , m_strPrompt(strPrompt)
{
    // Do an initial reset to get us ready
    Reset();
}


TConPager::~TConPager()
{
}


// ---------------------------------------------------------------------------
//  TConPager: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TConPager::operator++(int)
{
    tCIDLib::EConKeys   eKey = tCIDLib::EConKeys::Null;

    //
    //  If the current line is at the end of the window then we set the
    //  current line count to zero to get a whole new page's worth and put
    //  out the prompt and wait for a key press. Else, just bump up the
    //  current line and return that.
    //
    if (m_c4CurLine + 1 == m_c4WndSize)
    {
        *m_pconOut << m_strPrompt << kCIDLib::FlushIt;
        m_pconIn->chReadChar(eKey, kCIDLib::c4MaxCard);

        // Erase the prompt and put the cursor back at the start of the line
        *m_pconOut  << L"\r"
                    <<  TTextOutStream::RepChars
                        (
                            kCIDLib::chSpace
                            , m_strPrompt.c4Length()
                        )
                    << L"\r" << kCIDLib::FlushIt;

        // If it was a Break, then set the break flag
        if (eKey == tCIDLib::EConKeys::Break)
            m_bBreakSeen = kCIDLib::True;

        //
        //  Reset the line counter and update the window size, in case they
        //  resized it while we were waiting for input.
        //
        m_c4CurLine = 0;
        m_c4WndSize = m_pconOut->c4QueryVerticalLines();
    }
     else
    {
        m_c4CurLine++;
    }
    return m_c4CurLine;
}


// ---------------------------------------------------------------------------
//  TConPager: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TConPager::bBreakSeen()
{
    // Reset the current value and reset it
    const tCIDLib::TBoolean bRet = m_bBreakSeen;
    m_bBreakSeen = kCIDLib::False;
    return bRet;
}


tCIDLib::TVoid TConPager::Reset()
{
    // Re-initialize our two console line values
    m_c4WndSize = m_pconOut->c4QueryVerticalLines();
    m_c4CurLine = 0;
}



// ---------------------------------------------------------------------------
//   CLASS: TConsoleInImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TConsoleInImpl: Constructors and Destructor
// ---------------------------------------------------------------------------
TConsoleInImpl::TConsoleInImpl( const   tCIDLib::TBoolean   bInsertMode
                                , const tCIDLib::TCard4     c4MaxRecall) :

    m_c8FauxPos(0)
    , m_kconImpl(bInsertMode, c4MaxRecall)
{
    if (!m_kconImpl.bOpen())
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcCon_Open
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}

TConsoleInImpl::~TConsoleInImpl()
{
}


// ---------------------------------------------------------------------------
//  TConsoleInImpl: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TConsoleInImpl::bIsOpen() const
{
    return m_kconImpl.bIsOpen();
}


tCIDLib::TCard4
TConsoleInImpl::c4ReadBytes(        tCIDLib::TVoid* const   pToFill
                            , const tCIDLib::TCard4         c4BytesToRead)
{
    //
    //  Do a sanity check that we are being asked to read an even multiple
    //  the bytes per char. A console text stream should always have a
    //  native wide char converter installed, so this should always be true.
    //
    #if CID_DEBUG_ON
    if (c4BytesToRead % kCIDLib::c4CharBytes)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_OddByteCount
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , TCardinal(c4BytesToRead)
        );
    }
    #endif

    //
    //  How many wide chars can we fit into the passed buffer? This is
    //  how many chars we will provide as the max chars to read.
    //
    const tCIDLib::TCard4 c4MaxChars = c4BytesToRead / kCIDLib::c4CharBytes;
    if (!c4MaxChars)
        return 0;

    tCIDLib::TCard4 c4Len = 0;

    if (!m_kconImpl.bReadLine(static_cast<tCIDLib::TCh*>(pToFill), c4MaxChars, c4Len))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcCon_TextReadError
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Convert chars to bytes
    const tCIDLib::TCard4 c4Bytes = c4Len * kCIDLib::c4CharBytes;

    // Add the bytes to our faux position and return it
    m_c8FauxPos += c4Bytes;
    return c4Bytes;
}


tCIDLib::TVoid TConsoleInImpl::Reset()
{
    // This is a no-op for this class
}


tCIDLib::TVoid TConsoleInImpl::SkipForwardBy(const tCIDLib::TCard4 c4SkipBy)
{
    //
    //  This is pretty useless for this type of stream, so just add the value
    //  to our faux position.
    //
    m_c8FauxPos += c4SkipBy;
}


// ---------------------------------------------------------------------------
//  TConsoleInImpl: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TCard4
TConsoleInImpl::c4ReadLine(         TString&            strToFill
                            ,       TKrnlConIn::TIdleCB pfnCallback
                            ,       TObject* const      pobjCBData)
{
    const tCIDLib::TCard4 c4MaxChars = 4095;
    tCIDLib::TCh achBuf[c4MaxChars + 1];

    tCIDLib::TCard4 c4Read;
    if (!m_kconImpl.bReadLine(achBuf, c4MaxChars, c4Read, pfnCallback, pobjCBData))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcCon_ReadNext
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Update our faux position if we got input
    if (c4Read)
    {
        m_c8FauxPos += (c4Read * kCIDLib::c4CharBytes);
        strToFill = achBuf;
        strToFill.StripWhitespace();
    }
     else
    {
        strToFill.Clear();
    }
    return c4Read;
}


tCIDLib::TCh
TConsoleInImpl::chReadChar(         tCIDLib::EConKeys&  eKeyType
                            , const tCIDLib::TCard4     c4WaitMS
                            ,       TKrnlConIn::TIdleCB pfnCallback
                            ,       TObject* const      pobjCBData)
{
    tCIDLib::TCh chKey;
    if (!m_kconImpl.bReadCharMS(eKeyType, chKey, c4WaitMS, pfnCallback, pobjCBData))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcCon_ReadNext
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Update our faux position if we got a char
    if (chKey)
        m_c8FauxPos += kCIDLib::c4CharBytes;
    return chKey;
}


tCIDLib::TVoid TConsoleInImpl::ResetRecallBuffer()
{
    m_kconImpl.ResetRecallBuf();
}



// ---------------------------------------------------------------------------
//   CLASS: TConsoleOutImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TConsoleOutImpl: Constructors and Destructor
// ---------------------------------------------------------------------------
TConsoleOutImpl::TConsoleOutImpl() :

    m_c8FauxPos(0)
{
    if (!m_kconImpl.bOpen())
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcCon_Open
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}

TConsoleOutImpl::~TConsoleOutImpl()
{
}


// ---------------------------------------------------------------------------
//  TConsoleOutImpl: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TConsoleOutImpl::bBackspace()
{
    return m_kconImpl.bBackspace();
}


tCIDLib::TBoolean TConsoleOutImpl::bIsOpen() const
{
    return m_kconImpl.bIsOpen();
}


tCIDLib::TCard4
TConsoleOutImpl::c4WriteBytes(  const   tCIDLib::TVoid* const   pToWrite
                                , const tCIDLib::TCard4         c4BytesToWrite)
{
    //
    //  Convert the bytes to write to chars to write, we assume that console
    //  based text streams will always have a native wide converter, so this
    //  should always be safe. In debug mode, test it to make sure its an
    //  even multiple of the native wide char size.
    //
    #if CID_DEBUG_ON
    if (c4BytesToWrite % kCIDLib::c4CharBytes)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_OddByteCount
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , TCardinal(c4BytesToWrite)
        );
    }
    #endif

    tCIDLib::TCard4 c4Written;
    if (!m_kconImpl.bWriteBytes(pToWrite, c4BytesToWrite, c4Written))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcCon_TextWriteError
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Keep our faux position up to date
    m_c8FauxPos += c4Written;

    return c4Written;
}


tCIDLib::TVoid TConsoleOutImpl::Reset()
{
    // This is a no-op for a console
}


tCIDLib::TVoid TConsoleOutImpl::SeekToEnd()
{
    // This is a no-op for a console
}



// ---------------------------------------------------------------------------
//  TConsoleOutImpl: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TConsoleOutImpl::c4QueryVerticalLines() const
{
    tCIDLib::TCard4 c4VertLines;
    if (!m_kconImpl.bQueryVerticalLines(c4VertLines))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcCon_QueryVertLines
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return c4VertLines;
}


tCIDLib::TVoid TConsoleOutImpl::ClearScr()
{
    if (!m_kconImpl.bClearScr())
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcCon_ClearScr
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}



// ---------------------------------------------------------------------------
//   CLASS: TInConsole
//  PREFIX: cons
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TInConsole: Constructors and Destructor
// ---------------------------------------------------------------------------
TInConsole::TInConsole( const   tCIDLib::TBoolean   bInsertMode
                        , const tCIDLib::TCard4     c4MaxRecall) :

    TTextInStream(new TNativeWCConverter)
    , m_pstrmiCon(0)
{
    // Create a stream input implementation object
    m_pstrmiCon = new TConsoleInImpl(bInsertMode, c4MaxRecall);

    //
    //  Create a binary stream and give it this implementation. Give it the
    //  console impl object. Then set this object on our parent stream.
    //
    TBinInStream* pstrmCon = new TBinInStream(m_pstrmiCon);
    AdoptStream(pstrmCon);
}

TInConsole::~TInConsole()
{
}


// ---------------------------------------------------------------------------
//  TInConsole: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TCard8 TConsoleInImpl::c8CurPos() const
{
    return m_c8FauxPos;
}


// ---------------------------------------------------------------------------
//  TInConsole: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TInConsole::c4ReadLine(         TString&            strToFill
                        ,       TKrnlConIn::TIdleCB pfnCallback
                        ,       TObject* const      pobjCBData)
{
    return m_pstrmiCon->c4ReadLine(strToFill, pfnCallback, pobjCBData);
}


tCIDLib::TCh
TInConsole::chReadChar(         tCIDLib::EConKeys&  eKeyType
                        , const tCIDLib::TCard4     c4WaitMS
                        ,       TKrnlConIn::TIdleCB pfnCallback
                        ,       TObject* const      pobjCBData)
{
    return m_pstrmiCon->chReadChar
    (
        eKeyType, c4WaitMS, pfnCallback, pobjCBData
);
}


tCIDLib::TVoid TInConsole::ResetRecallBuffer()
{
    m_pstrmiCon->ResetRecallBuffer();
}


tCIDLib::TVoid TInConsole::WaitKey()
{
    // Just call the impl, indicating max wait time, and eat the result
    tCIDLib::EConKeys eKeyType;
    m_pstrmiCon->chReadChar(eKeyType, kCIDLib::c4MaxWait, 0, 0);
}



// ---------------------------------------------------------------------------
//   CLASS: TOutConsole
//  PREFIX: cons
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TOutConsole: Constructors and Destructor
// ---------------------------------------------------------------------------
TOutConsole::TOutConsole() :

    TTextOutStream(new TNativeWCConverter)
    , m_pstrmiCon(0)
{
    //
    //  Create a stream input implementation object. We save a copy for our
    //  own quick access, but we don't own it. Its owned by the binary stream
    //  we give it to below.
    //
    m_pstrmiCon = new TConsoleOutImpl;

    //
    //  Create a binary stream and give it this implementation. Give it the
    //  console impl object. Then set this object on our parent stream.
    //
    TBinOutStream* pstrmCon = new TBinOutStream(m_pstrmiCon);
    AdoptStream(pstrmCon);
}

TOutConsole::~TOutConsole()
{
}


// ---------------------------------------------------------------------------
//  TOutConsole: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TCard8 TConsoleOutImpl::c8CurPos() const
{
    return m_c8FauxPos;
}


// ---------------------------------------------------------------------------
//  TOutConsole: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TOutConsole::bBackspace()
{
    return m_pstrmiCon->bBackspace();
}


tCIDLib::TCard4 TOutConsole::c4QueryVerticalLines() const
{
    return m_pstrmiCon->c4QueryVerticalLines();
}

tCIDLib::TVoid TOutConsole::ClearScr()
{
    m_pstrmiCon->ClearScr();
}


