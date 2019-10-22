//
// FILE NAME: CIDLib_RefCounted.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/17/2019
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
//  This header implments the mixin class, MRefCounted, which bestows up its
//  mixed into classes the ability to be referenced counted in a consistent way.
//
//  The constructor sets the ref count initially to 1, which is pretty common,
//  on the assumption that it will be initially owned.
//
//  Copy/assignment takes the source count. It's assumed you will have decremented
//  the count of the one being overwritten, releasing associated resources if
//  appropriate.
//
//  We will throw on ref count underflow.
//
// CAVEATS/GOTCHAS:
//
//  1)  The caller is responsible for synchronizing ref counting if that is
//      important to his usage.
//
//  2)  This class doesn't support 'self-destruction', it's for use in scenarios
//      where some containing class is reference counting the thing and handling
//      release or return to a pool or whatever when the count hits zero.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: MRefCounted
// PREFIX: mrefc
// ---------------------------------------------------------------------------
class CIDLIBEXP MRefCounted
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        virtual ~MRefCounted() = default;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bIsDescendantOf
        (
            const   TClass&                 clsTarget
        )   const = 0;

        virtual const TClass& clsIsA() const = 0;

        virtual const TClass& clsParent() const = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDecMRefCount();

        tCIDLib::TCard4 c4MRefCounCnt() const
        {
            return m_c4MRefCount;
        }

        tCIDLib::TVoid IncMRefCount();

        tCIDLib::TVoid ResetMRefCount();


    protected   :
        // -------------------------------------------------------------------
        //  Hidden but available so that our derived classes can have default
        //  generated copy and assignment.
        // -------------------------------------------------------------------
        MRefCounted() = default;
        MRefCounted(const MRefCounted&) = default;
        MRefCounted& operator=(const MRefCounted&) = default;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4RefCount
        //      The reference count. Initialized to 1.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4MRefCount = 1;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        NulObject(MRefCounted)
};

#pragma CIDLIB_POPPACK

