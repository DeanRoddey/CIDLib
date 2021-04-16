//
// FILE NAME: CIDMacroEng_MethodImpl.hpp
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
//  This is the header file for the CIDMacroEng_MethodImpl.cpp file, which
//  implements the TMEngMethodImpl class. This represents the actual
//  implementation of a class, i.e. the compiled opcodes, descriptions of
//  locals and so forth, as apposed to the description of the method itself
//  which is provided by TMEngMethodInfo.
//
//  Since methods are invoked against some macro class, the instance data
//  object is passed to the method's Invoke() method, which can cast it to
//  the appropriate internal type and operate on it.
//
//  A method must be inherently reentrant. The parameters and return value
//  are all passed in and out, and any variables accessed are provided in
//  the context object. This allows compiled classes to be cached and shared
//  among macro engine instances, or reused.
//
//  Many methods, particularly the intrinsic ones, are not implemented
//  as macro engine opcodes, but are handled externally by C++ code. So
//  the Invoke() method is virtual. It can be overridden in order to provide
//  whatever type of processing is desired. So we derive a class that is
//  designed to be used for opcode based methods, called TMEngOpMethodImplImpl.
//  Each externally handled method will have it's own derivative so that
//  it can override Invoke().
//
//  A method can also have 'locals' which are temp vars that only exist within
//  the body of the method. When a method is called, the local list is used to
//  push storage onto the stack for use while in the method. These are pushed
//  above the call item that is pushed just before the call is made. So they
//  start at the base pointer, and can be accessed as an offset from it.
//
//  We also have a 'string pool' to hold any string literals used within the
//  method. These has to be per-implementation, because otherwise we cannot
//  generate reusable, standalone method impelementations that can be reused,
//  because the string pool ids could be different in each engine instance
//  if they were globally managed.
//
//  For those externally handled methods, there is often only a method info
//  object, and no implementation object, since they are just dispatched at
//  the class level, because it would be gross overkill to create and install
//  a dummy method impl object just to override the Invoke and do some trivial
//  operation.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


class TMEngStringVal;


// ---------------------------------------------------------------------------
//  CLASS: TMEngJumpTableItem
// PREFIX: jtbli
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngJumpTableItem
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngJumpTableItem
        (
            const   tCIDLib::TCard4         c4IPToSet
            ,       TMEngClassVal* const    pmecvToAdopt
        );

        TMEngJumpTableItem(const TMEngJumpTableItem&) = delete;

        ~TMEngJumpTableItem();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::ESortComps eComp
        (
            const   TMEngClassVal&          mecvToCheck
            ,       tCIDLib::TCard4&        c4IP
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        //
        //  m_c4IP
        //      The instruction pointer offset (within the method since all
        //      IPs are method relative), where the code for this block is
        //      located.
        //
        //  m_pmecvCase
        //      A pointer to the case value, which we store as a macro level
        //      value object so that we can be flexible about types (so that
        //      we can allow switches on enums, ints, chars, and cardinals.)
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4IP;
        TMEngClassVal*  m_pmecvCase;
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngJumpTable
// PREFIX: jtbl
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngJumpTable
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngJumpTable();

        TMEngJumpTable(const TMEngJumpTable&) = delete;

        ~TMEngJumpTable();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddDefaultItem
        (
            const   tCIDLib::TCard4         c4IPToSet
        );

        tCIDLib::TVoid AddItem
        (
            const   tCIDLib::TCard4         c4IPToSet
            ,       TMEngClassVal* const    pmecvToAdopt
        );

        tCIDLib::TBoolean bFindMatch
        (
            const   TMEngClassVal&          mecvToCheck
            ,       tCIDLib::TCard4&        c4IP
        )   const;

        tCIDLib::TBoolean bHasRequiredItems() const;

        tCIDLib::TCard4 c4DefCaseIP() const;

        TMEngJumpTableItem& jtbliAt
        (
            const   tCIDLib::TCard4         c4At
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        //
        //  m_c4DefIP
        //      The IP of the default case. We initialize to max card so if
        //      it's still that, then we never got the default.
        //
        //  m_colCases
        //      We keep a collection of jump table items for all of the non-
        //      default cases.
        // -------------------------------------------------------------------
        tCIDLib::TCard4                 m_c4DefIP;
        TRefVector<TMEngJumpTableItem>  m_colCases;
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngMethodImpl
// PREFIX: meth
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngMethodImpl : public TMEngNamedItem
{
    public  :
        // -------------------------------------------------------------------
        //  Class types
        // -------------------------------------------------------------------
        using TLocalList    = TVector<TMEngLocalInfo>;
        using TStringPool   = TRefVector<TMEngStringVal>;


        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strKey(const TMEngMethodImpl& memethSrc);


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid Invoke
        (
                    TMEngClassVal&          mecvInstance
            , const TMEngClassInfo&         meciTarget
            , const TMEngMethodInfo&        methiThis
            ,       TCIDMacroEngine&        meOwner
        ) = 0;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngMethodImpl() = delete;

        TMEngMethodImpl(const TMEngMethodImpl&) = delete;
        TMEngMethodImpl(TMEngMethodImpl&&) = delete;

        ~TMEngMethodImpl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngMethodImpl& operator=(const TMEngMethodImpl&) = delete;
        TMEngMethodImpl& operator=(TMEngMethodImpl&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard2 c2AddLocal
        (
            const   TMEngLocalInfo&         meliToAdd
        );

        tCIDLib::TCard2 c2AddString
        (
            const   TString&                strToAdd
            , const tCIDLib::TBoolean       bFindDups
        );

        tCIDLib::TCard4 c4LocalCount() const;

        tCIDLib::TCard4 c4StringCount() const;

        const TMEngClassVal& mecvFindPoolItem
        (
            const   tCIDLib::TCard2         c2Id
        )   const;

        TMEngClassVal& mecvFindPoolItem
        (
            const   tCIDLib::TCard2         c2Id
        );

        const TMEngLocalInfo& meliFind
        (
            const   tCIDLib::TCard2         c2Id
        )   const;

        TMEngLocalInfo& meliFind
        (
            const   tCIDLib::TCard2         c2At
        );

        const TMEngLocalInfo* pmeliFind
        (
            const   TString&                strName
        )   const;

        TMEngLocalInfo* pmeliFind
        (
            const   TString&                strName
        );

        tCIDLib::TVoid PushLocals
        (
                    TCIDMacroEngine&        meOwner
        );


    protected :
        // -------------------------------------------------------------------
        //  Hidden Constructors
        // -------------------------------------------------------------------
        TMEngMethodImpl
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2MethodId
        );


    private :
        // -------------------------------------------------------------------
        //  Private data
        // -------------------------------------------------------------------
        static const tCIDLib::TCard4        c4MaxLocals = 4096;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pcolLocals
        //      This is our list of locals. We store them as a vector since
        //      they are always accessed sequentially or via index. These
        //      locals can be consts or variables. This list is used to push
        //      storage onto the stack upon entry to the method. It's a pointer
        //      because many methods have none, and we don't want to allocate
        //      it unless required.
        //
        //  m_pcolStringPool
        //      Strings are the only non-numeric literals we have (well, we
        //      booleans also, but they can be treated like numerics), but
        //      they cannot be used in immediate opcodes, so we have to put
        //      them into a string pool, and opcodes refer to the string pool
        //      id. If optimizations indicate, we can collapse any dups into
        //      a single entry, but otherwise we don't bother. This is a
        //      pointer, since many methods will have none, and we don't want
        //      to allocate the collection if so.
        // -------------------------------------------------------------------
        TLocalList*     m_pcolLocalList;
        TStringPool*    m_pcolStringPool;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngMethodImpl,TMEngNamedItem)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngOpMethodImpl
// PREFIX: meth
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngOpMethodImpl : public TMEngMethodImpl
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngOpMethodImpl() = delete;

        TMEngOpMethodImpl
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2MethodId
        );

        TMEngOpMethodImpl(const TMEngOpMethodImpl&) = delete;
        TMEngOpMethodImpl(TMEngOpMethodImpl&&) = delete;

        ~TMEngOpMethodImpl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngOpMethodImpl& operator=(const TMEngOpMethodImpl&) = delete;
        TMEngOpMethodImpl& operator=(TMEngOpMethodImpl&&) = delete;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        const TMEngOpCode& operator[]
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        TMEngOpCode& operator[]
        (
            const   tCIDLib::TCard4         c4Index
        );


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Invoke
        (
                    TMEngClassVal&          mecvInstance
            , const TMEngClassInfo&         meciTarget
            , const TMEngMethodInfo&        methiThis
            ,       TCIDMacroEngine&        meOwner
        )   final;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bMoreLines
        (
            const   tCIDLib::TCard4         c4LastIP
        )   const;

        tCIDLib::TCard2 c2AddJumpTable();

        tCIDLib::TCard4 c4AddOpCode
        (
            const   TMEngOpCode&            meopToAdd
        );

        tCIDLib::TCard4 c4FirstLineNum() const;

        tCIDLib::TCard4 c4CurOffset() const;

        tCIDLib::TVoid DumpOpCodes
        (
                    TTextOutStream&         strmTarget
            , const TCIDMacroEngine&        meOwner
        )   const;

        TMEngJumpTable& jtblById
        (
            const   tCIDLib::TCard2         c2Id
        );

        const TMEngOpCode& meopAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        TMEngOpCode& meopAt
        (
            const   tCIDLib::TCard4         c4At
        );

        const TMEngOpCode& meopLast() const;

        TMEngOpCode& meopLast();


    private :
        // -------------------------------------------------------------------
        //  Private class types
        // -------------------------------------------------------------------
        using TMEngJumpTableTable = TRefVector<TMEngJumpTable>;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatErrTokens
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4Tokens
            ,       TString&                strToFill
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colOpCodes
        //      This is the list of opcodes that makes up the body of this
        //      method.
        //
        //  m_pcolSwitches
        //      If this method has any switch statements, then it needs jump
        //      tables for them. So each switch statement gets a table added
        //      here and the table jump opcode holds the index of it's table.
        //      We only allocate this if needed.
        // -------------------------------------------------------------------
        TVector<TMEngOpCode>    m_colOpCodes;
        TMEngJumpTableTable*    m_pjtblSwitches;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngOpMethodImpl,TMEngMethodImpl)
};


#pragma CIDLIB_POPPACK


