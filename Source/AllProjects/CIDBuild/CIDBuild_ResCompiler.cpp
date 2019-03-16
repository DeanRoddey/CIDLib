//
// FILE NAME: CIDBuild_ResCompiler.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/29/1998
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
//  This file implements the TResCompiler class. It processes the resource
//  files of a project and spits out the binary respresentation and the
//  header files with the resource ids.
//
//  A project can have no resources, in which case this class won't be used
//  for it. Or, it can have a message text file, for loadable message text,
//  and/or a GUI resource file.
//
//  Some GUI resources can refer to message text symbols, which allows it
//  to have text loaded into it automatically. For instance, a pushbutton
//  might want to refer to a text id for its button text. If so, the project
//  must have a message text file that defines those text messages.
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
//   CLASS: TResCompiler
//  PREFIX: msgp
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TResCompiler: Static data membres
// ---------------------------------------------------------------------------
tCIDLib::TBoolean   TResCompiler::s_bLoadedCommIds;
TList<TIdInfo>      TResCompiler::s_listCommonMsgIds;


// ---------------------------------------------------------------------------
//  TResCompiler: Constructors and Destructor
// ---------------------------------------------------------------------------
TResCompiler::TResCompiler( const   TProjectInfo&       projiSrc
                            , const tCIDLib::TBoolean   bVerbose
                            , const tCIDLib::TBoolean   bForce) :
    m_bForce(bForce)
    , m_bVerbose(bVerbose)
    , m_plsplInput(nullptr)
    , m_projiSrc(projiSrc)
{
}

TResCompiler::~TResCompiler()
{
    // Delete any all the files that got created
    delete m_plsplInput;
}


// ---------------------------------------------------------------------------
//  TResCompiler: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TResCompiler::bProcess()
{
    //
    //  We always need to process and load the common text symbols file, so
    //  do that first if it's not already been done (it's static data.)
    //
    if (!s_bLoadedCommIds)
        LoadCommonText();

    //
    //  Default the namespaces that we are to use in any header files we
    //  need to generate. They will be in the form:
    //
    //  kProjnameErrs   (error ids)
    //  kProjnameMsgs   (msg ids)
    //  kProjname       (resource ids)
    //
    m_strErrNamespace = L"k";
    m_strErrNamespace.Append(m_projiSrc.strProjectName());
    m_strMsgNamespace = m_strErrNamespace;
    m_strResNamespace = m_strErrNamespace;
    m_strErrNamespace.Append(L"Errs");
    m_strMsgNamespace.Append(L"Msgs");

    //
    //  Process the resource files that this project has. We have to do the
    //  messages first because GUI resources can refer to loadable text
    //  symbols. Once we've parsed the messages, the m_listMsgIds member is
    //  filled in with all of the message id info that we need.
    //
    tCIDLib::TBoolean bBuiltMessages = kCIDLib::False;
    if (m_projiSrc.bHasMsgFile())
        bBuiltMessages = bProcessMsgs();

    if (m_projiSrc.bHasResFile())
        ProcessResources(bBuiltMessages);

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TResCompiler: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Looks for a loadable text symbol in the loadable text file for the
//  facility whose resources are currently being processed.
//
tCIDLib::TBoolean
TResCompiler::bFindMsgSymbol(const  tCIDLib::TCh* const pszSymName
                            ,       tCIDLib::TMsgId&    midToFill) const
{
    // Default the return value just in case
    midToFill = kCIDLib::c4MaxCard;

    // Search the msg id info list for the passed symbol
    tCIDLib::TBoolean       bRet = kCIDLib::False;
    TList<TIdInfo>::TCursor cursIds(&m_listMsgIds);
    if (cursIds.bResetIter())
    {
        do
        {
            const TIdInfo& idiCur = cursIds.tCurElement();

            if (idiCur.strName() == pszSymName)
            {
                midToFill = idiCur.c4Id();
                bRet = kCIDLib::True;
                break;
            }
        }   while (cursIds.bNext());
    }
    return bRet;
}


//
//  Looks for a special loadable text symbol. If it's in the form [xxx],
//  we pull out the xxx part and see if it matches a symbol in our special
//  loadable text file. If so, then we return the id for that symbol, but
//  with the high bit turned on to indicate it's a special symbol id.
//
tCIDLib::TBoolean
TResCompiler::bFindSpecialMsgSymbol(const  tCIDLib::TCh* const  pszSymName
                                    ,       tCIDLib::TMsgId&    midToFill) const
{
    // Default the return value just in case
    midToFill = kCIDLib::c4MaxCard;

    //
    //  If it doesn't start with [, then obviousy not a special symbol. This
    //  would include an empty string of course.
    //
    if (*pszSymName != L'[')
        return kCIDLib::False;

    // Looks like one, so build up the actual id
    TBldStr strSym(L"midCTxt_");
    strSym.Append(&pszSymName[1]);
    strSym.DeleteLast();

    // Search the msg id info list for the passed symbol
    tCIDLib::TBoolean       bRet = kCIDLib::False;
    TList<TIdInfo>::TCursor cursIds(&s_listCommonMsgIds);
    if (cursIds.bResetIter())
    {
        do
        {
            const TIdInfo& idiCur = cursIds.tCurElement();

            if (idiCur.strName() == strSym)
            {
                midToFill = idiCur.c4Id();
                midToFill |= 0x80000000;
                bRet = kCIDLib::True;
                break;
            }
        }   while (cursIds.bNext());
    }
    return bRet;
}


//
//  A simple specalized tokenizer that supports our comma separated format
//  for the values of fields in resource source files.
//
tCIDLib::TCard4
TResCompiler::c4SplitFields(const   TBldStr&        strSrc
                            ,       TBldStr         strFlds[]
                            , const tCIDLib::TCard4 c4MaxFlds
                            , const tCIDLib::TCard4 c4LineNum)
{
    // Get a copy of the value that we can tokenize, and put a janitor on it
    tCIDLib::TCh* pszTmp = strSrc.pszDupBuffer();
    TArrayJanitor<tCIDLib::TCh> janTmp(pszTmp);

    // And break out the values and store them
    tCIDLib::TCard4 c4Found = 0;
    tCIDLib::TCh* pszTok = TRawStr::pszStrTok(pszTmp, L" \t,");
    while (pszTok)
    {
        if (c4MaxFlds == c4Found)
        {
            stdOut  << L"(Line " << c4LineNum << L") Too many fields found"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::IndexError;
        }

        // Store this one and look again
        strFlds[c4Found++] = pszTok;
        pszTok = TRawStr::pszStrTok(0, L" \t,");
    }
    return c4Found;
}


//
//  There are a set of common message ids that can be referenced in any facility's
//  dialog definitions. The list is static so for any one run of the the build tool,
//  we only load this once.
//
//  Note that we have to load the correct one for the current language. And note that
//  it'll get parsed again later in the normal way.
//
//  These are always referenced indirectly via the [xxx] type syntax in RC files, but we
//  still generate a header file so that they can be loaded programmatically when needed.
//
tCIDLib::TVoid TResCompiler::LoadCommonText()
{
    // Set this first, we only want to try once, even if we fail below
    s_bLoadedCommIds = kCIDLib::True;

    //
    //  The file has a fixed name and is in the main source directory of the CIDLib
    //  source tree.
    //
    TFindInfo   fndiSrc;
    TBldStr     strPath(facCIDBuild.strCIDLibSrcDir());
    strPath.Append(L"Source\\AllProjects\\CIDCommonText");
    strPath.Append(L"_");
    strPath.Append(facCIDBuild.strLangSuffix());
    strPath.Append(L".MsgText");

    if (!TFindInfo::bFindAFile(strPath, fndiSrc))
    {
        stdOut  << L"The indicated common loadable text file was not found. "
                   L" File=" << strPath << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::NotFound;
    }

    // Create the input spooler and load up the contents
    m_plsplInput = new TLineSpooler(strPath, kCIDLib::True);
    try
    {
        ProcessMsgFile(s_listCommonMsgIds);
    }

    catch(...)
    {
        stdOut << L"Failed to load common text messages" << kCIDBuild::EndLn;
        delete m_plsplInput;
        m_plsplInput = nullptr;
        throw;
    }

    delete m_plsplInput;
    m_plsplInput = nullptr;

    //
    //  Ok, if we also need to spit out the binary file, then let's create a spooler
    //  and spit it out. We do this if it doesn't exist, or if the source is newer.
    //
    strPath = facCIDBuild.strOutDir();
    strPath.Append(L"CIDCommonText");
    strPath.Append(L"_");
    strPath.Append(facCIDBuild.strLangSuffix());
    strPath.Append(L".CIDMsg");
    TFindInfo fndiTarget;
    if (!TFindInfo::bFindAFile(strPath, fndiTarget) || (fndiSrc > fndiTarget))
        OutputBinMsgData(strPath, s_listCommonMsgIds);
}

