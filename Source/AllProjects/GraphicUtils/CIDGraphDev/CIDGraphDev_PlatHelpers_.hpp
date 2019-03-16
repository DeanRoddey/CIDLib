//
// FILE NAME: CIDGraphDev_Device.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/27/1997
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the header for the CIDGraphDev_Device.cpp file. This file implements
//  the TGraphicDevice class, which is a wrapper for the host OS graphics
//  output device handle, a DC in Windows speak. TGraphicDevice provides
//  basic graphics output mechanisms, and there are derivatives of it (defined
//  elsewhere) that are used for particular circumstances.
//
//  The only derivative defined here is the TGraphDrawDev from which are
//  derived all of the device classes that support graphics output. Other
//  derivatives of TGraphicDevice might not support any drawing, i.e. they
//  might be only for querying information.
//
//  The drawing methods are virtual so that we can support remote graphics
//  output. It's always a real device, but it provides the means to do a
//  derivative that intercepts any desired graphical operations and send them
//  to some other program for rendering. It's always a real device because
//  there is also the need to do local capabilities and queries and other
//  ops, which would be very piggy to have to do remotely. So the app would
//  keep the local device content up to date with the remote for that reason.
//
// CAVEATS/GOTCHAS:
//
//  1)  Devices are NOT threadsafe, so protect them fully if you draw to them
//      from multiple threads.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


namespace TGrDevPlatform
{
    //
    // These translate from ours to theirs
    //
    tCIDLib::TCard4 c4XlatBackMixMode
    (
        const   tCIDGraphDev::EBackMixModes     eMode
    );

    tCIDLib::TCard4 c4XlatBmpMode
    (
        const   tCIDGraphDev::EBmpModes         eMode
    );

    tCIDLib::TCard4 c4XlatClipMode
    (
        const   tCIDGraphDev::EClipModes        eMode
    );

    tCIDLib::TCard4 c4XlatEndStyle
    (
        const   tCIDGraphDev::ELineEnds         eEnd
    );

    tCIDLib::TCard4 c4XlatLineStyle
    (
        const   tCIDGraphDev::ELineStyles       eStyle
    );

    tCIDLib::TCard4 c4XlatJoinStyle
    (
        const   tCIDGraphDev::ELineJoins        eJoin
    );

    tCIDLib::TCard4 c4XlatMixMode
    (
        const   tCIDGraphDev::EMixModes         eMode
    );

    tCIDLib::TCard4 c4XlatPattern
    (
        const   tCIDGraphDev::EPatterns         ePattern
    );


    //
    // These translate from theirs to ours
    //
    tCIDGraphDev::EBackMixModes eXlatBackMixMode
    (
        const   tCIDLib::TCard4                 c4Mode
    );

    tCIDGraphDev::EMixModes eXlatMixMode
    (
        const   tCIDLib::TCard4                 c4Mode
    );
};



