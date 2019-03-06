//
// FILE NAME: CIDAI_BTSeqNode_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/08/2016
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the sequence node type. It can have N children and works
//  through them in order until success of all, or failure/cancel of one.
//
// LOG:
//
//  $_CIDLib_Log_$
//


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TAIBTSeqNode
//  PREFIX: btnode
// ---------------------------------------------------------------------------
class CIDAIEXP TAIBTSeqNode : public TAIBTNode
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TAIBTSeqNode
        (
            const   TString&                strPath
            , const TString&                strName
        );

        TAIBTSeqNode(const TAIBTSeqNode&) = delete;

        ~TAIBTSeqNode();


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
        RTTIDefs(TAIBTSeqNode, TAIBTNode)
};

#pragma CIDLIB_POPPACK
