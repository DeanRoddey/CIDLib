//
// FILE NAME: MakeDocs.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/30/2017
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
//  This is the main header for a simple CSS preprocessor. It allows for the
//  definition of constants at the top of a file, which generally will be given a
//  .csspre extension. It will parse the file, find the constants, and replace
//  them in the remainder of the file, and spit out the results to a new file.
//
//  We assume that the CSS is line based, i.e. no replacement token will ever be
//  split across two lines. So we can read a line, do replacements, and write that
//  line to the new file. So very simple.
//
//  This is such a simple little program that we have no facility object, and no
//  header files.
//
//
// CAVEATS/GOTCHAS:
//
//  1)  Since our get next line helper eats empty lines and strips leading and trailing
//      space, we just naturally minimize the output file by removing anything from it
//      but the required content.
//
// LOG:
//
//  $_CIDLib_Log_$
//

// -----------------------------------------------------------------------------
//  Include underlying headers.
// -----------------------------------------------------------------------------
#include    "CIDLib.hpp"



// -----------------------------------------------------------------------------
//  Local function prototypes
//
//  eMainTheadFunc
//      This is the thread function for the main thread that provides the
//      execution for this program.
// -----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
        , tCIDLib::TVoid*   pData
);



// -----------------------------------------------------------------------------
//  Include magic main module code
// -----------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"CSSPreMainThread", eMainThreadFunc))


// -----------------------------------------------------------------------------
//  Local data
// -----------------------------------------------------------------------------
TTextOutStream& strmErr = TSysInfo::strmErr();
TTextOutStream& strmOut = TSysInfo::strmOut();
tCIDLib::TCard4 c4CurLine = 0;
TString strSrcFile;
TString strTarFile;
tCIDLib::TKVPList   colConstants;

// For use by the replacement method
TString strTmp1;
TString strTmp2;



// -----------------------------------------------------------------------------
//  Local helper functions
// -----------------------------------------------------------------------------

//
//  Get the next line, strip leading/trailing while space. Skip empty lines. We can
//  optionally tolerate end of stream here or not. If we are being tolerate, then return
//  indicates whether we got something (true) or hit end end of stream.
//
static tCIDLib::TBoolean
GetNextLine(        TTextInStream&      strmSrc
            ,       TString&            strToFill
            , const tCIDLib::TBoolean   bCanEOS = kCIDLib::False)
{
    strToFill.Clear();
    while (kCIDLib::True)
    {
        if (strmSrc.bEndOfStream() && bCanEOS)
            break;

        strmSrc >> strToFill;
        c4CurLine++;
        strToFill.StripWhitespace();
        if (!strToFill.bIsEmpty())
            break;
    }

    return !strToFill.bIsEmpty();
}


//
//  Replaces any constants in this string. They are in the form $xxx, where xxx is the
//  key part of the key=value mappings at the top of the file.
//
//  Values can contain references to other constants. So we loop until we find no new
//  values to replace.
//
static tCIDLib::TVoid ReplaceAndWrite(TTextOutStream& strmTar, TString& strVal)
{
    tCIDLib::TBoolean bChanged;
    do
    {
        //
        //  Do a pass through all of the possible consts. Reset the changed flag
        //  each time so that, if no changes occur, we'll end up with false at the
        //  end of the loop.
        //
        bChanged = kCIDLib::False;
        const tCIDLib::TCard4 c4ConstCnt = colConstants.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ConstCnt; c4Index++)
        {
            const TKeyValuePair& kvalCur = colConstants[c4Index];
            tCIDLib::TCard4 c4At = 0;
            bChanged |= strVal.bReplaceSubStr
            (
                kvalCur.strKey()
                , kvalCur.strValue()
                , c4At
                , kCIDLib::True
                , kCIDLib::False
            );
        }
    }   while (bChanged);


    // OK, we have the final result, so write it out
    strmTar << strVal<< kCIDLib::NewLn;
}


static tCIDLib::TVoid ShowUsage()
{
    strmErr << L"Usage:\n"
            << L"   CSSPre [srcfile] [outputfile]\n"
            << kCIDLib::NewEndLn;
}



// -----------------------------------------------------------------------------
//  Program entry point
// -----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid* const)
{
    // Let our caller go
    thrThis.Sync();

    try
    {
        // We have to get an input file and and output file
        const tCIDLib::TCard4 c4NumArgs = TSysInfo::c4CmdLineParmCount();
        if (c4NumArgs != 2)
        {
            ShowUsage();
            return tCIDLib::EExitCodes::BadParameters;
        }

        strSrcFile = TSysInfo::strCmdLineParmAt(0);
        strTarFile = TSysInfo::strCmdLineParmAt(1);

        // Open the two files
        TTextFileInStream strmSrc
        (
            strSrcFile
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::None
            , tCIDLib::EFileFlags::SequentialScan
            , tCIDLib::EAccessModes::Excl_Read
        );

        TTextFileOutStream strmTar
        (
            strTarFile
            , tCIDLib::ECreateActs::CreateAlways
            , tCIDLib::EFilePerms::AllAccess
            , tCIDLib::EFileFlags::SequentialScan
            , tCIDLib::EAccessModes::Excl_Write
        );

        //
        //  And now let's just process it. We expect the first line of the file to be
        //  $CSSPreStart. We will then process key/value constant definitions until
        //  we hit the $CSSPreEnd line. After that it's just reading lines, doing the
        //  replacements, and writing them out.
        //
        TString strCurLine;
        GetNextLine(strmSrc, strCurLine);

        if (!strCurLine.bCompareI(L"$CSSPreStart"))
        {
            strmErr << L"Expected $CSSPreStart as first line" << kCIDLib::NewEndLn;
            return tCIDLib::EExitCodes::Normal;
        }

        TString strKey;
        TString strValue;
        TKeyValuePair kvalCur;
        while (kCIDLib::True)
        {
            GetNextLine(strmSrc, strKey);
            if (strKey.bCompareI(L"$CSSPreEnd"))
                break;

            if (!strKey.bSplit(strValue, kCIDLib::chEquals))
            {
                strmErr << L"Expected key=value at line " << c4CurLine
                        << kCIDLib::NewEndLn;
            }

            strKey.StripWhitespace();
            strValue.StripWhitespace();

            // Prepend a $ to the key, which is what we look for in the CSS to replace
            strKey.Insert(kCIDLib::chDollarSign, 0);

            kvalCur.Set(strKey, strValue);
            colConstants.objAdd(kvalCur);
        }

        strmTar << L"/* Generated by CSSPre */\n\n";

        // Now just loop and do replacements until we hit the end of line
        while (GetNextLine(strmSrc, strCurLine, kCIDLib::True))
            ReplaceAndWrite(strmTar, strCurLine);
    }

    catch(TError& errToCatch)
    {
        strmErr  << errToCatch << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::FatalError;

    }

    return tCIDLib::EExitCodes::Normal;
}

