//
// FILE NAME: CIDLib_CollectCursor.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/24/1995
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
//  This header defines the base collection cursor classes. Collection cursors
//  allow code to move around within a collection without much knowledge of
//  the particular type of collection being manipulated. And it allows multiple
//  pieces of code to have its own iterator over the collection, which will
//  not work if the collection itself provided an internal cursor, since it
//  could not be shared by multiple clients simultaneously.
//
//  Note that it is basically impossible to create a completely type safe
//  hierarchy of cursors over the possible axes of read/write and
//  forward/reverse iteration, because we can only represent one of them
//  in the class hierarchy. So we make bidirectionality a runtime check.
//  The bIsBiDirectional() method will allow generic code to check whether
//  the cursor is bidirectional or not. Attempst to call bPrevious() or
//  bSeekToEnd() will throw if the cursor is not bidirectional. This is
//  unfortunate, but the need to have polymorphic cursors overrides any such
//  concerns.
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
//   CLASS: TColCursor
//  PREFIX: curs
// ---------------------------------------------------------------------------
template <typename TElem> class TColCursor : public TCursorBase
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        ~TColCursor() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------

        // Just syntax convenience for iteration loops mostly
        explicit operator tCIDLib::TBoolean() const
        {
            return bIsValid();
        }

        tCIDLib::TBoolean operator!() const
        {
            return !bIsValid();
        }

        const TElem* operator->() const
        {
            return &objRCur();
        }

        const TElem& operator*() const
        {
            return objRCur();
        }

        tCIDLib::TBoolean operator==(const TColCursor& cursSrc) const
        {
            if (&cursSrc == this)
                return kCIDLib::True;
            return (m_pcolBaseCurs == cursSrc.m_pcolBaseCurs);
        }

        tCIDLib::TBoolean operator!=(const TColCursor& cursSrc) const
        {
            return !TColCursor::operator==(cursSrc);
        }

        TColCursor<TElem>& operator++()
        {
            this->bNext();
            return *this;
        }


        // -------------------------------------------------------------------
        //  Public, pure virtual methods
        // -------------------------------------------------------------------
        virtual const TElem& objRCur() const = 0;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsEmpty() const override
        {
            // If not set yet...
            if (!m_pcolBaseCurs)
                return kCIDLib::True;

            // Lock the collection
            TLocker lockrCol(m_pcolBaseCurs);
            return m_pcolBaseCurs->bIsEmpty();
        }

        tCIDLib::TBoolean bIsValid() const override
        {
            // If not pointed at a collection yet, then obviously not
            if (!m_pcolBaseCurs)
                return kCIDLib::False;

            // If our serial number doesn't match that of the collection, then not
            return (m_pcolBaseCurs->c4SerialNum() == c4SerialNum());
        }

        tCIDLib::TCard4 c4ElemCount() const override
        {
            this->CheckInitialized(CID_FILE, CID_LINE);

            // Lock the collection and check the serial number
            TLocker lockrCol(m_pcolBaseCurs);
            CheckSerialNum(m_pcolBaseCurs->c4SerialNum(), CID_FILE, CID_LINE);
            return m_pcolBaseCurs->c4ElemCount();
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsCursoring(const TCollectionBase& colToCheck) const
        {
            // Make sure its the same cursor that we are cursoring
            return (m_pcolBaseCurs == &colToCheck);
        }


    protected   :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TColCursor() :

            m_pcolBaseCurs(nullptr)
        {
        }

        TColCursor(const TCollectionBase* const pcolToCursor) :

            TCursorBase(pcolToCursor)
            , m_pcolBaseCurs(pcolToCursor)
        {
        }

        TColCursor(const TColCursor&) = default;
        TColCursor<TElem>& operator=(const TColCursor&) = default;

        TColCursor(TColCursor&& cursSrc) :

            m_pcolBaseCurs(nullptr)
        {
            *this = tCIDLib::ForceMove(cursSrc);
        }

        TColCursor<TElem>& operator=(TColCursor&& cursSrc)
        {
            if (&cursSrc != this)
            {
                TParent::operator=(tCIDLib::ForceMove(cursSrc));
                tCIDLib::Swap(m_pcolBaseCurs, cursSrc.m_pcolBaseCurs);
            }
            return *this;
        }


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid
        CheckInitialized(const  tCIDLib::TCh* const pszFile
                        , const tCIDLib::TCard4     c4Line) const
        {
            if (!m_pcolBaseCurs)
                ThrowNotInitialized(pszFile, c4Line);
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pcolBaseCurs
        //      This is our own, basic, view of the collection being cursored.
        //      The actual derived cursor class will have a type specific view.
        //      But this view gives us enough info to handle a bit of cursor
        //      functionality on behalf of all derivatives.
        // -------------------------------------------------------------------
        const TCollectionBase*  m_pcolBaseCurs;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TColCursor<TElem>,TCursorBase)
};


// ---------------------------------------------------------------------------
//   CLASS: TBiColCursor
//  PREFIX: curs
// ---------------------------------------------------------------------------
template <typename TElem> class TBiColCursor : public TColCursor<TElem>
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        ~TBiColCursor() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TColCursor<TElem>& operator--()
        {
            bPrevious();
            return *this;
        }


        // -------------------------------------------------------------------
        //  Public, pure virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bPrevious() = 0;

        virtual tCIDLib::TBoolean bSeekToEnd() = 0;


    protected   :
        // -------------------------------------------------------------------
        //  Hidden construtors and operators
        // -------------------------------------------------------------------
        TBiColCursor()
        {
        }

        TBiColCursor(const TCollectionBase* const pcolToCursor) :

            TColCursor<TElem>(pcolToCursor)
        {
        }

        TBiColCursor(const TBiColCursor&) = default;
        TBiColCursor(TBiColCursor&&) = default;
        TBiColCursor<TElem>& operator=(const TBiColCursor&) = default;
        TBiColCursor<TElem>& operator=(TBiColCursor&&) = default;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TBiColCursor<TElem>,TColCursor<TElem>)
};


#pragma CIDLIB_POPPACK


