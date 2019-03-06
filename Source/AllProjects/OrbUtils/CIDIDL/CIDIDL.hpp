//
// FILE NAME: CIDIDL.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/08/2000
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
//
// DESCRIPTION:
//
//  This is the main header of the CIDLib IDL compiler. It does the usual
//  things that a CIDLib based facility header does.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Include our underlying headers
// ---------------------------------------------------------------------------
#include    "CIDXML.hpp"

#include    "CIDCrypto.hpp"


// ---------------------------------------------------------------------------
//  And nclude our base headers in the needed order
// ---------------------------------------------------------------------------
#include    "CIDIDL_ErrorIds.hpp"
#include    "CIDIDL_MessageIds.hpp"
#include    "CIDIDL_ParmInfo.hpp"
#include    "CIDIDL_StructInfo.hpp"


// ---------------------------------------------------------------------------
//  Types namespace
// ---------------------------------------------------------------------------
namespace tCIDIDL
{
    // -----------------------------------------------------------------------
    //  The actions we support
    // -----------------------------------------------------------------------
    enum class EActions
    {
        None
        , IdGen
        , CodeGen

        , Count
    };


    // -----------------------------------------------------------------------
    //  If we have AltText and if so how it is defined. If a pattern, then we insert the
    //  base name into a pattern defined at the enum level.
    // -----------------------------------------------------------------------
    enum class EAltTextSrcs
    {
        None
        , Inline
        , Pattern

        , Count
    };


    // -----------------------------------------------------------------------
    //  The types of output we generate
    // -----------------------------------------------------------------------
    enum class EOutputs
    {
        Client
        , Server
        , Global
    };


    // -----------------------------------------------------------------------
    //  The basic types of enums the IDL can define
    // -----------------------------------------------------------------------
    enum class EEnumTypes
    {
        Bmp
        , Std

        , Count
    };


    // -----------------------------------------------------------------------
    //  The types of inc/dec operators to generate
    // -----------------------------------------------------------------------
    enum class EIncDecOps
    {
        None
        , Inc
        , Both

        , Count
    };


    // -----------------------------------------------------------------------
    //  For the main text, we can have either an inline source or we can load the values
    //  from loadable text resources.
    // -----------------------------------------------------------------------
    enum class ETextSrcs
    {
        None
        , Inline
        , LoadRes
    };



    // -----------------------------------------------------------------------
    //  A list of method info objects, to pass into code generators when
    //  generating methods.
    // -----------------------------------------------------------------------
    typedef TVector<TCGenMethodParm>    TParmList;
}


// ---------------------------------------------------------------------------
//  Constants namespace
// ---------------------------------------------------------------------------
namespace kCIDIDL
{
    // -----------------------------------------------------------------------
    //  Some constants used by the code gen code
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const   pszClientBase   = L"TOrbClientBase";
    const tCIDLib::TCh* const   pszServerBase   = L"TOrbServerBase";
}


// ---------------------------------------------------------------------------
//  And sub-include our higher level headers in the needed order
// ---------------------------------------------------------------------------
#include    "CIDIDL_EnumInfo.hpp"
#include    "CIDIDL_CodeGenerator.hpp"
#include    "CIDIDL_CppGenerator.hpp"

#include    "CIDIDL_ThisFacility.hpp"



// ---------------------------------------------------------------------------
//  Make the facility object visible to all the program files. It is actually
//  declared in the main cpp file.
// ---------------------------------------------------------------------------
extern TFacCIDIDL   facCIDIDL;


// ---------------------------------------------------------------------------
//  A debug-only macro that is used in a number of files. It will assert
//  that an element of the XML tree has an expected element type name.
// ---------------------------------------------------------------------------
#if CID_DEBUG_ON
#define AssertElemName(xtn,nam) \
if ((xtn).strQName() != nam) \
{ \
    facCIDIDL.ThrowErr \
    ( \
        CID_FILE \
        , CID_LINE \
        , kIDLErrs::errcInp_ExpectedElem \
        , tCIDLib::ESeverities::Failed \
        , tCIDLib::EErrClasses::Format \
        , TString(nam) \
        , (xtn).strQName() \
    ); \
}
#else
#define AssertElemName(xtn,nam) {}
#endif

#if CID_DEBUG_ON
#define AssertElemName2(xtn,nam1,nam2) \
if (((xtn).strQName() != nam1) \
&&  ((xtn).strQName() != nam2)) \
{ \
    facCIDIDL.ThrowErr \
    ( \
        CID_FILE \
        , CID_LINE \
        , kIDLErrs::errcInp_ExpectedElem \
        , tCIDLib::ESeverities::Failed \
        , tCIDLib::EErrClasses::Format \
        , (TString(nam1) + TString(L" or ") + TString(nam2)) \
        , (xtn).strQName() \
    ); \
}
#else
#define AssertElemName2(xtn,nam1,nam2) {}
#endif

