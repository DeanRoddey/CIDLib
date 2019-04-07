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
//  module (Exe or Dll) and the things that you do via it.
//
//
// CAVEATS/GOTCHAS:
//
//  1)  LoadFromName() loads a module, and should be used for dynamic loading.
//      QueryFromName just queries the handle for an existing loaded module.
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
        static tCIDLib::TBoolean bBuildRawModName
        (
            const   tCIDLib::TCh* const     pszBaseName
            , const tCIDLib::TCard4         c4MajVersion
            , const tCIDLib::TCard4         c4MinVersion
            , const tCIDLib::EModTypes      eModType
            , const tCIDLib::TCard4         c4MaxChars
            ,       tCIDLib::TCh* const     pchToFill
        );

        static tCIDLib::TBoolean bRawQueryModName
        (
            const   TModuleHandle&          hmodToQuery
            ,       tCIDLib::TCh* const     pszNameToFill
            ,       tCIDLib::TCh* const     pszPathToFill
            , const tCIDLib::TCard4         c4MaxChars
        );

        static tCIDLib::TBoolean bLoadMessages
        (
            const   TModuleHandle&          hmodSrc
            ,       tCIDLib::TMsgIndex*&    pmiLoadMessages
        );

        static tCIDLib::TBoolean bLoadMessages
        (
            const   tCIDLib::TCh* const     pszPath
            , const tCIDLib::TCh* const     pszFile
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
            const   tCIDLib::TCh* const     pszModuleName
            , const tCIDLib::EModTypes      eModType
            , const tCIDLib::TCard4         c4MajVer
            , const tCIDLib::TCard4         c4MinVer
            , const tCIDLib::EModFlags      eModFlags
        );

        tCIDLib::TBoolean bLoadFromName
        (
            const   tCIDLib::TCh* const     pszFullPath
        );

        tCIDLib::TBoolean bQueryFromName
        (
            const   tCIDLib::TCh* const     pszModuleName
            , const tCIDLib::EModTypes      eModType
            , const tCIDLib::TCard4         c4MajVer
            , const tCIDLib::TCard4         c4MinVer
            , const tCIDLib::EModFlags      eModFlags
        );

        tCIDLib::TBoolean bQueryFromName
        (
            const   tCIDLib::TCh* const     pszFullPath
        );

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


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bClearHandle();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bViaLoad
        //      Indicates whether the module handle was obtained by loading
        //      the library or just querying the handle of an existing
        //      one.  If loaded, we free it when the handle is destroyed.
        //
        //  m_hmodThis
        //      The 'handle' to the module controlled by this object.
        //
        //  m_pmiThis
        //      This is a pointer to the message file data for this module,
        //      if there are any. If not, then its 0.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bViaLoad;
        TModuleHandle       m_hmodThis;
        tCIDLib::TMsgIndex* m_pmiThis;


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


