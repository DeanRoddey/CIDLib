//
// FILE NAME: CIDNet_Constant.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/17/2005
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
    const tCIDLib::TCard4 c4HTTPStatus_Continue         = 100;
    const tCIDLib::TCard4 c4HTTPStatus_SwitchProto      = 101;
    const tCIDLib::TCard4 c4HTTPStatus_OK               = 200;
    const tCIDLib::TCard4 c4HTTPStatus_Created          = 201;
    const tCIDLib::TCard4 c4HTTPStatus_Accepted         = 202;
    const tCIDLib::TCard4 c4HTTPStatus_MovedPerm        = 301;
    const tCIDLib::TCard4 c4HTTPStatus_MovedTemp        = 302;
    const tCIDLib::TCard4 c4HTTPStatus_Found            = 303;
    const tCIDLib::TCard4 c4HTTPStatus_NotModified      = 304;
    const tCIDLib::TCard4 c4HTTPStatus_TempRedir        = 307;
    const tCIDLib::TCard4 c4HTTPStatus_PermRedir        = 308;
    const tCIDLib::TCard4 c4HTTPStatus_BadRequest       = 400;
    const tCIDLib::TCard4 c4HTTPStatus_Unauthorized     = 401;
    const tCIDLib::TCard4 c4HTTPStatus_NotFound         = 404;
    const tCIDLib::TCard4 c4HTTPStatus_ReqTimeout       = 408;
    const tCIDLib::TCard4 c4HTTPStatus_UnsupMedia       = 415;
    const tCIDLib::TCard4 c4HTTPStatus_ParamNotKnown    = 451;
    const tCIDLib::TCard4 c4HTTPStatus_SessNotFound     = 454;
    const tCIDLib::TCard4 c4HTTPStatus_NotValidForState = 455;
    const tCIDLib::TCard4 c4HTTPStatus_BadHdrForResource= 456;
    const tCIDLib::TCard4 c4HTTPStatus_InvalidRange     = 457;
    const tCIDLib::TCard4 c4HTTPStatus_UnsupTransport   = 461;
    const tCIDLib::TCard4 c4HTTPStatus_SrvError         = 500;
    const tCIDLib::TCard4 c4HTTPStatus_NotImpl          = 501;
    const tCIDLib::TCard4 c4HTTPStatus_VerNotSup        = 505;
    const tCIDLib::TCard4 c4HTTPStatus_OptionNotSup     = 551;


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
    const tCIDLib::TIPPortNum   ippnHTTP        = 80;
    const tCIDLib::TIPPortNum   ippnHTTPS       = 443;
    const tCIDLib::TIPPortNum   ippnNEWS        = 119;
    const tCIDLib::TIPPortNum   ippnSIP         = 5060;
    const tCIDLib::TIPPortNum   ippnSMTP        = 25;
    const tCIDLib::TIPPortNum   ippnSMTPS       = 587;
    const tCIDLib::TIPPortNum   ippnWS          = 80;
    const tCIDLib::TIPPortNum   ippnWSS         = 443;
}


