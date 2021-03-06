//
// FILE NAME: CIDRegX_RegExNFA.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/28/1998
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
//  This is the header for the CIDRegX_RegExNFA.cpp file, which implements
//  the TRegXNFA class. This class represents the transition table for the
//  regular expression engine, TRegEx.
//
//  It also provides a public abstract interface, TGrepMatcher, which is the
//  base for the pluggable objects that provide various variations on pattern
//  matching operations. The rest of the derivatives of this class are provided
//  in an internal only hpp/cpp file since the outside world does not need
//  to see them.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ----------------------------------------------------------------------------
//   CLASS: TRXMatcher
//  PREFIX: match
// ----------------------------------------------------------------------------
class CIDREGXEXP TRXMatcher : public TObject, public MFormattable
{
    public :
        // --------------------------------------------------------------------
        //  Constructors and Destructor
        // --------------------------------------------------------------------
        TRXMatcher() {}

        TRXMatcher(const TRXMatcher&) = delete;
        TRXMatcher(TRXMatcher&&) = delete;

        ~TRXMatcher() {}


        // --------------------------------------------------------------------
        //  Public operators
        // --------------------------------------------------------------------
        TRXMatcher& operator=(const TRXMatcher&) = delete;
        TRXMatcher& operator=(TRXMatcher&&) = delete;


        // --------------------------------------------------------------------
        //  Public, virtual methods
        // --------------------------------------------------------------------
        virtual tCIDLib::TBoolean bMatches
        (
            const   tCIDLib::TCh            chToCheck
            , const tCIDLib::TCard4         c4CurOfs
            , const tCIDLib::TCard4         c4SearchLen
            , const tCIDLib::TBoolean       bCaseSensitive
        )   const = 0;


    private :
        // --------------------------------------------------------------------
        //  Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TRXMatcher,TObject)
};



// ----------------------------------------------------------------------------
//   CLASS: TRegExNFA
//  PREFIX: rxnfa
// ----------------------------------------------------------------------------
class CIDREGXEXP TRegExNFA : public TObject, public MFormattable
{
    public :
        // --------------------------------------------------------------------
        //  Constructors and Destructor
        // --------------------------------------------------------------------
        TRegExNFA() = delete;

        TRegExNFA
        (
            const   tCIDLib::TCard4         c4MaxStates
        );

        TRegExNFA(const TRegExNFA&) = delete;
        TRegExNFA(TRegExNFA&&) = delete;

        ~TRegExNFA();


        // --------------------------------------------------------------------
        //  Public operators
        // --------------------------------------------------------------------
        TRegExNFA& operator==(const TRegExNFA&) = delete;
        TRegExNFA& operator==(TRegExNFA&&) = delete;


        // --------------------------------------------------------------------
        //  Public, non-virtual methods
        // --------------------------------------------------------------------
        tCIDLib::TBoolean bIsEpsilonState
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        tCIDLib::TBoolean bIsNullable() const
        {
            return m_bNullable;
        }

        tCIDLib::TCard4 c4AddState();

        tCIDLib::TCard4 c4AddState
        (
                    TRXMatcher* const       pmatchNew
            , const tCIDLib::TCard4         c4Trans1
            , const tCIDLib::TCard4         c4Trans2
        );

        tCIDLib::TCard4 c4AddStatePointedPastEnd
        (
                    TRXMatcher* const       pmatchNew = nullptr
        );

        tCIDLib::TCard4 c4Reset
        (
            const   tCIDLib::TCard4         c4MaxStates
        );

        [[nodiscard]] tCIDLib::TCard4 c4StateCount() const;

        tCIDLib::TCard4 c4LastState() const;

        tCIDLib::TCard4 c4MaxStates() const;

        tCIDLib::TCard4 c4State1At
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        tCIDLib::TCard4 c4State2At
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        tCIDLib::TVoid Complete();

        const TRXMatcher& matchAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        TRXMatcher& matchAt
        (
            const   tCIDLib::TCard4         c4At
        );

        TRXMatcher* pmatchAt
        (
            const   tCIDLib::TCard4         c4At
        );

        tCIDLib::TVoid PointPastEnd
        (
            const   tCIDLib::TCard4         c4At
        );

        tCIDLib::TVoid UpdateLast
        (
                    TRXMatcher* const       pmatchNew
            , const tCIDLib::TCard4         c4Trans
        );

        tCIDLib::TVoid UpdateLast
        (
            const   tCIDLib::TCard4         c4Trans1
            , const tCIDLib::TCard4         c4Trans2
        );

        tCIDLib::TVoid UpdateMatchAt
        (
            const   tCIDLib::TCard4         c4At
            ,       TRXMatcher* const       pmatchNew
        );

        tCIDLib::TVoid UpdateStateAt
        (
            const   tCIDLib::TCard4         c4At
            ,       TRXMatcher* const       pmatchNew
            , const tCIDLib::TCard4         c4Trans
        );

        tCIDLib::TVoid UpdateStateAt
        (
            const   tCIDLib::TCard4         c4At
            , const tCIDLib::TCard4         c4Trans1
            , const tCIDLib::TCard4         c4Trans2
        );

        tCIDLib::TVoid UpdateState1At
        (
            const   tCIDLib::TCard4         c4At
            , const tCIDLib::TCard4         c4Trans1
        );

        tCIDLib::TVoid UpdateState2At
        (
            const   tCIDLib::TCard4         c4At
            , const tCIDLib::TCard4         c4Trans2
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
        )   const final;


    private :
        // --------------------------------------------------------------------
        //  Private, non-virtual methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid CleanupMatches();

        tCIDLib::TVoid TestIndex
        (
            const   tCIDLib::TCard4         c4At
            , const tCIDLib::TCard4         c4Line
            , const tCIDLib::TCh* const     pszFile
        )   const;


        // --------------------------------------------------------------------
        //  Private data members
        //
        //  m_bNullable
        //      When Complete is called, we see if there is path through the NFA
        //      from start to end via epsilon nodes. If so, then this expression will
        //      match an empty input pattern.
        //
        //  m_bReady
        //      After setting all states the client code must call Complete to finish
        //      setup and we set this to indicate we are ready. We clear it upon
        //      a call to Reset() which the client code uses to get us ready to start
        //      setting up a new parsed expression.
        //
        //  m_c4StateCount
        //      The number of states in our NFA.
        //
        //  m_c4Entries
        //      The number of entries in the m_pc4StateX and m_pmatchTrans
        //      arrays. We don't know how many statse we can have so we
        //      allocated them to the worst case.
        //
        //  m_pc4State1
        //  m_pc4State2
        //      The state transition arrays for each state in the NFA.
        //
        //  m_pmatchTrans
        //      The array of transition matchers. Each one of them is matcher
        //      object that represents a single state's match criteria.
        // --------------------------------------------------------------------
        tCIDLib::TBoolean       m_bNullable;
        tCIDLib::TBoolean       m_bReady;
        tCIDLib::TCard4         m_c4StateCount;
        tCIDLib::TCard4         m_c4Entries;
        tCIDLib::TCard4*        m_pc4State1;
        tCIDLib::TCard4*        m_pc4State2;
        TRXMatcher**            m_pmatchTrans;


        // --------------------------------------------------------------------
        //  Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TRegExNFA,TObject)
};

#pragma CIDLIB_POPPACK


