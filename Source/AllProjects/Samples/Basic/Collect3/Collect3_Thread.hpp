//
// FILE NAME: Collect3_Thread.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/07/1997
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
//  This is the header for the Collect3_Thread.cpp module. This module
//  implements the thread class used by this program.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TColThread
//  PREFIX: thr
// ---------------------------------------------------------------------------
class TColThread : public TThread
{
    public :
        // -------------------------------------------------------------------
        //  Public types
        // -------------------------------------------------------------------
        enum class EThreadTypes
        {
            Adder
            , Remover
        };


        // -------------------------------------------------------------------
        //  Constructors and destructors
        // -------------------------------------------------------------------
        TColThread() = delete;

        TColThread
        (
                    TTestCol* const         pcolToUse
            , const EThreadTypes            eType
            ,       TTextOutStream* const   pstrmOut
        );

        TColThread(const TColThread&) = delete;

        ~TColThread();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TColThread& operator=(const TColThread&) = delete;


    protected   :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eProcess();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eType
        //      The type of thread this instance should be, an adder or a
        //      remover.
        //
        //  m_pcolToUse
        //      This is a pointer to the collection that the thread works
        //      on. Its not adopted, just referenced. The type is just a
        //      typedef of a template collection.
        //
        //  m_pstrmOut
        //      The stream to use for text output. Note that text output
        //      is effectively synchronized using the collection lock. Since
        //      the thread only outputs when the collection is locked, the
        //      output is naturally synchronized.
        // -------------------------------------------------------------------
        EThreadTypes    m_eType;
        TTestCol*       m_pcolToUse;
        TTextOutStream* m_pstrmOut;


        // -------------------------------------------------------------------
        //  Do any required macros
        // -------------------------------------------------------------------
        RTTIDefs(TColThread,TThread)
};
