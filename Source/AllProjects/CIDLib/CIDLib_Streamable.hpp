//
// FILE NAME: CIDLib_Streamable.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/25.96
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
//  This is the header for the CIDLib_Streamable.Cpp file. This file
//  implements the MStreamable mixin class. This class is mixed in via
//  multiple inheritance to any class that needs to be streamed to or from
//  a binary stream (or derived) object.
//
//  Also provided here are the global streaming operators. Any class that
//  implements the streamable interface gets use of these operators.
//
// CAVEATS/GOTCHAS:
//
//  1)  By providing the same RTTI public methods here as TObject, we
//      insure that we can use these same capabilities (that all CIDLib
//      objects have) when we are looking at them via the streamable mixin.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class   TBinInStream;
class   TBinOutStream;
class   TClass;


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: MStreamable
// PREFIX: strmbl
// ---------------------------------------------------------------------------
class CIDLIBEXP MStreamable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        MStreamable() = default;

        virtual ~MStreamable() = default;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bIsDescendantOf
        (
            const   TClass&                 clsTarget
        )   const = 0;

        virtual const TClass& clsIsA() const = 0;

        virtual const TClass& clsParent() const = 0;


    protected   :
        // -------------------------------------------------------------------
        //  Declare our friends
        //
        //  The global methods below must have access to our protected stream
        //  methods.
        // -------------------------------------------------------------------
        friend TBinOutStream& operator<<
        (
                    TBinOutStream&          strmToWriteTo
            , const MStreamable&            strmblToWrite
        );

        friend TBinInStream& operator>>
        (
                    TBinInStream&           strmToReadFrom
            ,       MStreamable&            strmblToRead
        );


        // -------------------------------------------------------------------
        //  Hidden but available so that including classes can have default
        //  generated copy/assignment.
        // -------------------------------------------------------------------
        MStreamable(const MStreamable&) = default;
        MStreamable& operator=(const MStreamable&) = default;


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   = 0;

        virtual tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const = 0;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        NulObject(MStreamable)
};

#pragma CIDLIB_POPPACK



// ---------------------------------------------------------------------------
//  Provide the global operators that allow any streamable object to be
//  read from or written to a binary stream. This is what the outside world
//  uses.
// ---------------------------------------------------------------------------
inline TBinOutStream&
operator<<(TBinOutStream& strmTarget, const MStreamable& strmblToWrite)
{
    strmblToWrite.StreamTo(strmTarget);
    return strmTarget;
}

inline TBinInStream&
operator>>(TBinInStream& strmSource, MStreamable& strmblToFill)
{
    strmblToFill.StreamFrom(strmSource);
    return strmSource;
}


