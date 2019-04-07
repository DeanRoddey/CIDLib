//
// FILE NAME: CIDKernel_ConsoleHelpers.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/16/1999
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
//  This file implements the Win32 console helper classes.
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
#include    "CIDKernel_ConsoleHelpers_.hpp"



// ---------------------------------------------------------------------------
//  TWin32ConCursor: Constructors and Destructor
// ---------------------------------------------------------------------------
TWin32ConCursor::TWin32ConCursor(const HANDLE hCon) :

    m_hCon(hCon)
{
    // This is a temp buffer into which we can query current buffer info
    CONSOLE_SCREEN_BUFFER_INFO BufInfo;

    // Do an initial query and set up some info we keep up with
    if (!::GetConsoleScreenBufferInfo(m_hCon, &BufInfo))
        TKrnlError::ThrowHostError(::GetLastError());

    m_Attrs = BufInfo.wAttributes;
    m_OrgPos = BufInfo.dwCursorPosition;
    m_BufSize = BufInfo.dwSize;
    m_CurPos = BufInfo.dwCursorPosition;

    MoveToCurPos();
}

TWin32ConCursor::~TWin32ConCursor()
{
}


// ---------------------------------------------------------------------------
//  TWin32ConCursor: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TWin32ConCursor::CR()
{
    m_CurPos.X = 0;
    MoveToCurPos();
}


tCIDLib::TVoid TWin32ConCursor::DecPosition()
{
    if (m_CurPos.X)
    {
        m_CurPos.X--;
    }
     else
    {
        // Wrap it around to the end of the previous line
        m_CurPos.Y--;
        m_CurPos.X = short(m_BufSize.X - 1);
    }
    MoveToCurPos();
}


tCIDLib::TVoid TWin32ConCursor::IncPosition()
{
    if (m_CurPos.X + 1 == m_BufSize.X)
    {
        // Reset the x back to zero
        m_CurPos.X = 0;

        // Scroll if required, else move the Y position up
        if (m_CurPos.Y  + 1 == m_BufSize.Y)
        {
            Scroll();

            // Adjust the original position up one
            m_OrgPos.Y--;
        }
         else
        {
            m_CurPos.Y++;
        }
    }
     else
    {
        // Just increment the X
        m_CurPos.X++;
    }

    MoveToCurPos();
}


tCIDLib::TVoid TWin32ConCursor::LF()
{
    if (m_CurPos.Y + 1 == m_BufSize.Y)
    {
        Scroll();

        // Adjust the original position up one
        m_OrgPos.Y--;
    }
     else
    {
        m_CurPos.Y++;
        MoveToCurPos();
    }
}


tCIDLib::TVoid TWin32ConCursor::MoveLeftBy(const tCIDLib::TCard4 c4By)
{
    if (!c4By)
        return;

    m_CurPos.Y = short(m_CurPos.Y - (c4By / m_BufSize.Y));
    SHORT LeftOver = short(c4By % m_BufSize.X);

    if (LeftOver > m_CurPos.X)
    {
        LeftOver = short(LeftOver - m_CurPos.X);
        m_CurPos.Y--;
        m_CurPos.X = short(m_BufSize.X - LeftOver);
    }
     else
    {
        m_CurPos.X = short(m_CurPos.X - LeftOver);
    }
    MoveToCurPos();
}


tCIDLib::TVoid TWin32ConCursor::MoveRightBy(const tCIDLib::TCard4 c4By)
{
    if (!c4By)
        return;

    m_CurPos.Y = short(m_CurPos.Y + (c4By / m_BufSize.Y));
    m_CurPos.X = short(m_CurPos.X + (c4By % m_BufSize.X));
    if (m_CurPos.X > m_BufSize.X)
    {
        m_CurPos.X = short(m_CurPos.X - m_BufSize.X);
        m_CurPos.Y++;
    }
    MoveToCurPos();
}


tCIDLib::TVoid TWin32ConCursor::ResetFromOrg(const tCIDLib::TCard4 c4FromOrg)
{
    m_CurPos.Y = short(m_OrgPos.Y + (c4FromOrg / m_BufSize.X));
    m_CurPos.X = short(m_OrgPos.X + (c4FromOrg % m_BufSize.X));
    if (m_CurPos.X > m_BufSize.X)
    {
        m_CurPos.X = short(m_CurPos.X - m_BufSize.X);
        m_CurPos.Y++;
    }
    MoveToCurPos();
}


tCIDLib::TVoid TWin32ConCursor::ResetToOrg()
{
    m_CurPos = m_OrgPos;
    MoveToCurPos();
}


tCIDLib::TVoid TWin32ConCursor::SetNewPosition(const COORD& NewPos)
{
    m_CurPos = NewPos;
    MoveToCurPos();
}


tCIDLib::TVoid TWin32ConCursor::SyncConsolePos()
{
    MoveToCurPos();
}


tCIDLib::TVoid TWin32ConCursor::UpdateFromConsole()
{
    CONSOLE_SCREEN_BUFFER_INFO BufInfo;
    if (!::GetConsoleScreenBufferInfo(m_hCon, &BufInfo))
        TKrnlError::ThrowHostError(::GetLastError());

    m_CurPos = BufInfo.dwCursorPosition;
}


// ---------------------------------------------------------------------------
//  TWin32ConCursor: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TWin32ConCursor::MoveToCurPos()
{
    if (!::SetConsoleCursorPosition(m_hCon, m_CurPos))
        TKrnlError::ThrowHostError(::GetLastError());
}

tCIDLib::TVoid TWin32ConCursor::Scroll()
{
    SMALL_RECT WholeBuf;
    WholeBuf.Left = 0;
    WholeBuf.Top = 1;
    WholeBuf.Right = short(m_BufSize.X - 1);
    WholeBuf.Bottom = short(m_BufSize.Y - 1);

    // The new target upper left
    COORD NewTarget = {0, 0};

    // Set the fill char/attr
    CHAR_INFO Fill;
    Fill.Char.UnicodeChar = kCIDLib::chSpace;
    Fill.Attributes = m_Attrs;

    if (!::ScrollConsoleScreenBuffer
    (
        m_hCon
        , &WholeBuf
        , 0
        , NewTarget
        , &Fill))
    {
        TKrnlError::ThrowHostError(::GetLastError());
    }
}
