//
// FILE NAME: CIDAI_BehaviorTree.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/24/2017
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
//  This file implements the actual tree class, which loads and coordiantes all of the
//  stuff involved in invoking an behavior tree (and the sub-trees it references as
//  well.)
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDAI_.hpp"
#include    "CIDAI_BTParser_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TAIBehaviorTree, TObject)


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDAI_BehaviorTree
{
    // -----------------------------------------------------------------------
    //  The format version of our overall binary representation of a tree
    // -----------------------------------------------------------------------
    const tCIDLib::TCard1   c1FmtVersion = 1;


    // -----------------------------------------------------------------------
    //  The format version of the per-tree part of our binary representation
    // -----------------------------------------------------------------------
    const tCIDLib::TCard1   c1FmtSlotVersion = 1;


    // -----------------------------------------------------------------------
    //  Some marker bytes we put into the header section of the binary file format
    // -----------------------------------------------------------------------
    const tCIDLib::TCard1   c1Marker1 = 0xC9;
    const tCIDLib::TCard1   c1Marker2 = 0xD7;
}



// ---------------------------------------------------------------------------
//   CLASS: TAIBehaviorTree
//  PREFIX: btree
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TAIBehaviorTree: Constructors and Destructor
// ---------------------------------------------------------------------------
TAIBehaviorTree::TAIBehaviorTree(const TString& strName) :

    m_bStop(kCIDLib::False)
    , m_colCallStack(tCIDLib::EAdoptOpts::NoAdopt)
    , m_colTrees
      (
        tCIDLib::EAdoptOpts::Adopt
        , 109
        , new TStringKeyOps(kCIDLib::False)
        , TAIBTRootNode::strRootNodeKey
      )
    , m_strName(strName)
{
}

TAIBehaviorTree::~TAIBehaviorTree()
{
}


// ---------------------------------------------------------------------------
//  TAIBehaviorTree: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Get/set our stop flag
tCIDLib::TBoolean TAIBehaviorTree::bStop() const
{
    return m_bStop;
}

tCIDLib::TBoolean TAIBehaviorTree::bStop(const tCIDLib::TBoolean bToSet)
{
    m_bStop = bToSet;
    return m_bStop;
}


// Provide outside access to our context object in case it's needed
const TAIBTCtx& TAIBehaviorTree::ctxToUse() const
{
    return m_ctxToUse;
}

TAIBTCtx& TAIBehaviorTree::ctxToUse()
{
    return m_ctxToUse;
}


//
//  This method will 'compile' all of the contents of this tree to a binary file. It's
//  essentially the original XML files, but in a format that users cannot mess with it,
//  and which will be vastly easier to distibute and load (one file) and more efficient.
//
//  We have to put them out in order of dependence, so that when we load them, we can
//  resolve link references just as when the XML files were loaded. So we search
//  each tree to see what other trees are referenced. We process those first, in a
//  recursive way, basically a depth first search. We keep a hash set of the names of
//  the trees we've done so far, so we don't duplicate any.
//
tCIDLib::TVoid TAIBehaviorTree::CompileToFile(TBinOutStream& strmTar)
{
    // Write out our starting bits
    strmTar << tCIDLib::EStreamMarkers::StartObject
            << CIDAI_BehaviorTree::c1Marker1
            << CIDAI_BehaviorTree::c1Marker2
            << CIDAI_BehaviorTree::c1FmtVersion
            << m_colTrees.c4ElemCount()
            << tCIDLib::TCard4(m_colTrees.c4ElemCount() ^ kCIDLib::c4MaxCard);


    // Set up a cursor over our tree list
    tCIDAI::TTreeList::TCursor cursTrees(&m_colTrees);
    if (cursTrees.bReset())
    {
        // We need a string hash set to keep up with the trees we've done so far
        tCIDLib::TStrHashSet colTreesDone(109, new TStringKeyOps(kCIDLib::False));

        //
        //  Let's now start the process. At this level, we just loop through the trees.
        //  For each one, we call a recursive helper that processes each one and outputs
        //  any so far unprocessed trees.
        //
        for (; cursTrees; ++cursTrees)
        {
            const TAIBTRootNode& btnodeCur = *cursTrees;

            // If this guy is not in the trees done list, call the recursive helper
            if (!colTreesDone.bHasElement(btnodeCur.strName()))
                CompilerHelper(btnodeCur, colTreesDone, strmTar);

        }
    }

    // Close it off now
    strmTar << tCIDLib::EStreamMarkers::EndObject;
}


//
//  This is how we start the tree running on a specific tree in our list. The caller
//  can pass in initial variables and tree data to be put into the context for this
//  run.
//
tCIDAI::ENodeStates
TAIBehaviorTree::eRun(const TString& strTreeName, const tCIDLib::TKValsList& colInitVars)
{
    // Reset the context, setting any incoming initial variables
    m_ctxToUse.Reset(colInitVars);

    // Look for this tree, throwing if not found
    TAIBTRootNode* pbtnodeRun = pbtnodeFindTree(strTreeName, kCIDLib::True);

    // Clear the stop flag for this round
    m_bStop = kCIDLib::False;

    // And let's run it and return the result
    return pbtnodeRun->eRun(*this);
}

tCIDAI::ENodeStates
TAIBehaviorTree::eRun(  const   TString&                strTreeName
                        , const tCIDLib::TKValsList&    colInitVars
                        , const tCIDLib::TKNumList&     colKNumList)
{
    // Reset the context, setting any incoming initial variables and tree content
    m_ctxToUse.Reset(colInitVars, colKNumList);

    // Look for this tree, throwing if not found
    TAIBTRootNode* pbtnodeRun = pbtnodeFindTree(strTreeName, kCIDLib::True);

    // And let's run it and return the result
    return pbtnodeRun->eRun(*this);
}


//
//  We can load a tree from a file, or from a file preloaded into memory. Our file
//  based method just preloads the file and calls the other.
//
tCIDLib::TVoid
TAIBehaviorTree::LoadXMLTree(const TMemBuf& mbufData, const tCIDLib::TCard4 c4Bytes)
{
    try
    {
        // Create a parser and ask him to parse the passed file for us
        TCIDAIBTParser aibtpLoad;
        TAIBTRootNode* pbtnodeNew = aibtpLoad.pbtnodeParse(mbufData, c4Bytes);

        // It worked, so make sure the name is unique
        if (m_colTrees.bKeyExists(pbtnodeNew->strName()))
        {
            // Save the name before we trash the node
            const TString strSaveName = pbtnodeNew->strName();
            try
            {
                delete pbtnodeNew;
            }

            catch(TError& errToCatch)
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }

            facCIDAI().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kAIErrs::errcBTParse_DupTreeName
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Duplicate
                , strSaveName
            );
        }

        // Looks ok, so save it
        m_colTrees.Add(pbtnodeNew);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }}


tCIDLib::TVoid TAIBehaviorTree::LoadXMLTree(const TString& strToParse)
{
    // Try to open the file and read it in
    tCIDLib::TCard4 c4Bytes = 0;
    THeapBuf mbufFile;
    try
    {
        TBinaryFile bflSrc(strToParse);
        bflSrc.Open
        (
            tCIDLib::EAccessModes::Read
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::None
        );

        c4Bytes = tCIDLib::TCard4(bflSrc.c8CurSize());
        bflSrc.c4ReadBuffer(mbufFile, c4Bytes, tCIDLib::EAllData::FailIfNotAll);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    // And we can pass it to the other version
    LoadXMLTree(mbufFile, c4Bytes);
}


// Get the current top node, or null if none has been run yet
const TAIBTNode* TAIBehaviorTree::pbtnodeCur() const
{
    if (m_colCallStack.bIsEmpty())
        return nullptr;

    return m_colCallStack.pobjPeek();
}


// Look up a tree by its name. We can return null or throw if not found
TAIBTRootNode*
TAIBehaviorTree::pbtnodeFindTree(const  TString&            strTreeName
                                , const tCIDLib::TBoolean   bThrowIfNot)
{
    TAIBTRootNode* pbtnodeRet = m_colTrees.pobjFindByKey(strTreeName, kCIDLib::False);
    if (!pbtnodeRet)
    {
        if (bThrowIfNot)
        {
            facCIDAI().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kAIErrs::errcBT_TreeNotFound
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , strTreeName
            );
        }
        return nullptr;
    }
    return pbtnodeRet;
}


// Pop the top node from the call stack
tCIDLib::TVoid TAIBehaviorTree::PopNode()
{
    m_colCallStack.pobjPop();
}


// Push a new node onto the call stack
tCIDLib::TVoid TAIBehaviorTree::PushNode(TAIBTNode* const pbtnodePush)
{
    m_colCallStack.Push(pbtnodePush);
}


// Reset this tree back to inital state
tCIDLib::TVoid TAIBehaviorTree::Reset()
{
    m_colCallStack.RemoveAll();
    m_colTrees.RemoveAll();
    m_ctxToUse.Reset();
}


// Get the name that was set on this top level tree
const TString& TAIBehaviorTree::strName() const
{
    return m_strName;
}


//
//  Update the application info. This is useful for nested operations. We don't clear
//  the call stack or variables.
//
tCIDLib::TVoid
TAIBehaviorTree::SetAppInfo(const tCIDLib::TKNumList& colKNumList)
{
    m_ctxToUse.SetAppInfo(colKNumList);
}


// ---------------------------------------------------------------------------
//  TAIBehaviorTree: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This is a recursive helper. We scan the incoing tree for other trees that it
//  references, i.e. look for link nodes. For each one, we see if it is in the list
//  of already done trees. If not, we recurse on it.
//
//  For each one that we have not done, and which does not reference any other
//  tree, we write out its information and return.
//
tCIDLib::TVoid
TAIBehaviorTree::CompilerHelper(const   TAIBTRootNode&          btnodeTree
                                ,       tCIDLib::TStrHashSet&   colTreesDone
                                ,       TBinOutStream&          strmTar)
{
    // Let's scan this guy for link nodes
    const tCIDLib::TCard4 c4ChildCnt = btnodeTree.c4ChildCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ChildCnt; c4Index++)
    {
        const TAIBTNode* pbtnodeCur = btnodeTree.pbtnodeChildAt(c4Index);

        if (pbtnodeCur->strType() == kCIDAI::strDefFact_Link)
        {
            // Get the link target parameter
            const TString& strTar = pbtnodeCur->strFindParam(kCIDAI::strNParam_LinkTarget);

            // See if this guy is in the done list. If not, recurse
            if (!colTreesDone.bHasElement(strTar))
            {
                const TAIBTRootNode* pbtnodeRef = pbtnodeFindTree(strTar, kCIDLib::True);
                CompilerHelper(*pbtnodeRef, colTreesDone, strmTar);
            }
        }
    }

    // Our the incoming tree to the list of done trees
    colTreesDone.objAdd(btnodeTree.strName());

    // And let's do our guy now, first the overall tree info
    strmTar << tCIDLib::EStreamMarkers::Frame
            << CIDAI_BehaviorTree::c1FmtSlotVersion
            << btnodeTree.strName()
            << btnodeTree.c4ChildCount()
            << tCIDLib::TCard4(btnodeTree.c4ChildCount() ^ kCIDLib::c4MaxCard);

    // Now write out the children
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ChildCnt; c4Index++)
    {
        const TAIBTNode* pbtnodeCur = btnodeTree.pbtnodeChildAt(c4Index);

        // Do the basic node info
        const tCIDLib::TKVPList& colParams = pbtnodeCur->colParams();
        const tCIDLib::TCard4 c4PCnt = colParams.c4ElemCount();
        strmTar << tCIDLib::EStreamMarkers::Frame
                << pbtnodeCur->strName()
                << pbtnodeCur->strType()
                << pbtnodeCur->strPath()
                << c4PCnt;

        // If any parameters do those
        for (tCIDLib::TCard4 c4PInd = 0; c4PInd < c4PCnt; c4PInd++)
        {
            strmTar << colParams[c4PInd]
                    << tCIDLib::EStreamMarkers::Frame;
        }
    }

    // Cap off this tree
    strmTar << tCIDLib::EStreamMarkers::Frame;
}

