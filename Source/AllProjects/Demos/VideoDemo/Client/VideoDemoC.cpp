#include    "VideoDemoC.hpp"

// For this simple program, start us on a global function
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
    ,   tCIDLib::TVoid*     pData
);
CIDLib_MainModule(TThread(L"VideoDemoCMainThread", eMainThreadFunc))


static TInConsole   conIn;
static TOutConsole  conOut;


tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    conOut << L"Starting the client side ORB" << kCIDLib::EndLn;
    facCIDOrb().InitClient();

    // We need the object id of the server interface
    TOrbObjId ooidSrv;
    {
        conOut << L"Getting a name server client proxy..." << kCIDLib::EndLn;
        tCIDOrbUC::TNSrvProxy orbcNS = facCIDOrbUC().orbcNameSrvProxy();
        tCIDLib::TCard8 c8Cookie;
        orbcNS->bGetObject(TVideoDemoClientProxy::strBinding, ooidSrv, c8Cookie);
    }

    conOut << L"Interface found in name server, creating client proxy" << kCIDLib::EndLn;
    TVideoDemoClientProxy orbcTest(ooidSrv, TVideoDemoClientProxy::strBinding);

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
            strCmd.Cut(0, 9);
            TDemoRecord recFound;
            if (orbcTest.bQueryByName(strCmd, recFound))
                conOut << L"Found: " << recFound << kCIDLib::EndLn;
            else
                conOut << L"No record found for that name" << kCIDLib::EndLn;
        }
         else if (strCmd.bStartsWithI(L"findstatus "))
        {
            strCmd.Cut(0, 11);

tVideoDemoSh::EStatus eToFind = tVideoDemoSh::eXlatEStatus(strCmd);
TVector<TDemoRecord> colFound;
if (orbcTest.bQueryAllStatus(eToFind, colFound))
{
    TStreamIndentJan janIndent(&conOut, 8);
    conOut  << L"\nFound " << colFound.c4ElemCount() << L" records\n"
            << L"------------------------\n";
    colFound.ForEach
    (
        [&](const TDemoRecord& recCur)
        {
            conOut << recCur << kCIDLib::NewLn; return kCIDLib::True;
        }
    );
            }
             else
            {
                conOut << L"No records matched";
            }
            conOut << kCIDLib::EndLn;
        }
    }

    conOut << L"Terminating the ORB" << kCIDLib::EndLn;
    facCIDOrb().Terminate();

    return tCIDLib::EExitCodes::Normal;
}
