//
// FILE NAME: TestCIDLib2_LinkedList.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 5/8/2021
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2021
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file contains tests related to the fundamental double and single
//  linked list.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Include underlying headers
// ---------------------------------------------------------------------------
#include    "TestCIDLib2.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_SLinkedList, TTestFWTest)

// We need a derivaive of the single linked list node to hold a test value
class TSLLTestNode : public TSLstNode
{
    public :
        TSLLTestNode() = delete;
        TSLLTestNode(const tCIDLib::TCard4 c4Index) :

            m_c4Index(c4Index)
        {
        }
        tCIDLib::TCard4 m_c4Index;
};



// ---------------------------------------------------------------------------
//  CLASS: TTest_SLinkedList
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_SLinkedList: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_SLinkedList::TTest_SLinkedList() :

    TTestFWTest
    (
        L"SLinkedList", L"Basic Single Linked List", 2
    )
{
}

TTest_SLinkedList::~TTest_SLinkedList()
{
}


// ---------------------------------------------------------------------------
//  TTest_SLinkedList: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_SLinkedList::eRunTest(TTextStringOutStream&   strmOut
                            , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    // Create a linked list and load up a list of values
    TSLinkedList llstTest;
    if (llstTest.c4ElemCount() != 0)
    {
        strmOut << TFWCurLn << L"Linked list has wrong initial element count\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    for (tCIDLib::TCard4 c4Index = 1; c4Index <= 50; c4Index++)
        llstTest.AppendNode(new TSLLTestNode(c4Index));

    if (llstTest.c4ElemCount() != 50)
    {
        strmOut << TFWCurLn << L"Linked list has wrong element count\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // Iterate it and check the order
    tCIDLib::TCard4 c4TestInd = 1;
    TSLstNode* pnodeCur = llstTest.pnodeHead();
    while (pnodeCur)
    {
        TSLLTestNode* pnodeTest = static_cast<TSLLTestNode*>(pnodeCur);
        if (pnodeTest->m_c4Index != c4TestInd)
        {
            strmOut << TFWCurLn << L"Linked list element order was wrong\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }
        c4TestInd++;
        pnodeCur = pnodeCur->pnodeNext();
    }

    //
    //  Reverse the list and check the order again. Remember the head and tail nodes
    //  and check that they are opposite afterwards.
    //
    TSLstNode* pnodeOldHead = llstTest.pnodeHead();
    TSLstNode* pnodeOldTail = llstTest.pnodeTail();
    llstTest.Reverse();
    if ((pnodeOldHead != llstTest.pnodeTail()) || (pnodeOldTail != llstTest.pnodeHead()))
    {
        strmOut << TFWCurLn << L"Linked list reverse did nto swap head/tail nodes\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    // Element count should be the same
    if (llstTest.c4ElemCount() != 50)
    {
        strmOut << TFWCurLn << L"Linked list reverse changed element count\n\n";
        eRes = tTestFWLib::ETestRes::Failed;
    }

    c4TestInd = 50;
    pnodeCur = llstTest.pnodeHead();
    while (pnodeCur)
    {
        TSLLTestNode* pnodeTest = static_cast<TSLLTestNode*>(pnodeCur);
        if (pnodeTest->m_c4Index != c4TestInd)
        {
            strmOut << TFWCurLn << L"Linked list element order was wrong after reversal\n\n";
            eRes = tTestFWLib::ETestRes::Failed;
        }
        c4TestInd--;
        pnodeCur = pnodeCur->pnodeNext();
    }


    return eRes;
}