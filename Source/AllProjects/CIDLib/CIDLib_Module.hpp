//
// FILE NAME: CIDLib_Module.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/05/1997
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
//  This is the header file for the CIDLib_Module.Cpp file. This file
//  provides the basic TModule class, which represents a DLL or Exe module.
//  This class is the base class for the more important (and more often used)
//  TFacility class. TModule can be used for more generic module operations,
//  where a need exists to create a module object for temporary use.
//
//  This class draws upon the TKrnlModule class, which provides the underlying
//  platform specific functionality.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class   TLogEvent;
class   MLogger;
class   TMutex;
template <typename TLogEvent> class TCollection;


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TModule
// PREFIX: mod
// ---------------------------------------------------------------------------
class CIDLIBEXP TModule : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bLoadOSMsg
        (
            const   tCIDLib::TOSErrCode     errcId
            ,       TString&                strToFill
        );

        static tCIDLib::TVoid BuildRawModName
        (
            const   TString&                strBaseName
            , const tCIDLib::TCard4         c4MajVersion
            , const tCIDLib::TCard4         c4MinVersion
            , const tCIDLib::EModTypes      eModType
            ,       TString&                strToFill
        );

        static tCIDLib::TVoid BuildRawModName
        (
            const   TCIDModuleInfo&         modiSrc
            ,       TString&                strToFill
        );

        static tCIDLib::TVoid BuildRawModName
        (
            const   TString&                strBaseName
            , const tCIDLib::TCard4         c4MajVersion
            , const tCIDLib::TCard4         c4MinVersion
            , const tCIDLib::EModTypes      eModType
            ,       TString&                strToFill
            ,       TString&                strPrefLangMsgs
            ,       TString&                strDefLangMsgs
        );

        static tCIDLib::TVoid BuildRawModName
        (
            const   TCIDModuleInfo&         modiSrc
            ,       TString&                strToFill
            ,       TString&                strPrefLangMsgs
            ,       TString&                strDefLangMsgs
        );

        static tCIDLib::TVoid BuildRawModName
        (
            const   TString&                strBaseName
            , const tCIDLib::TCard4         c4MajVersion
            , const tCIDLib::TCard4         c4MinVersion
            , const tCIDLib::EModTypes      eModType
            , const tCIDLib::ELanguages     ePrefLanguage
            , const tCIDLib::ELanguages     eDefLanguage
            ,       TString&                strToFill
            ,       TString&                strPrefLangMsgs
            ,       TString&                strDefLangMsgs
            ,       TString&                strEnLangMsgs
        );

        static tCIDLib::TVoid BuildRawModName
        (
            const   TCIDModuleInfo&         modiSrc
            , const tCIDLib::ELanguages     ePrefLanguage
            , const tCIDLib::ELanguages     eDefLanguage
            ,       TString&                strToFill
            ,       TString&                strPrefLangMsgs
            ,       TString&                strDefLangMsgs
            ,       TString&                strEnLangMsgs
        );

        static tCIDLib::TCard8 c8ParseVersionStr
        (
            const   TString&                strToParse
            , const tCIDLib::TBoolean       bFullFormat
        );

        static tCIDLib::TVoid InstallLogger
        (
                    MLogger* const          plgrToSet
            , const tCIDLib::EAdoptOpts     eAdopt
        );

        static tCIDLib::TVoid LogEventObj
        (
            const   TLogEvent&              logevToLog
        );

        static tCIDLib::TVoid LogEventObj
        (
                    TLogEvent&&             logevToLog
        );

        // The events will be moved!
        static tCIDLib::TVoid LogEventObjs
        (
                    TCollection<TLogEvent>& colToLog
        );

        static tCIDLib::TVoid OrphanLogger();

        static tCIDLib::TVoid ParseVersionStr
        (
            const   TString&                strToParse
            ,       tCIDLib::TCard4&        c4MajVer
            ,       tCIDLib::TCard4&        c4MinVer
        );

        static tCIDLib::TVoid ParseVersionStr
        (
            const   TString&                strToParse
            ,       tCIDLib::TCard4&        c4MajVer
            ,       tCIDLib::TCard4&        c4MinVer
            ,       tCIDLib::TCard4&        c4Revision
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TModule() = delete;

        TModule
        (
            const   TString&                strName
            , const tCIDLib::EModTypes      eModType
            , const tCIDLib::TCard4         c4MajVer
            , const tCIDLib::TCard4         c4MinVer
            , const tCIDLib::EModFlags      eFlags
            , const tCIDLib::TBoolean       bLoad = kCIDLib::False
        );

        TModule
        (
            const   TCIDModuleInfo&         modiSrc
            , const tCIDLib::TBoolean       bLoad = kCIDLib::False
        );

        TModule
        (
            const   TString&                strFullPath
            , const tCIDLib::EModTypes      eModType
            , const tCIDLib::EModFlags      eFlags
            , const tCIDLib::TBoolean       bLoad = kCIDLib::False
        );

        TModule
        (
            const   TString&                strFacName
            , const TString&                strLoadFrom
            , const tCIDLib::EModTypes      eModType
            , const tCIDLib::EModFlags      eFlags
            , const tCIDLib::TBoolean       bLoad = kCIDLib::False
        );

        TModule(const TModule&) = delete;
        TModule(TModule&&) = delete;

        ~TModule();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TModule& operator=(const TModule&) = delete;
        TModule& operator=(TModule&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bHasMessageFile() const;

        tCIDLib::TBoolean bLoadCIDMsg
        (
            const   tCIDLib::TMsgId         midToLoad
            ,       TString&                strMsgBuf
        )   const;

        tCIDLib::TBoolean bLoadCIDMsg
        (
            const   tCIDLib::TMsgId         midToLoad
            ,       TString&                strMsgBuf
            , const MFormattable&           fmtblToken1
            , const MFormattable&           fmtblToken2 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken3 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken4 = MFormattable::Nul_MFormattable()
        )   const;

        tCIDLib::EModTypes eModType() const;

        tCIDLib::TVoid ErrorPopUp
        (
            const   tCIDLib::TErrCode       errcToDisplay
            , const TString&                strFile
            , const tCIDLib::TCard4         c4Line
            , const TString&                strAuxMsg = TString::Nul_TString()
        );

        tCIDLib::TVoid ErrorPopUp
        (
            const   TLogEvent&              logevToShow
        );

        const TKrnlModule& kmodThis() const;

        tCIDLib::TVoid KrnlErrorPopUp
        (
            const   TString&                strFile
            , const tCIDLib::TCard4         c4Line
            , const tCIDLib::TErrCode       errcToDisplay
            , const TKrnlError&             kerrToDisplay
        );

        tCIDLib::TVoid LogKrnlErr
        (
            const   TString&                strFileName
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TErrCode       errcToThrow
            , const TKrnlError&             kerrToThrow
            , const tCIDLib::ESeverities    eSeverity
            , const tCIDLib::EErrClasses    eClass
        )   const;

        tCIDLib::TVoid LogKrnlErr
        (
            const   TString&                strFileName
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TErrCode       errcToThrow
            , const TKrnlError&             kerrToThrow
            , const tCIDLib::ESeverities    eSeverity
            , const tCIDLib::EErrClasses    eClass
            , const MFormattable&           fmtblToken1
            , const MFormattable&           fmtblToken2 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken3 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken4 = MFormattable::Nul_MFormattable()
        )   const;

        tCIDLib::TVoid LogMsg
        (
            const TString&                  strFileName
            , const tCIDLib::TCard4         c4LineNum
            , const TString&                strMsg
            , const tCIDLib::ESeverities    eSeverity
            , const tCIDLib::EErrClasses    eClass = tCIDLib::EErrClasses::AppStatus
        )   const;

        tCIDLib::TVoid LogMsg
        (
            const TString&                  strFileName
            , const tCIDLib::TCard4         c4LineNum
            , const TString&                strMsg
            , const TString&                strAuxText
            , const tCIDLib::ESeverities    eSeverity
            , const tCIDLib::EErrClasses    eClass = tCIDLib::EErrClasses::AppStatus
        )   const;

        tCIDLib::TVoid LogMsg
        (
            const TString&                  strFileName
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TMsgId         midToLog
            , const tCIDLib::ESeverities    eSeverity
            , const tCIDLib::EErrClasses    eClass = tCIDLib::EErrClasses::AppStatus
        )   const;

        tCIDLib::TVoid LogMsg
        (
            const TString&                  strFileName
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TMsgId         midToLog
            , const TString&                strAuxText
            , const tCIDLib::ESeverities    eSeverity
            , const tCIDLib::EErrClasses    eClass = tCIDLib::EErrClasses::AppStatus
        )   const;

        tCIDLib::TVoid LogMsg
        (
            const   TString&                strFileName
            , const tCIDLib::TCard4         c4LineNum
            , const TString&                strMsg
            , const tCIDLib::ESeverities    eSeverity
            , const tCIDLib::EErrClasses    eClass
            , const MFormattable&           fmtblToken1
            , const MFormattable&           fmtblToken2 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken3 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken4 = MFormattable::Nul_MFormattable()
        )   const;

        tCIDLib::TVoid LogMsg
        (
            const   TString&                strFileName
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TMsgId         midToLog
            , const tCIDLib::ESeverities    eSeverity
            , const tCIDLib::EErrClasses    eClass
            , const MFormattable&           fmtblToken1
            , const MFormattable&           fmtblToken2 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken3 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken4 = MFormattable::Nul_MFormattable()
        )   const;

        tCIDLib::TVoid LogMsg
        (
            const   TString&                strFileName
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TMsgId         midToLog
            , const TString&                strAuxText
            , const tCIDLib::ESeverities    eSeverity
            , const tCIDLib::EErrClasses    eClass
            , const MFormattable&           fmtblToken1
            , const MFormattable&           fmtblToken2 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken3 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken4 = MFormattable::Nul_MFormattable()
        )   const;

        tCIDLib::TVoid MsgPopUp
        (
            const   TString&                strMsg
            , const TString&                strFile
            , const tCIDLib::TCard4         c4Line
            , const TString&                strAuxMsg = TString::Nul_TString()
        );

        const tCIDLib::TCard1* pc1FindRes
        (
                    tCIDLib::TCard4&        c4ToFill
            , const tCIDLib::TResId         ridToFind
            , const tCIDLib::EResTypes      eType
            , const tCIDLib::TBoolean       bErrIfNotFound
        )   const;

        const tCIDLib::TCard1* pc1FindRes
        (
                    tCIDLib::EResTypes&     eType
            ,       tCIDLib::TCard4&        c4ToFill
            , const tCIDLib::TResId         ridToFind
            , const tCIDLib::TBoolean       bErrIfNotFound
        )   const;

        tCIDLib::FuncPtr pfnGetFuncPtr
        (
            const   tCIDLib::TSCh* const    pszFuncName
        )   const;

        const tCIDLib::TCh* pszLoadCIDMsg
        (
            const   tCIDLib::TMsgIndex&     miSrc
            , const tCIDLib::TMsgId         midToLoad
            ,       tCIDLib::TBoolean&      bLoaded
        )   const;

        const tCIDLib::TCh* pszLoadCIDMsg
        (
            const   tCIDLib::TMsgId         midToLoad
            ,       tCIDLib::TBoolean&      bLoaded
        )   const;

        const tCIDLib::TCh* pszLoadCIDMsg
        (
            const   tCIDLib::TMsgId         midToLoad
        )   const;

        TString strMsg
        (
            const   tCIDLib::TMsgId         midToLoad
            , const tCIDLib::TCard4         c4ExtraSpace = 0
        )   const;

        TString strMsg
        (
            const   tCIDLib::TMsgId         midToLoad
            , const MFormattable&           fmtblToken1
            , const MFormattable&           fmtblToken2 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken3 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken4 = MFormattable::Nul_MFormattable()
        )   const;

        TString strPath() const;

        TString strName() const;

        tCIDLib::TVoid ThrowErr
        (
            const   TString&                strFileName
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TErrCode       errcToThrow
            , const tCIDLib::ESeverities    eSeverity
            , const tCIDLib::EErrClasses    eClass = tCIDLib::EErrClasses::Unknown
        )   const;

        tCIDLib::TVoid ThrowErr
        (
            const   TString&                strFileName
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TErrCode       errcToThrow
            , const tCIDLib::ESeverities    eSeverity
            , const tCIDLib::EErrClasses    eClass
            , const MFormattable&           fmtblToken1
            , const MFormattable&           fmtblToken2 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken3 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken4 = MFormattable::Nul_MFormattable()
        )   const;

        tCIDLib::TVoid ThrowErr
        (
            const   TString&                strFileName
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TErrCode       errcToThrow
            , const TString&                strAuxText
            , const tCIDLib::ESeverities    eSeverity
            , const tCIDLib::EErrClasses    eClass = tCIDLib::EErrClasses::Unknown
        )   const;

        tCIDLib::TVoid ThrowErr
        (
            const   TString&                strFileName
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TErrCode       errcToThrow
            , const TString&                strAuxText
            , const tCIDLib::ESeverities    eSeverity
            , const tCIDLib::EErrClasses    eClass
            , const MFormattable&           fmtblToken1
            , const MFormattable&           fmtblToken2 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken3 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken4 = MFormattable::Nul_MFormattable()
        )   const;

        tCIDLib::TVoid ThrowKrnlErr
        (
            const   TString&                strFileName
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TErrCode       errcToThrow
            , const TKrnlError&             kerrToThrow
            , const tCIDLib::ESeverities    eSeverity
            , const tCIDLib::EErrClasses    eClass
        )   const;

        tCIDLib::TVoid ThrowKrnlErr
        (
            const   TString&                strFileName
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TErrCode       errcToThrow
            , const TKrnlError&             kerrToThrow
            , const tCIDLib::ESeverities    eSeverity
            , const tCIDLib::EErrClasses    eClass
            , const MFormattable&           fmtblToken1
            , const MFormattable&           fmtblToken2 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken3 = MFormattable::Nul_MFormattable()
            , const MFormattable&           fmtblToken4 = MFormattable::Nul_MFormattable()
        )   const;


    protected :
        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bTraceErrs() const;



    private :
        // -------------------------------------------------------------------
        //  Private, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid CheckFatal
        (
            const   tCIDLib::ESeverities    eSev
        );

        static tCIDLib::TVoid ShowLogFailure
        (
            const   TLogEvent&              logevShow
        );

        static tCIDLib::TVoid ShowLogFailure
        (
            const   tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        );


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DoInit
        (
            const   tCIDLib::TCard4         c4MajVer
            , const tCIDLib::TCard4         c4MinVer
            , const tCIDLib::EModFlags      eFlags
            , const tCIDLib::TBoolean       bLoad
        );

        tCIDLib::TVoid DoInit2
        (
            const   TString&                strModPath
            , const tCIDLib::EModFlags      eFlags
        );

        tCIDLib::TVoid InitStats();

        tCIDLib::TVoid LoadRes
        (
            const   TString&                strResFilePath
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_crsSync
        //      We need to synchronize some operations internally in this
        //      class, mainly oriented towards loading and caching resources.
        //
        //  m_eModType
        //      The type of this facility, set during the constructor.
        //
        //  m_kmodThis
        //      The facility module object. This is set by the constructor.
        //
        //  m_pc1Res
        //      If we ever load the resources, this is where it is read
        //      into and used from.
        //
        //  m_strName
        //      The name of the facility, passed in the constructor and stored here.
        //      No extension, no version stamp.
        //
        //  m_strPath
        //      The path where the facility is located, just the path, no file name.
        // -------------------------------------------------------------------
        TCriticalSection    m_crsSync;
        tCIDLib::EModTypes  m_eModType;
        TKrnlModule         m_kmodThis;
        tCIDLib::TCard1*    m_pc1Res;
        TString             m_strName;
        TString             m_strPath;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TModule,TObject)
};

#pragma CIDLIB_POPPACK


