//
// FILE NAME: CIDMacroEng_CallStackItem.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2003
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
//  This is the header file for the CIDMacroEng_CallStackItem.cpp file, which
//  implements the TMEngCallStackItem class. This class is used to push items
//  onto the call stack maintained by the macro engine. There are two things
//  that can get pushed, a value or a call trace (the latter is just used in
//  call stack interpretation), and there are a few variations on the value
//  type. Since there isn't much real variation in the data though, we just
//  use a class with a type enum, and don't bother with a polymorphic scheme.
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
//  CLASS: TMEngCallStackItem
// PREFIX: mecsi
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngCallStackItem : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngCallStackItem();

        TMEngCallStackItem(const TMEngCallStackItem&) = delete;

        ~TMEngCallStackItem();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngCallStackItem& operator=(const TMEngCallStackItem&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bHasCallerInfo() const;

        tCIDLib::TBoolean bIsImValue() const;

        tCIDLib::TBoolean bIsMethod() const;

        tCIDLib::TBoolean bIsRepush() const;

        tCIDLib::TBoolean bIsRepush
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TBoolean bIsTempVal() const;

        tCIDLib::TBoolean bIsValue() const;

        tCIDLib::TCard4 c4QueryCallerInfo
        (
            const   TMEngClassInfo*&        pmeciCaller
            , const TMEngMethodInfo*&       pmethiCaller
            , const TMEngOpMethodImpl*&     pmethCaller
            ,       TMEngClassVal*&         pmecvCaller
            ,       tCIDLib::TCard4&        c4CallIP
        );

        tCIDLib::TCard4 c4Value() const;

        tCIDLib::TVoid Clear();

        tCIDMacroEng::EStackItems eType() const;

        const TMEngMethodInfo& methiCalled() const;

        const TMEngClassInfo& meciCalled() const;

        TMEngClassInfo& meciCalled();

        const TMEngClassVal& mecvPushed() const;

        TMEngClassVal& mecvPushed();

        const TMEngClassInfo* pmeciCaller
        (
                    tCIDLib::TCard4&        c4CallLine
        )   const;

        tCIDLib::TVoid Reset
        (
                    TMEngClassInfo* const   pmeciCalled
            , const TMEngMethodInfo* const  pmethiCalled
        );

        tCIDLib::TVoid Reset
        (
                    TMEngClassInfo* const   pmeciCalled
            , const TMEngMethodInfo* const  pmethiCalled
            , const TMEngClassInfo* const   pmeciCaller
            , const TMEngMethodInfo* const  pmethiCaller
            , const TMEngOpMethodImpl* const pmethCaller
            ,       TMEngClassVal* const    pmecvCaller
            , const tCIDLib::TCard4         c4CallLine
            , const tCIDLib::TCard4         c4CallIP
        );

        tCIDLib::TVoid Reset
        (
                    TMEngClassVal* const    pmecvPushed
            , const tCIDMacroEng::EStackItems  eType
        );

        tCIDLib::TVoid Reset
        (
            const   tCIDMacroEng::EStackItems  eType
            , const tCIDLib::TCard4         c4Value
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bRepush
        //      This is a repush of an item already on the stack. This tells
        //      the popping code that it shouldn't make any attempt to do
        //      cleanup based on type, because this isn't the real one.
        //
        //  m_c4CallIP
        //  m_c4CallLine
        //      If the caller is a macro level class, it will push this info
        //      when it makes a call, so that we can back trace.
        //
        //  m_c4Value
        //      Some stack types need to store a literal cardinal value.
        //
        //  m_eType
        //      Indicates the type of stack item this is. It's either a
        //      procedure, or one of the value types.
        //
        //  m_pmeciCalled
        //  m_pmethiCalled
        //      If this is a method call item, these are pointers to the
        //      class and method called. This is both for debugging and to
        //      validate parameters. The type member will indicate a method.
        //
        //  m_pmeciCaller
        //  m_pmecvCaller
        //  m_pmethiCaller
        //  m_pmethCaller
        //      If the caller is a macro level class, it will push itself
        //      as well, so that we can back trace.
        //
        //  m_pmecvPushed
        //      If this is a value item, this is the value object. The type
        //      of value specifically is indicated in the m_eType member.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean           m_bRepush;
        tCIDLib::TCard4             m_c4CallIP;
        tCIDLib::TCard4             m_c4CallLine;
        tCIDLib::TCard4             m_c4Value;
        tCIDMacroEng::EStackItems      m_eType;
        TMEngClassInfo*             m_pmeciCalled;
        const TMEngClassInfo*       m_pmeciCaller;
        TMEngClassVal*              m_pmecvPushed;
        const TMEngMethodInfo*      m_pmethiCalled;
        const TMEngMethodInfo*      m_pmethiCaller;
        const TMEngOpMethodImpl*    m_pmethCaller;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngCallStackItem,TObject)
};

#pragma CIDLIB_POPPACK

