//
// FILE NAME: CIDLib_Lockable.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/25/2019
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
//  This is the header for the CIDLib_Lockable.Cpp file. This file defines a
//  mixin interface that allows generic locking and unlocking of things. So
//  thinkgs like mutexes and critical sections implement this.
//
//  We also implement a derivative of it that uses a mutex internally and can
//  be used by other things to make themselves lockable without having to do
//  the actual grunt work code.
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
class   TClass;


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: MLockable
// PREFIX: mdup
// ---------------------------------------------------------------------------
class CIDLIBEXP MLockable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        virtual ~MLockable() = default;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bTryLock
        (
            const   tCIDLib::TCard4         c4WaitMSs
        ) = 0;

        virtual tCIDLib::TBoolean bIsDescendantOf
        (
            const   TClass&                 clsTarget
        )   const = 0;

        virtual const TClass& clsIsA() const = 0;

        virtual const TClass& clsParent() const = 0;

        virtual tCIDLib::TVoid Lock
        (
            const   tCIDLib::TCard4         c4WaitMSs
        ) = 0;

        [[nodiscard]] virtual TObject* pobjDuplicate() const = 0;

        virtual tCIDLib::TVoid Unlock() = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators, so that including classes can still
        //  have default copy/assignment
        // -------------------------------------------------------------------
        MLockable() = default;
        MLockable(const MLockable&) = default;
        MLockable(MLockable&&) = default;
        MLockable& operator=(const MLockable&) = default;
        MLockable& operator=(MLockable&&) = default;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        NulObject(MLockable)
};

#pragma CIDLIB_POPPACK
