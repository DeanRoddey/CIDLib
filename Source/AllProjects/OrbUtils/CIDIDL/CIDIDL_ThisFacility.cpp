//
// FILE NAME: CIDIDL_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/08/2000
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
//  This file implements the more generic parts of the facility class for the
//  CID IDL compiler. Some private methods, which implement various specific
//  pieces of the server, are implemented in other files.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDIDL.hpp"


// ---------------------------------------------------------------------------
//   CLASS: TFacCIDIDL
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDIDL: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDIDL::TFacCIDIDL() :

    TFacility
    (
        L"CIDIDL"
        , tCIDLib::EModTypes::Exe
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
    , m_bGenClient(kCIDLib::False)
    , m_bGenGlobals(kCIDLib::False)
    , m_bGenServer(kCIDLib::False)
    , m_bNoLogo(kCIDLib::False)
    , m_bPrivHeader(kCIDLib::False)
    , m_eAction(tCIDIDL::EActions::None)
{
}

TFacCIDIDL::~TFacCIDIDL()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDIDL: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes TFacCIDIDL::eMainThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // Let our caller go
    thrThis.Sync();

    // Try to parse the parameters
    if (!bParseParams())
        return tCIDLib::EExitCodes::BadParameters;

    // Output the program logo
    if (!m_bNoLogo)
    {
        TSysInfo::strmOut() << strMsg(kIDLMsgs::midGen_Blurb, strVersion())
                            << kCIDLib::EndLn;
    }

    // And perform the requested action
    tCIDLib::EExitCodes eRet = tCIDLib::EExitCodes::Normal;
    try
    {
        if (m_eAction == tCIDIDL::EActions::IdGen)
        {
            GenId();
        }
         else if (m_eAction == tCIDIDL::EActions::CodeGen)
        {
            if (bParseInput())
            {
                //
                //  For now, just load up the generator list with all of the
                //  supported output generators.
                //
                GenCode();
            }
             else
            {
                eRet = tCIDLib::EExitCodes::RuntimeError;
            }
        }
         else
        {
            TSysInfo::strmErr() << L"Internal Err: Unknown action\n";
            eRet = tCIDLib::EExitCodes::RuntimeError;
        }
    }

    catch(TError& errToCatch)
    {
        // If doing verbose logging, and this hasn't been logged already...
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        TSysInfo::strmErr() << L"\nAn error occured during processing.\nError: \n";
        {
            TStreamIndentJan janIndent(&TSysInfo::strmErr(), 4);
            TSysInfo::strmErr() << errToCatch;
        }
        TSysInfo::strmErr() << kCIDLib::EndLn << kCIDLib::FlushIt;
        eRet = tCIDLib::EExitCodes::RuntimeError;
    }

    catch(...)
    {
        TSysInfo::strmErr() << L"\nAn unknown exception occured during processing\n"
                            << kCIDLib::FlushIt;
        eRet = tCIDLib::EExitCodes::RuntimeError;
    }
    return eRet;
}


tCIDLib::TVoid
TFacCIDIDL::GenErr( const   tCIDLib::TCh* const pszFile
                    , const tCIDLib::TCard4     c4Line
                    , const tCIDLib::TErrCode   errcToThrow)
{
    ThrowErr
    (
        pszFile
        , c4Line
        , errcToThrow
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::CantDo
    );
}


tCIDLib::TVoid
TFacCIDIDL::GenErr( const   tCIDLib::TCh* const pszFile
                    , const tCIDLib::TCard4     c4Line
                    , const tCIDLib::TErrCode   errcToThrow
                    , const MFormattable&       fmtblToken1)
{
    ThrowErr
    (
        pszFile
        , c4Line
        , errcToThrow
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::CantDo
        , fmtblToken1
    );
}


tCIDLib::TVoid
TFacCIDIDL::GenErr( const   tCIDLib::TCh* const pszFile
                    , const tCIDLib::TCard4     c4Line
                    , const tCIDLib::TErrCode   errcToThrow
                    , const MFormattable&       fmtblToken1
                    , const MFormattable&       fmtblToken2)
{
    ThrowErr
    (
        pszFile
        , c4Line
        , errcToThrow
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::CantDo
        , fmtblToken1
        , fmtblToken2
    );
}



// ---------------------------------------------------------------------------
//  TFacCIDIDL: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TFacCIDIDL::bLoadMapping(TString& strMapping)
{
    //
    //  There should be one and only one = sign and it must not be first
    //  or last char.
    //
    tCIDLib::TCard4 c4Ofs;
    if (!strMapping.bFirstOccurrence(kCIDLib::chEquals, c4Ofs)
    ||  !c4Ofs
    ||  (c4Ofs == (strMapping.c4Length() - 1)))
    {
        TSysInfo::strmErr() << L"\nThe mapping '" << strMapping
                            << L"' was not well formed"
                            << kCIDLib::EndLn;
        return kCIDLib::False;
    }

    //
    //  Break out the two parts into separate strings. The incoming string is
    //  not const, so we can keep the base part there, and copy the other part
    //  out to a second string.
    //
    TString strSysId;
    strMapping.CopyOutSubStr(strSysId, c4Ofs + 1);
    strMapping.CapAt(c4Ofs);

    // And lets use this to add a mapping to the parser instance
    TFileEntitySrc* pxesNew = new TFileEntitySrc(strSysId);
    pxesNew->strPublicId(strMapping);
    m_xtprsInput.AddMapping(pxesNew);
    return kCIDLib::True;
}


tCIDLib::TBoolean TFacCIDIDL::bParseInput()
{
    //
    //  Add one magic entity mapping to map the public id for the IDL DTD
    //  to our hard coded internal DTD text.
    //
    m_xtprsInput.AddMapping
    (
        new TMemBufEntitySrc
        (
            L"http://www.charmedquark.com/CIDIDL/CIDIDL.DTD"
            , L"urn:charmedquark.com:XML-CIDIDL.Dtd"
            , TFacCIDIDL::s_strEmbeddedDTDText
        )
    );

    // Try to parse the input file
    const tCIDLib::TBoolean bOk = m_xtprsInput.bParseRootEntity
    (
        m_strInputFile
        , tCIDXML::EParseOpts::Validate
        , tCIDLib::eOREnumBits
          (
            tCIDXML::EParseFlags::Chars, tCIDXML::EParseFlags::Tags
          )
    );

    if (!bOk)
    {
        //
        //  Format the first error in the error list, into a string that we
        //  can pass as a replacement parameter to the real error we log.
        //
        const TXMLTreeParser::TErrInfo& erriFirst = m_xtprsInput.colErrors()[0];

        TSysInfo::strmErr()
                    << L"Input XML is in error.\n  ["
                    << erriFirst.strSystemId() << L"/"
                    << erriFirst.c4Line() << L"." << erriFirst.c4Column()
                    << L"] " << erriFirst.strText() << kCIDLib::EndLn;
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TFacCIDIDL::bParseParams()
{
    // We have to at least get the action
    TSysInfo::TCmdLineCursor cursParms = TSysInfo::cursCmdLineParms();
    if (!cursParms.bIsValid())
    {
        ShowUsage();
        return kCIDLib::False;
    }


    // The first parameter must be the action, so get it and bump the cursor
    const TString& strCmd = *cursParms++;
    if (strCmd.bCompareI(L"IdGen"))
    {
        // There cannot be any more parameters
        if (cursParms.bIsValid())
        {
            ShowUsage();
            return kCIDLib::False;
        }

        m_eAction = tCIDIDL::EActions::IdGen;
    }
     else if (strCmd.bCompareI(L"CodeGen"))
    {
        // At minimum we have to have an input and client or server settings
        if (TSysInfo::c4CmdLineParmCount() < 2)
        {
            ShowUsage();
            return kCIDLib::False;
        }

        // Get the input file
        m_strInputFile = *cursParms++;

        // And then any options
        for (; cursParms; ++cursParms)
        {
            // Get the param
            TString strParm = *cursParms;

            if ((strParm.bStartsWithI(L"/Server="))
            ||  (strParm.bStartsWithI(L"/Client="))
            ||  (strParm.bStartsWithI(L"/Global=")))
            {
                TString strSavePref;
                strParm.CopyOutSubStr(strSavePref, 0, 8);
                strSavePref.ToUpper();
                strParm.Cut(0, 8);

                // Parse out the information encoded in the string
                TStringTokenizer stokTmp(&strParm, L";");
                stokTmp.Reset();

                TString strFacName;
                if (!stokTmp.bGetNextToken(strFacName))
                {
                    ShowUsage();
                    return kCIDLib::False;
                }

                TString strOutDir;
                if (!stokTmp.bGetNextToken(strOutDir))
                {
                    ShowUsage();
                    return kCIDLib::False;
                }

                TString strOptName;
                if (stokTmp.bGetNextToken(strOptName))
                {
                    if (stokTmp.bMoreTokens())
                    {
                        ShowUsage();
                        return kCIDLib::False;
                    }
                }
                 else
                {
                    // The name is not optional for globals
                    if (strSavePref.bStartsWith(L"/GLOBAL="))
                    {
                        ShowUsage();
                        return kCIDLib::False;
                    }
                }

                if (strSavePref.bStartsWith(L"/SERVER="))
                {
                    m_bGenServer = kCIDLib::True;
                    m_strServerFacName = strFacName;
                    m_strServerOutputDir = strOutDir;
                    m_strServerOptName = strOptName;
                }
                 else if (strSavePref.bStartsWith(L"/CLIENT="))
                {
                    m_bGenClient = kCIDLib::True;
                    m_strClientFacName = strFacName;
                    m_strClientOutputDir = strOutDir;
                    m_strClientOptName = strOptName;
                }
                 else if (strSavePref.bStartsWith(L"/GLOBAL="))
                {
                    m_bGenGlobals = kCIDLib::True;
                    m_strGlobalsFacName = strFacName;
                    m_strGlobalsOutputDir = strOutDir;
                    m_strGlobalsName = strOptName;
                }
            }
             else if (strParm.bStartsWithI(L"/CExport="))
            {
                strParm.Cut(0, 9);
                m_strCExportKeyword = strParm;
            }
             else if (strParm.bStartsWithI(L"/SExport="))
            {
                strParm.Cut(0, 9);
                m_strSExportKeyword = strParm;
            }
             else if (strParm.bStartsWithI(L"/GExport="))
            {
                strParm.Cut(0, 9);
                m_strGExportKeyword = strParm;
            }
             else if (strParm.bCompareI(L"/PrivHdr"))
            {
                m_bPrivHeader = kCIDLib::True;
            }
             else if (strParm.bCompareI(L"/NoLogo"))
            {
                m_bNoLogo = kCIDLib::True;
            }
             else if (strParm.bStartsWithI(L"/Mapping="))
            {
                //
                //  Parse out this mapping and add it to the XML parser
                //  instance. They are in the form:
                //
                //      /Mapping=pubid=sysid
                //
                //  so its a mapping of a public id to a system id that will
                //  be added to the parser, which will add them to an internal
                //  catalog. It will then do automatic entity resolution for us.
                //
                strParm.Cut(0, 9);
                if (!bLoadMapping(strParm))
                    return kCIDLib::False;
            }
             else
            {
                ShowUsage();
                return kCIDLib::False;
            }
        }

        // And set the action to code gen
        m_eAction = tCIDIDL::EActions::CodeGen;
    }
     else
    {
        ShowUsage();
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


//
//  This method handles the IdGen action. Its very simple so we just do it
//  here in this file. It uses the standard CIDCrypto unique id generation
//  class.
//
tCIDLib::TVoid TFacCIDIDL::GenId()
{
    TString strId = TUniqueId::strMakeId();
    TSysInfo::strmOut() << kCIDLib::NewLn << strMsg(kIDLMsgs::midGen_NewId)
                        << strId << kCIDLib::EndLn;
}


tCIDLib::TVoid TFacCIDIDL::ShowUsage()
{
    TSysInfo::strmErr()
                << L"\nUsage:\n"
                   L"  CIDIDL IdGen\n"
                   L"  CIDIDL CodeGen srcfile [/NoLogo /Server=xxx /Client=xxx /Global=ggg]\n"
                   L"  xxx = facname;targetdir[;optname]\n"
                   L"  ggg = facname;targetdir;name\n"
                   L" Example:\n"
                   L"  CIDIDL CodeGen /Server=MyDVDServer;.\\Server\\"
                   L" /Client=MyDVDClient;.\\Client/\n\n"
                   L"For CodeGen, one of the server, client or globals output "
                   L"settings parameters must be provided.\n"
                << kCIDLib::EndLn;
}


