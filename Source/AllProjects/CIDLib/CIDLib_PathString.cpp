//
// FILE NAME: CIDLib_PathString.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/27/1993
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements a path string class. It is basically a string
//  with added methods to manipulate the path components.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
AdvRTTIDecls(TPathStr,TString)


// ---------------------------------------------------------------------------
//   CLASS: TPathStr
//  PREFIX: path
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TPathStr: Constructors and Destructor
// ---------------------------------------------------------------------------
TPathStr::TPathStr() :

    TString(kCIDLib::c4MaxPathLen)
{
}

TPathStr::TPathStr(const TString& strInitVal) :

    TString(strInitVal)
{
}

TPathStr::TPathStr(const tCIDLib::TCard4 c4InitSize) :

    TString(c4InitSize)
{
}

TPathStr::TPathStr(const TString& strFirstPart, const TString& strSecPart) :

    TString(strFirstPart)
{
    AddLevel(strSecPart);
}

TPathStr::TPathStr(const tCIDLib::TCh* const pszInitValue) :

    TString(pszInitValue)
{
}

TPathStr::~TPathStr()
{
}


// ---------------------------------------------------------------------------
//  TPathStr: Public, non-virtual
// ---------------------------------------------------------------------------

//
//  Add a new directory level into this path, dealing with separators
//  appropriately.
//
tCIDLib::TVoid TPathStr::AddLevel(const TString& strNewLevel)
{
    // If there is no text in the new level, then just return
    if (!strNewLevel.c4Length())
        return;

    //
    //  If anything in this string object, then see if we or the new level
    //  has the separator. If not add one. If both have it, get rid one of.
    //
    if (strNewLevel.c4Length())
    {
        if ((chLast() == kCIDLib::chBackSlash)
        &&  (strNewLevel.chFirst() == kCIDLib::chBackSlash))
        {
            DeleteLast();
        }
         else if ((chLast() != kCIDLib::chBackSlash)
              &&  (strNewLevel.chFirst() != kCIDLib::chBackSlash))
        {
            if (!bIsEmpty())
                Append(kCIDLib::chBackSlash);
        }
    }
    Append(strNewLevel);
}


// These are just conveniences for multiple calls to AddLevel
tCIDLib::TVoid
TPathStr::AddLevels(const TString& strNewLevel1, const TString& strNewLevel2)
{
    AddLevel(strNewLevel1);
    AddLevel(strNewLevel2);
}

tCIDLib::TVoid
TPathStr::AddLevels(const   TString& strNewLevel1
                    , const TString& strNewLevel2
                    , const TString& strNewLevel3)
{
    AddLevel(strNewLevel1);
    AddLevel(strNewLevel2);
    AddLevel(strNewLevel3);
}


// Add our current contents to the end of the passed base path
tCIDLib::TVoid TPathStr::AddToBasePath(const TString& strBasePath)
{
    //
    //  If we are fully qualified, then we don't take anything from the base
    //  path. Else, check for a another special case. If we start with a
    //  separator, then we will only keep any volume info from the base path.
    //  Else, we do the standard scenario.
    //
    if (bIsEmpty())
    {
        *this = strBasePath;
    }
     else if (!bIsFullyQualified())
    {
        const TPathStr pathTmp(*this);
        if (chFirst() == kCIDLib::chBackSlash)
        {
            //
            //  If there is some volume, tack ourself onto that, else we don't
            //  change.
            //
            TPathStr pathTmpBase(strBasePath);
            if (pathTmpBase.bQueryVolume(*this))
                AddLevel(pathTmp);
            else
                *this = pathTmp;
        }
         else
        {
            // Just add ourselves to the base
            *this = strBasePath;
            AddLevel(pathTmp);
        }
    }

    // And now normalize
    Normalize();
}


tCIDLib::TVoid TPathStr::AddTrailingSeparator()
{
    if (chLast() != kCIDLib::chBackSlash)
        Append(kCIDLib::chBackSlash);
}


tCIDLib::TVoid TPathStr::AppendExt(const TString& strExt)
{
    if (strExt.bIsEmpty())
        return;

    tCIDLib::TBoolean bPerEnd   = (chLast() == L'.');
    tCIDLib::TBoolean bExtStart = (strExt.chFirst() == L'.');

    if (bPerEnd && bExtStart)
    {
        DeleteLast();
        bPerEnd = kCIDLib::False;
    }

    if (!bPerEnd && !bExtStart)
        Append(L'.');

    Append(strExt);
}


tCIDLib::TBoolean TPathStr::bExtractExt(TString& strBuf)
{
    tCIDLib::TCard4     c4End;
    tCIDLib::TCard4     c4Start;
    const tCIDLib::TCh* pszBuf = pszBuffer();

    if (!TKrnlPathStr::bFindPart(pszBuf, c4Start, c4End, tCIDLib::EPathParts::Extension))
        return kCIDLib::False;

    // Copy that substring into the caller's buffer
    if (c4End == kCIDLib::c4MaxCard)
        strBuf.CopyInSubStr(pszBuf, c4Start, c4End);
    else
        strBuf.CopyInSubStr(pszBuf, c4Start, (c4End - c4Start) + 1);

    //
    //  And cap this string at the start of the extension. Actually we have to
    //  move one back to get the separator.
    //
    CapAt(c4Start - 1);
    return kCIDLib::True;
}


tCIDLib::TBoolean TPathStr::bExtractNameExt(TString& strBuf)
{
    tCIDLib::TCard4     c4End;
    tCIDLib::TCard4     c4Start;
    const tCIDLib::TCh* pszBuf = pszBuffer();

    if (!TKrnlPathStr::bFindPart(pszBuf, c4Start, c4End, tCIDLib::EPathParts::Name))
        return kCIDLib::False;

    // Copy from the start to the end of the string
    strBuf.CopyInSubStr(pszBuf, c4Start, kCIDLib::c4MaxCard);

    //
    //  And cap this string at the start of the name. If there is any path parth,
    //  this leaves it with a trailing separator.
    //
    CapAt(c4Start);
    return kCIDLib::True;
}


tCIDLib::TBoolean TPathStr::bExtractPath(TString& strBuf)
{
    tCIDLib::TCard4     c4End;
    tCIDLib::TCard4     c4Start;
    const tCIDLib::TCh* pszBuf = pszBuffer();

    if (!TKrnlPathStr::bFindPart(pszBuf, c4Start, c4End, tCIDLib::EPathParts::Path))
        return kCIDLib::False;

    // Copy that substring into the caller's buffer, and cap us off
    if (c4End == kCIDLib::c4MaxCard)
    {
        strBuf.CopyInSubStr(pszBuf, c4Start, c4End);
        CapAt(c4Start);
    }
     else
    {
        strBuf.CopyInSubStr(pszBuf, c4Start, (c4End - c4Start) + 1);
        Cut(c4Start, (c4End - c4Start) + 1);
    }

    //
    //  If this left us with X:, then put the trailing / back on, to make it a
    //  valid volume name.
    //
    if ((c4Length() == 2) && (chAt(1) == kCIDLib::chColon))
        Append(kCIDLib::chBackSlash);
    return kCIDLib::True;
}


tCIDLib::TBoolean TPathStr::bIsFullyQualified() const
{
    return TKrnlPathStr::bIsFullyQualified(pszBuffer());
}


tCIDLib::TBoolean TPathStr::bIsNormalDir() const
{
    TPathStr pathTmp;

    if (!bQueryName(pathTmp))
        return kCIDLib::False;

    return TKrnlFileSys::bIsNormalDir(pathTmp.pszBuffer());
}


tCIDLib::TBoolean TPathStr::bHasExt() const
{
    return TKrnlPathStr::bHasExt(pszBuffer());
}


tCIDLib::TBoolean TPathStr::bHasName() const
{
    return TKrnlPathStr::bHasName(pszBuffer());
}


tCIDLib::TBoolean TPathStr::bHasPath() const
{
    return TKrnlPathStr::bHasPath(pszBuffer());
}


tCIDLib::TBoolean TPathStr::bHasVolume() const
{
    return TKrnlPathStr::bHasVolume(pszBuffer());
}


tCIDLib::TBoolean TPathStr::bHasWildCards() const
{
    return TKrnlFileSys::bHasWildCards(pszBuffer());
}


tCIDLib::TBoolean TPathStr::bQueryExt(TString& strBuf) const
{
    tCIDLib::TCh szTmp[kCIDLib::c4MaxPathLen+1];
    if (!TKrnlPathStr::bQueryExt(pszBuffer(), szTmp, kCIDLib::c4MaxPathLen))
        return kCIDLib::False;

    strBuf = szTmp;
    return kCIDLib::True;
}


tCIDLib::TBoolean TPathStr::bQueryName(TString& strBuf) const
{
    tCIDLib::TCh szTmp[kCIDLib::c4MaxPathLen+1];
    if (!TKrnlPathStr::bQueryName(pszBuffer(), szTmp, kCIDLib::c4MaxPathLen))
        return kCIDLib::False;

    strBuf = szTmp;
    return kCIDLib::True;
}


tCIDLib::TBoolean TPathStr::bQueryNameExt(TString& strBuf) const
{
    tCIDLib::TCh szTmp[kCIDLib::c4MaxPathLen+1];
    if (!TKrnlPathStr::bQueryNameExt(pszBuffer(), szTmp, kCIDLib::c4MaxPathLen))
        return kCIDLib::False;

    strBuf = szTmp;
    return kCIDLib::True;
}


tCIDLib::TBoolean TPathStr::bQueryPath(TString& strBuf) const
{
    tCIDLib::TCh szTmp[kCIDLib::c4MaxPathLen+1];
    if (!TKrnlPathStr::bQueryPath(pszBuffer(), szTmp, kCIDLib::c4MaxPathLen))
        return kCIDLib::False;

    strBuf = szTmp;
    return kCIDLib::True;
}


tCIDLib::TBoolean TPathStr::bQueryVolume(TString& strBuf) const
{
    // It cannot be relative if this is going to work
    if (!bIsFullyQualified())
        return kCIDLib::False;

    tCIDLib::TCh szTmp[kCIDLib::c4MaxPathLen+1];
    if (!TKrnlPathStr::bQueryVolume(pszBuffer(), szTmp, kCIDLib::c4MaxPathLen))
        return kCIDLib::False;

    strBuf = szTmp;
    return kCIDLib::True;
}


tCIDLib::TBoolean TPathStr::bRemoveExt()
{
    tCIDLib::TCard4 c4End, c4Start;

    if (!TKrnlPathStr::bFindPart(pszBuffer(), c4Start, c4End, tCIDLib::EPathParts::Extension))
        return kCIDLib::False;

    //
    //  Cap it at the start of the extension. Actually we have to move one
    //  back to get the separator.
    //
    CapAt(c4Start - 1);
    return kCIDLib::True;
}


tCIDLib::TBoolean TPathStr::bRemoveName()
{
    tCIDLib::TCard4 c4End, c4Start;

    if (!TKrnlPathStr::bFindPart(pszBuffer(), c4Start, c4End, tCIDLib::EPathParts::Name))
        return kCIDLib::False;

    if (c4End == kCIDLib::c4MaxCard)
        CapAt(c4Start);
    else
        Cut(c4Start, (c4End - c4Start) + 1);
    return kCIDLib::True;
}


tCIDLib::TBoolean TPathStr::bRemoveNameExt()
{
    tCIDLib::TCard4 c4End, c4Start;

    if (!TKrnlPathStr::bFindPart(pszBuffer(), c4Start, c4End, tCIDLib::EPathParts::Name))
        return kCIDLib::False;

    // Cap it off at the start of the name/ext section
    CapAt(c4Start);
    return kCIDLib::True;
}


tCIDLib::TBoolean TPathStr::bRemovePath()
{
    tCIDLib::TCard4 c4End, c4Start;

    // Find the name part of the path, and cut the part before it
    if (!TKrnlPathStr::bFindPart(pszBuffer(), c4Start, c4End, tCIDLib::EPathParts::Name))
        return kCIDLib::False;
    Cut(0, c4Start);

    //
    //  If this left us with X:, then put the trailing / back on, to make it a
    //  valid volume name.
    //
    if ((c4Length() == 2) && (chAt(1) == kCIDLib::chColon))
        Append(kCIDLib::chBackSlash);
    return kCIDLib::True;
}


tCIDLib::TBoolean TPathStr::bRemoveLevel()
{
    //
    //  This one is difficult. We cannot pass this guy the buffer directly,
    //  since non-const access to the raw buffer is not allowed. So we have
    //  to make a copy, let him modify it, then assign it.
    //
    tCIDLib::TCh* pszTmp = TRawStr::pszReplicate(pszBuffer());
    TArrayJanitor<tCIDLib::TCh> janTmp(pszTmp);

    if (!TKrnlPathStr::bRemoveLevel(pszTmp))
        return kCIDLib::False;

    *this = pszTmp;
    return kCIDLib::True;
}


tCIDLib::TBoolean TPathStr::bRemoveTrailingSeparator()
{
    //
    //  If the last char is a path separator, and it's not the one for the
    //  volume itself, then remove it. If it's somethink like X:\ we don't
    //  remove it, because that will create an invalid path.
    //
    if (chLast() == kCIDLib::chBackSlash)
    {
        const tCIDLib::TCard4 c4Len = c4Length();
        if ((c4Len == 3) && (chAt(1) == kCIDLib::chColon))
            return kCIDLib::False;

        DeleteLast();
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TVoid TPathStr::Normalize()
{
    // Create a buffer big enough to hold a legal path on this current platform
    tCIDLib::TCh szBuf[kCIDLib::c4MaxPathLen + 1];

    if (!TKrnlFileSys::bNormalizePath(pszBuffer(), szBuf, kCIDLib::c4MaxPathLen))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_Normalize
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , *this
        );
    }

    // Looks ok, so set our value to the modified temp buffer
    *this = szBuf;
}
