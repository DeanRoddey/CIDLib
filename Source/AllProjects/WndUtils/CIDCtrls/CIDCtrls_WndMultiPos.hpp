//
// FILE NAME: CIDCtrls_WndMultiPos.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/19/2015
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
//  This file implements a wrapper around the 'deferred window position' stuff, to
//  support moving/sizing multiple windows all at once. It supports more than that,
//  but that's all that we support currently.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TWndMultiPos
// PREFIX: wmpos
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TWndMultiPos
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TWndMultiPos() = delete;

        TWndMultiPos
        (
            const   tCIDLib::TCard4         c4Count
        );

        TWndMultiPos(const TWndMultiPos&) = delete;

        ~TWndMultiPos();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TWndMultiPos& operator=(const TWndMultiPos&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddWindow
        (
            const   TWindow&                wndToAdd
            , const TArea&                  areaNew
        );

        tCIDLib::TVoid AddWindow
        (
            const   TWindow&                wndToAdd
            , const TPoint&                 pntNew
        );

        tCIDLib::TVoid AddWindow
        (
            const   TWindow&                wndToAdd
            , const TSize&                  szNew
        );

        tCIDLib::TVoid Complete();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pData
        //      An opaque data pointer, for the OS info we have to track.
        // -------------------------------------------------------------------
        tCIDLib::TVoid*     m_pData;
};

#pragma CIDLIB_POPPACK

