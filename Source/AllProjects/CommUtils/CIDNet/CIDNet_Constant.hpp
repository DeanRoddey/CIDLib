//
// FILE NAME: CIDNet_Constant.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/17/2005
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
//  This is the constants header for this facility
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

namespace kCIDNet
{
    // -----------------------------------------------------------------------
    //  Common HTTP status codes
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TCard4 c4HTTPStatus_Continue         = 100;
    constexpr tCIDLib::TCard4 c4HTTPStatus_SwitchProto      = 101;
    constexpr tCIDLib::TCard4 c4HTTPStatus_OK               = 200;
    constexpr tCIDLib::TCard4 c4HTTPStatus_Created          = 201;
    constexpr tCIDLib::TCard4 c4HTTPStatus_Accepted         = 202;
    constexpr tCIDLib::TCard4 c4HTTPStatus_MovedPerm        = 301;
    constexpr tCIDLib::TCard4 c4HTTPStatus_MovedTemp        = 302;
    constexpr tCIDLib::TCard4 c4HTTPStatus_Found            = 303;
    constexpr tCIDLib::TCard4 c4HTTPStatus_NotModified      = 304;
    constexpr tCIDLib::TCard4 c4HTTPStatus_TempRedir        = 307;
    constexpr tCIDLib::TCard4 c4HTTPStatus_PermRedir        = 308;
    constexpr tCIDLib::TCard4 c4HTTPStatus_BadRequest       = 400;
    constexpr tCIDLib::TCard4 c4HTTPStatus_Unauthorized     = 401;
    constexpr tCIDLib::TCard4 c4HTTPStatus_NotFound         = 404;
    constexpr tCIDLib::TCard4 c4HTTPStatus_ReqTimeout       = 408;
    constexpr tCIDLib::TCard4 c4HTTPStatus_UnsupMedia       = 415;
    constexpr tCIDLib::TCard4 c4HTTPStatus_ParamNotKnown    = 451;
    constexpr tCIDLib::TCard4 c4HTTPStatus_SessNotFound     = 454;
    constexpr tCIDLib::TCard4 c4HTTPStatus_NotValidForState = 455;
    constexpr tCIDLib::TCard4 c4HTTPStatus_BadHdrForResource= 456;
    constexpr tCIDLib::TCard4 c4HTTPStatus_InvalidRange     = 457;
    constexpr tCIDLib::TCard4 c4HTTPStatus_UnsupTransport   = 461;
    constexpr tCIDLib::TCard4 c4HTTPStatus_SrvError         = 500;
    constexpr tCIDLib::TCard4 c4HTTPStatus_NotImpl          = 501;
    constexpr tCIDLib::TCard4 c4HTTPStatus_VerNotSup        = 505;
    constexpr tCIDLib::TCard4 c4HTTPStatus_OptionNotSup     = 551;


    // -----------------------------------------------------------------------
    //  Some HTTP constants
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const   pszDefHTTPEncoding = L"ISO-8859-1";


    // -----------------------------------------------------------------------
    //  HTTP operations
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const   pszHTTP_GET     = L"GET";
    const tCIDLib::TCh* const   pszHTTP_HEAD    = L"HEAD";
    const tCIDLib::TCh* const   pszHTTP_POST    = L"POST";
    const tCIDLib::TCh* const   pszHTTP_PUT     = L"PUT";


    // -----------------------------------------------------------------------
    //  Some standard port numbers
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TIPPortNum   ippnHTTP        = 80;
    constexpr tCIDLib::TIPPortNum   ippnHTTPS       = 443;
    constexpr tCIDLib::TIPPortNum   ippnNEWS        = 119;
    constexpr tCIDLib::TIPPortNum   ippnSIP         = 5060;
    constexpr tCIDLib::TIPPortNum   ippnSMTP        = 25;
    constexpr tCIDLib::TIPPortNum   ippnSMTPS       = 587;
    constexpr tCIDLib::TIPPortNum   ippnWS          = 80;
    constexpr tCIDLib::TIPPortNum   ippnWSS         = 443;
}


