//
// FILE NAME: CIDKernel_Module.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/22/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file provides the platform independent implementation of the
//  TKrnlModule class, the core wrapper class for module handles. Most of
//  the implementation is platform dependent and is in the per-platform
//  directory in the CIDKernel_Module_Win32.Cpp file.
//
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
#include    "CIDKernel_.hpp"



// ---------------------------------------------------------------------------
//   CLASS: TKrnlModule
//  PREFIX: kmod
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlModule: Static data members
//
//  The text message pointers are initialized to default english messages in
//  case we cannot load our message file.
// ---------------------------------------------------------------------------
const tCIDLib::TCh* TKrnlModule::s_pszMsgNotFound = kCIDKernel_::pszMsgNotFound;
const tCIDLib::TCh* TKrnlModule::s_pszResNotFound = kCIDKernel_::pszResNotFound;
const tCIDLib::TCh* TKrnlModule::s_pszNoMessages  = kCIDKernel_::pszNoMsgFile;



// ---------------------------------------------------------------------------
//  TKrnlModule: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlModule::bLoadMessages( const   TModuleHandle&          hmodSrc
                            ,       tCIDLib::TMsgIndex*&    pmiToFill)
{
    // Assume we won't find anything
    pmiToFill = 0;

    //
    //  Get the name of the module, which we will use to build the name to
    //  the message file, which should be in the same directory and have the
    //  same base part of the name (to which we add stuff.)
    //
    const tCIDLib::TCard4 c4NameChars = kCIDLib::c4MaxPathLen;
    tCIDLib::TCh szModPath[c4NameChars+1];
    tCIDLib::TCh szModName[c4NameChars+1];
    if (!bRawQueryModName(hmodSrc, szModName, szModPath, c4NameChars))
        return kCIDLib::False;
    return bLoadMessages(szModPath, szModName, pmiToFill);
}


tCIDLib::TBoolean
TKrnlModule::bLoadMessages( const   tCIDLib::TCh* const     pszPath
                            , const tCIDLib::TCh* const     pszFile
                            ,       tCIDLib::TMsgIndex*&    pmiToFill)
{
    //
    //  Lets build up the array of up to 3 languages suffixes that we will
    //  try to load. We optimize by first putting in the default locale's
    //  prefered language. Then we put in the locale's default language
    //  if its not the same as the prefered. Then we put in the english
    //  suffix if neither of the other two were english.
    //
    tCIDLib::TCard4 c4SuffixCount = 0;
    const tCIDLib::TCh* apszSuffixes[3];

    // We always start with the prefered locale language first
    TKrnlLocale::bLangSuffix(apszSuffixes[c4SuffixCount]);
    c4SuffixCount++;

    // See if the default is any different and take it next if so
    if (TKrnlLocale::eLanguage() != TKrnlLocale::eDefLanguage())
    {
        TKrnlLocale::bDefLangSuffix(apszSuffixes[c4SuffixCount]);
        c4SuffixCount++;
    }

    // And if neither was English, add it last as a fallback
    if ((TKrnlLocale::eLanguage() != tCIDLib::ELanguages::English)
    &&  (TKrnlLocale::eDefLanguage() != tCIDLib::ELanguages::English))
    {
        TKrnlLocale::bLangSuffixFor
        (
            tCIDLib::ELanguages::English
            , apszSuffixes[c4SuffixCount]
        );
        c4SuffixCount++;
    }

    //
    //  Ok, now lets go through these suffixes until we get a hit or we
    //  fail to find anything.
    //
    tCIDLib::TBoolean       bFoundIt = kCIDLib::False;
    tCIDLib::TCard4         c4Index;
    TKrnlFile               kflMsgs;
    for (c4Index = 0; c4Index < c4SuffixCount; c4Index++)
    {
        //
        //  Lets first try to find the message file for the current
        //  language suffix in the list.
        //
        tCIDLib::TCh szFileName[kCIDLib::c4MaxPathLen + 1];
        TRawStr::CopyCatStr
        (
            szFileName
            , kCIDLib::c4MaxPathLen
            , pszPath
            , L"\\"
            , pszFile
        );
        TRawStr::CatStr(szFileName, L"_", kCIDLib::c4MaxPathLen);
        TRawStr::CatStr(szFileName, apszSuffixes[c4Index], kCIDLib::c4MaxPathLen);
        TRawStr::CatStr(szFileName, L".", kCIDLib::c4MaxPathLen);
        TRawStr::CatStr(szFileName, kCIDLib::szBinMsgFileExt, kCIDLib::c4MaxPathLen);

        if (kflMsgs.bSetName(szFileName))
        {
            if (kflMsgs.bOpen
            (
                tCIDLib::EAccessModes::Multi_Read
                , tCIDLib::ECreateActs::OpenIfExists
                , tCIDLib::EFilePerms::None
                , tCIDLib::EFileFlags::None))
            {
                bFoundIt = kCIDLib::True;
                break;
            }
        }
    }

    // Didn't find any so return failure
    if (!bFoundIt)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcMod_NoMsgs);
        return kCIDLib::False;
    }

    //
    //  See now large the file contents is. If we cannot, then return that
    //  as a general failure to load the message file.
    //
    tCIDLib::TCard8 c8Size;
    if (!kflMsgs.bQueryCurSize(c8Size))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcMod_LoadMsgs);
        return kCIDLib::False;
    }

    // The file must be at least large enough to hold the header info
    if (c8Size < sizeof(tCIDLib::TMsgIndex))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcMod_BadMsgFileFormat);
        return kCIDLib::False;
    }

    //
    //  It looks at least to be enough to have the header in there. So
    //  allocate a buffer and put a janitor on it, just in case we fail.
    //
    const tCIDLib::TCard4 c4BufSize = static_cast<tCIDLib::TCard4>(c8Size);
    tCIDLib::TCard1* pszBuf = new tCIDLib::TCard1[c4BufSize];
    TArrayJanitor<tCIDLib::TCard1> janBuf(pszBuf);

    //
    //  Read in the data. If cannot read it, then return that as a general
    //  failure to load the message file.
    //
    tCIDLib::TCard4 c4Read;
    if (!kflMsgs.bReadBuffer(pszBuf, c4BufSize, c4Read))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcMod_LoadMsgs);
        return kCIDLib::False;
    }

    // Make sure that we read as much as we asked for
    if (c4Read != c4BufSize)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcMod_NotAllMsgFile);
        return kCIDLib::False;
    }

    // Ok so far, so cast the buffer to the right type into a temp pointer
    tCIDLib::TMsgIndex* pmiTmp = reinterpret_cast<tCIDLib::TMsgIndex*>(pszBuf);

    //
    //  Do some basic validation of the contents to make sure that its a
    //  reasonable message file. We do this by calculating the size that
    //  the file should be, given what its contents indicate. This is done
    //  in a couple of steps with sanity checks along the way.
    //
    tCIDLib::TCard4 c4CalcSize = sizeof(tCIDLib::TCard4);
    c4CalcSize += (pmiTmp->c4MsgCount * sizeof(tCIDLib::TMsgIndexEntry));

    //
    //  The file must be at least large enough to hold the header and the
    //  index entries. If not, it cannot be right.
    //
    if (c4BufSize < c4CalcSize)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcMod_BadMsgFileFormat);
        return kCIDLib::False;
    }

    //
    //  Its big enough to hold the header and index, so lets go through the
    //  indexes and add up the size of the text data.
    //
    for (c4Index = 0; c4Index < pmiTmp->c4MsgCount; c4Index++)
    {
        // Calc the bytes needed for the text of this entry
        const tCIDLib::TCard4 c4TextSize =
        (
            pmiTmp->amieList[c4Index].c2TextChars * kCIDLib::c4CharBytes
        );

        //
        //  If the text offset plus the text size of any entry is larger
        //  than the file size, it cannot be right.
        //
        const tCIDLib::TCard4 c4EndOfText =
                        pmiTmp->amieList[c4Index].c4TextOffset + c4TextSize;

        if (c4EndOfText > c4BufSize)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcMod_BadMsgFileFormat);
            return kCIDLib::False;
        }

        // Add the text size to the accumulator
        c4CalcSize += c4TextSize;
    }

    // If our calculated size does not match the read in size, its bad
    if (c4CalcSize != c4BufSize)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcMod_BadMsgFileFormat);
        return kCIDLib::False;
    }

    // It went ok, so let the janitor go and return the pointer
    janBuf.Orphan();
    pmiToFill = pmiTmp;
    return kCIDLib::True;
}


const tCIDLib::TCh*
TKrnlModule::pszLoadCIDMsg( const   tCIDLib::TMsgIndex& miSrc
                            , const tCIDLib::TMsgId     midToLoad
                            ,       tCIDLib::TBoolean&  bLoaded)
{
    // Assume a failure, since we are pessimists
    bLoaded = kCIDLib::False;

    for (tCIDLib::TCard4 c4Index = 0; c4Index < miSrc.c4MsgCount; c4Index++)
    {
        //
        //  See if its id matches our target id. If so, then this is
        //  our guy.
        //
        if (miSrc.amieList[c4Index].midThis == midToLoad)
        {
            // Get a pointer to the base resource data
            const tCIDLib::TCh* pszRet = (const tCIDLib::TCh*)&miSrc;

            // And bump it up to the offset of the text for this entry.
            pszRet = tCIDLib::pOffsetPtr<tCIDLib::TCh>
            (
                pszRet, miSrc.amieList[c4Index].c4TextOffset
            );

            // Indicate that we've found it
            bLoaded = kCIDLib::True;

            // Return a pointer to this entry
            return pszRet;
        }
    }

    // Say it was not found
    return s_pszMsgNotFound;
}



// ---------------------------------------------------------------------------
//  TKrnlModule: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlModule::bHasMessageFile() const
{
    return (m_pmiThis != 0);
}


tCIDLib::TBoolean
TKrnlModule::bLoadCIDFacMsg(const   tCIDLib::TMsgId     midToLoad
                            ,       tCIDLib::TCh* const pszTarget
                            , const tCIDLib::TCard4     c4MaxChars) const
{
    tCIDLib::TBoolean bLoaded;
    const tCIDLib::TCh* pszTmp = pszLoadCIDFacMsg(midToLoad, bLoaded);
    TRawStr::CopyStr(pszTarget, pszTmp, c4MaxChars);
    return bLoaded;
}


const TModuleHandle& TKrnlModule::hmodThis() const
{
    return m_hmodThis;
}


//
//  These load up a loadable text message from the loadable text file
//  associated with this facliity. They just turn around and call a static
//  and pass long our message index.
//
const tCIDLib::TCh*
TKrnlModule::pszLoadCIDFacMsg(const tCIDLib::TMsgId midToLoad) const
{
    //
    //  If the message data pointer is 0, then there is no messages for this
    //  module so we return a default message. We cannot throw exceptions
    //  here because that would almost certainly kick off a circle jerk.
    //
    if (!m_pmiThis)
        return s_pszMsgNotFound;
    tCIDLib::TBoolean bDummy;
    return pszLoadCIDMsg(*m_pmiThis, midToLoad, bDummy);
}

const tCIDLib::TCh*
TKrnlModule::pszLoadCIDFacMsg(  const   tCIDLib::TMsgId     midToLoad
                                ,       tCIDLib::TBoolean&  bLoaded) const
{
    //
    //  If the message data pointer is 0, then there is no messages for this
    //  module so we return a default message. We cannot throw exceptions
    //  here because that would almost certainly kick off a circle jerk.
    //
    if (!m_pmiThis)
        return s_pszMsgNotFound;
    return pszLoadCIDMsg(*m_pmiThis, midToLoad, bLoaded);
}

