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


namespace tCIDDocComp
{
    // ------------------------------------------------------------------------
    //  The types of general markup nodes we support in those places where we
    //  allow open ended text content.
    // ------------------------------------------------------------------------
    enum ETypes
    {
        EType_None

        , EType_Bold
        , EType_Break
        , EType_Code
        , EType_DIV
        , EType_Image
        , EType_Indent
        , EType_InlineNote
        , EType_Link
        , EType_List
        , EType_ListItem
        , EType_Note
        , EType_Paragraph
        , EType_SecTitle
        , EType_SubSecTitle
        , EType_Superscript
        , EType_Table
        , EType_TableCol
        , EType_TableRow
        , EType_Text
    };
}


// -----------------------------------------------------------------------------
//  Include our intra-facility headers
// -----------------------------------------------------------------------------
#include    "CIDDocComp_ErrorIds.hpp"
#include    "CIDDocComp_MessageIds.hpp"
#include    "CIDDocComp_HelpNode.hpp"
#include    "CIDDocComp_Pages.hpp"
#include    "CIDDocComp_ThisFacility.hpp"


// -----------------------------------------------------------------------------
//  Export the facility object internally
// -----------------------------------------------------------------------------
extern TFacCIDDocComp facCIDDocComp;
