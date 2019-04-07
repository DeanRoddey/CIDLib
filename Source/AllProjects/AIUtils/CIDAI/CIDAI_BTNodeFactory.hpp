//
// FILE NAME: CIDAI_BTNodeFactory_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/07/2016
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
//  This is the header for the base class from which BT node factories derive. We can't
//  have a fixed set of node types because derived classes have to be able to create
//  their own specialized ones. So factories are registered to handle the creation
//  of nodes.
//
//  We define the ABC from which they are derived. Internally we will define a default
//  factory derivative that will always be installed to handle the nodes we do define
//  here at this level.
//
//  Each factory has to provide a factory name to this base class in the ctor. Each
//  node class has a path in the form /[factname]/[nodetype]. So that indicate which
//  factory should be asked to create the node. It will be passed the node type, and
//  any parameters from the XML tree definition.
//
// CAVEATS/GOTCHAS:
//
//
// LOG:
//
//  $_CIDLib_Log_$
//

#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TAIBTNodeFact
//  PREFIX: nfact
// ---------------------------------------------------------------------------
class CIDAIEXP TAIBTNodeFact : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strKey
        (
            const   TAIBTNodeFact&          nfactSrc
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TAIBTNodeFact(const TAIBTNodeFact&) = delete;

        ~TAIBTNodeFact();


        // -------------------------------------------------------------------
        //  Public constructors
        // -------------------------------------------------------------------
        TAIBTNodeFact& operator=(const TAIBTNodeFact&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        [[nodiscard]] TAIBTNode* pbtnodeMakeNew
        (
            const   TString&                strPath
            , const TString&                strName
            , const TString&                strType
            , const tCIDLib::TBoolean       bFlag
        );

        const TString& strFactKey() const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TAIBTNodeFact
        (
            const   TString&                strFactKey
        );


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual TAIBTNode* pbtnodeNew
        (
            const   TString&                strPath
            , const TString&                strName
            , const TString&                strType
        ) = 0;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strFactKey
        //      Each derived class provides us with a factory name.
        // -------------------------------------------------------------------
        TString     m_strFactKey;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TAIBTNodeFact, TObject)
};

#pragma CIDLIB_POPPACK

