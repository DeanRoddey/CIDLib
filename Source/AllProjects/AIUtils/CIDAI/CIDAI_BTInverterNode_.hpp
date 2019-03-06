//
// FILE NAME: CIDAI_BTInverterNode.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/27/2016
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the inverter type node, which takes one child node and just
//  inverts the success/failure result that child returns.
//
// LOG:
//
//  $_CIDLib_Log_$
//


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TAIBTInverterNode
//  PREFIX: btnode
// ---------------------------------------------------------------------------
class CIDAIEXP TAIBTInverterNode : public TAIBTNode
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TAIBTInverterNode
        (
            const   TString&                strPath
            , const TString&                strName
        );

        TAIBTInverterNode(const TAIBTInverterNode&) = delete;

        ~TAIBTInverterNode();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDAI::ENodeStates eRun
        (
                    TAIBehaviorTree&        btreeOwner
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TAIBTInverterNode, TAIBTNode)
};

#pragma CIDLIB_POPPACK
