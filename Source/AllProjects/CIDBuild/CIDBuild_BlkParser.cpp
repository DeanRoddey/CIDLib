//
// FILE NAME: CIDBuild_BlkParser.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/31/2000
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TBlkParser class.
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
#include    "CIDBuild.hpp"


// ---------------------------------------------------------------------------
//   CLASS: TBlkField
//  PREFIX: blkf
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBlkField: Constructors and Destructor
// ---------------------------------------------------------------------------
TBlkField::TBlkField(const  tCIDLib::TCh*       pszFieldName
                    , const tCIDLib::TBoolean   bRequired
                    , const tCIDLib::TCard4     c4MinVals
                    , const tCIDLib::TCard4     c4MaxVals) :

    m_bFound(kCIDLib::False)
    , m_bRequired(bRequired)
    , m_c4LineAt(0)
    , m_c4MaxVals(c4MaxVals)
    , m_c4MinVals(c4MinVals)
    , m_c4ValsFound(0)
    , m_pstrValues(0)
    , m_strFieldName(pszFieldName)
{
    // Allocate the storage for the value values and types
    m_peValTypes = new EValTypes[m_c4MaxVals];
    m_pstrValues = new TBldStr[m_c4MaxVals];

    // Init all of the types to the most lax checking, which is none
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4MaxVals; c4Index++)
        m_peValTypes[c4Index] = EValTypes::Text;
}

TBlkField::~TBlkField()
{
    delete [] m_peValTypes;
    delete [] m_pstrValues;
}


// ---------------------------------------------------------------------------
//  TBlkField: Public, non-virtual methods
// ---------------------------------------------------------------------------
TBlkField::EValTypes TBlkField::eValType(const tCIDLib::TCard4 c4Index) const
{
    CheckMaxIndex(c4Index);
    return m_peValTypes[c4Index];
}

TBlkField::EValTypes TBlkField::eValType(const  tCIDLib::TCard4 c4Index
                                        , const EValTypes       eToSet)
{
    CheckMaxIndex(c4Index);
    m_peValTypes[c4Index] = eToSet;
    return m_peValTypes[c4Index];
}


tCIDLib::TVoid TBlkField::Reset()
{
    m_bFound        = kCIDLib::False;
    m_c4ValsFound   = 0;
    m_c4LineAt      = 0;
    m_strFullValue.Clear();
}

const TBldStr& TBlkField::strFullValue(const TBldStr& strNew)
{
    // Store the full value, and break out the defined number of fields
    m_strFullValue = strNew;

    // If the max value count is one, just set the 0th value field, else tokenize
    if (m_c4MaxVals == 1)
    {
        m_pstrValues[0] = strNew;
        m_c4ValsFound = 1;
    }
     else
    {
        // Get a copy of the value that we can tokenize, and put a janitor on it
        tCIDLib::TCh* pszTmp = strNew.pszDupBuffer();
        TArrayJanitor<tCIDLib::TCh> janTmp(pszTmp);

        // And break out the values and store them
        m_c4ValsFound = 0;
        tCIDLib::TCh* pszTok = TRawStr::pszStrTok(pszTmp, L" \t,");
        while (pszTok)
        {
            // Test this token against the type we were told it should be
            TestAgainstType(pszTok, m_c4ValsFound);

            // Its ok to that degree anyway, so store it
            m_pstrValues[m_c4ValsFound++] = pszTok;
            pszTok = TRawStr::pszStrTok(0, L" \t,");

            if (m_c4ValsFound > m_c4MaxVals)
            {
                stdOut  << L"(Line " << m_c4LineAt
                        << L") Too many values for field: " << m_strFieldName
                        << L" expected a maximum of " << m_c4MaxVals
                        << L" values" << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::FileFormat;
            }
        }
    }

    // We know we didn't get too many fields, make sure we got enough
    if (m_c4ValsFound < m_c4MinVals)
    {
        stdOut  << L"(Line " << m_c4LineAt << L") Too few values for field: "
                << m_strFieldName << L" expected a minimum of "
                << m_c4MinVals << L" values" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::FileFormat;
    }
    return m_strFullValue;
}


const TBldStr& TBlkField::strField(const tCIDLib::TCard4 c4Index) const
{
    CheckFoundIndex(c4Index);
    return m_pstrValues[c4Index];
}


tCIDLib::TVoid TBlkField::SetAllValTypes(const EValTypes eType)
{
    // Set the type of all the values of this field to the passed type
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4MaxVals; c4Index++)
        m_peValTypes[c4Index] = eType;
}



// ---------------------------------------------------------------------------
//  TBlkParser: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TBlkField::CheckFoundIndex(const tCIDLib::TCard4 c4Index) const
{
    if (c4Index > m_c4ValsFound)
    {
        stdOut  << c4Index << L" is not a legal found value index for field: "
                << m_strFieldName << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::Internal;
    }
}


tCIDLib::TVoid TBlkField::CheckMaxIndex(const tCIDLib::TCard4 c4Index) const
{
    if (c4Index > m_c4MaxVals)
    {
        stdOut  << c4Index << L" is not a legal value index for field: "
                << m_strFieldName << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::Internal;
    }
}


tCIDLib::TVoid
TBlkField::TestAgainstType( const  tCIDLib::TCh* const pszVal
                            , const tCIDLib::TCard4     c4Index) const
{
    // Get a short name to the current type we are testing
    const EValTypes eCurType = m_peValTypes[c4Index];

    if (eCurType == EValTypes::Text)
    {
        // This one always will be ok, since it allows any text
    }
     else if (eCurType == EValTypes::Cardinal)
    {
        tCIDLib::TCard4 c4Dummy;
        if (!TRawStr::bXlatCard4(pszVal, c4Dummy))
        {
            stdOut  << L"(Line " << m_c4LineAt
                    << L") Value #" << c4Index << L" must be an unsigned value"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }
    }
     else if (eCurType == EValTypes::Integer)
    {
        tCIDLib::TInt4 i4Dummy;
        if (!TRawStr::bXlatInt4(pszVal, i4Dummy))
        {
            stdOut  << L"(Line " << m_c4LineAt
                    << L") Value #" << c4Index << L" must be an signed value"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }
    }
     else if (eCurType == EValTypes::Boolean)
    {
        tCIDLib::TBoolean bDummy;
        if (!TRawStr::bXlatBoolean(pszVal, bDummy))
        {
            stdOut  << L"(Line " << m_c4LineAt
                    << L") Value #" << c4Index << L" must be a boolean value"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }
    }
     else if (eCurType == EValTypes::Alpha)
    {
        const tCIDLib::TCh* pszTmp = pszVal;
        while (*pszTmp)
        {
            if (!TRawStr::bIsAlpha(*pszTmp))
            {
                stdOut  << L"(Line " << m_c4LineAt
                        << L") Value #" << c4Index << L" must be an alpha value"
                        << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::FileFormat;
            }
            pszTmp++;
        }
    }
     else if (eCurType == EValTypes::AlphNum)
    {
        const tCIDLib::TCh* pszTmp = pszVal;
        while (*pszTmp)
        {
            // It can be alpha separated by space
            if (!TRawStr::bIsAlphaNum(*pszTmp))
            {
                stdOut  << L"(Line " << m_c4LineAt
                        << L") Value #" << c4Index
                        << L" must be an alpha-numeric value" << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::FileFormat;
            }
            pszTmp++;
        }
    }
     else if (eCurType == EValTypes::Alphas)
    {
        const tCIDLib::TCh* pszTmp = pszVal;
        while (*pszTmp)
        {
            // It can be alpha separated by space
            if (!TRawStr::bIsAlphaNum(*pszTmp) && !TRawStr::bIsSpace(*pszTmp))
            {
                stdOut  << L"(Line " << m_c4LineAt
                        << L") Value #" << c4Index
                        << L" must be one or more alpha tokens separated by spaces"
                        << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::FileFormat;
            }
            pszTmp++;
        }
    }
     else if (eCurType == EValTypes::CppName)
    {
        const tCIDLib::TCh* pszTmp = pszVal;
        while (*pszTmp)
        {
            // Either alpha or an underscore
            if (!TRawStr::bIsAlphaNum(*pszTmp) && (*pszTmp != L'_'))
            {
                stdOut  << L"(Line " << m_c4LineAt
                        << L") Value #" << c4Index
                        << L" must be a valid C++ id"
                        << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::FileFormat;
            }
            pszTmp++;
        }
    }
     else
    {
        stdOut  << L"Unknown value type for value: " << pszVal << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::Internal;
    }
}



// ---------------------------------------------------------------------------
//   CLASS: TBlkParser
//  PREFIX: blkp
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBlkParser: Constructors and Destructor
// ---------------------------------------------------------------------------
TBlkParser::TBlkParser(         TBlkField* const    pablkfToParse
                        , const tCIDLib::TCard4     c4FldInfoCount
                        ,       TLineSpooler*       plsplInput
                        , const TBldStr&            strEndText) :

    m_c4FldCount(c4FldInfoCount)
    , m_pablkfToParse(pablkfToParse)
    , m_plsplInput(plsplInput)
    , m_strEndText(strEndText)
{
    // The field count cannot be zero
    if (!m_c4FldCount)
    {
        stdOut  << L"Field count cannot be zero to block parser" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::Internal;
    }
}

TBlkParser::~TBlkParser()
{
}


// ---------------------------------------------------------------------------
//  TBlkParser: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TBlkParser::Parse()
{
    // Reset all of our output data
    tCIDLib::TCard4 c4Index;
    for (c4Index = 0; c4Index < m_c4FldCount; c4Index++)
        m_pablkfToParse[c4Index].Reset();

    //
    //  And now do the parse. We just get lines from the input and, for each
    //  one, see if it matches the field prefix text.
    //
    TBldStr strReadBuf;
    while (kCIDLib::True)
    {
        if (!m_plsplInput->bReadLine(strReadBuf))
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") Expected field value or " << m_strEndText
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        // Watch for the end text
        if (strReadBuf == m_strEndText)
            break;

        //
        //  See if it matches one of our list of field names.
        //
        for (c4Index = 0; c4Index < m_c4FldCount; c4Index++)
        {
            const TBlkField& blkfCur = m_pablkfToParse[c4Index];
            const tCIDLib::TCard4 c4NameLen = blkfCur.strFieldName().c4Length();

            if ((strReadBuf.c4Length() > c4NameLen)
            &&  (strReadBuf[c4NameLen] == L'=')
            &&  strReadBuf.bStartsWith(blkfCur.strFieldName()))
            {
                strReadBuf.Cut(c4NameLen + 1);
                break;
            }
        }

        if (c4Index == m_c4FldCount)
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") Expected field value or " << m_strEndText
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }

        // Strip off the leading/'trailing whitespace
        strReadBuf.StripWhitespace();

        //
        //  And store the info. Storing the full value will do any needed
        //  parsing out of individual field values. If that fails, it will
        //  throw to end the parse.
        //
        //  SO SET THE line number first, so that it can be reported if
        //  the value count isn't correct.
        //
        m_pablkfToParse[c4Index].bFound(kCIDLib::True);
        m_pablkfToParse[c4Index].c4LineAt(m_plsplInput->c4CurLine());
        m_pablkfToParse[c4Index].strFullValue(strReadBuf);
    }

    //
    //  See if any required fields were not provided. If so, then report
    //  them, and return failure.
    //
    for (c4Index = 0; c4Index < m_c4FldCount; c4Index++)
    {
        const TBlkField& blkfCur = m_pablkfToParse[c4Index];

        if (blkfCur.bRequired() && !blkfCur.bFound())
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") Missing required field: " << blkfCur.strFieldName()
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }
    }
}


// ---------------------------------------------------------------------------
//  TBlkParser: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TBlkParser::CheckIndex(const tCIDLib::TCard4 c4ToCheck) const
{
    if (c4ToCheck > m_c4FldCount)
    {
        stdOut  << L"Invalid index " << c4ToCheck << L" to block parser"
                << kCIDBuild::EndLn;
    }
}

