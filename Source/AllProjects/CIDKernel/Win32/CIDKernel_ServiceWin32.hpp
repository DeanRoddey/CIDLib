//
// FILE NAME: CIDKernel_ServiceWin32.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/07/2002
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
//  This file provides the platform dependent TKrnlWin32Service namespace,
//  which provides some support code for creating Windows services.
//
//  An internal control handler is implemented, and it will provide simple
//  responses to controls, but the client application is expected to set
//  an instance of a derivative of the TWin32ServiceHandler class which will
//  handle control events in an application specific manner.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


struct SC_HANDLE__;


// ---------------------------------------------------------------------------
//  Some additions to the types namespace for our needs
// ---------------------------------------------------------------------------
namespace tCIDKernel
{
    // -----------------------------------------------------------------------
    //  The possible service operations
    // -----------------------------------------------------------------------
    enum class EWSrvOps
    {
        Continue
        , Interrogate
        , NetBindAdd
        , NetBindDisable
        , NetBindEnable
        , NetBindRemove
        , ParamChange
        , Pause
        , PowerEvent
        , Shutdown
        , Stop

        , Count
        , Unknown
    };


    // -----------------------------------------------------------------------
    //  If the event is a PowerEvent, the special power op callback is called,
    //  and it has a parameter of this type.
    // -----------------------------------------------------------------------
    enum class EWSrvPwrOps
    {
        BatteryLow
        , StatusChange
        , QueryFailed
        , ResumeAuto
        , ResumeCritical
        , Resume
        , Suspend

        , Count
        , Unknown
    };


    // -----------------------------------------------------------------------
    //  The possible service start types
    // -----------------------------------------------------------------------
    enum class EWSrvStarts
    {
        Auto
        , Demand
        , Delayed
        , Disabled
    };


    // -----------------------------------------------------------------------
    //  The possible service states
    // -----------------------------------------------------------------------
    enum class EWSrvStates
    {
        Continue_Pending
        , Pause_Pending
        , Paused
        , Running
        , Start_Pending
        , Stop_Pending
        , Stopped

        , Count
        , Unknown
    };


    // -----------------------------------------------------------------------
    //  A service handle
    // -----------------------------------------------------------------------
    using TWSvcHandle = SC_HANDLE__*;
}


// ---------------------------------------------------------------------------
//   CLASS: TWin32ServiceHandler
//  PREFIX: srvh
// ---------------------------------------------------------------------------
class KRNLEXPORT TWin32ServiceHandler
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TWin32ServiceHandler();

        TWin32ServiceHandler(const TWin32ServiceHandler&) = delete;

        virtual ~TWin32ServiceHandler();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TWin32ServiceHandler& operator=(const TWin32ServiceHandler&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bStartServiceThread() = 0;

        virtual tCIDLib::TBoolean bStopServiceThread() = 0;

        virtual tCIDLib::TBoolean bSuspendRequest() = 0;

        virtual tCIDLib::TVoid ServiceOp
        (
            const   tCIDKernel::EWSrvOps    eOperation
        ) = 0;

        virtual tCIDLib::TVoid ServiceOp
        (
            const   tCIDKernel::EWSrvPwrOps ePowerOp
        ) = 0;

};



// ---------------------------------------------------------------------------
//  NAMESPACE: TKrnlWin32Service
// ---------------------------------------------------------------------------
namespace TKrnlWin32Service
{
    KRNLEXPORT tCIDLib::TBoolean bChangeConfig
    (
        const   tCIDKernel::TWSvcHandle     hsvcToChange
        , const tCIDLib::TCh* const         pszPath
        , const tCIDLib::TCh* const         pszDisplayName
        , const tCIDLib::TCh* const         pszDescription
        , const tCIDKernel::EWSrvStarts     eStartType
        , const tCIDLib::TCh**              apszDepends
        , const tCIDLib::TCard4             c4DepCount
        , const tCIDLib::TCh**              apszParms
        , const tCIDLib::TCard4             c4ParmCount
    );

    KRNLEXPORT tCIDLib::TBoolean bClose
    (
        const   tCIDKernel::TWSvcHandle     hsvcToClose
    );

    KRNLEXPORT tCIDLib::TBoolean bInit
    (
        const   tCIDLib::TCh* const         pszServiceName
        ,       TWin32ServiceHandler* const psrvhToSet
        , const tCIDLib::TBoolean           bDebugMode
    );

    KRNLEXPORT tCIDLib::TBoolean bInService();

    KRNLEXPORT tCIDLib::TBoolean bPleaseWait
    (
        const   tCIDKernel::EWSrvStates     eToSet
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryStartType
    (
        const   tCIDKernel::TWSvcHandle     hsvcSrc
        ,       tCIDKernel::EWSrvStarts&    eToFill
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryState
    (
        const   tCIDKernel::TWSvcHandle     hsvcToQuery
        ,       tCIDKernel::EWSrvStates&    eToFill
    );

    KRNLEXPORT tCIDLib::TBoolean bRemove
    (
        const   tCIDKernel::TWSvcHandle     hsvcToRemove
    );

    KRNLEXPORT tCIDLib::TBoolean bSendOpCode
    (
        const   tCIDKernel::TWSvcHandle     hsvcToTarget
        , const tCIDKernel::EWSrvOps        eToSend
    );

    KRNLEXPORT tCIDLib::TBoolean bSetState
    (
        const   tCIDKernel::EWSrvStates     eToSet
        , const tCIDLib::TCard4             c4WaitHint
        , const tCIDLib::TCard4             c4ExitCode = 0
    );

    KRNLEXPORT tCIDLib::TBoolean bStart
    (
        const   tCIDKernel::TWSvcHandle     hsvcToStart
        , const tCIDLib::TCh**              apszParms
        , const tCIDLib::TCard4             c4ParmCount
        , const tCIDLib::TCard4             c4WaitFor
    );

    KRNLEXPORT tCIDLib::TBoolean bStop
    (
        const   tCIDKernel::TWSvcHandle     hsvcToStop
        , const tCIDLib::TCard4             c4WaitFor
    );

    KRNLEXPORT tCIDKernel::EWSrvStates eCurState();

    KRNLEXPORT tCIDKernel::TWSvcHandle hsvcCreate
    (
        const   tCIDLib::TCh* const         pszPath
        , const tCIDLib::TCh* const         pszServiceName
        , const tCIDLib::TCh* const         pszDisplayName
        , const tCIDLib::TCh* const         pszDescription
        , const tCIDKernel::EWSrvStarts     eStartType
        , const tCIDLib::TCh**              apszDepends
        , const tCIDLib::TCard4             c4DepCount
        , const tCIDLib::TCh**              apszParms
        , const tCIDLib::TCard4             c4ParmCount
    );

    KRNLEXPORT tCIDKernel::TWSvcHandle hsvcOpen
    (
        const   tCIDLib::TCh* const         pszServiceName
    );

    KRNLEXPORT tCIDLib::TVoid RefreshStayAwake();
}


