//
// FILE NAME: CIDKernel_PlatformStrOps.Hpp
//
// AUTHOR: Will Mason
//
// CREATED: 04/27/1999
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
//  This header allows each platform to provides its own mappings of the
//  standard CIDLib raw string methods to the correct underlying C RTL
//  methods, or to its own if it wants to handle them.
//
//  A macro is provided for each operation that TRawStr needs the platform to
//  provide. Each platform just defines them to map to what it needs.
//
//  A second set are provided for a small set of short character ops that
//  we support.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#define CIDStrOp_ChToLower(c)           ::towlower(c)
#define CIDStrOp_ChToUpper(c)           ::towupper(c)
#define CIDStrOp_CnvtFloat(s,ep)        ::wcstod(s,ep)
#define CIDStrOp_FmtFloat(b,l,v,p,d,s)  ::fcvt_r(v,p,d,s,b,l)
#define CIDStrOp_IsAlpha(c)             ::iswalpha(c)
#define CIDStrOp_IsAlphaNum(c)          ::iswalnum(c)
#define CIDStrOp_IsDigit(c)             ::iswdigit(c)
#define CIDStrOp_IsLower(c)             ::iswlower(c)
#define CIDStrOp_IsPunct(c)             ::iswpunct(c)
#define CIDStrOp_IsSpace(c)             ::iswspace(c)
#define CIDStrOp_IsUpper(c)             ::iswupper(c)
#define CIDStrOp_IsXDigit(c)            ::iswxdigit(c)
#define CIDStrOp_MBToWC(t,mc,s,oc)      ::mbstowcs_s(&oc,t,mc,s,_TRUNCATE)
#define CIDStrOp_StrTok(s,ws,c)         ::wcstok_s(s,ws,c)
#define CIDStrOp_WCToMB(t,s,ob)         ::wcstombs(t,s,ob)
#define CIDStrOp_CalcMBSize(s,nb)       ::wcstombs(0, s, nb)

#define CIDSStrOp_CompareI(s1,s2)       ::strcasecmp(s1,s2)
#define CIDSStrOp_CompareN(s1,s2,n)     ::strncmp(s1,s2,n)
#define CIDSStrOp_CompareNI(s1,s2,n)    ::strncasecmp(s1,s2,n)
