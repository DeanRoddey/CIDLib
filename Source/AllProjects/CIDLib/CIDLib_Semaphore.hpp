//
// FILE NAME: CIDLib_Semphore.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/18/1997
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
//  This is the header for the CIDLib_Semphore.Cpp file. It implements the
//  TSemaphore class, which is a standard counting semaphore.
//
//  TSemJanitor is a janitor class for a semaphore. It makes it safe to enter
//  a semaphore, and insures that it gets exited in the face of exceptions.
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
//  CLASS: TSemaphore
// PREFIX: sem
// ---------------------------------------------------------------------------
class CIDLIBEXP TSemaphore : public TObject, public MFormattable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TSemaphore() = delete;

        TSemaphore
        (
            const   tCIDLib::TCard4         c4MaxCount
            , const tCIDLib::TCard4         c4InitCount
        );

        TSemaphore
        (
            const   TResourceName&          rsnToUse
            , const tCIDLib::TCard4         c4InitCount
            , const tCIDLib::TCard4         c4MaxCount
            , const tCIDLib::ECreateActs    eCreateAct = tCIDLib::ECreateActs::OpenOrCreate
        );

        TSemaphore
        (
            const   TResourceName&          rsnToUse
            ,       tCIDLib::TBoolean&      bCreated
            , const tCIDLib::TCard4         c4InitCount
            , const tCIDLib::TCard4         c4MaxCount
            , const tCIDLib::ECreateActs    eCreateAct = tCIDLib::ECreateActs::OpenOrCreate
        );

        TSemaphore(const TSemaphore&) = delete;

        ~TSemaphore();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSemaphore& operator=(const TSemaphore&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsNamed() const;

        tCIDLib::TCard4 c4MaxCount() const;

        tCIDLib::TVoid Enter
        (
            const   tCIDLib::TCard4         c4Timeout = kCIDLib::c4MaxWait
        );

        tCIDLib::TVoid Exit();

        const TResourceName& rsnName() const;


    protected   :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid InitNamed
        (
                    tCIDLib::TBoolean&      bCreated
            , const tCIDLib::TCard4         c4InitCount
            , const tCIDLib::ECreateActs    eAction
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bNamed
        //      Indicates whether this is a named or unnamed semaphore.
        //
        //  m_ksemImpl
        //      This is the kernel semphore object that provides the real
        //      implementation and platform abstraction.
        //
        //  m_rsnThis
        //      The name of the semaphore, in a resource name object.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bNamed;
        TKrnlSemaphore      m_ksemImpl;
        TResourceName       m_rsnThis;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSemaphore,TObject)
};



// ---------------------------------------------------------------------------
//  CLASS: TSemJanitor
// PREFIX: jan
// ---------------------------------------------------------------------------
class CIDLIBEXP TSemJanitor : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TSemJanitor() = delete;

        TSemJanitor
        (
                    TSemaphore* const       psemToEnter
            , const tCIDLib::TCard4         c4MilliSecs = kCIDLib::c4MaxWait
        );

        TSemJanitor(const TSemJanitor&) = delete;

        ~TSemJanitor();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSemJanitor& operator=(const TSemJanitor&) = delete;
        tCIDLib::TVoid* operator new(const tCIDLib::TUInt) = delete;


    private :
        // -------------------------------------------------------------------
        //  Private data
        //
        //  m_psemToEnter
        //      The semaphore to enter and exit. We keep a pointer to it but
        //      don't adopt it.
        // -------------------------------------------------------------------
        TSemaphore*     m_psemToEnter;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSemJanitor, TObject)
};


#pragma CIDLIB_POPPACK

