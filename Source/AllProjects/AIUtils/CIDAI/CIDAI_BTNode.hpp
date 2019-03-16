//
// FILE NAME: CIDAI_BTNode.hpp
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
//  This is the header for the base node from which all of the behavior tree nodes derive.
//  We don't bother using the tree collection, since the requirements here are very basic.
//  This base node class just has an adopting vector that holds the nodes, and the
//  derived classes indicate, when they call our ctor, the min and max number of nodes
//  they can accept. This class manages those nodes, and the derived class can just ask
//  to run nodes a specific indices.
//
//  Any given node in a tree is uniquely identified by a 'path' which is a slash separated
//  list of the index of each node within its parent, e.g. /0/3/2/0/0. Since a tree, once
//  built, never changes, this is a valid way to find a given node.
//
//  Since this class manages the child nodes, and the derived class has to call down to
//  here to run them, we manage a stack in the context object by pushing the newly run
//  child's index into the stack, and popping it when the child ends. So we always have
//  a path to the currently running node and the path down to it, without having to store
//  any pointers.
//
//
//  The derived class has to override eRun() and run them in the required order; and, based
//  on that, return the correct result.
//
//  Each node can also override Validate() to do any checking that is appropriate. They
//  should also always call us as well. We check the number of child nodes to make sure
//  they are within the requested min/max, and any other things that we can check at this
//  level.
//
//  One gotcha of course is a link node, which is just a way to link to another tree. In
//  that case, the other tree must be found by name. Every node CAN be given a name for
//  debugging purposes, but the root node of a tree must be given a node for lookup
//  purposes.
//
//  Each node as is also given a human readable path as the tree is created. It is in
//  the form of a forward slash delimited string, where each component is the index of the
//  child within the parent, so that that node can be located within the original XML
//  file.
//
//
//
//  !!!!!! NODES MUST BE REENTRANT!
//
//  So, as a rule, the nodes will never have any members, other than the base node class
//  that stores the node type, name, child nodes, etc... Otherwise, the nodes should
//  always operate on context variables (which they can define in terms of the names of
//  variables they look at as inputs and outputs.) They should not even pre-process their
//  node parameters that came from the XML file. We might at some point allow those to be
//  changed dynamically.
//
//  So, except in perhaps some extraordinary circumstances, they will never have any
//  members so as to be completely re-entrant. They will operate on variables stored in
//  the context object.
//
//  !!!!!!
//
//  The above also means that each node is inherently 'reset' once it returns, because it
//  stores no state. So there is no requirement to either reset the whole tree upon start
//  of a pass (which could be a lot of overhead since they can be quite larger) or to try
//  to do any sort of faulted in initialization.
//
//
// CAVEATS/GOTCHAS:
//
//  1.  Because of the mixed nature of behavior tree nodes, and the need to be able use
//      factory objects to gen them up, construction parameters are tricky. So it is all
//      done via a collection of key/value pairs. Factory objects only have a single method
//      to gen up objects, so it can't take all kinds of different parameters. And the
//      parameters come from XML anyway, so they are already text.
//
//  2.  Node type paths have the form /[factoryname]/[type]. So the first part is the name of
//      the factory that knows how to create it (which will be used to find that factory
//      one needs to be created, and then the actual type, which the factory will use to
//      create it. For built in node types, the first part is /CIDAI/. Client code cannot
//      use that prefix, they must select one of their own, and register a factory with our
//      facility class under the name (not including the two slashes.)
//
//  3)  Neither the tree nor the context object require any sync because only one thread uses
//      them at a time, and the movement through the nodes is single threaded.
//
// LOG:
//
//  $_CIDLib_Log_$
//


#pragma CIDLIB_PACK(CIDLIBPACK)

class TAIBTNode;
class TAIBTNodeFact;
class TAIBehaviorTree;


// ---------------------------------------------------------------------------
//   CLASS: TAIBTErr
//  PREFIX: bterr
// ---------------------------------------------------------------------------
class CIDAIEXP TAIBTErr
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TAIBTErr();

        TAIBTErr
        (
            const   TString&                strNodePath
            , const TString&                strNodeType
            , const TString&                strErr
        );

        ~TAIBTErr();


        // -------------------------------------------------------------------
        //  Public data members
        // -------------------------------------------------------------------
        TString     m_strErr;
        TString     m_strNodePath;
        TString     m_strNodeType;
};




// ---------------------------------------------------------------------------
//   CLASS: TAIBTNode
//  PREFIX: btnode
// ---------------------------------------------------------------------------
class CIDAIEXP TAIBTNode : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strKey
        (
            const   TAIBTNode&              btnodeSrc
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TAIBTNode(const TAIBTNode&) = delete;

        ~TAIBTNode();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TAIBTNode operator=(const TAIBTNode&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDAI::ENodeStates eRun
        (
                    TAIBehaviorTree&        btreeOwner
        ) = 0;

        virtual tCIDLib::TVoid Validate
        (
                    TAIBehaviorTree&        btreeOwner
            ,       TVector<TAIBTErr>&      colToFill
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bFlag() const;

        tCIDLib::TBoolean bFlag
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TCard4 c4ChildCount() const;

        const tCIDLib::TKVPList& colParams() const;

        TAIBTNode* pbtnodeAddChild
        (
            const   TString&                strName
            , const TString&                strType
            , const tCIDLib::TBoolean       bFlag
        );

        const TAIBTNode* pbtnodeChildAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        const TAIBTNode* pbtnodeChildByType
        (
            const   TString&                strToFind
        )   const;

        const TString& strFindParam
        (
            const   TString&                strToFind
        )   const;

        const TString& strName() const;

        const TString& strType() const;

        const TString& strPath() const;


    protected :
        // -------------------------------------------------------------------
        //  The parser is our friend so he can force some stuff on all nodes that the
        //  rest of the world can only read.
        // -------------------------------------------------------------------
        friend class TCIDAIBTParser;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TAIBTNode
        (
           const    TString&                strPath
           ,  const TString&                strName
            , const TString&                strNodeType
            , const tCIDLib::TCard4         c4MinChildCnt
            , const tCIDLib::TCard4         c4MaxChildCnt
        );


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bChildExists
        (
            const   TString&                strName
            ,       tCIDLib::TCard4&        c4Index
        )   const;

        tCIDAI::ENodeStates eRunChildAt
        (
                    TAIBehaviorTree&        btreeOwner
            , const tCIDLib::TCard4         c4At
        );

        tCIDAI::ENodeStates eRunChildByName
        (
                    TAIBehaviorTree&        btreeOwner
            , const TString&                strToRun
        );

        tCIDAI::ENodeStates eRunChildByType
        (
                    TAIBehaviorTree&        btreeOwner
            , const TString&                strToRun
        );

        tCIDAI::ENodeStates eRunNode
        (
                    TAIBehaviorTree&        btreeOwner
            ,       TAIBTNode&              btnodeToRun
        );

        tCIDLib::TVoid SetParams
        (
            const   tCIDLib::TKVPList&      colToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckChildIndex
        (
            const   tCIDLib::TCard4         c4ToCheck
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bFlag
        //      A flag that the application can use.
        //
        //  m_c4MaxChildCnt
        //  m_c4MinChildCnt
        //      The derived class tells our ctor the min/max number of children it can
        //      have. This lets us verify the XML content when we are parsing this
        //      node's children.
        //
        //  m_colChildren
        //      The child nodes. It will have from m_c4MinChildCnt children to
        //      m_c4MaxChildCnt children.
        //
        //  m_colParams
        //      We store the incoming parameters here for later use during init or run
        //      as required.
        //
        //  m_strPath
        //      Our path (list of child indices) through the true to get to this node
        //          uniquely.
        //
        //  m_strName
        //      The name given to this node in the tree configuration. This is some
        //      sort of semantic type name or something that will make it findable
        //      in the tree given the full path.
        //
        //  m_strNodeType
        //      The path style type indicator. This is required because client code can
        //      create their own types of nodes, and this avoids the need for the overhead
        //      of dynamic RTTI. And it allows us to easily support factory objects that
        //      client code can use to gen up nodes of its own types. We get the path from
        //      the XML and can see immediately, by path convention, who knows how to create
        //      it.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bFlag;
        tCIDLib::TCard4         m_c4MaxChildCnt;
        tCIDLib::TCard4         m_c4MinChildCnt;
        TRefVector<TAIBTNode>   m_colChildren;
        tCIDLib::TKVPList       m_colParams;
        TString                 m_strPath;
        TString                 m_strName;
        TString                 m_strNodeType;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TAIBTNode,TObject)
};

#pragma CIDLIB_POPPACK
