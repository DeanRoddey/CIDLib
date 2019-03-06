//
// FILE NAME: CIDKernel_ConOut.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/10/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDKernel_Console.Cpp file, which implements
//  the TKrnlConOut and TKrnlConIn classes These classes provide input and
//  output console support.
//
//  These classes serve two purposes. One is to support the higher level
//  streaming system, console streams being one possible type of stream. In
//  this scheme, they just provide a means to get the console input and
//  output handles and read and write bulk data to and from them. The streams
//  are required to read and write native wide characters which is all that
//  these console classes understand.
//
//  Redirection at this level is ignored. CIDLib provides its own, more
//  flexible, redirection scheme.
//
//
// CAVEATS/GOTCHAS:
//
//  1)  Construction just sets up the object, which must then be bOpen()'d
//      before it can be used.
//
//  2)  Each platform can store state info in the m_pExtra field of the
//      console classes, which they will generally need to do. The platform
//      independent stuff will call the bOpen and Close methods to provide a
//      chance to create and clean up this extra information. It must be seen
//      as a void pointer at this level.
//
//  3)  TKrnlConIn implements the MSignalHandler mixin so that it can set
//      up itself as a signal handler for break events (to be able to break
//      out of the line input stuff.)
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//   CLASS: TKrnlConIn
//  PREFIX: kcon
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlConIn : public MSignalHandler
{
    public  :
        // -------------------------------------------------------------------
        //  Public class types
        // -------------------------------------------------------------------
        using TIdleCB = tCIDLib::TVoid (*)(TObject* const pobjData);


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlConIn
        (
            const   tCIDLib::TBoolean       bInsertMode = kCIDLib::False
            , const tCIDLib::TCard4         c4MaxRecall = 0
        );

        TKrnlConIn(const TKrnlConIn&) = delete;

        ~TKrnlConIn();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlConIn& operator=(const TKrnlConIn&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bHandleSignal
        (
            const   tCIDLib::ESignals       eSignal
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        //
        //  Some of these are provided by the per-platform code, and some
        //  are higher level, platform independent stuff.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsOpen() const;

        tCIDLib::TBoolean bOpen();

        tCIDLib::TBoolean bReadLine
        (
                    tCIDLib::TCh* const     pchToFill
            , const tCIDLib::TCard4         c4CharsToRead
            ,       tCIDLib::TCard4&        c4CharsRead
            ,       TIdleCB                 pfnCallback = nullptr
            ,       TObject* const          pobjCBData = nullptr
        );

        tCIDLib::TBoolean bReadCharMS
        (
                    tCIDLib::EConKeys&      eKeyType
            ,       tCIDLib::TCh&           chGotten
            , const tCIDLib::TCard4         c4Wait = kCIDLib::c4MaxWait
            ,       TIdleCB                 pfnCallback = nullptr
            ,       TObject* const          pobjCBData = nullptr
        );

        tCIDLib::TBoolean bReadChar
        (
                    tCIDLib::EConKeys&      eKeyType
            ,       tCIDLib::TCh&           chGotten
            , const tCIDLib::TEncodedTime   enctEnd
            ,       TIdleCB                 pfnCallback = nullptr
            ,       TObject* const          pobjCBData = nullptr
        );

        tCIDLib::TVoid ResetRecallBuf();


    private :
        // -------------------------------------------------------------------
        //  Private type forward references
        // -------------------------------------------------------------------
        struct TConPlatInfo;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        //
        //  These are implemented by the per-platform code.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bOpenCon();

        tCIDLib::TBoolean bSetInsertMode
        (
            const   tCIDLib::TBoolean       bNewState
        );

        tCIDLib::TVoid Close();

        tCIDLib::TVoid NewEntry
        (
            const   tCIDLib::TCh* const     pszBuffer
        );

        TConPlatInfo* pInitPlatform
        (
            const   tCIDLib::TCard4         c4MaxRecall
        );

        tCIDLib::TVoid ResetRecall();

        tCIDLib::TVoid TermPlatform();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bBreakSeen
        //      A break signal has been seen. This is set by the signal
        //      handler, which will then be seen by the line input method
        //      who will berak out of the input and return an empty string.
        //
        //  m_bInsertMode
        //      The current insert mode. This is likely enough to be needed
        //      by any implementation, that its provided here for all instead
        //      making them do it in the m_pExtra field.
        //
        //  m_pPlatform
        //      Extra storage space for some platforms that might to store
        //      some state across calls that is not provided here. The
        //      ctor/dtor calls the private methods m_pInitPlatform() and
        //      m_TermPlatform() so that this info can be created and cleaned
        //      up.
        //
        //  m_hconThis
        //      The input handle for the console.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bBreakSeen;
        tCIDLib::TBoolean       m_bInsertMode;
        TConPlatInfo*           m_pPlatform;
        TConsoleHandle          m_hconThis;
};



// ---------------------------------------------------------------------------
//   CLASS: TKrnlConOut
//  PREFIX: kcon
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlConOut
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlConOut();

        TKrnlConOut(const TKrnlConOut&) = delete;

        ~TKrnlConOut();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlConOut& operator=(const TKrnlConOut&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bBackspace();

        tCIDLib::TBoolean bClearScr();

        tCIDLib::TBoolean bIsOpen() const;

        tCIDLib::TBoolean bOpen();

        tCIDLib::TBoolean bPutChar
        (
            const   tCIDLib::TCh            chToPut
        );

        tCIDLib::TBoolean bQueryVerticalLines
        (
                    tCIDLib::TCard4&        c4ToFill
        )   const;

        tCIDLib::TBoolean bReset();

        tCIDLib::TBoolean bWriteBytes
        (
            const   tCIDLib::TVoid* const   pToWrite
            , const tCIDLib::TCard4         c4ToWrite
            ,       tCIDLib::TCard4&        c4Written
        );

        tCIDLib::TBoolean bWriteLine
        (
            const   tCIDLib::TCh* const     pszToWrite
        );


    private :
        // -------------------------------------------------------------------
        //  Class type forward references
        // -------------------------------------------------------------------
        struct TConPlatInfo;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        //
        //  These are all per-platform implementation methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bOpenCon();

        tCIDLib::TVoid Close();

        TConPlatInfo* pInitPlatform();

        tCIDLib::TVoid TermPlatform();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_hconThis
        //      The output handle for the console.
        //
        //  m_pPlatform
        //      This is a space for each platform to store extra information
        //      needed.
        // -------------------------------------------------------------------
        TConsoleHandle      m_hconThis;
        TConPlatInfo*       m_pPlatform;
};

#pragma CIDLIB_POPPACK


