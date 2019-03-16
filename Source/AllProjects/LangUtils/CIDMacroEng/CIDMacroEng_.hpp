//
// FILE NAME: CIDMacroEng_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/11/2003
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
//  This is the main internal header for the facility. It includes the main
//  public header, any internal headers that are needed, intrafacility
//  function prototypes, intrafacility constants, and macros.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Bring in any other facility headers that we only need internally.
// ---------------------------------------------------------------------------
#include    "CIDMath.hpp"
#include    "CIDDBase.hpp"
#include    "CIDCrypto.hpp"
#include    "CIDEncode.hpp"
#include    "CIDComm.hpp"
#include    "CIDSock.hpp"
#include    "CIDNet.hpp"
#include    "CIDXML.hpp"


// ---------------------------------------------------------------------------
//  Include our own public header and any internal headers we need
// ---------------------------------------------------------------------------
#include    "CIDMacroEng.hpp"
#include    "CIDMacroEng_MessageIds.hpp"
#include    "CIDMacroEng_StringPool_.hpp"

#include    "CIDMacroEng_ASCIIClass_.hpp"
#include    "CIDMacroEng_AudioClass_.hpp"
#include    "CIDMacroEng_Base64Class_.hpp"
#include    "CIDMacroEng_MD5Classes_.hpp"
#include    "CIDMacroEng_SHA1Classes_.hpp"
#include    "CIDMacroEng_SpeechClass_.hpp"
#include    "CIDMacroEng_DBaseClasses_.hpp"
#include    "CIDMacroEng_StringTokClass_.hpp"
#include    "CIDMacroEng_SysInfoClass_.hpp"
#include    "CIDMacroEng_FileSysClass_.hpp"
#include    "CIDMacroEng_CryptoKey_.hpp"
#include    "CIDMacroEng_BinFileClass_.hpp"
#include    "CIDMacroEng_USB_HID_.hpp"
#include    "CIDMacroEng_NetClasses_.hpp"
#include    "CIDMacroEng_AsyncHTTP_.hpp"
#include    "CIDMacroEng_MPartMIME_.hpp"
#include    "CIDMacroEng_JSONClasses_.hpp"
#include    "CIDMacroEng_CommClasses_.hpp"
#include    "CIDMacroEng_SockClasses_.hpp"
#include    "CIDMacroEng_RandomClasses_.hpp"
#include    "CIDMacroEng_XMLClasses_.hpp"


// ---------------------------------------------------------------------------
//  This is the intra-facilities constants namespace.
// ---------------------------------------------------------------------------
namespace kCIDMacroEng_
{
    const tCIDLib::TCh* const   pszNoParentClass = L"$NoParentClass$";
}


