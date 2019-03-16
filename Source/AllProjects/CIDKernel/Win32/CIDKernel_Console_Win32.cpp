//
// FILE NAME: CIDKernel_Console_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/04/1999
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
//  This file provides the Win32 specific implementations for the console
//  input and output classes, TKrnlConIn and TKrnlConOut.
//
//  We also implement the TKrnlConsoleHandle class here, which provides our
//  implementation of a console handle.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"
#include    "CIDKernel_ScanCodes.hpp"
#include    "CIDKernel_ConsoleHelpers_.hpp"
#include    "CIDKernel_InternalHelpers_.hpp"
#include    <wchar.h>


namespace CIDKernel_Console
{
    // -----------------------------------------------------------------------
    //  Local const data
    //
    //  c4TmpOutMode
    //      These are the flags we turn on when we temporarily want the console
    //      to do a little work for us, when we write out a string and want it
    //      to handle new lines and wrap.
    //
    //  c4StdInMode
    //      These are the flags at are used normally for an input console
    //      handle. It might be temporarily changed, but these are what are
    //      always put back.
    //
    //  c4StdOutMode
    //      These are the flags at are used normally for an output console
    //      handle. It might be temporarily changed, but these are what are
    //      always put back.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4 c4TmpOutMode = ENABLE_PROCESSED_OUTPUT
                                       | ENABLE_WRAP_AT_EOL_OUTPUT;
    const tCIDLib::TCard4 c4StdInMode  = ENABLE_PROCESSED_INPUT;
    const tCIDLib::TCard4 c4StdOutMode = 0;


    // -----------------------------------------------------------------------
    //  The return statuses from our bGetChar() helper
    // -----------------------------------------------------------------------
    enum EGetCharRes
    {
        EGetCharRes_Ok
        , EGetCharRes_Error
        , EGetCharRes_Timeout
        , EGetCharRes_Break
    };
}



// ---------------------------------------------------------------------------
//  Local data types
//
//  TConPlatInfo
//      This is our platform info structure. Each platform driver can have its
//      own persistent state info structure for the console input class. This
//      is because they can have radically different needs.
//
//      apszRecallBuf
//          This is the array of strings that holds our recall buffer entries.
//          This is used to support the recall of previous command lines.
//
//      c4MaxRecall
//          The number of entries in the apszRecallBuffer array. So its the
//          number of previous command lines are stored.
//
//      c4RecallIndex
//          The current index into the recall buffer. When a new entry is
//          added, it is set to c4MaxCard to let the recall code know to start
//          on the first element at the next recall.
//
//      chLeading
//          Win32 uses a leading byte to indicate extended chars. We need a
//          place to store that info across some calls internally.
//
//      hOut
//          Since input consoles must echo output and providing editing, it
//          needs an output handle.
// ---------------------------------------------------------------------------
struct TKrnlConIn::TConPlatInfo
{
    tCIDLib::TCh**      apszRecallBuf;
    tCIDLib::TCard4     c4MaxRecall;
    tCIDLib::TCard4     c4RecallIndex;
    tCIDLib::TCh        chLeading;
    HANDLE              hOut;
};



// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------

//
//  This method will get a single raw character, dealing with leading chars.
//  It loops, using low level console input methods, until it gets a relevant
//  character event to return or times out. On a timeout, it returns true
//  and sets chToFill to null.
//
CIDKernel_Console::EGetCharRes
eGetChar(       HANDLE                  hIn
        ,       tCIDLib::TCh&           chToFill
        ,       tCIDLib::TCh&           chLeading
        , const tCIDLib::TBoolean&      bBreakFlag
        , const tCIDLib::TEncodedTime   enctEnd
        ,       TKrnlConIn::TIdleCB     pfnCallback
        ,       TObject* const          pobjCBData)
{
    CIDKernel_Console::EGetCharRes  eRes = CIDKernel_Console::EGetCharRes_Error;
    tCIDLib::TCard4                 c4Read;

    //
    //  If there is a follow up character for the leading zero of an
    //  extended key, then return it now.
    //
    if (chLeading != kCIDLib::chNull)
    {
        chToFill = chLeading;
        chLeading = kCIDLib::chNull;
        return CIDKernel_Console::EGetCharRes_Ok;
    }

    //
    //  We loop until we get a character event, throwing the rest of the
    //  events away since we don't know what to do with them.
    //
    INPUT_RECORD        Input = {0};
    tCIDLib::TBoolean   bDone = kCIDLib::False;
    while (!bDone)
    {
        //
        //  Loop, looking for input. We sleep on each round and check the
        //  break flag. If we get a break, we return false.
        //
        while (!bDone)
        {
            //
            //  Wait for some input on the console handle or msgs to be
            //  pumped.
            //
            tCIDLib::EWaitRes eWaitRes = TKrnlWin32::eWaitHandle(hIn, 100);
            if (eWaitRes == tCIDLib::EWaitRes::Timeout)
            {
                // Call the idle callback if we have one
                if (pfnCallback)
                    pfnCallback(pobjCBData);
            }
             else if (eWaitRes == tCIDLib::EWaitRes::Success)
            {
                // Read the event
                ::ReadConsoleInput(hIn, &Input, 1, &c4Read);
                if (Input.EventType == KEY_EVENT)
                    break;
            }
             else
            {
                // It was a failure of some sort
                return CIDKernel_Console::EGetCharRes_Error;
            }

            // If we got a break flag, then return
            if (bBreakFlag)
                return CIDKernel_Console::EGetCharRes_Break;

            // If we've timed out, return now
            if (TKrnlTimeStamp::enctNow() >= enctEnd)
            {
                chToFill = kCIDLib::chNull;
                return CIDKernel_Console::EGetCharRes_Timeout;
            }
        }

        // Start by getting the character code out of the input record
        chToFill = Input.Event.KeyEvent.uChar.UnicodeChar;

        //
        //  If its an extended key, then we need to store the actual code
        //  and return the correct leading code prefix; else, return the
        //  actual code.
        //
        if (chToFill == kCIDLib::chNull)
        {
            // Save the virtual code
            const tCIDLib::TCh chVK = tCIDLib::TCh
            (
                Input.Event.KeyEvent.wVirtualScanCode
            );

            //
            //  The logic might seem wierd here. ENHANCED_KEY means
            //  basically a keypad key. Otherwise, its an extended
            //  key in our parlance.
            //
            chLeading = chVK;

            if (Input.Event.KeyEvent.dwControlKeyState & ENHANCED_KEY)
                chToFill = Key_PadPrefix;
            else
                chToFill = Key_ExtPrefix;
        }

        // Don't return key up events, just key down
        if (!Input.Event.KeyEvent.bKeyDown)
        {
            chLeading = kCIDLib::chNull;
            chToFill = kCIDLib::chNull;
            continue;
        }

        eRes = CIDKernel_Console::EGetCharRes_Ok;
        break;
    }
    return eRes;
}


//
//  This method is called to do a backspace when at the end of the input. It
//  overwrites the previous char cell with a space and moves the cursor back.
//
static tCIDLib::TVoid Backspace(const HANDLE hCon, TWin32ConCursor& cursPos)
{
    // Move the cursor back one space
    cursPos.DecPosition();

    //
    //  And do a single character fill with a space. This will not move the
    //  cursor, so it stays where it is.
    //
    tCIDLib::TCard4 c4Written;
    if (!::FillConsoleOutputCharacter
    (
        hCon
        , kCIDLib::chSpace
        , 1
        , cursPos.CurPosition()
        , &c4Written))
    {
        TKrnlError::ThrowHostError(::GetLastError());
    }
}


//
//  This method is called to clean out the currently typed input. Note that,
//  because of the way its called, it will never try to write blanks beyond
//  the buffer end. So we don't have to use a loop here to output and handle
//  scrolling. We can just do a single operation and assume it will work.
//
static tCIDLib::TVoid ClearInput(const  HANDLE              hCon
                                ,       TWin32ConCursor&    cursPos
                                , const tCIDLib::TCard4     c4CharCount)
{
    //
    //  Fill the buffer from the original cursor position (possibly adjusted
    //  due to scroll events) and put the cursor back at that position.
    //
    tCIDLib::TCard4 c4Written;
    if (!::FillConsoleOutputCharacter
    (
        hCon
        , kCIDLib::chSpace
        , c4CharCount
        , cursPos.OrgPosition()
        , &c4Written))
    {
        TKrnlError::ThrowHostError(::GetLastError());
    }
    cursPos.ResetToOrg();
}


//
//  This method will output a single character to the console. It will process
//  CR/LF chracters correctly to keep the passed cursor up to date.
//
static tCIDLib::TVoid WriteChar(const   HANDLE              hCon
                                , const tCIDLib::TCh        chToWrite
                                ,       TWin32ConCursor&    cursPos)
{
    tCIDLib::TCard4  c4ActualWritten;

    //
    //  Watch for either of the line control chars and just adjust the
    //  cursor position instead of outputting anything. Else output it
    //  as a character.
    //
    if (chToWrite == kCIDLib::chLF)
    {
        cursPos.LF();
    }
     else if (chToWrite == kCIDLib::chCR)
    {
        cursPos.CR();
    }
     else
    {
        if (!::WriteConsoleOutputCharacter
        (
            hCon
            , &chToWrite
            , 1
            , cursPos.CurPosition()
            , &c4ActualWritten))
        {
            TKrnlError::ThrowHostError(::GetLastError());
        }

        // Make sure we wrote what we should have
        if (c4ActualWritten != 1)
            TKrnlError::ThrowKrnlError(kKrnlErrs::errcData_NotAllData);

        // Move the cursor forward one place
        cursPos.IncPosition();
    }
}


//
//  This function is called to output a sequence of characters. It does not
//  assume that its a null terminated string. It handles CR/LF chars and it
//  updates the cursor position to just past where the output ended.
//
static tCIDLib::TVoid
WriteStr(   const   HANDLE              hCon
            , const tCIDLib::TCh* const pszToWrite
            , const tCIDLib::TCard4     c4ToWrite
            ,       TWin32ConCursor&    cursPos)
{
    //
    //  Temporarily turn on the 'cooked mode' console mode and output the
    //  whole string at once. Then turn them back off to raw mode, and
    //  update our cursor with the position at which the console cursor
    //  ended up.
    //
    tCIDLib::TCard4 c4Written = 0;
    ::SetConsoleMode(hCon, CIDKernel_Console::c4TmpOutMode);
    ::WriteConsole(hCon, pszToWrite, c4ToWrite, &c4Written, 0);
    ::SetConsoleMode(hCon, CIDKernel_Console::c4StdOutMode);
    cursPos.UpdateFromConsole();
}


//
//  This method is called for a Delete or Backspace key, and handles adjusting
//  the input buffer and the displayed input on the console.
//
tCIDLib::TVoid DelInpChar(  const   HANDLE              hCon
                            ,       tCIDLib::TCh* const pszBuffer
                            ,       TWin32ConCursor&    cursPos
                            ,       tCIDLib::TCard4&    c4CharCount
                            ,       tCIDLib::TCard4&    c4CursorPos
                            , const tCIDLib::TBoolean   bOnChar)
{
    //
    //  If we are already at the start of the input field and we are not
    //  on the char (i.e. backspace), then just do nothing.
    //
    if (!c4CursorPos && !bOnChar)
        return;

    //
    //  If we are at the end, and on the char (i.e. delete), then there also
    //  nothing to do.
    //
    if ((c4CursorPos == c4CharCount) && bOnChar)
        return;

    //
    //  If we are already at the end of the input, then all we have to do
    //  is get rid of the last character. We checked above so we know its
    //  a backspace if at the end.
    //
    //  Otherwise, we are in the middle somewhere and have to move all of
    //  the chars past the delete position back and then get rid of the last
    //  displayed char.
    //
    if (c4CursorPos == c4CharCount)
    {
        //
        //  Write a space at the calculated position. Use a temp position so
        //  that it does not update us with the new position after the write.
        //
        Backspace(hCon, cursPos);
    }
     else
    {
        //
        //  We first need to get rid of the character in the buffer that
        //  we don't want to keep. Since we got here, its not at the end, so
        //  we have to copy down chars. So calculate the starting index, which
        //  depends on whether we are on the char or not.
        //
        tCIDLib::TCard4 c4Ind = c4CursorPos;
        if (!bOnChar)
        {
            if (!c4CursorPos)
                TKrnlError::ThrowKrnlError(kKrnlErrs::errcDev_NegativeSeek);
            c4Ind--;
            cursPos.DecPosition();
        }

        // Remember this index
        const tCIDLib::TCard4 c4StartInd = c4Ind;

        //
        //  Adjust the buffer. Put a space at the end. This is not part of the
        //  real input, but it will overwrite the last char. It won't become
        //  part of the input because we will adjust the char count below to
        //  ignore it. It just makes it easier.
        //
        for (; c4Ind < c4CharCount-1; c4Ind++)
            pszBuffer[c4Ind] = pszBuffer[c4Ind+1];
        pszBuffer[c4CharCount - 1] = kCIDLib::chSpace;

        //
        //  Write out the chars from the start index onward. It won't scroll.
        //  We haven't adjusted the char count yet, but that's cool because
        //  we want to write that last 'faked in' space to get rid of the
        //  trailing char.
        //
        //  Note that we have to temporarily turn on the line wrap mode when
        //  we do this. Then we turn it back off again.
        //
        tCIDLib::TCard4 c4Written;
        SetConsoleMode(hCon, ENABLE_WRAP_AT_EOL_OUTPUT);
        if (!::WriteConsole
        (
            hCon
            , &pszBuffer[c4StartInd]
            , c4CharCount - c4StartInd
            , &c4Written
            , 0))
        {
            TKrnlError::ThrowHostError(::GetLastError());
        }
        SetConsoleMode(hCon, CIDKernel_Console::c4StdOutMode);

        // Get the cursor back where it should be
        cursPos.SyncConsolePos();
    }

    //
    //  If deleting, then we move the char count down, but not the cursor
    //  index. If backspacing, we bump them both back.
    //
    c4CharCount--;
    if (!bOnChar)
        c4CursorPos--;
}


//
//  This method is called for Ctrl-Left and Ctrl-Right keys, and handles
//  finding the next or previous word and moving the cursor there.
//
tCIDLib::TVoid NextPrevWord(const   HANDLE
                            , const tCIDLib::TCh* const pszBuffer
                            ,       TWin32ConCursor&    cursPos
                            ,       tCIDLib::TCard4&    c4CursorPos
                            , const tCIDLib::TCard4     c4CharCount
                            , const tCIDLib::EConKeys   eKey)
{
    // Save the original cursor index position
    const tCIDLib::TCard4 c4OrgPos = c4CursorPos;

    tCIDLib::TBoolean   bContinue;
    if (eKey == tCIDLib::EConKeys::Previous)
    {
        // If already at the start, then nothing to do
        if (!c4CursorPos)
            return;

        //
        //  Calculate the positions we have to move backwards. Handle the
        //  special case of being on the second char which always moves us
        //  to the zeroth one.
        //
        if (c4CursorPos == 1)
        {
            c4CursorPos = 0;
        }
         else
        {
            //
            //  If the current char is not a space, and neither is the
            //  previous one, then look backward for a space. When we hit it,
            //  go back up one. If the current char is not a space, but the
            //  previous one is, then back up 1 and fall through to next block
            //
            bContinue = kCIDLib::False;
            if (pszBuffer[c4CursorPos] != kCIDLib::chSpace)
            {
                if (pszBuffer[c4CursorPos-1] == kCIDLib::chSpace)
                {
                    c4CursorPos--;
                    bContinue = kCIDLib::True;
                }
                 else
                {
                    for (; c4CursorPos > 0; c4CursorPos--)
                    {
                        if (pszBuffer[c4CursorPos] == kCIDLib::chSpace)
                        {
                            c4CursorPos++;
                            break;
                        }
                    }
                }
            }
             else
            {
                bContinue = kCIDLib::True;
            }

            if (bContinue)
            {
                // Look backward for a non-space
                for (; c4CursorPos > 0; c4CursorPos--)
                {
                    if (pszBuffer[c4CursorPos] != kCIDLib::chSpace)
                        break;
                }

                //
                //  Look backwards for a space, then move up one.
                //
                for (; c4CursorPos > 0; c4CursorPos--)
                {
                    if (pszBuffer[c4CursorPos] == kCIDLib::chSpace)
                    {
                        c4CursorPos++;
                        break;
                    }
                }
            }
        }

        //
        //  Adjust the cursor position by the number of positions we have
        //  to move.
        //
        cursPos.MoveLeftBy(c4OrgPos - c4CursorPos);
    }
     else if (eKey == tCIDLib::EConKeys::Next)
    {
        // If at the end, then do nothing
        if (c4CursorPos == c4CharCount)
            return;

        //
        //  If the current char is a space, then we just need to look for
        //  the next non-space. If its not a space, then we need to find
        //  the next space, then the next non space.
        //
        if (pszBuffer[c4CursorPos] != kCIDLib::chSpace)
        {
            // Look forward for a space
            for (; c4CursorPos < c4CharCount; c4CursorPos++)
            {
                if (pszBuffer[c4CursorPos] == kCIDLib::chSpace)
                    break;
            }
        }

        // And now look forward for a non-space
        for (; c4CursorPos < c4CharCount; c4CursorPos++)
        {
            if (pszBuffer[c4CursorPos] != kCIDLib::chSpace)
                break;
        }

        //
        //  Adjust the cursor position by the number of positions we have
        //  to move.
        //
        cursPos.MoveRightBy(c4CursorPos - c4OrgPos);
    }
}



// ---------------------------------------------------------------------------
//  Initialization methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TCIDKrnlModule::bInitTermConsole(const tCIDLib::EInitTerm)
{
    // Nothing to do at this time
    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//   CLASS: TConsoleHandle
//  PREFIX: hcon
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TConsoleHandle: Constructors and Destructor
// ---------------------------------------------------------------------------
TConsoleHandle::TConsoleHandle() :

    m_phconiThis(0)
{
    m_phconiThis = new TConsoleHandleImpl;
    m_phconiThis->hCon = 0;
}

TConsoleHandle::TConsoleHandle(const TConsoleHandle& hconToCopy) :

    m_phconiThis(0)
{
    m_phconiThis = new TConsoleHandleImpl;
    m_phconiThis->hCon = hconToCopy.m_phconiThis->hCon;
}

TConsoleHandle::~TConsoleHandle()
{
    delete m_phconiThis;
    m_phconiThis = 0;
}


// ---------------------------------------------------------------------------
//  TConsoleHandle: Public operators
// ---------------------------------------------------------------------------
TConsoleHandle& TConsoleHandle::operator=(const TConsoleHandle& hconToAssign)
{
    if (this == &hconToAssign)
        return *this;

    m_phconiThis->hCon = hconToAssign.m_phconiThis->hCon;
    return *this;
}


tCIDLib::TBoolean
TConsoleHandle::operator==(const TConsoleHandle& hconToCompare) const
{
    return (m_phconiThis->hCon == hconToCompare.m_phconiThis->hCon);
}

tCIDLib::TBoolean
TConsoleHandle::operator!=(const TConsoleHandle& hconToCompare) const
{
    return !operator==(hconToCompare);
}


// ---------------------------------------------------------------------------
//  Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TConsoleHandle::bIsValid() const
{
    return (m_phconiThis->hCon != 0);
}


tCIDLib::TVoid TConsoleHandle::Clear()
{
    m_phconiThis->hCon = 0;
}


tCIDLib::TVoid
TConsoleHandle::FormatToStr(        tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal
    (
        tCIDLib::TCard4(m_phconiThis->hCon)
        , pszToFill
        , c4MaxChars
        , tCIDLib::ERadices::Hex
    );
}

const TConsoleHandleImpl& TConsoleHandle::hconiThis() const
{
    return *m_phconiThis;
}



// ---------------------------------------------------------------------------
//   CLASS: TKrnlConIn
//  PREFIX: kcon
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlConIn: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TKrnlConIn::bReadCharMS(        tCIDLib::EConKeys&  eKeyType
                        ,       tCIDLib::TCh&       chGotten
                        , const tCIDLib::TCard4     c4Wait
                        ,       TIdleCB             pfnCallback
                        ,       TObject* const      pobjCBData)
{
    // Just call the end time based one with the calculated end time
    return bReadChar
    (
        eKeyType
        , chGotten
        , TKrnlTimeStamp::enctNow() + (c4Wait * kCIDLib::enctOneMilliSec)
        , pfnCallback
        , pobjCBData
    );
}

tCIDLib::TBoolean
TKrnlConIn::bReadChar(          tCIDLib::EConKeys&      eKeyType
                        ,       tCIDLib::TCh&           chGotten
                        , const tCIDLib::TEncodedTime   enctEnd
                        ,       TIdleCB                 pfnCallback
                        ,       TObject* const          pobjCBData)
{
    //
    //  We need to set ourselves up to handle break signals. So we use a
    //  kernel signal janitors to do it, to insure it gets removed on exit.
    //
    TKrnlSignalJanitor janBreak(this);
    m_bBreakSeen = kCIDLib::False;

    // Loop until we are done or time out
    tCIDLib::TBoolean               bDone = kCIDLib::False;
    CIDKernel_Console::EGetCharRes  eRes;
    while (!bDone)
    {
        // Read in a raw character
        eRes = eGetChar
        (
            m_hconThis.m_phconiThis->hCon
            , chGotten
            , m_pPlatform->chLeading
            , m_bBreakSeen
            , enctEnd
            , pfnCallback
            , pobjCBData
        );

        if (eRes == CIDKernel_Console::EGetCharRes_Error)
            return kCIDLib::False;

        if (eRes == CIDKernel_Console::EGetCharRes_Break)
        {
            // If because of a break signal, then return a breakby faking it
            eKeyType = tCIDLib::EConKeys::Break;
            return kCIDLib::True;
        }

        // If we timed out, then just return a null char
        if (eRes == CIDKernel_Console::EGetCharRes_Timeout)
        {
            eKeyType = tCIDLib::EConKeys::Null;
            chGotten = kCIDLib::chNull;
            return kCIDLib::True;
        }

        //
        //  If its one of the prefixes that indicates an extended key, then
        //  we need to look further. Otherwise, just indicate that its a
        //  regular char and return what we got.
        //
        if ((chGotten == Key_PadPrefix) || (chGotten == Key_ExtPrefix))
        {
            // Get the next char, which will be the real key
            eRes = eGetChar
            (
                m_hconThis.m_phconiThis->hCon
                , chGotten
                , m_pPlatform->chLeading
                , m_bBreakSeen
                , enctEnd
                , pfnCallback
                , pobjCBData
            );

            if (eRes == CIDKernel_Console::EGetCharRes_Error)
                return kCIDLib::False;

            if (eRes == CIDKernel_Console::EGetCharRes_Break)
            {
                eKeyType = tCIDLib::EConKeys::Break;
                return kCIDLib::True;
            }

            // Get the control key state
            const tCIDLib::TBoolean bCtrl
            (
                (::GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0
            );

            //
            //  Map the input char to the correct virtual key code that we
            //  return.
            //
            switch(chGotten)
            {
                case Key_Delete :
                    eKeyType = tCIDLib::EConKeys::Delete;
                    return kCIDLib::True;

                case Key_Down :
                    eKeyType = tCIDLib::EConKeys::Down;
                    return kCIDLib::True;

                case Key_End :
                    eKeyType = tCIDLib::EConKeys::End;
                    return kCIDLib::True;

                case Key_Home :
                    eKeyType = tCIDLib::EConKeys::Home;
                    return kCIDLib::True;

                case Key_Insert :
                    eKeyType = tCIDLib::EConKeys::Insert;
                    return kCIDLib::True;

                case Key_Left :
                    if (bCtrl)
                        eKeyType = tCIDLib::EConKeys::Previous;
                    else
                        eKeyType = tCIDLib::EConKeys::Left;
                    return kCIDLib::True;

                case Key_Right :
                    if (bCtrl)
                        eKeyType = tCIDLib::EConKeys::Next;
                    else
                        eKeyType = tCIDLib::EConKeys::Right;
                    return kCIDLib::True;

                case Key_Up :
                    eKeyType = tCIDLib::EConKeys::Up;
                    return kCIDLib::True;

                default :
                    eKeyType = tCIDLib::EConKeys::Null;
                    break;
            }
        }
         else
        {
            // Map some to 'virtual keys' and the rest are regular chars
            switch(chGotten)
            {
                case Key_BackTab :
                    eKeyType = tCIDLib::EConKeys::BackTab;
                    return kCIDLib::True;

                case Key_BS :
                    eKeyType = tCIDLib::EConKeys::Backspace;
                    return kCIDLib::True;

                case Key_Enter :
                    eKeyType = tCIDLib::EConKeys::Enter;
                    return kCIDLib::True;

                case Key_Escape :
                    eKeyType = tCIDLib::EConKeys::Escape;
                    return kCIDLib::True;

                case Key_Tab :
                    eKeyType = tCIDLib::EConKeys::Tab;
                    return kCIDLib::True;

                default :
                    eKeyType = tCIDLib::EConKeys::Char;
                    return kCIDLib::True;
            }
        }
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlConIn::bReadLine(          tCIDLib::TCh* const     pszToFill
                        , const tCIDLib::TCard4         c4CharsToRead
                        ,       tCIDLib::TCard4&        c4CharsRead
                        ,       TIdleCB                 pfnCallback
                        ,       TObject* const          pobjCBData)
{
    //
    //  Catch any kernel errors propogating out of the internal implemntation
    //  methods and convert them to an error return.
    //
    try
    {
        // Create a cursor object to manage the cursor while editing
        TWin32ConCursor cursPos(m_pPlatform->hOut);

        // Assume no input unless we get something
        c4CharsRead = 0;

        //
        //  Initialize our character count and cursor position flags for an
        //  empty string.
        //
        tCIDLib::TCard4 c4CharCount = 0;
        tCIDLib::TCard4 c4CursorPos = 0;

        // And make sure the passed buffer starts empty
        pszToFill[0] = 0;

        tCIDLib::TCh        chCur;
        tCIDLib::TCard4     c4TmpInd;
        tCIDLib::TCard4     c4Written;
        tCIDLib::EConKeys   eType;

        tCIDLib::TBoolean   bDone = kCIDLib::False;
        while (!bDone)
        {
            //
            //  Get another char. If it fails, then we return a failure.
            //  Block for 100MS to proivde throttling. If it just times
            //  out then the type is set to null and we won't do anything
            //  below. We'll just come back around.
            //
            if (!bReadCharMS(eType, chCur, 100, pfnCallback, pobjCBData))
                return kCIDLib::False;

            //
            //  If its not a regular character, then process it as a special
            //  key, else process it as a regular character.
            //
            if (eType == tCIDLib::EConKeys::Char)
            {
                //
                //  If we are full, then beep at the user. Otherwise,
                //  output the char and add it to the user buffer.
                //
                if ((m_bInsertMode && (c4CharCount == c4CharsToRead))
                ||  (!m_bInsertMode && (c4CursorPos == c4CharsToRead)))
                {
                    TKrnlAudio::Beep(440, 50);
                    continue;
                }

                //
                //  We either need to insert or overwrite here. If at the end,
                //  then its not an issue so do that first.
                //
                if (c4CursorPos == c4CharCount)
                {
                    pszToFill[c4CharCount++] = chCur;

                    // Echo to the screen at current cursor position.
                    WriteChar(m_pPlatform->hOut, chCur, cursPos);
                }
                 else if (m_bInsertMode)
                {
                    //
                    //  Move the characters all up in the string, then stick
                    //  in the new character at the created spot.
                    //
                    for (c4TmpInd = c4CharCount; c4TmpInd > c4CursorPos; c4TmpInd--)
                        pszToFill[c4TmpInd] = pszToFill[c4TmpInd-1];
                    pszToFill[c4CursorPos] = chCur;

                    // Bump up the chart count now
                    c4CharCount++;

                    //
                    //  Display the chars from the insert position. Save the
                    //  current position for later reuse.
                    //
                    ::SetConsoleMode
                    (
                        m_pPlatform->hOut, CIDKernel_Console::c4TmpOutMode
                    );

                    ::WriteConsole
                    (
                        m_pPlatform->hOut
                        , &pszToFill[c4TmpInd]
                        , c4CharCount - c4TmpInd
                        , &c4Written
                        , 0
                    );
                    ::SetConsoleMode
                    (
                        m_pPlatform->hOut, CIDKernel_Console::c4StdOutMode
                    );

                    //
                    //  Now we need to move the cursor to the next position,
                    //  which will also undo the movement that occured when
                    //  the string was written out above.
                    //
                    cursPos.IncPosition();
                }
                 else
                {
                    //
                    //  Just overwrite the current character both in the input
                    //  buffer and on the console.
                    //
                    pszToFill[c4CursorPos] = chCur;
                    WriteChar(m_pPlatform->hOut, chCur, cursPos);
                }

                // Move the cursor index up by one
                c4CursorPos++;
            }
             else
            {
                //
                //  Its one of the non-char keys, so its not input but editing
                //  control stuff.
                //
                if (eType == tCIDLib::EConKeys::Break)
                {
                    // Its a break event, so throw away input and return
                    pszToFill[0] = kCIDLib::chNull;
                    c4CharsRead = 0;
                    return kCIDLib::True;
                }
                 else if (eType == tCIDLib::EConKeys::Backspace)
                {
                    DelInpChar
                    (
                        m_pPlatform->hOut
                        , pszToFill
                        , cursPos
                        , c4CharCount
                        , c4CursorPos
                        , kCIDLib::False
                    );
                }
                 else if (eType == tCIDLib::EConKeys::Enter)
                {
                    // Move to the end of the input
                    cursPos.ResetFromOrg(c4CharCount);

                    // Store the new line and cap it off
                    pszToFill[c4CharCount++] = kCIDLib::chLF;
                    pszToFill[c4CharCount] = kCIDLib::chNull;

                    //
                    //  Add it to the recall list, if there is one and the
                    //  string was not just whitespace or empty.
                    //
                    if (m_pPlatform->apszRecallBuf && c4CharCount)
                        NewEntry(pszToFill);

                    // Output a new line
                    tCIDLib::TCh szTmp[2];
                    szTmp[0] = kCIDLib::chCR;
                    szTmp[1] = kCIDLib::chLF;

                    WriteStr
                    (
                        m_pPlatform->hOut
                        , szTmp
                        , 2
                        , cursPos
                    );

                    // Return the chars in the string.
                    c4CharsRead = c4CharCount;

                    return kCIDLib::True;
                }
                 else if (eType == tCIDLib::EConKeys::Escape)
                {
                    // Clear out the input and put the cursor back at home
                    ClearInput(m_pPlatform->hOut, cursPos, c4CharCount);

                    // And reset all our position info
                    c4CharCount = 0;
                    c4CursorPos = 0;
                }
                 else if ((eType == tCIDLib::EConKeys::Tab)
                      ||  (eType == tCIDLib::EConKeys::BackTab))
                {
                    TKrnlAudio::Beep(440, 50);
                }
                else if (((eType == tCIDLib::EConKeys::Down)
                     ||   (eType == tCIDLib::EConKeys::Up))
                     &&  m_pPlatform->apszRecallBuf)
                {
                    //
                    //  The current recall index becomes the entry to recall
                    //  this time, then we search up or down in the list to
                    //  find the next/prev entry for the next time.
                    //
                    //  Note that when Enter is pressed and a non-empty new
                    //  entry is added, the recall index is set to c4MaxCard
                    //  to let us know that we can only accept an up arrow to
                    //  take us to the zeroth entry.
                    //
                    //  Zero is the head, and the entries are moved upwards
                    //  when a new entry is added. When the list is full, the
                    //  last one is thrown away.
                    //
                    //  Arrow up moves toward older entries, and down moves
                    //  toward new entries.
                    //
                    const tCIDLib::TCard4 c4OldIndex = m_pPlatform->c4RecallIndex;
                    if (m_pPlatform->c4RecallIndex != kCIDLib::c4MaxCard)
                    {
                        if (chCur == Key_Down)
                        {
                            // If we can move down further, then do it
                            if (m_pPlatform->c4RecallIndex > 0)
                                m_pPlatform->c4RecallIndex--;
                        }
                         else if (chCur == Key_Up)
                        {
                            //
                            //  If we can move up, and there is something
                            //  there to move up to, then do it.
                            //
                            if ((m_pPlatform->c4RecallIndex < m_pPlatform->c4MaxRecall-1)
                            &&  (m_pPlatform->apszRecallBuf[m_pPlatform->c4RecallIndex+1]))
                            {
                                m_pPlatform->c4RecallIndex++;
                            }
                        }
                    }
                     else
                    {
                        //
                        //  In this state, only a key up is recognized. If we
                        //  get one, see if there is a zeroth entry. If so,
                        //  then set the recall index to that zeroth element.
                        //
                        if (chCur == Key_Up)
                        {
                            if (m_pPlatform->apszRecallBuf[0])
                                m_pPlatform->c4RecallIndex = 0;
                        }
                    }

                    // If we changed the recall index, then take the new
                    if (c4OldIndex != m_pPlatform->c4RecallIndex)
                    {
                        ClearInput(m_pPlatform->hOut, cursPos, c4CharCount);
                        c4CharCount = 0;
                        c4CursorPos = 0;

                        //
                        //  Output the selected recall item and make it the
                        //  input string. Put the cursor at the end of it.
                        //
                        //  Note that we only use as much of it as will fit
                        //  into the caller's buffer!
                        //
                        TRawStr::CopyStr
                        (
                            pszToFill
                            , m_pPlatform->apszRecallBuf[m_pPlatform->c4RecallIndex]
                            , c4CharsToRead
                        );

                        //
                        //  Write out the new buffer. This will naturally
                        //  leave the cursor at the end, which is what we
                        //  want.
                        //
                        WriteStr
                        (
                            m_pPlatform->hOut
                            , pszToFill
                            , TRawStr::c4StrLen(pszToFill)
                            , cursPos
                        );

                        c4CharCount = TRawStr::c4StrLen(pszToFill);
                        c4CursorPos = c4CharCount;
                    }
                }
                 else if (eType == tCIDLib::EConKeys::Delete)
                {
                    // If not at the end, then delete
                    if (c4CursorPos != c4CharCount)
                    {
                        DelInpChar
                        (
                            m_pPlatform->hOut
                            , pszToFill
                            , cursPos
                            , c4CharCount
                            , c4CursorPos
                            , kCIDLib::True
                        );
                    }
                }
                 else if (eType == tCIDLib::EConKeys::End)
                {
                    cursPos.ResetFromOrg(c4CharCount);
                    c4CursorPos = c4CharCount;
                }
                 else if (eType == tCIDLib::EConKeys::Home)
                {
                    cursPos.ResetToOrg();
                    c4CursorPos = 0;
                }
                 else if (eType == tCIDLib::EConKeys::Insert)
                {
                    // Toggle the insert mode and cursor shape
                    bSetInsertMode(!m_bInsertMode);
                }
                 else if (eType == tCIDLib::EConKeys::Left)
                {
                    //
                    //  Move the current position to the left if we are
                    //  not already at the start.
                    //
                    if (c4CursorPos)
                    {
                        cursPos.DecPosition();
                        c4CursorPos--;
                    }
                }
                 else if (eType == tCIDLib::EConKeys::Right)
                {
                    // If already at the end, do nothing
                    if (c4CharCount && (c4CursorPos < c4CharCount))
                    {
                        cursPos.IncPosition();
                        c4CursorPos++;
                    }
                }
                 else if (eType == tCIDLib::EConKeys::Previous)
                {
                    NextPrevWord
                    (
                        m_pPlatform->hOut
                        , pszToFill
                        , cursPos
                        , c4CursorPos
                        , c4CharCount
                        , tCIDLib::EConKeys::Previous
                    );
                }
                 else if (eType == tCIDLib::EConKeys::Next)
                {
                    NextPrevWord
                    (
                        m_pPlatform->hOut
                        , pszToFill
                        , cursPos
                        , c4CursorPos
                        , c4CharCount
                        , tCIDLib::EConKeys::Next
                    );
                }
            }
        }
    }

    catch(const TKrnlError& kerrToCatch)
    {
        TKrnlError::SetLastError(kerrToCatch);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlConIn::bOpenCon()
{
    //
    //  Allocate, init, and store our per-platform info structure. Its stored
    //  as a void pointer in the outside world, so we have to cast it back
    //  everywhere else.
    //
    HANDLE hTmpIn = INVALID_HANDLE_VALUE;
    HANDLE hTmpOut = INVALID_HANDLE_VALUE;

    try
    {
        //
        //  Try to open an input console handle and an output console handle.
        //  We ignore any redirection here.
        //
        hTmpIn = ::CreateFile
        (
            L"CONIN$"
            , GENERIC_READ | GENERIC_WRITE
            , FILE_SHARE_READ | FILE_SHARE_WRITE
            , 0
            , OPEN_EXISTING
            , FILE_ATTRIBUTE_NORMAL
            , 0
        );

        if (hTmpIn == INVALID_HANDLE_VALUE)
            TKrnlError::ThrowHostError(::GetLastError());

        hTmpOut = ::CreateFile
        (
            L"CONOUT$"
            , GENERIC_READ | GENERIC_WRITE
            , FILE_SHARE_READ | FILE_SHARE_WRITE
            , 0
            , OPEN_EXISTING
            , FILE_ATTRIBUTE_NORMAL
            , 0
        );

        if (hTmpOut == INVALID_HANDLE_VALUE)
            TKrnlError::ThrowHostError(::GetLastError());

        //
        //  Set the desired console modes. For input, we enable window
        //  input so that we'll get resize events. For output, we don't
        //  enable any, since we take complete control of output.
        //
        if (!::SetConsoleMode(hTmpIn, CIDKernel_Console::c4StdInMode))
            TKrnlError::ThrowHostError(::GetLastError());

        if (!::SetConsoleMode(hTmpOut, CIDKernel_Console::c4StdOutMode))
            TKrnlError::ThrowHostError(::GetLastError());
    }

    catch(const TKrnlError& kerrToCatch)
    {
        // Clean up the out handle if it got created
        if (hTmpOut != INVALID_HANDLE_VALUE)
            ::CloseHandle(hTmpOut);

        // Close down the input handle if we got it opened
        if (hTmpIn != INVALID_HANDLE_VALUE)
            ::CloseHandle(hTmpIn);

        // Store the caught error as the last error and return failure
        TKrnlError::SetLastError(kerrToCatch);
        return kCIDLib::False;
    }

    // It all went ok, so store the info
    m_pPlatform->hOut = hTmpOut;
    m_hconThis.m_phconiThis->hCon = hTmpIn;
    m_pPlatform->chLeading = kCIDLib::chNull;

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlConIn::bSetInsertMode(const tCIDLib::TBoolean bNewState)
{
    // Store the new state
    m_bInsertMode = bNewState;

    //
    //  Adjust the cursor shape according to the insert mode that we just
    //  got. If in insert mode, then make it half height, else make it
    //  just an underline.
    //
    CONSOLE_CURSOR_INFO Info;
    Info.bVisible = 1;
    if (m_bInsertMode)
        Info.dwSize = 50;
    else
        Info.dwSize = 10;

    if (!::SetConsoleCursorInfo(m_pPlatform->hOut, &Info))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TVoid TKrnlConIn::Close()
{
    // Close down the handles if they are open
    if (m_hconThis.m_phconiThis->hCon != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(m_hconThis.m_phconiThis->hCon);
        m_hconThis.m_phconiThis->hCon = 0;
    }

    if (m_pPlatform->hOut != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(m_pPlatform->hOut);
        m_pPlatform->hOut = INVALID_HANDLE_VALUE;
    }
}


tCIDLib::TVoid TKrnlConIn::NewEntry(const tCIDLib::TCh* const pszBuffer)
{
    //
    //  Get the length. If its empty, then do nothing. If its one and the
    //  character is the ending line feed, then do nothing.
    //
    const tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(pszBuffer);
    if (!c4Len)
        return;

    if ((c4Len == 1) && (*pszBuffer == kCIDLib::chLF))
        return;

    //
    //  When a new entry is added, the recall index is set to c4MaxCard. This
    //  tells the recall code that the first recall must be an up arrow and
    //  must recall the 0th entry.
    //
    m_pPlatform->c4RecallIndex = kCIDLib::c4MaxCard;

    //
    //  If the new guy is the same as the current top, then don't bother to
    //  add another one. We don't compare the trailing LF!
    //
    if (m_pPlatform->apszRecallBuf[0])
    {
        if (TRawStr::bCompareStrN(m_pPlatform->apszRecallBuf[0], pszBuffer, c4Len - 1))
            return;
    }

    // If its full, then delete the last one
    if (m_pPlatform->apszRecallBuf[m_pPlatform->c4MaxRecall-1])
    {
        delete [] m_pPlatform->apszRecallBuf[m_pPlatform->c4MaxRecall-1];
        m_pPlatform->apszRecallBuf[m_pPlatform->c4MaxRecall - 1] = 0;
    }

    //
    //  Rotate the elements all upwards and copy the new string into the 0th
    //  element. Note that we get rid of the trailing line feed as we go.
    //
    for (tCIDLib::TCard4 c4TmpInd = m_pPlatform->c4MaxRecall-1; c4TmpInd > 0; c4TmpInd--)
        m_pPlatform->apszRecallBuf[c4TmpInd] = m_pPlatform->apszRecallBuf[c4TmpInd-1];

    m_pPlatform->apszRecallBuf[0] = new tCIDLib::TCh[c4Len + 1];
    TRawStr::CopyStr(m_pPlatform->apszRecallBuf[0], pszBuffer, c4Len - 1);
    m_pPlatform->apszRecallBuf[0][c4Len] = kCIDLib::chNull;
}


TKrnlConIn::TConPlatInfo*
TKrnlConIn::pInitPlatform(const tCIDLib::TCard4 c4MaxRecall)
{
    TKrnlConIn::TConPlatInfo* pPlatInfo = new TConPlatInfo;
    pPlatInfo->hOut = INVALID_HANDLE_VALUE;
    pPlatInfo->chLeading = kCIDLib::chNull;
    pPlatInfo->c4MaxRecall = c4MaxRecall;
    pPlatInfo->c4RecallIndex = kCIDLib::c4MaxCard;
    pPlatInfo->apszRecallBuf = 0;

    // If they wanted recall support, then allocate and init the buffer
    if (c4MaxRecall)
    {
        pPlatInfo->apszRecallBuf = new tCIDLib::TCh*[c4MaxRecall];

        //
        //  Note the fill must be typed because there are multiple versions
        //  that will fill with different sized values.
        //
        TRawMem::SetMemBuf
        (
            pPlatInfo->apszRecallBuf
            , tCIDLib::TCard1(0)
            , c4MaxRecall * sizeof(pPlatInfo->apszRecallBuf[0])
        );
    }
    return pPlatInfo;
}


tCIDLib::TVoid TKrnlConIn::ResetRecall()
{
    // Delete all the recall buffer entries and reset index to 'none' value
    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < m_pPlatform->c4MaxRecall; c4Ind++)
        delete [] m_pPlatform->apszRecallBuf[c4Ind];
    m_pPlatform->c4RecallIndex = kCIDLib::c4MaxCard;
}


tCIDLib::TVoid TKrnlConIn::TermPlatform()
{
    // If we never got it allocated, then nothing to do
    if (!m_pPlatform)
        return;

    // If we have a recall buffer, then clean it up and its entries
    if (m_pPlatform->apszRecallBuf)
    {
        // Delete any string pointers we might have
        for (tCIDLib::TCard4 c4Ind = 0; c4Ind < m_pPlatform->c4MaxRecall; c4Ind++)
            delete [] m_pPlatform->apszRecallBuf[c4Ind];

        // Then delete the buffer itself
        delete [] m_pPlatform->apszRecallBuf;
    }

    delete m_pPlatform;
    m_pPlatform = 0;
}




// ---------------------------------------------------------------------------
//   CLASS: TKrnlConOut
//  PREFIX: kcon
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlConOut: Public, non-virtual methods
// ---------------------------------------------------------------------------
// If they are managing the input themselves, they have to be able to do a backspace
tCIDLib::TBoolean TKrnlConOut::bBackspace()
{
    // Create a cursor pos and initialize it from the console
    TWin32ConCursor cursPos(m_hconThis.m_phconiThis->hCon);
    cursPos.UpdateFromConsole();

    // If we can move back, then do so
    if (cursPos.CurPosition().X > 0)
    {
        Backspace(m_hconThis.m_phconiThis->hCon, cursPos);
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean TKrnlConOut::bClearScr()
{
    // Query the current buffer info
    CONSOLE_SCREEN_BUFFER_INFO HostInfo;
    if (!::GetConsoleScreenBufferInfo(m_hconThis.m_phconiThis->hCon, &HostInfo))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    //
    //  Now use that to calculate the info we need to fill the buffer with
    //  spaces.
    //
    COORD cordStart = {0,0};
    tCIDLib::TCard4 c4Written;
    if (!::FillConsoleOutputCharacter
    (
        m_hconThis.m_phconiThis->hCon
        , kCIDLib::chSpace
        , HostInfo.dwSize.X * HostInfo.dwSize.Y
        , cordStart
        , &c4Written))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Set the cursor position to the buffer origin
    if (!::SetConsoleCursorPosition(m_hconThis.m_phconiThis->hCon, cordStart))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Set the window position back to the buffer origin, too
    SMALL_RECT newWndPos =
    {
        0
        , 0
        , short(HostInfo.srWindow.Right - HostInfo.srWindow.Left)
        , short(HostInfo.srWindow.Bottom - HostInfo.srWindow.Top)
    };
    if (!::SetConsoleWindowInfo(m_hconThis.m_phconiThis->hCon, 1, &newWndPos))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlConOut::bPutChar(const tCIDLib::TCh chToWrite)
{
    tCIDLib::TCard4 c4ActualWritten;

    // Console works in terms of chars, not bytes
    if (!::WriteConsole(m_hconThis.m_phconiThis->hCon
                        , &chToWrite
                        , 1
                        , &c4ActualWritten
                        , 0))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    if (c4ActualWritten != 1)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_NotAllData);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlConOut::bQueryVerticalLines(tCIDLib::TCard4& c4ToFill) const
{
    // Query the current buffer info
    CONSOLE_SCREEN_BUFFER_INFO HostInfo;
    if (!::GetConsoleScreenBufferInfo(m_hconThis.m_phconiThis->hCon, &HostInfo))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Calculate the visible number of lines and fill in the caller's buffer
    c4ToFill = HostInfo.srWindow.Bottom - HostInfo.srWindow.Top;
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlConOut::bReset()
{
    // Just clear the screen
    if (!bClearScr())
        return kCIDLib::False;
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlConOut::bWriteBytes(const  tCIDLib::TVoid* const   pToWrite
                        , const tCIDLib::TCard4         c4ToWrite
                        ,       tCIDLib::TCard4&        c4Written)
{
    // If nothing to write, then do nothing
    if (!c4ToWrite)
        return kCIDLib::True;

    // Initialize the written bytes
    c4Written = 0;

    // Look at the buffer as a Unicode string
    const tCIDLib::TCh* pszToWrite
                        = reinterpret_cast<const tCIDLib::TCh*>(pToWrite);

    //
    //  Temporarily turn on cooked mode on the console and then write out
    //  the string, then put it back into raw mode. We convert the passed
    //  bytes to write to characters to write, since WriteConsole works in
    //  terms of characters.
    //
    ::SetConsoleMode
    (
        m_hconThis.m_phconiThis->hCon
        , CIDKernel_Console::c4TmpOutMode
    );

    if (!::WriteConsole(m_hconThis.m_phconiThis->hCon
                        , pszToWrite
                        , c4ToWrite / kCIDLib::c4CharBytes
                        , &c4Written
                        , 0))
    {
        ::SetConsoleMode
        (
            m_hconThis.m_phconiThis->hCon
            , CIDKernel_Console::c4StdOutMode
        );
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    ::SetConsoleMode(m_hconThis.m_phconiThis->hCon, CIDKernel_Console::c4StdOutMode);

    // Convert the char written count back to bytes written
    c4Written *= kCIDLib::c4CharBytes;

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlConOut::bWriteLine(const tCIDLib::TCh* const pszToWrite)
{
    tCIDLib::TCard4 c4Written;

    const tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(pszToWrite);

    //
    //  Temporarily turn on cooked mode on the console and then write out
    //  the string, then put it back into raw mode. We convert the passed
    //  bytes to write to characters to write, since WriteConsole works in
    //  terms of characters.
    //
    ::SetConsoleMode
    (
        m_hconThis.m_phconiThis->hCon
        , CIDKernel_Console::c4TmpOutMode
    );

    if (!::WriteConsole(m_hconThis.m_phconiThis->hCon
                        , pszToWrite
                        , c4Len
                        , &c4Written
                        , 0))
    {
        ::SetConsoleMode
        (
            m_hconThis.m_phconiThis->hCon
            , CIDKernel_Console::c4StdOutMode
        );
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    ::SetConsoleMode
    (
        m_hconThis.m_phconiThis->hCon
        , CIDKernel_Console::c4StdOutMode
    );

    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  TKrnlConOut: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlConOut::bOpenCon()
{
    //
    //  Try to open an output console handle. We ignore any redirection
    //  here.
    //
    HANDLE hTmp = ::CreateFile
    (
        L"CONOUT$"
        , GENERIC_READ | GENERIC_WRITE
        , FILE_SHARE_READ | FILE_SHARE_WRITE
        , 0
        , OPEN_EXISTING
        , FILE_ATTRIBUTE_NORMAL
        , 0
    );

    if (hTmp == INVALID_HANDLE_VALUE)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    m_hconThis.m_phconiThis->hCon = hTmp;

    // Set up the desired default console mode
    if (!::SetConsoleMode(hTmp, CIDKernel_Console::c4StdOutMode))
    {
        ::CloseHandle(hTmp);
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    return kCIDLib::True;
}

tCIDLib::TVoid TKrnlConOut::Close()
{
    // Close down the handle if its open
    if (m_hconThis.m_phconiThis->hCon)
    {
        ::CloseHandle(m_hconThis.m_phconiThis->hCon);
        m_hconThis.m_phconiThis->hCon = 0;
    }
}


TKrnlConOut::TConPlatInfo* TKrnlConOut::pInitPlatform()
{
    // We don't use this so we just return a null pointer
    return nullptr;
}


tCIDLib::TVoid TKrnlConOut::TermPlatform()
{
    // No-op for us
}

