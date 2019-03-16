//
// FILE NAME: CIDLib_Console.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/22/1996
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
//  This is the header for the CIDLib_Console.Cpp file, which implements
//  the input and output console classes, and underlying stream impl classes
//  that allow a console to be used as a stream sink/source.
//
//  TConsoleInImpl and TConsoleOutImpl are standard stream implementation
//  derivatives. These allow TBinInStream and TBinOutStream's (respectively)
//  work over a console. Text streams work on top of binary streams so, even
//  though consoles only deal in text, the only way to get the text to/from
//  the console via a text stream is to go through a binary stream and its
//  implementation object.
//
//  TInConsole and TOutConsole are simple text stream derivatives that just
//  set up a binary stream (with a console implementation object) as the
//  underlying data stream of the console. These insure that the in and out
//  text converter is a UTF-16 converter, since that what needs to pass
//  though the binary stream to the underlying console implementation (and
//  from there on through the CIDKernel console classes that give access to
//  the actual console I/O services.)
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
//   CLASS: TConsoleInImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------
class CIDLIBEXP TConsoleInImpl : public TInStreamImpl
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TConsoleInImpl
        (
            const   tCIDLib::TBoolean       bInsertMode
            , const tCIDLib::TCard4         c4MaxRecall
        );

        TConsoleInImpl(const TConsoleInImpl&) = delete;

        ~TConsoleInImpl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TConsoleInImpl& operator=(const TConsoleInImpl&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsOpen() const override;

        tCIDLib::TCard4 c4ReadBytes
        (
                    tCIDLib::TVoid* const   pToFill
            , const tCIDLib::TCard4         c4BytesToRead
        ) override;

        tCIDLib::TCard8 c8CurPos() const override;

        tCIDLib::TVoid Reset() override;

        tCIDLib::TVoid SkipForwardBy
        (
            const   tCIDLib::TCard4         c4SkipBy
        ) override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4ReadLine
        (
                    TString&                strToFill
            ,       TKrnlConIn::TIdleCB     pfnCallback
            ,       TObject* const          pobjCBData
        );

        tCIDLib::TCh chReadChar
        (
                    tCIDLib::EConKeys&      eKeyType
            , const tCIDLib::TCard4         c4WaitMS
            ,       TKrnlConIn::TIdleCB     pfnCallback
            ,       TObject* const          pobjCBData
        );

        tCIDLib::TVoid ResetRecallBuffer();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c8FauxPos
        //      We have to honor input stream impl semantics, which requires
        //      that we report our current position. This has little meaning
        //      in a console, but we fake one in order to fit in.
        //
        //  m_kconImpl
        //      The kernel console input wrapper object that provides the
        //      access the local console implementation.
        // -------------------------------------------------------------------
        tCIDLib::TCard8 m_c8FauxPos;
        TKrnlConIn      m_kconImpl;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TConsoleInImpl,TInStreamImpl)
};



// ---------------------------------------------------------------------------
//   CLASS: TConsoleOutImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------
class CIDLIBEXP TConsoleOutImpl : public TOutStreamImpl
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TConsoleOutImpl();

        TConsoleOutImpl(const TConsoleOutImpl&) = delete;

        ~TConsoleOutImpl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TConsoleOutImpl& operator=(const TConsoleOutImpl&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bBackspace();

        tCIDLib::TBoolean bIsOpen() const;

        tCIDLib::TCard4 c4WriteBytes
        (
            const   tCIDLib::TVoid* const   pToWrite
            , const tCIDLib::TCard4         c4BytesToWrite
        );

        tCIDLib::TCard8 c8CurPos() const;

        tCIDLib::TVoid Reset();

        tCIDLib::TVoid SeekToEnd();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4QueryVerticalLines() const;

        tCIDLib::TVoid ClearScr();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c8FauxPos
        //      We have to honor input stream impl semantics, which requires
        //      that we report our current position. This has little meaning
        //      in a console, but we fake one in order to fit in.
        //
        //  m_kconImpl
        //      The kernel console output wrapper object that provides the
        //      access the local console implementation.
        // -------------------------------------------------------------------
        tCIDLib::TCard8 m_c8FauxPos;
        TKrnlConOut     m_kconImpl;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TConsoleOutImpl,TOutStreamImpl)
};



// ---------------------------------------------------------------------------
//   CLASS: TInConsole
//  PREFIX: cons
// ---------------------------------------------------------------------------
class CIDLIBEXP TInConsole : public TTextInStream
{
    public  :
        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TInConsole
        (
            const   tCIDLib::TBoolean       bInsertMode = kCIDLib::False
            , const tCIDLib::TCard4         c4RecallMax = 0
        );

        TInConsole(const TInConsole&) = delete;

        ~TInConsole();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TInConsole& operator=(const TInConsole&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4ReadLine
        (
                    TString&                strToFill
            ,       TKrnlConIn::TIdleCB     pfnCallback = nullptr
            ,       TObject* const          pobjCBData = nullptr
        );

        tCIDLib::TCh chReadChar
        (
                    tCIDLib::EConKeys&      eKeyType
            , const tCIDLib::TCard4         c4WaitMS
            ,       TKrnlConIn::TIdleCB     pfnCallback = nullptr
            ,       TObject* const          pobjCBData = nullptr
        );

        tCIDLib::TVoid ResetRecallBuffer();

        tCIDLib::TVoid WaitKey();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pstrmiCon
        //      Our own copy of the pointer to the console input impl object.
        //      We don't own it, but need a copy for our own use internally.
        // -------------------------------------------------------------------
        TConsoleInImpl* m_pstrmiCon;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TInConsole,TTextInStream)
};



// ---------------------------------------------------------------------------
//   CLASS: TOutConsole
//  PREFIX: cons
// ---------------------------------------------------------------------------
class CIDLIBEXP TOutConsole : public TTextOutStream
{
    public  :
        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TOutConsole();

        TOutConsole(const TOutConsole&) = delete;

        ~TOutConsole();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TOutConsole& operator=(const TOutConsole&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bBackspace();

        tCIDLib::TCard4 c4QueryVerticalLines() const;

        tCIDLib::TVoid ClearScr();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pstrmiCon
        //      Our own copy of the pointer to the console output impl object.
        //      We don't own it, but need a copy for our own use internally.
        // -------------------------------------------------------------------
        TConsoleOutImpl*    m_pstrmiCon;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TOutConsole,TTextOutStream)
};



// ---------------------------------------------------------------------------
//   CLASS: TConPager
//  PREFIX: conp
// ---------------------------------------------------------------------------
class CIDLIBEXP TConPager : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TConPager() = delete ;

        TConPager
        (
                    TInConsole* const       pconIn
            ,       TOutConsole* const      pconOut
            , const TString&                strPrompt
        );

        TConPager(const TOutConsole&) = delete;

        ~TConPager();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TCard4 operator++(int);

        TConPager& operator=(const TConPager&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bBreakSeen();

        tCIDLib::TVoid Reset();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bBreakSeen
        //      This is set when a Break key is seen during a page break
        //      pause. It can be queried by the bBreakSeen() method, which
        //      also resets it.
        //
        //  m_c4CurLine
        //      The current console window line that we are on.
        //
        //  m_c4WndSize
        //      The console window size, vertically in lines.
        //
        //  m_pconIn
        //  m_pconOut
        //      Our input and output consoles. We need both of these to hanlde
        //      outputting the pause message, waiting for input, querying the
        //      console window size, etc...
        //
        //  m_strPrompt
        //      The prompt to output when we have to pause. It should be short
        //      and sweet, like "--- more ---" or something like that. Most
        //      definitely it must be one line or less.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bBreakSeen;
        tCIDLib::TCard4     m_c4CurLine;
        tCIDLib::TCard4     m_c4WndSize;
        TInConsole*         m_pconIn;
        TOutConsole*        m_pconOut;
        TString             m_strPrompt;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TConPager,TObject)
};

#pragma CIDLIB_POPPACK



