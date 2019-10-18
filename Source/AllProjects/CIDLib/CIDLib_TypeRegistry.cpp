//
// FILE NAME: CIDLib_TypeRegistry.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/21/1996
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
//  This file implements a couple of the static methods of the facility
//  class, TfacCIDLib(). These are the ones related to the dynamic type
//  registration system. This file maintains an internal hash table (which
//  is just a simple and raw implementation because lots of low level stuff
//  depend upon it.) It is used to store registered TClass objects and their
//  factory functions.
//
//  A simple class is used to store the class objects and factory function
//  in the hash table. It is internal only.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class   TRegNode;


namespace CIDLib_TypeRegistry
{
    // ---------------------------------------------------------------------------
    //  Local, static data
    //
    //  apnodeTable
    //      This is the hash table for type names. It is an array of pointers
    //      to nodes. Each pointer is a slot in the hash table and all of the
    //      entries in that slot's linked list has the same hash value. The
    //      hash table's size is driven by the modulus value used by the TClass
    //      class' internally.
    // ---------------------------------------------------------------------------
    TRegNode*   apnodeTable[kCIDLib::c4ClassModulus];
}


// ---------------------------------------------------------------------------
//   CLASS: TRegNode
//  PREFIX: node
// ---------------------------------------------------------------------------
class TRegNode
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TRegNode() = delete;
        TRegNode
        (
            const   tCIDLib::TCh* const     pszToReg
            ,       tCIDLib::TObjFactory    pFactoryFunc
            ,       TRegNode* const         pNext = nullptr
        );

        TRegNode(const TRegNode&) = delete;

        ~TRegNode() {}


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TRegNode& operator=(const TRegNode&) = delete;


        // -------------------------------------------------------------------
        //  Public data member
        //
        //  pszClassName
        //      This is the registered class name.
        //
        //  hshClass
        //      This is a fast access pointer to the internal hash of the
        //      class. This is gotten from the class object itself.
        //
        //  pFactory
        //      This is the factory function for this class. It will create
        //      a new object of this type. Its set from the one passed to
        //      the constructor.
        //
        //  pnodeNext
        //      This is a pointer to the next node in this slot. 0 if the
        //      end.
        // -------------------------------------------------------------------
        tCIDLib::TCh*           pszClassName;
        tCIDLib::THashVal       hshClass;
        tCIDLib::TObjFactory    pFactory;
        TRegNode*               pnodeNext;
};



TRegNode::TRegNode( const   tCIDLib::TCh* const     pszToReg
                    ,       tCIDLib::TObjFactory    pFactoryFunc
                    ,       TRegNode* const         pNext) :
    hshClass(0)
    , pFactory(pFactoryFunc)
    , pnodeNext(pNext)
    , pszClassName(nullptr)
{
    // Replicate the passed name and calc and store its hash
    pszClassName = TRawStr::pszReplicate(pszToReg);
    hshClass = TRawStr::hshHashStr(pszClassName, kCIDLib::c4ClassModulus);

    if (!hshClass)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_NotValidUnicode
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }
}


// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------

//
//  This method handles the lazy init of the critical section used to
//  synchronize access to the registry
//
static TCriticalSection* pcrsLock()
{
    static  TCriticalSection crsTRLock;
    return &crsTRLock;
}


//
//  This method will find the node that represents the passed name. If not
//  found it will log an exception, giving the passed method name and line
//  number.
//
static TRegNode* pnodeFindByName(const  tCIDLib::TCh* const pszClassName
                                ,       tCIDLib::TCard4     c4Line
                                , const tCIDLib::TBoolean   bFailIfNotFound)
{
    //
    //  Lets find the type in the hash table. So we do a hash on the
    //  string, using the modulus of the TCIDLib class.
    //
    tCIDLib::THashVal hshFind = TRawStr::hshHashStr
    (
        pszClassName
        , kCIDLib::c4ClassModulus
    );

    if (!hshFind)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_NotValidUnicode
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    //
    //  If there is no list head at this slot, then of couse it does
    //  not exist.
    //
    if (CIDLib_TypeRegistry::apnodeTable[hshFind])
    {
        TRegNode* pnodeCur = CIDLib_TypeRegistry::apnodeTable[hshFind];
        while (pnodeCur)
        {
            if (pnodeCur->hshClass == hshFind)
            {
                if (TRawStr::bCompareStr(pszClassName, pnodeCur->pszClassName))
                    return pnodeCur;
            }
            pnodeCur = pnodeCur->pnodeNext;
        }
    }

    // We did not find it, so log an error if the caller said to
    if (bFailIfNotFound)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , c4Line
            , kCIDErrs::errcTReg_NotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TString(pszClassName)
        );
    }

    // Not found, so return a null
    return nullptr;
}



// ---------------------------------------------------------------------------
//  TClass: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TClass::bIsClassRegistered(const tCIDLib::TCh* const pszClassName)
{
    TCritSecLocker crslAccess(pcrsLock());

    // Find the node for the passed type name
    if (pnodeFindByName(pszClassName, CID_LINE, kCIDLib::False) != nullptr)
        return kCIDLib::True;
    return kCIDLib::False;
}

tCIDLib::TBoolean TClass::bIsClassRegistered(const TString& strClassName)
{
    TCritSecLocker crslAccess(pcrsLock());

    // Find the node for the passed type name
    if (pnodeFindByName(strClassName.pszBuffer(), CID_LINE, kCIDLib::False) != nullptr)
        return kCIDLib::True;
    return kCIDLib::False;
}


TClass TClass::clsForClass(const tCIDLib::TCh* const pszClassName)
{
    TCritSecLocker crslAccess(pcrsLock());

    // Find the node for the passed type name
    const TRegNode* pnodeNew = pnodeFindByName
    (
        pszClassName, CID_LINE, kCIDLib::True
    );
    return TClass(pnodeNew->pszClassName);
}

TClass TClass::clsForClass(const TString& strClassName)
{
    TCritSecLocker crslAccess(pcrsLock());

    // Find the node for the passed type name
    const TRegNode* pnodeNew = pnodeFindByName
    (
        strClassName.pszBuffer(), CID_LINE, kCIDLib::True
    );
    return TClass(pnodeNew->pszClassName);
}


TObject* TClass::pobjMakeNewOfClass(const tCIDLib::TCh* const pszClassName)
{
    TCritSecLocker crslAccess(pcrsLock());

    // Find the node for the passed type name
    const TRegNode* pnodeNew = pnodeFindByName
    (
        pszClassName, CID_LINE, kCIDLib::True
    );
    return pnodeNew->pFactory();
}

TObject* TClass::pobjMakeNewOfClass(const TString& strClassName)
{
    TCritSecLocker crslAccess(pcrsLock());

    // Find the node for the passed type name
    const TRegNode* pnodeNew = pnodeFindByName
    (
        strClassName.pszBuffer(), CID_LINE, kCIDLib::True
    );
    return pnodeNew->pFactory();
}


tCIDLib::TVoid
TClass::RegisterClass(  const   tCIDLib::TCh* const     pszClassName
                        ,       tCIDLib::TObjFactory    pFactoryFunc)
{
    // Make sure that the class name is not empty or null.
    if (!pszClassName || !TRawStr::c4StrLen(pszClassName))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTReg_InvalidName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }

    // Get the hash of the passed class name
    tCIDLib::THashVal hshOfClass = TRawStr::hshHashStr
    (
        pszClassName, kCIDLib::c4ClassModulus
    );

    if (!hshOfClass)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_NotValidUnicode
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    // Should not happen but check during development
    #if CID_DEBUG_ON
    if (hshOfClass >= kCIDLib::c4ClassModulus)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTReg_InvalidHash
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::AppError
            , TCardinal(hshOfClass)
        );
    }
    #endif

    // Lock the reigstry now that we know the stuff is good
    TCritSecLocker crslAccess(pcrsLock());

    //
    //  Add this new class to the registry. First we use the hash value to
    //  find the slot in the hash table. If the slot is open, then this is
    //  the new head of that slot. Else insert at the start of the list.
    //
    TRegNode* pHead = CIDLib_TypeRegistry::apnodeTable[hshOfClass];
    if (!pHead)
    {
        CIDLib_TypeRegistry::apnodeTable[hshOfClass] = new TRegNode
        (
            pszClassName, pFactoryFunc
        );
    }
     else
    {
        CIDLib_TypeRegistry::apnodeTable[hshOfClass] = new TRegNode
        (
            pszClassName, pFactoryFunc, pHead
        );
    }
}

tCIDLib::TVoid
TClass::RegisterClass(  const   TString&                strClassName
                        ,       tCIDLib::TObjFactory    pFactoryFunc)
{
    RegisterClass(strClassName.pszBuffer(), pFactoryFunc);
}

