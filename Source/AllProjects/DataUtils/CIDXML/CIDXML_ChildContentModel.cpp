//
// FILE NAME: CIDXML_ChildContentModel.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/23/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TXMLChildCM class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDXML_.hpp"
#include    "CIDXML_CMNodes_.hpp"



// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TXMLChildCM,TXMLContentModel)


namespace CIDXML_ChildCM
{
    // -----------------------------------------------------------------------
    //  Local static data
    //
    //  c4EOCId
    //  c4EpsilonId
    //      There are a couple of special case values that are used here,
    //      which are used like element ids but which are not real element
    //      decls. So we provide these very large fake ids for that.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4    c4EOCId       = kCIDLib::c4MaxCard - 1;
    const tCIDLib::TCard4    c4EpsilonId   = kCIDLib::c4MaxCard - 2;
}



// ---------------------------------------------------------------------------
//  Local methods
// ---------------------------------------------------------------------------
static tCIDLib::TVoid FormatCS(         TTextOutStream&         strmDest
                                , const TXMLValidator&          xvalSrc
                                ,       TXMLCMSpecNode* const   pxcsnCur
                                ,       TString&                strToUse
                                , const tCIDXML::ECMNodeTypes   eLastType)
{
    #if CID_DEBUG_ON
    if (!pxcsnCur)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcCM_CurNodeNull
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif

    //
    //  If its a leaf, we just display the element. If its a rep node, then
    //  we recurse on its child and then display the rep char.
    //
    //  Else its a binary node. In this case we check the left node. If its
    //  a binary, we recurse and then display the right leaf if there is one
    //  there. If the left is a leaf, we just display the leaf and there will
    //  not be any right child.
    //
    const tCIDXML::ECMNodeTypes eType = pxcsnCur->eNodeType();

    switch(eType)
    {
        case tCIDXML::ECMNodeTypes::Leaf :
        {
            // Get the name of the element id
            const TXMLElemDecl* pxdeclLeaf = xvalSrc.pxdeclFindElemById
            (
                pxcsnCur->c4ElemId()
            );

            if (!pxdeclLeaf)
            {
                facCIDXML().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kXMLErrs::errcPool_IdNotInPool
                    , tCIDLib::ESeverities::ProcFatal
                    , tCIDLib::EErrClasses::Internal
                    , TCardinal(pxcsnCur->c4ElemId())
                    , facCIDXML().strMsg(kXMLMsgs::midGen_element)
                );
            }
            strmDest << pxdeclLeaf->strFullName();
            break;
        }

        case tCIDXML::ECMNodeTypes::ZeroOrOne :
        case tCIDXML::ECMNodeTypes::ZeroOrMore :
        case tCIDXML::ECMNodeTypes::OneOrMore :
            //
            //  Recurse, then display the rep char. We recurse on the left
            //  child which is the only valid one for a rep node.
            //
            FormatCS
            (
                strmDest
                , xvalSrc
                , pxcsnCur->pxcsnLeft()
                , strToUse
                , eLastType
            );

            // Now display the rep char
            if (eType == tCIDXML::ECMNodeTypes::ZeroOrOne)
                strmDest << L"?";
            else if (eType == tCIDXML::ECMNodeTypes::ZeroOrMore)
                strmDest << L"*";
            else if (eType == tCIDXML::ECMNodeTypes::OneOrMore)
                strmDest << L"+";
            break;

        case tCIDXML::ECMNodeTypes::Alternation :
        case tCIDXML::ECMNodeTypes::Sequence :
        {
            //
            //  If the previous type was one of these, and this one is a
            //  different one, then we must be a nested subclause. So set
            //  a bool that will tell us below whether to output bracketing
            //  parens
            //
            const tCIDLib::TBoolean bNested =
            (
                (eLastType == tCIDXML::ECMNodeTypes::Alternation)
                || (eLastType == tCIDXML::ECMNodeTypes::Sequence)
                && (eType != eLastType)
            );

            if (bNested)
                strmDest << L"(";

            //
            //  First, recurse on the left node. This is will be the
            //  leaf node for the next element in the set. Then recurse
            //  on the right if there is one. If not, then we've hit the
            //  end.
            //
            FormatCS
            (
                strmDest
                , xvalSrc
                , pxcsnCur->pxcsnLeft()
                , strToUse
                , eType
            );

            // Output the character for the seq/alt
            if (eType == tCIDXML::ECMNodeTypes::Alternation)
                strmDest << L"|";
            else
                strmDest << L",";

            FormatCS
            (
                strmDest
                , xvalSrc
                , pxcsnCur->pxcsnRight()
                , strToUse
                , eType
            );

            if (bNested)
                strmDest << L")";
            break;
        }

        default :
            break;
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TXMLChildCM
// PREFIX: xcm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLChildCM: Constructors and Destructor
// ---------------------------------------------------------------------------
TXMLChildCM::TXMLChildCM(TXMLCMSpecNode* const pxcsnToAdopt) :

    m_bCanBeEmpty(kCIDLib::False)
    , m_c4ElemListSize(0)
    , m_c4EOCPosition(0)
    , m_c4LeafCount(0)
    , m_c4TransTableSize(0)
    , m_ppc4TransTable(0)
    , m_pbFinalFlags(0)
    , m_pbtsFollowLists(0)
    , m_pc4ElemList(0)
    , m_pxcmnLeaves(0)
    , m_pxcmnRoot(0)
    , m_pxcsnRoot(pxcsnToAdopt)
{
    // Lets call a private method to build the DFA data structures
    BuildDFA();

// A little debug code to print the transition table
#if 0
    TOutConsole strmDest;
    for (tCIDLib::TCard4 c4Outer = 0; c4Outer < m_c4TransTableSize; c4Outer++)
    {
        // We need to print out the element names for this table entry
        for (tCIDLib::TCard4 c4Inner = 0; c4Inner < m_c4ElemListSize; c4Inner++)
        {
            if (c4Inner)
                strmDest << L",";

            if (m_ppc4TransTable[c4Outer][c4Inner] == kCIDLib::c4MaxCard)
                strmDest << L"XX";
            else
                strmDest << m_ppc4TransTable[c4Outer][c4Inner];
        }
        strmDest << kCIDLib::NewLn;
    }
#endif
}

TXMLChildCM::~TXMLChildCM()
{
    // Clean up the info we had to keep around the whole time
    delete [] m_pbFinalFlags;
    delete [] m_pc4ElemList;
    delete m_pxcsnRoot;

    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4TransTableSize; c4Index++)
        delete [] m_ppc4TransTable[c4Index];
    delete [] m_ppc4TransTable;
}


// ---------------------------------------------------------------------------
//  TXMLChildCM: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDXML::EValidRes
TXMLChildCM::eValidate( const   tCIDLib::TCard4* const  pc4ChildIds
                        , const tCIDLib::TCard4         c4ChildCount
                        ,       tCIDLib::TCard4&        c4FailedAt) const
{
    //
    //  We can quickly check an empty content. During the DFA build we stored
    //  a flag that indicates whether empty content is legal.
    //
    if (!c4ChildCount)
    {
        if (m_bCanBeEmpty)
            return tCIDXML::EValidRes::Success;

        c4FailedAt = 0;
        return tCIDXML::EValidRes::TooFew;
    }

    //
    //  We just go through each child in the list and, for each one, look it
    //  up in the current entry in the transition table. If its a legal
    //  transition, we move to that entry for the next time, else the content
    //  is not legal.
    //
    tCIDLib::TCard4 c4CurState = 0;
    for (c4FailedAt = 0; c4FailedAt < c4ChildCount; c4FailedAt++)
    {
        // Remember the current child element id and look it up in the list
        const tCIDLib::TCard4 c4CurId = pc4ChildIds[c4FailedAt];

        tCIDLib::TCard4 c4ElemInd = 0;
        for (; c4ElemInd < m_c4ElemListSize; c4ElemInd++)
        {
            if (m_pc4ElemList[c4ElemInd] == c4CurId)
                break;
        }

        // If not in the list, then we failed
        if (c4ElemInd == m_c4ElemListSize)
            return tCIDXML::EValidRes::Mismatch;

        // See if there is a legal transition from here
        c4CurState = m_ppc4TransTable[c4CurState][c4ElemInd];

        if (c4CurState == kCIDLib::c4MaxCard)
            return tCIDXML::EValidRes::Mismatch;
    }

    // If we ended up on a non-final state, we failed also
    if (!m_pbFinalFlags[c4CurState])
        return tCIDXML::EValidRes::TooFew;

    return tCIDXML::EValidRes::Success;
}


tCIDLib::TVoid
TXMLChildCM::FormatTo(          TTextOutStream& strmDest
                        , const TXMLValidator&  xvalSrc) const
{
    // Output the opening paren
    strmDest << L"(";

    //
    //  We call a local recursive method which will run down through the
    //  content spec tree and format it. Pass it a buffer to use so that
    //  it won't create one over and over during recursion.
    //
    TString strToUse;
    FormatCS
    (
        strmDest
        , xvalSrc
        , m_pxcsnRoot
        , strToUse
        , tCIDXML::ECMNodeTypes::Unknown
    );

    // And the closing paren
    strmDest << L")";
}


// ---------------------------------------------------------------------------
//  TXMLChildCM: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TXMLChildCM::BuildDFA()
{
    tCIDLib::TCard4 c4Index;

    //
    //  The first step we must take is to rewrite the original content spec
    //  node tree into a new tree which is more amenable to the NFA to DFA
    //  process. The nodes of this tree are specialized for this operation,
    //  but we also have to take some of the expressions and break them down
    //  into more fundamental operations.
    //
    //  foo+  becomes  (foo|foo*)
    //  foo?  becomes  (foo|epsilon)
    //
    //  When done, we have a top level binary node with the rewritten tree
    //  on the left and the EOC special node on the right.
    //
    //  While doing this, we also number the leaf nodes with the position
    //  numbers required during the NFA to DFA transformation.
    //
    //  The rewrite method is recursive, so we pass it the root node of
    //  the content spec node tree. It walks that tree and creates another
    //  tree which it returns.
    //
    TXMLCMLeaf* pxcmnEOC = new TXMLCMLeaf(CIDXML_ChildCM::c4EOCId);
    TXMLCMNode* pxcmnOrgTree = pxcmnRewriteTree(m_pxcsnRoot);

    // Create the new root node and store it in our member
    m_pxcmnRoot = new TXMLCMBinOp
    (
        tCIDXML::ECMNodeTypes::Sequence
        , pxcmnOrgTree
        , pxcmnEOC
    );

    //
    //  Now we know how many leaf nodes are present, and we've numbered them
    //  all in the correct sequence. So we can store the EOC leaf position
    //  in a member (since we'll need to use it a lot later.) And we can
    //  then set the EOC leaf node's position number and bump the leaf count
    //  to take the EOC leaf into account.
    //
    m_c4EOCPosition = m_c4LeafCount;
    pxcmnEOC->c4StatePos(m_c4LeafCount++);

    //
    //  And we have to iterate the new tree, now that we know the real leaf
    //  count, and set up all of their first/last position bit sets. As we
    //  are doing this, we get an array of pointers to the leaf nodes. This
    //  will be used later on.
    //
    //  NOTE: We only store non-epsilon leaf nodes! So the array might be
    //  a little bigger than is really needed.
    //
    m_pxcmnLeaves = new TXMLCMLeaf*[m_c4LeafCount];
    c4InitLeaves(m_pxcmnRoot, 0);

    //
    //  Now we have to calculate the follow list for each non-epsilon leaf
    //  node. So we allocate an array of bit sets, with one entry for each
    //  leaf node in our lists of leaves. Then we call the method that bulds
    //  the lists. Like the other ones, its recursive and works its way
    //  through the tree.
    //
    m_pbtsFollowLists = new TBitset[m_c4LeafCount];
    for (c4Index = 0; c4Index < m_c4LeafCount; c4Index++)
        m_pbtsFollowLists[c4Index].ChangeBitCount(m_c4LeafCount);
    BuildFollowLists(m_pxcmnRoot);

    //
    //  If the original content part of the rewritten tree is nullable, then
    //  zero children is legal for this model. So store this value once for
    //  much faster checking of this special case later.
    //
    m_bCanBeEmpty = pxcmnOrgTree->bIsNullable();

    //
    //  Now we need to create an array of unique ids of the elements that can
    //  be in this content model. This is used to map element ids into a zero
    //  based index into transition tables.
    //
    m_pc4ElemList = new tCIDLib::TCard4[m_c4LeafCount];
    m_c4ElemListSize = 0;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4LeafCount; c4Index++)
    {
        // Get the element id of the current leaf node
        const tCIDLib::TCard4 c4ElemId = m_pxcmnLeaves[c4Index]->c4ElemId();

        // See if this one is already in our list
        tCIDLib::TCard4 c4InnerIndex = 0;
        for (; c4InnerIndex < m_c4ElemListSize; c4InnerIndex++)
        {
            if (m_pc4ElemList[c4InnerIndex] == c4ElemId)
                break;
        }

        // If it was not, then add it
        if (c4InnerIndex == m_c4ElemListSize)
            m_pc4ElemList[m_c4ElemListSize++] = c4ElemId;
    }

    //
    //  And now we need to create a couple more data structures before we
    //  enter the algorithm loop. We need to have a stack of states that need
    //  to be checked, an array of boolean values that indicate which states
    //  are final states, and the actual transition table for the DFA, which
    //  is an array of pointers to arrays to TCard4 values.
    //
    //  We cannot say for sure how many elements these will have, but we have
    //  a reasonable idea what they be on average, which is the leaf count
    //  times two. So we use the leaf count times 4 on the assumption that
    //  few expansions of these structures will be required, without using an
    //  undue amount of memory.
    //
    tCIDLib::TCard4 c4CurAlloc = m_c4LeafCount * 4;
    const TBitset** ppbtsToDoStack = new const TBitset*[c4CurAlloc];
    m_pbFinalFlags = new tCIDLib::TBoolean[c4CurAlloc];
    m_ppc4TransTable = new tCIDLib::TCard4*[c4CurAlloc];

    //
    //  We have two state flags used during the loop. As the algorithm runs,
    //  we add new states to the stack to be dealt with, and we deal with them
    //  as we get the chance. So we need a current top of stack marker and
    //  a current stack index marker. When the index catches up with the stack
    //  top, we are done (i.e. no new state sets were added to the stack during
    //  the last loop.)
    //
    tCIDLib::TCard4 c4StackTop = 0;
    tCIDLib::TCard4 c4StackIndex = 0;

    //
    //  Now we add the initial set to the state stack as one to be dealt with
    //  and bump up the stack top. We also init the transition table entry for
    //  the new state with all special values that indicate no legal transition
    //  for that element. It will be updated with the transitions that are
    //  legal as we process this state.
    //
    m_ppc4TransTable[c4StackTop] = pc4NewTransTableEntry();
    ppbtsToDoStack[c4StackTop] = new TBitset(m_pxcmnRoot->btsFirstPos());
    c4StackTop++;

    //
    //  And, whew finally, we can loop until no new states are on the stack
    //  to process. We need a temp set pointer that will be used to build up
    //  the set for each round. We reuse it until we have to store it, then
    //  we create a new one. And we need anoterh temp set pointer to hold the
    //  set that we are currently working on.
    //
    TBitset* pbtsNew = 0;
    const TBitset* pbtsCur = 0;
    while (c4StackIndex < c4StackTop)
    {
        //
        //  Get pointers to the next start to do from the set stack. And we
        //  need a pointer to the entry in the transition table for this
        //  state.
        //
        pbtsCur = ppbtsToDoStack[c4StackIndex];
        tCIDLib::TCard4* pc4TransEntry = m_ppc4TransTable[c4StackIndex];

        // Remember if its final or not
        m_pbFinalFlags[c4StackIndex] = pbtsCur->bBitState(m_c4EOCPosition);

        // And now bump the stack index up to the next slot
        c4StackIndex++;

        //
        //  Now we loop through each of the possible input elements, which
        //  we figured out above when we set up the unique element list.
        //
        for (tCIDLib::TCard4 c4ElemInd = 0; c4ElemInd < m_c4ElemListSize; c4ElemInd++)
        {
            //
            //  We have to create a set of states which is the union of all of
            //  the follow sets of leaves which are in the current state. Reset
            //  the temp bitset, or create a new one if we store the last one.
            //
            if (pbtsNew)
                pbtsNew->Clear();
            else
                pbtsNew = new TBitset(m_c4LeafCount);

            for (tCIDLib::TCard4 c4LeafInd = 0; c4LeafInd < m_c4LeafCount; c4LeafInd++)
            {
                // Check if the current leaf is in the current set
                if (pbtsCur->bBitState(c4LeafInd))
                {
                    //
                    //  If the current leaf is the the current element in the
                    //  element list, then add its follow list to the set of
                    //  transition states for the current state.
                    //
                    if (m_pxcmnLeaves[c4LeafInd]->c4ElemId() == m_pc4ElemList[c4ElemInd])
                        *pbtsNew |= m_pbtsFollowLists[c4LeafInd];
                }
            }

            //
            //  If we set any bits in the new set, then we have to see if this
            //  set of states is already on the stack. If not, we have to add
            //  it as another state set to process.
            //
            if (!pbtsNew->bAllCleared())
            {
                tCIDLib::TCard4 c4SearchInd = 0;
                for (; c4SearchInd < c4StackTop; c4SearchInd++)
                {
                    // If we find it, then break out
                    if (*ppbtsToDoStack[c4SearchInd] == *pbtsNew)
                        break;
                }

                // If we didn't find it, then we have to add it to the stack
                if (c4SearchInd == c4StackTop)
                {
                    ppbtsToDoStack[c4StackTop] = pbtsNew;
                    m_ppc4TransTable[c4StackTop] = pc4NewTransTableEntry();

                    // We've used the temp set so zero it out now
                    pbtsNew = 0;

                    // And bump up the stack top
                    c4StackTop++;
                }

                // Set this state in the current transition table
                pc4TransEntry[c4ElemInd] = c4SearchInd;

                //
                //  If our current guess at the required storage was too small,
                //  then we need to expand our arrays.
                //
                if (c4StackTop == c4CurAlloc)
                {
                    // Calculate the new size
                    const tCIDLib::TCard4 c4NewSize = tCIDLib::TCard4(c4CurAlloc * 1.25);

                    // Allocate the new replacments
                    const TBitset** ppbtsNew = new const TBitset*[c4NewSize];
                    tCIDLib::TBoolean* pbNew = new tCIDLib::TBoolean[c4NewSize];
                    tCIDLib::TCard4** ppc4New = new tCIDLib::TCard4*[c4NewSize];

                    // Copy over everything from the old lists
                    TRawMem::CopyMemBuf(ppbtsNew, ppbtsToDoStack, c4CurAlloc * sizeof(ppbtsNew[0]));
                    TRawMem::CopyMemBuf(pbNew, m_pbFinalFlags, c4CurAlloc * sizeof(pbNew[0]));
                    TRawMem::CopyMemBuf(ppc4New, m_ppc4TransTable, c4CurAlloc * sizeof(ppc4New[0]));

                    // Clean up the old stuff
                    delete [] ppbtsToDoStack;
                    delete [] m_pbFinalFlags;
                    delete [] m_ppc4TransTable;

                    // And store all the new stuff
                    c4CurAlloc = c4NewSize;
                    ppbtsToDoStack = ppbtsNew;
                    m_pbFinalFlags = pbNew;
                    m_ppc4TransTable = ppc4New;
                }
            }
        }
    }

    // Get rid of the temp set if it was left over
    if (pbtsNew)
        delete pbtsNew;

    // The current stack top is the size of the transition table
    m_c4TransTableSize = c4StackTop;

    //
    //  Clean up all of the temporary data structures that are no longer
    //  needed, now that we've built the transition tables.
    //
    delete [] m_pxcmnLeaves;
    m_pxcmnLeaves = 0;
    delete [] m_pbtsFollowLists;
    m_pbtsFollowLists = 0;
    delete m_pxcmnRoot;
    m_pxcmnRoot = 0;

    for (c4Index = 0; c4Index < c4StackTop; c4Index++)
        delete ppbtsToDoStack[c4Index];
    delete [] ppbtsToDoStack;
}


//
//  This method is called to traverse the rewritten content model tree and
//  calculate the follow list for each state.
//
tCIDLib::TVoid TXMLChildCM::BuildFollowLists(TXMLCMNode* const pxcmnCur)
{
    #if CID_DEBUG_ON
    if (!pxcmnCur)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcCM_CurNodeNull
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif

    // For convenience, get the node type out
    const tCIDXML::ECMNodeTypes eType = pxcmnCur->eNodeType();

    switch(eType)
    {
        case tCIDXML::ECMNodeTypes::Alternation :
        {
            // Just recurse on each child
            TXMLCMBinOp* pxcmnBin = (TXMLCMBinOp*)pxcmnCur;
            BuildFollowLists(pxcmnBin->pxcmnLeft());
            BuildFollowLists(pxcmnBin->pxcmnRight());
            break;
        }

        case tCIDXML::ECMNodeTypes::Sequence :
        {
            // First recurse on each child
            TXMLCMBinOp* pxcmnBin = (TXMLCMBinOp*)pxcmnCur;
            BuildFollowLists(pxcmnBin->pxcmnLeft());
            BuildFollowLists(pxcmnBin->pxcmnRight());

            //
            //  Now update each set in the list that would affected by our
            //  first/last sets. For every position which is in our left
            //  child's last set, add allof the states in our right child's
            //  first set to the follow set for that position.
            //
            const TBitset& btsLast = pxcmnBin->pxcmnLeft()->btsLastPos();
            const TBitset& btsFirst = pxcmnBin->pxcmnRight()->btsFirstPos();
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4LeafCount; c4Index++)
            {
                if (btsLast.bBitState(c4Index))
                    m_pbtsFollowLists[c4Index] |= btsFirst;
            }
            break;
        }

        case tCIDXML::ECMNodeTypes::ZeroOrMore :
        {
            // Recurse on the child first
            TXMLCMUnaryOp* pxcmnUnary = (TXMLCMUnaryOp*)pxcmnCur;
            BuildFollowLists(pxcmnUnary->pxcmnChild());

            //
            //  And then update the follow sets for any leaves that would be
            //  affected by our first/last sets. For every position which is
            //  in our last pos, add all of our first pos states to the follow
            //  set for that position.
            //
            const TBitset& btsFirst = pxcmnCur->btsFirstPos();
            const TBitset& btsLast = pxcmnCur->btsLastPos();
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4LeafCount; c4Index++)
            {
                if (btsLast.bBitState(c4Index))
                    m_pbtsFollowLists[c4Index] |= btsFirst;
            }
            break;
        }

        case tCIDXML::ECMNodeTypes::Leaf :
            // Leaves are ignored here
            break;

        default :
            #if CID_DEBUG_ON
            facCIDXML().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kXMLErrs::errcCM_BadCMTypeHere
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::Internal
                , TInteger(tCIDLib::i4EnumOrd(eType))
            );
            #endif
            break;
    }
}


//
//  This method is called after the tree is rewritten and the number of leaves
//  is known. We tell each node the leaf count and we store each non-epsilon
//  leaf in an array of pointers to CM nodes.
//
tCIDLib::TCard4
TXMLChildCM::c4InitLeaves(          TXMLCMNode* const   pxcmnCur
                            , const tCIDLib::TCard4     c4CurIndex)
{
    #if CID_DEBUG_ON
    if (!pxcmnCur)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcCM_CurNodeNull
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif

    // Tell this node the leaf count
    pxcmnCur->c4MaxStates(m_c4LeafCount);

    // Get a copy of the index so we can bump it up during our child processing
    tCIDLib::TCard4 c4LocalIndex = c4CurIndex;

    // For convenience, get the node type out
    const tCIDXML::ECMNodeTypes eType = pxcmnCur->eNodeType();

    switch(eType)
    {
        case tCIDXML::ECMNodeTypes::Alternation :
        case tCIDXML::ECMNodeTypes::Sequence :
        {
            TXMLCMBinOp* pxcmnBin = (TXMLCMBinOp*)pxcmnCur;
            TXMLCMNode* pxcmnLeft = pxcmnBin->pxcmnLeft();
            TXMLCMNode* pxcmnRight = pxcmnBin->pxcmnRight();
            c4LocalIndex = c4InitLeaves(pxcmnLeft, c4LocalIndex);
            c4LocalIndex = c4InitLeaves(pxcmnRight, c4LocalIndex);
            break;
        }

        case tCIDXML::ECMNodeTypes::ZeroOrMore :
        {
            TXMLCMUnaryOp* pxcmnUnary = (TXMLCMUnaryOp*)pxcmnCur;
            c4LocalIndex = c4InitLeaves(pxcmnUnary->pxcmnChild(), c4LocalIndex);
            break;
        }

        case tCIDXML::ECMNodeTypes::Leaf :
        {
            // If its a non-epsilon node, save it away
            TXMLCMLeaf* pxcmnLeaf = (TXMLCMLeaf*)pxcmnCur;
            if (pxcmnLeaf->c4ElemId() != CIDXML_ChildCM::c4EpsilonId)
                m_pxcmnLeaves[c4LocalIndex++] = pxcmnLeaf;
            break;
        }

        default :
            #if CID_DEBUG_ON
            facCIDXML().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kXMLErrs::errcCM_BadCMTypeHere
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::Internal
                , TInteger(tCIDLib::i4EnumOrd(eType))
            );
            #endif
            break;
    }
    return c4LocalIndex;
}


tCIDLib::TCard4* TXMLChildCM::pc4NewTransTableEntry() const
{
    //
    //  Allocate an column for the transition table and initialize all of
    //  the entries to c4MaxCard.
    //
    tCIDLib::TCard4* pc4Ret = new tCIDLib::TCard4[m_c4ElemListSize];
    TRawMem::SetMemBuf(pc4Ret, kCIDLib::c4MaxCard, m_c4ElemListSize);

    return pc4Ret;
}


//
//  This method is recursive. It walks the passed content spec node tree and
//  builds up a rewritten content model node tree. Its called initially with
//  the root of the content spec node tree.
//
TXMLCMNode* TXMLChildCM::pxcmnRewriteTree(const TXMLCMSpecNode* const pxcsnCur)
{
    TXMLCMNode* pxcmnRet = 0;

    // For speed and convenience, get a copy of the node type
    const tCIDXML::ECMNodeTypes eType = pxcsnCur->eNodeType();

    //
    //  According to the type of node, we create the correct corresponding
    //  rewritten node, recursing as required.
    //
    if (eType == tCIDXML::ECMNodeTypes::Leaf)
    {
        //
        //  Just create a corresponding leaf node. Give it the element
        //  id of our spec node and set its position as we bump up the
        //  count of leafs.
        //
        pxcmnRet = new TXMLCMLeaf(pxcsnCur->c4ElemId(), m_c4LeafCount++);
    }
     else
    {
        switch(eType)
        {
            case tCIDXML::ECMNodeTypes::Alternation :
            case tCIDXML::ECMNodeTypes::Sequence :
            {
                //
                //  We have to recurse, first on our left node and then on
                //  our right node, if there is a right node.
                //
                //  NOTE!! We have to build the child trees first, then set
                //  them on the new bin op node. This insures the correct
                //  order of evaluation, i.e. don't just recurse directly
                //  within the call to the new constructor.
                //
                TXMLCMNode* pxcmnLeft = pxcmnRewriteTree(pxcsnCur->pxcsnLeft());
                TXMLCMNode* pxcmnRight = pxcmnRewriteTree(pxcsnCur->pxcsnRight());
                pxcmnRet = new TXMLCMBinOp(eType, pxcmnLeft, pxcmnRight);
                break;
            }

            case tCIDXML::ECMNodeTypes::ZeroOrMore :
            {
                //
                //  Just create a corresponding unary op node, recursing on
                //  the left (only) child of the current spec node.
                //
                pxcmnRet = new TXMLCMUnaryOp
                (
                    tCIDXML::ECMNodeTypes::ZeroOrMore
                    , pxcmnRewriteTree(pxcsnCur->pxcsnLeft())
                );
                break;
            }

            case tCIDXML::ECMNodeTypes::OneOrMore :
            {
                //
                //  This is one that we rewrite. We change it to (foo,foo*).
                //  So we return a sequence bin op node with the rewritten left
                //  child of the current spec node on the left and a
                //  zero or more bunary node on the right (with the same
                //  rewritten content of the left child of the current spec
                //  node.)
                //
                TXMLCMNode* pxcmnLeft = pxcmnRewriteTree(pxcsnCur->pxcsnLeft());
                TXMLCMNode* pxcmnLeft2 = pxcmnRewriteTree(pxcsnCur->pxcsnLeft());
                pxcmnRet = new TXMLCMBinOp
                (
                    tCIDXML::ECMNodeTypes::Sequence
                    , pxcmnLeft
                    , new TXMLCMUnaryOp(tCIDXML::ECMNodeTypes::ZeroOrMore, pxcmnLeft2)
                );
                break;
            }

            case tCIDXML::ECMNodeTypes::ZeroOrOne :
            {
                //
                //  This is one that we rewrite. We change it to (foo|epsilon).
                //  So we return an alternation bin op node with the rewritten
                //  left child of the current spec node on the left, and an
                //  epsilon leaf on the right.
                //
                TXMLCMNode* pxcmnLeft = pxcmnRewriteTree(pxcsnCur->pxcsnLeft());
                pxcmnRet = new TXMLCMBinOp
                (
                    tCIDXML::ECMNodeTypes::Alternation
                    , pxcmnLeft
                    , new TXMLCMLeaf(CIDXML_ChildCM::c4EpsilonId)
                );
                break;
            }

            default :
                #if CID_DEBUG_ON
                facCIDXML().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kXMLErrs::errcCM_UnknownCMType
                    , tCIDLib::ESeverities::ProcFatal
                    , tCIDLib::EErrClasses::Internal
                    , TInteger(tCIDLib::i4EnumOrd(eType))
                );
                #endif
                break;
        }
    }

    return pxcmnRet;
}
