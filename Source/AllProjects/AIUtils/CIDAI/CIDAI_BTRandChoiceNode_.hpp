//
// FILE NAME: CIDAI_BTRandChoiceNode_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/27/2016
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
//  This is the header for the random choice node. This guy will randomly select
//  one of its children and invoke it.
//
// LOG:
//
//  $_CIDLib_Log_$
//


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TAIBTRandChoiceNode
//  PREFIX: btnode
// ---------------------------------------------------------------------------
class CIDAIEXP TAIBTRandChoiceNode : public TAIBTNode
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TAIBTRandChoiceNode
        (
            const   TString&                strPath
            , const TString&                strName
        );

        TAIBTRandChoiceNode(const TAIBTRandChoiceNode&) = delete;

        ~TAIBTRandChoiceNode();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDAI::ENodeStates eRun
        (
                    TAIBehaviorTree&        btreeOwner
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_prandSel
        //      A random number selector object to use. We use a pointer so that we
        //      don't have to force the math facility on downstream code.
        // -------------------------------------------------------------------
        TRandomNum* m_prandSel;

        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TAIBTRandChoiceNode, TAIBTNode)
};

#pragma CIDLIB_POPPACK
