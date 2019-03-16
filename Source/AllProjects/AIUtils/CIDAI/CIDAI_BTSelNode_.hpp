//
// FILE NAME: CIDAI_BTSelNode_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/08/2016
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
//  This is the header for the selector node type. It can have N children and works
//  through them in order until one of them succeeds (at which point it returns success)
//  or they all fail in which case it returns failure.
//
// LOG:
//
//  $_CIDLib_Log_$
//


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TAIBTSelNode
//  PREFIX: btnode
// ---------------------------------------------------------------------------
class CIDAIEXP TAIBTSelNode : public TAIBTNode
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TAIBTSelNode
        (
            const   TString&                strPath
            , const TString&                strName
        );

        TAIBTSelNode(const TAIBTSelNode&) = delete;

        ~TAIBTSelNode();


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
        RTTIDefs(TAIBTSelNode, TAIBTNode)
};

#pragma CIDLIB_POPPACK
