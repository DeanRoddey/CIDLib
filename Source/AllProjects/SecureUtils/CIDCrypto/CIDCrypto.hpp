//
// FILE NAME: CIDCrypto.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/21/1997
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
//  have to offer. It also insures consistent order of inclusion which
//  is needed for the precompiled headers stuff to work.
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
#if     defined(PROJ_CIDCRYPTO)
#define CIDCRYPTEXP CID_DLLEXPORT
#else
#define CIDCRYPTEXP CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ---------------------------------------------------------------------------
//  Include our public headers
// ---------------------------------------------------------------------------
#include "CIDCrypto_Type.hpp"
#include "CIDCrypto_Constant.hpp"
#include "CIDCrypto_ErrorIds.hpp"
#include "CIDCrypto_Key.hpp"
#include "CIDCrypto_BlockEncrypt.hpp"
#include "CIDCrypto_StreamEncrypt.hpp"
#include "CIDCrypto_AES.hpp"
#include "CIDCrypto_Blowfish.hpp"
#include "CIDCrypto_Xor.hpp"
#include "CIDCrypto_MsgHash.hpp"
#include "CIDCrypto_Hashers.hpp"
#include "CIDCrypto_MD5Hash.hpp"
#include "CIDCrypto_MD5.hpp"
#include "CIDCrypto_SHA1Hash.hpp"
#include "CIDCrypto_SHA1.hpp"
#include "CIDCrypto_SHA256Hash.hpp"
#include "CIDCrypto_SHA256.hpp"
#include "CIDCrypto_UniqueId.hpp"
#include "CIDCrypto_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the facility object lazy evaluator function
// ---------------------------------------------------------------------------
extern CIDCRYPTEXP TFacCIDCrypto& facCIDCrypto();
