//
// FILE NAME: CIDAI_BTContext.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/30/2016
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the header for the file that implements the context system for our behavior
//  tree classes. The nodes themselves are completely stateless, so that single instances
//  of a tree can be loaded and invoked from many different places. All of their data for
//  running themselves is on the stack. Any data that they are to operate on, or that they
//  create for other nodes to use, is placed in the context object.
//
//  We also support a list of string/numeric pairs, which the client code can use.
//  Typically it would be used to store things like the semantic value confidence info
//  from speech recognition.
//
// CAVEATS/GOTCHAS:
//
//  1)  The context object does not require any sync because only one thread uses them at
//      a time, the one running the tree, so the movement through the nodes is single
//      threaded. Another thread may check the status of the tree (is there a currently
//      running node), but only one thread actually runs the tree.
//
// LOG:
//
//  $_CIDLib_Log_$
//


#pragma CIDLIB_PACK(CIDLIBPACK)

class TAIBTNode;

// ---------------------------------------------------------------------------
//   CLASS: TAIBTCtxVar
//  PREFIX: ctxv
// ---------------------------------------------------------------------------
class CIDAIEXP TAIBTCtxVar
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strVarKey
        (
            const   TAIBTCtxVar&            ctxvSrc
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TAIBTCtxVar();

        TAIBTCtxVar
        (
            const   TString&                strKey
        );

        TAIBTCtxVar
        (
            const   TAIBTCtxVar&            ctxvSrc
        );

        ~TAIBTCtxVar();


        // -------------------------------------------------------------------
        //  Public constructors
        // -------------------------------------------------------------------
        TAIBTCtxVar& operator=
        (
            const   TAIBTCtxVar&            ctxvSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bValAs
        (
            const   TString&                strValKey
        )   const;

        tCIDLib::TCard4 c4ValAs
        (
            const   TString&                strValKey
        )   const;

        tCIDLib::TInt4 i4ValAs
        (
            const   TString&                strValKey
        )   const;

        const TString& strVarKey() const;

        const TString& strVal
        (
            const   TString&                strValKey
        )   const;

        tCIDLib::TVoid SetVal
        (
            const   TString&                strValKey
            , const tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TVoid SetVal
        (
            const   TString&                strValKey
            , const tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TVoid SetVal
        (
            const   TString&                strValKey
            , const tCIDLib::TInt4          i4ToSet
        );

        tCIDLib::TVoid SetVal
        (
            const   TString&                strValKey
            , const TString&                strToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        const TKeyValuePair* pkvalFindVal
        (
            const   TString&                strValKey
            , const tCIDLib::TBoolean       bThrowIfNot
        )   const;

        TKeyValuePair* pkvalFindVal
        (
            const   TString&                strValKey
            , const tCIDLib::TBoolean       bThrowIfNot
        );

        tCIDLib::TVoid TypeCastErr
        (
            const   TString&                strValKey
            , const tCIDLib::TCh* const     pszType
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colMembers
        //      A list of key/value pairs that represent the values of this variable.
        //      We need to be able to represent object type variables, in a generic
        //      way, so this is how we do it.
        //
        //  m_strKey
        //      The name for this variable. These are defined by the application
        //      generally, though it's possible that some nodes might define a variable
        //      that they will accept input on or put values into. For the latter type,
        //      they should use a simple path style key, like /type/varname, just to
        //      insure they don't conflict with any application defined variables.
        // -------------------------------------------------------------------
        tCIDLib::TKVHashSet m_colMembers;
        TString             m_strKey;
};


namespace tCIDAI
{
    using TBTCtxVarList = TKeyedHashSet<TAIBTCtxVar, TString, TStringKeyOps>;
}


// ---------------------------------------------------------------------------
//   CLASS: TAIBTCtx
//  PREFIX: ctx
// ---------------------------------------------------------------------------
class CIDAIEXP TAIBTCtx
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TAIBTCtx();

        TAIBTCtx(const TAIBTCtx&) = delete;

        ~TAIBTCtx();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bFindKNumListVal
        (
            const   TString&                strKey
            ,       tCIDLib::TFloat4&       f4Value
        );

        tCIDLib::TBoolean bFindKNumListVal
        (
            const   TString&                strKey
            ,       TString&                strToFill
        );

        tCIDLib::TBoolean bQueryValue
        (
            const   TString&                strVarKey
            , const TString&                strValKey
            ,       TString&                strToFill
        )   const;

        tCIDLib::TBoolean bVarExists
        (
            const   TString&                strVarKey
        )   const;

        tCIDLib::TBoolean bVarValue
        (
            const   TString&                strVarKey
            , const TString&                strValKey
        )   const;

        tCIDLib::TCard4 c4VarValue
        (
            const   TString&                strVarKey
            , const TString&                strValKey
        )   const;

        tCIDLib::TInt4 i4VarValue
        (
            const   TString&                strVarKey
            , const TString&                strValKey
        )   const;

        const tCIDLib::TKNumList& colKNumList() const;

        tCIDLib::TVoid Reset();

        tCIDLib::TVoid Reset
        (
            const   tCIDLib::TKValsList&    colInitVars
        );

        tCIDLib::TVoid Reset
        (
            const   tCIDLib::TKValsList&    colInitVars
            , const tCIDLib::TKNumList&     colKNumList
        );

        const TString& strFindKNumListVal
        (
            const   TString&                strKey
        );

        const TString& strFindKNumListVal
        (
            const   TString&                strKey
            ,       tCIDLib::TFloat4&       f4Value
        );

        const TString& strVarValue
        (
            const   TString&                strVarKey
            , const TString&                strValKey
        )   const;

        tCIDLib::TVoid SetAppInfo
        (
            const   tCIDLib::TKNumList&     colKNumList
        );

        tCIDLib::TVoid SetBoolVar
        (
            const   TString&                strVarKey
            , const TString&                strValKey
            , const tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TVoid SetCardVar
        (
            const   TString&                strVarKey
            , const TString&                strValKey
            , const tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TVoid SetIntVar
        (
            const   TString&                strVarKey
            , const TString&                strValKey
            , const tCIDLib::TInt4          i4ToSet
        );

        tCIDLib::TVoid SetStringVar
        (
            const   TString&                strVarKey
            , const TString&                strValKey
            , const TString&                strToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        const TKeyValuePair* pkvalFindVal
        (
            const   TString&                strValKey
            , const tCIDLib::TBoolean       bThrowIfNot
        )   const;

        TKeyValuePair* pkvalFindVal
        (
            const   TString&                strValKey
            , const tCIDLib::TBoolean       bThrowIfNot
        );

        const TAIBTCtxVar* pctxvCheckVarExists
        (
            const   TString&                strVarKey
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colKNumList
        //      A list of key/numeric pairs, for use by the class to pass information
        //      down into the nodes when running the tree. See class comments above.
        //
        //  m_colVars
        //      Our variables list that we provide for nodes to use. It's a keyed hash set
        //      of our context value class above, with the value path being the has key.
        // -------------------------------------------------------------------
        tCIDLib::TKNumList      m_colKNumList;
        tCIDAI::TBTCtxVarList   m_colVars;
};

#pragma CIDLIB_POPPACK

