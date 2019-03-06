//
// FILE NAME: CIDKernel_ConsoleHelpers.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/16/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements some helper classes for the console implementation.
//  You'd think that console support would be relatively straightforward, but
//  its one of the most complicated parts of the Win32 driver.
//
// CAVEATS/GOTCHAS:
//
//  1)  This header is internal to the Win32 driver, and only used by the
//      console implementation files, so it can use Win32 specific types if
//      needed.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//   CLASS: TWin32ConCursor
//  PREFIX: curs
// ---------------------------------------------------------------------------
class TWin32ConCursor
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TWin32ConCursor
        (
            const   HANDLE                  hCon
        );

        TWin32ConCursor(const TWin32ConCursor&) = delete;

        ~TWin32ConCursor();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TWin32ConCursor& operator=(const TWin32ConCursor&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const COORD& CurPosition() const;

        tCIDLib::TVoid CR();

        tCIDLib::TVoid DecPosition();

        tCIDLib::TVoid IncPosition();

        tCIDLib::TVoid LF();

        tCIDLib::TVoid MoveLeftBy
        (
            const   tCIDLib::TCard4         c4By
        );

        tCIDLib::TVoid MoveRightBy
        (
            const   tCIDLib::TCard4         c4By
        );

        const COORD& OrgPosition() const;

        tCIDLib::TVoid ResetFromOrg
        (
            const   tCIDLib::TCard4         c4FromOrg
        );

        tCIDLib::TVoid ResetToOrg();

        tCIDLib::TVoid SetNewPosition
        (
            const   COORD&                  NewPos
        );

        tCIDLib::TVoid SyncConsolePos();

        tCIDLib::TVoid UpdateFromConsole();


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid MoveToCurPos();

        tCIDLib::TVoid Scroll();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_Attrs
        //      The defaults attributes to use when outputting or scrolling.
        //
        //  m_BufSize
        //      The current buffer size, which is required to handle the
        //      wrap around effect.
        //
        //  m_CurPos
        //      The current cursor position
        //
        //  m_hCon
        //      The raw output console handle that we need to move the
        //      cursor around.
        //
        //  m_OrgPos
        //      The original position, where the input started at. We cannot
        //      go back past here.
        // -------------------------------------------------------------------
        WORD    m_Attrs;
        COORD   m_BufSize;
        COORD   m_CurPos;
        HANDLE  m_hCon;
        COORD   m_OrgPos;
};

inline const COORD& TWin32ConCursor::CurPosition() const
{
    return m_CurPos;
}

inline const COORD& TWin32ConCursor::OrgPosition() const
{
    return m_OrgPos;
}

