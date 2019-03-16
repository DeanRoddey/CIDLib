//
// FILE NAME: CIDObjStore.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/16/2002
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
//  This is the main header of the CIDLib object store engine. It provides a simple store
//  for object oriented data, passed on a heirarchical 'path' to represent each object
//  stored. So you might have a path like:
//
//      /Windows/Dialogs
//
//  under which would store a set of objects, each with it's own name under this path,
//  for dialog positions, so that they could be opened at the same place again. So it
//  would be something like:
//
//      /Windows/Dialogs/Config
//      /Windows/Dialogs/Options
//      /Windows/Dialogs/FileOpen
//
//  So each dialog has its own storage slot under the /Windows/Dialogs path. So it is
//  much like a disk directory. You can find all of the objects under a particular path
//  or iterate through the hierarchy just like with a disk directory.
//
//  The key information is kept in memory, and not stored, since it can, and is, rebuilt
//  from the store data upon startup. This avoids storing two pieces of information that
//  could get out of sync. The key is a keyed hash set, in which the key is the path of
//  the object. So all access is by the path.
//
//  Note that this implies some overhead. But this object store is definitely NOT intended
//  for rapid access. It's for storing configuration data, window position data, application
//  state data, and so forth, i.e. stuff that doesn't change rapidly, and generally is read
//  on program startup, and stored periodically or upon program shutdown or when modified.
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
#if     defined(PROJ_CIDOBJSTORE)
#define CIDOBJSTOREEXP CID_DLLEXPORT
#else
#define CIDOBJSTOREEXP CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include required underlying headers
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"



// ---------------------------------------------------------------------------
//  And sub-include our core headers in the needed order
// ---------------------------------------------------------------------------
#include    "CIDObjStore_ErrorIds.hpp"
#include    "CIDObjStore_MessageIds.hpp"
#include    "CIDObjStore_Constant.hpp"
#include    "CIDObjStore_Type.hpp"
#include    "CIDObjStore_ObjStore.hpp"
#include    "CIDObjStore_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the facility object lazy eval function
// ---------------------------------------------------------------------------
extern CIDOBJSTOREEXP TFacCIDObjStore& facCIDObjStore();


