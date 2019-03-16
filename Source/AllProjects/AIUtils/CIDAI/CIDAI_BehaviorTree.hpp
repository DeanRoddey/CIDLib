//
// FILE NAME: CIDAI_BehaviorTree.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/07/2016
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
//  This is the header for a simple class that manages a behavior tree. It handles
//  some grunt work and makes sure it gets done correctly. It also handles management
//  of multiple trees so that trees can reference other trees.
//
//  There are two mechanisms involved here. One allows the containing program to load
//  up all of the required trees. We parse the XML and store the trees away. The tree
//  can then be run.
//
//  Or that can be used to spit out a single, binary file that can also later be loaded
//  back into a tree object. This is how generally they will be distributed for
//  production use. So we have XML and binary loading methods, and we have a 'write to
//  bin' type method to take all of the loaded trees and spit them out in a format that
//  can be later read by in.
//
//  Because of the fact that trees have to be defined in a very generic way via XML,
//  that means that all of the info required to spit out the binary file is available
//  without all of the node derivatives having to support streaming methods. We just
//  write out a binary version of what is in the XML files basically.
//
//  All trees have to be named, and that is now references are done. There is a special
//  node type that exists just to act as a passthrough to an external node. Upon load of
//  the tree those nodes will look up the target trees they reference. So, when doing the
//  XML loading scheme, the referenced trees must be loaded first and upwards. When
//  the binary version is spit out, they will automatically be done in that order.
//
//  The last tree loaded is the 'top level' tree that will be invoked. All other trees
//  before it in the list are ones that are directly or indirectly referenced by it.
//
// LOG:
//
//  $_CIDLib_Log_$
//


#pragma CIDLIB_PACK(CIDLIBPACK)

class TAIBTRootNode;

// ---------------------------------------------------------------------------
//   CLASS: TAIBehaviorTree
//  PREFIX: btree
// ---------------------------------------------------------------------------
class CIDAIEXP TAIBehaviorTree : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TAIBehaviorTree
        (
            const   TString&                strName
        );

        TAIBehaviorTree(const TAIBehaviorTree&) = delete;

        ~TAIBehaviorTree();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bStop() const;

        tCIDLib::TBoolean bStop
        (
            const   tCIDLib::TBoolean       bToSet
        );

        const TAIBTCtx& ctxToUse() const;

        TAIBTCtx& ctxToUse();

        tCIDLib::TVoid CompileToFile
        (
                    TBinOutStream&          strmTar
        );

        tCIDAI::ENodeStates eRun
        (
            const   TString&                strTreeName
            , const tCIDLib::TKValsList&    colInitVars
        );

        tCIDAI::ENodeStates eRun
        (
            const   TString&                strTreeName
            , const tCIDLib::TKValsList&    colInitVars
            , const tCIDLib::TKNumList&     colKNumList
        );

        tCIDLib::TVoid LoadXMLTree
        (
            const   TString&                strToParse
        );

        tCIDLib::TVoid LoadXMLTree
        (
            const   TMemBuf&                mbufData
            , const tCIDLib::TCard4         c4ByteCnt
        );

        const TAIBTNode* pbtnodeCur() const;

        TAIBTNode* pbtnodeCur();

        TAIBTRootNode* pbtnodeFindTree
        (
            const   TString&                strTreeName
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::True
        );

        tCIDLib::TVoid PopNode();

        tCIDLib::TVoid PushNode
        (
                    TAIBTNode* const        pbtnodePush
        );

        tCIDLib::TVoid Reset();

        const TString& strName() const;

        tCIDLib::TVoid SetAppInfo
        (
            const   tCIDLib::TKNumList&     colKNumList
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CompilerHelper
        (
            const   TAIBTRootNode&          btnodeTree
            ,       tCIDLib::TStrHashSet&   colTreesDone
            ,       TBinOutStream&          strmTar
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bStop
        //      A convenient way to force a long running node to stop, without
        //      actually having to force the thread running the tree to stop. Any
        //      node whould check this in any loop and return Cancel status if it sees
        //      this set. It will be reset up each time eRun() is called.
        //
        //  m_colCallStack
        //      The the base node class pushes each child onto the stack when the
        //      derived class asks to run it, and pops it back off when done. This is
        //      non-adopting of course.
        //
        //  m_colTrees
        //      A list of trees that have been loaded into us. It's a list of root
        //      nodes since all trees must start with a root node. This is an adopting
        //      list. The trees must be added in order of dependency, so that each
        //      newly added tree can resolve references as it is loaded. Each tree
        //      must have a unique name within the list of trees added.
        //
        //  m_ctxToUse
        //      We need a context object to run the tree.
        //
        //  m_strName
        //      A name for the tree for debugging purposes and such.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bStop;
        tCIDAI::TNodeStack  m_colCallStack;
        tCIDAI::TTreeList   m_colTrees;
        TAIBTCtx            m_ctxToUse;
        TString             m_strName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TAIBehaviorTree, TObject)
};

#pragma CIDLIB_POPPACK
