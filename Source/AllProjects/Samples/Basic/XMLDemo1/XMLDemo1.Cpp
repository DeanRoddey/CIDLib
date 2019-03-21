//
// FILE NAME: XMLDemo1.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/21/99
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
//  This is the main module for the first of the XML demo programs. This one
//  shows how to use the core XML services at its lowest level. It just
//  uses the core parser and installs handlers for event callbacks, which are
//  very fast and which provide maximum access to the XML data parsed.
//
//  If you need sparse access to the data, and there's a good bit of it, this
//  is far more efficient than the tree parser that builds the whole thing
//  into memory.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Include our own header which will bring in what we need and define our
//  facility object.
// ---------------------------------------------------------------------------
#include    "XMLDemo1.Hpp"


// ----------------------------------------------------------------------------
//  Global Data
//
//  facXMLDemo1
//      The facility object for this program. This object implements all of the
//      callback interfaces from that XML parser core that we need. So it will
//      be informed of all of the XML events and handle them.
// ----------------------------------------------------------------------------
TFacXMLDemo1     facXMLDemo1;


// ----------------------------------------------------------------------------
//  Magic macros for the facility class
// ----------------------------------------------------------------------------
RTTIDecls(TFacXMLDemo1,TFacility)


// ----------------------------------------------------------------------------
//  Include magic main module code
//
//  Note that we make the main thread run a member of the facility object. The
//  TMemberFunc<> template class takes a pointer to the object and the name of a
//  member on that object to run. It has to match the required prototype. See
//  the TMemberFunc class for details.
// ----------------------------------------------------------------------------
CIDLib_MainModule
(
    TThread
    (
        L"XMLDemo1MainThread"
        , TMemberFunc<TFacXMLDemo1>(&facXMLDemo1, &TFacXMLDemo1::eMainThreadFunc)
    )
)




// ---------------------------------------------------------------------------
//   CLASS: TFacXMLDemo1
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacXMLDemo1: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacXMLDemo1::TFacXMLDemo1() :

    TFacility
    (
        L"XMLDemo1"
        , tCIDLib::EModTypes::Exe
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::None
    )
    , m_bShowWarnings(kCIDLib::False)
    , m_bTimeIt(kCIDLib::False)
    , m_c4ElemDepth(0)
    , m_c4EntityNesting(0)
    , m_c4MaxErrs(1)
    , m_eDisplayMode(EDisplayModes::None)
    , m_eOpts(tCIDXML::EParseOpts::None)
    , m_pstrmErr(nullptr)
    , m_pstrmOut(nullptr)
{
    //
    //  Create our output streams. If they are not redirected, then they just
    //  go to the console. Otherwise, we create file streams with UTF-8
    //  text converters.
    //
    if (TFileSys::bIsRedirected(tCIDLib::EStdFiles::StdOut))
    {
        m_pstrmOut = new TTextFileOutStream
        (
            tCIDLib::EStdFiles::StdOut
            , new TUTFConverter(TUTFConverter::EEncodings::UTF8)
        );
    }
     else
    {
        m_pstrmOut = new TOutConsole;
    }

    if (TFileSys::bIsRedirected(tCIDLib::EStdFiles::StdErr))
    {
        m_pstrmErr = new TTextFileOutStream
        (
            tCIDLib::EStdFiles::StdErr
            , new TUTFConverter(TUTFConverter::EEncodings::UTF8)
        );
    }
     else
    {
        if (!TFileSys::bIsRedirected(tCIDLib::EStdFiles::StdOut))
            m_pstrmErr = m_pstrmOut;
        else
            m_pstrmErr = new TOutConsole;
    }
}

TFacXMLDemo1::~TFacXMLDemo1()
{
    if (m_pstrmErr != m_pstrmOut)
        delete m_pstrmErr;
    delete m_pstrmOut;
}


// ---------------------------------------------------------------------------
//  TFacXMLDemo1: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This is the the thread function for the main thread. It is started by
//  the CIDLib_MainModule() macro above.
//
tCIDLib::EExitCodes TFacXMLDemo1::eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    try
    {
        // Get the command line parms
        if (TSysInfo::c4CmdLineParmCount() < 1)
        {
            ShowUsage();
            return tCIDLib::EExitCodes::BadParameters;
        }

        TSysInfo::TCmdLineCursor cursParms = TSysInfo::cursCmdLineParms();
        TString strFileParm = *cursParms++;

        // Check for option parameters
        tCIDLib::TCard4     c4MaxErrs = 1;
        TString             strTmp;
        for (; cursParms; ++cursParms)
        {
            strTmp = *cursParms;
            if (strTmp.bCompareI(L"/Validate"))
            {
                m_eOpts = tCIDLib::eOREnumBits(m_eOpts, tCIDXML::EParseOpts::Validate);
            }
             else if (strTmp.bCompareI(L"/Canonical"))
            {
                m_eDisplayMode = EDisplayModes::Canonical;
            }
             else if (strTmp.bCompareI(L"/ErrLoc"))
            {
                m_eDisplayMode = EDisplayModes::ErrLoc;
            }
             else if (strTmp.bCompareI(L"/ErrLoc2"))
            {
                m_eDisplayMode = EDisplayModes::ErrLoc2;
            }
             else if (strTmp.bCompareI(L"/IgnoreDTD"))
            {
                m_eOpts = tCIDLib::eOREnumBits(m_eOpts, tCIDXML::EParseOpts::IgnoreDTD);
            }
             else if (strTmp.bCompareI(L"/Std"))
            {
                m_eDisplayMode = EDisplayModes::Standard;
            }
             else if (strTmp.bCompareI(L"/ShowWarnings"))
            {
                m_bShowWarnings = kCIDLib::True;
            }
             else if (strTmp.bCompareI(L"/Time"))
            {
                m_bTimeIt = kCIDLib::True;
            }
             else if (strTmp.bCompareNI(L"/MaxErr=", 8))
            {
                strTmp.Cut(0,8);
                c4MaxErrs = strTmp.c4Val();
                if (!c4MaxErrs)
                    *m_pstrmOut << L"Invalid maximum error value\n" << kCIDLib::EndLn;
            }
             else if (strTmp.bStartsWithI(L"/Mapping:"))
            {
                //
                //  Parse out this mapping and add it to our catalog, which
                //  we'll use to do entity redirection. They look like:
                //
                //      /Mapping:pubid=sysid
                //
                //  so its a mapping of a public id to a system id
                //
                strTmp.Cut(0, 9);
                if (!bLoadMapping(strTmp))
                    return tCIDLib::EExitCodes::BadParameters;
            }
             else
            {
                *m_pstrmOut << L"Unknown option: " << strTmp << kCIDLib::EndLn;
                ShowUsage();
                return tCIDLib::EExitCodes::BadParameters;
            }
        }

        // Do the parse operation
        TXMLParserCore xprsTest;
        DoParse(xprsTest, strFileParm);

        #if CID_DEBUG_ON
        #define DEBUG_LEAKS 0
        #if DEBUG_LEAKS
        TKrnlMemCheck kmchkTest;
        kmchkTest.ReportToFile(L"MemDump.Txt");

        xprsTest.Reset();
        kmchkTest.TakeSnapshot();

        DoParse(xprsTest, strFileParm);
        DoParse(xprsTest, strFileParm);
        DoParse(xprsTest, strFileParm);
        DoParse(xprsTest, strFileParm);
        DoParse(xprsTest, strFileParm);
        DoParse(xprsTest, strFileParm);
        DoParse(xprsTest, strFileParm);
        DoParse(xprsTest, strFileParm);
        DoParse(xprsTest, strFileParm);

        xprsTest.Reset();
        kmchkTest.DumpSnapDiffs();

        #endif
        #endif

        // Flush the output stream to force all output out
        m_pstrmOut->Flush();
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
        *m_pstrmOut << L"A CIDLib runtime error occured during processing.\n"
                    << L"Error: " << errToCatch.strErrText() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but I test
    //  for them in my demo programs so I can catch them if they do
    //  and fix them.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        *m_pstrmOut << L"A kernel error occured during processing.\nError="
                    << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        *m_pstrmOut << L"A general exception occured during processing"
                    << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }

    return tCIDLib::EExitCodes::Normal;
}


// ---------------------------------------------------------------------------
//  TFacXMLDemo1: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TFacXMLDemo1::bLoadMapping(TString& strMapping)
{
    //
    //  There should be one and only one = sign and it must not be first
    //  or last char.
    //
    tCIDLib::TCard4 c4Ofs;
    if (!strMapping.bFirstOccurrence(kCIDLib::chEquals, c4Ofs)
    ||  !c4Ofs
    ||  (c4Ofs == (strMapping.c4Length() - 1)))
    {
        TSysInfo::strmErr() << L"\nThe mapping '" << strMapping
                            << L"' was not well formed"
                            << kCIDLib::EndLn;
        return kCIDLib::False;
    }

    //
    //  Break out the two parts into separate strings. The incoming string is
    //  not const, so we can keep the base part there, and copy the other part
    //  out to a second string.
    //
    TString strSysId;
    strMapping.CopyOutSubStr(strSysId, c4Ofs + 1);
    strMapping.CapAt(c4Ofs);

    // And add this new item to the catalog
    TFileEntitySrc* pxesNew = new TFileEntitySrc(strSysId);
    pxesNew->strPublicId(strMapping);
    m_xcatMappings.AddMapping(pxesNew);
    return kCIDLib::True;
}


//
//  We split out the actual work to aid with heap leak checks. We can invoke
//  it once from the main thread method and then invoke it more times to
//  watch for leaks. So this way we completely destroy everything again
//  after we are done, so subsequent invocations shouldn't really create
//  any more faulted in data. We also take a parser to use, so that we
//  can check when using the same parser, or destroying one and creating
//  another, to see if there's any difference.
//
tCIDLib::TVoid
TFacXMLDemo1::DoParse(TXMLParserCore& xprsTest, const TString& strToParse)
{
    //
    //  Create the validator, and give him a pointer to the parser. He has to
    //  use this to do a number of things, e.g. reporting errors. The parser
    //  adopts the validator but we keep a pointer so we can directly access
    //  it if needed.
    //
    m_pxvalTest = new TDTDValidator(&xprsTest);
    xprsTest.pxvalValidator(m_pxvalTest);

    //
    //  We always install the entity event handler in this program. Its used
    //  to maintain an entity nesting level counter.
    //
    xprsTest.pmxevEntityEvents(this);

    // Set up the max errors value we defaulted or were given
    xprsTest.c4MaxErrors(m_c4MaxErrs);

    //
    //  We have to set the parse flags so that we get only what we are
    //  interested in. If we are doing canonical format, we don't want
    //  any stuff outside the content.
    //
    //  If we are doing canonical, we don't add ou ourselfs as the DTD event
    //  handler on the validator because we don't want any stuff from there
    //  (and the parse flags only control optional stuff not the markup decl
    //  stuff.)
    //
    tCIDXML::EParseFlags eFlags = tCIDXML::EParseFlags::Standard;
    if (m_eDisplayMode == EDisplayModes::Canonical)
    {
        eFlags = tCIDXML::EParseFlags::JustContent
                 | tCIDXML::EParseFlags::PIsAC
                 | tCIDXML::EParseFlags::PIsBC;

        // We only need to add the document events and errors
        xprsTest.pmxevDocEvents(this);
        xprsTest.pmxevErrorEvents(this);
    }
     else if ((m_eDisplayMode == EDisplayModes::ErrLoc)
          ||  (m_eDisplayMode == EDisplayModes::ErrLoc2))
    {
        // We just need to add the error events
        xprsTest.pmxevErrorEvents(this);
    }
     else
    {
        //
        //  Its just normal output mode, so we need all the event handlers
        xprsTest.pmxevDocEvents(this);
        xprsTest.pmxevErrorEvents(this);
        m_pxvalTest->pmxevDTDEventHandler(this);
    }

    //
    //  If we got any entity mappings, set us as the entity resolver. Otherwise
    //  there aren't any to resolve.
    //
    if (!m_xcatMappings.c4MapCount())
        xprsTest.pmxevEntityResolver(this);

    // Get the current time
    TTime tmStart(tCIDLib::ESpecialTimes::CurrentTime);

    // And now parse the file
    xprsTest.ParseRootEntity(strToParse, m_eOpts, eFlags);

    // Get the end time
    TTime tmEnd(tCIDLib::ESpecialTimes::CurrentTime);
    if (m_bTimeIt)
    {
        *m_pstrmOut << L"Elapsed MS: "
                    << (tmEnd.enctMilliSeconds() - tmStart.enctMilliSeconds())
                    << kCIDLib::EndLn;
    }

    //
    //  The parser adopts the validator, so we need to clear our pointer,
    //  and force the parser to drop it as well, since we'll create a new
    //  one next time.
    //
    m_pxvalTest = nullptr;
    xprsTest.pxvalValidator(nullptr);
}


tCIDLib::TVoid TFacXMLDemo1::ShowUsage()
{
    *m_pstrmOut <<  L"Usage: XMLDemo1 filepath [options]\n"
                    L"       Options:\n"
                    L"          /Validate\n"
                    L"          /IgnoreDTD\n"
                    L"          /Mapping:pubid=sysid\n"
                    L"          /[Canonical | ErrLoc | ErrLoc2 | Std]\n\n"
                    L"   * Using /Validate and /IgnoreDTD will not work of course\n"
                    L"     because you told it to ignore the DTD it needs in order\n"
                    L"     to validate. You would ignore the DTD if it isn't actually\n"
                    L"     available, i.e. it's just some public ID reference\n\n"

                    L"   * You can output in a few different formats. Std is the normal\n"
                    L"     sort of readable format. The ErrLoc types only output errors,\n"
                    L"     in a well defined way to help make automated location easier.\n"
                    L"     Canonical is a format that could be used to look for content\n"
                    L"     changes, i.e. not just format differences.\n\n"

                    L"     You can use /Mapping to map a public id to a file, such as the\n"
                    L"     DTD public id, so that validation can be done. It could be mapped\n"
                    L"     to a URL but this program doesn't support that."
                << kCIDLib::EndLn;
}
