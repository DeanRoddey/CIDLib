//
// FILE NAME: CIDAI_BTNode.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/30/2016
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
//  This file implements the base behavior tree node class.
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


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TAIBTNode, TObject)



// ---------------------------------------------------------------------------
//   CLASS: TAIBTErr
//  PREFIX: bterr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TAIBTErr: Constructors and Destructor
// ---------------------------------------------------------------------------
TAIBTErr::TAIBTErr()
{
}

TAIBTErr::TAIBTErr( const   TString&    strNodePath
                    , const TString&    strNodeType
                    , const TString&    strErr) :

    m_strErr(strErr)
    , m_strNodePath(strNodePath)
    , m_strNodeType(strNodeType)
{
}

TAIBTErr::~TAIBTErr()
{
}




// ---------------------------------------------------------------------------
//   CLASS: TAIBTNode
//  PREFIX: btnode
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TAIBTNode: Public, static methods
// ---------------------------------------------------------------------------

// To support keyed collections
const TString& TAIBTNode::strKey(const TAIBTNode& btnodeSrc)
{
    return btnodeSrc.m_strName;
}


// ---------------------------------------------------------------------------
//  TAIBTNode: Constructors and Destructor
// ---------------------------------------------------------------------------
TAIBTNode::~TAIBTNode()
{
}


// ---------------------------------------------------------------------------
//  TAIBTNode: Public, virtual methods
// ---------------------------------------------------------------------------

//
//  At this level we do soem basic checking. IF that works we recurse on our children.
//  THe derived class can override if he needs to do some of his own, but he has to
//  call us as well.
//
tCIDLib::TVoid
TAIBTNode::Validate(TAIBehaviorTree& btreeOwner, TVector<TAIBTErr>& colToFill)
{
    // Make sure we have the appropriate number of children
    const tCIDLib::TCard4 c4Count = m_colChildren.c4ElemCount();
    if ((c4Count < m_c4MinChildCnt) || (c4Count > m_c4MaxChildCnt))
    {
        TAIBTErr bterrNew
        (
            m_strPath
            , m_strNodeType
            , facCIDAI().strMsg
            (
                kAIErrs::errcBT_ChildCount
                , TCardinal(m_c4MinChildCnt)
                , TCardinal(m_c4MaxChildCnt)
                , TCardinal(c4Count)
            )
        );
        colToFill.objAdd(bterrNew);
    }

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        m_colChildren[c4Index]->Validate(btreeOwner, colToFill);
}


// ---------------------------------------------------------------------------
//  TAIBTNode: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Get/set the application flag
tCIDLib::TBoolean TAIBTNode::bFlag() const
{
    return m_bFlag;
}

tCIDLib::TBoolean TAIBTNode::bFlag(const tCIDLib::TBoolean bToSet)
{
    m_bFlag = bToSet;
    return m_bFlag;
}


//
//  Get the count of children. For derived classes that don't have a fixed count, they
//  need to know this.
//
tCIDLib::TCard4 TAIBTNode::c4ChildCount() const
{
    return m_colChildren.c4ElemCount();
}


// Allow the derived class to see his parameters and the compiler needs it
const tCIDLib::TKVPList& TAIBTNode::colParams() const
{
    return m_colParams;
}


//
//  This is now new nodes get created. The type is in the form /fact/type, so we
//  break it up, use the first part to find the factory, and then pass it the type
//  to have it make one of that type. We add the new node as a child of ours.
//
TAIBTNode*
TAIBTNode::pbtnodeAddChild( const   TString&            strName
                            , const TString&            strType
                            , const tCIDLib::TBoolean   bFlag)
{
    // Break the type into factory type and node type
    TStringTokenizer stokTmp(&strType, L"/");
    TString strFactType;
    TString strNodeType;

    if (!stokTmp.bGetNextToken(strFactType)
    ||  !stokTmp.bGetNextToken(strNodeType)
    ||  stokTmp.bMoreTokens())
    {
        facCIDAI().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kAIErrs::errcBT_NodeTypeFmt
            , strType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    // Look up the factory
    TAIBTNodeFact& nfactTar = facCIDAI().nfactFind(strFactType);

    //
    //  Create the new path for this guy. It's our path, plus what is going to be the
    //  child index of the new node. We have to deal with the special case where the
    //  root node is just /, so it already has a trailing slash.
    //
    TString strPath(m_strPath);
    if (strPath.chLast() != kCIDLib::chForwardSlash)
        strPath.Append(kCIDLib::chForwardSlash);
    strPath.AppendFormatted(m_colChildren.c4ElemCount());

    // Ask the factory to create the node
    TAIBTNode* pbtnodeNew = nfactTar.pbtnodeMakeNew
    (
        strPath, strName, strNodeType, bFlag
    );
    if (!pbtnodeNew)
    {
        facCIDAI().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kAIErrs::errcBT_NodeTypeNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strNodeType
            , strFactType
        );
    }

    // Add it to our child list
    m_colChildren.Add(pbtnodeNew);

    //
    //  And return the new node as well, since the caller will likely need to do things
    //  with it as well.
    //
    return pbtnodeNew;
}


// Provide read only access to our children by index
const TAIBTNode* TAIBTNode::pbtnodeChildAt(const tCIDLib::TCard4 c4At) const
{
    return m_colChildren[c4At];
}


//
//  Provide read only access to our children by the type. This is only practical where
//  each child has a unique type, since we'll only return the first one we find.
//
const TAIBTNode*
TAIBTNode::pbtnodeChildByType(const TString& strType) const
{
    const tCIDLib::TCard4 c4Count = m_colChildren.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TAIBTNode* pbtnodeCur = m_colChildren[c4Index];
        if (pbtnodeCur->strType().bCompareI(strType))
            return pbtnodeCur;
    }
    return nullptr;
}


//
//  A helper for the derived classes to look up their parameters, which were stored
//  on us by the node factory after we were created. It's public because the compiler
//  needs it as well.
//
const TString& TAIBTNode::strFindParam(const TString& strToFind) const
{
    const tCIDLib::TCard4 c4Count = m_colParams.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TKeyValuePair& kvalCur = m_colParams[c4Index];
        if (strToFind.bCompareI(kvalCur.strKey()))
            return kvalCur.strValue();
    }

    facCIDAI().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kAIErrs::errcBT_ParamNotFound
        , m_strPath
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotFound
        , strToFind
        , m_strName
    );

    // Make the compiler happy
    return TString::strEmpty();
}


// Get the node name
const TString& TAIBTNode::strName() const
{
    return m_strName;
}


// Get the node type
const TString& TAIBTNode::strType() const
{
    return m_strNodeType;
}


// Get the node path
const TString& TAIBTNode::strPath() const
{
    return m_strPath;
}




// ---------------------------------------------------------------------------
//  TAIBTNode: Hidden constructors
// ---------------------------------------------------------------------------

//
//  If this node takes no children, we just set the child collection to a max size
//  of 1, since it can't be zero. Else we set it to the max child count. We set the
//  init size to the min child count, which can be zero.
//
TAIBTNode::TAIBTNode(const  TString&            strPath
                    , const TString&            strName
                    , const TString&            strNodeType
                    , const tCIDLib::TCard4     c4MinChildCnt
                    , const tCIDLib::TCard4     c4MaxChildCnt) :

    m_bFlag(kCIDLib::False)
    , m_c4MaxChildCnt(c4MaxChildCnt)
    , m_c4MinChildCnt(c4MinChildCnt)
    , m_colChildren(tCIDLib::EAdoptOpts::Adopt, c4MinChildCnt)
    , m_strPath(strPath)
    , m_strName(strName)
    , m_strNodeType(strNodeType)
{
    //
    //  There is no known scenario were we can have zero min and non-zero max, i.e. a
    //  node either has no children at all, or it has as least one. There are no optional
    //  children nodes.
    //
    if (m_c4MaxChildCnt && !m_c4MinChildCnt)
    {
        facCIDAI().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kAIErrs::errcBT_NoOptChildren
            , m_strPath
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }
}


// ---------------------------------------------------------------------------
//  TAIBTNode: Protected, non-virtual
// ---------------------------------------------------------------------------

// Tell the caller if a child exists with the given name. If so, return the index
tCIDLib::TBoolean
TAIBTNode::bChildExists(const TString& strName, tCIDLib::TCard4& c4Index) const
{
    const tCIDLib::TCard4 c4Count = m_colChildren.c4ElemCount();
    for (c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colChildren[c4Index]->strName().bCompareI(strName))
            return kCIDLib::True;
    }
    return kCIDLib::False;
}



//
//  The derived class needs to run the children in the correct order for its needs, which
//  it can do by calling this to invoke a child at a given index. We call the other helper
//  that will do the actual work, passing it the target child node.
//
tCIDAI::ENodeStates
TAIBTNode::eRunChildAt(TAIBehaviorTree& btreeOwner, const tCIDLib::TCard4 c4At)
{
    CheckChildIndex(c4At);
    return eRunNode(btreeOwner, *m_colChildren[c4At]);
}


//
//  Sometimes the derived class wants to run a child node by its name, so we provide a
//  a helper for that. We will just run the first one with that name that we find, so
//  if they have more than one with the same name only the first will ever get run by
//  this method.
//
tCIDAI::ENodeStates
TAIBTNode::eRunChildByName(TAIBehaviorTree& btreeOwner, const TString& strToRun)
{
    const tCIDLib::TCard4 c4Count = m_colChildren.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colChildren[c4Index]->strName().bCompareI(strToRun))
            return eRunNode(btreeOwner, *m_colChildren[c4Index]);
    }

    // We never found it
    facCIDAI().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kAIErrs::errcBT_ChildName
        , strPath()
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotFound
        , strToRun
    );

    // Make the compiler happy
    return tCIDAI::ENodeStates::Failure;
}


//
//  Sometimes the derived class wants to run a child node by its type, so we provide a
//  a helper for that. We will just run the first one with that type that we find, so
//  if they have more than one with the same type only the first will ever get run by
//  this method.
//
tCIDAI::ENodeStates
TAIBTNode::eRunChildByType(TAIBehaviorTree& btreeOwner, const TString& strToRun)
{
    const tCIDLib::TCard4 c4Count = m_colChildren.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colChildren[c4Index]->strType().bCompareI(strToRun))
            return eRunNode(btreeOwner, *m_colChildren[c4Index]);
    }

    // We never found it
    facCIDAI().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kAIErrs::errcBT_ChildType
        , strPath()
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotFound
        , strToRun
    );

    // Make the compiler happy
    return tCIDAI::ENodeStates::Failure;
}


//
//  This one exists mainly because of the requirements of the link node class. It links
//  to another tree, which means it has to invoke a node that is not a child of itself.
//  Since we have to provide this, we also have eRunChildAt() above just work in terms
//  of this.
//
tCIDAI::ENodeStates
TAIBTNode::eRunNode(TAIBehaviorTree& btreeOwner, TAIBTNode& btnodeTar)
{
    // Assume failure unless proven otherwise
    tCIDAI::ENodeStates eRet = tCIDAI::ENodeStates::Failure;
    try
    {
        btreeOwner.PushNode(&btnodeTar);
        eRet = btnodeTar.eRun(btreeOwner);
        btreeOwner.PopNode();
    }

    catch(TError& errToCatch)
    {
        btreeOwner.PopNode();

        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);

        facCIDAI().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kAIErrs::errcBT_RunExcept
            , btnodeTar.strPath()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppStatus
        );
    }
    return eRet;
}


//
//  Allows the base node factory class to set the parameters on a node after it is
//  created.
//
tCIDLib::TVoid TAIBTNode::SetParams(const tCIDLib::TKVPList& colToSet)
{
    m_colParams = colToSet;
}



// ---------------------------------------------------------------------------
//  TAIBTNode: Private, non-virtual
// ---------------------------------------------------------------------------

// A helper to check whether the passed index is a legal child node index for this node
tCIDLib::TVoid TAIBTNode::CheckChildIndex(const tCIDLib::TCard4 c4ToCheck) const
{
    if (c4ToCheck >= m_colChildren.c4ElemCount())
    {
        facCIDAI().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kAIErrs::errcBT_ChildIndex
            , m_strPath
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4ToCheck)
            , TCardinal(m_colChildren.c4ElemCount())
        );
    }
}

