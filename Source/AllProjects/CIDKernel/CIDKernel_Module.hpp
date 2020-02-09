//
// FILE NAME: CIDKernel_Module.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/22/1996
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
//  This is the header for the CIDKernel_Module.Cpp file, which implements
//  the TKrnlModule class. This class encapsulates the concept of a loadable
//  module (Exe or shared library) and the things that you do via it.
//
//
// CAVEATS/GOTCHAS:
//
//  1)  LoadXXX() loads a module, and should be used for dynamic loading.
//      QueryXXX() just queries the handle for an existing loaded module.
//
//  2)  We have to support loading external modules (not CIDLib facilities)
//      so we have some methods for that, which don't try to load any
//      associated CIDLib resources (since there are none.)
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TCIDKrnlModule;

// ---------------------------------------------------------------------------
//   CLASS: TKrnlModule
//  PREFIX: kmod
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlModule
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bBuildModNames
        (
             const  tCIDLib::TCh* const     pszModName
            ,       TKrnlString&            kstrPortableName
            ,       TKrnlString&            kstrLoadableName
            , const tCIDLib::TCard4         c4MajVer
            , const tCIDLib::TCard4         c4MinVer
            , const tCIDLib::EModTypes      eModType
        );

        static tCIDLib::TBoolean bRawQueryModName
        (
            const   TModuleHandle&          hmodToQuery
            ,       TKrnlString&            kstrNameToFill
            ,       TKrnlString&            kstrPathToFill
        );

        static tCIDLib::TBoolean bRawQueryModPath
        (
            const   TModuleHandle&          hmodToQuery
            ,       TKrnlString&            kstrPathToFill
        );

        static tCIDLib::TBoolean bLoadMessages
        (
            const   TKrnlString&            kstrPath
            , const TKrnlString&            kstrFile
            ,       tCIDLib::TMsgIndex*&    pmiLoadMessages
        );

        static const tCIDLib::TCh* pszLoadCIDMsg
        (
            const   tCIDLib::TMsgIndex&     miSrc
            , const tCIDLib::TMsgId         midToLoad
            ,       tCIDLib::TBoolean&      bLoaded
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlModule();

        TKrnlModule(const TKrnlModule&) = delete;

        ~TKrnlModule();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlModule& operator=(const TKrnlModule&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bGetFuncPtr
        (
            const   tCIDLib::TSCh* const    pszFuncName
            ,       tCIDLib::FuncPtr&       pfnToFill
        )   const;

        tCIDLib::TBoolean bHasMessageFile() const;

        tCIDLib::TBoolean bLoadCIDFacMsg
        (
            const   tCIDLib::TMsgId         midToLoad
            ,       tCIDLib::TCh* const     pszTarget
            , const tCIDLib::TCard4         c4MaxChars
        )   const;

        tCIDLib::TBoolean bLoadFromName
        (
            const   tCIDLib::TCh* const     pszBaseName
            , const tCIDLib::TCard4         c4MajVer
            , const tCIDLib::TCard4         c4MinVer
            , const tCIDLib::EModTypes      eModType
            , const tCIDLib::EModFlags      eModFlags
        );

        tCIDLib::TBoolean bLoadFromPath
        (
            const   tCIDLib::TCh* const     pszBaseName
            , const tCIDLib::TCard4         c4MajVer
            , const tCIDLib::TCard4         c4MinVer
            , const tCIDLib::TCh* const     pszFromPath
            , const tCIDLib::EModTypes      eModType
            , const tCIDLib::EModFlags      eModFlags
        );

        tCIDLib::TBoolean bLoadExternal
        (
            const   tCIDLib::TCh* const     pszToLoad
        );

        tCIDLib::TBoolean bQueryFromName
        (
            const   tCIDLib::TCh* const     pszBaseName
            , const tCIDLib::TCard4         c4MajVer
            , const tCIDLib::TCard4         c4MinVer
            , const tCIDLib::EModTypes      eModType
            , const tCIDLib::EModFlags      eModFlags
        );

        tCIDLib::TBoolean bQueryExternal
        (
            const   tCIDLib::TCh* const     pszFullPath
        );

        tCIDLib::EModTypes eModType() const
        {
            return m_eModType;
        }

        const TModuleHandle& hmodThis() const;

        const tCIDLib::TCh* pszLoadCIDFacMsg
        (
            const   tCIDLib::TMsgId         midToLoad
            ,       tCIDLib::TBoolean&      bLoaded
        )   const;

        const tCIDLib::TCh* pszLoadCIDFacMsg
        (
            const   tCIDLib::TMsgId         midToLoad
        )   const;

        const TKrnlString& kstrBaseName() const
        {
            return m_kstrBaseName;
        }

        const TKrnlString& kstrLoadableName() const
        {
            return m_kstrLoadName;
        }

        const TKrnlString& kstrPortableName() const
        {
            return m_kstrPortName;
        }

        const TKrnlString& kstrSrcPath() const
        {
            return m_kstrSrcPath;
        }


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCleanup();

        tCIDLib::TBoolean bPlatCleanup();

        tCIDLib::TBoolean bLoadPlatModByName
        (
            const   tCIDLib::EModTypes  eModType
        );

        tCIDLib::TBoolean bLoadPlatModByPath
        (
            const   tCIDLib::EModTypes      eModType
            , const tCIDLib::TCh* const     pszLoadPath
        );

        tCIDLib::TBoolean bQueryPlatModByName
        (
            const   tCIDLib::EModTypes      eModType
        );


        // -------------------------------------------------------------------
        //  Private, statiic methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid MakePlatNames
        (
             const  tCIDLib::TCh* const     pszModName
            ,       TKrnlString&            kstrPortable
            ,       TKrnlString&            kstrLoadable
            , const tCIDLib::TCard4         c4MajVer
            , const tCIDLib::TCard4         c4MinVer
            , const tCIDLib::EModTypes      eModType
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bViaLoad
        //      Indicates whether the module handle was obtained by loading
        //      the library or just querying the handle of an existing
        //      one.  If loaded, we free it when the handle is destroyed.
        //
        //  m_eModeType
        //      The module type.
        //
        //  m_hmodThis
        //      The 'handle' to the module controlled by this object.
        //
        //  m_pmiThis
        //      This is a pointer to the message file data for this module,
        //      if there are any. If not, then its null.
        //
        //  m_kstrBaseName
        //  m_kstrLoadName
        //  m_kstrPortNmame
        //      The base name is the basic facility name, when loading a CIDLib
        //      facility type module. When loading an external module it's
        //      the name extracted from the path to load. The load name is the
        //      platform specific loadable name. This is the same as base name if
        //      loading an external module. The portable name is only for CIDLib
        //      type modules and is the base name, plus versioning for libraries.
        //      For externals, it's not set.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bViaLoad;
        tCIDLib::EModTypes  m_eModType;
        TModuleHandle       m_hmodThis;
        tCIDLib::TMsgIndex* m_pmiThis;
        TKrnlString         m_kstrBaseName;
        TKrnlString         m_kstrLoadName;
        TKrnlString         m_kstrPortName;
        TKrnlString         m_kstrSrcPath;


        // -------------------------------------------------------------------
        //  Private, static data members
        //
        //  s_pszMsgNotFound
        //      The string that is substituted if a CIDLib message string
        //      cannot be loaded. It is defaulted to an English string if the
        //      text cannot be loaded.
        //
        //  s_pszResNotFound
        //      The string that is substituted if a resource string cannot
        //      be loaded. It is defaulted to an English string if the text
        //      cannot be loaded.
        //
        //  s_pszNoMsgFile
        //      The string that is substituted if an attempt is made to load
        //      a string from a module with no message file attached.
        // -------------------------------------------------------------------
        static const tCIDLib::TCh*  s_pszMsgNotFound;
        static const tCIDLib::TCh*  s_pszResNotFound;
        static const tCIDLib::TCh*  s_pszNoMessages;
};

#pragma CIDLIB_POPPACK


