//
// FILE NAME: CIDNet.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/05/2000
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
//  This is the main public header for the facility. It is the single point
//  of contact for the outside world. By including it, they get what we
//  have to offer.
//
//  This is a bit of a grab-bag facility of Net oriented protocol implemnetations.
//  HTTP, SMTP, JSON, and SMTP are all implemented here.
//
//  The HTTP implementation is not a super-deep one, so it could still use some
//  work.
//
//  The HTTP parser is implemented in terms of a 'core parse' because there are
//  some other protocols, such as SIP, that are very similar and this should
//  allow SIP to be parsed with minimal extra work and redundancy.
//
//  We also create a derivative of the XML parser's entity source class that
//  allows the XML parser to access URL based resources and parse them.
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
#if     defined(PROJ_CIDNET)
#define CIDNETEXP   CID_DLLEXPORT
#else
#define CIDNETEXP   CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers if not already included
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"
#include    "CIDSock.hpp"
#include    "CIDSChan.hpp"
#include    "CIDXML.hpp"



// ---------------------------------------------------------------------------
//  Include the fundamental headers first
// ---------------------------------------------------------------------------
#include    "CIDNet_ErrorIds.hpp"
#include    "CIDNet_Shared.hpp"
#include    "CIDNet_Constant.hpp"
#include    "CIDNet_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the lazy evalution method for the facility object.
// ---------------------------------------------------------------------------
extern CIDNETEXP TFacCIDNet& facCIDNet();


// ---------------------------------------------------------------------------
//  And subinclude our other headers
// ---------------------------------------------------------------------------
#include    "CIDNet_CoreParser.hpp"
#include    "CIDNet_HTTPClient.hpp"
#include    "CIDNet_SMTPClient.hpp"
#include    "CIDNet_JSONParser.hpp"
#include    "CIDNet_XMLURLEntitySrc.hpp"



// ---------------------------------------------------------------------------
//  For those collection types that we pre-generate in our main cpp file, we want to
//  insure that they don't get generated again in users of the DLL.
// ---------------------------------------------------------------------------
#if     !defined(CIDNET_PREINST)
extern template class TRefQueue<TEmailMsg>;
extern template class TRefVector<TEmailAttachment>;
extern template class TRefVector<TJSONValue>;
#endif


