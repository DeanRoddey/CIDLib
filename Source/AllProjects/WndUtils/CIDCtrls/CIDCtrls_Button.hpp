//
// FILE NAME: CIDCtrls_Button.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/14/2015
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
//  Implements the common notification class for button type controls. There is no
//  common base button class. Partly because some are standard control wrappers and some
//  are custom, and also there would be so little common code that getting around the
//  first problem wouldn't be worth it.
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
//   CLASS: TButtClickInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TButtClickInfo : public TCtrlNotify
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TNotificationId    nidClick;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TButtClickInfo
        (
            const   tCIDLib::TBoolean       bState
            , const TWindow&                wndSrc
        );

        TButtClickInfo
        (
            const   TButtClickInfo&         wnotSrc
        );

        ~TButtClickInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TButtClickInfo& operator=
        (
            const   TButtClickInfo&         wnotSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bState() const;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TButtClickInfo();

        TButtClickInfo
        (
            const   tCIDLib::TBoolean       bDummy
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bState
        //      For those buttons that have boolean states, this indicates the
        //      new state. For something like a pushbutton, this has no meaning.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bState;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TButtClickInfo,TCtrlNotify)
        DefPolyDup(TButtClickInfo)
};

#pragma CIDLIB_POPPACK


