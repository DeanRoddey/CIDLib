//
// FILE NAME: CIDAI_BTRootNode.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/07/2016
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the simple root node type. The one is always created as the
//  root of a tree. There's not any big technical reason for that currently, but it
//  leaves us open to always have a specific node at at the top in case we want to
//  provide some tree-wide operations where we have an initial non-virtual methods that
//  can insure initial conditions and check results and such.
//
//  This is the one node type that isn't created via the node factory, so it can have
//  regular parameters.
//
// LOG:
//
//  $_CIDLib_Log_$
//


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TAIBTRootNode
//  PREFIX: btnode
// ---------------------------------------------------------------------------
class CIDAIEXP TAIBTRootNode : public TAIBTNode
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strRootNodeKey
        (
            const   TAIBTRootNode&          btnodeSrc
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TAIBTRootNode
        (
            const   TString&                strPath
            , const TString&                strName
            , const tCIDLib::TCard4         c4Version
        );

        TAIBTRootNode(const TAIBTRootNode&) = delete;

        ~TAIBTRootNode();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDAI::ENodeStates eRun
        (
                    TAIBehaviorTree&        btreeOwner
        )   override;

        tCIDLib::TVoid Validate
        (
                    TAIBehaviorTree&        btreeOwner
            ,       TVector<TAIBTErr>&      colToFill
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Version
        //      The version of the XML that was used to create this tree. Not used
        //      currently, but for future expansion. For now it will always be 1,
        //      since it is fixed in the DTD.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4Version;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TAIBTRootNode, TAIBTNode)
};

#pragma CIDLIB_POPPACK
