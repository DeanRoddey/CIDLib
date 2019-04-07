//
// FILE NAME: CIDMacroEng_ErrorHandler.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/15/2003
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
//  This is the header file for the CIDMacroEng_ErrorHandler.cpp file, which
//  implements the MMEngErrHandler and MMEngPrsErrHandler classes. Thes are
//  pure abstract base classes that are used as a mixins, to allow classes
//  to implement error handlers exceptions from the engine at runtime and for
//  parse errors during parsing.
//
//  We also provide standard pre-fab implementations that provides a stand
//  alone error handler that can dump to a given stream.
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
//  CLASS: MMEngErrHandler
// PREFIX: meeh
// ---------------------------------------------------------------------------
class CIDMACROENGEXP MMEngErrHandler
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        MMEngErrHandler(const MMEngErrHandler&) = delete;

        ~MMEngErrHandler();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TVoid operator=(const MMEngErrHandler&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid Exception
        (
            const   TError&                 errThrown
            ,       TCIDMacroEngine&        meSource
        ) = 0;

        virtual tCIDLib::TVoid MacroException
        (
            const   TMEngExceptVal&         mecvThrown
            ,       TCIDMacroEngine&        meSource
        ) = 0;

        virtual tCIDLib::TVoid UnknownException
        (
                    TCIDMacroEngine&        meSource
        ) = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        MMEngErrHandler();
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngStrmErrHandler
// PREFIX: meeh
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngStrmErrHandler

    : public TObject, public MMEngErrHandler
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngStrmErrHandler();

        TMEngStrmErrHandler
        (
                    TTextOutStream* const   pstrmToUse
        );

        TMEngStrmErrHandler(const TMEngStrmErrHandler&) = delete;

        ~TMEngStrmErrHandler();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngStrmErrHandler& operator=(const TMEngStrmErrHandler&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Exception
        (
            const   TError&                 errThrown
            ,       TCIDMacroEngine&        meSource
        )   override;

        tCIDLib::TVoid MacroException
        (
            const   TMEngExceptVal&         mecvThrown
            ,       TCIDMacroEngine&        meSource
        )   override;

        tCIDLib::TVoid UnknownException
        (
                    TCIDMacroEngine&        meSource
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SetStream
        (
                    TTextOutStream* const   pstrmToUse
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pstrmTarget
        //      The target stream to dump errors to. We don't own it, we just
        //      reference it.
        // -------------------------------------------------------------------
        TTextOutStream*     m_pstrmTarget;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngStrmErrHandler,TObject)
};



// ---------------------------------------------------------------------------
//  CLASS: MMEngPrsErrHandler
// PREFIX: meeh
// ---------------------------------------------------------------------------
class CIDMACROENGEXP MMEngPrsErrHandler
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        MMEngPrsErrHandler(const MMEngPrsErrHandler&) = delete;

        ~MMEngPrsErrHandler();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TVoid operator=(const MMEngPrsErrHandler&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid ParseEvent
        (
            const   tCIDMacroEng::EPrsEvs   eEvent
            , const TString&                strText
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TCard4         c4ColNum
            , const TString&                strClassPath
        ) = 0;

        virtual tCIDLib::TVoid ParseException
        (
            const   TError&                 errCaught
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TCard4         c4ColNum
            , const TString&                strClassPath
        ) = 0;

        virtual tCIDLib::TVoid ParseException
        (
            const   tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TCard4         c4ColNum
            , const TString&                strClassPath
        ) = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        MMEngPrsErrHandler();

};



// ---------------------------------------------------------------------------
//  CLASS: TMEngStrmPrsErrHandler
// PREFIX: meeh
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngStrmPrsErrHandler

    : public TObject, public MMEngPrsErrHandler
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngStrmPrsErrHandler();

        TMEngStrmPrsErrHandler
        (
                    TTextOutStream* const   pstrmToUse
        );

        TMEngStrmPrsErrHandler(const TMEngStrmPrsErrHandler&) = delete;

        ~TMEngStrmPrsErrHandler();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngStrmPrsErrHandler& operator=(const TMEngStrmPrsErrHandler&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ParseEvent
        (
            const   tCIDMacroEng::EPrsEvs   eEvent
            , const TString&                strText
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TCard4         c4ColNum
            , const TString&                strClassPath
        )   override;

        tCIDLib::TVoid ParseException
        (
            const   TError&                 errCaught
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TCard4         c4ColNum
            , const TString&                strClassPath
        )   override;

        tCIDLib::TVoid ParseException
        (
            const   tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TCard4         c4ColNum
            , const TString&                strClassPath
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SetStream
        (
                    TTextOutStream* const   pstrmToUse
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pstrmTarget
        //      The target stream to dump errors to. We don't own it, we just
        //      reference it.
        // -------------------------------------------------------------------
        TTextOutStream*     m_pstrmTarget;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngStrmPrsErrHandler,TObject)
};

#pragma CIDLIB_POPPACK


