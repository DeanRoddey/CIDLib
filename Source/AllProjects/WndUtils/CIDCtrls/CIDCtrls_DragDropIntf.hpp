//
// FILE NAME: CIDCtrls_DropDropIntf.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/16/2015
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
//  This file implements a mixin interface that lets any given window support
//  drag and drop. The window class just mixes in this interface and overrides
//  the virtual methods. This mixin class handles the OS internals of dealing
//  with drag an drop interactions and just calls the virtual methods as required.
//
// CAVEATS/GOTCHAS:
//
//  1)  The window must call InitDragAndDrop() to initialize the support and must
//      call TermDragAndDrop() to clean it up. Generally it will do tese calls
//      in the bCreated() and Destroyed() callbacks, to insure it's done in the
//      same thread. I.e. don't just wait for the destructor of the window to do
//      the cleanup.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: MDragAndDrop
// PREFIX: mdad
// ---------------------------------------------------------------------------
class CIDCTRLSEXP MDragAndDrop
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        MDragAndDrop(const MDragAndDrop&) = delete;

        virtual ~MDragAndDrop();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        MDragAndDrop& operator=(const MDragAndDrop&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid InitDragAndDrop
        (
                    TWindow&                wndCont
        );

        tCIDLib::TVoid TermDragAndDrop
        (
                    TWindow&                wndCont
        );


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid DragExit() = 0;

        virtual tCIDLib::TBoolean bCanAcceptFiles
        (
            const   tCIDLib::TStrList&      colFiles
            , const tCIDLib::TStrHashSet&   colExts
        )   const = 0;

        virtual tCIDLib::TBoolean bDragDropped
        (
            const   TPoint&                 pntAt
            , const tCIDLib::TStrList&      colFiles
            , const tCIDLib::TStrHashSet&   colExts
        ) = 0;

        virtual tCIDLib::TBoolean bDragEnter
        (
            const   TPoint&                 pntAt
            , const tCIDLib::TStrList&      colFiles
            , const tCIDLib::TStrHashSet&   colExts
        )   = 0;

        virtual tCIDLib::TBoolean bDragOver
        (
            const   TPoint&                 pntAt
            , const tCIDLib::TStrList&      colFiles
            , const tCIDLib::TStrHashSet&   colExts
        )   = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        MDragAndDrop();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pData
        //      This is an opaque pointer which we we use internally to store the
        //      data required.
        // -------------------------------------------------------------------
        tCIDLib::TVoid*     m_pData;

};

#pragma CIDLIB_POPPACK


