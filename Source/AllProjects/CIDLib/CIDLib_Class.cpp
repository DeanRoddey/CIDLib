//
// FILE NAME: CIDLib_Class.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/20/1993
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
//  This file implements the TClass class, which is the RTTI class for
//  the CIDLib system. It represents a class in the system.
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
//   CLASS: TTypeFactoryKicker
//  PREFIX: N/A
//
//  This is the very simple implementation of the factory type kicker class
//  which is used behind the scenes to make sure that classes supporting the
//  advanced RTTI stuff get registered automagically.
// ---------------------------------------------------------------------------
TTypeFactoryKicker::TTypeFactoryKicker( const  tCIDLib::TCh* const   pszClassName
                                        ,       tCIDLib::TObjFactory pfnFactory)
{
    TClass::RegisterClass(pszClassName, pfnFactory);
}

TTypeFactoryKicker::~TTypeFactoryKicker()
{
}



// ---------------------------------------------------------------------------
//   CLASS: TClass
//  PREFIX: cls
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TClass: Public, static methods
// ---------------------------------------------------------------------------
TClass& TClass::Nul_TClass()
{
    static TClass clsNull;
    return clsNull;
}


tCIDLib::TVoid TClass::ThrowBadCast(const   TString&    strFromClass
                                    , const TString&    strToClass)
{
    facCIDLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCIDErrs::errcRTTI_BadCast
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::DynamicType
        , strFromClass
        , strToClass
    );
}


// ---------------------------------------------------------------------------
//  TClass: Constructors and Destructor
// ---------------------------------------------------------------------------
TClass::TClass() :

    m_c4BufChars(64)
    , m_hshName(0)
    , m_pszClassName(0)
{
    //
    //  Allocate a reasonable sized string that will hold a likely class
    //  name without reallocation.
    //
    m_pszClassName = new tCIDLib::TCh[65];
    m_pszClassName[0] = 0;
}

TClass::TClass(const TClass& clsSrc) :

    m_c4BufChars(clsSrc.m_c4BufChars)
    , m_hshName(clsSrc.m_hshName)
    , m_pszClassName(0)
{
    m_pszClassName = TRawStr::pszReplicate(clsSrc.m_pszClassName);
}

TClass::TClass(const TString& strClassName) :

    m_c4BufChars(0)
    , m_hshName(0)
    , m_pszClassName(0)
{
    // Replicate the name and store its hash
    m_pszClassName = strClassName.pszDupBuffer();
    m_hshName = TRawStr::hshHashStr(m_pszClassName, kCIDLib::c4ClassModulus);
    if (!m_hshName)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_NotValidUnicode
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    // Store the buffer size
    m_c4BufChars = TRawStr::c4StrLen(m_pszClassName);
}

TClass::TClass(const tCIDLib::TCh* const pszClassName) :

    m_c4BufChars(0)
    , m_hshName(0)
    , m_pszClassName(0)
{
    if (!pszClassName)
    {
        tCIDLib::TZStr1K szTmp;
        TRawStr::CopyCatStr
        (
            szTmp
            , c4MaxBufChars(szTmp)
            , facCIDLib().pszLoadCIDMsg(kCIDMsgs::midClass_InvalidName)
            , pszClassName
            , L")"
        );
        facCIDLib().MsgPopUp(szTmp, CID_FILE, CID_LINE);
        TProcess::ExitProcess(tCIDLib::EExitCodes::FatalError);
    }

    // Replicate the passed class name
    m_pszClassName = TRawStr::pszReplicate(pszClassName);

    // Calculate the hash value
    m_hshName = TRawStr::hshHashStr(m_pszClassName, kCIDLib::c4ClassModulus);
    if (!m_hshName)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_NotValidUnicode
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    // Store the buffer size
    m_c4BufChars = TRawStr::c4StrLen(m_pszClassName);
}

TClass::~TClass()
{
    delete m_pszClassName;
}


// ---------------------------------------------------------------------------
//  TClass: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TClass::operator>=(const TClass& clsToCompare) const
{
    return !operator<(clsToCompare);
}

tCIDLib::TBoolean TClass::operator<=(const TClass& clsToCompare) const
{
    return !operator>(clsToCompare);
}


TClass::operator const tCIDLib::TCh*() const
{
    return m_pszClassName;
}


tCIDLib::TBoolean TClass::operator<(const TClass& clsToCompare) const
{
    const tCIDLib::ESortComps eRes = TRawStr::eCompareStr
    (
        m_pszClassName, clsToCompare.m_pszClassName
    );
    return (eRes == tCIDLib::ESortComps::FirstLess);
}

tCIDLib::TBoolean TClass::operator>(const TClass& clsToCompare) const
{
    const tCIDLib::ESortComps eRes = TRawStr::eCompareStr
    (
        m_pszClassName, clsToCompare.m_pszClassName
    );
    return (eRes == tCIDLib::ESortComps::FirstGreater);
}


tCIDLib::TBoolean TClass::operator==(const TClass& clsToTest) const
{
    if (&clsToTest == this)
        return kCIDLib::True;

    // Check the hash value first
    if (m_hshName != clsToTest.m_hshName)
        return kCIDLib::False;

    // Could be equal, so compare the names
    if (TRawStr::bCompareStr(m_pszClassName, clsToTest.m_pszClassName))
        return kCIDLib::True;

    return kCIDLib::False;
}

tCIDLib::TBoolean TClass::operator!=(const TClass& clsToTest) const
{
    return !operator==(clsToTest);
}


tCIDLib::TBoolean
TClass::operator==(const tCIDLib::TCh* const pszClassName) const
{
    if (TRawStr::bCompareStr(m_pszClassName, pszClassName))
        return kCIDLib::True;
    return kCIDLib::False;
}

tCIDLib::TBoolean
TClass::operator!=(const tCIDLib::TCh* const pszClassName) const
{
    return !operator==(pszClassName);
}


tCIDLib::TBoolean
TClass::operator==(const TString& strClassName) const
{
    return (strClassName == m_pszClassName);
}

tCIDLib::TBoolean TClass::operator!=(const TString& strClassName) const
{
    return !operator==(strClassName);
}


TClass& TClass::operator=(const TClass& clsSrc)
{
    if (this == &clsSrc)
        return *this;

    //
    //  Note that we don't use clsSrc.m_c4BufChars here, becuase it might
    //  be way bigger than the actual class name, which could cause us to
    //  reallocate unnecessarily.
    //
    tCIDLib::TCard4 c4NewChars = TRawStr::c4StrLen(clsSrc.m_pszClassName);

    // If the current buffer is not big enough, reallocate
    if (m_c4BufChars < c4NewChars)
    {
        delete [] m_pszClassName;
        m_pszClassName = 0;
        m_pszClassName = new tCIDLib::TCh[c4NewChars + 1];

        // Store the new buffer size
        m_c4BufChars = c4NewChars;
    }

    // Copy over the source name
    TRawStr::CopyStr(m_pszClassName, clsSrc.m_pszClassName, c4NewChars);

    // Copy over the hash value
    m_hshName = clsSrc.m_hshName;

    return *this;
}


// ---------------------------------------------------------------------------
//  TClass: Public, inherited methods
// ---------------------------------------------------------------------------
const TClass& TClass::clsIsA() const
{
    static TClass clsThis(L"TClass");
    return clsThis;
}


const TClass& TClass::clsParent() const
{
    static TClass clsParent(L"TObject");
    return clsParent;
}


tCIDLib::TBoolean
TClass::bIsDescendantOf(const TClass& clsTarget) const
{
    if (clsTarget.m_hshName != m_hshName)
        return TParent::bIsDescendantOf(clsTarget);

    if (TRawStr::bCompareStr
    (
        clsTarget.m_pszClassName
        , clsThis().m_pszClassName))
    {
        return kCIDLib::True;
    }

    return TParent::bIsDescendantOf(clsTarget);
}


// ---------------------------------------------------------------------------
//  TClass: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TClass::bCanFactoryCreate() const
{
    return bIsClassRegistered(m_pszClassName);
}

tCIDLib::TBoolean TClass::bIsNameValid() const
{
    if (m_pszClassName)
        return kCIDLib::True;

    return kCIDLib::False;
}


tCIDLib::THashVal
TClass::hshCalcHash(const tCIDLib::TCard4 c4Modulus) const
{
    tCIDLib::THashVal hshRet = TRawStr::hshHashStr(m_pszClassName, c4Modulus);
    if (!hshRet)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_NotValidUnicode
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }
    return hshRet;
}

tCIDLib::THashVal TClass::hshInternal() const
{
    return m_hshName;
}


const tCIDLib::TCh* TClass::pszClassName() const
{
    return m_pszClassName;
}

TObject* TClass::pobjMakeNew() const
{
    return TClass::pobjMakeNewOfClass(m_pszClassName);
}


TString TClass::strClassName() const
{
    return TString(m_pszClassName);
}


// ---------------------------------------------------------------------------
//  TClass: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TClass::FormatTo(TTextOutStream& strmToWriteTo) const
{
    strmToWriteTo << m_pszClassName;
}


tCIDLib::TVoid TClass::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Look for the start object marker
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Parse out our hash value
    strmToReadFrom >> m_hshName;

    // Parse out the length and buffer size into temps
    tCIDLib::TCard4 c4Len;
    tCIDLib::TCard4 c4Buf;
    strmToReadFrom >> c4Buf >> c4Len;

    // Test for a 0 sized buffer, which never should happen
    if (!c4Buf)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_BadNameCount
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Reallocate the buffer if needed
    if (c4Buf != m_c4BufChars)
    {
        delete [] m_pszClassName;
        m_pszClassName = 0;
        m_pszClassName = new tCIDLib::TCh[c4Buf + 1];
        m_pszClassName[0] = 0;

        // Store the new buffer size
        m_c4BufChars = c4Buf;
    }

    //
    //  Parse out our text now and cap it off, since the null was not stored.
    //  This will read it in from the UTF-8 on the wire format to our internal
    //  wide char format.
    //
    strmToReadFrom.ReadArray(m_pszClassName, c4Len);
    m_pszClassName[c4Len] = 0;

    //
    //  Hash it and compare with the hash we read in. If not the same then
    //  are are not reading in the right place.
    //
    tCIDLib::THashVal hshTmp = TRawStr::hshHashStr
    (
        m_pszClassName
        , kCIDLib::c4ClassModulus
    );

    if (!hshTmp)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_NotValidUnicode
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    if (hshTmp != m_hshName)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcClass_BadHash
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TString(m_pszClassName)
        );
    }

    // Look for the start object marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}


tCIDLib::TVoid TClass::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // Format our hash value and buffer size
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << m_hshName
                    << m_c4BufChars;

    // Write out how many characters we have
    tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(m_pszClassName);
    strmToWriteTo << c4Len;

    //
    //  Format our class name into the buffer, only the chars used, no nul.
    //  This will format the data to the canonical UTF-8 format.
    //
    strmToWriteTo.WriteArray(m_pszClassName, c4Len);

    strmToWriteTo << tCIDLib::EStreamMarkers::EndObject;
}


