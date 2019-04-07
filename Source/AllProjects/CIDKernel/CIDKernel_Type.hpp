//
// FILE NAME: CIDKernel_Type.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/06/1996
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
//  This file is the main types file of the facility. Since this is the
//  core facility, it has all of the fundamental stuff. We use the name
//  space system to make this stuff fit into the CIDLib.Dll facility's
//  name spaces, since this facility is really hidden from end users.
//
//  This file only defines the platform independent types, or things that
//  are defined in terms of platform dependent types that are already
//  abstracted. The CIDKernel_PlatformTypes.hpp file (in each per-platform
//  subdirectory) provides the definitions for the types that could change
//  according to platform.
//
//  We also define some stuff for the CIDSock facility here. The low level
//  socket functionality is in the kernel so access to the enums that are
//  part of the abstract socket and TCP/IP interfaces have to be available
//  down here. But (as with the CIDLib dll) the enums need to look like they
//  are part of CIDSock, so we do a tCIDSock namespace here for those enums.
//
//
// CAVEATS/GOTCHAS:
//
//  1)  This file might be used by some simple, external programs that
//      want to interface to some CIDLib data. So it supports the
//      CIDKRNL_NOCLASSES conditional. If its defined, then only non-class
//      stuff is included here.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward reference some types that are used here but not actually defined
//  until later.
// ---------------------------------------------------------------------------
#if !defined(CIDKRNL_NOCLASSES)
class   TObject;
class   TThread;
class   TModule;
class   TPoint;
#endif


namespace tCIDLib
{
    // -----------------------------------------------------------------------
    //  Platform independent types
    //
    //  A Unicode char is for UTF-16 chars. The local short and wide character
    //  representations are driven by the platform driver.
    // -----------------------------------------------------------------------
    using TVoid         = void;
    using THashVal      = TCard4;       // hsh
    using TUniCh        = TCard2;       // uch


    // -----------------------------------------------------------------------
    //  Define some fixed length asciiz string types. They are all one
    //  character larger than their name, to leave space for the nul.
    // -----------------------------------------------------------------------
    using TZStr8    = TCh[9];
    using TZStr16   = TCh[17];
    using TZStr32   = TCh[33];
    using TZStr64   = TCh[65];
    using TZStr80   = TCh[81];
    using TZStr128  = TCh[129];
    using TZStr256  = TCh[257];
    using TZStr512  = TCh[513];
    using TZStr1K   = TCh[1025];



    // -----------------------------------------------------------------------
    //  The adopt/noadopt possiblities
    // -----------------------------------------------------------------------
    enum class EAdoptOpts
    {
        NoAdopt
        , Adopt

        , Count
        , Min           = NoAdopt
        , Max           = Adopt
    };


    // -----------------------------------------------------------------------
    //  The standard read/write access modes. These are mapped internally
    //  to whatever the particular subsystem defines on the OS.
    // -----------------------------------------------------------------------
    enum class EAccessModes
    {
        None                = 0x0
        , Read              = 0x01
        , Write             = 0x02

        , DenyRead          = 0x10
        , DenyWrite         = 0x20

        // Convenient combos
        , ReadWrite         = 0x03
        , Multi_Read        = 0x21
        , Excl_Read         = 0x31
        , Excl_Write        = 0x32
        , Excl_ReadWrite    = 0x33
        , Excl_OutStream    = 0x22
    };


    // -----------------------------------------------------------------------
    //  This is used whenever the calle must indicate whether not reading or
    //  writing all requested bytes is an error or not.
    // -----------------------------------------------------------------------
    enum class EAllData
    {
        OkIfNotAll
        , FailIfNotAll

        , Count
        , Min           = OkIfNotAll
        , Max           = FailIfNotAll
    };


    // -----------------------------------------------------------------------
    //  The types of allocation schemes used by shared memory buffers
    // -----------------------------------------------------------------------
    enum class EAllocTypes
    {
        Commit
        , Lazy

        , Count
    };


    // -----------------------------------------------------------------------
    //  This enum is used to indicate whether angles are in radians or
    //  degrees.
    // -----------------------------------------------------------------------
    enum class EAngleTypes
    {
        Radians
        , Degrees

        , Count
        , Min           = Radians
        , Max           = Degrees
    };


    // -----------------------------------------------------------------------
    //  An option for when we can append something or overwrite existing
    //  info, to make it more self-documenting than a boolean.
    // -----------------------------------------------------------------------
    enum class EAppendOver
    {
        Append
        , Overwrite

        , Count
        , Min           = Append
        , Max           = Overwrite
    };


    // -----------------------------------------------------------------------
    //  When doing some sort of async operation, this is often useful as a
    //  status. Such operations are either idle (none active), in progress,
    //  or has ended and the results need to be dealt with, or has ended with
    //  an error.
    // -----------------------------------------------------------------------
    enum class EAsyncStates
    {
          Idle
        , Working
        , DoneError
        , Done
    };


    // -----------------------------------------------------------------------
    //  The possible types of audible cues that can be requested from an
    //  TAudio object. Everyone does basic audio cues via the facCIDLib facility
    //  object which has a single, public TAudio object. This object can be
    //  replaced by higher level code to say, map the cues to a multimedia
    //  layer (and then add lots of other functionality.) The default version
    //  just uses the speaker.
    //
    //  !These must remain zero based and contiguous!
    // -----------------------------------------------------------------------
    enum class EAudioCues
    {
          Alert
        , Bell
        , Click
        , Chime
        , Error
        , HiPulse
        , Information
        , LowPulse
        , MedPulse
        , Question
        , Rising
        , Sinking
        , Warning

        , Count
        , Min            = Alert
        , Max            = Warning
    };


    // -----------------------------------------------------------------------
    //  Some audio formats used in some audio rendering operations, such as
    //  text to WAV via the speech class.
    // -----------------------------------------------------------------------
    enum class EAudioFmts
    {
        Default
        , F8Khz16BitMono
        , F8Khz16BitStereo
        , F12Khz16BitMono
        , F12Khz16BitStereo
        , F16Khz16BitMono
        , F16Khz16BitStereo
        , F22Khz16BitMono
        , F22Khz16BitStereo
        , F44Khz16BitMono
        , F44Khz16BitStereo
        , F48Khz16BitMono
        , F48Khz16BitStereo
    };


    // -----------------------------------------------------------------------
    //  The possible settings of an auto/manual flag
    // -----------------------------------------------------------------------
    enum class EAutoModes
    {
        Manual
        , Automatic

        , Count
        , Min           = Manual
        , Max           = Automatic
    };


    // -----------------------------------------------------------------------
    //  These are the basic text formats thta can exist. Everything fits into
    //  one of these rough categories.
    // -----------------------------------------------------------------------
    enum class EBaseTextFmts
    {
        SingleByte
        , MultiByte
        , TwoByte_BE
        , TwoByte_LE
        , FourByte_BE
        , FourByte_LE

        , Count
        , Min           = SingleByte
        , Max           = FourByte_LE
    };


    // -----------------------------------------------------------------------
    //  General purpose, for use by 'which is better' type algorithms.
    // -----------------------------------------------------------------------
    enum class EBestRes
    {
        FirstBetter
        , SecBetter
        , FirstOptimal
        , SecOptimal
    };


    // -----------------------------------------------------------------------
    //  Used by the TBitSeq class
    // -----------------------------------------------------------------------
    enum class EBitSeqs
    {
        Normal
        , DupCompl
        , InterleavedCompl
    };


    // -----------------------------------------------------------------------
    //  The state flags for a two state object
    // -----------------------------------------------------------------------
    enum class EBiStates
    {
        Off
        , On

        , Count
        , Min           = Off
        , Max           = On
    };


    // -----------------------------------------------------------------------
    //  This is an enum for the very common scenario where something can either have
    //  changed, not have changed, or not have a valid value.
    // -----------------------------------------------------------------------
    enum class EChangeRes
    {
        None
        , New
        , Error

        , Count
        , Min           = None
        , Max           = Error
    };


    // -----------------------------------------------------------------------
    //  The various color components
    // -----------------------------------------------------------------------
    enum class EClrComps
    {
        Red
        , Green
        , Blue
        , Alpha
        , Gray
    };


    // -----------------------------------------------------------------------
    //  The standard set of replies from most kinds of 'do you want to do it'
    //  questions.
    // -----------------------------------------------------------------------
    enum class ECommitOps
    {
        Cancel
        , No
        , Yes
    };


    // -----------------------------------------------------------------------
    //  The possible corners of a rectangle
    // -----------------------------------------------------------------------
    enum class ECorners
    {
        UL
        , LL
        , LR
        , UR

        , Count
        , Min           = UL
        , Max           = UR
    };


    // -----------------------------------------------------------------------
    //  The sides of a client/server connection
    // -----------------------------------------------------------------------
    enum class ECSSides
    {
        Client
        , Server
        , Both

        , Count
        , Min           = Client
        , Max           = Both
    };


    // -----------------------------------------------------------------------
    //  Useful as a status for connecting to somethere where you might already be
    //  connected and need to just use that existing connection.
    // -----------------------------------------------------------------------
    enum class EConnRes
    {
        Failed
        , AlreadyConnected
        , Connected
    };


    // -----------------------------------------------------------------------
    //  The CPU types that we might find outselves running on, for reporting
    //  back in system information.
    // -----------------------------------------------------------------------
    enum class ECPUTypes
    {
        Unknown
        , Alpha
        , Alpha64
        , Arm
        , Intel32
        , Intel64
        , MIPS
        , PPC
        , SPARC

        , Count
        , Min           = Unknown
        , Max           = SPARC
    };


    // -----------------------------------------------------------------------
    //  These are flags used when creating or opening files or other
    //  resources that may already exist. It tells the open method how to
    //  react to an existing or non-existent resource.
    // -----------------------------------------------------------------------
    enum class ECreateActs
    {
        None
        , CreateIfNew
        , CreateAlways
        , OpenIfExists
        , OpenOrCreate
        , TruncateExisting

        , Count
        , Min        = None
        , Max        = TruncateExisting
    };


    // -----------------------------------------------------------------------
    //  This is used to indicate delete options to any methods that require
    //  such control.
    // -----------------------------------------------------------------------
    enum class EDelModes
    {
        NoDelete
        , Default
        , Delete

        , Count
        , Min           = NoDelete
        , Max           = Delete
    };


    // -----------------------------------------------------------------------
    //  The compass directions
    // -----------------------------------------------------------------------
    enum class EDirs
    {
        Left
        , Right
        , Down
        , Up

        , Count
        , Min               = Left
        , Max               = Up
    };


    // -----------------------------------------------------------------------
    //  Directory change filters used by the TKrnlDirChangeMon stuff, and the
    //  change flags reported, and a return value used in the changes read call.
    //
    //  We have a special one for our internal use by which we report an out of
    //  sync error, in which case the caller has to manually resync.
    // -----------------------------------------------------------------------
    enum class EDirChanges
    {
        None
        , Added
        , Removed
        , Modified
        , Renamed
        , OutOfSync
    };

    enum class EDirChFilters
    {
        None                = 0
        , FileName          = 0x00000001
        , DirName           = 0x00000002
        , Size              = 0x00000008
        , LastWrite         = 0x00000010
        , LastAccess        = 0x00000020
        , Creation          = 0x00000040

        , AllWrite          = 0x0000005B
        , All               = 0x0000007B
    };


    // -----------------------------------------------------------------------
    //  These flags are used in directory searches and indicate what types
    //  of entries the caller wants to see.
    // -----------------------------------------------------------------------
    enum class EDirSearchFlags
    {
        FindDirs            = 0x00000001
        , NormalDirsOnly    = 0x00000002
        , FindFiles         = 0x00000004
        , NormalFilesOnly   = 0x00000008

        // Use these, not the individual flags above!
        , All               = 0x00000005
        , AllNormal         = 0x0000000F
        , NormalFiles       = 0x0000000C
        , NormalDirs        = 0x00000003
        , AllFiles          = 0x00000004
        , AllDirs           = 0x00000001
    };


    // ------------------------------------------------------------------------
    //  The possible states for things that can emphasized or not.
    // ------------------------------------------------------------------------
    enum class EEmphases
    {
        Off
        , On

        , Count
        , Min           = Off
        , Max           = On
    };


    // -----------------------------------------------------------------------
    //  The possible enabled states
    // -----------------------------------------------------------------------
    enum class EEnableStates
    {
        Disabled
        , Enabled

        , Count
        , Min           = Disabled
        , Max           = Enabled
    };


    // -----------------------------------------------------------------------
    //  The possible 'endian' modes of the host machine
    // -----------------------------------------------------------------------
    enum class EEndianModes
    {
        Little
        , Big

        , Count
        , Min           = Little
        , Max           = Big
    };


    // -----------------------------------------------------------------------
    //  The possible ends of any two ended thing
    // -----------------------------------------------------------------------
    enum class EEnds
    {
        First
        , Last

        , Count
        , Min           = First
        , Max           = Last
    };


    // -----------------------------------------------------------------------
    //  The possible error classes
    //
    //  NOTE:   This must remain contiguous and coordinated with the xlat
    //          methods in tCIDLib, since we can't use the IDL compiler to
    //          generate them at this level.
    // -----------------------------------------------------------------------
    enum class EErrClasses
    {
        None
        , OutResource
        , TempSituation
        , Authority
        , Internal
        , HardwareFailure
        , SystemFailure
        , AppError
        , BadParms
        , Locked
        , Media
        , Already
        , Unknown
        , CantDo
        , Timeout
        , DynamicType
        , TypeMatch
        , Format
        , UserInput
        , NotReady
        , ObjState
        , NotSupported
        , AppStatus
        , NotFound
        , Protocol
        , Range
        , Duplicate
        , Config
        , Index
        , NotAllRead
        , NotAllWritten
        , LostConnection
        , Overflow
        , Underflow
        , InitFailed
        , OutOfSync
        , XmitFailed
        , ReceiveFailed
        , NoReply
        , Assert
        , InUse
        , Term
        , Init
        , Shutdown
        , RejectedConn
        , Busy
        , Nak

        , Count
        , Min                   = None
        , Max                   = Nak
    };


    // -----------------------------------------------------------------------
    //  The states of an event
    // -----------------------------------------------------------------------
    enum class EEventStates
    {
        Reset
        , Triggered

        , Count
        , Min           = Reset
        , Max           = Triggered
    };


    // -----------------------------------------------------------------------
    //  The standard exit codes that CIDLib will use, and which are
    //  recommended for CIDLib apps. All the lower numbers are left open
    //  for user use (just cast your value to this type.)
    //
    //  NOTE:   This must be kept in sync with CIDLib_Type.cpp class which
    //          provides the code to translate between the text and enum
    //          versions.
    // -----------------------------------------------------------------------
    enum class EExitCodes
    {
        Normal              = 0

        , Deschedule        = 114
        , AdminStop         = 115
        , ConfigError       = 116
        , InternalError     = 117
        , AdminRestart      = 118
        , FatalError        = 119
        , RuntimeError      = 120
        , SystemException   = 121
        , BadParameters     = 122
        , ResourceAccess    = 123
        , PermissionLevel   = 124
        , NotFound          = 125
        , InitFailed        = 126
        , BadEnvironment    = 127

        , FirstNamed        = 114
        , LastNamed         = 127
        , Unknown           = 128
        , Count             = tCIDLib::TCard4(LastNamed - FirstNamed) + 1
    };


    // -----------------------------------------------------------------------
    //  The flags used to invoke an external process.
    // -----------------------------------------------------------------------
    enum class EExtProcFlags
    {
        None                = 0x00000000
        , Detached          = 0x00000001
        , UseStdHandles     = 0x00000002
        , NewProcGroup      = 0x00000004
        , WaitFor           = 0x00000008

        , RedirCmdLn        = 0x0000000A
    };

    enum class EExtProcShows
    {
        Hidden
        , Normal
        , NormNoActivate
        , Maximized
        , Minimized
        , MinNoActivate
    };


    // -----------------------------------------------------------------------
    //  The flags that indicate access permissions that are given to newly
    //  created files. These are supported as best as possible under each
    //  platform.
    // -----------------------------------------------------------------------
    enum class EFilePerms : tCIDLib::TCard2
    {
        None                = 0
        , OwnerRead         = 0x0001
        , OwnerWrite        = 0x0002
        , OwnerExecute      = 0x0004
        , OwnerRW           = 0x0003
        , AllOwnerAccess    = 0x0007

        , GroupRead         = 0x0010
        , GroupWrite        = 0x0020
        , GroupExecute      = 0x0040
        , GroupRW           = 0x0030
        , AllGroupAccess    = 0x0070

        , WorldRead         = 0x0100
        , WorldWrite        = 0x0200
        , WorldExecute      = 0x0400
        , WorldRW           = 0x0300
        , AllWorldAccess    = 0x0700

        , AllAccess         = 0x0777
        , Default           = 0x0117
        , AllRead           = 0x0111
        , AllWrite          = 0x0222
        , AllRW             = 0x0333
        , AllWE             = 0x0666
        , Standard          = 0x0711
        , Standard2         = 0x0733
        , Standard3         = 0x0755
    };


    // -----------------------------------------------------------------------
    //  The standard file control flags, which control various attributes of
    //  an opened or created file.
    //
    //  Random and Sequential are mutually exclusive. Absense of both implies
    //  no overriding access pattern. The 'SaveStream' combo is used when the
    //  file is used as storage for a stream (which are always sequential) and
    //  needs to be sure to be written. This is common enough to want a combo
    //  just for that.
    // -----------------------------------------------------------------------
    enum class EFileFlags : tCIDLib::TCard4
    {
        None                = 0x0
        , WriteThrough      = 0x00000001
        , RandomAccess      = 0x00000002
        , SequentialScan    = 0x00000004
        , Overlapped        = 0x00000008

        , SafeStream        = 0x00000005
        , USBDevice         = 0x00000009
    };


    // -------------------------------------------------------------------
    //  The are the flags that are returned in the directory search
    //  operations.
    //
    //  The _CanXXX flags indicate what the calling process can do. If
    //  that is purely driven by file attributes, then it will return the
    //  same thing for all processes. On some systems, it will be driven
    //  by the access rights of the process.
    //
    //  The _IsXXXX flags indicate attributes of the file. IsDirectory
    //  indicates if its a directory. If not, then its a file. IsNormal
    //  indicates whether its a 'normal' directory or file. For a
    //  directory, normal means is not one of the '.' or '..' directories
    //  under Win32 for instance. For a file, this means it does not for
    //  instance have the Win32 System or Hidden attributes.
    // -------------------------------------------------------------------
    enum class EFileInfoFlags : tCIDLib::TCard4
    {
        None                = 0x00000000
        , CanRead           = 0x00000001
        , CanWrite          = 0x00000002
        , CanExecute        = 0x00000004

        , IsDirectory       = 0x00000100
        , IsNormal          = 0x00000200
    };


    // -----------------------------------------------------------------------
    //  The force/noforce type of option that many functions support, to
    //  either force some to happen that otherwise would not, or to not force.
    // -----------------------------------------------------------------------
    enum class EForceOpts
    {
        NoForce
        , Force

        , Count
        , Min           = NoForce
        , Max           = Force
    };


    // -----------------------------------------------------------------------
    //  A common set of flags used by code that search a list for something and will
    //  want to possible throw if it either already exists or doesn't exist.
    // -----------------------------------------------------------------------
    enum class EFindFlags
    {
        None
        , ThrowIfNotFnd
        , ThrowIfExists
    };


    // -----------------------------------------------------------------------
    //  A common result for some types of search operations. It can be found,
    //  not found but more possible finds, or not found and no more possible
    //  finds.
    // -----------------------------------------------------------------------
    enum class EFindRes
    {
        Found
        , NotFound
        , NoMore
    };


    // -----------------------------------------------------------------------
    //  Another possibly common find operation.
    //
    //  Error       - An error occurred trying to find it
    //  NotFound    - It was not found
    //  NoMatch     - It was found but wasn't acceptable
    //  Default     - It was not found, but defaults were returned
    //  Found       - It was found and was acceptable
    //
    //  They are in order of increasing preference basically
    // -----------------------------------------------------------------------
    enum class EFindRes2
    {
        Error
        , NotFound
        , NoMatch
        , Default
        , Found
    };


    // -----------------------------------------------------------------------
    //  Horizontal and vertical justification
    // -----------------------------------------------------------------------
    enum class EHJustify
    {
        Left
        , Right
        , Center

        , Count
        , Min               = Left
        , Max               = Center
    };

    enum class EVJustify
    {
        Bottom
        , Center
        , Top

        , Count
        , Min               = Bottom
        , Max               = Top
    };


    // -----------------------------------------------------------------------
    //  The possible horizontal/vertical orientations.
    // -----------------------------------------------------------------------
    enum class EHVOrients
    {
        EHVOrient_Horizontal
        , EHVOrient_Vertical

        , EHVOrients_Count
        , EHVOrients_Min    = 0
        , EHVOrients_Max    = EHVOrients_Count - 1
    };


    // -----------------------------------------------------------------------
    //  The values for any init/term flag
    // -----------------------------------------------------------------------
    enum class EInitTerm
    {
        Terminate
        , Initialize

        , Count
        , Min             = Terminate
        , Max             = Initialize
    };


    // -----------------------------------------------------------------------
    //  The well known IP ports
    // -----------------------------------------------------------------------
    enum class EIPPorts
    {
        Echo                = 7
        , Discard           = 9
        , SysStatus         = 11
        , DayTime           = 13
        , NetStat           = 15
        , FTP               = 21
        , TelNet            = 23
        , SMTP              = 25
        , TimeServer        = 37
        , NameServer        = 42
        , WhoIs             = 43
        , MTP               = 57
        , TFTP              = 69
        , RJE               = 77
        , Finger            = 79
        , TTYLink           = 87
        , SupDup            = 95
    };


    // ------------------------------------------------------------------------
    //  The available heap checking levels. Used by TKrnlMemCheck.
    // ------------------------------------------------------------------------
    enum class EHeapChkLevels
    {
        Off
        , Low
        , Medium
        , High
        , Max
    };


    // -----------------------------------------------------------------------
    //  The languages supportable by CIDLib. This is not everything, but
    //  its a quite reasonable set of languages and more than enough of a
    //  challenge.
    //
    //  Also documented here are the initials that each language maps to.
    //  These initials are suffixed onto the MsgText and CIDMsg files of
    //  CIDLib to indicate the language that they contain. And they are used
    //  on the command line of some utilities to indicate which language to
    //  operate on.
    //
    //  !!!!This MUST be kept in sync with the CIDBuild utility. See the
    //      CIDBuild_SyncedData.hpp file where all such data lives in the
    //      build utility.
    //
    //      And it must be kept in sync with the TFacCIDLib class, which has
    //      the code for translating between the enum and text formats.
    // -----------------------------------------------------------------------
    enum class ELanguages
    {
        Afrikaans         // af
        , Albanian        // al
        , Arabic          // ar
        , Basque          // ba
        , Belarusian      // be
        , Bulgarian       // bu
        , Catalan         // ca
        , Chinese         // ch
        , Croatian        // cr
        , Czech           // cz
        , Danish          // da
        , Dutch           // de
        , English         // en
        , Farsi           // fa
        , Finnish         // fi
        , French          // fr
        , German          // ge
        , Greek           // gr
        , Hebrew          // he
        , Hungarian       // hu
        , Icelandic       // ic
        , Indonesian      // in
        , Italian         // it
        , Japanese        // ja
        , Korean          // ko
        , Latvian         // la
        , Lithuanian      // li
        , Norwegian       // no
        , Polish          // po
        , Portuguese      // pr
        , Romanian        // ro
        , Russian         // ru
        , Serbian         // se
        , Slovak          // sl
        , Slovenian       // sv
        , Spanish         // sp
        , Swedish         // sw
        , Turkish         // tu
        , Ukrainian       // uk
        , Vietnamese      // vi

        , Count
        , Min           = Afrikaans
        , Max           = Vietnamese

        , Default       = 0xFFFF
    };


    // -------------------------------------------------------------------
    //  Used when reading lines of text, which is done in various place, and often
    //  from online sources that might timeout.
    // -------------------------------------------------------------------
    enum class ELineRes
    {
        GotLine
        , EmptyLine
        , Timeout
        , Partial
    };


    // -----------------------------------------------------------------------
    //  We have various methods that load up something and either don't find
    //  it, find it but nothing has changed, or find it and it's new data.
    // -----------------------------------------------------------------------
    enum class ELoadRes
    {
        NotFound
        , NoNewData
        , NewData

        , Count
        , Min               = NotFound
        , Max               = NewData
    };


    // -----------------------------------------------------------------------
    //  The state of a locked thing, either locked or unlocked. Its set up
    //  so that unlocked is 0 and locked is non-zero.
    // -----------------------------------------------------------------------
    enum class ELockStates
    {
        Unlocked
        , Locked

        , Count
        , Min               = Unlocked
        , Max               = Locked
    };


    // -----------------------------------------------------------------------
    //  The ways that a TLogMap object will style its look up table. If
    //  its ELogMapMode_Spread, then the lower indexes are spread out
    //  smoothly, up until the numbers start hitting more than one input
    //  index per output index.
    // -----------------------------------------------------------------------
    enum class ELogMapModes
    {
        Regular
        , Spread

        , Count
        , Min           = Regular
        , Max           = Spread
    };


    // -----------------------------------------------------------------------
    //  A common search matching option scheme
    // -----------------------------------------------------------------------
    enum class EMatchTypes
    {
        StartsWith
        , Contains
        , Equals

        , Count
    };


    // -----------------------------------------------------------------------
    //  The measuring systems reported by the locale information
    // -----------------------------------------------------------------------
    enum class EMeasures
    {
        Unknown
        , Metric
        , US
    };


    // -----------------------------------------------------------------------
    //  Memory buffer access rights and allocation flags. System allocated
    //  memory supports these, not heap allocated memory.
    // -----------------------------------------------------------------------
    enum class EMemAccFlags : tCIDLib::TCard4
    {
        None                = 0x00000001UL
        , ReadOnly          = 0x00000002UL
        , ReadWrite         = 0x00000004UL
        , Execute           = 0x00000010UL
        , ExecuteRead       = 0x00000020UL
        , ExecuteReadWrite  = 0x00000040UL
        , Guard             = 0x00000100UL
    };


    // -----------------------------------------------------------------------
    //  The states of memory regions
    // -----------------------------------------------------------------------
    enum class EMemStates : tCIDLib::TCard4
    {
        Commit              = 0x00001000UL
        , Reserved          = 0x00002000UL
        , Free              = 0x00010000UL
    };


    // -----------------------------------------------------------------------
    //  The types of memory regions
    // -----------------------------------------------------------------------
    enum class EMemTypes : tCIDLib::TCard4
    {
        Private             = 0x00200000UL
        , Mapped            = 0x00400000UL
        , Image             = 0x01000000UL
    };


    // -----------------------------------------------------------------------
    //  The module flags, which are used when loading load DLL and Exe
    //  modules.
    // -----------------------------------------------------------------------
    enum class EModFlags : tCIDLib::TCard4
    {
        None                = 0x0UL
        , HasMsgFile        = 0x00000001UL
        , HasResFile        = 0x00000002UL

        , HasMsgsAndRes     = 0x00000003UL
    };


    // -----------------------------------------------------------------------
    //  These are the possible loadable module types.
    // -----------------------------------------------------------------------
    enum class EModTypes
    {
        Dll
        , Exe

        , Count
        , Min               = Dll
        , Max               = Exe
    };


    // -----------------------------------------------------------------------
    //  An enumeration for the months of the year
    //
    //  !!These MUST remain zero based and contiguous!
    // -----------------------------------------------------------------------
    enum class EMonths
    {
        January
        , February
        , March
        , April
        , May
        , June
        , July
        , August
        , September
        , October
        , November
        , December

        , Count
        , Min               = January
        , Max               = December
    };


    // -----------------------------------------------------------------------
    //  The possible safe/unsafe states for anything that can be optionally
    //  threadsafe.
    // -----------------------------------------------------------------------
    enum class EMTStates
    {
        Unsafe
        , Safe
    };


    // -----------------------------------------------------------------------
    //  The types of named resources
    // -----------------------------------------------------------------------
    enum class ENamedRscTypes
    {
        Event
        , Memory
        , Mutex
        , Semaphore

        , Count
        , Min           = Event
        , Max           = Semaphore
    };


    // -----------------------------------------------------------------------
    //  The types of line ends spit out by text output streams. They will
    //  convert all CR/LF/CR-LF sequences to this sequence.
    // -----------------------------------------------------------------------
    enum class ENewLineTypes
    {
        CR
        , LF
        , CRLF
    };


    // -----------------------------------------------------------------------
    //  For use whenever there are next/previous semantics
    // -----------------------------------------------------------------------
    enum class ENextPrev
    {
        Next
        , Previous

        , Count
    };


    // -----------------------------------------------------------------------
    //  Represents open/closed states
    // -----------------------------------------------------------------------
    enum class EOpenStates
    {
        Closed
        , Open

        , Count
        , Min           = Closed
        , Max           = Open
    };


    // -----------------------------------------------------------------------
    //  Returned from things that allow you to open something existing,
    //  create something new, or cancel.
    // -----------------------------------------------------------------------
    enum class EOpenRes
    {
        Cancel
        , NewFile
        , ExistingFile
    };


    // -----------------------------------------------------------------------
    //  Parameter directions
    // -----------------------------------------------------------------------
    enum class EParmDirs
    {
        In
        , Out
        , InOut

        , Count
    };


    // -----------------------------------------------------------------------
    //  The parts of a file system path
    // -----------------------------------------------------------------------
    enum class EPathParts
    {
        Path                = 0x00000001UL
        , Name              = 0x00000002UL
        , Extension         = 0x00000004UL
        , Volume            = 0x00010000UL

        , NameExt           = 0x00000006UL
        , FullPath          = 0x00010007UL
    };


    // --------------------------------------------------------------------
    //  Pressed/released states, which are likely to be commonly used
    // --------------------------------------------------------------------
    enum class EPressStates
    {
        Released
        , Pressed

        , Count
        , Min           = Released
        , Max           = Pressed
    };


    // -----------------------------------------------------------------------
    //  The available priority classes for processes.
    // -----------------------------------------------------------------------
    enum class EPrioClasses
    {
        IdleTime
        , Normal
        , High
        , RealTime

        , Count
        , Min           = IdleTime
        , Max           = RealTime
    };


    // -----------------------------------------------------------------------
    //  The available thread priority levels.
    // -----------------------------------------------------------------------
    enum class EPrioLevels
    {
        Lowest
        , BelowNormal
        , Normal
        , AboveNormal
        , Highest

        , Count
        , Min           = Lowest
        , Max           = Highest
    };


    // -----------------------------------------------------------------------
    //  The valid priorities for an element in a queue collection.
    // -----------------------------------------------------------------------
    enum class EQPrios
    {
        P0
        , P1
        , P2
        , P3
        , P4
        , P5
        , P6
        , P7
        , P8
        , P9
        , P10
        , P11
        , P12
        , P13
        , P14
        , P15

        , Count
        , Min           = P0
        , Max           = P15
    };


    // -----------------------------------------------------------------------
    //  Some classes that provide a data source can support a mode where new
    //  data is destructively read out, or just read and left in place.
    // -----------------------------------------------------------------------
    enum class EReadModes
    {
        NonDestructive
        , Destructive
    };


    // -----------------------------------------------------------------------
    //  The basic radices of numbers. These are the ones that are supported
    //  by the CIDLib system for formatting values.
    // -----------------------------------------------------------------------
    enum class ERadices
    {
        Auto                = 0
        , Bin               = 2
        , Oct               = 8
        , Dec               = 10
        , Hex               = 16
    };


    // -----------------------------------------------------------------------
    //  The possible states of a value within a range. Its either at the min
    //  or one of the max values, or somewhere in between. None is first so
    //  that being at either end is non-zero.
    // -----------------------------------------------------------------------
    enum class ERangeStates
    {
        None
        , MinValue
        , LegalMax
        , MaxValue

        , Count
        , Min               = None
        , Max               = MaxValue
    };


    // -----------------------------------------------------------------------
    //  The recoverable states of something that can be recoverable or
    //  fatal.
    // -----------------------------------------------------------------------
    enum class ERecoverable
    {
        Recoverable
        , Unrecoverable

        , Count
        , Min           = Recoverable
        , Max           = Unrecoverable
    };


    // -----------------------------------------------------------------------
    //  These are the possible inclusive/non-inclusive states of a
    //  rectangle
    // -----------------------------------------------------------------------
    enum class ERectlTypes
    {
        Inclusive
        , NonInclusive

        , Count
        , Min           = Inclusive
        , Max           = NonInclusive
    };


    // -----------------------------------------------------------------------
    //  The available kinds of access to a registry key
    // -----------------------------------------------------------------------
    enum class ERegAccessModes : tCIDLib::TCard4
    {
        QueryValue      = 0x00000001
        , SetValue      = 0x00000002
        , CreateSub     = 0x00000004
        , EnumSubKeys   = 0x00000008
        , Read          = 0x00000010
        , Notify        = 0x00000020
        , CreateLink    = 0x00000040
        , Write         = 0x00000082
        , All           = 0x00000100
    };


    // -----------------------------------------------------------------------
    //  A result that can be returned from rename operations to indicate
    //  success or the common errors.
    // -----------------------------------------------------------------------
    enum class ERenameRes
    {
        OK
        , AlreadyExists
        , NotFound
        , BadTargetDir
    };


    // -----------------------------------------------------------------------
    //  Used for those scenarios where something is either not used, used but
    //  not required, or required, which is pretty common.
    // -----------------------------------------------------------------------
    enum class EReqOpts
    {
        Unused
        , Optional
        , Required

        , Count
        , Min           = Unused
        , Max           = Required
    };


    // -----------------------------------------------------------------------
    //  The types of (non-text) resources. These are really mostly for GUI apps,
    //  but some non-GUI facilities may want to load these resources (which are
    //  just data blobs at the TModule level where they are loaded, so this doesn't
    //  imply that the actual format of the data is understood at the CIDLib
    //  level, which they aren't.)
    //
    //  Add new ones at the end since these are persisted in resource files!
    // -----------------------------------------------------------------------
    enum class EResTypes
    {
        Bitmap
        , Dialog
        , Icon
        , Menu
        , PNG
        , JPEG

        , Count
        , Min           = Bitmap
        , Max           = JPEG
    };


    // -----------------------------------------------------------------------
    //  Used by floating point rounding among other things
    // -----------------------------------------------------------------------
    enum class ERoundTypes
    {
        Closest
        , Up
        , Down
    };


    // -----------------------------------------------------------------------
    //  A basic read/write access bitmapped enum
    // -----------------------------------------------------------------------
    enum class ERWAccess
    {
        None            = 0x0
        , Read          = 0x01
        , Write         = 0x02

        // Convenient combos
        , ReadWrite     = 0x03
    };


    // -----------------------------------------------------------------------
    //  This is an enum that provides a return status for the common scenario where you
    //  want to report that the thing being saved either has not changed, is ok and can
    //  be saved, or it has errors. So it's often used in a 'validate' type method prior
    //  to saving the content.
    // -----------------------------------------------------------------------
    enum class ESaveRes
    {
        NoChanges
        , OK
        , Errors
    };


    // -----------------------------------------------------------------------
    //  The severity levels of an error
    //
    //  NOTE:   This must remain contiguous and coordinated with the TFacCIDLib
    //          class since it is used to index an array for text to enum
    //          translation.
    // -----------------------------------------------------------------------
    enum class ESeverities
    {
        Info
        , Warn
        , Failed
        , ProcFatal
        , SysFatal
        , Status

        , Count
        , Min           = Info
        , Max           = Status
    };


    // -----------------------------------------------------------------------
    //  The shared state of any kind any shared resource
    // -----------------------------------------------------------------------
    enum class EShareStates
    {
        Unshared
        , Shared

        , Count
        , Min       = Unshared
        , Max       = Shared
    };


    // -----------------------------------------------------------------------
    //  The signals supported by TKrnlSignalHandler
    // -----------------------------------------------------------------------
    enum class ESignals
    {
        Unknown             = 0
        , CtrlC             = 1
        , Break             = 2
        , CloseConsole      = 3
        , LogOff            = 4
        , Shutdown          = 5
    };


    // -----------------------------------------------------------------------
    //  The result of a sort comparison operation. Sorts need to know the
    //  relative position of two elements to each other.
    //
    //  Because this one is used in sorting algorithsm, we will provide a global
    //  negation operator for it below, so that returns of this sort can be
    //  not'ed.
    // -----------------------------------------------------------------------
    enum class ESortComps
    {
        FirstLess           = -1
        , Equal             = 0
        , FirstGreater      = 1
    };


    // -----------------------------------------------------------------------
    //  Used when sorting things. Indicates the direction of the sort
    // -----------------------------------------------------------------------
    enum class ESortDirs
    {
        Ascending
        , Descending

        , Count
        , Min           = Ascending
        , Max           = Descending
    };


    // -----------------------------------------------------------------------
    //  Used by the system info class to allow query of a set of special
    //  system paths.
    // -----------------------------------------------------------------------
    enum class ESpecialPaths
    {
        CommonAppData
        , CommonDocs
        , ProgramFiles
        , ProgramFilesCommon
        , SystemPath
        , CommonMenuPrograms
        , CommonStartMenu
        , CommonStartup

        // From here on out, they aren't valid in a service
        , UserAppData
        , MyMusic
        , MyPictures
        , UserFavorites
        , UserMenuPrograms
        , UserMenuStartup
        , UserStartMenu

        , Count
        , Min           = CommonAppData
        , Max           = UserStartMenu
        , FirstUser     = UserAppData
    };


    // -----------------------------------------------------------------------
    //  Used by the time classes to construct to particular special times.
    // -----------------------------------------------------------------------
    enum class ESpecialTimes
    {
        Base
        , CurrentDate
        , CurrentTime
        , NoonToday
        , CurrentUTC

        , Count
        , Min           = Base
        , Max           = CurrentUTC
    };


    // -----------------------------------------------------------------------
    //  Flags used by the speech support class
    // -----------------------------------------------------------------------
    enum class ESpeechFlags
    {
        None            = 0x0000
        , Async         = 0x0001
        , Markup        = 0x0002

        , AsyncMarkup   = 0x0003
    };


    // -----------------------------------------------------------------------
    //  The values for any start/end flag
    // -----------------------------------------------------------------------
    enum class EStartEnd
    {
        Start
        , End

        , Count
        , Min           = Start
        , Max           = End
    };


    // -----------------------------------------------------------------------
    //  Used in the TStatsCache system, to indicate how to interpret a given
    //  stats item. They are all Card8 values, but this gives viewers of the
    //  stats some hit as to how they might be displayed.
    // -----------------------------------------------------------------------
    enum class EStatItemTypes
    {
        BitFlags
        , Counter
        , Flag
        , Percent
        , Time
        , Value
    };


    // -----------------------------------------------------------------------
    //  The standard files that can be opened without an explicit name.
    // -----------------------------------------------------------------------
    enum class EStdFiles
    {
        StdOut
        , StdIn
        , StdErr

        , Count
    };


    // -----------------------------------------------------------------------
    //  These are used to put special string combinations or cause actions
    //  on text or binary streams.
    // -----------------------------------------------------------------------
    enum class EStrmSpecial : tCIDLib::TCard2
    {
        FlushIt             = 0x1000
    };

    enum class ETStrmSpecial : tCIDLib::TCard2
    {
        DblNewLine          = 0x2001
        , EndLine           = 0x2002
        , NewLine           = 0x2003
        , NewEndLine        = 0x2004

        , Min               = DblNewLine
        , Max               = NewEndLine
    };


    // -----------------------------------------------------------------------
    //  For string stripping methods, indicates where the stripping should
    //  occur and how extreme it should be. Used by TString and TRawStr.
    // -----------------------------------------------------------------------
    enum class EStripModes : tCIDLib::TCard2
    {
        None            = 0x0000
        , Leading       = 0x0001
        , Trailing      = 0x0002
        , LeadTrail     = 0x0003
        , Middle        = 0x0004
        , Total         = 0x0008

        , Full          = 0x0007
        , Complete      = 0x000F

        , Count         = 6
        , All           = 0x000F
        , Reserve       = 0xFFFF
    };


    // -----------------------------------------------------------------------
    //  These are used by streams to put special marker values into the data. These
    //  are single bytes. Even if the compiler ignored the type specifier, the
    //  binary in/out stream classes force it to Card1.
    // -----------------------------------------------------------------------
    enum class EStreamMarkers : tCIDLib::TCard1
    {
        None            = 0
        , TypeName      = 0xFA
        , EndObject     = 0xFB
        , StartObject   = 0xFC
        , Frame         = 0xFD

        , Min           = 0xFA
        , Max           = 0xFD
    };


    // -----------------------------------------------------------------------
    //  When we need a sync/async flag
    // -----------------------------------------------------------------------
    enum class ESyncTypes
    {
        Synchronous
        , Asynchronous
    };


    // -----------------------------------------------------------------------
    //  The possible symmetries that a quadrant oriented system can have.
    // -----------------------------------------------------------------------
    enum class ESymmetries
    {
        None
        , VerticalFlip
        , HorizontalFlip
        , QuadrantFlip

        , Count
        , Min           = None
        , Max           = QuadrantFlip
    };


    // -----------------------------------------------------------------------
    //  The flags used by the TKrnlExtProcess for system escape
    // -----------------------------------------------------------------------
    enum class ESysEscFlags
    {
        None            = 0
        , Wait          = 0x0001
        , NewWindow     = 0x0002
        , StayOpen      = 0x0004

        , Count
        , Min           = None
        , Max           = StayOpen

        , DropOut       = 0x0005
        , StartNew      = 0x0006
        , StartNewWait  = 0x0007
    };


    // -----------------------------------------------------------------------
    //  The supported reasons for a system exception
    // -----------------------------------------------------------------------
    enum class ESysExcepts
    {
        Unknown

        , BreakPoint
        , DataAlignment
        , Float_Denormal
        , Float_DivByZero
        , Float_InexactResult
        , Float_InvalidOp
        , Float_Overflow
        , Float_Stack
        , Float_Underflow
        , Int_DivByZero
        , Int_Overflow
        , PageLoadFailed
        , ReadAccess
        , StackOverflow
        , WriteAccess

        , Count
        , Min           = Unknown
        , Max           = WriteAccess
    };


    // -----------------------------------------------------------------------
    //  The supported responses to a system exception.
    // -----------------------------------------------------------------------
    enum class ESysExceptRes
    {
        Dismiss
        , Propogate
        , ExecuteHandler
    };


    // -----------------------------------------------------------------------
    //  These flags are used in text encoders, to indicate what actions to
    //  take in some circumstances.
    //
    //  Note that Replace is ignored when converting outwards and is treated
    //  like Throw. It's only used when internalizing text.
    // -----------------------------------------------------------------------
    enum class ETCvtActions
    {
        Throw
        , StopThenThrow
        , Replace
    };


    // -----------------------------------------------------------------------
    //  The available thread creation flags. There are none at this time,
    //  but this enum is defined so that the thread methods that might have
    //  some flags at some point can have the parameter available.
    // -----------------------------------------------------------------------
    enum class EThreadFlags
    {
        None            = 0x0000
        , Reserve       = 0xFFFF
    };


    // -----------------------------------------------------------------------
    //  The components that make up a time. Used mostly for parsing format
    //  strings, that tell the TTime class how to parse a text formatted time.
    // -----------------------------------------------------------------------
    enum class ETimeComps
    {
        ISO8601TZ
        , AMPM
        , Colon
        , DateSep
        , DayName
        , DayNum
        , Hour12
        , Hour24
        , Hyphen
        , Minute
        , MonthName
        , MonthNum
        , Punct
        , Second
        , Space
        , T
        , TimeSep
        , TZ
        , TZName
        , TZSep
        , Year2
        , Year4

        , Count
    };


    // -----------------------------------------------------------------------
    //  The ways to handle formatting trailing digits
    // -----------------------------------------------------------------------
    enum class ETrailFmts
    {
        Zeroes
        , Spaces
        , Ignore
    };


    // -----------------------------------------------------------------------
    //  Tree delete modes. Clean means remove the contents but leave the tree
    //  structure. Remove means remove the tree structure, too. Shallow means
    //  just remove files from the target, don't delete any sub-structure or
    //  anything below the start directory.
    // -----------------------------------------------------------------------
    enum class ETreeDelModes
    {
        Clean
        , Remove
        , Shallow
    };


    // -----------------------------------------------------------------------
    //  The types of nodes in a tree
    // -----------------------------------------------------------------------
    enum class ETreeNodes
    {
        Terminal
        , NonTerminal
    };


    // -----------------------------------------------------------------------
    //  The state flags for a three state object
    // -----------------------------------------------------------------------
    enum class ETriStates
    {
        Off
        , Kinda
        , On

        , Count
        , Min           = Off
        , Max           = On
    };


    // -----------------------------------------------------------------------
    //  The possible states for something that can be visible or not
    // -----------------------------------------------------------------------
    enum class EVisible
    {
        Invisible
        , Visible

        , Count
        , Min               = Invisible
        , Max               = Visible
    };


    // -----------------------------------------------------------------------
    //  These are returned by the file system volume information classes.
    //  They indicate various attributes of the volume.
    // -----------------------------------------------------------------------
    enum class EVolumeFlags : tCIDLib::TCard4
    {
        None                = 0x00000000
        , CaseSensitive     = 0x00000001
        , CasePreserved     = 0x00000002
        , UnicodeNames      = 0x00000004
    };


    // -----------------------------------------------------------------------
    //  The gross level types of the hardware on which volumes exist. Note
    //  that 35Floppy is a 3.5" floppy and 525Floppy is a 5.25" floppy. We
    //  don't distinguish other types of removeables and just lump them under
    //  Removable.
    //
    //  Share means it's a shared directory volume, not a real or viritual
    //  drive.
    // -----------------------------------------------------------------------
    enum class EVolHWTypes
    {
        Unknown
        , Floppy35
        , Floppy525
        , Removable
        , Fixed
        , Remote
        , CD
        , RAMDisk
        , Share

        , Count
        , Min           = Unknown
        , Max           = Share
    };



    // -----------------------------------------------------------------------
    //  A flag for any situation where the caller can optionally block for
    //  it to complete.
    // -----------------------------------------------------------------------
    enum class EWaitModes
    {
        NoWait
        , Wait

        , Count
        , Min           = NoWait
        , Max           = Wait
    };


    // -----------------------------------------------------------------------
    //  A result that can be returned from any wait operation whether we either
    //  got what we were waiting for, timed out, or got an error while waiting.
    // -----------------------------------------------------------------------
    enum class EWaitRes
    {
        Success
        , Timeout
        , Error

        , Count
        , Min           = Success
        , Max           = Error
    };


    // -----------------------------------------------------------------------
    //  Similar to above but for async wait operations
    // -----------------------------------------------------------------------
    enum class EAsyncWaitRes
    {
        Waiting
        , Complete
        , Timeout
        , Error

        , Count
        , Min           = Waiting
        , Max           = Error
    };


    // -----------------------------------------------------------------------
    //  An enumeration for the days of the week
    //
    //  !!These MUST remain zero based and contiguous!
    // -----------------------------------------------------------------------
    enum class EWeekDays
    {
        Sunday
        , Monday
        , Tuesday
        , Wednesday
        , Thursday
        , Friday
        , Saturday

        , Count
        , Min           = Sunday
        , Max           = Saturday
    };


#if !defined(CIDKRNL_NOCLASSES)

    // -----------------------------------------------------------------------
    //  This is the prototype for a thread exit function. Used by TThread.
    // -----------------------------------------------------------------------
    using TExitFuncPtr = TVoid (*)(TThread&);


    // -----------------------------------------------------------------------
    //  This is the prototype for a thread function, for those threads that
    //  choose to provide a function instead of overriding eProcess().
    // -----------------------------------------------------------------------
    using TThreadFuncPtr = tCIDLib::EExitCodes (*)(TThread&, tCIDLib::TVoid*);


    // -----------------------------------------------------------------------
    //  This is the prototype for an object factory function, used by the
    //  TClass class.
    // -----------------------------------------------------------------------
    using TObjFactory = TObject* (*)();


    // -----------------------------------------------------------------------
    //  The prototype for the installable handler for signals, via the
    //  TKrnlSignalHandler class.
    // -----------------------------------------------------------------------
    using TSigHandler = tCIDLib::TBoolean (*)(const tCIDLib::ESignals);

#endif


    // -----------------------------------------------------------------------
    //  This conditional structure allows one to write endian independent
    //  code in some cases.
    // -----------------------------------------------------------------------
    #pragma CIDLIB_PACK(1)

    #if defined(CIDLIB_LITTLEENDIAN)
        union TCard4Union
        {
            tCIDLib::TCard4     c4Val;
            tCIDLib::TCard1     ac1Bytes[4];
            struct
            {
                tCIDLib::TCard4 c4Byte3 : 8;
                tCIDLib::TCard4 c4Byte2 : 8;
                tCIDLib::TCard4 c4Byte1 : 8;
                tCIDLib::TCard4 c4Byte0 : 8;
            }   Fields;
        };
    #else
        union TCard4Union
        {
            tCIDLib::TCard4     c4Val;
            tCIDLib::TCard1     ac1Bytes[4];
            struct
            {
                tCIDLib::TCard4 c4Byte0 : 8;
                tCIDLib::TCard4 c4Byte1 : 8;
                tCIDLib::TCard4 c4Byte2 : 8;
                tCIDLib::TCard4 c4Byte3 : 8;
            }   Fields;
        };
    #endif


    // ------------------------------------------------------------------------
    //  The format of index entries in a CIDMsg binary message file.
    // ------------------------------------------------------------------------
    struct TMsgIndexEntry
    {
        tCIDLib::TMsgId         midThis;
        tCIDLib::TCard4         c4TextOffset;
        tCIDLib::TCard2         c2TextChars;
    };

    struct TMsgIndex
    {
        tCIDLib::TCard4         c4MsgCount;
        TMsgIndexEntry          amieList[1];
    };


    // -----------------------------------------------------------------------
    //  Some platform independent structural representations of colors, points,
    //  and areas. There are platform dependent ones in the per-platform
    //  headers, for use internally where we need to get things in and out
    //  of host APIs. These are more for when we need to pass them in and out
    //  to some non-CIDLib based interface, but can define what the format is.
    // -----------------------------------------------------------------------
    struct TRawArea
    {
        tCIDLib::TInt4  i4X;
        tCIDLib::TInt4  i4Y;
        tCIDLib::TCard4 c4CX;
        tCIDLib::TCard4 c4CY;
    };

    struct TRawColor
    {
        tCIDLib::TCard1 c1Red;
        tCIDLib::TCard1 c1Green;
        tCIDLib::TCard1 c1Blue;
    };

    struct TRawAlphaColor
    {
        tCIDLib::TCard1 c1Red;
        tCIDLib::TCard1 c1Green;
        tCIDLib::TCard1 c1Blue;
        tCIDLib::TCard1 c1Alpha;
    };

    struct TRawPoint
    {
        tCIDLib::TInt4  i4X;
        tCIDLib::TInt4  i4Y;
    };

    struct TRawSize
    {
        tCIDLib::TCard4 c4CX;
        tCIDLib::TCard4 c4CY;
    };



    // -----------------------------------------------------------------------
    //  The header structure for the .CIDRes binary resource file, and the simple
    //  structure used to write out the file location map for each resource in the
    //  binary file. So the format is a header followed by one of these maps for
    //  each resource, then the resource data.
    //
    //  This is the bare minimum suff we need in order to load the raw resource
    //  data, which is done via TModule. It doesn't understand what is in the data
    //  blobs, that is done by higher level code, graphics and GUI oriented stuff.
    // -----------------------------------------------------------------------
    struct TResHeader
    {
        tCIDLib::TCard4     c4MagicVal;
        tCIDLib::TCard4     c4FmtVersion;
        tCIDLib::TCard4     c4ResCount;
        tCIDLib::TCard4     c4Size;
    };

    struct TResItemMap
    {
        tCIDLib::TResId     ridRes;
        tCIDLib::TCard4     c4At;
        tCIDLib::TCard4     c4Size;
        EResTypes           eType;
    };

    #pragma CIDLIB_POPPACK
}



// ---------------------------------------------------------------------------
//  Some of the CIDComm types have to exist here, because they are needed by
//  the platform support.
//
//  NOTE:   A number of these enums are mapped into the macro language com
//          port support! So if you change these, you must update the macro
//          language wrapper classes that use them!
// ---------------------------------------------------------------------------
namespace tCIDComm
{
    // -----------------------------------------------------------------------
    //  The data bits supported by comm ports
    // -----------------------------------------------------------------------
    enum class EDataBits
    {
        Four                = 4
        , Five              = 5
        , Six               = 6
        , Seven             = 7
        , Eight             = 8

        , Count             = 5
        , Min               = Four
        , Max               = Eight
    };


    // -----------------------------------------------------------------------
    //  Comm port I/O failure codes
    // -----------------------------------------------------------------------
    enum class EErrors : tCIDLib::TCard4
    {
        None                = 0x0
        , Break             = 0x00000001
        , Frame             = 0x00000002
        , Error             = 0x00000004
        , BadMode           = 0x00000008
        , Overrun           = 0x00000010
        , RXOver            = 0x00000020
        , TXFull            = 0x00000040
        , Parity            = 0x00000080

        , Other             = 0x10000000
        , Unknown           = 0x20000000
        , AllBits           = 0x000000FF
    };


    // -----------------------------------------------------------------------
    //  The set of boolean flags that indicate part of a comm port's state
    // -----------------------------------------------------------------------
    enum class EFlags : tCIDLib::TCard4
    {
        ParityChecking      = 0x00000001
        , CTSOut            = 0x00000002
        , DSROut            = 0x00000004
        , XOut              = 0x00000008
        , XIn               = 0x00000010

        , AllBits           = 0x0000001F
        , None              = 0x0
    };


    // -----------------------------------------------------------------------
    //  The flags for opening a COM port
    // -----------------------------------------------------------------------
    enum class EOpenFlags : tCIDLib::TCard4
    {
        None                = 0x0
        , WriteThrough      = 0x00000001

        , All               = 0x00000001
    };


    // -----------------------------------------------------------------------
    //  The parity schemes supported by comm ports
    // -----------------------------------------------------------------------
    enum class EParities
    {
        None
        , Odd
        , Even
        , Mark
        , Space

        , Count
        , Min               = None
        , Max               = Space
    };


    // -----------------------------------------------------------------------
    //  Flow controls for the DTR and RTS lines on a comm port
    // -----------------------------------------------------------------------
    enum class EPortDTR
    {
        Disable
        , Enable
        , Handshake

        , Count
        , Min               = Disable
        , Max               = Handshake
        , Unknown
    };

    enum class EPortRTS
    {
        Disable
        , Enable
        , Handshake
        , Toggle

        , Count
        , Min               = Disable
        , Max               = Toggle
        , Unknown
    };


    // -----------------------------------------------------------------------
    //  The stop bits supported by comm ports
    // -----------------------------------------------------------------------
    enum class EStopBits
    {
        One
        , OnePointFive
        , Two

        , Count
        , Min               = One
        , Max               = Two
    };
}


// ---------------------------------------------------------------------------
//  In order to abstract the host socket functionality we also have to define
//  some socket stuff here, though its made to show up as though its in the
//  CIDSock facility's namespace.
// ---------------------------------------------------------------------------
namespace tCIDSock
{
    // -----------------------------------------------------------------------
    //  Used in the TKrnlTCPIP::bIPAFromText method, it indicates how a text
    //  address was converted to binary. If it didn't fail, then the conversion
    //  either worked because the text was a dotted address and it was directly
    //  coverted to numeric, or it was not a dotted and a reverse lookup was
    //  done and it worked.
    // -----------------------------------------------------------------------
    enum class EAddrCvtRes
    {
        Failed
        , WasNumeric
        , ViaLookup
    };


    // -----------------------------------------------------------------------
    //  Used to indicate IPV4 or IPV6 addresses
    // -----------------------------------------------------------------------
    enum class EAddrTypes
    {
        Unspec
        , IPV4
        , IPV6

        , Count
        , Min           = Unspec
        , Max           = IPV6
    };


    // -----------------------------------------------------------------------
    //  Used in the methods that enumerate host addresses
    // -----------------------------------------------------------------------
    enum class EAddrInfoFlags : tCIDLib::TCard2
    {
        None                = 0x0000
        , Passive           = 0x0001

        , StdListen         = 0x0001
    };


    // -----------------------------------------------------------------------
    //  The flags used by a multi-select operation. The last one is our flag
    //  and is used to indicate that nothing has been recieved on the socket
    //  for more than some maximum idle time.
    // -----------------------------------------------------------------------
    enum class EMSelFlags : tCIDLib::TCard2
    {
        None                = 0x0000
        , Read              = 0x0001
        , Write             = 0x0002
        , Except            = 0x0004

        , MaxIdle           = 0x8000
    };


    // -----------------------------------------------------------------------
    //  Result returned from the TKrnlSockPinger class' method that waits for
    //  a response to come in. We also have a state enum that the object uses
    //  to track what state it is in. We expose this state to the outside
    //  work for reading so we have to define it here.
    // -----------------------------------------------------------------------
    enum class EPingRes
    {
        Error
        , Timeout
        , Success
    };

    enum class EPingStates
    {
        WaitInit
        , Idle
        , WaitReply
    };


    // -----------------------------------------------------------------------
    //  The available socket protocols. Keep this in sync with the translation
    //  methods in the CIDSock facility class.
    // -----------------------------------------------------------------------
    enum class ESockProtos
    {
        IP
        , ICMP
        , IGMP
        , TCP
        , PUP
        , UDP
        , IDP
        , RawIP

        , Count
        , Min           = IP
        , Max           = RawIP
    };


    // -----------------------------------------------------------------------
    //  The available shutdown modes for a socket
    // -----------------------------------------------------------------------
    enum class ESockShutdown
    {
        Receive
        , Send
        , Both

        , Count
        , Min           = Receive
        , Max           = Both
    };


    // -----------------------------------------------------------------------
    //  These are returned by the eEnumEvents() method of the socket class.
    //  They indicate which events occured.
    // -----------------------------------------------------------------------
    enum class ESockEvs : tCIDLib::TCard2
    {
        None            = 0x00000
        , Read          = 0x00001
        , Write         = 0x00002
        , Close         = 0x00004
        , Accept        = 0x00008
        , Connect       = 0x00010
    };


    // -----------------------------------------------------------------------
    //  The available types of sockets
    // -----------------------------------------------------------------------
    enum class ESocketTypes
    {
        Stream
        , Datagram

        , Count
        , Min           = Stream
        , Max           = Datagram
    };



    // -----------------------------------------------------------------------
    //  Used in the TKrnlIPAddr class to allow construction of some of the
    //  special IP addresses.
    // -----------------------------------------------------------------------
    enum class ESpecAddrs
    {
        Any
        , AllMulticast
        , Broadcast
        , Loopback

        , Count
    };
}


// ---------------------------------------------------------------------------
//  A few things from the Net library are down here so that they can be
//  seen by the kernel as well.
// ---------------------------------------------------------------------------
namespace tCIDNet
{
    // -----------------------------------------------------------------------
    //  The available SSL operations
    // -----------------------------------------------------------------------
    enum class ESSLOps
    {
        GET
        , POST
        , PUT
    };
}



// ---------------------------------------------------------------------------
//  These really belong to CIDImage, but they are so fundamental and are used
//  in a few places where only the enum and nothing else is required, that
//  we put them here.
// ---------------------------------------------------------------------------
namespace tCIDImage
{
    // -----------------------------------------------------------------------
    //  The available bit depths. What they really mean depends on combining
    //  them with the EPixFmts value.
    // -----------------------------------------------------------------------
    enum class EBitDepths
    {
        One                 = 1
        , Two               = 2
        , Four              = 4
        , Five              = 5
        , Eight             = 8
        , Sixteen           = 16
    };


    // -----------------------------------------------------------------------
    //  The eProbeImg() method of the image factory facility class, which
    //  tries to figure out image types by looking at the raw data, returns
    //  one of these types.
    // -----------------------------------------------------------------------
    enum class EImgTypes
    {
        Unknown
        , Bitmap
        , JPEG
        , PNG
    };


    // -----------------------------------------------------------------------
    //  The pixel format. This one is somewhat bitmapped but often used as
    //  a standard enum. We provide some masks to look at the individual bits
    //  whihc indicate the presence of color, alpha and palette. This,
    //  in conjunction with the bitdepth, determines the actual format.
    //
    //  All image types must conform to the limitations here, which could
    //  possibly require that they tweak some of their possible formats when
    //  loading them into memory.
    //
    //  The value combinations are:
    //
    //      GrayScale (1, 2, 4, 8, 16)
    //
    //          The data is a single value per-pixel, which indicates a
    //          level of gray. 8 and 16 can also have alpha, which will also
    //          be either 8 or 16 bits (same as the color samples.)
    //
    //      Palette   (1, 2, 4, 8)
    //
    //          The data is a single value per-pixel, which refer to the values
    //          in a palette.
    //
    //      Color     (5, 8)
    //
    //          A 'true color' image, in which each pixel is represented by
    //          3 5 or 8 bit values. 8 bit can have alpha, which will also be
    //          8 bits.
    //
    //          Note that we will convert any 16 bit per color/alpha images
    //          to 24 or 32 bits because it's just too big of a pain to support
    //          that format in the pixel array class, because we cannot return
    //          a single pixel in a 32 bit value.
    //
    //  This scheme is taken from PNG, but it is flexible enough that it's
    //  acceptable as a model to which all images in CIDLib must adhere.
    // -----------------------------------------------------------------------
    enum class EPixFmts : tCIDLib::TCard2
    {
        GrayScale   = 0x0000
        , TrueClr   = 0x0002
        , ClrPal    = 0x0003
        , GrayAlpha = 0x0004
        , TrueAlpha = 0x0006

        // Bits for above
        , Palette   = 0x0001
        , Color     = 0x0002
        , Alpha     = 0x0004

        , ValidBits = 0x0007
    };
}



// ---------------------------------------------------------------------------
//  Some basic media stuff that may be needed in a number of places that don't
//  necessarily have any common lower level functionality.
// ---------------------------------------------------------------------------
namespace tCIDMedia
{
    // -----------------------------------------------------------------------
    //  The basic RIFF WAV file header. There may be more, but this should work
    //  for any of them to get the basic info.
    // -----------------------------------------------------------------------
    #pragma CIDLIB_PACK(1)
    struct TWaveHdr
    {
        tCIDLib::TSCh   szRiff[4];
        tCIDLib::TCard4 c4Len;
        tCIDLib::TSCh   szWavFmt[8];
        tCIDLib::TCard4 c4HdrLen;
        tCIDLib::TCard2 c2Format;
        tCIDLib::TCard2 c2NumChannels;
        tCIDLib::TCard4 c4SampleRate;
        tCIDLib::TCard4 c4BytesPerSec;
        tCIDLib::TCard2 c2BlockAlign;
        tCIDLib::TCard2 c2BitsPerSample;
    };
    #pragma CIDLIB_POPPACK
}


namespace tCIDCtrls
{
    // --------------------------------------------------------------------
    //  The extended characters dispatched to a window. These are in here because they
    //  maybe sent to windows in some indirect way, such as application control in
    //  CQC where the normal CIDCtrls stuff won't be visible.
    //
    //  NOTE:   These are commonly persisted SO ADD NEW ONES AT THE END!
    // --------------------------------------------------------------------
    enum class EExtKeys : tCIDLib::TInt4
    {
        None
        , Button1
        , Button2
        , Button3
        , BackSpace
        , Tab
        , BackTab
        , Shift
        , Ctrl
        , Alt
        , Pause
        , CapsLock
        , Esc
        , PageUp
        , PageDown
        , End
        , Home
        , Left
        , Up
        , Right
        , Down
        , PrintScreen
        , Insert
        , Delete
        , ScrollLock
        , NumLock
        , Enter
        , Space

        , F1
        , F2
        , F3
        , F4
        , F5
        , F6
        , F7
        , F8
        , F9
        , F10
        , F11
        , F12
        , F13
        , F14
        , F15
        , F16
        , F17
        , F18
        , F19
        , F20
        , F21
        , F22
        , F23
        , F24

        , Browser_Back
        , Browser_Forward
        , Browser_Stop
        , Browser_Refresh
        , Browser_Search
        , Browser_Favorites
        , Browser_Home

        , Volume_Mute
        , Volume_Down
        , Volume_Up
        , Media_PlayPause
        , Media_Stop
        , Media_PrevTrack
        , Media_NextTrack

        , NumPad_Div
        , NumPad_Mul
        , NumPad_Sub
        , NumPad_Plus
        , NumPad_Decimal

        , Count
        , Min               = None
        , Max               = NumPad_Decimal
    };
}


//
//  Generate bitmap ops support for those that need it. This we can do down
//  here since it doesn't require any CIDLib level stuff. And this way we
//  have these available down here in the kernel.
//
BmpEnumTricks(tCIDLib::EAccessModes)
BmpEnumTricks(tCIDLib::EDirChFilters)
BmpEnumTricks(tCIDLib::EDirSearchFlags)
BmpEnumTricks(tCIDLib::EExtProcFlags)
BmpEnumTricks(tCIDLib::EFilePerms)
BmpEnumTricks(tCIDLib::EFileFlags)
BmpEnumTricks(tCIDLib::EFileInfoFlags)
BmpEnumTricks(tCIDLib::EMemAccFlags)
BmpEnumTricks(tCIDLib::EMemStates)
BmpEnumTricks(tCIDLib::EMemTypes)
BmpEnumTricks(tCIDLib::EModFlags)
BmpEnumTricks(tCIDLib::EPathParts)
BmpEnumTricks(tCIDLib::ERegAccessModes)
BmpEnumTricks(tCIDLib::ERWAccess)
BmpEnumTricks(tCIDLib::ESpeechFlags)
BmpEnumTricks(tCIDLib::EStripModes)
BmpEnumTricks(tCIDLib::ESysEscFlags)
BmpEnumTricks(tCIDLib::EVolumeFlags)
