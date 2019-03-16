//
// FILE NAME: CIDMacroEng_Constant.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/15/2003
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
//  This file is the constants header for this facility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


namespace kMacroEng
{
    // -----------------------------------------------------------------------
    //  The number of indices that an opcode can support
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4       c4OpIndices = 4;


    // -----------------------------------------------------------------------
    //  The maximum parms a method can take
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4       c4MaxMethodParms = 32;


    // -----------------------------------------------------------------------
    //  Indicates a bad id (of a parm, method, class, etc...)
    // -----------------------------------------------------------------------
    const tCIDLib::TCard2       c2BadId = 0xFFFF;


    // -----------------------------------------------------------------------
    //  Operators are implemented via specially named methods that a class
    //  must implement in order to support that operator.
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const   pszOp_Add       = L"Add";
    const tCIDLib::TCh* const   pszOp_And       = L"And";
    const tCIDLib::TCh* const   pszOp_AndEq     = L"AndEq";
    const tCIDLib::TCh* const   pszOp_Dec       = L"Dec";
    const tCIDLib::TCh* const   pszOp_Divide    = L"Div";
    const tCIDLib::TCh* const   pszOp_DivideEq  = L"DivEq";
    const tCIDLib::TCh* const   pszOp_Equal     = L"Equal";
    const tCIDLib::TCh* const   pszOp_GtThan    = L"GtThan";
    const tCIDLib::TCh* const   pszOp_GtThanEq  = L"GtThanEq";
    const tCIDLib::TCh* const   pszOp_Inc       = L"Inc";
    const tCIDLib::TCh* const   pszOp_LsThan    = L"LsThan";
    const tCIDLib::TCh* const   pszOp_LsThanEq  = L"LsThanEq";
    const tCIDLib::TCh* const   pszOp_MinusEq   = L"MinusEq";
    const tCIDLib::TCh* const   pszOp_ModDiv    = L"ModDiv";
    const tCIDLib::TCh* const   pszOp_ModDivEq  = L"ModDivEq";
    const tCIDLib::TCh* const   pszOp_Multiply  = L"Mul";
    const tCIDLib::TCh* const   pszOp_MultiplyEq= L"MulEq";
    const tCIDLib::TCh* const   pszOp_Or        = L"Or";
    const tCIDLib::TCh* const   pszOp_OrEq      = L"OrEq";
    const tCIDLib::TCh* const   pszOp_PlusEq    = L"PlusEq";
    const tCIDLib::TCh* const   pszOp_PrefDec   = L"PrefDec";
    const tCIDLib::TCh* const   pszOp_PrefInc   = L"PrefInc";
    const tCIDLib::TCh* const   pszOp_Subtract  = L"Sub";
    const tCIDLib::TCh* const   pszOp_Xor       = L"Xor";
    const tCIDLib::TCh* const   pszOp_XorEq     = L"XorEq";


    // -----------------------------------------------------------------------
    //  Some stats cache paths for the stats we provide
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const   pszStat_MEng_EngInstCount = L"/Stats/MEng/EngInstCount";


    // -----------------------------------------------------------------------
    //  We allow for dynamic type references. Normally it is a literal string or a
    //  literal string defined in some already imported class. But we also allow for
    //  a special one that can be set on the macro engine by the containing program.
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const   pszSpecDynTypRef(L"$DynTypeRef");
}


