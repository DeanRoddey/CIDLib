//
// FILE NAME: MakeDocs_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/10/1997
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
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
//  This module implements the facility class for this program. This program's
//  facility class provides the centralized access to program functionality.
//  Everything goes through it, making it easy and convenient to coordinate
//  the program's work.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// -----------------------------------------------------------------------------
//  Include underlying headers.
// -----------------------------------------------------------------------------
#include    "MakeDocs.hpp"


// -----------------------------------------------------------------------------
//  Local const data
// -----------------------------------------------------------------------------
static const tCIDLib::TCard4    c4ClassKeyHashMod = 109;


// -----------------------------------------------------------------------------
//  Local static data
//
//  acatiMap
//      An array of category information structures used to map category
//      names to category enumeration values.
// -----------------------------------------------------------------------------
static struct TCategoryMapItem
{
    tMakeDocs::EClassCats   eCat;
    const tCIDLib::TCh*     pszName;
}   acatiMap[] =
    {
            { tMakeDocs::EClassCats::Audio            , L"Audio" }
        ,   { tMakeDocs::EClassCats::ClientServer     , L"Client/Server" }
        ,   { tMakeDocs::EClassCats::Collections      , L"Collections" }
        ,   { tMakeDocs::EClassCats::Communications   , L"Communications" }
        ,   { tMakeDocs::EClassCats::Coordinates      , L"Coordinates" }
        ,   { tMakeDocs::EClassCats::Cryptography     , L"Cryptography" }
        ,   { tMakeDocs::EClassCats::Debugging        , L"Debugging" }
        ,   { tMakeDocs::EClassCats::EBiz             , L"e-Business" }
        ,   { tMakeDocs::EClassCats::ErrorMgmt        , L"Error Management" }
        ,   { tMakeDocs::EClassCats::Facilities       , L"Facilities" }
        ,   { tMakeDocs::EClassCats::FileSystem       , L"File System" }
        ,   { tMakeDocs::EClassCats::Fractals         , L"Fractals" }
        ,   { tMakeDocs::EClassCats::Graphics         , L"Graphics" }
        ,   { tMakeDocs::EClassCats::Handles          , L"Handles" }
        ,   { tMakeDocs::EClassCats::International    , L"Internationalization" }
        ,   { tMakeDocs::EClassCats::Janitors         , L"Janitors" }
        ,   { tMakeDocs::EClassCats::Mathematics      , L"Mathematics" }
        ,   { tMakeDocs::EClassCats::MemoryMgmt       , L"Memory Management" }
        ,   { tMakeDocs::EClassCats::Miscellaneous    , L"Miscellaneous" }
        ,   { tMakeDocs::EClassCats::Mixins           , L"Mixins" }
        ,   { tMakeDocs::EClassCats::NamingServices   , L"Naming Services" }
        ,   { tMakeDocs::EClassCats::ProcessControl   , L"Process Control" }
        ,   { tMakeDocs::EClassCats::RayTracing       , L"Ray Tracing" }
        ,   { tMakeDocs::EClassCats::RTTI             , L"RTTI" }
        ,   { tMakeDocs::EClassCats::Security         , L"Security" }
        ,   { tMakeDocs::EClassCats::Socket           , L"Socket" }
        ,   { tMakeDocs::EClassCats::Streams          , L"Streams" }
        ,   { tMakeDocs::EClassCats::Strings          , L"Strings" }
        ,   { tMakeDocs::EClassCats::Synchronization  , L"Synchronization" }
        ,   { tMakeDocs::EClassCats::SystemInfo       , L"System Info" }
        ,   { tMakeDocs::EClassCats::TCPIP            , L"TCP/IP" }
        ,   { tMakeDocs::EClassCats::TextProcessing   , L"Text Processing" }
        ,   { tMakeDocs::EClassCats::Threading        , L"Threading" }
        ,   { tMakeDocs::EClassCats::TimeServices     , L"Time Services" }
        ,   { tMakeDocs::EClassCats::Windowing        , L"Windowing" }
        ,   { tMakeDocs::EClassCats::XML              , L"XML" }
    };
static const tCIDLib::TCard4 c4MapItems = tCIDLib::c4ArrayElems(acatiMap);



// -----------------------------------------------------------------------------
//  Public functions
// -----------------------------------------------------------------------------
TTextOutStream&
operator<<(TTextOutStream& strmOut, const tMakeDocs::EClassCats eCat)
{
    if (tCIDLib::TCard4(eCat) >= c4MapItems)
    {
        facMakeDocs.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kDocErrs::errcBadCategory
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
            , TInteger(tCIDLib::i4EnumOrd(eCat))
        );
    }

    // Output the name at the passed category index
    strmOut << acatiMap[tCIDLib::c4EnumOrd(eCat)].pszName;
    return strmOut;
}



// ----------------------------------------------------------------------------
//  CLASS: TFacMakeDocs
// PREFIX: fac
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TFacMakeDocs: Constructors and destructors
// ----------------------------------------------------------------------------

//
//  The constructor just calls its parent and does some basic init of members
//  to defaults so that, if the user does not provide them on the command
//  line, they are reasonable.
//
TFacMakeDocs::TFacMakeDocs() :

    TFacility
    (
        L"MakeDocs"
        , tCIDLib::EModTypes::Exe
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
    , m_bForceUpdate(kCIDLib::False)
    , m_colClasses(c4ClassKeyHashMod, new TStringKeyOps, &TClassDocInfo::strGetKey)
    , m_ptransOut(0)
{
    #if CID_DEBUG_ON
    if (c4MapItems != tCIDLib::c4EnumOrd(tMakeDocs::EClassCats::Count))
    {
        facMakeDocs.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kDocErrs::errcCatItemCount
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif

    //
    //  Create a translator. Note that its not really useful until later
    //  when we can tell it the output stream its going to use.
    //
    m_ptransOut = new THtmlTranslator();
}

TFacMakeDocs::~TFacMakeDocs()
{
    delete m_ptransOut;
}


// ----------------------------------------------------------------------------
//  TFacMakeDocs: Public, non-virtual methods
// ----------------------------------------------------------------------------

//
//  This method will check that passed string to see if it is a token key
//  that needs to be replaced. If so, the passed strToFill parameter is filled
//  with the replacement text.
//
tCIDLib::TBoolean TFacMakeDocs::bIsAToken(  const   TString&    strToTest
                                            ,       TString&    strToFill) const
{
    return m_envTokens.bFind(strToTest, strToFill);
}


//
//  This method will check the list of classes to see if the passed name is
//  in the list.
//
tCIDLib::TBoolean
TFacMakeDocs::bIsValidClassName(const TString& strToTest) const
{
    // Create a cursor for the class list collection
    TClassList::TCursor cursClasses(&m_colClasses);

    // If not elements don't bother with the overhead below
    if (!cursClasses.bIsValid())
        return kCIDLib::False;

    // Get the hash of the string
    const tCIDLib::THashVal hshTest = strToTest.hshCalcHash(c4ClassKeyHashMod);

    //
    //  Iterate the list of classes and see if we can find the one with
    //  the passed class name.
    //
    for (; cursClasses; ++cursClasses)
    {
        if (hshTest == cursClasses.hshCurHash())
        {
            if (strToTest == cursClasses->strClassName())
                return kCIDLib::True;
        }
    }
    return kCIDLib::False;
}


//
//  This method is called on program startup to parse out everything from
//  the command line and make sure its happy. The information is stored in
//  member data for later use.
//
tCIDLib::TBoolean TFacMakeDocs::bParseCommandLine()
{
    //
    //  We need to have at least target directory and a source directory. If
    //  we don't get both, then we show an error and exit.
    //
    if (TSysInfo::c4CmdLineParmCount() < 2)
    {
        // Show the usage and exit
        m_conOut << strMsg(kDocMsgs::midUsage) << kCIDLib::NewLn << kCIDLib::EndLn;
        return kCIDLib::False;
    }

    // Get the two required parms out
    TSysInfo::TCmdLineCursor cursParms = TSysInfo::cursCmdLineParms();
    m_pathSourceDir = *cursParms++;
    m_pathTargetDir = *cursParms;

    // If either of them end with slashes, then remove them
    m_pathSourceDir.bRemoveTrailingSeparator();
    m_pathTargetDir.bRemoveTrailingSeparator();

    // Check the two paths
    if (!bCheckPath(m_pathSourceDir, kCIDLib::False))
        return kCIDLib::False;

    if (!bCheckPath(m_pathTargetDir, kCIDLib::True))
        return kCIDLib::False;

    //
    //  If there are any more parameters, check them for valid options
    //  and do what they ask.
    //
    for (; cursParms; ++cursParms)
    {
        TString strParm = *cursParms;
        if (strParm.bCompareI(L"/Force"))
            m_bForceUpdate = kCIDLib::True;
    }
    return kCIDLib::True;
}


//
//  This method is called before the main work begins. It scans all of the
//  subdirectories of the main directory and finds all of the ClassDoc files.
//  For each one, it pulls out the base part of the name (which is a class
//  name) and adds it to the m_colClasses collection, in a TClassDocInfo object.)
//  This list is used later to autogenerate links to class names in the text,
//  create index pages, create categorization pages, etc...
//
//  There is never more than one level of subdirectories so its not necessary
//  to do any recursion here. We just use a double loop.
//
tCIDLib::TVoid TFacMakeDocs::BuildClassList()
{
    TDirIter    diterSubs;
    TFindBuf    fndbSub;

    TPathStr    pathToFind(m_pathSourceDir);
    pathToFind.AddLevel(kCIDLib::pszAllDirsSpec);
    if (!diterSubs.bFindFirst(pathToFind, fndbSub, tCIDLib::EDirSearchFlags::NormalDirs))
    {
        m_conOut << strMsg(kDocMsgs::midNoSubsInSource) << kCIDLib::NewLn;
        return;
    }

    //
    //  Now we loop through all of the subdirectories and search each
    //  one.
    //
    do
    {
        TDirIter    diterFiles;
        TFindBuf    fndbFile;

        // Build up a search directory for this level
        TPathStr pathSearch(fndbSub.pathFileName());
        pathSearch.AddLevel(L"*.ClassDoc");

        //
        //  And do the inner loop search for all ClassDoc files in this
        //  directory.
        //
        if (diterFiles.bFindFirst(pathSearch, fndbFile, tCIDLib::EDirSearchFlags::AllFiles))
        {
            do
            {
                //
                //  Get the base part of the name out of this file and add
                //  it to the list of classes.
                //
                TPathStr pathTmp(fndbFile.pathFileName());
                pathTmp.bRemovePath();
                pathTmp.bRemoveExt();
                m_colClasses.objAdd(TClassDocInfo(pathTmp));

            }   while (diterFiles.bFindNext(fndbFile));
        }
    }   while (diterSubs.bFindNext(fndbSub));

    m_conOut << strMsg(kDocMsgs::midClassesFound, TCardinal(m_colClasses.c4ElemCount()))
             << kCIDLib::EndLn;
}


//
//  This method is called before the main work begins. It opens and parses
//  the Replacement.Txt file and builds a list of key=value replacement
//  tokens that are used later.
//
tCIDLib::TVoid TFacMakeDocs::BuildTokenList()
{
    //
    //  The file format is very simple. Read each line. If it has any non-
    //  whitespace then its a key=value pair, so parse out the two parts of
    //  the pair and add them to the list.
    //
    TPathStr    pathReplace(m_pathSourceDir);
    pathReplace.AddLevel(L"Replacement.Txt");

    TTextFileInStream strmSource
    (
        pathReplace
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::None
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Excl_Read
    );
    m_conOut << kCIDLib::NewLn << L"Replacement Parameters..." << kCIDLib::EndLn;

    //
    //  Create a fully allocated 512 character string to read each line
    //  into. This should much more than sufficient for the small key=value
    //  strings. We also need one other string for splitting out the
    //  value into.
    //
    tCIDLib::TCard4 c4Line = 0;
    TString         strRead(512UL);
    TString         strValue(512UL);

    while (!strmSource.bEndOfStream())
    {
        // Read in a line
        strmSource.c4GetLine(strRead);
        c4Line++;

        // Strip off any whitespace and see if its empty
        strRead.StripWhitespace();

        // If it is empty, then go get another string
        if (strRead.bIsEmpty())
            continue;

        //
        //  There is something there, so parse out the two parts. They are
        //  separated by the equal sign.
        //
        tCIDLib::TCard4 c4At;
        if (!strRead.bFirstOccurrence(L'=', c4At))
        {
            m_conOut << strMsg(kDocErrs::errcBadTokenLine, TCardinal(c4Line))
                     << kCIDLib::NewLn;
            continue;
        }

        if (c4At == 0)
        {
            m_conOut << strMsg(kDocErrs::errcNoTokenKey, TCardinal(c4Line))
                     << kCIDLib::NewLn;
            continue;
        }

        // Cut out the two parts into separate strings and add them
        strRead.CopyOutSubStr(strValue, c4At+1);
        strRead.Cut(c4At);

        //
        //  Make sure its not already in the list. If it is, then we have to
        //  give an error. Else we accept it.
        //
        if (!m_envTokens.bAddOrUpdate(strRead, strValue))
        {
            m_conOut << strMsg(kDocMsgs::midKeyExists, strRead, TCardinal(c4Line))
                     << kCIDLib::NewLn;
        }

        m_conOut << L"  " << strRead << L"=" << strValue << kCIDLib::NewLn;
    }

    // Flush out the cache
    m_conOut << kCIDLib::EndLn;
}


//
//  This method will map a string to one of the defined class categories. It
//  returns the enumeration for the category that was mapped to.
//
tMakeDocs::EClassCats
TFacMakeDocs::eMapCategoryName(const TString& strName) const
{
    //
    //  Find this name in the list of categories. If its there, then add this
    //  new category, else return false.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4MapItems; c4Index++)
    {
        if (strName.bCompareI(acatiMap[c4Index].pszName))
            return acatiMap[c4Index].eCat;
    }

    // Not found so return the count value to indicate failure
    return tMakeDocs::EClassCats::Count;
}


//
//  This method is the top level workhorse method. This one does this:
//
//      1)  Scans the main source directory and gets the name of all of
//          subsystem names (i.e. the directory names.)
//
//      2)  For each subdirectory, it generates the main file which has a
//          link to each subsystem main page. This is done by calling the
//          m_GenSubSystemPage().
//
//      3)  It goes back and iterates then again and calls m_DoSubSystem()
//          for each subsystem. This will iterate all of the files in that
//          subsystem and generate output files for them.
//
tCIDLib::TVoid TFacMakeDocs::GenerateDocs()
{
    //
    //  Create a directory iterator and iterate all of the subdirectories
    //  of the main source directory.
    //
    TDirIter    diterSubs;
    TFindBuf    fndbCurrent;

    TPathStr    pathToFind(m_pathSourceDir);
    pathToFind.AddLevel(L"*");
    if (!diterSubs.bFindFirst(pathToFind, fndbCurrent, tCIDLib::EDirSearchFlags::NormalDirs))
    {
        m_conOut << strMsg(kDocMsgs::midNoSubsInSource) << kCIDLib::EndLn;
        return;
    }

    //
    //  Lets create a text file stream for the main output file. This file
    //  contains links to the main subsystem pages. We know that all output
    //  files have to be ASCII format.
    //
    TPathStr pathOut(m_pathTargetDir);
    pathOut.AddLevel(L"MainIndex");
    pathOut.AppendExt(m_ptransOut->strFileExtension());
    TTextFileOutStream strmTarget
    (
        pathOut
        , tCIDLib::ECreateActs::CreateAlways
        , tCIDLib::EFilePerms::AllAccess
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Excl_Write
    );

    // Set the output stream of the translator object
    m_ptransOut->SetOutputStream(strmTarget);

    // Output the standard page header
    m_ptransOut->OutputPageHeader(L"CIDLib Class and Member Documentation");

    //
    //  Output some canned text here. There is no need to parse a file for
    //  this because its always the same. And there is no need to make it
    //  translatable because the help text is english only anyway.
    //
    m_ptransOut->OutputTag(TTranslator::ETags::Heading1);
    strmTarget << L"CIDLib Class and Member Documentation";
    m_ptransOut->OutputTag(TTranslator::ETags::EndHeading1);
    strmTarget << L"This is the main page of the online class and member "
                  L"documentation of the CIDLib system. This file provides "
                  L"links to the main pages of each subsystem (or 'Facilities' "
                  L"as they are called in CIDLib parlance.) These pages in "
                  L"turn provide links to each class available within that "
                  L"facility.";


    // Output the heading for the subsystem link section
    m_ptransOut->OutputTag(TTranslator::ETags::Heading2);
    strmTarget << L"CIDLib Facilities:";
    m_ptransOut->OutputTag(TTranslator::ETags::EndHeading2);

    // Indent all of the links
    m_ptransOut->OutputTag(TTranslator::ETags::Indent);

    //
    //  Now we loop through all of the subdirectories and generate the
    //  main page for each subsystem.
    //
    do
    {
        TPathStr pathSubSys(fndbCurrent.pathFileName());
        pathSubSys.bRemovePath();
        pathSubSys.bRemoveExt();

        //
        //  Look for particular subdirectories which hold special files,
        //  which are to just be copied as is to the output directory.
        //  Otherwise, we want to process the class doc files in this
        //  subdirectory.
        //
        if ((pathSubSys == L"Images")
        ||  (pathSubSys == L"HandHewn"))
        {
            CopyFiles(pathSubSys);
        }
         else if ((pathSubSys == L"CGI-Bin")
              ||  (pathSubSys == L"CVS"))
        {
            // Skip over these, they aren't documentation directories
        }
         else
        {
            //
            //  Set the translator's output stream back to our stream and
            //  output a link for this subsystem.
            //
            m_ptransOut->SetOutputStream(strmTarget);
            m_ptransOut->OutputLink(pathSubSys, pathSubSys);
            m_ptransOut->OutputTag(TTranslator::ETags::Break);

            //
            //  Lets cause the main page for this subsystem to be generated.
            //  If it returns success, then there are some ClassDoc files to
            //  process so go ahead and process them all.
            //
            if (bGenSubSystemPage(pathSubSys))
                DoSubSystem(pathSubSys);
        }
    }   while (diterSubs.bFindNext(fndbCurrent));

    // Set the translator's output stream back to our stream
    m_ptransOut->SetOutputStream(strmTarget);

    // Undo the indent from above
    m_ptransOut->OutputTag(TTranslator::ETags::Outdent);

    //
    //  Output a title for the categories class index page and a description
    //  of it. Then indent in again and output a link to it.
    //
    m_ptransOut->OutputTag(TTranslator::ETags::Heading2);
    strmTarget << L"Categorized Class Index";
    m_ptransOut->OutputTag(TTranslator::ETags::EndHeading2);

    strmTarget << L"Click here for an index of all documented classes, "
                  L"separated into large classifications. This is often "
                  L"an easier way to find the functionality you need.";

    m_ptransOut->OutputTag(TTranslator::ETags::Indent);
    m_ptransOut->OutputLink
    (
        L"Categorized Class Index"
        , L"CategorizedClassIndex"
    );
    m_ptransOut->OutputTag(TTranslator::ETags::Outdent);

    // Output the standard page footer
    m_ptransOut->OutputPageFooter();

    //
    //  Now lets do some post processing work. The first thing is to generate
    //  the categorized class index page.
    //
    GenCatClassIndex();
}


//
//  This method just shows a little program blurb.
//
tCIDLib::TVoid TFacMakeDocs::ShowBlurb()
{
    TString strBlurb
    (
        kDocMsgs::midBlurb
        , facMakeDocs
        , facCIDLib().strVersion()
    );
    m_conOut << strBlurb << kCIDLib::NewLn << kCIDLib::EndLn;
}


//
//  This method will return a reference to the value string for a token. If
//  it does not exist, this will cause an exception. Its just for calling when
//  you know that the token exists or that it must exist.
//
const TString& TFacMakeDocs::strToken(const TString& strToken) const
{
    return m_envTokens.strFind(strToken);
}


// ----------------------------------------------------------------------------
//  TFacMakeDocs: Private, non-virtual methods
// ----------------------------------------------------------------------------

//
//  This method will check to see if the passed path exists. If it does, it
//  makes sure its a directory. If not, it returns False. If it does not
//  exist, it queries if the user wants to create it. If so, it creates the
//  directory. bCreateOk must be True in order for the directory to be
//  created. If bCreateOk is False, then its an error not to exist.
//
tCIDLib::TBoolean
TFacMakeDocs::bCheckPath( const   TString&            strToCheck
                            , const tCIDLib::TBoolean   bCreateOk)
{
    //
    //  Check to see if the passed path exists. If not, then query whether
    //  we should create it or not.
    //
    TFindBuf fndbPath;
    if (TFileSys::bExists(strToCheck, fndbPath))
    {
        if (!fndbPath.bIsDirectory())
        {
            m_conOut << strMsg(kDocMsgs::midMustBeDir, strToCheck)
                     << kCIDLib::NewLn << kCIDLib::EndLn;
            return kCIDLib::False;
        }

        // Its exists and its a directory, so we are golden
        return kCIDLib::True;
    }

    // It doesn't exist, so if we are not to create, then its an error
    if (!bCreateOk)
    {
        m_conOut << strMsg(kDocMsgs::midDoesNotExist, strToCheck)
                 << kCIDLib::NewLn << kCIDLib::EndLn;
        return kCIDLib::False;
    }

    //
    //  See if the user wants to try to create this path and use it
    //  anyway.
    //
    TString strInput(64UL);
    m_conOut << strMsg(kDocMsgs::midDoesNotExistQ, strToCheck) << kCIDLib::FlushIt;

    if (!m_conIn.c4GetLine(strInput))
        return kCIDLib::False;
    m_conOut << kCIDLib::NewLn;

    if (!strInput.bCompareI(strMsg(kDocMsgs::midYes))
    &&  !strInput.bCompareI(strMsg(kDocMsgs::midShortYes)))
    {
        return kCIDLib::False;
    }

    try
    {
        TFileSys::MakeDirectory(strToCheck);
    }

    catch(TError& errToCatch)
    {
        // If doing verbose logging, and this hasn't been logged already...
        if (facMakeDocs.bShouldLog(errToCatch))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        m_conOut << strMsg(kDocMsgs::midDirMakeFailed)
                 << errToCatch.strErrText() << kCIDLib::NewLn << kCIDLib::EndLn;
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


//
//  This is called from the main code generation method. This one generates
//  a main page for a subsystem. It iterates the classes files in the
//  subsystem and creates links to each of the classes.
//
tCIDLib::TBoolean TFacMakeDocs::bGenSubSystemPage(const TString& strSubName)
{
    //
    //  Create a directory iterator and iterate all of the ClassDoc files
    //  in the source directory.
    //
    TDirIter    diterSource;
    TFindBuf    fndbCurrent;

    TPathStr    pathToFind(m_pathSourceDir);
    pathToFind.AddLevel(strSubName);
    pathToFind.AddLevel(L"*.ClassDoc");
    if (!diterSource.bFindFirst(pathToFind, fndbCurrent, tCIDLib::EDirSearchFlags::AllFiles))
    {
        m_conOut << strMsg(kDocMsgs::midNoDocsInSource, strSubName) << kCIDLib::EndLn;

        //
        //  Return false to tell him not to bother with any more work on this
        //  facility, since it has no class doc files anyway.
        //
        return kCIDLib::False;
    }

    //
    //  There are files in here, so lets open the output file and put out
    //  some preliminary text.
    //
    TPathStr pathOut(m_pathTargetDir);
    pathOut.AddLevel(strSubName);
    pathOut.AppendExt(m_ptransOut->strFileExtension());
    TTextFileOutStream strmTarget
    (
        pathOut
        , tCIDLib::ECreateActs::CreateAlways
        , tCIDLib::EFilePerms::AllAccess
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Excl_Write
    );

    // Point the output translator at our output file
    m_ptransOut->SetOutputStream(strmTarget);

    //
    //  There should be a file in the main source directory that has
    //  documentation for the subsystem. We translate it out to the
    //  output file, then generate the links to all of the classes.
    //
    TPathStr pathClassDoc(m_pathSourceDir);
    pathClassDoc.AddLevel(strSubName);
    pathClassDoc.AppendExt(L".ClassDoc");
    TTextFileInStream strmSource
    (
        pathClassDoc
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::None
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Excl_Read
    );

    // Create a parser for the source stream
    TDocParser prsrSource(strmSource, strSubName);

    // Create the title string and then output the standard page header
    TString strTitle(L"Subsystem: ");
    strTitle.Append(strSubName);
    m_ptransOut->OutputPageHeader(strTitle);

    //
    //  The whole file is just a free form text that we just pass to the
    //  translator to translate. There are no tokens, only formatting
    //  tags.
    //
    m_ptransOut->TranslateTagText(prsrSource);

    //
    //  And now generate the heading for the class list that we are about
    //  to generate.
    //
    m_ptransOut->OutputTag(TTranslator::ETags::Heading2);
    strmTarget << L"Classes in this facility";
    m_ptransOut->OutputTag(TTranslator::ETags::EndHeading2);

    // Indent the list of links
    m_ptransOut->OutputTag(TTranslator::ETags::Indent);

    do
    {
        //
        //  Strip the name we found down to just the base name. This will
        //  be the name of the class.
        //
        TPathStr pathClass(fndbCurrent.pathFileName());
        pathClass.bRemovePath();
        pathClass.bRemoveExt();

        //
        //  And generate a link to this guy. The file and link text is the
        //  same in this case.
        //
        m_ptransOut->OutputLink(pathClass, pathClass);

        // And do a break between them
        m_ptransOut->OutputTag(TTranslator::ETags::Break);

    }   while (diterSource.bFindNext(fndbCurrent));

    // Undo the indent we did above
    m_ptransOut->OutputTag(TTranslator::ETags::Outdent);

    // And now output the standard file footer
    m_ptransOut->OutputPageFooter();

    return kCIDLib::True;
}


//
//  This method will see if the first file is out of date with respect to the
//  second file. This is used to know when files need to get regenerated.
//
tCIDLib::TBoolean
TFacMakeDocs::bOutOfDate(const TString& strFileToCheck, const TString& strSourceFile)
{
    // If the user wants to force an update, just return true
    if (m_bForceUpdate)
        return kCIDLib::True;

    //
    //  See if the file to check even exists. If not, then there is no need
    //  to go further, it must be regenerated. If so, then we will get back
    //  the file info on this file.
    //
    TFindBuf fndbToCheck;
    if (!TFileSys::bExists(strFileToCheck, fndbToCheck))
        return kCIDLib::True;

    // Ok, so get the file information on the source file
    TFindBuf fndbSource;
    if (!TFileSys::bExists(strSourceFile, fndbSource))
    {
        // This is an error
        throw TDocError
        (
            kDocErrs::errcSourceNotFound
            , 0
            , strSourceFile
        );
    }

    // See who has the earliest last modification time
    return (fndbToCheck.tmLastModify() < fndbSource.tmLastModify());
}


//
//  This method will return a reference to the TClassDocInfo object for the
//  class that has the passed name. It is a non-const reference because its
//  intended to be used to update the object.
//
TClassDocInfo& TFacMakeDocs::cldiForClass(const TString& strClassName)
{
    //
    //  If it indicates that its empty, then throw an exception that the
    //  name is not in the list.
    //
    if (m_colClasses.bIsEmpty())
    {
        facMakeDocs.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kDocErrs::errcNotInClassList
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
            , strClassName
        );
    }

    // Get the hash of the string
    const tCIDLib::THashVal hshTest = strClassName.hshCalcHash(c4ClassKeyHashMod);

    //
    //  Iterate the list of classes and see if we can find the one with
    //  the passed class name.
    //
    TClassList::TNCCursor cursClasses(&m_colClasses);
    do
    {
        if (hshTest == cursClasses.hshCurHash())
        {
            if (strClassName == cursClasses.objRCur().strClassName())
                return cursClasses.objWCur();
        }
    }   while (cursClasses.bNext());

    facMakeDocs.ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kDocErrs::errcNotInClassList
        , tCIDLib::ESeverities::ProcFatal
        , tCIDLib::EErrClasses::Internal
        , strClassName
    );

    // Make the compiler happy
    static TClassDocInfo cldiDummy(L"DummyReturnValue");
    return cldiDummy;
}


//
//  This method is called from the main processing method above. Its called
//  to copy the contents of a subdirectory to the output directory.
//
tCIDLib::TVoid TFacMakeDocs::CopyFiles(const TString& strSubDir)
{
    // Build the path we want to iterate
    TPathStr    pathSource(m_pathSourceDir);
    pathSource.AddLevel(strSubDir);
    pathSource.AddLevel(kCIDLib::pszAllFilesSpec);

    //
    //  Create a directory iterator and iterate all of the files in the
    //  source directory.
    //
    TDirIter    diterSource;
    TFindBuf    fndbCurrent;

    if (!diterSource.bFindFirst(pathSource, fndbCurrent, tCIDLib::EDirSearchFlags::AllFiles))
    {
        m_conOut << strMsg(kDocMsgs::midNoFilesIn, strSubDir) << kCIDLib::EndLn;
        return;
    }

    m_conOut << strMsg(kDocMsgs::midCopyingFiles, strSubDir) << kCIDLib::EndLn;
    TPathStr pathTmp;
    do
    {
        // Build the target path
        pathTmp = fndbCurrent.pathFileName();
        pathTmp.bRemovePath();
        TPathStr pathTarget(m_pathTargetDir);
        pathTarget.AddLevel(pathTmp);

        //
        //  And copy the file if its out of date with its target
        //  version.
        //
        if (bOutOfDate(pathTarget, fndbCurrent.pathFileName()))
            TFileSys::CopyFile(fndbCurrent.pathFileName(), pathTarget);

    }   while (diterSource.bFindNext(fndbCurrent));
}


//
//  This is the second level method. When a subdirectory is found, which we
//  know that it is the name of a subsystem and contains document files for
//  that subsystem. So this method is called with the subsystem name. We
//  iterate the class files in it and generate docs for each class.
//
tCIDLib::TVoid TFacMakeDocs::DoSubSystem(const TString& strSubName)
{
    //
    //  Create a directory iterator and iterate all of the ClassDoc files
    //  in the source directory.
    //
    TDirIter    diterSource;
    TFindBuf    fndbCurrent;

    TPathStr    pathToFind(m_pathSourceDir);
    pathToFind.AddLevel(strSubName);
    pathToFind.AddLevel(L"*.ClassDoc");
    if (!diterSource.bFindFirst(pathToFind, fndbCurrent, tCIDLib::EDirSearchFlags::AllFiles))
    {
        m_conOut << strMsg(kDocMsgs::midNoDocsInSource, pathToFind) << kCIDLib::EndLn;
        return;
    }

    m_conOut << strMsg(kDocMsgs::midXlatingFiles, strSubName) << kCIDLib::EndLn;
    do
    {
        //
        //  Since there are potentially a lot of files, put each loop into
        //  a faux block to force cleanup of everything on each round.
        //
        {
            //
            //  Create the path to the source file and the path to the target
            //  file and try to create them both. The source path is in the
            //  find buffer. The target extension is generated via the
            //  translator so it can do what is required for files of its
            //  target type.
            //
            TPathStr pathSrcFile(fndbCurrent.pathFileName());

            //
            //  Get the output file in a temp. Strip off its extension and
            //  path, leaving just the base name. We need this base name in
            //  other places below.
            //
            TPathStr pathTmp(fndbCurrent.pathFileName());
            pathTmp.bRemoveExt();
            pathTmp.bRemovePath();

            //
            //  Now start the target file with the target dir. Then tack on
            //  the temp name that we just created, and the extension that
            //  the translator wants.
            //
            TPathStr    pathTargetFile(m_pathTargetDir);
            pathTargetFile.AddLevel(pathTmp);
            pathTargetFile.AppendExt(m_ptransOut->strFileExtension());

            //
            //  See if we need to update this file. We only need to if its
            //  out of date.
            //
            if (bOutOfDate(pathTargetFile, pathSrcFile))
            {
                // Output the files that we are working on for this go around.
                m_conOut << L"   " << pathSrcFile << kCIDLib::EndLn;

                //
                //  Now lets open the source file and create the target file,
                //  overwriting any current contents. We open the source
                //  stream first in Unicode mode, but we then autosense the
                //  format.
                //
                TTextFileInStream strmSource
                (
                    pathSrcFile
                    , tCIDLib::ECreateActs::OpenIfExists
                    , tCIDLib::EFilePerms::None
                    , tCIDLib::EFileFlags::SequentialScan
                    , tCIDLib::EAccessModes::Excl_Read
                );

                // The target stream is always ASCII only
                TTextFileOutStream strmTarget
                (
                    pathTargetFile
                    , tCIDLib::ECreateActs::CreateAlways
                    , tCIDLib::EFilePerms::AllAccess
                    , tCIDLib::EFileFlags::SequentialScan
                    , tCIDLib::EAccessModes::Excl_Write
                );

                // Tell the translator about the output stream
                m_ptransOut->SetOutputStream(strmTarget);

                // Build up the title string
                TString strPageTitle(L"Class and Member Info for Class: ");
                strPageTitle.Append(pathTmp);

                //
                //  Ask the translator to dump out the standard page header for
                //  this file file
                //
                m_ptransOut->OutputPageHeader(strPageTitle);

                //
                //  Create a parser for this source file. This is what
                //  provides us the easy parsing of the source text. Tell
                //  him the name of the class also.
                //
                TDocParser prsrSource(strmSource, pathTmp);

                //
                //  Ok, call the method that does the parsing of the source
                //  and outputs the translated text.
                //
                try
                {
                    TranslateSource(prsrSource, strmTarget);
                }

                catch(TError& errToCatch)
                {
                    errToCatch.AddStackLevel(CID_FILE, CID_LINE);

                    // Close the output file stream
                    strmTarget.Close();

                    // And delete the output file
                    TFileSys::DeleteFile(pathTargetFile);

                    // And let it propogate out
                    throw;
                }

                //
                //  Ok, we are just about done with this page. Ask the
                //  translator to output the standard page footer info.
                //
                m_ptransOut->OutputPageFooter();
            }
        }
    }   while (diterSource.bFindNext(fndbCurrent));
}


//
//  This is called from the main code generation method. This one generates
//  a categorized class index page. The program maintains a collection of
//  TClassDocInfo objects, one per class, that has information about them
//  including any categories they belong to. For each category we find all
//  of the classes that belong to it and output links to them.
//
//  Because the classes are stored in a hashed collection for fast access
//  by class name, we have to create another collection in order to get a
//  sorted list (since we want all the links under a particular category
//  to be in alphabetical order.) So we just iterate the hashed collection
//  and get each class name out and add it to a sorted bag of strings.
//
tCIDLib::TVoid TFacMakeDocs::GenCatClassIndex()
{
    //
    //  Create a sorted collection of class names, which we will use to do
    //  all outputting of links (so that they will be in order.)
    //
    TSortedBag<TString> colClassNames(TString::eComp);

    TClassList::TCursor cursClasses(&m_colClasses);
    for (; cursClasses; ++cursClasses)
        colClassNames.objAdd(cursClasses->strClassName());

    // Build up the output file name
    TPathStr pathTargetFile(m_pathTargetDir);
    pathTargetFile.AddLevel(L"CategorizedClassIndex");
    pathTargetFile.AppendExt(m_ptransOut->strFileExtension());

    //
    //  Ok, so now lets create a stream for the new output file and put
    //  out the standard header and some descriptive text.
    //
    TTextFileOutStream strmTarget
    (
        pathTargetFile
        , tCIDLib::ECreateActs::CreateAlways
        , tCIDLib::EFilePerms::AllAccess
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Excl_Write
    );
    m_ptransOut->SetOutputStream(strmTarget);
    m_ptransOut->OutputPageHeader(L"Categorized Class Index");

    //
    //  Output some canned text. It does not have to be translatable because
    //  the help is english only.
    //
    m_ptransOut->OutputTag(TTranslator::ETags::Heading1);
    strmTarget  << L"Categorized Class Index";
    m_ptransOut->OutputTag(TTranslator::ETags::EndHeading1);

    strmTarget << L"Here is a categorized list of classes. Each class can "
                  L"belong to more than one category, though usually they "
                  L"only belong to one.";

    // Create a cursor for the sorted bag that we will use for class iteration
    TSortedBag<TString>::TCursor cursNames(&colClassNames);

    //
    //  Now for every category we have, lets output any classes that fall
    //  into that category.
    //
    for (tMakeDocs::EClassCats eCat = tMakeDocs::EClassCats::Min;
                                    eCat <= tMakeDocs::EClassCats::Max; eCat++)
    {
        // Output the category text for this category
        m_ptransOut->OutputTag(TTranslator::ETags::Heading2);
        strmTarget << eCat;
        m_ptransOut->OutputTag(TTranslator::ETags::EndHeading2);

        // Indent all of the classes under this category
        m_ptransOut->OutputTag(TTranslator::ETags::Indent);

        //
        //  Now we need to look through the sorted bag of classes. For each
        //  class, we look up the class info for it. We check it to see if
        //  the class belongs to the current category.
        //
        for (; cursNames; ++cursNames)
        {
            // Get the class doc info for this class
            TClassDocInfo& cldiCur = cldiForClass(*cursNames);

            // If its in the category, then output a link to this class
            if (cldiCur.bIsInCategory(eCat))
            {
                m_ptransOut->OutputLink(cursNames.objRCur(), *cursNames);
                m_ptransOut->OutputTag(TTranslator::ETags::Break);
            }
        }

        // And outdent again
        m_ptransOut->OutputTag(TTranslator::ETags::Outdent);
    }

    // And now output the standard footer
    m_ptransOut->OutputPageFooter();
}


//
//  This is called when a @Overview tag is seen. It starts a new output file
//  and formats the overview text to the new file. The new file is made up of
//  the class name and a title string, e.g. MyClass_ClassOverviewInfo, so that
//  its easy  to find the file from the class name.
//
//  The overview consists of all tokens and their text up to the @EndOverview
//  token or the end of the file. However, the overview is assumed to only
//  include basic formatted text info.
//
tCIDLib::TVoid
TFacMakeDocs::TranslateOverview(        TDocParser& prsrSource
                                , const TString&    strFileName)
{
    static TString  strToken1(kCIDLib::pszEmptyZStr, 1024);

    // Build up the full path to the output file
    TPathStr pathTarget(m_pathTargetDir);
    pathTarget.AddLevel(strFileName);
    pathTarget.AppendExt(m_ptransOut->strFileExtension());

    // Create the output stream for this method.
    TTextFileOutStream strmTarget
    (
        pathTarget
        , tCIDLib::ECreateActs::CreateAlways
        , tCIDLib::EFilePerms::AllAccess
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Excl_Write
    );

    // Install our stream in the translator object
    m_ptransOut->SetOutputStream(strmTarget);

    // Output the standard file header
    m_ptransOut->OutputTag(TTranslator::ETags::Paragraph);
    TString strTitle(L"Class Overview: ");
    strTitle.Append(prsrSource.strClassName());
    m_ptransOut->OutputPageHeader(strTitle);

    // Output a little information
    m_ptransOut->OutputTag(TTranslator::ETags::Heading1);
    strmTarget  << L"Overview of: " << prsrSource.strClassName();
    m_ptransOut->OutputTag(TTranslator::ETags::EndHeading1);

    //
    //  Now handle all of the free form text. It will stop when it hits a
    //  tag. All of the overview information is just free form text
    //  stuff, terminated by an @EndOverview tag.
    //
    m_ptransOut->TranslateTagText(prsrSource);

    // Get the next token
    TDocParser::ETokens eToken = prsrSource.eNextToken();

    if (eToken != TDocParser::ETokens::EndOverview)
    {
        throw TDocError
        (
            kDocErrs::errcExpected
            , prsrSource.c4CurLine()
            , prsrSource.strTokenText(TDocParser::ETokens::EndOverview)
        );
    }

    // And now output the standard file footer
    m_ptransOut->OutputPageFooter();
}


//
//  This is called when a @Method tag is seen. It starts a new output file
//  and formats the method to the new file. The new file is made up of the
//  class name and the method name, e.g. MyClass_MyMethod so that its easy
//  to find the file from the class and method name.
//
//  The method consists of all tokens and their text up to the @EndMethod
//  token or the end of the file.
//
tCIDLib::TVoid
TFacMakeDocs::TranslateMethod(          TDocParser& prsrSource
                                , const TString&    strMethodName
                                , const TString&    strFileName)
{
    static TString  strToken1(kCIDLib::pszEmptyZStr, 1024);

    // Build up the full path to the output file
    TPathStr pathTarget(m_pathTargetDir);
    pathTarget.AddLevel(strFileName);
    pathTarget.AppendExt(m_ptransOut->strFileExtension());

    // Create the output stream for this method.
    TTextFileOutStream strmTarget
    (
        pathTarget
        , tCIDLib::ECreateActs::CreateAlways
        , tCIDLib::EFilePerms::AllAccess
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Excl_Write
    );

    // Install our stream in the translator object
    m_ptransOut->SetOutputStream(strmTarget);

    // Output the standard file header
    TString strTitle(L"Class: ");
    strTitle.Append(prsrSource.strClassName());
    strTitle.Append(L", Method: ");
    strTitle.Append(strMethodName);
    m_ptransOut->OutputPageHeader(strTitle);

    // The heading is in the first heading style
    m_ptransOut->OutputTag(TTranslator::ETags::Heading1);
    strmTarget << L"Method Name: " << strMethodName;
    m_ptransOut->OutputTag(TTranslator::ETags::EndHeading1);

    // Now handle any free form text of the general description
    m_ptransOut->TranslateTagText(prsrSource);

    //
    //  Now enter the loop where we deal with all of the tokens that
    //  are for this method. We end when we hit the end of the file or
    //  the method end token.
    //
    tCIDLib::TBoolean bInParams = kCIDLib::False;
    tCIDLib::TBoolean bInErrors = kCIDLib::False;
    tCIDLib::TBoolean bInExceptions = kCIDLib::False;
    while (1)
    {
        // Get the next token
        TDocParser::ETokens eToken = prsrSource.eNextToken();

        //
        //  If we are in a params block, then only a param or end params
        //  token is valid.
        //
        if (bInParams
        &&  ((eToken != TDocParser::ETokens::Param)
        &&   (eToken != TDocParser::ETokens::EndParams)))
        {
            throw TDocError
            (
                kDocErrs::errcExpected2
                , prsrSource.c4CurLine()
                , prsrSource.strTokenText(TDocParser::ETokens::Param)
                , prsrSource.strTokenText(TDocParser::ETokens::EndParams)
            );
        }

        //
        //  If we are in a exceptions block, then only an exception or end
        //  exceptions token is valid.
        //
        if (bInExceptions
        &&  ((eToken != TDocParser::ETokens::Exception)
        &&   (eToken != TDocParser::ETokens::EndExceptions)))
        {
            throw TDocError
            (
                kDocErrs::errcExpected2
                , prsrSource.c4CurLine()
                , prsrSource.strTokenText(TDocParser::ETokens::Exception)
                , prsrSource.strTokenText(TDocParser::ETokens::EndExceptions)
            );
        }

        //
        //  If we are in a errors block, then only an error or end errors
        //  token is valid.
        //
        if (bInErrors
        &&  ((eToken != TDocParser::ETokens::EndErrors)
        &&   (eToken != TDocParser::ETokens::Error)))
        {
            throw TDocError
            (
                kDocErrs::errcExpected2
                , prsrSource.c4CurLine()
                , prsrSource.strTokenText(TDocParser::ETokens::Error)
                , prsrSource.strTokenText(TDocParser::ETokens::EndErrors)
            );
        }

        if (eToken == TDocParser::ETokens::Caveats)
        {
            m_ptransOut->OutputTag(TTranslator::ETags::Heading2);
            strmTarget << eToken;
            m_ptransOut->OutputTag(TTranslator::ETags::EndHeading2);
            m_ptransOut->TranslateTagText(prsrSource);
        }
         else if ((eToken == TDocParser::ETokens::Params)
              ||  (eToken == TDocParser::ETokens::Errors)
              ||  (eToken == TDocParser::ETokens::Exceptions))
        {
            if (bInParams || bInExceptions || bInErrors)
            {
                throw TDocError
                (
                    kDocErrs::errcAlreadyInBlock
                    , prsrSource.c4CurLine()
                    , prsrSource.strTokenText(eToken)
                );
            }

            if (eToken == TDocParser::ETokens::Params)
                bInParams = kCIDLib::True;
            else if (eToken == TDocParser::ETokens::Errors)
                bInErrors = kCIDLib::True;
            else
                bInExceptions = kCIDLib::True;

            m_ptransOut->OutputTag(TTranslator::ETags::Heading2);
            strmTarget << eToken;
            m_ptransOut->OutputTag(TTranslator::ETags::EndHeading2);

            // Indent all the params
            m_ptransOut->OutputTag(TTranslator::ETags::Indent);

            // Output the start of a table for the params
            m_ptransOut->OutputTag(TTranslator::ETags::Table);
        }
         else if ((eToken == TDocParser::ETokens::EndParams)
              ||  (eToken == TDocParser::ETokens::EndErrors)
              ||  (eToken == TDocParser::ETokens::EndExceptions))
        {
            if (eToken == TDocParser::ETokens::EndParams)
            {
                if (!bInParams)
                {
                    throw TDocError
                    (
                        kDocErrs::errcNotInBlock
                        , prsrSource.c4CurLine()
                        , prsrSource.strTokenText(eToken)
                    );
                }
                bInParams = kCIDLib::False;
            }
             else if (eToken == TDocParser::ETokens::EndErrors)
            {
                if (!bInErrors)
                {
                    throw TDocError
                    (
                        kDocErrs::errcNotInBlock
                        , prsrSource.c4CurLine()
                        , prsrSource.strTokenText(eToken)
                    );
                }
                bInErrors = kCIDLib::False;
            }
            else
            {
                if (!bInExceptions)
                {
                    throw TDocError
                    (
                        kDocErrs::errcNotInBlock
                        , prsrSource.c4CurLine()
                        , prsrSource.strTokenText(eToken)
                    );
                }
                bInExceptions = kCIDLib::False;
            }

            m_ptransOut->OutputTag(TTranslator::ETags::EndTable);

            // And outdent now that we are done with parms
            m_ptransOut->OutputTag(TTranslator::ETags::Outdent);
        }
         else if ((eToken == TDocParser::ETokens::Param)
              ||  (eToken == TDocParser::ETokens::Error)
              ||  (eToken == TDocParser::ETokens::Exception))
        {
            //
            //  In this one, the first token is the parameter or error or
            //  exception name. So we bold it. Then the rest is free form
            //  text.
            //
            prsrSource.GetNextToken(L" ", strToken1);
            m_ptransOut->OutputTag(TTranslator::ETags::TableCol1);
            m_ptransOut->OutputTag(TTranslator::ETags::Bold);
            strmTarget << strToken1;
            m_ptransOut->OutputTag(TTranslator::ETags::EndBold);
            m_ptransOut->OutputTag(TTranslator::ETags::EndTableCol1);

            m_ptransOut->OutputTag(TTranslator::ETags::TableCol2);
            m_ptransOut->TranslateTagText(prsrSource);
            m_ptransOut->OutputTag(TTranslator::ETags::EndTableCol2);
        }
         else if (eToken == TDocParser::ETokens::Return)
        {
            m_ptransOut->OutputTag(TTranslator::ETags::Heading2);
            strmTarget << eToken;
            m_ptransOut->OutputTag(TTranslator::ETags::EndHeading2);

            m_ptransOut->OutputTag(TTranslator::ETags::Indent);
            m_ptransOut->TranslateTagText(prsrSource);
            m_ptransOut->OutputTag(TTranslator::ETags::Outdent);
        }
         else if (eToken == TDocParser::ETokens::End)
        {
            break;
        }
         else if (eToken == TDocParser::ETokens::EndMethod)
        {
            break;
        }
         else if (eToken == TDocParser::ETokens::None)
        {
            throw TDocError
            (
                kDocErrs::errcExpectedToken
                , prsrSource.c4CurLine()
            );
        }
         else
        {
            throw TDocError
            (
                kDocErrs::errcTokenBadHere
                , prsrSource.c4CurLine()
                , prsrSource.strTokenText(eToken)
            );
        }
    }

    // And output the standard footer
    m_ptransOut->OutputPageFooter();
}


//
//  This method does the actual parsing of a source file and outputs the
//  text that it translates to.
//
tCIDLib::TVoid
TFacMakeDocs::TranslateSource(  TDocParser&        prsrSource
                                , TTextOutStream&   strmTarget)
{
    // Output a little header
    m_ptransOut->OutputTag(TTranslator::ETags::Heading1);
    strmTarget << L"General Information";
    m_ptransOut->OutputTag(TTranslator::ETags::EndHeading1);

    static TString strToken1(kCIDLib::pszEmptyZStr, 1024);

    //
    //  Get the TClassDocInfo object for the class that is being parsed.
    //  This will be filled in with various information below. First make
    //  double sure its in our list of classes, which is the list of
    //  TClassInfo objects we have to work with.
    //
    if (!facMakeDocs.bIsValidClassName(prsrSource.strClassName()))
    {
        throw TDocError
        (
            kDocErrs::errcNotInClassList
            , prsrSource.c4CurLine()
            , prsrSource.strClassName()
        );
    }

    // It is in our list so get a reference to the object
    TClassDocInfo& cldiCur = cldiForClass(prsrSource.strClassName());

    while (1)
    {
        // Get the next token
        TDocParser::ETokens eToken = prsrSource.eNextToken();

        //
        //  We just go by what the token is and parse as we expect the
        //  text for that kind of token.
        //
        if ((eToken == TDocParser::ETokens::Class)
        ||  (eToken == TDocParser::ETokens::ParentClass)
        ||  (eToken == TDocParser::ETokens::Facility)
        ||  (eToken == TDocParser::ETokens::Mixins)
        ||  (eToken == TDocParser::ETokens::Prefix)
        ||  (eToken == TDocParser::ETokens::Header))
        {
            //
            //  All of these jave just one line of input and are output
            //  after putting out the text for their token and a semicolon.
            //
            prsrSource.GetLineRemainder(strToken1);
            strToken1.StripWhitespace();
            m_ptransOut->OutputTag(TTranslator::ETags::Bold);
            strmTarget << eToken << L": ";
            m_ptransOut->OutputTag(TTranslator::ETags::EndBold);

            //
            //  If this is the class name, then make sure that it agrees
            //  with the name of the file that we are parsing.
            //
            if (eToken == TDocParser::ETokens::Class)
            {
                if (prsrSource.strClassName() != strToken1)
                {
                    throw TDocError
                    (
                        kDocErrs::errcBadClassName
                        , prsrSource.c4CurLine()
                        , prsrSource.strClassName()
                        , strToken1
                    );
                }
            }

            //
            //  If this is the parent class and its not N/A, then out
            //  put it as a link. Otherwise just plain text.
            //
            if (eToken == TDocParser::ETokens::ParentClass)
            {
                if (strToken1 != L"N/A")
                    m_ptransOut->OutputLink(strToken1, strToken1);
                else
                    strmTarget << strToken1;

                // Store the parent class in the class info
                cldiCur.strParentClass(strToken1);
            }
             else if (eToken == TDocParser::ETokens::Mixins)
            {
                //
                //  Each of the mixin classes is on the same line, comma
                //  separated. We need to pull each one out and output a
                //  link for it.
                //
                TString strMixin(128UL);
                TString strDelimiters
                (
                    TString(kCIDLib::szWhitespace)
                    + TString(L",")
                );

                TStringTokenizer stokMixins(&strToken1, strDelimiters);
                while (stokMixins.bGetNextToken(strMixin))
                {
                    if (stokMixins.c4CurToken() > 1)
                        strmTarget << L", ";
                    m_ptransOut->OutputLink(strMixin, strMixin);
                }
            }
             else
            {
                strmTarget << strToken1;
            }

            m_ptransOut->OutputTag(TTranslator::ETags::Break);
        }
        else if (eToken == TDocParser::ETokens::Categories)
        {
            // Get the rest of the line and strip whitespace from it
            prsrSource.GetLineRemainder(strToken1);
            strToken1.StripWhitespace();

            //
            //  These categories need to be pulled out and added to
            //  the categories for this class. This information is not
            //  output to the target file.
            //
            TString             strCategory(128UL);
            TStringTokenizer    stokCats(&strToken1, L",");
            while (stokCats.bGetNextToken(strCategory))
            {
                // Strip the leading/trailing whitespace
                strCategory.StripWhitespace();

                // Try to map this string to a category
                tMakeDocs::EClassCats eCat = facMakeDocs.eMapCategoryName(strCategory);
                if (eCat == tMakeDocs::EClassCats::Count)
                {
                    throw TDocError
                    (
                        kDocErrs::errcUnknownCategory
                        , prsrSource.c4CurLine()
                        , strCategory
                    );
                }
                cldiCur.AddCategory(eCat);
            }
        }
        else if (eToken == TDocParser::ETokens::Caveats)
        {
            m_ptransOut->OutputTag(TTranslator::ETags::Heading3);
            strmTarget << eToken;
            m_ptransOut->OutputTag(TTranslator::ETags::EndHeading3);
            m_ptransOut->TranslateTagText(prsrSource);
        }
         else if (eToken == TDocParser::ETokens::Description)
        {
            m_ptransOut->OutputTag(TTranslator::ETags::Heading3);
            strmTarget << eToken;
            m_ptransOut->OutputTag(TTranslator::ETags::EndHeading3);
            m_ptransOut->TranslateTagText(prsrSource);
        }
         else if (eToken == TDocParser::ETokens::Group)
        {
            //
            //  These all have a heading text on one line, followed by
            //  optional free form text.
            //
            prsrSource.GetLineRemainder(strToken1);
            strToken1.StripWhitespace();

            // The heading is in the second heading style
            m_ptransOut->OutputTag(TTranslator::ETags::Heading2);
            strmTarget << eToken << L": ";
            strmTarget << strToken1;
            m_ptransOut->OutputTag(TTranslator::ETags::EndHeading2);

            // Now handle any free form text
            m_ptransOut->TranslateTagText(prsrSource);

            // And output a break
            m_ptransOut->OutputTag(TTranslator::ETags::Paragraph);
        }
         else if (eToken == TDocParser::ETokens::Method)
        {
            // The rest of the line is the method name.
            prsrSource.GetLineRemainder(strToken1);
            strToken1.StripWhitespace();

            if (strToken1.bIsEmpty())
            {
                throw TDocError
                (
                    kDocErrs::errcNoMethodName
                    , prsrSource.c4CurLine()
                );
            }

            // Build up the base name of the new file for this method
            TPathStr pathLink(prsrSource.strClassName());
            pathLink.Append(L'_');
            pathLink.Append(strToken1);

            //
            //  We need to handle spaces in the name, which the NFS file
            //  systems that these files will be one wouldn't be happy with.
            //
            pathLink.bReplaceChar(kCIDLib::chSpace, kCIDLib::chUnderscore);

            // Output a link to this new file
            m_ptransOut->OutputLink(strToken1, pathLink);
            m_ptransOut->OutputTag(TTranslator::ETags::Break);

            // Now translate the method into this new file
            TranslateMethod(prsrSource, strToken1, pathLink);

            // Put our stream back into the translator
            m_ptransOut->SetOutputStream(strmTarget);
        }
         else if (eToken == TDocParser::ETokens::Overview)
        {
            // Build up the base name of the new file for this method
            TPathStr pathLink(prsrSource.strClassName());
            pathLink.Append(L"_ClassOverviewInfo");

            // Output a link to this new file
            m_ptransOut->OutputTag(TTranslator::ETags::Paragraph);
            m_ptransOut->OutputLink(L"Class Overview", pathLink);
            m_ptransOut->OutputTag(TTranslator::ETags::Break);

            // Now translate the overview info into this new file.
            TranslateOverview(prsrSource, pathLink);

            // Put our stream back into the translator
            m_ptransOut->SetOutputStream(strmTarget);
        }
         else if (eToken == TDocParser::ETokens::Related)
        {
            //
            //  Each line after this tag indicates a link to a related
            //  class.
            //
            m_ptransOut->OutputTag(TTranslator::ETags::Heading2);
            strmTarget << L"Related Topics";
            m_ptransOut->OutputTag(TTranslator::ETags::EndHeading2);

            while (1)
            {
                prsrSource.GetNextToken(kCIDLib::szWhitespace, strToken1);
                if (strToken1.bIsEmpty())
                    break;

                if (strToken1 == L"//")
                {
                    prsrSource.FlushLine();
                    continue;
                }

                m_ptransOut->OutputLink(strToken1, strToken1);
                m_ptransOut->OutputTag(TTranslator::ETags::Break);
            }
        }
         else if (eToken == TDocParser::ETokens::End)
        {
            break;
        }
         else if (eToken == TDocParser::ETokens::None)
        {
            throw TDocError
            (
                kDocErrs::errcExpectedToken
                , prsrSource.c4CurLine()
            );
        }
         else
        {
            throw TDocError
            (
                kDocErrs::errcTokenBadHere
                , prsrSource.c4CurLine()
                , prsrSource.strTokenText(eToken)
            );
        }
    }
}
