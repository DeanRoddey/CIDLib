//
// FILE NAME: CIDMacroEng.cpp
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
//  This is the main file of the facility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  For this file we want to pre instantiate some collections, but not for any
//  other files. Only this file defines this toke, which prevents the use of the
//  extern declaration.
// ---------------------------------------------------------------------------
#define CIDMACROENG_PREINST  1



// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDMacroEng_.hpp"



// ---------------------------------------------------------------------------
//  Pre-generate some templates
// ---------------------------------------------------------------------------
template class TKeyedHashSet<TMEngMemberInfo,TString,TStringKeyOps>;
template class TKeyedHashSet<TMEngMethodInfo,TString,TStringKeyOps>;
template class TRefKeyedHashSet<TMEngLiteralVal,TString,TStringKeyOps>;
template class TRefVector<TMEngMethodImpl>;
template class TRefVector<TMEngMethodInfo>;
template class TRefVector<TMEngMemberInfo>;

template class TRefKeyedHashSet<TMEngClassInfo,TString,TStringKeyOps>;
template class TRefVector<TMEngClassInfo>;
template class TRefVector<TMEngClassVal>;

template class TRefVector<TMEngCallStackItem>;
template class TVector<TMEngLocalInfo>;
template class TRefVector<TMEngStringVal>;


// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDMacroEng& facCIDMacroEng()
{
    static TFacCIDMacroEng* pfacThis = new TFacCIDMacroEng();
    return *pfacThis;
}
