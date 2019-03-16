//
// FILE NAME: CIDLib_CommCol.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/31/2011
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
//  This is the header for the common collections cpp file. There are
//  some types of collections that are very common and we don't want to
//  just continually generate templatized collections for them. So we
//  create specific implementations that derive from the same simple
//  templatized base classes, but that then provide non-templatized
//  interfaces and just use the actual templatized class internally
//  as the implementation.
//
//  This cuts down on a lot of bloat since these types of collections,
//  such as vectors of strings, hash sets of strings, keyed hash sets
//  of key/value pairs, and so forth are so widely used. Since these
//  do derive from the same templatized collection base classes, they
//  can be passed to anything that can accept those basic interfaces,
//  which is a fair bit of stuff.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once
