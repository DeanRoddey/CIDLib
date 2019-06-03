//
// FILE NAME: ORB1SampC.cpp
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
//  This file implements the client side of the ORB1 sample program. See the readme
//  file in the top level directory of this sample for details.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// ---------------------------------------------------------------------------
//  Bring in our own header file which has all we need.
// ---------------------------------------------------------------------------
#include    "ORB1SampC.hpp"


// ---------------------------------------------------------------------------
// For this simple program, start us on a global function
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"ORB1SampCMainThread", eMainThreadFunc))


// ---------------------------------------------------------------------------
//  We need interactive console functionality so create in/out consoles.
// ---------------------------------------------------------------------------
static TInConsole   conIn;
static TOutConsole  conOut;


// ---------------------------------------------------------------------------
//  The main thread is tarted here
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    try
    {
        // We have to start up the client side ORB so we can talk to the server
        conOut << L"Starting the client side ORB" << kCIDLib::EndLn;
        facCIDOrb().InitClient();

        //
        //  We need the object id of the server interface, which is registered in
        //  the name server, so we need to get a client proxy and ask it for the
        //  binding for our object.
        //
        TOrbObjId ooidSrv;
        {
            conOut << L"Getting a name server client proxy..." << kCIDLib::EndLn;
            tCIDOrbUC::TNSrvProxy orbcNS = facCIDOrbUC().orbcNameSrvProxy();
            tCIDLib::TCard8 c8Cookie;
            orbcNS->bGetObject(TORB1SampClientProxy::strBinding, ooidSrv, c8Cookie);
        }

        // Now we can create a client proxy
        conOut << L"Interface found in name server, creating client proxy" << kCIDLib::EndLn;
        TORB1SampClientProxy orbcTest(ooidSrv, TORB1SampClientProxy::strBinding);

        // Get commands and process them until the user types exit
        TString strCmd;
        while (kCIDLib::True)
        {
            conOut << L"\n\n> " << kCIDLib::FlushIt;
            conIn >> strCmd;

            strCmd.StripWhitespace();
            if (strCmd.bCompareI(L"exit"))
                break;

            if (strCmd.bStartsWithI(L"findname "))
            {
                // Ask the server for a record with the indicated name
                strCmd.Cut(0, 9);
                TORB1SampRec recFound;
                if (orbcTest.bQueryByName(strCmd, recFound))
                    conOut << L"Found: " << recFound << kCIDLib::NewEndLn;
                else
                    conOut << L"No record found for that name" << kCIDLib::NewEndLn;
            }
            else if (strCmd.bStartsWithI(L"findstatus "))
            {
                //
                //  Ask the server for all records with the indicated status and format
                //  them out.
                //
                strCmd.Cut(0, 11);
                tORB1SampSh::EStatus eToFind = tORB1SampSh::eXlatEStatus(strCmd);
                TVector<TORB1SampRec> colFound;
                if (orbcTest.bQueryAllStatus(eToFind, colFound))
                {
                    TStreamIndentJan janIndent(&conOut, 8);
                    conOut  << L"\nFound " << colFound.c4ElemCount() << L" records\n"
                            << L"------------------------\n";

                    colFound.bForEach
                    (
                        [&](const TORB1SampRec& recCur)
                        {
                            conOut << recCur << kCIDLib::NewLn;
                            return kCIDLib::True;
                        }
                    );
                }
                else
                {
                    conOut << L"No records matched";
                }
                conOut << kCIDLib::NewEndLn;
            }
            else
            {
                conOut << L"Unknown command" << kCIDLib::NewEndLn;
            }
        }

        conOut << L"Terminating the ORB" << kCIDLib::EndLn;
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
