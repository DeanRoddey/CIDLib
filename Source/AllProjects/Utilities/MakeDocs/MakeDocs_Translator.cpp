//
// FILE NAME: MakeDocs_Translator.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/10/1997
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
//  This module implements the couple of implementation methods of the
//  abstract base class TTranslator. Most of the class is pure virtual but
//  some of it can be written in terms of that abstract interface.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "MakeDocs.hpp"


// ----------------------------------------------------------------------------
//  Do our RTTI macros
// ----------------------------------------------------------------------------
RTTIDecls(TTranslator,TObject)


// ----------------------------------------------------------------------------
//  Local types
//
//  TTagItem
//      This is a small structure that is used to map tag text to their
//      enum values and vice versa.
// ----------------------------------------------------------------------------
struct TTagItem
{
    TTranslator::ETags      eTag;
    tCIDLib::TCh*           pszTag;
    tCIDLib::THashVal       hshTag;
};


// ----------------------------------------------------------------------------
//  Local constant values
// ----------------------------------------------------------------------------
const tCIDLib::TCard4 c4HashModulus = 17;


// ----------------------------------------------------------------------------
//  Local data
//
//  atagList
//      This is an array of TTagItem structures that map all off the tags and
//      enums.
// ----------------------------------------------------------------------------
TTagItem atagList[] =
{
        { TTranslator::ETags::Bold            , L"b"     , 0 }
    ,   { TTranslator::ETags::Break           , L"br"    , 0 }
    ,   { TTranslator::ETags::Divider         , L"hr"    , 0 }
    ,   { TTranslator::ETags::EndBold         , L"eb"    , 0 }
    ,   { TTranslator::ETags::EndHeading1     , L"eh1"   , 0 }
    ,   { TTranslator::ETags::EndHeading2     , L"eh2"   , 0 }
    ,   { TTranslator::ETags::EndHeading3     , L"eh3"   , 0 }
    ,   { TTranslator::ETags::EndHeading4     , L"eh4"   , 0 }
    ,   { TTranslator::ETags::EndParagraph    , L"ep"    , 0 }
    ,   { TTranslator::ETags::EndItalic       , L"ei"    , 0 }
    ,   { TTranslator::ETags::EndList         , L"els"   , 0 }
    ,   { TTranslator::ETags::EndSource       , L"epre"  , 0 }
    ,   { TTranslator::ETags::EndTable        , L"etb"   , 0 }
    ,   { TTranslator::ETags::EndTableCol1    , L"etb1"  , 0 }
    ,   { TTranslator::ETags::EndTableCol2    , L"etb2"  , 0 }
    ,   { TTranslator::ETags::Heading1        , L"h1"    , 0 }
    ,   { TTranslator::ETags::Heading2        , L"h2"    , 0 }
    ,   { TTranslator::ETags::Heading3        , L"h3"    , 0 }
    ,   { TTranslator::ETags::Heading4        , L"h4"    , 0 }
    ,   { TTranslator::ETags::Indent          , L"id"    , 0 }
    ,   { TTranslator::ETags::Italic          , L"i"     , 0 }
    ,   { TTranslator::ETags::List            , L"ls"    , 0 }
    ,   { TTranslator::ETags::ListItem        , L"lsi"   , 0 }
    ,   { TTranslator::ETags::Outdent         , L"od"    , 0 }
    ,   { TTranslator::ETags::Paragraph       , L"p"     , 0 }
    ,   { TTranslator::ETags::Source          , L"pre"   , 0 }
    ,   { TTranslator::ETags::Table           , L"tb"    , 0 }
    ,   { TTranslator::ETags::EndTableCol1    , L"tb1"   , 0 }
    ,   { TTranslator::ETags::EndTableCol2    , L"tb2"   , 0 }
};
const tCIDLib::TCard4 c4TagCount = tCIDLib::c4ArrayElems(atagList);




// ----------------------------------------------------------------------------
//  CLASS: TTranslator
// PREFIX: trans
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TTranslator: Constructors and destructors
// ----------------------------------------------------------------------------

//
//  On the first time through, this guy will initialize the hashes for
//  the tag text in the atagList above.
//
TTranslator::TTranslator(const TString& strSpecialChars) :

    m_pstrmTarget(0)
    , m_strSpecialChars(strSpecialChars)
{
    static tCIDLib::TBoolean bFirstTime = kCIDLib::True;
    if (bFirstTime)
    {
        bFirstTime = kCIDLib::False;

        // Do the hashes for all of the tokens.
        for (tCIDLib::TCard4 c4Ind = 0; c4Ind < c4TagCount; c4Ind++)
        {
            atagList[c4Ind].hshTag = TRawStr::hshHashStr
            (
                atagList[c4Ind].pszTag
                , c4HashModulus
            );

            if (!atagList[c4Ind].hshTag)
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
        }
    }
}


// ----------------------------------------------------------------------------
//  TTranslator: Public, non-virtual methods
// ----------------------------------------------------------------------------

//
//  This method is called after a tag is hit that has associated free
//  form text. This method will copy text from the source parser to the
//  output stream until it hits a '@' character. It will unget this
//  character and return. If it hits any formatting tags along the way
//  it will call the derived class to have the format specific text output
//  to the output stream.
//
tCIDLib::TVoid TTranslator::TranslateTagText(TDocParser& prsrSource)
{
    //
    //  Just start looping through, one character at a time. Because of the
    //  nature of this operation, there really is not any more efficient
    //  way to do it. We do try to be more efficient by builing up small
    //  arrays of chars then writing them all at once.
    //
    tCIDLib::TBoolean       bInPreBlock = kCIDLib::False;
    const tCIDLib::TCard4   c4BufSz = 128;
    tCIDLib::TCard4         c4BufInd = 0;
    tCIDLib::TCh            chCur;
    tCIDLib::TCh            chNext;
    tCIDLib::TCh            szBuf[c4BufSz+1];
    TString                 strClass(128UL);
    TString                 strToken(128UL);

    while (1)
    {
        // Get the next character
        chCur = prsrSource.chGetNextChar();

        // If we get a nul, then we've hit the end
        if (chCur == kCIDLib::chNull)
        {
            // Flush out any chars in the temp buffer
            if (c4BufInd)
            {
                szBuf[c4BufInd] = kCIDLib::chNull;
                *m_pstrmTarget << szBuf;
                c4BufInd = 0;
            }
            return;
        }

        // If its @, then its a token, so we are done
        if (chCur == L'@')
        {
            // Flush out any chars in the temp buffer
            if (c4BufInd)
            {
                szBuf[c4BufInd] = kCIDLib::chNull;
                *m_pstrmTarget << szBuf;
                c4BufInd = 0;
            }

            // Unget the character
            prsrSource.UnGetChar(chCur);
            return;
        }

        //
        //  If its a /, then see if the next character is another one.
        //  If so, its a comment, so we flush the rest of the current
        //  line. If we are in a preformatted block, then let it through
        //  though.
        //
        if ((chCur == L'\\') && !bInPreBlock)
        {
            chNext = prsrSource.chGetNextChar();
            if (chNext == L'\\')
            {
                prsrSource.FlushLine();
                continue;
            }

            // It wasn't so unget the next char
            prsrSource.UnGetChar(chNext);
        }

        //
        //  If it is a '<' character then it could be one of our abstract
        //  formatting tags, so we have to look ahead and see if we can get
        //  a match.
        //
        if (chCur == L'<')
        {
            // Flush out any chars in the temp buffer
            if (c4BufInd)
            {
                szBuf[c4BufInd] = kCIDLib::chNull;
                *m_pstrmTarget << szBuf;
                c4BufInd = 0;
            }

            //
            //  Ok, so lets look forward and see if we find a closing
            //  bracket. If we hit a new line or any white space we know
            //  its not a token. If we fill the temp buffer we know its
            //  not because no tokens are that big.
            //
            szBuf[0] = kCIDLib::chNull;

            tCIDLib::TBoolean bGotToken = kCIDLib::False;
            while (!bGotToken)
            {
                // Get the next char
                tCIDLib::TCh chNext = prsrSource.chGetNextChar();

                if (chNext == kCIDLib::chNull)
                    break;

                // If its the closing brace, lets test the text we got
                if (chNext == L'>')
                {
                    //
                    //  See if the text we found maps to one of our tags.
                    //  If so, then output the tag.
                    //
                    ETags eTag = eMapToTag(szBuf);

                    if (eTag != ETags::None)
                    {
                        // Output this tag in the correct target format
                        OutputTag(eTag);

                        // Watch for pre and endpre blocks and toggle flag
                        if (eTag == TTranslator::ETags::Source)
                            bInPreBlock = kCIDLib::True;
                        else
                            bInPreBlock = kCIDLib::False;

                        // Continue back to the top
                        bGotToken = kCIDLib::True;
                        continue;
                    }
                     else
                    {
                        //
                        //  No tag, so unget the next char and break out. Note
                        //  that this char and the original < char are not in
                        //  in the szBuf string that was built up. So we put
                        //  this one back first and then the buffer will go back
                        //  when we break out.
                        //
                        prsrSource.UnGetChar(chNext);
                        break;
                    }
                }
                 else
                {
                    //
                    //  Put it into the temp buffer. If we fill the temp
                    //  buffer, then give up and unget the whole thing.
                    //
                    szBuf[c4BufInd++] = chNext;
                    szBuf[c4BufInd] = kCIDLib::chNull;

                    if (c4BufInd == c4BufSz)
                        break;
                }
            }

            // If we got a token, then skip back to the top
            if (bGotToken)
            {
                c4BufInd = 0;
                continue;
            }

            // We did not get the token, so unget the text and fall through
            prsrSource.UnGetToken(szBuf);
            c4BufInd = 0;
        }

        //
        //  If this character is 'T' or 'M', then it could be a class name. So
        //  lets search forward to the next whitespace and see if this word
        //  matches one of the classes in our class list.
        //
        if ((chCur == L'T') || (chCur == L'M'))
        {
            // Flush out any chars in the temp buffer
            if (c4BufInd)
            {
                szBuf[c4BufInd] = kCIDLib::chNull;
                *m_pstrmTarget << szBuf;
                c4BufInd = 0;
            }

            // Unget the current char so it will be in the token we get
            prsrSource.UnGetChar(chCur);

            //
            //  And get the next token. We use white space or most of the
            //  likely punctuation as delimiters.
            //
            const TString strDelimit
            (
                TString(kCIDLib::szWhitespace)
                + TString(L"!@#$%^&*()-+=;:'\",./<>")
            );
            prsrSource.GetNextToken(strDelimit, strClass);

            // And see if its in our class list and not the current class
            if (facMakeDocs.bIsValidClassName(strClass)
            &&  !prsrSource.bIsCurrentClass(strClass))
            {
                //
                //  It is, so we need to output a class link here instead
                //  of the basic text. The text for the link and the base
                //  part of the class name (all we have to provide) is the
                //  same.
                //
                OutputLink(strClass, strClass);

                // And go back to the top again
                continue;
            }
             else
            {
                //
                //  It was not a class, so put back the token and reget the
                //  current character.
                //
                prsrSource.UnGetToken(strClass);
                chCur = prsrSource.chGetNextChar();
            }
        }

        // If its a special char then let the derived class handle it
        tCIDLib::TCard4 c4Pos;
        if (m_strSpecialChars.bFirstOccurrence(chCur, c4Pos))
        {
            // Flush out any chars in the temp buffer
            if (c4BufInd)
            {
                szBuf[c4BufInd] = kCIDLib::chNull;
                *m_pstrmTarget << szBuf;
                c4BufInd = 0;
            }

            TranslateSpecialChar(chCur);
            continue;
        }
         else
        {
            //
            //  Its nothing special so just pass it on to the output stream.
            //  If the buffer is full, then flush it.
            //
            if (c4BufInd == c4BufSz)
            {
                szBuf[c4BufInd] = kCIDLib::chNull;
                *m_pstrmTarget << szBuf;
                c4BufInd = 0;
            }

            szBuf[c4BufInd++] = chCur;
        }
    }
}


// ----------------------------------------------------------------------------
//  TTranslator: Private, non-virtual methods
// ----------------------------------------------------------------------------
TTranslator::ETags TTranslator::eMapToTag(const TString& strTag)
{
    // Get the hash of the passed tag text
    tCIDLib::THashVal hshTag = strTag.hshCalcHash(c4HashModulus);

    //
    //  Now use that to check each tag for a possible match. If the
    //  hash matches, then do the actual comparision.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TagCount; c4Index++)
    {
        if (atagList[c4Index].hshTag == hshTag)
        {
            if (strTag == atagList[c4Index].pszTag)
                return atagList[c4Index].eTag;
        }
    }
    return ETags::None;
}
