//
// FILE NAME: CIDKernel_Handles.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/00/1998
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
//  This header contains the class definitions for all of the handle classes.
//  These are abstractions that let the outside world deal with handles to
//  system resources without having to know what's inside them (since that
//  would be platform dependent.)
//
//  The implementations of these classes is in the same Cpp file that
//  implements the actual class that controls the resource. These defintions
//  are just here so that a single header can be included early in the main
//  header (so that other headers can see handles to all types in case they
//  need to have an instance of one as a member.)
//
// CAVEATS/GOTCHAS:
//
//  1)  These handles are purely 'by value' handles just like raw fundamental
//      handle types. They don't control the system handles inside them, they
//      just allow the outside world to manipulate them without knowing about
//      their insides. Destroying one of these does not in any way destroy
//      the system resource, that is the responsibility of the class that
//      represents the resource itself. Copying these objects does not create
//      new objects, it just copies by value the contents of the handle.
//
//  2)  The platform layer may need to memory align the contents of their particular
//      definitions of the implementation structures.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once



// ---------------------------------------------------------------------------
//  Forward reference all of the internal implementation structures, so that
//  we can have opaque pointers to them as members of our handle classes
//  below.
// ---------------------------------------------------------------------------
struct  TCommHandleImpl;
struct  TConsoleHandleImpl;
struct  TDirSearchHandleImpl;
struct  TFileHandleImpl;
struct  TEventHandleImpl;
struct  TMediaChgHandleImpl;
struct  TMemoryHandleImpl;
struct  TMutexHandleImpl;
struct  TModuleHandleImpl;
struct  TProcessHandleImpl;
struct  TSemaphoreHandleImpl;
struct  TSocketHandleImpl;
struct  TThreadHandleImpl;



#pragma CIDLIB_PACK(CIDLIBPACK)



// ---------------------------------------------------------------------------
//   CLASS: TCommHandle
//  PREFIX: hcon
// ---------------------------------------------------------------------------
class KRNLEXPORT TCommHandle
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCommHandle();

        TCommHandle
        (
            const   TCommHandle&            hcommSrc
        );

        TCommHandle
        (
                    TCommHandle&&           hcommSrc
        );

        ~TCommHandle();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCommHandle& operator=
        (
            const   TCommHandle&            hcommSrc
        );

        TCommHandle& operator=
        (
                    TCommHandle&&           hcommSrc
        );

        tCIDLib::TBoolean operator==
        (
            const   TCommHandle&            hcommSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TCommHandle&            hcommSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsValid() const;

        tCIDLib::TVoid Clear();

        tCIDLib::TVoid FormatToStr
        (
                    tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        )   const;

        const TCommHandleImpl& hcommiThis() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TKrnlCommPort;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_phcommiThis
        //      The raw handle implementation pointer. Its an opaque type
        //      to the outside world.
        // -------------------------------------------------------------------
        TCommHandleImpl*    m_phcommiThis;
};



// ---------------------------------------------------------------------------
//   CLASS: TConsoleHandle
//  PREFIX: hcon
// ---------------------------------------------------------------------------
class KRNLEXPORT TConsoleHandle
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TConsoleHandle();

        TConsoleHandle
        (
            const   TConsoleHandle&         hconSrc
        );

        ~TConsoleHandle();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TConsoleHandle& operator=
        (
            const   TConsoleHandle&         hconSrc
        );

        tCIDLib::TBoolean operator==
        (
            const   TConsoleHandle&         hconSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TConsoleHandle&         hconSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsValid() const;

        tCIDLib::TVoid Clear();

        tCIDLib::TVoid FormatToStr
        (
                    tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        )   const;

        const TConsoleHandleImpl& hconiThis() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TKrnlConIn;
        friend class TKrnlConOut;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_phconiThis
        //      The raw handle implementation pointer. Its an opaque type
        //      to the outside world.
        // -------------------------------------------------------------------
        TConsoleHandleImpl* m_phconiThis;
};



// ---------------------------------------------------------------------------
//   CLASS: TDirSearchHandle
//  PREFIX: hdir
// ---------------------------------------------------------------------------
class KRNLEXPORT TDirSearchHandle
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDirSearchHandle();

        TDirSearchHandle
        (
            const   TDirSearchHandle&       hdirSrc
        );

        ~TDirSearchHandle();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDirSearchHandle& operator=
        (
            const   TDirSearchHandle&       hdirSrc
        );

        tCIDLib::TBoolean operator==
        (
            const   TDirSearchHandle&       hdirSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TDirSearchHandle&       hdirSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsValid() const;

        tCIDLib::TVoid Clear();

        tCIDLib::TVoid FormatToStr
        (
                    tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        )   const;

        const TDirSearchHandleImpl& hdiriThis() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TKrnlDirSearch;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_phdirThis
        //      The raw handle implementation pointer. Its an opaque type
        //      to the outside world.
        // -------------------------------------------------------------------
        TDirSearchHandleImpl*   m_phdiriThis;
};



// ---------------------------------------------------------------------------
//   CLASS: TEventHandle
//  PREFIX: hev
// ---------------------------------------------------------------------------
class KRNLEXPORT TEventHandle
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TEventHandle();

        TEventHandle
        (
            const   TEventHandle&           hevSrc
        );

        TEventHandle
        (
                    TEventHandle&&          hevSrc
        );

        ~TEventHandle();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TEventHandle& operator=
        (
            const   TEventHandle&           hevSrc
        );

        TEventHandle& operator=
        (
                    TEventHandle&&          hevSrc
        );

        tCIDLib::TBoolean operator==
        (
            const   TEventHandle&           hevSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TEventHandle&           hevSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsValid() const;

        tCIDLib::TVoid Clear();

        tCIDLib::TVoid FormatToStr
        (
                    tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        )   const;

        const TEventHandleImpl& heviThis() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TKrnlEvent;
        friend class TKrnlSocket;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pheviThis
        //      The raw handle implementation pointer. Its an opaque type
        //      to the outside world.
        // -------------------------------------------------------------------
        TEventHandleImpl*   m_pheviThis;
};



// ---------------------------------------------------------------------------
//   CLASS: TFileHandle
//  PREFIX: hev
// ---------------------------------------------------------------------------
class KRNLEXPORT TFileHandle
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFileHandle();

        TFileHandle
        (
            const   TFileHandle&            hflSrc
        );

        ~TFileHandle();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFileHandle& operator=
        (
            const   TFileHandle&            hflSrc
        );

        tCIDLib::TBoolean operator==
        (
            const   TFileHandle&            hflSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TFileHandle&            hflSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsValid() const;

        tCIDLib::TVoid Clear();

        tCIDLib::TVoid FormatToStr
        (
                    tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        )   const;

        const TFileHandleImpl& hfliThis() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TKrnlFile;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_phfliThis
        //      The raw handle implementation pointer. Its an opaque type
        //      to the outside world.
        // -------------------------------------------------------------------
        TFileHandleImpl*    m_phfliThis;
};



// ---------------------------------------------------------------------------
//   CLASS: TMediaChgHandle
//  PREFIX: hcon
// ---------------------------------------------------------------------------
class KRNLEXPORT TMediaChgHandle
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMediaChgHandle();

        TMediaChgHandle
        (
            const   TMediaChgHandle&        hmchgSrc
        );

        ~TMediaChgHandle();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMediaChgHandle& operator=
        (
            const   TMediaChgHandle&        hmchgSrc
        );

        tCIDLib::TBoolean operator==
        (
            const   TMediaChgHandle&        hmchgSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TMediaChgHandle&        hmchgSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsValid() const;

        tCIDLib::TVoid Clear();

        tCIDLib::TVoid FormatToStr
        (
                    tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        )   const;

        const TMediaChgHandleImpl& hmchgiThis() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TKrnlMediaChanger;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_phmchiThis
        //      The raw handle implementation pointer. Its an opaque type
        //      to the outside world.
        // -------------------------------------------------------------------
        TMediaChgHandleImpl*    m_phmchgiThis;
};



// ---------------------------------------------------------------------------
//   CLASS: TMemoryHandle
//  PREFIX: hmem
// ---------------------------------------------------------------------------
class KRNLEXPORT TMemoryHandle
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMemoryHandle();

        TMemoryHandle
        (
            const   TMemoryHandle&          hmemSrc
        );

        ~TMemoryHandle();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMemoryHandle& operator=
        (
            const   TMemoryHandle&          hmemSrc
        );

        tCIDLib::TBoolean operator==
        (
            const   TMemoryHandle&          hmemSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TMemoryHandle&          hmemSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsValid() const;

        tCIDLib::TVoid Clear();

        tCIDLib::TVoid FormatToStr
        (
                    tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        )   const;

        const TMemoryHandleImpl& hmemiThis() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TKrnlSharedMemBuf;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_phmemiThis
        //      The raw handle implementation pointer. Its an opaque type
        //      to the outside world.
        // -------------------------------------------------------------------
        TMemoryHandleImpl*  m_phmemiThis;
};



// ---------------------------------------------------------------------------
//   CLASS: TModuleHandle
//  PREFIX: hmod
// ---------------------------------------------------------------------------
class KRNLEXPORT TModuleHandle
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TModuleHandle();

        TModuleHandle
        (
            const   TModuleHandle&          hmodSrc
        );

        ~TModuleHandle();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TModuleHandle& operator=
        (
            const   TModuleHandle&          hmodSrc
        );

        tCIDLib::TBoolean operator==
        (
            const   TModuleHandle&          hmodSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TModuleHandle&          hmodSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsValid() const;

        tCIDLib::TVoid Clear();

        tCIDLib::TVoid FormatToStr
        (
                    tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        )   const;

        const TModuleHandleImpl& hmodiThis() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TKrnlModule;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_phmodiThis
        //      The raw handle implementation pointer. Its an opaque type
        //      to the outside world.
        // -------------------------------------------------------------------
        TModuleHandleImpl*  m_phmodiThis;
};



// ---------------------------------------------------------------------------
//   CLASS: TMutexHandle
//  PREFIX: hmtx
// ---------------------------------------------------------------------------
class KRNLEXPORT TMutexHandle
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMutexHandle();

        TMutexHandle
        (
            const   TMutexHandle&           hmtxSrc
        );

        ~TMutexHandle();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMutexHandle& operator=
        (
            const   TMutexHandle&           hmtxSrc
        );

        tCIDLib::TBoolean operator==
        (
            const   TMutexHandle&           hmtxSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TMutexHandle&           hmtxSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsValid() const;

        tCIDLib::TVoid Clear();

        tCIDLib::TVoid FormatToStr
        (
                    tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        )   const;

        const TMutexHandleImpl& hmtxiThis() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TKrnlMutex;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_phmtxiThis
        //      The raw handle implementation pointer. Its an opaque type
        //      to the outside world.
        // -------------------------------------------------------------------
        TMutexHandleImpl* m_phmtxiThis;
};



// ---------------------------------------------------------------------------
//   CLASS: TProcessHandle
//  PREFIX: hproc
// ---------------------------------------------------------------------------
class KRNLEXPORT TProcessHandle
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TProcessHandle();

        TProcessHandle
        (
            const   TProcessHandle&         hprocSrc
        );

        TProcessHandle
        (
            const   TProcessHandleImpl&     hprociSrc
        );

        ~TProcessHandle();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TProcessHandle& operator=
        (
            const   TProcessHandle&         hprocSrc
        );

        tCIDLib::TBoolean operator==
        (
            const   TProcessHandle&         hprocSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TProcessHandle&         hprocSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsValid() const;

        tCIDLib::TProcessId pidThis() const;

        tCIDLib::TVoid Clear();

        tCIDLib::TVoid FormatToStr
        (
                    tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        )   const;

        const TProcessHandleImpl& hprociThis() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TKrnlExtProcess;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_phprociThis
        //      The raw handle implementation pointer. Its an opaque type
        //      to the outside world.
        // -------------------------------------------------------------------
        TProcessHandleImpl* m_phprociThis;
};



// ---------------------------------------------------------------------------
//   CLASS: TSemaphoreHandle
//  PREFIX: hsem
// ---------------------------------------------------------------------------
class KRNLEXPORT TSemaphoreHandle
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSemaphoreHandle();

        TSemaphoreHandle
        (
            const   TSemaphoreHandle&       hsemSrc
        );

        ~TSemaphoreHandle();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSemaphoreHandle& operator=
        (
            const   TSemaphoreHandle&       hsemSrc
        );

        tCIDLib::TBoolean operator==
        (
            const   TSemaphoreHandle&       hsemSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TSemaphoreHandle&       hsemSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsValid() const;

        tCIDLib::TVoid Clear();

        tCIDLib::TVoid FormatToStr
        (
                    tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        )   const;

        const TSemaphoreHandleImpl& hsemiThis() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TKrnlSemaphore;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_phsemiThis
        //      The raw handle implementation pointer. Its an opaque type
        //      to the outside world.
        // -------------------------------------------------------------------
        TSemaphoreHandleImpl*   m_phsemiThis;
};



// ---------------------------------------------------------------------------
//   CLASS: TSocketHandle
//  PREFIX: hsock
// ---------------------------------------------------------------------------
class KRNLEXPORT TSocketHandle
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSocketHandle();

        TSocketHandle
        (
            const   TSocketHandle&          hsockSrc
        );

        ~TSocketHandle();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSocketHandle& operator=
        (
            const   TSocketHandle&          hsockSrc
        );

        tCIDLib::TBoolean operator==
        (
            const   TSocketHandle&          hsockSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TSocketHandle&          hsockSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsValid() const;

        tCIDLib::TVoid Clear();

        tCIDLib::TVoid FormatToStr
        (
                    tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        )   const;

        const TSocketHandleImpl& hsockiThis() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TKrnlSocket;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_phsockiThis
        //      The raw handle implementation pointer. Its an opaque type
        //      to the outside world.
        // -------------------------------------------------------------------
        TSocketHandleImpl*  m_phsockiThis;
};



// ---------------------------------------------------------------------------
//   CLASS: TThreadHandle
//  PREFIX: hthr
// ---------------------------------------------------------------------------
class KRNLEXPORT TThreadHandle
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TThreadHandle();

        TThreadHandle
        (
            const   TThreadHandle&          hthrSrc
        );

        TThreadHandle
        (
                    TThreadHandle&&         hthrSrc
        );

        ~TThreadHandle();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TThreadHandle& operator=
        (
            const   TThreadHandle&          hthrSrc
        );

        TThreadHandle& operator=
        (
                    TThreadHandle&&         hthrSrc
        );

        tCIDLib::TBoolean operator==
        (
            const   TThreadHandle&          hthrSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TThreadHandle&          hthrSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsValid() const;

        tCIDLib::TVoid Clear();

        tCIDLib::TVoid FormatToStr
        (
                    tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
        )   const;

        const TThreadHandleImpl& hthriThis() const;

        TThreadHandleImpl& hthriThis();

        tCIDLib::TThreadId tidThis() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TKrnlThread;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_phthriThis
        //      The raw handle implementation pointer. Its an opaque type
        //      to the outside world.
        // -------------------------------------------------------------------
        TThreadHandleImpl*  m_phthriThis;
};

#pragma CIDLIB_POPPACK

