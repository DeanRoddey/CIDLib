//
// FILE NAME: CIDKernel_MemCheck.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/13/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDKernel_MemCheck.Cpp file, which implements
//  the TKrnlMemCheck class. This class is an abstraction for the host or
//  development environment memory debugging tools.
//
//  Its only for use in testing programs. The constructor will take a snap
//  shot. The destructor will, if differences exist, dump the differences
//  between them. So it is for scope based hierachical memory checking.
//
//  The implementations are compiled away when not in debug mode.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

#if CID_DEBUG_ON

class TKrnlFile;


// ---------------------------------------------------------------------------
//   CLASS: TKrnlMemCheck
//  PREFIX: kmchk
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlMemCheck
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static method
        // -------------------------------------------------------------------
        static tCIDLib::TVoid DumpLeaks();

        static tCIDLib::TVoid ReportToFile
        (
            const   tCIDLib::TCh* const     pszFile
        );

        static tCIDLib::TVoid ReportToStdOut();

        static tCIDLib::TVoid SetHeapCheckLevel
        (
            const   tCIDLib::EHeapChkLevels eLevel
            , const tCIDLib::TBoolean       bNonFreeing
        );

        static tCIDLib::TVoid ValidateHeap();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlMemCheck();

        TKrnlMemCheck(const TKrnlMemCheck&) = delete;

        ~TKrnlMemCheck();


        // -------------------------------------------------------------------
        //  Public oeprators
        // -------------------------------------------------------------------
        TKrnlMemCheck& operator=(const TKrnlMemCheck&) = delete;
        tCIDLib::TVoid* operator new(const tCIDLib::TUInt) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DumpSnapDiffs();

        tCIDLib::TVoid TakeSnapshot();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pSnapShot
        //      This a shapshot buffer. Its non-typed here, and seen as its
        //      real type internally.
        // -------------------------------------------------------------------
        tCIDLib::TVoid* m_pSnapShot;
};

#endif

#pragma CIDLIB_POPPACK

