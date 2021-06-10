//
// FILE NAME: XMLFormat.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/08/2021
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2021
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the main (only) module for a little utility program that does
//  two things. It will take a directory of XML files and spit out another
//  directory of the same files pretty printed. And, it will go the other
//  way taking a directory of XML files and printing them back out with
//  all non-significant white space removed.
//
//  They are output in the same encoding as they were originally in.
//
//  You can ask for validation, else it won't do any. If you do ask for
//  validation, they have to have DTDs.
//
//  If will not overwrite unless you provide the /Overwrite option
//
// CAVEATS/GOTCHAS:
//
//  1)  This program is very simple so it has no facility class/object and
//      just starts up on a local function.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "CIDEncode.hpp"
#include    "CIDXML.hpp"


// ----------------------------------------------------------------------------
//  For this simple example, just start the main thread on a global function.
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eXMLFormatThread(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"XMLFormatThread", eXMLFormatThread))


// ----------------------------------------------------------------------------
//  Local data
//
//  conOut
//      The parsed XML output can go to this, or to an output file. Otherwise this
//      is used for program output, usage msg, error msg.
// ----------------------------------------------------------------------------
TOutConsole conOut;


// ----------------------------------------------------------------------------
//  Local helper functions
// ----------------------------------------------------------------------------

// Just shows a usage banner if we get bad parms
static tCIDLib::TVoid ShowUsage()
{
    conOut  << L"Usage: XMLFormat /InSpec=xxx /OutDir=xxx /Fmt=[Pretty|Flat] [options]\n"
               L"       Options:\n"
               L"          /Overwrite\n\n"
               L"       InSpec is the path and wildcard spec to search for, e.g.\n"
               L"       X:\\Files\\*.xml\n\n"
               L"       It will not overwrite any target files unless you give the \n"
               L"       /Overwrite option\n\n"
            << kCIDLib::NewEndLn;
}


//
//  This is called for each file.
//
static tCIDLib::TBoolean bProcess(          TXMLTreeParser&     xtprsToUse
                                    , const TPathStr&           pathFile
                                    , const TString&            strOutDir
                                    , const tCIDXML::EParseOpts eOpts
                                    , const tCIDXML::EPrintFmts eFmt
                                    , const tCIDLib::TBoolean   bOverwrite)
{
    // Build up our output path. We have to get the file.ext off the file and add to the out path
    TString strFilePart;
    if (!pathFile.bQueryNameExt(strFilePart))
    {
        conOut  << L"Couldn't get the file.ext from the input file:\n     "
                << pathFile << kCIDLib::DNewLn;
        return kCIDLib::False;
    }
    TPathStr pathOut(strOutDir);
    pathOut.AddLevel(strFilePart);

    // If overwrite is fals, see if it exists
    if (!bOverwrite && TFileSys::bExists(pathOut))
    {
        conOut  << L"Output file already exists:\n    "
                << pathOut << kCIDLib::DNewLn;
        return kCIDLib::False;
    }

    try
    {
        //
        //  Do the XML parse using the tree parser. Tell the tree to store all
        //  XML content. Often you will just want to store elements and character
        //  data if the purpose is to process the contents for your own purposes.
        //  here we are writing it back out so we want it all.
        //
        if (xtprsToUse.bParseRootEntity(pathFile, eOpts, tCIDXML::EParseFlags::All))
        {
            // Get the original encoding out of the header
            const TXMLTreeElement& xtnodeRoot = xtprsToUse.xtdocThis().xtnodeRoot();
            const TString strEncoding = xtprsToUse.xtdocThis().xtnodeDecl().strEncoding();

            //
            //  Try to create an encoder for that. This has to work since we were able to
            //  to parse thei file with this encoding.
            //
            TTextConverter* ptcvtOut = facCIDEncode().ptcvtMake(strEncoding);

            // Try to create the output stream. It adopts the converter
            TTextFileOutStream strmOut
            (
                pathOut
                , tCIDLib::ECreateActs::CreateAlways
                , tCIDLib::EFilePerms::Default
                , tCIDLib::EFileFlags::SequentialScan
                , tCIDLib::EAccessModes::Excl_Write
                , ptcvtOut
            );

            //  It worked, so format it back out to the output passed output stream.
            xtprsToUse.xtdocThis().PrintTo(strmOut, 0, kCIDLib::False, eFmt);
        }
         else
        {
            // XML errors occurred, display the first one
            const TXMLTreeParser::TErrInfo& errFirst = xtprsToUse.erriFirst();
            conOut.FormatF
            (
                L"\nThe parse failed\n    %(1)\n    (%(2).%(3)) %(4)\n"
                , errFirst.strText()
                , errFirst.c4Line()
                , errFirst.c4Column()
                , errFirst.strSystemId()
            );
            return kCIDLib::False;
        }
    }

    catch(TError& errToCatch)
    {
        conOut.FormatF
        (
            L"A CIDLib runtime error occured during parsing.\n    Error: %(1)\n\n"
            , errToCatch.strErrText()
        );
    }
    return kCIDLib::True;
}


// ----------------------------------------------------------------------------
//  The main thread entry point
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eXMLFormatThread(TThread& thrThis, tCIDLib::TVoid* pData)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    // Parse our parameters, some are defaulted
    tCIDLib::TBoolean   bParmsOK = kCIDLib::True;
    tCIDLib::TBoolean   bOvewrite = kCIDLib::False;
    tCIDXML::EParseOpts eOptsToUse = tCIDXML::EParseOpts::None;
    tCIDXML::EPrintFmts eFmt = tCIDXML::EPrintFmts::Unknown;
    TString             strInSpecParm;
    TString             strOutDirParm;
    TString             strEncoding;
    {
        TCmdLine cmdlLoad;

        // Remove consumed parms so we can check for unknown ones at the end
        cmdlLoad.bRemoveConsumed(kCIDLib::True);

        // The input and output and xlat type are required
        TString strFmtType;
        if (!cmdlLoad.bFindOptionVal(L"InSpec", strInSpecParm)
        ||  !cmdlLoad.bFindOptionVal(L"OutDir", strOutDirParm)
        ||  !cmdlLoad.bFindOptionVal(L"Fmt", strFmtType))
        {
            ShowUsage();
            return tCIDLib::EExitCodes::BadParameters;
        }

        if (strFmtType.bCompareI(L"Pretty"))
            eFmt = tCIDXML::EPrintFmts::Pretty;
        else if (strFmtType.bCompareI(L"Flat"))
            eFmt = tCIDXML::EPrintFmts::Flat;
        else
        {
            conOut  << L"'" << strFmtType << L"' is not a known output type"
                    << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::BadParameters;
        }

        //
        //  These are just option flags. Note we are OR'ing class type enums
        //  since our enums are IDL generated and support that.
        //
        if (cmdlLoad.bFindOption(L"IgnoreBadChars"))
            eOptsToUse |= tCIDXML::EParseOpts::IgnoreBadChars;
        if (cmdlLoad.bFindOption(L"IgnoreDTD"))
            eOptsToUse |= tCIDXML::EParseOpts::IgnoreDTD;
        if (cmdlLoad.bFindOption(L"Validate"))
            eOptsToUse |= tCIDXML::EParseOpts::Validate;
        if (cmdlLoad.bFindOption(L"Overwrite"))
            bOvewrite = kCIDLib::True;

        // If any left, they are unknown ones
        if (!cmdlLoad.bIsEmpty())
        {
            ShowUsage();
            return tCIDLib::EExitCodes::BadParameters;
        }
    }

    tCIDLib::TBoolean bRet = kCIDLib::True;

    // Find all the files that match the input spec
    tCIDLib::TFFindList colFiles;
    TFileSys::c4SearchDir(strInSpecParm, colFiles, tCIDLib::EDirSearchFlags::AllFiles);
    if (colFiles.bIsEmpty())
    {
        conOut << L"No files match the input spec" << kCIDLib::DNewLn;
    }
     else
    {
        // Loop through the files and call the processing method for each one
        for (tCIDLib::TFFindList::TCursor cursFl = colFiles.cursThis(); cursFl; ++cursFl)
        {
            TXMLTreeParser xtprsToUse;
            bRet &= bProcess
            (
                xtprsToUse, cursFl->pathFileName(), strOutDirParm, eOptsToUse, eFmt, bOvewrite
            );
        }
    }

    // Make sure all the output gets spit out
    conOut.Flush();
    return tCIDLib::EExitCodes::Normal;
}
