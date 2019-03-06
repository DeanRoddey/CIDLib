//
// FILE NAME: CIDRegX_RegExNFA.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/28/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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

        ~TRXMatcher() {}


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
        //  Unimplemented constructors and operators
        // --------------------------------------------------------------------
        TRXMatcher(const TRXMatcher&);
        tCIDLib::TVoid operator=(const TRXMatcher&);


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
        TRegExNFA
        (
            const   tCIDLib::TCard4         c4MaxStates
        );

        ~TRegExNFA();


        // --------------------------------------------------------------------
        //  Public, non-virtual methods
        // --------------------------------------------------------------------
        tCIDLib::TBoolean bIsEpsilonState
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        tCIDLib::TCard4 c4AddState();

        tCIDLib::TCard4 c4AddState
        (
                    TRXMatcher* const       pmatchNew
            , const tCIDLib::TCard4         c4Trans1
            , const tCIDLib::TCard4         c4Trans2
        );

        tCIDLib::TCard4 c4AddStatePointedPastEnd
        (
                    TRXMatcher* const       pmatchNew = 0
        );

        tCIDLib::TCard4 c4StateCount() const;

        tCIDLib::TCard4 c4LastState() const;

        tCIDLib::TCard4 c4MaxStates() const;

        tCIDLib::TCard4 c4MaxStates
        (
            const   tCIDLib::TCard4         c4NewMax
        );

        tCIDLib::TCard4 c4State1At
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        tCIDLib::TCard4 c4State2At
        (
            const   tCIDLib::TCard4         c4At
        )   const;

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
        )   const;


    private :
        // --------------------------------------------------------------------
        //  Unimplemented constructors and destructor
        // --------------------------------------------------------------------
        TRegExNFA();
        TRegExNFA(const TRegExNFA&);
        tCIDLib::TVoid operator==(const TRegExNFA&);


        // --------------------------------------------------------------------
        //  Private, non-virtual methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid TestIndex
        (
            const   tCIDLib::TCard4         c4At
            , const tCIDLib::TCard4         c4Line
            , const tCIDLib::TCh* const     pszFile
        )   const;


        // --------------------------------------------------------------------
        //  Private data members
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


