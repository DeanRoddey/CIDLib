//
// FILE NAME: CIDLib_BasicTreeCol.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/14/2001
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TBaseTreeNode,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TBaseTreeNode
//  PREFIX: node
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBaseTreeNode: Publick Destructor
// ---------------------------------------------------------------------------
TBaseTreeNode::~TBaseTreeNode()
{
    delete m_pstrDescription;
}


// ---------------------------------------------------------------------------
//  TBaseTreeNode: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TString& TBaseTreeNode::strDescription() const
{
    // Fault in if needed
    if (!m_pstrDescription)
        m_pstrDescription = new TString;
    return *m_pstrDescription;
}

const TString& TBaseTreeNode::strDescription(const TString& strNew)
{
    // Fault in if needed
    if (!m_pstrDescription)
        m_pstrDescription = new TString(strNew);
    else
        *m_pstrDescription = strNew;

    return *m_pstrDescription;
}


// ---------------------------------------------------------------------------
//  TBaseTreeNode: Hidden constructors
// ---------------------------------------------------------------------------
TBaseTreeNode::TBaseTreeNode(const  tCIDLib::ETreeNodes eType
                            , const TString&            strName
                            , const TString&            strDescription) :
    m_eType(eType)
    , m_strName(strName)
    , m_pstrDescription(0)
{
    // If its not the nul string, then store it
    if (!TString::bIsNullObject(strDescription))
        m_pstrDescription = new TString(strDescription);
}



// ---------------------------------------------------------------------------
//  Methods of the TBasicTreeHelpers namespace
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TBasicTreeHelpers::BadNode( const   tCIDLib::TCh* const pszFile
                            , const tCIDLib::TCard4     c4Line)
{
    facCIDLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCIDErrs::errcCol_BadNode
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::BadParms
    );
}


tCIDLib::TVoid
TBasicTreeHelpers::BadPath( const   tCIDLib::TCh* const pszFile
                            , const tCIDLib::TCard4     c4Line)
{
    facCIDLib().ThrowErr
    (
        pszFile
        , c4Line
        , kCIDErrs::errcCol_BadPath
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::BadParms
    );
}


tCIDLib::TVoid
TBasicTreeHelpers::CheckName(const  TString&            strToCheck
                            , const tCIDLib::TCh* const pszFile
                            , const tCIDLib::TCard4     c4Line)
{
    // Make sure the name doesn't have any separators in it
    tCIDLib::TCard4 c4Ofs;
    if (strToCheck.bFirstOccurrence(kCIDLib::chTreeSepChar, c4Ofs))
    {
        facCIDLib().ThrowErr
        (
            pszFile
            , c4Line
            , kCIDErrs::errcCol_BadNodeName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , strToCheck
        );
    }
}


tCIDLib::TVoid
TBasicTreeHelpers::CheckNodeType(const  TBaseTreeNode* const    pnodeToCheck
                                , const tCIDLib::ETreeNodes     eType
                                , const tCIDLib::TCh* const     pszFile
                                , const tCIDLib::TCard4         c4Line)
{
    if (pnodeToCheck->eType() != eType)
    {
        if (eType == tCIDLib::ETreeNodes::Terminal)
        {
            facCIDLib().ThrowErr
            (
                pszFile
                , c4Line
                , kCIDErrs::errcCol_MustBeTerminal
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::BadParms
                , pnodeToCheck->strName()
            );
        }
         else
        {
            facCIDLib().ThrowErr
            (
                pszFile
                , c4Line
                , kCIDErrs::errcCol_MustBeNonTerminal
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::BadParms
                , pnodeToCheck->strName()
            );
        }
    }
}


tCIDLib::TVoid
TBasicTreeHelpers::CheckPath(const  TString&            strToCheck
                            , const tCIDLib::TCh* const pszFile
                            , const tCIDLib::TCard4     c4Line)
{
    // Make sure it starts with a separator
    if (strToCheck.chFirst() != kCIDLib::chTreeSepChar)
    {
        facCIDLib().ThrowErr
        (
            pszFile
            , c4Line
            , kCIDErrs::errcCol_BadPath
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , strToCheck
        );
    }
}


tCIDLib::TVoid
TBasicTreeHelpers::CheckPathAndName(const   TString&            strPathToCheck
                                    , const TString&            strNameToCheck
                                    , const tCIDLib::TCh* const pszFile
                                    , const tCIDLib::TCard4     c4Line)
{
    // Just call the other two checkers
    CheckPath(strPathToCheck, pszFile, c4Line);
    CheckName(strNameToCheck, pszFile, c4Line);
}


tCIDLib::TVoid
TBasicTreeHelpers::DepthUnderflow(  const   tCIDLib::TCh* const pszFile
                                    , const tCIDLib::TCard4     c4Line)
{
    facCIDLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCIDErrs::errcCol_DepthUnderflow
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Internal
    );
}


tCIDLib::TVoid
TBasicTreeHelpers::MustBeNonTerminal(const  tCIDLib::TCh* const pszFile
                                    , const tCIDLib::TCard4     c4Line
                                    , const TString&            strName)
{
    facCIDLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCIDErrs::errcCol_MustBeNonTerminal
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::BadParms
        , strName
    );
}


tCIDLib::TVoid
TBasicTreeHelpers::NamedNodeNotFound(const  tCIDLib::TCh* const pszFile
                                    , const tCIDLib::TCard4     c4Line
                                    , const TString&            strName)
{
    facCIDLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCIDErrs::errcCol_NamedNodeNotFound
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotFound
        , strName
    );
}


tCIDLib::TVoid
TBasicTreeHelpers::NotMemberNode(const  tCIDLib::TCh* const pszFile
                                , const tCIDLib::TCard4     c4Line)
{
    facCIDLib().ThrowErr
    (
        pszFile
        , c4Line
        , kCIDErrs::errcCol_NotMemberNode
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotFound
    );
}


tCIDLib::TVoid
TBasicTreeHelpers::NotSupported(const   tCIDLib::TCh* const pszFile
                                , const tCIDLib::TCard4     c4Line
                                , const tCIDLib::TCh* const pszMethod)
{
    facCIDLib().ThrowErr
    (
        pszFile
        , c4Line
        , kCIDErrs::errcCol_NotSupported
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotSupported
        , TString(pszMethod)
        , TString(L"TBasicTreeCol")
    );
}


tCIDLib::TVoid
TBasicTreeHelpers::ThrowDup(const   tCIDLib::TCh* const pszFile
                            , const tCIDLib::TCard4     c4Line
                            , const TString&            strName)
{
    facCIDLib().ThrowErr
    (
        pszFile
        , c4Line
        , kCIDErrs::errcCol_DupTreeNode
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Duplicate
        , strName
    );
}

