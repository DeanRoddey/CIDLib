//
// FILE NAME: CIDLib_FindBuf.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/13/1993
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file implements the TFindBuf class, which represents a single
//  found file during a directory search.
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
AdvRTTIDecls(TFindBuf,TObject)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDLib_FindBuf
{
    // -----------------------------------------------------------------------
    //  Local const data
    //
    //  c2FmtVersion
    //      A current format version, so that we can auto upgrade our
    //      persistence format later if needed.
    //
    //      V2 - Added the creation time
    //
    //  pszDateFmt
    //  pszDefFmt
    //  pszTimeFmt
    //      These are the default formats used if there is not one set for a
    //      particular instance.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard2     c2FmtVersion = 2;
    const tCIDLib::TCh* const pszDateFmt = L"%(M,2,0)/%(D,2,0)/%(Y)";
    const tCIDLib::TCh* const pszDefFmt = L"%(m) %(M) %(S,10) %(N)";
    const tCIDLib::TCh* const pszTimeFmt = L"%(h,2,0):%(u,2,0)%(p)";
}


// ---------------------------------------------------------------------------
//   CLASS: TFindBuf
//  PREFIX: fnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFindBuf: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::ESortComps
TFindBuf::eCompByPath(const TFindBuf& fndb1, const TFindBuf& fndb2)
{
    return fndb1.m_pathFileName.eCompare(fndb2.m_pathFileName);
}


tCIDLib::ESortComps
TFindBuf::eCompByPathI(const TFindBuf& fndb1, const TFindBuf& fndb2)
{
    return fndb1.m_pathFileName.eCompareI(fndb2.m_pathFileName);
}


tCIDLib::ESortComps
TFindBuf::eCompByLastMod(const TFindBuf& fndb1, const TFindBuf& fndb2)
{
    if (fndb1.m_tmLastModify < fndb2.m_tmLastModify)
        return tCIDLib::ESortComps::FirstLess;
    else if (fndb1.m_tmLastModify > fndb2.m_tmLastModify)
        return tCIDLib::ESortComps::FirstGreater;
    return tCIDLib::ESortComps::Equal;
}


const TString& TFindBuf::strFull()
{
    static const TString* pstrFull = nullptr;
    if (!pstrFull)
    {
        TBaseLock lockInit;
        if (!pstrFull)
        {
            TRawMem::pExchangePtr<const TString>
            (
                &pstrFull, new TString(CIDLib_FindBuf::pszDefFmt)
            );
        }
    }
    return *pstrFull;
}


const TString& TFindBuf::strGetKey(const TFindBuf& fndbSrc)
{
    return fndbSrc.m_pathFileName;
}


const TString& TFindBuf::strNameAndSize()
{
    static const TString* pstrNameAndSize = nullptr;
    if (!pstrNameAndSize)
    {
        TBaseLock lockInit;
        if (!pstrNameAndSize)
        {
            TRawMem::pExchangePtr<const TString>
            (
                &pstrNameAndSize, new TString(L"%(S,10) %(P)")
            );
        }
    }
    return *pstrNameAndSize;
}


// ---------------------------------------------------------------------------
//  TFindBuf: Constructors and Destructor
// ---------------------------------------------------------------------------
TFindBuf::TFindBuf() :

    m_c8Size(0)
    , m_eFlags(tCIDLib::EFileInfoFlags::None)
    , m_pathFileName(kCIDLib::pszEmptyZStr)
    , m_pcolChildren(nullptr)
    , m_pstrDateFormat(nullptr)
    , m_pstrDefFormat(nullptr)
    , m_pstrTimeFormat(nullptr)
{
}

TFindBuf::TFindBuf(const TString& strDefFormat) :

    m_c8Size(0)
    , m_eFlags(tCIDLib::EFileInfoFlags::None)
    , m_pathFileName(kCIDLib::pszEmptyZStr)
    , m_pcolChildren(nullptr)
    , m_pstrDateFormat(nullptr)
    , m_pstrDefFormat(nullptr)
    , m_pstrTimeFormat(nullptr)
{
    m_pstrDefFormat = new TString(strDefFormat);
}

TFindBuf::TFindBuf(const TFindBuf& fndbSrc) :

    m_c8Size(fndbSrc.m_c8Size)
    , m_eFlags(fndbSrc.m_eFlags)
    , m_pathFileName(fndbSrc.m_pathFileName)
    , m_pcolChildren(nullptr)
    , m_tmCreated(fndbSrc.m_tmCreated)
    , m_tmLastAccess(fndbSrc.m_tmLastAccess)
    , m_tmLastModify(fndbSrc.m_tmLastModify)
    , m_pstrDateFormat(nullptr)
    , m_pstrDefFormat(nullptr)
    , m_pstrTimeFormat(nullptr)
{
    // See if we need to set up any of the format strings
    if (fndbSrc.m_pstrDateFormat)
        m_pstrDateFormat = new TString(*fndbSrc.m_pstrDateFormat);

    if (fndbSrc.m_pstrDefFormat)
        m_pstrDefFormat = new TString(*fndbSrc.m_pstrDefFormat);

    if (fndbSrc.m_pstrTimeFormat)
        m_pstrTimeFormat = new TString(*fndbSrc.m_pstrTimeFormat);

    // If no children in the source, then we are done
    if (fndbSrc.m_pcolChildren && !fndbSrc.m_pcolChildren->bIsEmpty())
    {
        // We have children so fault in the bag
        m_pcolChildren = new TBag<TFindBuf>;

        //
        //  Add a copy of the current child to our collection. Note that
        //  this will cause a copy constructor, which will cause us to
        //  be recursively called to create the entire nested structure
        //  of find buffer objects.
        //
        TNCCursor cursChildren(fndbSrc.m_pcolChildren);
        for (; cursChildren; ++cursChildren)
            m_pcolChildren->objAdd(*cursChildren);
    }
}

TFindBuf::~TFindBuf()
{
    try
    {
        delete m_pcolChildren;
        delete m_pstrDateFormat;
        delete m_pstrDefFormat;
        delete m_pstrTimeFormat;
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }
}


// ---------------------------------------------------------------------------
//  TFindBuf: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TFindBuf::operator<(const TFindBuf& fndbComp) const
{
    return m_pathFileName < fndbComp.m_pathFileName;
}

tCIDLib::TBoolean TFindBuf::operator>(const TFindBuf& fndbComp) const
{
    return m_pathFileName > fndbComp.m_pathFileName;
}

TFindBuf& TFindBuf::operator=(const TFindBuf& fndbSrc)
{
    if (this != &fndbSrc)
    {
        // Copy over the easy stuff
        m_c8Size        = fndbSrc.m_c8Size;
        m_eFlags        = fndbSrc.m_eFlags;
        m_pathFileName  = fndbSrc.m_pathFileName;
        m_tmCreated     = fndbSrc.m_tmCreated;
        m_tmLastAccess  = fndbSrc.m_tmLastAccess;
        m_tmLastModify  = fndbSrc.m_tmLastModify;

        //
        //  Delete the existing format strings and copy over any that we
        //  need to copy.
        //
        delete m_pstrDateFormat;
        m_pstrDateFormat = nullptr;
        if (fndbSrc.m_pstrDateFormat)
            m_pstrDateFormat  = new TString(*fndbSrc.m_pstrDateFormat);

        delete m_pstrDefFormat;
        m_pstrDefFormat = nullptr;
        if (fndbSrc.m_pstrDefFormat)
            m_pstrDefFormat  = new TString(*fndbSrc.m_pstrDefFormat);

        delete m_pstrTimeFormat;
        m_pstrTimeFormat = nullptr;
        if (fndbSrc.m_pstrTimeFormat)
            m_pstrTimeFormat  = new TString(*fndbSrc.m_pstrTimeFormat);

        // If there are children in the source, then copy them also
        if (fndbSrc.m_pcolChildren && !fndbSrc.m_pcolChildren->bIsEmpty())
        {
            if (!m_pcolChildren)
                m_pcolChildren = new TBag<TFindBuf>(*fndbSrc.m_pcolChildren);
            else
                *m_pcolChildren = *fndbSrc.m_pcolChildren;
        }
    }
    return *this;
}


tCIDLib::TBoolean TFindBuf::operator==(const TFindBuf& fndbToTest) const
{
    if (this == &fndbToTest)
        return kCIDLib::True;

    // Handle the fundamental types first
    if (m_eFlags != fndbToTest.m_eFlags)
        return kCIDLib::False;

    if (m_c8Size != fndbToTest.m_c8Size)
        return kCIDLib::False;

    //
    //  We only test the text of the name, not all of the string attributes,
    //  which would happen if we just did an equality test.
    //
    if (!m_pathFileName.bCompare(fndbToTest.m_pathFileName))
        return kCIDLib::False;

    // Test the times
    if (m_tmCreated != fndbToTest.m_tmCreated)
        return kCIDLib::False;

    if (m_tmLastAccess != fndbToTest.m_tmLastAccess)
        return kCIDLib::False;

    if (m_tmLastModify != fndbToTest.m_tmLastModify)
        return kCIDLib::False;

    //
    //  Compare the format strings. This one is more complex since they are
    //  faulted in only when required.
    //
    //  For each one we first see if both exist. If some compare them and
    //  return false if they don't compare. Otherwise, if one of them exists
    //  then it cannot be equal. Otherwise, they are both zero, so they are
    //  equal.
    //
    if (m_pstrDateFormat && fndbToTest.m_pstrDateFormat)
    {
        if (!m_pstrDateFormat->bCompare(*fndbToTest.m_pstrDateFormat))
            return kCIDLib::False;
    }
     else if (m_pstrDateFormat || fndbToTest.m_pstrDateFormat)
    {
        return kCIDLib::False;
    }

    if (m_pstrDefFormat && fndbToTest.m_pstrDefFormat)
    {
        if (!m_pstrDefFormat->bCompare(*fndbToTest.m_pstrDefFormat))
            return kCIDLib::False;
    }
     else if (m_pstrDefFormat || fndbToTest.m_pstrDefFormat)
    {
        return kCIDLib::False;
    }

    if (m_pstrTimeFormat && fndbToTest.m_pstrTimeFormat)
    {
        if (!m_pstrTimeFormat->bCompare(*fndbToTest.m_pstrTimeFormat))
            return kCIDLib::False;
    }
     else if (m_pstrTimeFormat || fndbToTest.m_pstrTimeFormat)
    {
        return kCIDLib::False;
    }

    //
    //  If one of us has children and the other does not, then obviously
    //  they are not equal.
    //
    if (!m_pcolChildren != !fndbToTest.m_pcolChildren)
        return kCIDLib::False;

    // Compare the children if any
    if (m_pcolChildren)
    {
        // If the element counts are not equal, don't bother
        if (m_pcolChildren->c4ElemCount() != fndbToTest.m_pcolChildren->c4ElemCount())
            return kCIDLib::False;

        TCursor cursSrc(fndbToTest.m_pcolChildren);
        TCursor cursThis(m_pcolChildren);

        while (cursSrc && cursThis)
        {
            if (*cursThis != *cursSrc)
                return kCIDLib::False;
            ++cursSrc;
            ++cursThis;
        }

        #if CID_DEBUG_ON
        if (cursThis.bIsValid() || cursSrc.bIsValid())
        {
            // <TBD> Make sure that they had the same number
        }
        #endif
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean
TFindBuf::operator!=(const TFindBuf& fndbToTest) const
{
    return !operator==(fndbToTest);
}



// ---------------------------------------------------------------------------
//  TFindBuf: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TFindBuf::bCanExecute() const
{
    return tCIDLib::bAllBitsOn(m_eFlags, tCIDLib::EFileInfoFlags::CanExecute);
}


tCIDLib::TBoolean TFindBuf::bCanRead() const
{
    return tCIDLib::bAllBitsOn(m_eFlags, tCIDLib::EFileInfoFlags::CanRead);
}


tCIDLib::TBoolean TFindBuf::bCanWrite() const
{
    return tCIDLib::bAllBitsOn(m_eFlags, tCIDLib::EFileInfoFlags::CanWrite);
}


tCIDLib::TBoolean TFindBuf::bIsDirectory() const
{
    return tCIDLib::bAllBitsOn(m_eFlags, tCIDLib::EFileInfoFlags::IsDirectory);
}


tCIDLib::TBoolean TFindBuf::bIsFile() const
{
    return !tCIDLib::bAnyBitsOn(m_eFlags, tCIDLib::EFileInfoFlags::IsDirectory);
}


tCIDLib::TBoolean TFindBuf::bIsNormal() const
{
    return tCIDLib::bAllBitsOn(m_eFlags, tCIDLib::EFileInfoFlags::IsNormal);
}


tCIDLib::TBoolean TFindBuf::bIsNormalDir() const
{
    return bIsNormal() && bIsDirectory();
}


tCIDLib::TBoolean TFindBuf::bIsNormalFile() const
{
    return bIsNormal() && bIsFile();
}


tCIDLib::TCard8 TFindBuf::c8Size() const
{
    return m_c8Size;
}


TBag<TFindBuf>& TFindBuf::colChildren()
{
    if (!m_pcolChildren)
        m_pcolChildren = new TBag<TFindBuf>;
    return *m_pcolChildren;
}


TFindBuf::TCursor TFindBuf::cursChildren() const
{
    if (!m_pcolChildren)
        m_pcolChildren = new TBag<TFindBuf>;
    return TCursor(m_pcolChildren);
}


TFindBuf::TNCCursor TFindBuf::cursChildrenNC()
{
    if (!m_pcolChildren)
        m_pcolChildren = new TBag<TFindBuf>;
    return TNCCursor(m_pcolChildren);
}


tCIDLib::TVoid
TFindBuf::FormatTo(         TTextOutStream&     strmToWriteTo
                    , const TString&            strFormat) const
{
    //
    //  Do a scan of the format storing for a list of all of the available
    //  replacement tokens. This will allow us to hugely optimize the code
    //  below by only bothering to replace tokens that we know are in
    //  the format string. If there aren't any, just foramt out the format
    //  string to the stream as is and we are done.
    //
    TString strTokens;
    if (!strFormat.bFindTokenList(strTokens))
    {
        strmToWriteTo << strFormat;
        return;
    }

    //
    //  There are tokens to replace so let's do it. Create a string to
    //  format into. Preallocate it a reasonable size to avoid expansion.
    //  And a temporary string for some secondary formatting we may have
    //  to do.
    //
    TString strFmt(strFormat, 128);
    TString strTmp(64UL);

    //
    //  Loop through the list of tokens that were found and replace each
    //  one.
    //
    const tCIDLib::TCard4 c4Count = strTokens.c4Length();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const tCIDLib::TCh chCurTok = strTokens[c4Index];

        switch(chCurTok)
        {
            case kCIDLib::chLatin_L :
                m_tmLastAccess.FormatToStr
                (
                    strTmp
                    , m_pstrTimeFormat ? *m_pstrTimeFormat : CIDLib_FindBuf::pszTimeFmt
                );
                strFmt.eReplaceToken(strTmp, kCIDLib::chLatin_L);
                break;

            case kCIDLib::chLatin_l :
                m_tmLastAccess.FormatToStr
                (
                    strTmp
                    , m_pstrDateFormat ? *m_pstrDateFormat : CIDLib_FindBuf::pszDateFmt
                );
                strFmt.eReplaceToken(strTmp, kCIDLib::chLatin_l);
                break;

            case kCIDLib::chLatin_M :
                m_tmLastModify.FormatToStr
                (
                    strTmp
                    , m_pstrTimeFormat ? *m_pstrTimeFormat : CIDLib_FindBuf::pszTimeFmt
                );
                strFmt.eReplaceToken(strTmp, kCIDLib::chLatin_M);
                break;

            case kCIDLib::chLatin_m :
                m_tmLastModify.FormatToStr
                (
                    strTmp
                    , m_pstrDateFormat ? *m_pstrDateFormat : CIDLib_FindBuf::pszDateFmt
                );
                strFmt.eReplaceToken(strTmp, kCIDLib::chLatin_m);
                break;

            case kCIDLib::chLatin_N :
            {
                TPathStr pathTmp;
                if (m_pathFileName.bQueryNameExt(pathTmp))
                    strFmt.eReplaceToken(pathTmp, kCIDLib::chLatin_N);
                break;
            }

            case kCIDLib::chLatin_P :
                strFmt.eReplaceToken(m_pathFileName, kCIDLib::chLatin_P);
                break;

            case kCIDLib::chLatin_S :
                strFmt.eReplaceToken(m_c8Size, kCIDLib::chLatin_S);
                break;

            default :
                // Not one we care about so ignore
                break;
        };
    }

    // Now dump the formatted string to the stream
    strmToWriteTo << strFmt;
}


const TPathStr& TFindBuf::pathFileName() const
{
    return m_pathFileName;
}

TPathStr& TFindBuf::pathFileName()
{
    return m_pathFileName;
}



TPathStr& TFindBuf::pathFileName(const TString& strToSet)
{
    m_pathFileName = strToSet;
    return m_pathFileName;
}


tCIDLib::TVoid TFindBuf::RemoveAllChildren()
{
    // If we have any children, then flush them
    if (m_pcolChildren)
        m_pcolChildren->RemoveAll();
}


const TString& TFindBuf::strDateFormat() const
{
    if (!m_pstrDateFormat)
        m_pstrDateFormat = new TString(CIDLib_FindBuf::pszDateFmt);
    return *m_pstrDateFormat;
}

const TString& TFindBuf::strDateFormat(const TString& strNewDef)
{
    if (!m_pstrDateFormat)
        m_pstrDateFormat = new TString(strNewDef);
    else
        *m_pstrDateFormat = strNewDef;
    return *m_pstrDateFormat;
}


const TString& TFindBuf::strDefFormat() const
{
    if (!m_pstrDefFormat)
        m_pstrDefFormat = new TString(CIDLib_FindBuf::pszDefFmt);
    return *m_pstrDefFormat;
}

const TString& TFindBuf::strDefFormat(const TString& strNewDef)
{
    if (!m_pstrDefFormat)
        m_pstrDefFormat = new TString(strNewDef);
    else
        *m_pstrDefFormat = strNewDef;
    return *m_pstrDefFormat;
}


const TString& TFindBuf::strTimeFormat() const
{
    if (!m_pstrTimeFormat)
        m_pstrTimeFormat = new TString(CIDLib_FindBuf::pszTimeFmt);
    return *m_pstrTimeFormat;
}

const TString& TFindBuf::strTimeFormat(const TString& strNewDef)
{
    if (!m_pstrTimeFormat)
        m_pstrTimeFormat = new TString(strNewDef);
    else
        *m_pstrTimeFormat = strNewDef;
    return *m_pstrTimeFormat;
}


// Provide access to the time stamps
const TTime& TFindBuf::tmCreated() const
{
    return m_tmCreated;
}

const TTime& TFindBuf::tmLastAccess() const
{
    return m_tmLastAccess;
}

const TTime& TFindBuf::tmLastModify() const
{
    return m_tmLastModify;
}


// ---------------------------------------------------------------------------
//  TFindBuf: Hidden constructors and operators
// ---------------------------------------------------------------------------
TFindBuf::TFindBuf( const   TKrnlFileSys::TRawFileFind& FindBuf
                    , const TString&                    strPath) :

    m_c8Size(FindBuf.c8FileSize)
    , m_eFlags(FindBuf.eInfoFlags)
    , m_pathFileName(strPath)
    , m_pcolChildren(nullptr)
    , m_tmCreated(FindBuf.enctCreationTime)
    , m_tmLastAccess(FindBuf.enctLastAccessTime)
    , m_tmLastModify(FindBuf.enctLastWriteTime)
    , m_pstrDateFormat(nullptr)
    , m_pstrDefFormat(nullptr)
    , m_pstrTimeFormat(nullptr)
{
    // Append the file name to the path
    m_pathFileName.AddLevel(FindBuf.szName);
}


// ---------------------------------------------------------------------------
//  TFindBuf: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TFindBuf::FormatTo(TTextOutStream& strmToWriteTo) const
{
    if (m_pstrDefFormat)
        FormatTo(strmToWriteTo, *m_pstrDefFormat);
    else
        FormatTo(strmToWriteTo, CIDLib_FindBuf::pszDefFmt);
}


tCIDLib::TVoid TFindBuf::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Look for the start object marker
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom  >> c2FmtVersion;
    if (!c2FmtVersion || (c2FmtVersion > CIDLib_FindBuf::c2FmtVersion))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_UnknownFmtVersion
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c2FmtVersion)
            , clsThis()
        );
    }

    // Get the simple attributes out
    strmToReadFrom  >> m_c8Size
                    >> m_eFlags
                    >> m_pathFileName
                    >> m_tmLastAccess
                    >> m_tmLastModify;

    // If V2 or greater read in the creation timne, else zero it
    if (c2FmtVersion > 1)
        strmToReadFrom >> m_tmCreated;
    else
        m_tmCreated = 0;

    //
    //  Stream in the formating strings. There is a leading boolean that
    //  indicates whether each one was streamed or not.
    //
    tCIDLib::TBoolean bLoad;

    delete m_pstrDateFormat;
    m_pstrDateFormat = nullptr;
    strmToReadFrom >> bLoad;
    if (bLoad)
    {
        m_pstrDateFormat = new TString(kCIDLib::pszEmptyZStr, 32);
        strmToReadFrom >> *m_pstrDateFormat;
    }

    delete m_pstrDefFormat;
    m_pstrDefFormat = nullptr;
    strmToReadFrom >> bLoad;
    if (bLoad)
    {
        m_pstrDefFormat = new TString(kCIDLib::pszEmptyZStr, 32);
        strmToReadFrom >> *m_pstrDefFormat;
    }

    delete m_pstrTimeFormat;
    m_pstrTimeFormat = nullptr;
    strmToReadFrom >> bLoad;
    if (bLoad)
    {
        m_pstrTimeFormat = new TString(kCIDLib::pszEmptyZStr, 32);
        strmToReadFrom >> *m_pstrTimeFormat;
    }

    // There should be a frame marker before the children
    strmToReadFrom.CheckForFrameMarker(CID_FILE, CID_LINE);

    // See if there were any children
    tCIDLib::TCard4 c4Children;
    strmToReadFrom >> c4Children;
    if (c4Children)
    {
        // Fault in the collection or flush it if it exists
        if (!m_pcolChildren)
            m_pcolChildren = new TBag<TFindBuf>;
        else
            m_pcolChildren->RemoveAll();

        TFindBuf fndbTmp;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Children; c4Index++)
        {
            strmToReadFrom >> fndbTmp;
            m_pcolChildren->objAdd(fndbTmp);
        }
    }

    // And we should end with an end object marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}


tCIDLib::TVoid TFindBuf::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // Stream out the non-optional members
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << CIDLib_FindBuf::c2FmtVersion
                    << m_c8Size
                    << m_eFlags
                    << m_pathFileName
                    << m_tmLastAccess
                    << m_tmLastModify
                    << m_tmCreated;

    //
    //  Stream out any format strings that got set. They are optional so
    //  we stream out a leading boolean to indicate whether each one is
    //  there.
    //
    if (m_pstrDateFormat)
    {
        strmToWriteTo << kCIDLib::True;
        strmToWriteTo << *m_pstrDateFormat;
    }
     else
    {
        strmToWriteTo << kCIDLib::False;
    }

    if (m_pstrDefFormat)
    {
        strmToWriteTo << kCIDLib::True;
        strmToWriteTo << *m_pstrDefFormat;
    }
     else
    {
        strmToWriteTo << kCIDLib::False;
    }

    if (m_pstrTimeFormat)
    {
        strmToWriteTo << kCIDLib::True;
        strmToWriteTo << *m_pstrTimeFormat;
    }
     else
    {
        strmToWriteTo << kCIDLib::False;
    }

    // Do a frame marker before the children
    strmToWriteTo << tCIDLib::EStreamMarkers::Frame;

    //
    //  If we have children, then stream them. Either way we have to write
    //  out the number of children streamed.
    //
    tCIDLib::TCard4 c4Count = 0;
    if (m_pcolChildren)
    {
        c4Count = m_pcolChildren->c4ElemCount();
        if (c4Count)
        {
            strmToWriteTo << c4Count;

            TCursor cursChildren(m_pcolChildren);
            for (; cursChildren; ++cursChildren)
                strmToWriteTo << *cursChildren;
        }
    }
     else
    {
        strmToWriteTo << c4Count;
    }

    // End up with an end object marker
    strmToWriteTo << tCIDLib::EStreamMarkers::EndObject;
}



// ---------------------------------------------------------------------------
//  TFindBuf: Protected, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This method copies the info from a find operation into this find buffer
//  object.
//
tCIDLib::TVoid
TFindBuf::FromFindBuf(  const   TKrnlFileSys::TRawFileFind& FindBuf
                        , const TString&                    strPath)
{
    // Copy over the data
    m_c8Size        = FindBuf.c8FileSize;
    m_eFlags        = FindBuf.eInfoFlags;
    m_pathFileName  = strPath;
    m_pathFileName.AddLevel(TPathStr(FindBuf.szName));
    m_tmCreated    = FindBuf.enctCreationTime;
    m_tmLastAccess  = FindBuf.enctLastAccessTime;
    m_tmLastModify  = FindBuf.enctLastWriteTime;
}

