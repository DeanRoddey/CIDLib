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
//  all white space removed.
//
//  They are output in the same encoding as they were originally in.
//
//  You can ask for validation, else it won't do any. If you do ask for
//  validation, they have to have DTDs that are accessible via the system
//  id. If it does have a DTD but you want to ingore it, use the /IgnoreDTD
//  option.
//
//  If the /Overwrite option is provided, it will overwrite target files that
//  already exist. If you want to be sure you only get new content, then
//  you must clean out the target directory first.
//
//  It will operate recursively if you provide the /Recurse option.
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
//  strOut
//      We want to support redirection, so we have to get our output stream dynamically.
//      So we use a non-adopting smart pointer to hold a text output stream which we'll
//      set first thing.
// ----------------------------------------------------------------------------
TMngPtr<TTextOutStream> mptrOut;


// ----------------------------------------------------------------------------
//  Local helper functions
// ----------------------------------------------------------------------------

// Just shows a usage banner if we get bad parms or no parms
static tCIDLib::TVoid ShowUsage()
{
    *mptrOut << L"\nUsage: XMLFormat /SrcDir=xxx /OutDir=xxx /SpecList=xxx /Fmt=[Pretty|Flat] [options]\n"
                L"       Options:\n"
                L"          /SpecList=*.xxx;*.yyy;\n"
                L"          /Overwrite\n"
                L"          /Nowrap\n"
                L"          /StripLTSpace\n"
                L"         /EscapeNL\n"
                L"          /Recurse\n\n"
                L"       SrcDir is the path to search\n\n"
                L"       SpecList is one or more wild card files to search for in the\n"
                L"       source directory. If spaces are involved, quote the parameter.\n\n"
                L"       It will not overwrite any target files unless you give\n"
                L"       the /Overwrite option. It will not clean the target directory.\n\n"
                L"       If /Recurse is not given, it just does the target directory\n\n"
                L"       StripLTSpace will strip any leading/trailing white space off of\n"
                L"       element text in Pretty mode\n\n"
                L"       EscapeNL will render new lines as character references in Pretty mode\n\n"
                L"       NoWrap will force the start/end elements around text content to be\n"
                L"       on the same line in Pretty mode, otherwise it will indent longer text\n"
                L"       values inside vertically aligned start/end elements.\n"
            << kCIDLib::NewEndLn;
}


//
//  This is a potentially recursive call to process the source files and spit them
//  out to the target directorty. We process all files at the pathCur level. Then,
//  if bRecurse is true, we search for all sub-dirs at that level and recurse into
//  those.
//
static tCIDLib::TBoolean bProcess(  CIOP    TXMLTreeParser&     xtprsToUse
                                    , const TPathStr&           pathCur
                                    , const tCIDLib::TStrList&  colSpecList
                                    , const TString&            strOutDir
                                    , const tCIDXML::EParseOpts eOpts
                                    , const tCIDXML::EPrintFmts eFmt
                                    , const tCIDXML::EPrintOpts ePrintOpts
                                    , const tCIDLib::TBoolean   bOverwrite
                                    , const tCIDLib::TBoolean   bRecurse)
{
    // If the target path doesn't exist, try to create it
    if (!TFileSys::bExists(strOutDir, tCIDLib::EDirSearchFlags::NormalDirs))
    {
        try
        {
            TFileSys::MakePath(strOutDir);
        }

        catch(const TError& errToCatch)
        {
            *mptrOut    << L"Could not create output path: '" << pathCur
                        << L"'. Error=\n" << errToCatch.strErrText()
                        << kCIDLib::DNewLn;
            return kCIDLib::False;
        }
    }

    // Keep an overall return status for our level and below
    tCIDLib::TBoolean bResult = kCIDLib::True;

    // Search for any files at this level and process those. We have to do it for each wild card spec
    tCIDLib::TFFindList colFiles;
    for (auto cursWCs = colSpecList.cursThis(); cursWCs; ++cursWCs)
    {
        TPathStr pathSpec(pathCur);
        pathSpec.AddLevel(*cursWCs);
        if (TFileSys::c4SearchDir(pathSpec, colFiles, tCIDLib::EDirSearchFlags::NormalFiles))
        {
            // We found some files, so let's process them
            for (tCIDLib::TFFindList::TCursor cursFl = colFiles.cursThis(); cursFl; ++cursFl)
            {
                // Build up our output path. We have to get the file.ext off the src and add to the out path
                TString strFilePart;
                if (!cursFl->pathFileName().bQueryNameExt(strFilePart))
                {
                    *mptrOut    << L"Couldn't get the file.ext from the input file:\n     "
                                << cursFl->pathFileName() << kCIDLib::DNewLn;
                    bResult = kCIDLib::False;
                    continue;
                }
                TPathStr pathOut(strOutDir);
                pathOut.AddLevel(strFilePart);

                // If overwrite is false, see if it exists
                if (!bOverwrite && TFileSys::bExists(pathOut))
                {
                    *mptrOut    << L"Output file already exists, not overwriting:\n    "
                                << pathOut << kCIDLib::DNewLn;
                    bResult = kCIDLib::False;
                    continue;
                }

                try
                {
                    //
                    //  Do the XML parse using the tree parser. Tell the tree to store all
                    //  XML content. Often you will just want to store elements and character
                    //  data if the purpose is to process the contents for your own purposes.
                    //  here we are writing it back out so we want it all.
                    //
                    if (xtprsToUse.bParseRootEntity(cursFl->pathFileName(), eOpts, tCIDXML::EParseFlags::All))
                    {
                        // Get the original encoding out of the header
                        const TXMLTreeElement& xtnodeRoot = xtprsToUse.xtdocThis().xtnodeRoot();
                        const TString strEncoding = xtprsToUse.xtdocThis().xtnodeDecl().strEncoding();

                        //
                        //  Try to create the output stream. Give it a converter for the original
                        //  encoding. That encoding has to be supported since we parsed the file
                        //  with that encoding. It adopts the converter.
                        //
                        TTextFileOutStream strmOut
                        (
                            pathOut
                            , tCIDLib::ECreateActs::CreateAlways
                            , tCIDLib::EFilePerms::Default
                            , tCIDLib::EFileFlags::SequentialScan
                            , tCIDLib::EAccessModes::Excl_Write
                            , facCIDEncode().ptcvtMake(strEncoding)
                        );

                        //  It worked, so format it back out to the output passed output stream.
                        xtprsToUse.xtdocThis().PrintTo(strmOut, 0, eFmt, ePrintOpts);
                    }
                    else
                    {
                        // XML errors occurred, display the first one
                        const TXMLTreeParser::TErrInfo& errFirst = xtprsToUse.erriFirst();
                        mptrOut->FormatF
                        (
                            L"\nThe parse failed\n    %(1)\n    (%(2).%(3)) %(4)\n"
                            , errFirst.strText()
                            , errFirst.c4Line()
                            , errFirst.c4Column()
                            , errFirst.strSystemId()
                        );
                        bResult = kCIDLib::False;
                    }
                }

                catch(TError& errToCatch)
                {
                    mptrOut->FormatF
                    (
                        L"A CIDLib runtime error occured during parsing.\n    Error: %(1)\n\n"
                        , errToCatch.strErrText()
                    );
                    bResult = kCIDLib::False;
                }
            }
        }
    }


    // If recursing, we now need to search for sub-dirs and do those
    if (bRecurse)
    {
        TPathStr pathSpec = pathCur;
        pathSpec.AddLevel(kCIDLib::pszAllDirsSpec);
        colFiles.RemoveAll();
        if (TFileSys::c4SearchDir(pathSpec, colFiles, tCIDLib::EDirSearchFlags::NormalDirs))
        {
            TString strFilePart;
            for (tCIDLib::TFFindList::TCursor cursFl = colFiles.cursThis(); cursFl; ++cursFl)
            {
                // Build up the new target path for this level
                TPathStr pathOut(strOutDir);
                if (!cursFl->pathFileName().bQueryNameExt(strFilePart))
                {
                    *mptrOut    << L"Couldn't get the file.ext from the source path:\n     "
                                << cursFl->pathFileName() << kCIDLib::DNewLn;
                    bResult = kCIDLib::False;
                }
                 else
                {
                    pathOut.AddLevel(strFilePart);
                    bResult &= bProcess
                    (
                        xtprsToUse
                        , cursFl->pathFileName()
                        , colSpecList
                        , pathOut
                        , eOpts
                        , eFmt
                        , ePrintOpts
                        , bOverwrite
                        , bRecurse
                    );
                }
            }
        }
    }
    return bResult;
}


// ----------------------------------------------------------------------------
//  The main thread entry point
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eXMLFormatThread(TThread& thrThis, tCIDLib::TVoid* pData)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    // Get our default output stream
    mptrOut.SetPointer(&TSysInfo::strmOut());

    // Parse our parameters, some are defaulted
    tCIDLib::TBoolean   bParmsOK = kCIDLib::True;
    tCIDLib::TBoolean   bOvewrite = kCIDLib::False;
    tCIDLib::TBoolean   bRecurse = kCIDLib::False;
    tCIDLib::TStrList   colSpecList;
    tCIDXML::EParseOpts eOptsToUse = tCIDXML::EParseOpts::None;
    tCIDXML::EPrintOpts ePrintOpts = tCIDXML::EPrintOpts::None;
    tCIDXML::EPrintFmts eFmt = tCIDXML::EPrintFmts::Unknown;
    TString             strInPath;
    TString             strOutDir;
    TString             strEncoding;
    {
        TCmdLine cmdlLoad;

        // Remove consumed parms so we can check for unknown ones at the end
        cmdlLoad.bRemoveConsumed(kCIDLib::True);

        // These are all required
        {
            TString strFmtType;
            TString strSpecList;
            if (!cmdlLoad.bFindOptionVal(L"SrcDir", strInPath)
            ||  !cmdlLoad.bFindOptionVal(L"SpecList", strSpecList)
            ||  !cmdlLoad.bFindOptionVal(L"OutDir", strOutDir)
            ||  !cmdlLoad.bFindOptionVal(L"Fmt", strFmtType))
            {
                ShowUsage();
                return tCIDLib::EExitCodes::BadParameters;
            }

            // Validate the format type before it goes out of scope
            if (strFmtType.bCompareI(L"Pretty"))
                eFmt = tCIDXML::EPrintFmts::Pretty;
            else if (strFmtType.bCompareI(L"Flat"))
                eFmt = tCIDXML::EPrintFmts::Flat;
            else
            {
                *mptrOut    << L"'" << strFmtType << L"' is not a known output type"
                            << kCIDLib::EndLn;
                return tCIDLib::EExitCodes::BadParameters;
            }

            // Break out the spec list
            TStringTokenizer::SplitOnChar(strSpecList, colSpecList, kCIDLib::chSemiColon);
            if (colSpecList.bIsEmpty())
            {
                *mptrOut << L"The wild card spec list was not valid" << kCIDLib::EndLn;
                return tCIDLib::EExitCodes::BadParameters;
            }
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

        if (cmdlLoad.bFindOption(L"NoWrap"))
            ePrintOpts |= tCIDXML::EPrintOpts::NoWrap;
        if (cmdlLoad.bFindOption(L"StripLTSpace"))
            ePrintOpts |= tCIDXML::EPrintOpts::StripLTSpace;
        if (cmdlLoad.bFindOption(L"EscapeNL"))
            ePrintOpts |= tCIDXML::EPrintOpts::EscapeNL;

        if (cmdlLoad.bFindOption(L"Overwrite"))
            bOvewrite = kCIDLib::True;
        if (cmdlLoad.bFindOption(L"Recurse"))
            bRecurse = kCIDLib::True;

        // If any left, they are unknown ones
        if (!cmdlLoad.bIsEmpty())
        {
            ShowUsage();
            return tCIDLib::EExitCodes::BadParameters;
        }
    }

    //
    //  If the source and target are the same, refuse to do it. We need to get
    //  fully qualified, canonical versions of the directories to be sure.
    //


    // Tell the parser to ignore lead/trailing white space in text nodes
    TXMLTreeParser xtprsToUse;

    // And now kick off the process, which may be recursive
    const tCIDLib::TBoolean bResult = bProcess
    (
        xtprsToUse
        , strInPath
        , colSpecList
        , strOutDir
        , eOptsToUse
        , eFmt
        , ePrintOpts
        , bOvewrite
        , bRecurse
    );

    if (!bResult)
        *mptrOut << L"\n[ERROR] One or more files could not be processed\n\n";

    // Make sure all the output gets spit out
    mptrOut->Flush();

    if (bResult)
        return tCIDLib::EExitCodes::Normal;
    return  tCIDLib::EExitCodes::Unknown;
}
