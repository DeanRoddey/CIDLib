//
// FILE NAME: CIDDocComp.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/10/1997
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
//  This is the main header for the program. It brings in any other headers
//  that are needed. All our cpp files include this.
//
//  This program is a documentation compiler for CIDLib. The documentation files
//  are in AllProjects\Docs\CIDDocs. This compiler processes those files and spits
//  out HTML content for accessing via a web browser. The output is under the build
//  output directory, in a CIDDocs.out directory. Point the browser at the
//  CIDDocs.html file as the starting point.
//
//  Some very simple javascript is used to manage links and page loading and such.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

// -----------------------------------------------------------------------------
//  Include underlying headers.
// -----------------------------------------------------------------------------
#include    "CIDLib.hpp"
#include    "CIDEncode.hpp"
#include    "CIDXML.hpp"


// -----------------------------------------------------------------------------
//  Include our out facility for internal use
// -----------------------------------------------------------------------------
#include    "CIDDocComp_ErrorIds.hpp"
#include    "CIDDocComp_MessageIds.hpp"
#include    "CIDDocComp_Info.hpp"

#include    "CIDDocComp_HelpNode.hpp"
#include    "CIDDocComp_BasePage.hpp"
#include    "CIDDocComp_Topics.hpp"
#include    "CIDDocComp_HelpPage.hpp"

#include    "CIDDocComp_ThisFacility.hpp"


// -----------------------------------------------------------------------------
//  Export the facility object internally
// -----------------------------------------------------------------------------
extern TFacCIDDocComp facCIDDocComp;
