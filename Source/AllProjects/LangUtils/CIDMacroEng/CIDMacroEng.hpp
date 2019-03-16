//
// FILE NAME: CIDMacroEng.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/11/2003
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
//  This facility implements a 'macro engine'. It defines an object oriented
//  development language which is pseudo compiled to a high level opcode
//  format, the virtual machine to interpret those opcodes, the core classes
//  that represent fundamental values (ints, cards, strings, etc...), and
//  the runtime libraries required for macros to access system services.
//
//  The language is strictly object oriented, so it consists completely of
//  classes, which in turn consist of methods that act on their internal
//  state.
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
#if     defined(PROJ_CIDMACROENG)
#define CIDMACROENGEXP  CID_DLLEXPORT
#else
#define CIDMACROENGEXP  CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers. Note that most of what we use is brought
//  in via the private header since we just need them to provide the internal
//  implementation of the various runtime classes. We bend over backwards to
//  avoid this facility forcing our underlying headers on everything that uses
//  us since we bring in a good bit of stuff.
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ---------------------------------------------------------------------------
//  WE break a tiny bit of stuff out to the CIDLib facility so that code that
//  might indirectly invoke CML but only needs to handle possible excpetions, they
//  don't have to be dependent on us. That code will also need to bring this in
//  themselves if they don't include us themselves.
// ---------------------------------------------------------------------------
#include    "CIDLib_MacroExcept.hpp"


// ---------------------------------------------------------------------------
//  Forward ref some classes, since they must be passed to some stuff whose
//  headers come first, or are referenced in external headers.
// ---------------------------------------------------------------------------
class TCIDMacroEngine;



// ---------------------------------------------------------------------------
//  Subinclude our other headers, in the needed order.
// ---------------------------------------------------------------------------
#include    "CIDMacroEng_Constant.hpp"
#include    "CIDMacroEng_Shared.hpp"
#include    "CIDMacroEng_Type.hpp"
#include    "CIDMacroEng_ErrorIds.hpp"

#include    "CIDMacroEng_OpCode.hpp"

#include    "CIDMacroEng_NamedItem.hpp"
#include    "CIDMacroEng_ClassValue.hpp"

namespace tCIDMacroEng
{
    using TClassValList = TRefVector<TMEngClassVal>;
    using TCntValList   = TCntPtr<TClassValList>;
}

#include    "CIDMacroEng_LiteralVal.hpp"
#include    "CIDMacroEng_LocalInfo.hpp"
#include    "CIDMacroEng_ParmInfo.hpp"
#include    "CIDMacroEng_MemberInfo.hpp"
#include    "CIDMacroEng_MethodInfo.hpp"
#include    "CIDMacroEng_MethodImpl.hpp"
#include    "CIDMacroEng_ClassInfo.hpp"
#include    "CIDMacroEng_MiscClasses.hpp"
#include    "CIDMacroEng_EnumClass.hpp"
#include    "CIDMacroEng_MemBufClass.hpp"
#include    "CIDMacroEng_CardClasses.hpp"
#include    "CIDMacroEng_FloatClasses.hpp"
#include    "CIDMacroEng_IntClasses.hpp"
#include    "CIDMacroEng_BooleanClass.hpp"
#include    "CIDMacroEng_TimeClass.hpp"
#include    "CIDMacroEng_KVPairClass.hpp"
#include    "CIDMacroEng_Collection.hpp"
#include    "CIDMacroEng_ArrayClass.hpp"
#include    "CIDMacroEng_VectorClass.hpp"

#include    "CIDMacroEng_BaseInfoClass.hpp"
#include    "CIDMacroEng_OutputStreamClasses.hpp"
#include    "CIDMacroEng_NamedValMapClass.hpp"
#include    "CIDMacroEng_StringClass.hpp"
#include    "CIDMacroEng_InputStreamClasses.hpp"

#include    "CIDMacroEng_ErrorHandler.hpp"
#include    "CIDMacroEng_FileResolver.hpp"
#include    "CIDMacroEng_ClassManager.hpp"
#include    "CIDMacroEng_CallStackItem.hpp"
#include    "CIDMacroEng_DebugIntf.hpp"
#include    "CIDMacroEng_StdClass.hpp"
#include    "CIDMacroEng_Engine.hpp"

#include    "CIDMacroEng_FlowCtrlItem.hpp"
#include    "CIDMacroEng_Parser.hpp"

#include    "CIDMacroEng_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the facility object lazy evaluator function. This guy is in the
//  main module, as usual.
// ---------------------------------------------------------------------------
extern CIDMACROENGEXP TFacCIDMacroEng& facCIDMacroEng();



// ---------------------------------------------------------------------------
//  For those collection types that we pre-generate in our main cpp file, we want to
//  insure that they don't get generated again in users of the DLL.
// ---------------------------------------------------------------------------
#if     !defined(CIDMACROENG_PREINST)
extern template class TKeyedHashSet<TMEngMemberInfo,TString,TStringKeyOps>;
extern template class TKeyedHashSet<TMEngMethodInfo,TString,TStringKeyOps>;
extern template class TRefKeyedHashSet<TMEngLiteralVal,TString,TStringKeyOps>;
extern template class TRefVector<TMEngMethodImpl>;
extern template class TRefVector<TMEngMethodInfo>;
extern template class TRefVector<TMEngMemberInfo>;

extern template class TRefKeyedHashSet<TMEngClassInfo,TString,TStringKeyOps>;
extern template class TRefVector<TMEngClassInfo>;
extern template class TRefVector<TMEngClassVal>;
extern template class TRefVector<TMEngCallStackItem>;

extern template class TVector<TMEngLocalInfo>;
extern template class TRefVector<TMEngStringVal>;
#endif

