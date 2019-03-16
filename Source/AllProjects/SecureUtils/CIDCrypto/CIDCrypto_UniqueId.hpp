//
// FILE NAME: CIDCrypto_UniqueId.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/21/1998
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
//  This is the header for the CIDCrypto_UniqueId.Cpp file. This file
//  implements the TUniqueId namespace, which exists to produce unique id
//  strings that are used for things like software interfaces and such. The
//  id is a string in the form "1234567890123456-1234567890123456", so it is
//  a 32 digit string with a dash in the middle (i.e. 33 chars altogether.)
//
//  The value hashed is made up from a number of internal system values, which
//  includes the current time, workstation name, etc.... It is infinitesimally
//  unlikely to ever create the same hash twice.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

namespace TUniqueId
{
    CIDCRYPTEXP TMD5Hash mhashMakeId();

    CIDCRYPTEXP tCIDLib::TVoid MakeId
    (
                TString&                strToFill
    );

    CIDCRYPTEXP tCIDLib::TVoid MakeId
    (
                TMD5Hash&               mhashToFill
    );

    CIDCRYPTEXP tCIDLib::TVoid MakeSystemId
    (
                TMD5Hash&               mhashToFill
    );

    CIDCRYPTEXP TString strMakeId();
}


