//
// FILE NAME: CIDLib_LogEvent.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/07/1993
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
//  This is the header for the CIDLib_LogEvent.Cpp file, which implements
//  the standard object logged and thrown in the CIDLib system.
//
// CAVEATS/GOTCHAS:
//
//  1)  Since this class is also the exception class, we provide an alias
//      that maps TError to TLogEvent. This allows people to catch as
//      a TError type, but its just one of these.
//
//  2)  Kernel errors are defined in the CIDKernel layer, so it cannot
//      proivde streaming, so we provide a global streaming operator for
//      it here.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TLogEvent
// PREFIX: err
// ---------------------------------------------------------------------------
class CIDLIBEXP TLogEvent :

    public TObject, public MDuplicable, public MStreamable, public MFormattable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TLogEvent();

        TLogEvent
        (
            const   TString&                strFacName
            , const TString&                strFileName
            , const tCIDLib::TCard4         c4LineNum
            , const TString&                strMsgText
            , const tCIDLib::ESeverities    eSev = tCIDLib::ESeverities::Status
            , const tCIDLib::EErrClasses    eClass = tCIDLib::EErrClasses::None
        );

        TLogEvent
        (
            const   TString&                strFacName
            , const TString&                strFileName
            , const tCIDLib::TCard4         c4LineNum
            , const TString&                strMsgText
            , const TString&                strAuxText
            , const tCIDLib::ESeverities    eSev = tCIDLib::ESeverities::Status
            , const tCIDLib::EErrClasses    eClass = tCIDLib::EErrClasses::None
        );

        TLogEvent
        (
            const   TString&                strFacName
            , const TString&                strFileName
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TMsgId         midLoad
            , const TString&                strErrText
            , const TString&                strAuxText
            , const tCIDLib::ESeverities    eSeverity
            , const tCIDLib::EErrClasses    eClass
        );

        TLogEvent
        (
            const   TString&                strFacName
            , const TString&                strFileName
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TMsgId         midLoad
            , const TString&                strErrText
            , const tCIDLib::ESeverities    eSeverity
            , const tCIDLib::EErrClasses    eClass
        );

        TLogEvent
        (
            const   TString&                strFacName
            , const TString&                strFileName
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TMsgId         midLoad
            , const TKrnlError&             kerrIds
            , const TString&                strErrText
            , const TString&                strAuxText
            , const tCIDLib::ESeverities    eSeverity
            , const tCIDLib::EErrClasses    eClass
        );

        TLogEvent
        (
            const   TString&                strFacName
            , const TString&                strFileName
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TMsgId         midLoad
            , const TKrnlError&             kerrIds
            , const TString&                strErrText
            , const tCIDLib::ESeverities    eSeverity
            , const tCIDLib::EErrClasses    eClass
        );

        TLogEvent(const TLogEvent&) = default;
        TLogEvent(TLogEvent&&) = default;

        ~TLogEvent();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TLogEvent& operator=(const TLogEvent&) = default;
        TLogEvent& operator=(TLogEvent&&) = default;

        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddStackLevel
        (
            const   TString&                strFile
            , const tCIDLib::TCard4         c4LineNum
        );

        tCIDLib::TVoid AdvFormat
        (
                    TTextOutStream&         strmTar
            ,       TTime&                  tmFmt
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bAnyError() const;

        [[nodiscard]] tCIDLib::TBoolean bCheckEvent
        (
            const   TString&                strModName
            , const tCIDLib::TErrCode       errcToCheck
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bCheckEvent
        (
            const   tCIDLib::TCh* const     pszModName
            , const tCIDLib::TErrCode       errcToCheck
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bHasAuxText() const;

        [[nodiscard]] tCIDLib::TBoolean bLogged() const;

        tCIDLib::TBoolean bLogged
        (
            const   tCIDLib::TBoolean       bToSet
        );

        [[nodiscard]] tCIDLib::TBoolean bReported() const;

        tCIDLib::TBoolean bReported
        (
            const   tCIDLib::TBoolean       bToSet
        )   const;

        [[nodiscard]] tCIDLib::TBoolean bSameEvent
        (
            const   TLogEvent&              errToCheck
        )   const;

        [[nodiscard]] tCIDLib::TCard4 c4LineNum() const;

        tCIDLib::TCard4 c4LineNum
        (
            const   tCIDLib::TCard4         c4NewLineNum
        );

        [[nodiscard]] tCIDLib::TEncodedTime enctLogged() const;

        tCIDLib::TEncodedTime enctLogged
        (
            const   tCIDLib::TEncodedTime   enctNew
        );

        [[nodiscard]] tCIDLib::TOSErrCode errcHostId() const;

        tCIDLib::TOSErrCode errcHostId
        (
            const   tCIDLib::TOSErrCode     errcNew
        );

        [[nodiscard]] tCIDLib::TErrCode errcId() const;

        tCIDLib::TErrCode errcId
        (
            const   tCIDLib::TErrCode       errcNew
        );

        [[nodiscard]] tCIDLib::TErrCode errcKrnlId() const;

        tCIDLib::TErrCode errcKrnlId
        (
            const   tCIDLib::TErrCode       errcNew
        );

        [[nodiscard]] tCIDLib::EErrClasses eClass() const;

        tCIDLib::EErrClasses eClass
        (
            const   tCIDLib::EErrClasses    eNewClass
        );

        [[nodiscard]] tCIDLib::ESeverities eSeverity() const;

        tCIDLib::ESeverities eSeverity
        (
            const   tCIDLib::ESeverities    eNewSev
        );

        tCIDLib::TVoid Reset();

        [[nodiscard]] const TString& strAuxText() const;

        const TString& strAuxText
        (
            const   TString&                strNewText
        );

        [[nodiscard]] const TString& strErrText() const;

        const TString& strErrText
        (
            const   TString&                strNewText
        );

        [[nodiscard]] const TString& strFacName() const;

        const TString& strFacName
        (
            const   TString&                strNewName
        );

        [[nodiscard]] const TString& strFileName() const;

        const TString& strFileName
        (
            const   TString&                strNewName
        );

        [[nodiscard]] const TString& strHostName() const;

        const TString& strHostName
        (
            const   TString&                strNewName
        );

        [[nodiscard]] const TString& strProcess() const;

        const TString& strProcess
        (
            const   TString&                strNewName
        );

        [[nodiscard]] const TString& strStackTrace() const;

        [[nodiscard]] const TString& strThread() const;

        const TString& strThread
        (
            const   TString&                strNewName
        );

        tCIDLib::TVoid SetLogged() const;


    protected   :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TModule;


        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
            CIOP    TTextOutStream&         strmToWriteTo
        )   const final;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   final;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const final;


    private       :
        // -------------------------------------------------------------------
        //  Private data
        //
        //  m_bLogged
        //  m_bReported
        //      Indicates whether or not its been sent to the installed logger
        //      or not and whether it's been reported to the user or not.
        //      Default to false, and are set by the logging code or any code
        //      that displays an error. They are mutable, so that they can be
        //      set when logged or displayed, which will take const refs to the
        //      object.
        //
        //  m_c4LineNum
        //      The number of the line on which the error is reported. It can
        //      easily be provided by the CID_LINE macro and most facilities
        //      provide an internal macro to provide these automatically.
        //
        //  m_eClass
        //      And indicator of the cause of the error. It may be EClass_None
        //      if the cause is unknown.
        //
        //  m_errcHostId
        //      The underlying host error code, if any. If none, then it will
        //      be 0.
        //
        //  m_errcId
        //      This is the CIDLib error code. What error it really maps to
        //      is of course relative to the module that it came from.
        //
        //  m_errcKrnlId
        //      This the underlying kernel error code, if any. If none, then
        //      it will be 0.
        //
        //  m_eSeverity
        //      The severity level of the error, tells the caller how bad
        //      it was.
        //
        //  m_strAuxText
        //      This is the storage for the auxiliary text string. This is to
        //      give an extra spin on the error text.
        //
        //  m_strErrText
        //      This is the storage for the error text. This may be set
        //      manually with a string or by loading the error text from a
        //      message file and storing it here.
        //
        //  m_strFacName
        //      This is the name of the facility that returned the error. It
        //      is provided for convenience and so that it can be reported
        //      along with the other information.
        //
        //  m_strFileName
        //      This is the name of the file in which the error occured.
        //      This is easily provided via the CID_FILE macro.
        //
        //  m_strHostName
        //      The host name of the host that logged the message. It's not
        //      always obvious because of the distributed nature of CIDLib
        //      based programs, which can throw errors back from ORB based
        //      calls, and when things are logged to the log server. It's
        //      provided automatically when the event is logged.
        //
        //  m_strProcess
        //      This is the name of the process that had the error. You'd
        //      think that the process would be implicit, but remember that
        //      events are often logged to a remote server and viewed from
        //      still other machines.
        //
        //  m_strStackTrace
        //      Catchers of exceptions can add a file name/line number to this
        //      string, which will be just formatted out as new line separated
        //      text, and then rethrow. This allows for tracing the flow of
        //      the exception, key to in the field diagnosis.
        //
        //  m_strThread
        //      This is the name of the thread that had the error.
        // -------------------------------------------------------------------
        mutable tCIDLib::TBoolean       m_bLogged;
        mutable tCIDLib::TBoolean       m_bReported;
        tCIDLib::TCard4                 m_c4LineNum;
        tCIDLib::EErrClasses            m_eClass;
        mutable tCIDLib::TEncodedTime   m_enctLogged;
        tCIDLib::TOSErrCode             m_errcHostId;
        tCIDLib::TErrCode               m_errcId;
        tCIDLib::TErrCode               m_errcKrnlId;
        tCIDLib::ESeverities            m_eSeverity;
        TString                         m_strAuxText;
        TString                         m_strErrText;
        TString                         m_strFacName;
        TString                         m_strFileName;
        TString                         m_strHostName;
        TString                         m_strProcess;
        TString                         m_strStackTrace;
        TString                         m_strThread;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TLogEvent,TObject)
        DefPolyDup(TLogEvent)
};

// We alias this and use it as the exception type as well
using TError = TLogEvent;


// ---------------------------------------------------------------------------
//  Global streaming operators to allow streaming of kernel errors, which are
//  defined in the kernel layer and cannot provide them itself.
// ---------------------------------------------------------------------------
CIDLIBEXP TBinOutStream&
operator<<(TBinOutStream& strmToWriteTo, const TKrnlError& kerrToWrite);
CIDLIBEXP TBinInStream& operator>>(TBinInStream& strmToReadTo, TKrnlError& kerrToRead);

#pragma CIDLIB_POPPACK

