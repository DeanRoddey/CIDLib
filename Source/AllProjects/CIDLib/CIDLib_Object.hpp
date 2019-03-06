//
// FILE NAME: CIDLib_Object.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/06/1992
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDLib_Object.Cpp file. This file
//  implements the base CIDLib class, TObject, from which all others are
//  derived. This class defines the RTTI interface that all must use.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class CIDLIBEXP TClass;


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TObject
// PREFIX: obj
// ---------------------------------------------------------------------------
class CIDLIBEXP TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TClass& clsThis();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        virtual ~TObject();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bIsDescendantOf
        (
            const   TClass&              clsTarget
        )   const = 0;

        virtual const TClass& clsIsA() const = 0;

        virtual const TClass& clsParent() const = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsA
        (
            const   TClass&              clsTest
        )   const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden Constructors and operators
        // -------------------------------------------------------------------
        TObject();
        TObject(const TObject&);
};

#pragma CIDLIB_POPPACK

