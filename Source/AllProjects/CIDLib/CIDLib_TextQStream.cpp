//
// FILE NAME: CIDLib_TextQStream.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/09/2002
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
//  This file implements our queue based stream implementation class and
//  text stream wrapper class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TQOutStreamImpl,TOutStreamImpl)
RTTIDecls(TTextQOutStream,TTextOutStream)



// ---------------------------------------------------------------------------
//   CLASS: TQOutStreamImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TQOutStreamImpl: Constructors and Destructor
// ---------------------------------------------------------------------------
TQOutStreamImpl::TQOutStreamImpl(const  tCIDLib::TCard4     c4MaxLines
                                , const tCIDLib::EReadModes eMode) :
    m_apstrLines(0)
    , m_c4MaxLines(c4MaxLines)
    , m_c4BaseIndex(0)
    , m_c4MsgIndex(0)
    , m_eMode(eMode)
    , m_c8CurPos(0)
    , m_strCurLine()
{
    // We have to have a non-zero size
    if (!c4MaxLines)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_ZeroBufSize
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    // Allocate the buffer and initialize the pointers
    m_apstrLines = new TString*[c4MaxLines];
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4MaxLines; c4Index++)
        m_apstrLines[c4Index] = 0;
}

TQOutStreamImpl::~TQOutStreamImpl()
{
    //
    //  Clean up the individual strings and then the array itself. Any not
    //  used will be zero so delete won't do anything.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4MaxLines; c4Index++)
        delete m_apstrLines[c4Index];
    delete [] m_apstrLines;
}


// ---------------------------------------------------------------------------
//  TQOutStreamImpl: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TQOutStreamImpl::bIsOpen() const
{
    // We are always 'open'
    return kCIDLib::True;
}


tCIDLib::TCard4
TQOutStreamImpl::c4WriteBytes(  const   tCIDLib::TVoid* const   pBuffer
                                , const tCIDLib::TCard4         c4BytesToWrite)
{
    // Lock and process the new data
    TLocker lockrQ(&m_mtxSync);

    //
    //  We will transcode buffers full of chars from the passed generic
    //  buffer until we process them all. We just a local buffer to do
    //  chunks of chars at a time.
    //
    const tCIDLib::TCard4   c4BufSz = 2047;
    tCIDLib::TCh            achBuf[c4BufSz + 1];
    tCIDLib::TCard4         c4CurChars = 0;
    tCIDLib::TCard4         c4CurIndex = 0;
    const tCIDLib::TCard1*  pc1Src = static_cast<const tCIDLib::TCard1*>(pBuffer);

    //
    //  If we have any partial line content from the last flush, then we'll
    //  naturally pick up from there and finish it.
    //
    tCIDLib::TCard4 c4BytesSoFar = 0;
    while (c4BytesSoFar < c4BytesToWrite)
    {
        // Do a chunk of chars
        const tCIDLib::TCard4 c4CurBytes = m_tcvtRead.c4ConvertFrom
        (
            pc1Src
            , c4BytesToWrite - c4BytesSoFar
            , achBuf
            , c4BufSz
            , c4CurChars
        );

        // If we didn't get any chars converted, then break out
        if (!c4CurBytes)
            break;

        // Update the bytes so far
        c4BytesSoFar += c4CurBytes;

        // Now loop through the chars we got this time
        for (c4CurIndex = 0; c4CurIndex < c4CurChars; c4CurIndex++)
        {
            tCIDLib::TCh chCur = achBuf[c4CurIndex];

            //
            //  Watch for new lines. When we get one, we put the current
            //  string into the queue and flush it for the next round.
            //
            if (chCur == kCIDLib::chLF)
            {
                AddNewLine(m_strCurLine);

                // And clear it for the next round
                m_strCurLine.Clear();
            }
             else
            {
                m_strCurLine.Append(chCur);
            }
        }
    }

    //
    //  Indicate how many bytes we wrote. It might not be all of them if
    //  they flushed mid-character.
    //
    return c4BytesSoFar;
}


tCIDLib::TCard8 TQOutStreamImpl::c8CurPos() const
{
    // Just return our dummy value
    TLocker lockrQ(&m_mtxSync);
    return m_c8CurPos;
}


tCIDLib::TVoid TQOutStreamImpl::Reset()
{
    // Just reset our dummy position value
    TLocker lockrQ(&m_mtxSync);
    m_c8CurPos = 0;
}


tCIDLib::TVoid TQOutStreamImpl::SeekToEnd()
{
    //
    //  This is a no-op for us. We don't have to support an input stream,
    //  so this won't get called anyway. Output streams are always at the
    //  end.
    //
}


// ---------------------------------------------------------------------------
//  TQOutStreamImpl: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TQOutStreamImpl::bQueryNewMsgs(         tCIDLib::TCard4&        c4MsgIndex
                                ,       tCIDLib::TStrCollect&   colToFill
                                , const tCIDLib::TBoolean       bAddNewLine)
{
    TLocker lockrQ(&m_mtxSync);

    // Calc the current last index
    const tCIDLib::TCard4 c4LastIndex = m_c4BaseIndex + m_c4MsgIndex;

    //
    //  If in destructive mode, then we don't care much what the passed msg
    //  index is. Just set it to the first available message, so that we
    //  get all new messages.
    //
    if (m_eMode == tCIDLib::EReadModes::Destructive)
        c4MsgIndex = 0;

    // If their message index isn't up to date, then give them the new ones
    const tCIDLib::TBoolean bRet = (c4MsgIndex != c4LastIndex);
    if (bRet)
    {
        colToFill.RemoveAll();

        //
        //  If the passed index is below our base index, then we cannot
        //  provide all of the elements he wants, so adjust the actual
        //  index. Else, subtract the base to get the actual.
        //
        tCIDLib::TCard4 c4Index;
        if (c4MsgIndex < m_c4BaseIndex)
            c4Index = 0;
        else if (c4MsgIndex < c4LastIndex)
            c4Index = m_c4MaxLines - (c4LastIndex - c4MsgIndex);
        else
            c4Index = m_c4MaxLines;

        // Copy the ones they don't have yet
        for (; c4Index < m_c4MaxLines; c4Index++)
        {
            TString& strNew = colToFill.objAdd(*m_apstrLines[c4Index]);
            if (bAddNewLine)
                strNew.Append(L"\n");
        }

        // Update the caller's idnex
        c4MsgIndex = c4LastIndex;
    }

    // If we are in destructive read mode, then reset the indexes
    if (m_eMode == tCIDLib::EReadModes::Destructive)
    {
        m_c4BaseIndex = 0;
        m_c4MsgIndex = 0;
        c4MsgIndex = 0;
    }

    return bRet;
}


tCIDLib::TBoolean
TQOutStreamImpl::bQueryNewMsgs( tCIDLib::TCard4&    c4MsgIndex
                                , TString&          strToFill
                                , tCIDLib::TCard4&  c4LineCount)
{
    TLocker lockrQ(&m_mtxSync);

    // Calc the current last index
    const tCIDLib::TCard4 c4LastIndex = m_c4BaseIndex + m_c4MsgIndex;

    //
    //  If in destructive mode, then we don't care much what the passed msg
    //  index is. Just set it to the first available message, so that we
    //  get all new messages.
    //
    if (m_eMode == tCIDLib::EReadModes::Destructive)
        c4MsgIndex = 0;

    // If their message index isn't up to date, then give them the new ones
    const tCIDLib::TBoolean bRet = (c4MsgIndex != c4LastIndex);
    if (bRet)
    {
        strToFill.Clear();
        c4LineCount = 0;

        //
        //  If the passed index is below our base index, then we cannot
        //  provide all of the elements he wants, so adjust the actual
        //  index. Else, subtract the base to get the actual.
        //
        tCIDLib::TCard4 c4Index;
        if (c4MsgIndex < m_c4BaseIndex)
            c4Index = 0;
        else if (c4MsgIndex < c4LastIndex)
            c4Index = m_c4MaxLines - (c4LastIndex - c4MsgIndex);
        else
            c4Index = m_c4MaxLines;

        // Copy the ones they don't have yet
        for (; c4Index < m_c4MaxLines; c4Index++)
        {
            strToFill.Append(*m_apstrLines[c4Index]);
            strToFill.Append(L"\n");
            c4LineCount++;
        }

        // Update the caller's idnex
        c4MsgIndex = c4LastIndex;
    }

    // If we are in destructive read mode, then reset the indexes
    if (m_eMode == tCIDLib::EReadModes::Destructive)
    {
        m_c4BaseIndex = 0;
        m_c4MsgIndex = 0;
        c4MsgIndex = 0;
    }

    return bRet;
}


tCIDLib::TCard4 TQOutStreamImpl::c4CurMsgIndex() const
{
    TLocker lockrQ(&m_mtxSync);

    // Add the current index to the base index to get the whole thing
    return m_c4BaseIndex + m_c4MsgIndex;
}


tCIDLib::TCard4 TQOutStreamImpl::c4LineCount() const
{
    return m_c4MsgIndex;
}



// ---------------------------------------------------------------------------
//  TQOutStreamImpl: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TQOutStreamImpl::AddNewLine(const TString& strToAdd)
{
    //
    //  Remember the 0th element. If we are full, we'll rotate the bottom
    //  one out, and can just reuse it at the end as the new string.
    //
    TString* pstrNew = 0;
    pstrNew = m_apstrLines[0];

    //
    //  Rotate all of the current elements back. New items go at the end,
    //  so we rotate them all. We just start at the 0th element every time
    //  and don't bother just doing the ones currently used, since
    //  eventually they all get used.
    //
    for (tCIDLib::TCard4 c4Index = 1; c4Index < m_c4MaxLines; c4Index++)
        m_apstrLines[c4Index - 1] = m_apstrLines[c4Index];

    //
    //  If full, just bump up the base index. Else, we bump up the message
    //  index.
    //
    if (m_c4MsgIndex == m_c4MaxLines)
        m_c4BaseIndex++;
    else
        m_c4MsgIndex++;

    //
    //  Now put this guy at the last index, which is one less than the new
    //  message index. If we rotated a line out of the bottom above, the
    //  reuse it, else create a new one.
    //
    if (pstrNew)
        *pstrNew = strToAdd;
    else
        pstrNew = new TString(strToAdd);

    // And stick it into the queue at the end
    m_apstrLines[m_c4MaxLines - 1] = pstrNew;
}



// ---------------------------------------------------------------------------
//   CLASS: TTextQOutStream
//  PREFIX: strm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTextQOutStream: Constructors and Destructor
// ---------------------------------------------------------------------------
TTextQOutStream::TTextQOutStream(const  tCIDLib::TCard4     c4MaxLines
                                , const tCIDLib::EReadModes eMode) :

    TTextOutStream(new TNativeWCConverter)
    , m_pstrmiQ(0)
{
    //
    //  Create a stream input implementation object. We save a copy for our
    //  own quick access, but we don't own it. Its owned by the binary stream
    //  we give it to below.
    //
    m_pstrmiQ = new TQOutStreamImpl(c4MaxLines, eMode);

    //
    //  Create a binary stream and give it this implementation. Give it the
    //  console impl object. Then set this object on our parent stream.
    //
    TBinOutStream* pstrmCon = new TBinOutStream(m_pstrmiQ);
    AdoptStream(pstrmCon);
}

TTextQOutStream::~TTextQOutStream()
{
}


// ---------------------------------------------------------------------------
//  TTextQOutStream: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TTextQOutStream::bQueryNewMsgs(         tCIDLib::TCard4&        c4MsgIndex
                                ,       tCIDLib::TStrCollect&   colToFill
                                , const tCIDLib::TBoolean       bAddNewLine)
{
    // Delegate to the implementation object
    return m_pstrmiQ->bQueryNewMsgs(c4MsgIndex, colToFill, bAddNewLine);
}

tCIDLib::TBoolean
TTextQOutStream::bQueryNewMsgs( tCIDLib::TCard4&    c4MsgIndex
                                , TString&          strToFill
                                , tCIDLib::TCard4&  c4LineCount)
{
    // Delegate to the implementation object
    return m_pstrmiQ->bQueryNewMsgs(c4MsgIndex, strToFill, c4LineCount);
}


tCIDLib::TCard4 TTextQOutStream::c4CurMsgIndex() const
{
    return m_pstrmiQ->c4CurMsgIndex();
}


tCIDLib::TCard4 TTextQOutStream::c4LineCount() const
{
    return m_pstrmiQ->c4LineCount();
}

