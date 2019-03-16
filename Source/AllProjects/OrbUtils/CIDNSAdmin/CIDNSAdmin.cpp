//
// FILE NAME: CIDNSAdmin.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/20/2001
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
#include    "CIDOrbUC.hpp"



// ----------------------------------------------------------------------------
//  Forward references
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);


// ----------------------------------------------------------------------------
//  Main module magic macro
// ----------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"MainThread", eMainThreadFunc))


// ----------------------------------------------------------------------------
//  Local data
// ----------------------------------------------------------------------------
namespace NSAdmin
{
    // ------------------------------------------------------------------------
    //  Local types
    // ------------------------------------------------------------------------
    enum ECmds
    {
        ECmd_ClearScope
        , ECmd_Dump
        , ECmd_MakeScope
        , ECmd_RemoveScope
        , ECmd_ShowScope

        , ECmds_Count
    };

    enum ENodeTypes
    {
        ENodeType_Scope
        , ENodeType_Binding

        , ENodeTypes_Count
    };


    // ------------------------------------------------------------------------
    //  Local variables
    //
    //  eCmd
    //      The first parameter is the command which we translate to one of
    //      the command enum values.
    //
    //  eDumpFmt
    //      If the command dumps out the tree or parts of it, this controls
    //      the format we ask the name server to give us.
    //
    //  eNodeType
    //      If the command acts on either scopes or bindings, this indicates
    //      which one to do.
    //
    //  strCmd
    //      This is the command string, which tells us what we are to do
    //
    //  strDescription
    //      If the command creates a scope or binding, it can have a description
    //      field, which is stored here.
    //
    //  strParent
    //      If the command requires a parent scope. this is where its stored.
    //
    //  strScope
    //      If the command requires a scope, this is where its stored
    //
    //  strTarget
    //      If the command has a target, it is stored here
    // ------------------------------------------------------------------------
    ECmds                   eCmd;
    tCIDOrbUC::EDumpFmts eDumpFmt = tCIDOrbUC::EDumpFmts::PlainText;
    ENodeTypes              eNodeType;
    TString                 strCmd;
    TString                 strDescription;
    TString                 strParent;
    TString                 strScope;
    TString                 strTarget;
}


// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------
static tCIDLib::TBoolean bXlatNodeType(const TString& strText)
{
    if (strText.bCompareI(L"scope"))
        NSAdmin::eNodeType = NSAdmin::ENodeType_Scope;
    else if (strText.bCompareI(L"binding"))
        NSAdmin::eNodeType = NSAdmin::ENodeType_Binding;
    else
        return kCIDLib::False;
    return kCIDLib::True;
}


static tCIDLib::TBoolean bParseParms()
{
    const tCIDLib::TCard4   c4Count = TSysInfo::c4CmdLineParmCount();

    // Have to have at least a command
    if (c4Count == 0)
        return kCIDLib::False;

    //
    //  The first parameter must the be command. Which command we get drives
    //  what other options we need.
    //
    TSysInfo::TCmdLineCursor cursParms = TSysInfo::cursCmdLineParms();
    TString strCurParm = *cursParms++;

    // Translate it to a command enum
    if (strCurParm.bCompareI(L"clearscope"))
        NSAdmin::eCmd = NSAdmin::ECmd_ClearScope;
    else if (strCurParm.bCompareI(L"dump"))
        NSAdmin::eCmd = NSAdmin::ECmd_Dump;
    else if (strCurParm.bCompareI(L"makescope"))
        NSAdmin::eCmd = NSAdmin::ECmd_MakeScope;
    else if (strCurParm.bCompareI(L"removescope"))
        NSAdmin::eCmd = NSAdmin::ECmd_RemoveScope;
    else if (strCurParm.bCompareI(L"showscope"))
        NSAdmin::eCmd = NSAdmin::ECmd_ShowScope;
    else
    {
        TSysInfo::strmOut() << strCurParm << L" is not a valid command" << kCIDLib::EndLn;
        return kCIDLib::False;
    }

    if ((NSAdmin::eCmd == NSAdmin::ECmd_ClearScope)
    ||  (NSAdmin::eCmd == NSAdmin::ECmd_ShowScope)
    ||  (NSAdmin::eCmd == NSAdmin::ECmd_RemoveScope))
    {
        // We have to have one more parm, which is the scope name
        if (!cursParms.bIsValid())
            return kCIDLib::False;

        NSAdmin::strTarget = *cursParms++;
    }
     else if (NSAdmin::eCmd == NSAdmin::ECmd_MakeScope)
    {
        if (c4Count < 4)
            return kCIDLib::False;

        // We have to have three more parms, a parent scope, a name and a description
        NSAdmin::strParent = *cursParms++;
        NSAdmin::strTarget = *cursParms++;
        NSAdmin::strDescription = *cursParms++;
    }

    // And now look for any remaining optional parameters
    for (; cursParms; ++cursParms)
    {
        strCurParm = *cursParms;

        if (strCurParm.bStartsWithI(L"/Format="))
        {
            strCurParm.Cut(0, 8);
            if (strCurParm.bCompareI(L"XML"))
                NSAdmin::eDumpFmt = tCIDOrbUC::EDumpFmts::XML;
            else if (strCurParm.bCompareI(L"Text"))
                NSAdmin::eDumpFmt = tCIDOrbUC::EDumpFmts::PlainText;
            else
            {
                TSysInfo::strmOut() << strCurParm << L" is not a valid format"
                                    << kCIDLib::EndLn;
            }
        }
         else
        {
            TSysInfo::strmOut() << L"Unknown parameter: " << strCurParm << kCIDLib::EndLn;
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}


static tCIDLib::TVoid ShowUsage()
{
    TSysInfo::strmOut() << L"Usage:\n"
                        << L"   NSAdmin cmd [cmdopts* /NSAddr=xx /Format=xx"
                        << L"\n"
                        << L"   Cmd = Dump | Show | Make\n"
                        << L"\n"
                        << L"   Options depend on the command\n"
                        << kCIDLib::FlushIt;
}


//
//  This is the the thread function for the main thread. We just parse the
//  command line parameters, validate them, crank up the Orb client, and do
//  the requested operation.
//
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    try
    {
        if (!bParseParms())
        {
            ShowUsage();
            return tCIDLib::EExitCodes::BadParameters;
        }

        // Crank up the Orb client
        facCIDOrb().InitClient();

        // And get a name server client proxy
        tCIDOrbUC::TNSrvProxy orbcNS = facCIDOrbUC().orbcNameSrvProxy();

        if (NSAdmin::eCmd == NSAdmin::ECmd_ClearScope)
        {
            orbcNS->ClearScope(NSAdmin::strTarget);
        }
         else if (NSAdmin::eCmd == NSAdmin::ECmd_Dump)
        {
            TString strDump;
            orbcNS->Dump(strDump, NSAdmin::eDumpFmt);
            TSysInfo::strmOut() << strDump << kCIDLib::FlushIt;
        }
         else if (NSAdmin::eCmd == NSAdmin::ECmd_MakeScope)
        {
            orbcNS->CreateScope
            (
                NSAdmin::strParent
                , NSAdmin::strTarget
                , NSAdmin::strDescription
            );
        }
         else if (NSAdmin::eCmd == NSAdmin::ECmd_RemoveScope)
        {
            orbcNS->RemoveScope(NSAdmin::strTarget);
        }
         else if (NSAdmin::eCmd == NSAdmin::ECmd_ShowScope)
        {
            // Do the subscopes first
            tCIDLib::TKVPList colSubscopes;
            tCIDLib::TCard4 c4Count = orbcNS->c4EnumSubScopes
            (
                NSAdmin::strTarget
                , colSubscopes
                , kCIDLib::True
            );

            if (c4Count)
            {
                TSysInfo::strmOut() << L"Subscopes:\n";
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
                    TSysInfo::strmOut() << L"  " << colSubscopes[c4Index] << kCIDLib::EndLn;
            }

            // And show any bindings
            TVector<TNameServerInfo> colBindings;
            c4Count = orbcNS->c4EnumObjects
            (
                NSAdmin::strTarget
                , colBindings
                , kCIDLib::True
            );

            if (c4Count)
            {
                TSysInfo::strmOut() << L"Bindings:\n";
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
                {
                    const TNameServerInfo& nsiCur = colBindings[c4Index];

                    TSysInfo::strmOut() << L"  "
                                        << nsiCur.strNodeName()
                                        << L"\n      Descr: "
                                        << nsiCur.strDescription()
                                        << L"\n     Extra1: "
                                        << nsiCur.strExtra1()
                                        << L"\n     Extra1: "
                                        << nsiCur.strExtra2()
                                        << L"\n     Extra1: "
                                        << nsiCur.strExtra3()
                                        << kCIDLib::EndLn;
                }
            }
        }
         else
        {
            TSysInfo::strmOut() << L"Got an unknown ECmds value" << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::InternalError;
        }
    }

    catch(TError& errToCatch)
    {
        // If not logged, then log it.
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        TSysInfo::strmOut() << L"An exception occured:\n"
                            << errToCatch << kCIDLib::EndLn;
    }

    // Terminate the Orb support
    facCIDOrb().Terminate();

    return tCIDLib::EExitCodes::Normal;
}

