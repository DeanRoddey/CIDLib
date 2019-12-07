//
// FILE NAME: CIDKernel_InternalHelpers_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/04/1998
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
//  This is the header for the CIDKernel_Internal.Cpp module. This module
//  provides some internal Win32 helper methods that are not part of any
//  externally provided services but are used in more than one module
//  internally. These methods are wrapped in the TKrnlWin32 namespace.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


namespace TKrnlWin32
{
    // -----------------------------------------------------------------------
    //  The helper functions
    // -----------------------------------------------------------------------
    tCIDLib::TVoid AtomicHandleSet
    (
                HANDLE&                 pToFill
        ,       HANDLE                  hToSet
    );

    tCIDLib::TBoolean bParseCmdLine
    (
        const   tCIDLib::TCh* const     pszToParse
        ,       tCIDLib::TCh**          apszToFill
        ,       tCIDLib::TCard4&        c4ToFill
        , const tCIDLib::TCard4         c4MaxParms
    );

    tCIDLib::TBoolean bQueryExeHdr
    (
        const   tCIDLib::TCh* const     pszExePath
        ,       IMAGE_OPTIONAL_HEADER&  toFill
    );

    tCIDLib::TVoid BuildModName
    (
                tCIDLib::TCh* const     pszNameBuf
        , const tCIDLib::TCard4         c4MaxChars
        , const tCIDLib::TCh* const     pszModName
        , const tCIDLib::TCard4         c4MajVer
        , const tCIDLib::TCard4         c4MinVer
        , const tCIDLib::EModTypes      eModType
    );

    tCIDLib::TCard4 c4XlatCreateAction
    (
        const   tCIDLib::ECreateActs    eAction
    );

    tCIDLib::EFileInfoFlags eConvertAttrs
    (
        const   tCIDLib::TCard4         c4Win32Attrs
        , const tCIDLib::TCh* const     pszFileName
    );

    KRNLEXPORT tCIDLib::EWaitRes eWaitHandle
    (
                HANDLE                  hWait
        , const tCIDLib::TCard4         c4WaitMillis
    );

    KRNLEXPORT tCIDLib::EWaitRes eWaitHandles
    (
                HANDLE* const           ahWait
        ,       tCIDLib::TCard4         c4Count
        ,       tCIDLib::TCard4&        c4Signaled
        , const tCIDLib::TCard4         c4WaitMillis
    );

    namespace
    {
        // -----------------------------------------------------------------------
        //  Some internal system specific constants
        // -----------------------------------------------------------------------
        constexpr tCIDLib::TCard4 c4NonNormalAttrs =  FILE_ATTRIBUTE_SYSTEM;
    }
}



// ---------------------------------------------------------------------------
//  Macro for convenient release of COM objects, and a simple COM object
//  janitor to allow us to create them in a scope and insure they are cleaned
//  up.
// ---------------------------------------------------------------------------
#define SafeCOMFree(p) \
if (p) \
{ \
   if (p) \
   { \
       try{p->Release();} catch(...){} \
       p = 0; \
   } \
}

template <class T> class TCOMJanitor
{
    public :
        TCOMJanitor(T** ppPtr) :

            m_ppPtr(ppPtr)
        {
        }

        ~TCOMJanitor()
        {
            //
            //  If our pointer to the pointer and the pointer itself are
            //  non-null, then release it, and zero out the caller's
            //  pointer.
            //
            if (m_ppPtr && *m_ppPtr)
            {
                (*m_ppPtr)->Release();
                *m_ppPtr = 0;
            }
        }

        T* pobjOrphan()
        {
            T* pRet = *m_ppPtr;

            //
            //  Clear our pointer to the pointer, not the pointer itself.
            //  That's what we are returning.
            //
            m_ppPtr = 0;
            return pRet;
        }

        T** m_ppPtr;
};


// ---------------------------------------------------------------------------
//  A simple BSTR janitor
// ---------------------------------------------------------------------------
class KRNLEXPORT TBSTRJanitor
{
    public :
        TBSTRJanitor(const tCIDLib::TCh* const pszVal);
        ~TBSTRJanitor();
        const tCIDLib::TCh* pszData() const;
        tCIDLib::TCh* pszData();
        BSTR* pbsData();
        tCIDLib::TVoid Set(const tCIDLib::TCh* const pszToSet);

        BSTR bsData;

    private :
        TBSTRJanitor(const TBSTRJanitor&);
        tCIDLib::TVoid operator=(const TBSTRJanitor&);
};

