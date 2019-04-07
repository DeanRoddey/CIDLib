//
// FILE NAME: CIDResEd_FileSpooler.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/26/2010
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
//  Implements the simple source text file spooler we use for reading in
//  source files.
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
#include "CIDResEd.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TResEdSpooler,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TResEdSpooler
//  PREFIX: spool
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Constructors and Destructor
// ---------------------------------------------------------------------------
TResEdSpooler::TResEdSpooler(const TString& strSrcFile) :

    m_c4CurLine(0)
    , m_strmSrc
      (
        strSrcFile
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Excl_Read
      )
    , m_strSrcFile(strSrcFile)
{
}

TResEdSpooler::~TResEdSpooler()
{
}



// ---------------------------------------------------------------------------
//  Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Peaks the next line. If it starts with the indicated prefix, it goes
//  ahead and consumes the line and returns the part after the prefix, returning
//  true. Else it just returns false or throws if the match was required.
//
tCIDLib::TBoolean
TResEdSpooler::bCheckPrefix(const   TString&            strPref
                            ,       TString&            strRemainder
                            , const tCIDLib::TBoolean   bRequired)
{
    bPeakLine(strRemainder, kCIDLib::False);

    if (strRemainder.bStartsWith(strPref))
    {
        EatLine();
        strRemainder.Cut(0, strPref.c4Length());
        return kCIDLib::True;
    }

    // If it was required
    if (bRequired)
    {
        facCIDResEd.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kResEdErrs::errcSrcFile_Expected
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , strPref
            , TCardinal(m_c4CurLine)
            , m_strSrcFile
        );
    }
    return kCIDLib::False;
}


//
//  Peaks the next line. If it starts with the indicated prefix, it goes
//  ahead and eats the next line, and tries to convert the remainder to
//  a numeric value which it gives back. If it doesn't match the prefix,
//  we either just return false or we throw if this is a required value.
//
tCIDLib::TBoolean
TResEdSpooler::bCheckPrefixVal( const   TString&            strPref
                                ,       tCIDLib::TCard4&    c4Val
                                , const tCIDLib::TBoolean   bRequired)
{
    bPeakLine(m_strTmp, kCIDLib::False);

    if (m_strTmp.bStartsWith(strPref))
    {
        EatLine();
        m_strTmp.Cut(0, strPref.c4Length());

        if (!m_strTmp.bToCard4(c4Val))
        {
            facCIDResEd.ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kResEdErrs::errcSrcFile_BadId
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , m_strTmp
                , TCardinal(m_c4CurLine)
                , m_strSrcFile
            );
        }
        return kCIDLib::True;
    }

    // If it was required
    if (bRequired)
    {
        facCIDResEd.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kResEdErrs::errcSrcFile_Expected
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , strPref
            , TCardinal(m_c4CurLine)
            , m_strSrcFile
        );
    }
    return kCIDLib::False;
}


// Outside probably doesn't need it, but allow check for EOS
tCIDLib::TBoolean TResEdSpooler::bEndOfStream() const
{
    return m_strmSrc.bEndOfStream();
}


// Allows a single line read ahead
tCIDLib::TBoolean
TResEdSpooler::bPeakLine(TString& strToFill, const tCIDLib::TBoolean bEndOk)
{
    // Try to read a line using the real reader method
    if (bReadLine(strToFill, bEndOk))
    {
        //
        //  Save it in the pushback string. We could just be pushing back
        //  what it gave us back from this same string.
        //
        m_strPushBack = strToFill;
        return kCIDLib::True;
    }

    //
    //  If it wasn't ok to hit the end of stream it would have thrown,
    //  so we can just return False.
    //
    return kCIDLib::False;
}


tCIDLib::TBoolean
TResEdSpooler::bReadLine(TString& strToFill, const tCIDLib::TBoolean bEndOk)
{
    // If we have a pushed back line, then give them that
    if (!m_strPushBack.bIsEmpty())
    {
        strToFill = m_strPushBack;
        m_strPushBack.Clear();
        return kCIDLib::True;
    }

    while (!m_strmSrc.bEndOfStream())
    {
        // Get a line and strip lead/triling whitespace
        m_strmSrc.c4GetLine(strToFill, 0, kCIDLib::True);
        m_c4CurLine++;

        // If a non-empty, non-comment line, break out
        if (!strToFill.bIsEmpty() && (strToFill[0] != kCIDLib::chSemiColon))
            break;
    }

    // If no input, and end of stream isn't OK, then throw
    const tCIDLib::TBoolean bRes = !strToFill.bIsEmpty();
    if (!bRes && !bEndOk)
    {
        facCIDResEd.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kResEdErrs::errcSrcFile_EndOfStream
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotAllRead
            , m_strSrcFile
        );
    }
    return bRes;
}


//
//  A helper that will get the next line and see if it starts with one of
//  the passed prefixes. If not, it can start with the end string passed.
//  If neither, then we throw. If it matches a prefix, we return the index
//  or max card if it matches the end string.
//
tCIDLib::TCard4
TResEdSpooler::c4CheckPrefix(const  tCIDLib::TStrList&  colPrefs
                            , const TString&            strEnd
                            ,       TString&            strRemainder)
{
    // Read a line, EOS is not legal here
    bReadLine(strRemainder, kCIDLib::False);

    const tCIDLib::TCard4 c4Count = colPrefs.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (strRemainder.bStartsWith(colPrefs[c4Index]))
        {
            // Cut out the prefix, leaving the remainder
            strRemainder.Cut(0, colPrefs[c4Index].c4Length());
            return c4Index;
        }
    }

    // If not the end string, then we failed
    if (strRemainder != strEnd)
    {
        m_strTmp.Clear();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            if (c4Index)
                m_strTmp.Append(L" ,");
            m_strTmp.Append(colPrefs[c4Index]);
        }

        facCIDResEd.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kResEdErrs::errcSrcFile_Expected
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , m_strTmp
            , TCardinal(m_c4CurLine)
            , m_strSrcFile
        );
    }

    // It was the end string
    return kCIDLib::c4MaxCard;
}


// Just returns the current line for error reporting purposes
tCIDLib::TCard4 TResEdSpooler::c4CurLine() const
{
    return m_c4CurLine;
}


//
//  This is a helper that gets the next line and makes sure it matches
//  the passed text to check. If not, it throws an exception. This saves
//  a lot of grunt work.
//
tCIDLib::TVoid TResEdSpooler::CheckNext(const TString& strToCheck)
{
    // Read a line, EOS is not legal here
    bReadLine(m_strTmp, kCIDLib::False);

    if (m_strTmp != strToCheck)
    {
        facCIDResEd.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kResEdErrs::errcSrcFile_Expected
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , strToCheck
            , TCardinal(m_c4CurLine)
            , m_strSrcFile
        );
    }
}


// Eat the next line
tCIDLib::TVoid TResEdSpooler::EatLine()
{
    // If we have a pushed back line, then just clear it
    if (!m_strPushBack.bIsEmpty())
    {
        m_strPushBack.Clear();
        return;
    }

    //
    //  Otherwise, read a li'ne into a temp and toss it. We assume it can
    //  never be legal to hit end of stream here.
    //
    bReadLine(m_strTmp, kCIDLib::False);
}


//
//  Handles the common scenario of parsing out a line that is compsed of
//  a name, a comma, then a numeric id. The name part is left in the
//  incoming string.
//
//  For dialogs, we support some special ids, IdCancel and IdOK, which we
//  convert to known values.
//
tCIDLib::TVoid
TResEdSpooler::ParseNameVal(TString& strToParse, tCIDLib::TCard4& c4Value)
{
    // Split the incoming into two parts
    if (!strToParse.bSplit(m_strTmp, kCIDLib::chComma))
    {
        facCIDResEd.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kResEdErrs::errcSrcFile_BadItem
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(m_c4CurLine)
            , m_strSrcFile
        );
    }

    // Strip lead/trail whitespace from the name and id
    strToParse.StripWhitespace();
    m_strTmp.StripWhitespace();

    if (m_strTmp.bCompareI(L"IdOk"))
    {
        c4Value = 1;
    }
     else if (m_strTmp.bCompareI(L"IdCancel"))
    {
        c4Value = 2;
    }
     else
    {
        // Convert the id to a number
        if (!m_strTmp.bToCard4(c4Value, tCIDLib::ERadices::Dec))
        {
            facCIDResEd.ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kResEdErrs::errcSrcFile_BadId
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , m_strTmp
                , TCardinal(m_c4CurLine)
                , m_strSrcFile
            );
        }
    }
}


//
//  Handles the common scenario of parsing out a three part line, which
//  has a name, numeric id, and text all separated by spaces. The name
//  part is left in the incoming string.
//
tCIDLib::TVoid
TResEdSpooler::ParseNameValText(TString&            strToParse
                                , tCIDLib::TCard4&  c4Value
                                , TString&          strText)
{
    // Split the incoming into two parts
    if (!strToParse.bSplit(m_strTmp, kCIDLib::chSpace)
    ||  !m_strTmp.bSplit(strText, kCIDLib::chSpace))
    {
        facCIDResEd.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kResEdErrs::errcSrcFile_BadItem
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(m_c4CurLine)
            , m_strSrcFile
        );
    }

    // Strip lead/trail whitespace from the id and text
    m_strTmp.StripWhitespace();
    strText.StripWhitespace();

    // Convert the id to a number
    if (!m_strTmp.bToCard4(c4Value, tCIDLib::ERadices::Dec))
    {
        facCIDResEd.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kResEdErrs::errcSrcFile_BadId
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , m_strTmp
            , TCardinal(m_c4CurLine)
            , m_strSrcFile
        );
    }
}


const TString& TResEdSpooler::strSrcFile() const
{
    return m_strSrcFile;
}


