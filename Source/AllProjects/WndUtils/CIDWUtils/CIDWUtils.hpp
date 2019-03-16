//
// FILE NAME: CIDWUtils.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/23/2015
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
//  This is the main public header for the facility. It is the single point
//  of contact for the outside world. By including it, they get what we
//  have to offer.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Set up our import/export attributes
// ---------------------------------------------------------------------------
#if     defined(PROJ_CIDWUTILS)
#define CIDWUTILSEXP    DLLEXPORT
#else
#define CIDWUTILSEXP    DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our needed underlying headers.
// ---------------------------------------------------------------------------
#include    "CIDMData.hpp"
#include    "CIDCtrls.hpp"


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class TCommPortCfg;


// ---------------------------------------------------------------------------
//  Facilities namespace
// ---------------------------------------------------------------------------
namespace kCIDWUtils
{
};


// ---------------------------------------------------------------------------
//  Include our public headers
// ---------------------------------------------------------------------------
#include    "CIDWUtils_ErrorIds.hpp"
#include    "CIDWUtils_Types.hpp"
#include    "CIDWUtils_24BitPalette.hpp"
#include    "CIDWUtils_CheckedMCList.hpp"
#include    "CIDWUtils_ColoredList.hpp"
#include    "CIDWUtils_2ColSectList.hpp"
#include    "CIDWUtils_AttrEdit.hpp"
#include    "CIDWUtils_ImgPreview.hpp"
#include    "CIDWUtils_SectionalList.hpp"
#include    "CIDWUtils_SrcEditor.hpp"
#include    "CIDWUtils_TextOut.hpp"
#include    "CIDWUtils_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the facility object lazy evaluator function.
// ---------------------------------------------------------------------------
extern CIDWUTILSEXP TFacCIDWUtils& facCIDWUtils();

