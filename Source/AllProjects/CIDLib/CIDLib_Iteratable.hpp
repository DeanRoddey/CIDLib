//
// FILE NAME: CIDLib_Iteratable.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/04/2021
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2021
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This header implments the mixin class, MIteratable, which defines the
//  fundamental interface for iterating things. The TColCursor class that is
//  the basis for all of the collection cursors, implements this, as does the
//  filter iterator, and anything else that wants to provide an iterating
//  mechanism.
//
//  It has to be a template class since it has to be able to provide access
//  to the element values.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: MIteratable
// PREFIX: miter
// ---------------------------------------------------------------------------
template <class TElem> class MIteratable
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        virtual ~MIteratable() = default;


        // -------------------------------------------------------------------
        //  Public, pure virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bIsEmpty() const = 0;

        virtual tCIDLib::TBoolean bIsValid() const = 0;

        virtual tCIDLib::TBoolean bNext() = 0;

        virtual tCIDLib::TBoolean bReset() = 0;

        virtual tCIDLib::TCard4 c4ElemCount() const = 0;

        virtual const TElem& objRCur() const = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden but available so that our derived classes can have default
        //  generated copy and assignment.
        // -------------------------------------------------------------------
        MIteratable() = default;
        MIteratable(const MIteratable&) = default;
        MIteratable(MIteratable&&) = default;
        MIteratable& operator=(const MIteratable&) = default;
        MIteratable& operator=(MIteratable&&) = default;
};

#pragma CIDLIB_POPPACK
