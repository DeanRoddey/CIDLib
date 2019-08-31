//
// FILE NAME: CIDBuild_Janitor.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/21/1998
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
//  This header implements a couple simple template classes that provide
//  janitor services for heap allocated data.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

template <class T> class TJanitor
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TJanitor() = delete;        

        TJanitor(T* const pToDelete) :

            m_pToDelete(pToDelete)
        {
        }

        TJanitor(const TJanitor&) = delete;

        ~TJanitor()
        {
            delete m_pToDelete;
        }

        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TJanitor& operator=(const TJanitor&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pToDelete
        //      A pointer to the thing to delete
        // -------------------------------------------------------------------
        T*  m_pToDelete;
};


template <class T> class TArrayJanitor
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TArrayJanitor() = delete;

        TArrayJanitor(T* const pToDelete) :
            m_pToDelete(pToDelete)
        {
        }

        TArrayJanitor(const TArrayJanitor&) = delete;

        ~TArrayJanitor()
        {
            delete [] m_pToDelete;
        }

        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TArrayJanitor& operator=(const TArrayJanitor&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pToDelete
        //      A pointer to the thing to delete
        // -------------------------------------------------------------------
        T*  m_pToDelete;
};

