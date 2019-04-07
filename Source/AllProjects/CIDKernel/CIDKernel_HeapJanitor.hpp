//
// FILE NAME: CIDKernel_HeapJanitor.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/10/1996
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
//  This header implements two very simple, all inline janitors for dealing
//  with heap allocated objects or raw structures or arrays of objects or
//  fundamental types. All of this stuff is platform independent.
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
//   CLASS: TJanitor
//  PREFIX: jan
// ---------------------------------------------------------------------------
template <class T> class TJanitor
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TJanitor() :

            m_pobjToSanitize(nullptr)
        {
        }

        TJanitor(T* const pToDelete) :

            m_pobjToSanitize(pToDelete)
        {
        }

        TJanitor(const TJanitor&) = delete;

        ~TJanitor()
        {
            delete m_pobjToSanitize;
            m_pobjToSanitize = nullptr;
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TJanitor& operator=(const TJanitor&) = delete;
        tCIDLib::TVoid* operator new(const tCIDLib::TUInt) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Orphan()
        {
            m_pobjToSanitize = nullptr;
        }

        T* pobjOrphan()
        {
            T* pobjRet = m_pobjToSanitize;
            m_pobjToSanitize = nullptr;
            return pobjRet;
        }

        T* pobjThis()
        {
            return m_pobjToSanitize;
        }

        const T* pobjThis() const
        {
            return m_pobjToSanitize;
        }

        tCIDLib::TVoid Set(T* const pToSet)
        {
            //
            //  Delete the current and store the new. Make sure the new one gets stored,
            //  even if deleting the old one causes an exception.
            //
            T* pOld = m_pobjToSanitize;
            m_pobjToSanitize = pToSet;
            delete pOld;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pobjToSanitize
        //      This is the pointer to the object or structure that must be
        //      destroyed when this object is destroyed.
        // -------------------------------------------------------------------
        T*  m_pobjToSanitize;
};


// ---------------------------------------------------------------------------
//   CLASS: TArrayJanitor
//  PREFIX: jan
// ---------------------------------------------------------------------------
template <class T> class TArrayJanitor
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TArrayJanitor() :

            m_paToSanitize(nullptr)
        {
        }

        TArrayJanitor(T* const paToDelete) :

            m_paToSanitize(paToDelete)
        {
        }

        explicit TArrayJanitor(const tCIDLib::TCard4 c4Size) :

            m_paToSanitize(new T[c4Size])
        {
        }

        TArrayJanitor(const TArrayJanitor&) = delete;

        ~TArrayJanitor()
        {
            delete [] m_paToSanitize;
            m_paToSanitize = nullptr;
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TArrayJanitor& operator=(const TArrayJanitor&) = delete;
        tCIDLib::TVoid* operator new(const tCIDLib::TUInt) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Orphan()
        {
            m_paToSanitize = nullptr;
        }

        T* paOrphan()
        {
            T* paRet = m_paToSanitize;
            m_paToSanitize = nullptr;
            return paRet;
        }

        const T* paThis() const
        {
            return m_paToSanitize;
        }

        T* paThis()
        {
            return m_paToSanitize;
        }

         tCIDLib::TVoid Set(T* const pToSet)
        {
            //
            //  Delete the current and store the new. Make sure the new one gets stored,
            //  even if deleting the old one causes an exception.
            //
            T* paOld = m_paToSanitize;
            m_paToSanitize = pToSet;
            delete [] paOld;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_paToSanitize
        //      This is the pointer to the object or structure that must be
        //      destroyed when this object is destroyed.
        // -------------------------------------------------------------------
        T*  m_paToSanitize;
};

#pragma CIDLIB_POPPACK

