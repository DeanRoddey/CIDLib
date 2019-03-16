//
// FILE NAME: CIDKernel.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/06/1992
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
//  This is the main public header for the facility, which brings in all the
//  public functionality of this facility, though it generally is only for
//  direct use by the CIDLib facility and a couple other facilities that
//  wrap some system services.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Set up our import/export attributes
// ---------------------------------------------------------------------------
#if     defined(PROJ_CIDKERNEL)
#define KRNLEXPORT  CID_DLLEXPORT
#else
#define KRNLEXPORT  CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Subinclude the really fundamental headers that must be before everything.
// ---------------------------------------------------------------------------
#include    "CIDKernel_PlatformDefines.hpp"
#include    "CIDKernel_PlatformIncludes.hpp"
#include    "CIDKernel_DevMacros.hpp"
#include    "CIDKernel_PlatformTypes.hpp"
#include    "CIDKernel_Type.hpp"
#include    "CIDKernel_Constant.hpp"
#include    "CIDKernel_Version.hpp"
#include    "CIDKernel_PlatformConstants.hpp"
#include    "CIDKernel_BaseInlines.hpp"
#include    "CIDKernel_MemCheck.hpp"
#include    "CIDKernel_Unicode.hpp"
#include    "CIDKernel_Error.hpp"
#include    "CIDKernel_Handles.hpp"
#include    "CIDKernel_Module.hpp"
#include    "CIDKernel_ConCharSet.hpp"


// ---------------------------------------------------------------------------
//  Do the standard enum tricks for our enums that can handle it.
// ---------------------------------------------------------------------------
#if !defined(CIDKRNL_NOCLASSES)
StdEnumTricks(tCIDLib::EAdoptOpts)
StdEnumTricks(tCIDLib::EAngleTypes)
StdEnumTricks(tCIDLib::EAudioCues)
StdEnumTricks(tCIDLib::EAutoModes)
StdEnumTricks(tCIDLib::ECPUTypes)
StdEnumTricks(tCIDLib::ECreateActs)
StdEnumTricks(tCIDLib::EDelModes)
StdEnumTricks(tCIDLib::EDirs)
StdEnumTricks(tCIDLib::EVolHWTypes)
StdEnumTricks(tCIDLib::EEndianModes)
StdEnumTricks(tCIDLib::EEnds)
StdEnumTricks(tCIDLib::EErrClasses)
StdEnumTricks(tCIDLib::EEventStates)
StdEnumTricks(tCIDLib::EForceOpts)
StdEnumTricks(tCIDLib::EHJustify)
StdEnumTricks(tCIDLib::EInitTerm)
StdEnumTricks(tCIDLib::ELanguages)
StdEnumTricks(tCIDLib::ELockStates)
StdEnumTricks(tCIDLib::ELogMapModes)
StdEnumTricks(tCIDLib::EModTypes)
StdEnumTricks(tCIDLib::EMonths)
StdEnumTricks(tCIDLib::ENamedRscTypes)
StdEnumTricks(tCIDLib::EPrioClasses)
StdEnumTricks(tCIDLib::EPrioLevels)
StdEnumTricks(tCIDLib::EQPrios)
StdEnumTricks(tCIDLib::ERangeStates)
StdEnumTricks(tCIDLib::ERecoverable)
StdEnumTricks(tCIDLib::ERectlTypes)
StdEnumTricks(tCIDLib::ESeverities)
StdEnumTricks(tCIDLib::EShareStates)
StdEnumTricks(tCIDLib::ESortDirs)
StdEnumTricks(tCIDLib::ESpecialPaths)
StdEnumTricks(tCIDLib::EStartEnd)
StdEnumTricks(tCIDLib::ESysExcepts)
StdEnumTricks(tCIDLib::ESymmetries)
StdEnumTricks(tCIDLib::ETriStates)
StdEnumTricks(tCIDLib::EVJustify)
StdEnumTricks(tCIDLib::EVisible)
StdEnumTricks(tCIDLib::EWeekDays)
#endif



// ---------------------------------------------------------------------------
//   CLASS: TCIDKrnlModule
//  PREFIX: kmod
//
//  We need to be able to kick off a little bit of init internally that
//  cannot be reasonably handled by way of lazy init (which is what is used
//  whereever possible.) So this class is just a simple derivative of the
//  basic kernel module class that calls a couple of init methods during
//  its construction.
//
//  The init/term methods are implemented in their respective implementation
//  files.
// ---------------------------------------------------------------------------
class KRNLEXPORT TCIDKrnlModule : public TKrnlModule
{
    public :
        // -------------------------------------------------------------------
        //  Public class types
        //
        //  NOTE:   This is public temporarily because the Kai compiler doesn't
        //          like it when its hidden.
        //
        //  TInitMethod
        //      This is a method pointer prototype for the methods that are
        //      used to initialize modules. This is used to create an array
        //      of init methods, in the correct bootstrapping order.
        // -------------------------------------------------------------------
        using TInitMethod = tCIDLib::TBoolean (TCIDKrnlModule::*)(const tCIDLib::EInitTerm);


        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bServiceMode();

        static tCIDLib::TVoid InitKrnlModule
        (
            const   tCIDLib::TBoolean       bServiceMode
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDKrnlModule();

        TCIDKrnlModule(const TCIDKrnlModule&) = delete;

        ~TCIDKrnlModule();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDKrnlModule& operator=(const TCIDKrnlModule&) = delete;


        // --------------------------------------------------------------------
        //  Public, methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid ErrorPopUp
        (
            const   tCIDLib::TErrCode       errcToDisplay
            , const tCIDLib::TCh* const     pszFileName
            , const tCIDLib::TCard4         c4Line
            , const tCIDLib::TCh* const     pszAuxMsg = kCIDLib::pszEmptyZStr
        );

        tCIDLib::TVoid KrnlErrorPopUp
        (
            const   TKrnlError&             kerrToDisplay
            , const tCIDLib::TCh* const     pszFileName
            , const tCIDLib::TCard4         c4Line
            , const tCIDLib::TCh* const     pszAuxMsg = kCIDLib::pszEmptyZStr
        );

        tCIDLib::TVoid MsgPopUp
        (
            const   tCIDLib::TCh* const     pszMsg
            , const tCIDLib::TCh* const     pszFileName
            , const tCIDLib::TCard4         c4Line
        );


    private :
        // -------------------------------------------------------------------
        //  Private class types
        //
        //  TInitRec
        //      A simple structure used to build up a list of the init/term
        //      methods and their associated message ids.
        // -------------------------------------------------------------------
        struct TInitRec
        {
            TInitMethod     pfnInitTerm;
            tCIDLib::TMsgId midDescription;
        };


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bInitTermAudio
        (
            const   tCIDLib::EInitTerm      eState
        );

        tCIDLib::TBoolean bInitTermRemMedia
        (
            const   tCIDLib::EInitTerm      eState
        );

        tCIDLib::TBoolean bInitTermConsole
        (
            const   tCIDLib::EInitTerm      eState
        );

        tCIDLib::TBoolean bInitTermCore
        (
            const   tCIDLib::EInitTerm      eState
        );

        tCIDLib::TBoolean bInitTermEnvironment
        (
            const   tCIDLib::EInitTerm      eState
        );

        tCIDLib::TBoolean bInitTermFileSys
        (
            const   tCIDLib::EInitTerm      eState
        );

        tCIDLib::TBoolean bInitTermPerThread
        (
            const   tCIDLib::EInitTerm      eState
        );

        tCIDLib::TBoolean bInitTermSigHandler
        (
            const   tCIDLib::EInitTerm      eState
        );

        tCIDLib::TBoolean bInitTermSysInfo
        (
            const   tCIDLib::EInitTerm      eState
        );

        tCIDLib::TBoolean bInitTermIP
        (
            const   tCIDLib::EInitTerm      eState
        );

        tCIDLib::TBoolean bInitTermTime
        (
            const   tCIDLib::EInitTerm      eState
        );

        tCIDLib::TBoolean bInitTermThread
        (
            const   tCIDLib::EInitTerm      eState
        );

        tCIDLib::TVoid InitTermError
        (
            const   tCIDLib::EInitTerm      eState
        );

        tCIDLib::TVoid InitTermLocale
        (
            const   tCIDLib::EInitTerm      eState
        );


        // --------------------------------------------------------------------
        //  Private data
        //
        //  m_bCanLoadText
        //      This is used to indicate to the low level popup methods when
        //      we've gotten far enough along to load text.
        //
        //  m_c4InitCount
        //      The number of init/term methods in the m_pafInitMethods array.
        //
        //  m_pafInitMethods
        //      A array of TInitMethod structures that is set up with the list
        //      of init methods. It is set up during construction.
        //
        //  m_pszKrnlInitErr
        //  m_pszKrnlTermErr
        //      These are fundamental strings used if a kernel excp is thrown
        //      while calling module init methods. They are preloaded during
        //      construction before the init methods are called.
        //
        //  m_pszMsgNotFound
        //      This message is used if a low level error popup is done on an
        //      error for which text cannot be loaded, or when the init is not
        //      far enough along to load it. Its initialized to a default
        //      English version that will be there until/unless the real one
        //      is loaded.
        //
        //  m_pszPopUpTitle1
        //  m_pszPopUpTitle2
        //      The strings used in the low level popups done if something
        //      awry during fundamental initialization. The constructor tries
        //      to load them from the message file. If its not found, then a
        //      hard coded English version is put into place.
        //
        //  m_pszSysInitErr
        //  m_pszSysTermErr
        //      These are fundamental strings used if a system excp is thrown
        //      while calling module init methods. They are preloaded during
        //      construction before the init methods are called.
        //
        //  m_pszUnknownErr
        //      This is used in a lot of places during init and some others,
        //      so we preload it and let all our methods use it from here.
        // --------------------------------------------------------------------
        TInitRec*           m_parecInit;
        tCIDLib::TBoolean   m_bCanLoadText;
        tCIDLib::TCard4     m_c4InitCount;
        TInitMethod*        m_pafInitMethods;
        const tCIDLib::TCh* m_pszKrnlInitErr;
        const tCIDLib::TCh* m_pszKrnlTermErr;
        const tCIDLib::TCh* m_pszMsgNotFound;
        const tCIDLib::TCh* m_pszPopUpTitle1;
        const tCIDLib::TCh* m_pszPopUpTitle2;
        const tCIDLib::TCh* m_pszSysInitErr;
        const tCIDLib::TCh* m_pszSysTermErr;
        const tCIDLib::TCh* m_pszUnknownErr;
};



// ---------------------------------------------------------------------------
//  Now subinclude our other headers that we want everyone to see
// ---------------------------------------------------------------------------
#include    "CIDKernel_ErrorIds.hpp"
#include    "CIDKernel_LList.hpp"
#include    "CIDKernel_BaseLock.hpp"
#include    "CIDKernel_RawBits.hpp"
#include    "CIDKernel_String.hpp"
#include    "CIDKernel_KVPair.hpp"

#include    "CIDKernel_Audio.hpp"
#include    "CIDKernel_RawMemory.hpp"
#include    "CIDKernel_RawString.hpp"
#include    "CIDKernel_FlagJanitors.hpp"
#include    "CIDKernel_Environment.hpp"
#include    "CIDKernel_Process.hpp"
#include    "CIDKernel_ResourceName.hpp"
#include    "CIDKernel_CriticalSection.hpp"
#include    "CIDKernel_SharedMemBuf.hpp"
#include    "CIDKernel_Event.hpp"
#include    "CIDKernel_Mutex.hpp"
#include    "CIDKernel_Semaphore.hpp"
#include    "CIDKernel_ExternalProcess.hpp"
#include    "CIDKernel_CommPort.hpp"
#include    "CIDKernel_FileSystem.hpp"
#include    "CIDKernel_File.hpp"
#include    "CIDKernel_HashMap.hpp"
#include    "CIDKernel_HeapJanitor.hpp"
#include    "CIDKernel_Locale.hpp"
#include    "CIDKernel_LowLevelPopUp.hpp"
#include    "CIDKernel_MathLib.hpp"
#include    "CIDKernel_PathString.hpp"
#include    "CIDKernel_PerThreadData.hpp"
#include    "CIDKernel_SafeCounter.hpp"
#include    "CIDKernel_Signals.hpp"
#include    "CIDKernel_SystemInfo.hpp"
#include    "CIDKernel_Time.hpp"
#include    "CIDKernel_Thread.hpp"
#include    "CIDKernel_Console.hpp"
#include    "CIDKernel_USB.hpp"
#include    "CIDKernel_Speech.hpp"
#include    "CIDKernel_IP.hpp"
#include    "CIDKernel_Socket.hpp"
#include    "CIDKernel_SockPinger.hpp"
#include    "CIDKernel_MediaChanger.hpp"
#include    "CIDKernel_TaskScheduler.hpp"



// ---------------------------------------------------------------------------
//  Export the kernel module object.
// ---------------------------------------------------------------------------
extern KRNLEXPORT TCIDKrnlModule kmodCIDKernel;


