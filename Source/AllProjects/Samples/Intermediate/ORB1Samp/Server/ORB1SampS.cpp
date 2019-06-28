//
// FILE NAME: ORB1SampC.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/17/2019
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
//  This is the client side part of the ORB1 Sample program. See the readme
//  file in the top level directory of this sample for details, but basically
//  we connect to the server side and allow you to do some ORB based remote
//  calls.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#include    "ORB1SampS.hpp"


// ---------------------------------------------------------------------------
//  For this simple program, we just start our main thread on a global function below
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
    ,   tCIDLib::TVoid*     pData
);
CIDLib_MainModule(TThread(L"ORB1SampSMainThread", eMainThreadFunc))


// ---------------------------------------------------------------------------
// For user input and our output
// ---------------------------------------------------------------------------
TOutConsole conOut;
TInConsole  conIn;



// ---------------------------------------------------------------------------
//  The main thread is started on this function
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    try
    {
        // Though we are a server we are a client to the name server
        conOut << L"Starting client ORB" << kCIDLib::EndLn;
        facCIDOrb().InitClient();

        // And now as a client, listening on port 9999
        conOut << L"Initializing ORB for port 9999, max clients is 1" << kCIDLib::EndLn;
        facCIDOrb().InitServer(9999, 1);

        // Create an instance of our interface and register with the ORB
        conOut << L"Registering interface with the ORB" << kCIDLib::EndLn;
        TORB1SampIntfImpl* porbsImpl = new TORB1SampIntfImpl();
        facCIDOrb().RegisterObject(porbsImpl, tCIDLib::EAdoptOpts::Adopt);

        // Register it with the ORB's rebinder to make sure is stays bound
        conOut << L"Registering interfaces with Name Server" << kCIDLib::EndLn;
        facCIDOrbUC().RegRebindObj
        (
            porbsImpl->ooidThis()
            , TORB1SampServerBase::strBinding
            , L"ORB 1 Sample Server Interface"
        );

        // And now we start the rebinding, which makes our interface public
        facCIDOrbUC().StartRebinder();
    }

    catch(const TError& errToCatch)
    {
        conOut  << L"Error initializing ORB:\n"
                << errToCatch
                << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    try
    {
        // Wait for the user to hit a key. Else we are prcoessing client requests
        conOut << L"Hit an key to exit" << kCIDLib::EndLn;
        tCIDLib::EConKeys eKeyType;
        conIn.chReadChar(eKeyType, kCIDLib::c4MaxWait);

        // Stop the rebinder so it won't keep trying to rebind us
        conOut << L"Stopping the rebinder" << kCIDLib::EndLn;
        facCIDOrbUC().StopRebinder();

        // And nwo unbind our interface from the ORB
        conOut << L"Unbinding our interface" << kCIDLib::EndLn;
        try
        {
            tCIDOrbUC::TNSrvProxy orbcNS = facCIDOrbUC().orbcNameSrvProxy();
            orbcNS->RemoveBinding(TORB1SampServerBase::strBinding, kCIDLib::False);
        }

        catch(TError& errToCatch)
        {
            conOut  << L"Error unbinding interface:\n"
                    << errToCatch << kCIDLib::EndLn;
        }

        // And nwo we can shut down the ORB
        conOut << L"And terminating the ORB" << kCIDLib::EndLn;
        facCIDOrb().Terminate();
    }

    catch(TError& errToCatch)
    {
        conOut  <<  L"A CIDLib runtime error occured during processing. "
                <<  L"\nError: " << errToCatch.strErrText()
                << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    return tCIDLib::EExitCodes::Normal;
}
