//
// FILE NAME: CML1.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/16/2019
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
//  This is the main module for a program that demonstrates how to use CML from
//  within your CIDLib applications. It doesn't do anything that would in reality
//  justify using CML, in order to keep the focus on the mechanics of embedding
//  the CML engine and interacting with it.
//
//  This one emulates a scenario where you'd use CML to support user customization
//  allowing the user to write CML code which you execute. For this very simple
//  example we just let them fill in some parameters. In reality you would likely
//  create a CML class, create an instance of it, and let them fill that in on the
//  CML side.
//
//  But, to keep things simple for this first CML based sample, we'll just use
//  two output string parameters, one for a name and one for an address. The CML
//  code will fill those in, and we display the output.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  We need the macro engine, which will also bring underlying stuff that we
//  need.
// ---------------------------------------------------------------------------
#include    "CIDMacroEng.hpp"


// ---------------------------------------------------------------------------
//  For this simple program, start main thread on a global
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"CML1MainThread", eMainThreadFunc))

tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    TTextOutStream& strmOut = TSysInfo::strmOut();
    try
    {
        //
        //  We need a CML macro engine. Give it an output stream to use for
        //  CML console output, though we don't actually do any in our CML
        //  macro.
        //
        TCIDMacroEngine meCustom;
        meCustom.SetConsole(&strmOut);

        //
        //  We need a class manager (which resolves CML class paths to
        //  something that can be loaded.) We use a simple one that we can
        //  just point at a top level local directory. We point it a Classes
        //  directory under this sample directory. That becomes the top
        //  level MEng. of class paths.
        //
        TString strCurDir;
        TFileSys::QueryCurrentDir(strCurDir);
        TMEngFixedBaseClassMgr mecmLocal(strCurDir, L"Classes");

        //
        //  Though we don't use it, we need a file resolver to resolve the
        //  paths of files accessed from CML to loadable files, in our case
        //  a Files directory under our sample dir.
        //
        TMEngFixedBaseFileResolver mefrLocal(strCurDir, L"Files");

        //
        //  Give the parser and macro engine error handlers a stream to send
        //  errors to. We just use our local output console. In a more realistic
        //  scenario we might give it a memory or string based stream so that we
        //  can display the errors in a popup or something. Or use handlers that
        //  queue them up for later processing
        //
        TMEngStrmPrsErrHandler  meehParser;
        TMEngStrmErrHandler     meehEngine;
        meehParser.SetStream(&strmOut);
        meehEngine.SetStream(&strmOut);

        // Set the error handler and file resolver on the macro engine
        TMacroEngParser meprsCust;
        meCustom.SetErrHandler(&meehEngine);
        meCustom.SetFileResolver(&mefrLocal);

        //
        //  The parse will set up the macro engine and gives us back a pointer
        //  to the main class info class, the type we need to create. We give
        //  it the error handler and class manager to use.
        //
        TMEngClassInfo* pmeciMain;
        if (!meprsCust.bParse(  L"MEng.User.Customize"
                                , pmeciMain
                                , &meCustom
                                , &meehParser
                                , &mecmLocal))
        {
            strmOut << L"The parsed failed" << kCIDLib::NewEndLn;
            return tCIDLib::EExitCodes::RuntimeError;
        }

        // Create an object of the main class. Put a janitor on it
        TMEngClassVal* pmecvTarget = pmeciMain->pmecvMakeStorage
        (
            L"$Main$", meCustom, tCIDMacroEng::EConstTypes::NonConst
        );
        TJanitor<TMEngClassVal> janTarget(pmecvTarget);

        // Invoke the default CML constructor. If it failed, give up
        if (!meCustom.bInvokeDefCtor(*pmecvTarget, 0))
        {
            strmOut << L"The default constructor failed" << kCIDLib::NewEndLn;
            return tCIDLib::EExitCodes::RuntimeError;
        }

        //
        //  Set up a couple output string parameters. We have to get the string
        //  class class, which we can use to create instances of that type. The
        //  engine has a helper to add some common parameter types. These must
        //  match the paramters of the Start() method in the CML macro.
        //
        TCIDMacroEngine::TParmList colParms(tCIDLib::EAdoptOpts::Adopt);
        meCustom.AddStringParm(colParms, L"Name", kCIDLib::False);
        meCustom.AddStringParm(colParms, L"Addr", kCIDLib::False);

        // And now run the macro, passing the parameters
        if (!meCustom.i4Run(*pmecvTarget, colParms, 0) != 0)
        {
            strmOut << L"The macro failed to run" << kCIDLib::NewEndLn;
            return tCIDLib::EExitCodes::RuntimeError;
        }

        // Let's display the name and address the CML code set
        strmOut << L"Name="
                << static_cast<TMEngStringVal*>(colParms[0])->strValue()
                << L"\nAddr="
                << static_cast<TMEngStringVal*>(colParms[1])->strValue()
                << kCIDLib::NewEndLn;
    }

    catch(const TError& errToCatch)
    {
        strmOut << L"An exception occurred:\n";
        TStreamIndentJan janIndent(&strmOut, 8);
        strmOut << errToCatch << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    return tCIDLib::EExitCodes::Normal;
}
