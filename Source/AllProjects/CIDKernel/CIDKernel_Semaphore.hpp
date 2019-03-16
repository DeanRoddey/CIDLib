//
// FILE NAME: CIDKernel_Semaphore.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/01/1997
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
//  This is the header for the CIDKernel_Semaphore.Cpp file, which implements
//  the TKrnlSemaphore class. This class is the kernel wrapper for the host
//  counting semaphore service.
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
//   CLASS: TKrnlSemaphore
//  PREFIX: ksem
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlSemaphore
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlSemaphore() = delete;

        TKrnlSemaphore
        (
            const   tCIDLib::TCard4         c4MaxCount
        );

        TKrnlSemaphore
        (
            const   tCIDLib::TCh* const     pszName
            , const tCIDLib::TCard4         c4MaxCount
        );

        TKrnlSemaphore(const TKrnlSemaphore&) = delete;

        ~TKrnlSemaphore();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlSemaphore& operator=(const TKrnlSemaphore&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bClose();

        tCIDLib::TBoolean bCreate
        (
            const   tCIDLib::TCard4         c4InitCount = 0
        );

        tCIDLib::TBoolean bDuplicate
        (
            const   TKrnlSemaphore&         ksemToDup
        );

        tCIDLib::TBoolean bEnter
        (
            const   tCIDLib::TCard4         c4MilliSecs = kCIDLib::c4MaxWait
        );

        tCIDLib::TBoolean bExit();

        tCIDLib::TBoolean bIsValid() const;

        tCIDLib::TBoolean bNamed() const;

        tCIDLib::TBoolean bOpen();

        tCIDLib::TBoolean bOpenOrCreate
        (
                    tCIDLib::TBoolean&      bCreated
            , const tCIDLib::TCard4         c4InitCount = 0
        );

        tCIDLib::TBoolean bSetName
        (
            const   tCIDLib::TCh* const     pszNewName
        );

        tCIDLib::TCard4 c4MaxCount() const;

        const tCIDLib::TCh* pszName() const;



    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreateNamed
        (
            const   tCIDLib::TCard4         c4InitCount
            , const tCIDLib::TBoolean       bFailIfExists
            ,       tCIDLib::TBoolean&      bCreated
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4MaxCount
        //      This is the maximum threads that can be in the semaphore at
        //      once.
        //
        //  m_hsemThis
        //      This is the handle to the semaphore.
        //
        //  m_pszName
        //      The name of the semaphore.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4MaxCount;
        TSemaphoreHandle    m_hsemThis;
        tCIDLib::TCh*       m_pszName;

};

#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  TKrnlSemaphore: Public, non-virtual methods
// ---------------------------------------------------------------------------
inline tCIDLib::TBoolean TKrnlSemaphore::bIsValid() const
{
    return (m_hsemThis.bIsValid());
}

inline tCIDLib::TBoolean TKrnlSemaphore::bNamed() const
{
    return (m_pszName != 0);
}

inline tCIDLib::TCard4 TKrnlSemaphore::c4MaxCount() const
{
    return m_c4MaxCount;
}

inline const tCIDLib::TCh* TKrnlSemaphore::pszName() const
{
    return m_pszName;
}

