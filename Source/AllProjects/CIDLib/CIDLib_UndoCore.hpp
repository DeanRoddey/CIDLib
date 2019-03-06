//
// FILE NAME: CIDLib_UndoCore.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/02/2004
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the file CIDLib_UndoCore.cpp file, which implements
//  the core part of the undo framework. This framework defines an undo
//  class which provides the management of undo information, and an abstract
//  base class that represents a single undoable operation. At this level
//  we know nothing about what is being done or undone, we just manage the
//  objects via the abstract base class. The users of this core framework
//  will define their own derivatives of the undo class which contain the
//  actual information needed to undo the operations.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


class TUndoHandler;

// ---------------------------------------------------------------------------
//   CLASS: TUndoCmd
//  PREFIX: uc
// ---------------------------------------------------------------------------
class CIDLIBEXP TUndoCmd : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TUndoCmd(const TUndoCmd&) = delete;

        ~TUndoCmd();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUndoCmd& operator=(const TUndoCmd&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bIsSame(const TUndoCmd&) const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TUndoCmd();


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TUndoCmd,TObject)
};



// ---------------------------------------------------------------------------
//   CLASS: TUndoCmdSep
//  PREFIX: uc
// ---------------------------------------------------------------------------
class CIDLIBEXP TUndoCmdSep : public TUndoCmd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TUndoCmdSep();

        TUndoCmdSep(const TUndoCmdSep&) = delete;

        ~TUndoCmdSep();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUndoCmdSep& operator=(const TUndoCmdSep&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsSame(const TUndoCmdSep&) const;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TUndoCmdSep,TUndoCmd)
};



// ---------------------------------------------------------------------------
//   CLASS: TUndoHandler
//  PREFIX: uh
// ---------------------------------------------------------------------------
class CIDLIBEXP TUndoHandler : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TUndoHandler
        (
            const   tCIDLib::TCard4         c4MaxUndo = 1024
        );

        TUndoHandler(const TUndoHandler&) = delete;

        ~TUndoHandler();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUndoHandler& operator=(const TUndoHandler&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsEmpty() const;

        tCIDLib::TBoolean bLastWithin
        (
            const   tCIDLib::TCard4         c4Millis
        )   const;

        tCIDLib::TCard4 c4Count() const;

        tCIDLib::TCard4 c4SinceLast() const;

        tCIDLib::TVoid DiscardTop();

        tCIDLib::TVoid DiscardToSep();

        TUndoCmd* pucTop();

        tCIDLib::TVoid Push
        (
                    TUndoCmd* const         pucToAdopt
            , const tCIDLib::TBoolean       bPushSep = kCIDLib::False
        );

        tCIDLib::TVoid PushSep();

        TUndoCmd* pucPop();

        tCIDLib::TVoid Reset();

        tCIDLib::TVoid SwapTop();


    private :
        // -------------------------------------------------------------------
        //  Private class types
        // -------------------------------------------------------------------
        using TUndoStack = TRefStack<TUndoCmd>;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        //
        //  m_c4MaxUndo
        //      The maximum number of undo items we will support. After this
        //      number is reached, we start throwing away the older ones,
        //      which means that those won't be undoable.
        //
        //  m_c4LastPost
        //      In order to allow for greater efficiency, we allow the users
        //      of this class to piggyback multiple operations onto the last
        //      posted item. This most common version of this is when the user
        //      is typing they won't put one item in per character when the
        //      user is typing quickly. Instead, they will check whether the
        //      last item posted was a character event, and just add another
        //      char to that same item as long as it was typed within some
        //      minimum time since the last item was posted.
        //
        //      So we set this to the current millisecond stamp each time a
        //      new item is posted to the stack, and provide a method to
        //      let the outside world check how long it's been since the last
        //      item was posted.
        //
        //  m_colUndoStack
        //      This is the undo stack. Undo operations are LIFO in nature so
        //      a stack is an optimal representation.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4LastPost;
        tCIDLib::TCard4 m_c4MaxUndo;
        TUndoStack      m_colUndoStack;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TUndoHandler,TObject)
};


// ---------------------------------------------------------------------------
//   CLASS: TUndoJanitor
//  PREFIX: jan
// ---------------------------------------------------------------------------
class CIDLIBEXP TUndoJanitor
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TUndoJanitor
        (
                    TUndoHandler* const     puhTar
        );

        TUndoJanitor(const TUndoJanitor&) = delete;

        ~TUndoJanitor();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUndoJanitor& operator=(const TUndoJanitor&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Orphan();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_puhTar
        //      The undo handler we operate on
        // -------------------------------------------------------------------
        TUndoHandler*   m_puhTar;
};

#pragma CIDLIB_POPPACK


