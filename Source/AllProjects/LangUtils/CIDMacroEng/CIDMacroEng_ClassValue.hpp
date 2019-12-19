//
// FILE NAME: CIDMacroEng_ClassValue.hpp
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
//  This is the header file for the CIDMacroEng_ClassValue.cpp file, which
//  implements the TMEngClassVal class. This class represents the storage
//  for a particular class. The TMEngClassInfo class represents the class
//  itself, and has a factory method that will return a new instance of a
//  derivative of this class, that has storage for that particular type.
//
//  This is an abstract base class, so it has a hidden ctor. Everything will
//  tend to work in terms of pointers to this base class (unless they take a
//  specific type only), so all value manipulation will tend to be abstracted
//  via virtuals of this class.
//
// CAVEATS/GOTCHAS:
//
//  1)  We derive from the base named item class, but in one case, the case
//      of the temp pool, the name isn't used.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TMEngClassInfo;

// ---------------------------------------------------------------------------
//  CLASS: TMEngClassVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngClassVal : public TMEngNamedItem
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TMEngClassVal(const TMEngClassVal&) = delete;

        ~TMEngClassVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngClassVal& operator=(const TMEngClassVal&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bDbgFormat
        (
                    TTextOutStream&         strmTarget
            , const TMEngClassInfo&         meciThis
            , const tCIDMacroEng::EDbgFmts     eFormat
            , const tCIDLib::ERadices       eRadix
            , const TCIDMacroEngine&        meOwner
        )   const;

        virtual tCIDLib::TBoolean bParseFromText
        (
            const   TString&                strValue
            , const TMEngClassInfo&         mecvThis
        );

        virtual tCIDLib::TVoid CopyFrom
        (
            const   TMEngClassVal&          mecvToCopy
            ,       TCIDMacroEngine&        meOwner
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsConst() const;

        tCIDLib::TBoolean bIsUsed() const;

        tCIDLib::TBoolean bIsUsed
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TCard2 c2ClassId() const;

        tCIDLib::TCard2 c2ClassId
        (
            const   tCIDLib::TCard2         c2ToSet
        );

        tCIDLib::TCard4 c4MemberCount() const;

        tCIDMacroEng::EConstTypes eConst() const;

        const TMEngClassVal& mecvFind
        (
            const   tCIDLib::TCard2         c2Id
            , const TCIDMacroEngine&        meOwner
        )   const;

        TMEngClassVal& mecvFind
        (
            const   tCIDLib::TCard2         c2Id
            , const TCIDMacroEngine&        meOwner
        );

        template <typename T>
        const T& mecvFindAs(const   tCIDLib::TCard2     c2Id
                            , const TCIDMacroEngine&    meOwner) const
        {
            return static_cast<const T&>(mecvFind(c2Id, meOwner));
        }

        template <typename T> T& mecvFindAs(const  tCIDLib::TCard2     c2Id
                                        , const TCIDMacroEngine&    meOwner)
        {
            return static_cast<T&>(mecvFind(c2Id, meOwner));
        }

        tCIDLib::TVoid* pExtra() const;

        TMEngClassVal* pmecvFind
        (
            const   TString&                strName
        );

        tCIDLib::TVoid SetExtra
        (
                    tCIDLib::TVoid* const   pExtra
        );


    protected :
        // -------------------------------------------------------------------
        //  The engine is our friend so that he can call ForceConstType()
        //  because the temps given out must be of different const types
        //  according the needs of the code asking for a temp.
        //
        //  The base class info class must also be a friend because when
        //  a new isntance is created, we have to be loaded up with all of the
        //  members defined by that class.
        // -------------------------------------------------------------------
        friend class TCIDMacroEngine;
        friend class TMEngClassInfo;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TMEngClassVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2ClassId
            , const tCIDMacroEng::EConstTypes  eConst
        );


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddMember
        (
                    TMEngClassVal* const    pmecvToAdopt
        );

        tCIDLib::TVoid ForceConstType
        (
            const   tCIDMacroEng::EConstTypes  eToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Class types
        // -------------------------------------------------------------------
        using TClassValList = TRefVector<TMEngClassVal>;


        // -------------------------------------------------------------------
        //  Public data members
        //
        //  m_bUsed
        //      These objects are often gotten from a temp pool, from which they are
        //      borrowed and returned. This flag is used to keep up with which ones
        //      are loaned out or available.
        //
        //  m_c2ClassId
        //      This is the id of the class represented by the derived class. This
        //      allows for fast type checks without doing string compares. It's set
        //      by the contructor of the derived class.
        //
        //  m_colMembers
        //      Our list of the instance data for the members of this class. When our
        //      info class is asked to create an instance of this class, it will load
        //      us up with value objects for the members that were defined for it.
        //
        //      In this case, we don't worry too much about lookup by mame speed here,
        //      so we just use a ref vector for fast lookup by id.
        //
        //  m_eConst
        //      Indicates whether this value represents storage for a constant
        //      or non-const entity.
        //
        //  m_pExtra
        //      In some cases, when gluing CML code into C++ code, it's convenient for
        //      the C++ code to store some sort of linkage into the CML value object
        //      it is interacting with. All we do is store it and give it back. We
        //      don't own it.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bUsed;
        tCIDLib::TCard2         m_c2ClassId;
        TClassValList           m_colMembers;
        tCIDMacroEng::EConstTypes  m_eConst;
        tCIDLib::TVoid*         m_pExtra;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngClassVal,TMEngNamedItem)
};

#pragma CIDLIB_POPPACK

