//
// FILE NAME: CIDKernel_Version.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/06/1996
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
//  This file contains a couple of version constants that are used to set
//  the release version of CIDLib. All of the CIDLib facilities will use these
//  as their version. We set their values from defines that come in from the
//  CIDBuild build tool. Those values in turn came from the user's environment,
//  so that all they have to do in order to move to the next version is to
//  change their environment and rebuild and we'll pick them up.
//
//  This was not put in the CIDKernel_Constant.hpp file, where stuff like this
//  normally goes, because that file is also shared by the CIDBuild program.
//  Since CIDBuild sets these constants, there is a bootstrapping issue, so we
//  move them out to their own file.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

namespace kCIDLib
{
    // -----------------------------------------------------------------------
    //  These represent the CIDLib release version, and are used anywhere in
    //  the CIDLib code that version is stored, displayed, etc... These come
    //  in from the build environment, which we then set here and all other
    //  CIDLib code uses these.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4           c4MajVersion    = CID_MAJVER;
    const tCIDLib::TCard4           c4MinVersion    = CID_MINVER;
    const tCIDLib::TCard4           c4Revision      = CID_REVISION;
    const tCIDLib::TCh* const       pszVersion      = CIDLib_MakeLStr3(CID_VERSTRING);
    const tCIDLib::TCh* const       pszVerSuffix    = CIDLib_MakeLStr3(CID_VERSUFF);
}

