//
// FILE NAME: CIDMacroEng_OpCode.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/15/2003
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
//  This is the header file for the CIDMacroEng_OpCode.cpp file, which
//  implements the TMEngOpCode class. This class is a very simple class that
//  defines the contents of an opcode. Opcodes are fixed size for simplicity,
//  though that might waste a little space. They consist of an opcode enum
//  value that indicates what code it is, and some amount of data, which are
//  usually indices into variable lists, constant lists, parameter lists, or
//  a jump forward to a new opcode. It can in some cases contain constant
//  numeric values, since it is more efficient to just build a constant into
//  an opcode if that can be done, than to create a temp for it.
//
//  There are also some non-executed opcodes, which are used to put line
//  number information into the opcode stream, for error reporting.
//
//  The opcode value is a union since it can be used for a few different
//  things. We want to keep them fast to copy, since we will have to move
//  them around during optimization phases.
//
// CAVEATS/GOTCHAS:
//
//  1)  The opcode class does not derive from TObject, since it doesn't need
//      that overhead.
//
//  2)  We also have some inlines because this class is key to performance
//      since the information in them are constantly being accessed. The
//      setting methods don't matter, since the are used during parsing, but
//      the getting methods are used at runtime.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TMEngOpCode
// PREFIX: meop
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngOpCode
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngOpCode();

        TMEngOpCode
        (
            const   TMEngOpCode&            meopToCopy
        );

        ~TMEngOpCode();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngOpCode& operator=
        (
            const   TMEngOpCode&            meopToAssign
        );

        tCIDLib::TCard2 operator[]
        (
            const   tCIDLib::TCard4         c4Index
        )   const;



        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bConvertNumeric
        (
            const   tCIDLib::TCard2         c2TargetId
        );

        tCIDLib::TBoolean bIsNoOp() const;

        tCIDLib::TBoolean bImmediate() const;

        tCIDLib::TCard1 c1Immediate() const;

        tCIDLib::TCard2 c2Immediate() const;

        tCIDLib::TCard4 c4Immediate() const;

        tCIDLib::TCard4 c4Immediate
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TCard8 c8Immediate() const;

        tCIDLib::TCh chImmediate() const;

        tCIDMacroEng::EOpCodes eOpCode() const;

        tCIDLib::TFloat4 f4Immediate() const;

        tCIDLib::TFloat8 f8Immediate() const;

        tCIDLib::TVoid Format
        (
                    TTextOutStream&         strmTarget
            , const TCIDMacroEngine&        meOwner
        )   const;

        tCIDLib::TInt1 i1Immediate() const;

        tCIDLib::TInt2 i2Immediate() const;

        tCIDLib::TInt4 i4Immediate() const;

        tCIDLib::TVoid SetOpCode
        (
            const   tCIDMacroEng::EOpCodes  eOpCode
        );

        tCIDLib::TVoid SetSingleIndex
        (
            const   tCIDMacroEng::EOpCodes  eOpCode
            , const tCIDLib::TCard2         c2Index
        );

        tCIDLib::TVoid SetDoubleIndex
        (
            const   tCIDMacroEng::EOpCodes  eOpCode
            , const tCIDLib::TCard2         c2Index1
            , const tCIDLib::TCard2         c2Index2
        );

        tCIDLib::TVoid SetImmediate
        (
            const   tCIDMacroEng::EOpCodes  eOpCode
            , const tCIDLib::TBoolean       bImmediate
        );

        tCIDLib::TVoid SetImmediate
        (
            const   tCIDMacroEng::EOpCodes  eOpCode
            , const tCIDLib::TCard1         c1Immediate
        );

        tCIDLib::TVoid SetImmediate
        (
            const   tCIDMacroEng::EOpCodes  eOpCode
            , const tCIDLib::TCard2         c2Immediate
        );

        tCIDLib::TVoid SetImmediate
        (
            const   tCIDMacroEng::EOpCodes  eOpCode
            , const tCIDLib::TCard4         c4Immediate
        );

        tCIDLib::TVoid SetImmediate
        (
            const   tCIDMacroEng::EOpCodes  eOpCode
            , const tCIDLib::TCard8         c8Immediate
        );

        tCIDLib::TVoid SetImmediate
        (
            const   tCIDMacroEng::EOpCodes  eOpCode
            , const tCIDLib::TCh            chImmediate
        );

        tCIDLib::TVoid SetImmediate
        (
            const   tCIDMacroEng::EOpCodes  eOpCode
            , const tCIDLib::TFloat4        f4Immediate
        );

        tCIDLib::TVoid SetImmediate
        (
            const   tCIDMacroEng::EOpCodes  eOpCode
            , const tCIDLib::TFloat8        f8Immediate
        );

        tCIDLib::TVoid SetImmediate
        (
            const   tCIDMacroEng::EOpCodes  eOpCode
            , const tCIDLib::TInt1          i1Immediate
        );

        tCIDLib::TVoid SetImmediate
        (
            const   tCIDMacroEng::EOpCodes  eOpCode
            , const tCIDLib::TInt2          i2Immediate
        );

        tCIDLib::TVoid SetImmediate
        (
            const   tCIDMacroEng::EOpCodes  eOpCode
            , const tCIDLib::TInt4          i4Immediate
        );


    protected :
        // -------------------------------------------------------------------
        // Declare our friends
        // -------------------------------------------------------------------
        friend TTextOutStream& operator<<
        (
                    TTextOutStream&         strmToWriteTo
            , const TMEngOpCode&            meopToFormat
        );


    private :
        // -------------------------------------------------------------------
        //  Class types
        // -------------------------------------------------------------------
        union UOpStorage
        {
            tCIDLib::TCard2     ac2Indices[kCIDMacroEng::c4OpIndices];
            tCIDLib::TBoolean   bImmediate;
            tCIDLib::TCard1     c1Immediate;
            tCIDLib::TCard2     c2Immediate;
            tCIDLib::TCard4     c4Immediate;
            tCIDLib::TCard8     c8Immediate;
            tCIDLib::TCh        chImmediate;
            tCIDLib::TInt1      i1Immediate;
            tCIDLib::TInt2      i2Immediate;
            tCIDLib::TInt4      i4Immediate;
            tCIDLib::TFloat4    f4Immediate;
            tCIDLib::TFloat8    f8Immediate;
            tCIDLib::TCard4     c4JumpIndex;
        };


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid BadIndex
        (
            const   tCIDLib::TCard4         c4Index
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eOpCode
        //      The particular opcode represented by this object.
        //
        //  m_uStorage
        //      The data storage for this opcode. It may or may not be used
        //      and it is a union so the opcode indicates which of it's
        //      fields is active.
        // -------------------------------------------------------------------
        tCIDMacroEng::EOpCodes  m_eOpCode;
        UOpStorage              m_uStorage;
};

#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  Global operators
// ---------------------------------------------------------------------------
TTextOutStream&
operator<<(TTextOutStream& strmTarget, const TMEngOpCode& meopToFormat);


// ---------------------------------------------------------------------------
//  TMEngOpCode: Public operators
// ---------------------------------------------------------------------------
inline tCIDLib::TCard2
TMEngOpCode::operator[](const tCIDLib::TCard4 c4Index) const
{
    if (c4Index < kCIDMacroEng::c4OpIndices)
        return m_uStorage.ac2Indices[c4Index];

    // It's invalid so throw, and a bogus return to make the compiler happy
    BadIndex(c4Index);
    return 0;
}


// ---------------------------------------------------------------------------
//  TMEngOpCode: Public, non-virtual methods
// ---------------------------------------------------------------------------
inline tCIDLib::TBoolean TMEngOpCode::bIsNoOp() const
{
    return (m_eOpCode == tCIDMacroEng::EOpCodes::NoOp);
}

inline tCIDLib::TBoolean TMEngOpCode::bImmediate() const
{
    return m_uStorage.bImmediate;
}


inline tCIDLib::TCard1 TMEngOpCode::c1Immediate() const
{
    return m_uStorage.c1Immediate;
}


inline tCIDLib::TCard2 TMEngOpCode::c2Immediate() const
{
    return m_uStorage.c2Immediate;
}


inline tCIDLib::TCard4 TMEngOpCode::c4Immediate() const
{
    return m_uStorage.c4Immediate;
}


inline tCIDLib::TCard8 TMEngOpCode::c8Immediate() const
{
    return m_uStorage.c8Immediate;
}


inline tCIDLib::TCh TMEngOpCode::chImmediate() const
{
    return m_uStorage.chImmediate;
}

inline tCIDMacroEng::EOpCodes TMEngOpCode::eOpCode() const
{
    return m_eOpCode;
}


inline tCIDLib::TFloat4 TMEngOpCode::f4Immediate() const
{
    return m_uStorage.f4Immediate;
}


inline tCIDLib::TFloat8 TMEngOpCode::f8Immediate() const
{
    return m_uStorage.f8Immediate;
}


inline tCIDLib::TInt1 TMEngOpCode::i1Immediate() const
{
    return m_uStorage.i1Immediate;
}


inline tCIDLib::TInt2 TMEngOpCode::i2Immediate() const
{
    return m_uStorage.i2Immediate;
}


inline tCIDLib::TInt4 TMEngOpCode::i4Immediate() const
{
    return m_uStorage.i4Immediate;
}


