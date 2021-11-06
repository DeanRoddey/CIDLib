//
// FILE NAME: CIDMacroEng_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/11/2003
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
//  This file implements TFacCIDMacroEng class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDMacroEng_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDMacroEng,TFacility)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace
{
    namespace kCIDMacroEng_ThisFacility
    {
        // -----------------------------------------------------------------------
        //  A simple default class that is inserted when a new class is created.
        //  There is a replacement token that is used to put in the actual new
        //  class path. So it creates an initial valid and compilable class.
        // -----------------------------------------------------------------------
        constexpr const tCIDLib::TCh* pszDefClassText =
        (
            L"Class=[NonFinal]\n"
            L"    ClassPath %(1);\n"
            L"    ParentClass MEng.Object;\n"
            L"EndClass;\n"
            L"\n"
            L"Imports=\n"
            L"EndImports;\n"
            L"\n"
            L"Members=\n"
            L"EndMembers;\n"
            L"\n"
            L"\n"
            L"Methods=[Public,Final]\n"
            L"\n"
            L"    Constructor()\n"
            L"    Begin\n"
            L"    EndConstructor;\n"
            L"\n"
            L"    Method Start() Returns Int4\n"
            L"    Begin\n"
            L"        Return 0;\n"
            L"    EndMethod;\n"
            L"\n"
            L"EndMethods;\n"
        );
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngRTClassLoader
// PREFIX: mecl
//
//  This is a trivial derivative of the external class loader that handles
//  loading our built in runtime (but not intrinsic) classes. We always
//  install one of these on a new engine. Intrinsics are automatically loaded
//  while these runtime classes are only loaded if imported.
// ---------------------------------------------------------------------------
class TMEngRTClassLoader : public MMEngExtClassLoader
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngRTClassLoader() = default;

        TMEngRTClassLoader(const TMEngRTClassLoader&) = delete;
        TMEngRTClassLoader(TMEngRTClassLoader&&) = delete;

        ~TMEngRTClassLoader() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngRTClassLoader& operator=(const TMEngRTClassLoader&) = delete;
        TMEngRTClassLoader& operator=(TMEngRTClassLoader&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        TMEngClassInfo* pmeciLoadClass
        (
                    TCIDMacroEngine&        meTarget
            , const TString&                strClassPath
        )   final;
};


TMEngClassInfo*
TMEngRTClassLoader::pmeciLoadClass(         TCIDMacroEngine&    meTarget
                                    , const TString&            strClassPath)
{
    // If it doesn't start with the runtime root path, then obviously not
    if (!strClassPath.bStartsWith(TFacCIDMacroEng::strRuntimeClassPath))
        return 0;

    //
    //  NOTE:   Some of the core runtime classes are unconditionally loaded
    //          by the engine itself so we don't load them here. Mostly they
    //          are the tricky ones that have special needs.
    //
    TMEngClassInfo* pmeciRet = nullptr;
    if (strClassPath == TMEngAsyncHTTPClInfo::strPath())
        pmeciRet = new TMEngAsyncHTTPClInfo(meTarget);
    else if (strClassPath == TMEngAudioInfo::strPath())
        pmeciRet = new TMEngAudioInfo(meTarget);
    else if (strClassPath == TMEngASCIIInfo::strPath())
        pmeciRet = new TMEngASCIIInfo(meTarget);
    else if (strClassPath == TMEngBase64Info::strPath())
        pmeciRet = new TMEngBase64Info(meTarget);
    else if (strClassPath == TMEngBinFileInfo::strPath())
        pmeciRet = new TMEngBinFileInfo(meTarget);
    else if (strClassPath == TMEngCommPortInfo::strPath())
        pmeciRet = new TMEngCommPortInfo(meTarget);
    else if (strClassPath == TMEngCommCfgInfo::strPath())
        pmeciRet = new TMEngCommCfgInfo(meTarget);
    else if (strClassPath == TMEngConInStreamInfo::strPath())
        pmeciRet = new TMEngConInStreamInfo(meTarget);
    else if (strClassPath == L"MEng.System.Runtime.ConsoleOutStream")
        pmeciRet = new TMEngConOutStreamInfo(meTarget);
    else if (strClassPath == TMEngCryptoKeyInfo::strPath())
        pmeciRet = new TMEngCryptoKeyInfo(meTarget);
    else if (strClassPath == L"MEng.System.Runtime.DirIter")
        pmeciRet = new TMEngDirIterInfo(meTarget);
    else if (strClassPath == TMEngDGramSocketInfo::strPath())
        pmeciRet = new TMEngDGramSocketInfo(meTarget);
    else if (strClassPath == L"MEng.System.Runtime.DBConnect")
        pmeciRet = new TMEngDBConnInfo(meTarget);
    else if (strClassPath == L"MEng.System.Runtime.DBStatement")
        pmeciRet = new TMEngDBStmtInfo(meTarget);
    else if (strClassPath == TMEngIPEPInfo::strPath())
        pmeciRet = new TMEngIPEPInfo(meTarget);
    else if (strClassPath == TMEngFileInStreamInfo::strPath())
        pmeciRet = new TMEngFileInStreamInfo(meTarget);
    else if (strClassPath == L"MEng.System.Runtime.FileOutStream")
        pmeciRet = new TMEngFileOutStreamInfo(meTarget);
    else if (strClassPath == L"MEng.System.Runtime.FileSystem")
        pmeciRet = new TMEngFileSysInfo(meTarget);
    else if (strClassPath == TMEngHTTPClientInfo::strPath())
        pmeciRet = new TMEngHTTPClientInfo(meTarget);
    else if (strClassPath == TMEngJSONAnchorInfo::strPath())
        pmeciRet = new TMEngJSONAnchorInfo(meTarget);
    else if (strClassPath == TMEngJSONParserInfo::strPath())
        pmeciRet = new TMEngJSONParserInfo(meTarget);
    else if (strClassPath == TMEngKVPairInfo::strPath())
        pmeciRet = new TMEngKVPairInfo(meTarget);
    else if (strClassPath == TMEngMD5Info::strPath())
        pmeciRet = new TMEngMD5Info(meTarget);
    else if (strClassPath == L"MEng.System.Runtime.MD5Hash")
        pmeciRet = new TMEngMD5HashInfo(meTarget);
    else if (strClassPath == L"MEng.System.Runtime.NamedValMap")
        pmeciRet = new TMEngNamedValMapInfo(meTarget);
    else if (strClassPath == TMEngDataSrcInfo::strPath())
        pmeciRet = new TMEngDataSrcInfo(meTarget);
    else if (strClassPath == L"MEng.System.Runtime.RandomNum")
        pmeciRet = new TMEngRandomNumInfo(meTarget);
    else if (strClassPath == L"MEng.System.Runtime.SHA1Digest")
        pmeciRet = new TMEngSHA1Info(meTarget);
    else if (strClassPath == L"MEng.System.Runtime.SHA1Hash")
        pmeciRet = new TMEngSHA1HashInfo(meTarget);
    else if (strClassPath == TMEngSocketInfo::strPath())
        pmeciRet = new TMEngSocketInfo(meTarget);
    else if (strClassPath == TMEngSockPingerInfo::strPath())
        pmeciRet = new TMEngSockPingerInfo(meTarget);
    else if (strClassPath == TMEngSpeechInfo::strPath())
        pmeciRet = new TMEngSpeechInfo(meTarget);
    else if (strClassPath == TMEngStreamSocketInfo::strPath())
        pmeciRet = new TMEngStreamSocketInfo(meTarget);
    else if (strClassPath == TMEngStringInStreamInfo::strPath())
        pmeciRet = new TMEngStringInStreamInfo(meTarget);
    else if (strClassPath == TMEngStringTokInfo::strPath())
        pmeciRet = new TMEngStringTokInfo(meTarget);
    else if (strClassPath == L"MEng.System.Runtime.TextInStream")
        pmeciRet = new TMEngTextInStreamInfo(meTarget);
    else if (strClassPath == L"MEng.System.Runtime.TextXCoder")
        pmeciRet = new TMEngTextXCoderInfo(meTarget);
    else if (strClassPath == L"MEng.System.Runtime.SysInfo")
        pmeciRet = new TMEngSysInfoInfo(meTarget);
    else if (strClassPath == L"MEng.System.Runtime.USBHID")
        pmeciRet = new TMEngUSBHIDInfo(meTarget);
    else if (strClassPath == TMEngURLInfo::strPath())
        pmeciRet = new TMEngURLInfo(meTarget);
    else if (strClassPath == TMEngXMLTreeAnchorInfo::strPath())
        pmeciRet = new TMEngXMLTreeAnchorInfo(meTarget);
    else if (strClassPath == TMEngXMLTreeParserInfo::strPath())
        pmeciRet = new TMEngXMLTreeParserInfo(meTarget);
    else if (strClassPath == TMEngZLibInfo::strPath())
        pmeciRet = new TMEngZLibInfo(meTarget);

    return pmeciRet;
}




// ---------------------------------------------------------------------------
//  CLASS: TFacCIDMacroEng
// PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDMacroEng: Public, static data
// ---------------------------------------------------------------------------
const TString TFacCIDMacroEng::strFalseKeyword(L"False");
const TString TFacCIDMacroEng::strRootClassPath(L"MEng");
const TString TFacCIDMacroEng::strRuntimeClassPath(L"MEng.System.Runtime");
const TString TFacCIDMacroEng::strTrueKeyword(L"True");
const TString TFacCIDMacroEng::strUnknownClass(L"<Unknown");



// ---------------------------------------------------------------------------
//  TFacCIDMacroEng: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDMacroEng::TFacCIDMacroEng() :

    TFacility
    (
        L"CIDMacroEng"
        , tCIDLib::EModTypes::SharedLib
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
    , m_colLoaderList(tCIDLib::EAdoptOpts::Adopt, 8)
    , m_strEmptyClassTemplate(kCIDMacroEng_ThisFacility::pszDefClassText)
{
    // Load up some strings used in the file open dialog
    m_strMacroFileDescr.LoadFromMsg(kMEngMsgs::midFile_MacroDescr, *this);
    m_strMacroFileSelWC.LoadFromMsg(kMEngMsgs::midFile_MacroWildCard, *this);

    // Register default class loader that handles the standard runtime classes
    m_colLoaderList.Add(new TMEngRTClassLoader);
}

TFacCIDMacroEng::~TFacCIDMacroEng()
{
    // Flush the class loader list
    try
    {
        m_colLoaderList.RemoveAll();
    }

    catch(...)
    {
    }
}


// ---------------------------------------------------------------------------
//  TFacCIDMacroEng: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Note that we don't do any locking here. Apps must register their loaders
//  during init, before they spin up other threads that might start running
//  macors.
//
tCIDLib::TVoid
TFacCIDMacroEng::AddClassLoader(        MMEngExtClassLoader* const  pmeclToAdd
                                , const tCIDLib::EEnds              eEnd)
{
    // Lock the list while we do this
    TLocker lockrSync(&m_mtxSync);

    // Add the new loader where indicated
    if (eEnd == tCIDLib::EEnds::First)
        m_colLoaderList.InsertAt(pmeclToAdd, 0);
    else
        m_colLoaderList.Add(pmeclToAdd);
}


//
//  Checks that the passed CML path is at least syntactically correct. Doesn't mean that
//  the class actually exists. So it must:
//
//  1. Start with MEng. (we don't enforce case though typically it's a good thing to
//      insure it is just like this.)
//  2. Then consist of valid name tokens separated by periods
//
//  The parser/engine doesn't need this, but IDEs and such want to validate user entered
//  paths and provide helpful feedback on any issues.
//
//  If an empty path is OK, they have to tell us that.
//
tCIDLib::TBoolean
TFacCIDMacroEng::bIsValidCMLPath(const  TString&            strVal
                                ,       TString&            strErrText
                                , const tCIDLib::TBoolean   bEmptyOK) const
{
    const tCIDLib::TCard4 c4Count = strVal.c4Length();
    if (!c4Count)
    {
        if (bEmptyOK)
            return kCIDLib::True;

        strErrText = L"The macro path cannot be empty";
        return kCIDLib::False;
    }

    // We have something to parse, so let's do it
    tCIDLib::TCard4 c4TokenCnt = 0;
    TString strCurToken;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const tCIDLib::TCh chCur = strVal[c4Index];

        if (chCur == kCIDLib::chPeriod)
        {
            // The end of a token. It cannot be empty
            if (strCurToken.bIsEmpty())
            {
                //
                //  If the first token we have a specific eror msg, else this just
                //  means they had an empty token.
                //
                if (c4TokenCnt)
                    strErrText = L"A CML path component cannot be empty";
                else
                    strErrText = L"A CML path must start with MEng.";
                return kCIDLib::False;
            }

            // If the first token, it must be MEng
            if (!c4TokenCnt)
            {
                // Don't enforce case here, though it would be nice if we could
                if (!strCurToken.bCompareI(L"MEng"))
                {
                    strErrText = L"A CML path must start with MEng.";
                    return kCIDLib::False;
                }
            }

            // Clear the token for the next time
            strCurToken.Clear();
        }
         else
        {
            // Has to be a valid token character
            if (TRawStr::bIsAlphaNum(chCur)
            ||  (chCur == kCIDLib::chUnderscore)
            ||  (chCur == kCIDLib::chHyphenMinus))
            {
                // If it was empty, this is a new token so bump the count
                if (strCurToken.bIsEmpty())
                    c4TokenCnt++;

                // Looks ok so store it
                strCurToken.Append(chCur);
            }
             else
            {
                strErrText = L"CML path components must be alpha-numeric, underscore, or hyphen";
                return kCIDLib::False;
            }
        }
    }

    // Can't end with an empty token
    if (strCurToken.bIsEmpty())
    {
        strErrText = L"A CML path cannot end with a period separator";
        return kCIDLib::False;
    }

    //
    //  WE have to have seen at least two tokens, one is MEng and the other is some
    //  actual useful path component.
    //
    if (c4TokenCnt < 2)
    {
        strErrText = L"A CML path cannot be just MEng by itself";
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  We use 16 bit ids for various things, so this is a helper to watch for them
//  overflowing. The value we get is pre-increment, so we check if it's >=.
//
tCIDLib::TVoid
TFacCIDMacroEng::CheckIdOverflow(const  tCIDLib::TCard4     c4Next
                                , const tCIDLib::TCh* const pszName)
{
    if (c4Next >= kCIDLib::c2MaxCard)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcEng_IdOverflow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
            , TString(pszName)
        );
    }
}

//
//  Combines a base CML path with a class name, making sure that there is one and only
//  one separator (period in this case.)
//
tCIDLib::TVoid
TFacCIDMacroEng::MakeFullPath(  const   TString&    strBasePath
                                , const TString&    strName
                                ,       TString&    strToFill)
{
    strToFill = strBasePath;
    if (strToFill.chLast() != kCIDLib::chPeriod)
        strToFill.Append(kCIDLib::chPeriod);
    strToFill.Append(strName);
}


TMEngClassInfo* TFacCIDMacroEng
::pmeciInvokeExternalLoaders(       TCIDMacroEngine&    meCaller
                            , const TString&            strClassPathToLoad)
{
    // Lock the list while we do this
    TLocker lockrSync(&m_mtxSync);

    //
    //  Loop through the installed class loaders till we get a hit or run
    //  out of loaders.
    //
    TMEngClassInfo* pmeciRet = 0;
    const tCIDLib::TCard4 c4Count = m_colLoaderList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        // Ask this one if he can load it. If so, return that guy
        pmeciRet = m_colLoaderList[c4Index]->pmeciLoadClass(meCaller, strClassPathToLoad);
        if (pmeciRet)
            return pmeciRet;
    }
    return 0;
}


const TString& TFacCIDMacroEng::strMacroFileDescr() const
{
    return m_strMacroFileDescr;
}


const TString& TFacCIDMacroEng::strEmptyClassTemplate() const
{
    return m_strEmptyClassTemplate;
}


const TString& TFacCIDMacroEng::strMacroFileSelWC() const
{
    return m_strMacroFileSelWC;
}


tCIDLib::TVoid
TFacCIDMacroEng::SplitClassPath(const   TString&    strClassPath
                                ,       TString&    strBasePath
                                ,       TString&    strName)
{
    //
    //  Validate the class path first, which will also return to us the index
    //  of the last path component. If we don't find a period, then assume
    //  no name part.
    //
    tCIDLib::TCard4 c4LastPart;
    if (!strClassPath.bLastOccurrence(kCIDLib::chPeriod, c4LastPart))
    {
        strBasePath = strClassPath;
        strName.Clear();
        return;
    }

    strClassPath.CopyOutSubStr(strName, c4LastPart + 1);
    strClassPath.CopyOutSubStr(strBasePath, 0, c4LastPart);
}

