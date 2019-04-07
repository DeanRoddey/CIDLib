//
// FILE NAME: CIDOrb_ObjList_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/11/2000
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
//  This is the header for the CIDOrb_ObjList.cpp file, which implements the
//  TOrbSObjList class. This class provides a specialized collection class
//  for storing server objects. None of the standard collection classes is
//  quite right for what we want to do, and we need maximum performance in
//  this case.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TOrbSObjList
//  PREFIX: sobjl
// ---------------------------------------------------------------------------
class CIDORBEXP TOrbSObjList : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TOrbSObjList();

        TOrbSObjList(const TOrbSObjList&) = delete;

        ~TOrbSObjList();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TOrbSObjList& operator=(const TOrbSObjList&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Add
        (
                    TOrbServerBase* const   porbsToAdd
        );

        tCIDLib::TBoolean bNext();

        tCIDLib::TBoolean bResetIter();

        tCIDLib::TBoolean bRemove
        (
                    TOrbServerBase* const   porbsToRemove
        );

        tCIDLib::TCard4 c4ElemCount() const;

        const TOrbServerBase& objCur() const;

        TOrbServerBase& objCur();

        TOrbServerBase* porbsFind
        (
            const   TOrbId&                 oidToFind
        );

        const TOrbServerBase* porbsFind
        (
            const   TOrbId&                 oidToFind
        )   const;

        TOrbServerBase* porbsOrphan
        (
                    TOrbServerBase* const   porbsToOrphan
        );

        tCIDLib::TVoid RemoveAll();


    private :
        // -------------------------------------------------------------------
        //  Private class types
        // -------------------------------------------------------------------
        struct TBucketItem
        {
            TOrbServerBase*     porbsThis;
            TBucketItem*        pbiNext;
        };


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pabiList
        //      This is the list of pointers to bucket items, which provides
        //      the hash table.
        //
        //  m_c4ElemCount
        //      The current number of objects in the list.
        //
        //  m_c4BucketInd
        //  m_pbiIter
        //      These two members provide the iterator support for this object
        //      list collection. The bucket index is run up through the bucket
        //      list, and the pointer is run up through each bucket. When the
        //      bucket index is equal to the modulue, the iterator is invalid.
        // -------------------------------------------------------------------
        TBucketItem**   m_pabiList;
        tCIDLib::TCard4 m_c4BucketInd;
        tCIDLib::TCard4 m_c4ElemCount;
        TBucketItem*    m_pbiIter;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TOrbSObjList,TObject)
};

#pragma CIDLIB_POPPACK

