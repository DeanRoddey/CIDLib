//
// FILE NAME: CIDLib_StatusController.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/21/1996
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
//  This class implements a pure virtual class called TStatusContoller. This
//  class abstracts the standard communications that occur between a program
//  and a background thread that is doing work for it. The background thread
//  needs to indicate ongoing status of the operation and completion. The
//  program just creates a simple derivaitve of this class to handle the
//  correct actions for those generic interations.
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
//   CLASS: TStatusController
//  PREFIX: statc
// ---------------------------------------------------------------------------
class CIDLIBEXP TStatusController : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TStatusController() {};

        TStatusController(const TStatusController&) = delete;

        ~TStatusController() {};


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStatusController& operator=(const TStatusController&) = delete;


        // -------------------------------------------------------------------
        //  Public, pure virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid Starting
        (
            const   TString&                strStartText
        ) = 0;

        virtual tCIDLib::TVoid WaitForComplete
        (
            const   tCIDLib::TCard4         c4MilliSecs
        ) = 0;

        virtual tCIDLib::TVoid Working
        (
            const   TString&                strStatusText
            , const tCIDLib::TCard4         c4PercentComplete
         ) = 0;

        virtual tCIDLib::TVoid WorkComplete
        (
            const   TString&                strClosingComment
        ) = 0;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TStatusController,TObject)
};

#pragma CIDLIB_POPPACK

