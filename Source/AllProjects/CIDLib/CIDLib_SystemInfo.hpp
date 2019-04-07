//
// FILE NAME: CIDLib_SystemInfo.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/25/1992
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
//  This is the header for the CIDLib_SystemInfo.Cpp file which implements
//  the TSysInfo class. This class provides access to most of the system
//  wide environmental information available via CIDLib.
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
//  CLASS: TSysInfo
// PREFIX: sysi
// ---------------------------------------------------------------------------
class CIDLIBEXP TSysInfo : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public types
        // -------------------------------------------------------------------
        using TCmdLineList   = TVector<TString>;
        using TCmdLineCursor = TVector<TString>::TConstCursor<TString>;


        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static inline tCIDLib::TBoolean bBigEndian()
        {
            #if defined(CIDLIB_BIGENDIAN)
            return kCIDLib::True;
            #else
            return kCIDLib::False;
            #endif
        }

        static tCIDLib::TBoolean bIsHostAdmin();

        static tCIDLib::TBoolean bInstallMode();

        static inline tCIDLib::TBoolean bLittleEndian()
        {
            #if defined(CIDLIB_LITTLEENDIAN)
            return kCIDLib::True;
            #else
            return kCIDLib::False;
            #endif
        }

        static tCIDLib::TBoolean bQueryLatLong
        (
                    tCIDLib::TFloat8&       f8Lat
            ,       tCIDLib::TFloat8&       f8Long
        );

        static tCIDLib::TBoolean bTestMode();

        static tCIDLib::TBoolean bTestMode
        (
            const   tCIDLib::TBoolean       bNewState
        );

        static tCIDLib::TCard4 c4CmdLineParmCount();

        static tCIDLib::TCard4 c4CPUCount();

        static tCIDLib::TCard8 c8AvailPhysicalMem();

        static tCIDLib::TCard8 c8TotalPhysicalMem();

        static const TCmdLineList& colCmdLineParms();

        static TCmdLineCursor cursCmdLineParms();

        static tCIDLib::ESeverities eDefModuleLog
        (
            const   TString&                strModule
        );

        static tCIDLib::ECPUTypes eCPUType();

        static inline tCIDLib::EEndianModes eEndianMode()
        {
            #if defined(CIDLIB_LITTLEENDIAN)
            return tCIDLib::EEndianModes::Little;
            #else
            return tCIDLib::EEndianModes::Big;
            #endif
        }

        static tCIDLib::TVoid QueryOSInfo
        (
                    tCIDLib::TCard4&        c4OSMajVersion
            ,       tCIDLib::TCard4&        c4OSMinVersion
            ,       tCIDLib::TCard4&        c4OSRev
            ,       tCIDLib::TCard4&        c4OSBuildNum
        );

        static tCIDLib::TVoid QueryOSInfoStr
        (
                    TString&                strToFill
        );

        static tCIDLib::TVoid RemoveCmdLineParm
        (
                    TCmdLineCursor&         cursAt
        );

        static TTextOutStream& strmErr();

        static TTextInStream& strmIn();

        static TTextOutStream& strmOut();

        static const TString& strCmdLineParmAt
        (
            const   tCIDLib::TCard4         c4At
        );

        static const TString& strOSHostName();

        static const TString& strIPHostName();

        static const TString& strNSAddr();

        static const TString& strSpecialPath
        (
            const   tCIDLib::ESpecialPaths  ePath
        );

        static const TString& strUserName();

        static tCIDLib::TVoid SetNSAddr
        (
            const   TString&                strDNSAddr
            , const tCIDLib::TIPPortNum     ippnToUse
        );


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TModule;


        // -------------------------------------------------------------------
        //  Hidden stuff
        // -------------------------------------------------------------------
        TSysInfo();

        TSysInfo(const TSysInfo&) = delete;

        ~TSysInfo();

        TSysInfo& operator=(const TSysInfo&) = delete;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        static const tCIDLib::TCh* pszLogInfo();


    private :
        // -------------------------------------------------------------------
        //  Private class types
        // -------------------------------------------------------------------
        using TSpecPathList = TEArray
        <
            TString*, tCIDLib::ESpecialPaths, tCIDLib::ESpecialPaths::Count
        >;


        // -------------------------------------------------------------------
        //  Private, non-virtaul methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid DoInit();


        // -------------------------------------------------------------------
        //  Private static, data members
        //
        //  s_apstrSpecPaths
        //      An array of pointers to strings that are faulted in upon access of
        //      any one of them. They provide access to the special paths, and
        //      correspond to the values of the special paths enum.
        //
        //  s_bInstallMode
        //      If the command line has the /INSTALL parm, it is removed and this
        //      flag is set. This allows programs to support an install mode. It is
        //      not interpreted by CIDLib.
        //
        //  s_bTestMode
        //      If the command line has the /TESTMODE parm, it is removed
        //      and this flag is set. This flag being set causes the logging
        //      code not to abort when a fatal error is logged.
        //
        //  s_pcolCmdLineParms
        //      We build up a vector of command line params, starting after the
        //      magic 0th one. It will only include the non-magic parametres.
        //
        //  s_pstrDefModuleLog
        //      If the command line has the /MODULELOG= parm or the
        //      CID_MODULELOG environment var. It is removed and stored here
        //      for later use by eDefModuleLog().
        //
        //  s_pstrIPHostName
        //      The IP level (DNS) host name of the local computer.

        //  s_pstrOSHostName
        //      The OS level (NetBIOS in our case) host name of the local computer.
        //
        //  s_pstrNSAddr
        //      This is the name server end point address. It can be set via
        //      either the CID_NSADDR environment variable, or via the /NSAddr=
        //      command line parameter. The latter overrides if both are
        //      provided.
        //
        //  s_pstrUserName
        //      The name of the current user.
        //
        //  s_pstrmErr
        //  s_pstrmIn
        //  s_pstrmOut
        //      These are the lazily faulted in standard streams. These are accessed
        //      via the strmXX() methods, and will be pointed to either a real console
        //      or a file stream, according to whether the stream has been redirected
        //      or not. If not redirected, the out and error streams will be the same
        //      output console object.
        //
        //  s_pszLogInfo
        //      If the command line has the /LOCALLOG= parm, it is removed
        //      and the part past the equal sign is stored here. The logging
        //      code will interpret this to set up a logger.
        // -------------------------------------------------------------------
        static TSpecPathList        s_apstrSpecPaths;
        static tCIDLib::TBoolean    s_bInstallMode;
        static tCIDLib::TBoolean    s_bTestMode;
        static TVector<TString>*    s_pcolCmdLineParms;
        static TString*             s_pstrDefModuleLog;
        static TString*             s_pstrIPHostName;
        static TString*             s_pstrOSHostName;
        static TString*             s_pstrNSAddr;
        static TString*             s_pstrUserName;
        static TTextOutStream*      s_pstrmErr;
        static TTextInStream*       s_pstrmIn;
        static TTextOutStream*      s_pstrmOut;
        static const tCIDLib::TCh*  s_pszLogInfo;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSysInfo,TObject)
};

#pragma CIDLIB_POPPACK

