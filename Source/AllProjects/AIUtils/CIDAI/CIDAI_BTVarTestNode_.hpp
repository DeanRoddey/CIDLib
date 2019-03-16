//
// FILE NAME: CIDAI_BTVarTestNode.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/26/2016
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
//  This is the header for the variable test node. This will compare the value of a
//  variable to a value, both of which are provided as parameters, and return success
//  if they are equal.
//
//  This node takes the following parameters:
//
//  VarTestVar  - The name of the target variable to test
//  VarTestVal  - The name of the value within the variable to test
//  VarTestComp - The value to test the variable value against
//
// LOG:
//
//  $_CIDLib_Log_$
//


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TAIBTVarTestNode
//  PREFIX: btnode
// ---------------------------------------------------------------------------
class CIDAIEXP TAIBTVarTestNode : public TAIBTNode
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TAIBTVarTestNode
        (
            const   TString&                strPath
            , const TString&                strName
        );

        TAIBTVarTestNode(const TAIBTVarTestNode&) = delete;

        ~TAIBTVarTestNode();


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
        RTTIDefs(TAIBTVarTestNode, TAIBTNode)
};

#pragma CIDLIB_POPPACK
