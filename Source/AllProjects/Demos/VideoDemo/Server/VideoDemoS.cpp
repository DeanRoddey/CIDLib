#include    "VideoDemoS.hpp"


//  For this simple program, we just start our main thread on a global function below
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
    ,   tCIDLib::TVoid*     pData
);
CIDLib_MainModule(TThread(L"VideoDemoSMainThread", eMainThreadFunc))


// For user input and our output
TOutConsole conOut;
TInConsole  conIn;


tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    try
    {
        conOut << L"Starting client ORB" << kCIDLib::EndLn;
        facCIDOrb().InitClient();

        conOut << L"Initializing ORB for port 9999, max clients is 1" << kCIDLib::EndLn;
        facCIDOrb().InitServer(9999, 1);

        conOut << L"Registering interface with the ORB" << kCIDLib::EndLn;
        TVideoDemoIntfImpl* porbsImpl = new TVideoDemoIntfImpl();
        facCIDOrb().RegisterObject(porbsImpl);

        conOut << L"Registering interfaces with Name Server" << kCIDLib::EndLn;
        facCIDOrbUC().RegRebindObj
        (
            porbsImpl->ooidThis()
            , TVideoDemoServerBase::strBinding
            , L"Video Demo Server Interface"
        );
        facCIDOrbUC().StartRebinder();
    }

    catch(const TError& errToCatch)
    {
        conOut  << L"Error initializing ORB:\n"
                << errToCatch
                << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    conOut << L"Hit an key to exit" << kCIDLib::EndLn;
    tCIDLib::EConKeys eKeyType;
    conIn.chReadChar(eKeyType, kCIDLib::c4MaxWait);

    conOut << L"Stopping the rebinder" << kCIDLib::EndLn;
    facCIDOrbUC().StopRebinder();

    conOut << L"Unbinding our interface" << kCIDLib::EndLn;
    try
    {
        tCIDOrbUC::TNSrvProxy orbcNS = facCIDOrbUC().orbcNameSrvProxy();
        orbcNS->RemoveBinding(TVideoDemoServerBase::strBinding, kCIDLib::False);
    }

    catch(TError& errToCatch)
    {
        conOut  << L"Error unbinding interface:\n"
                << errToCatch << kCIDLib::EndLn;
    }

    conOut << L"And terminating the ORB" << kCIDLib::EndLn;
    facCIDOrb().Terminate();

    return tCIDLib::EExitCodes::Normal;
}
