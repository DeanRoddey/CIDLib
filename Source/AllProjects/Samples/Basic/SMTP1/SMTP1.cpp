//
// FILE NAME: SMTP1.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/27/2000
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
//  This is the main (only) module of the SMTP1 demo program. This program demonstrates
//  some slightly more complex uses of sockets. It provides a very simple SMTP client
//  program. It will query you for the information required to send a single line e-mail
//  message, then it will send the message for you.
//
//  You will asked if you want to send secure or insecure. If secure, a TLS type
//  connection will be made to the server. Be sure to provide the appropriate port
//  for the type of connection you are doing.
//
//  It handles the standard SMTP authorization handshake.
//
//  Like most of the basic samples, this one doesn't create a facility object, it
//  just starts a main thread on a local
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "CIDNet.hpp"



// ----------------------------------------------------------------------------
//  Local data
//
//  conIn
//  conOut
//      This is an interactive program, so we create an input and output
//      console to do our interaction with the user. We create the input
//      console so that it supports recall and editing, with a recall buffer
//      of 16 elements.
// ----------------------------------------------------------------------------
static TInConsole   conIn(kCIDLib::True, 16);
static TOutConsole  conOut;


// ----------------------------------------------------------------------------
//  Do the magic main module code to start the main thread
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"SMTP1MainThread", eMainThreadFunc))



// ----------------------------------------------------------------------------
//  Local helper methods
// ----------------------------------------------------------------------------
static void ShowUsage()
{
    conOut << L"Usage:\n  SMTP1 /Server=s /Port=p /Name=n /Password=pw\n\n"
              L"          /Server = The SMTP server name or address\n"
              L"          /Port = Port number. 25 is used if not provided\n"
              L"          /Name = User name to authenticate with\n"
              L"          /Password = Password to authenticate with\n"
              L"          /Secure = No value. Indicates a TLS type connection\n\n"
              L"          If no authentication desired, don't provide name/password.\n"
              L"          If name is provided, password must be also\n"
           << kCIDLib::EndLn;
}


// ----------------------------------------------------------------------------
//  Program entry point
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    // Output a little program blurb
    conOut  << L"\nSMTP.Exe\n"
            << L"CIDLib SMTP Sample #1\n" << kCIDLib::EndLn;

    // Parse and validate the parameters
    tCIDLib::TBoolean   bSecure;
    tCIDLib::TIPPortNum ippnSMTP;
    TString             strPassword;
    TString             strServer;
    TString             strUserName;
    {
        // Tell it to remove consumed parms so that we can check for unknown ones
        TCmdLine cmdlLoad;
        cmdlLoad.bRemoveConsumed(kCIDLib::True);

        //
        //  We must get server and port must be a valid number (taking default if not
        //  provided.) In case one was provided, make sure it's reasonably reasonable.
        //
        if (!cmdlLoad.bFindOptionVal(L"Server", strServer)
        ||  !cmdlLoad.bFindOptionVal(L"Port", ippnSMTP, kCIDNet::ippnSMTP)
        ||  !ippnSMTP
        ||  (ippnSMTP > kCIDLib::c2MaxCard))
        {
            ShowUsage();
            return tCIDLib::EExitCodes::BadParameters;
        }

        // These are optional, don't care if they are found or not
        cmdlLoad.bFindOptionVal(L"Name", strUserName);
        cmdlLoad.bFindOptionVal(L"Password", strPassword);

        bSecure = cmdlLoad.bFindOption(L"Secure");

        // If any left, they are unknown ones
        if (!cmdlLoad.bIsEmpty())
        {
            ShowUsage();
            return tCIDLib::EExitCodes::BadParameters;
        }
    }

    // If we have a unser name, we need a password
    if (!strUserName.bIsEmpty() && strPassword.bIsEmpty())
    {
        ShowUsage();
        return tCIDLib::EExitCodes::BadParameters;
    }

    //
    //  Get the from field. This is what you'd think, it shows up as the
    //  person that the message is from.
    //
    conOut << L"Enter 'From' Address: " << kCIDLib::FlushIt;
    TString strFromAddr;
    if (!conIn.c4GetLine(strFromAddr))
        return tCIDLib::EExitCodes::Normal;
    strFromAddr.StripWhitespace();

    //
    //  Ok, now get the to address. This can be more than one, but we are
    //  just doing a simple example here and just get one.
    //
    conOut << L"Enter 'To' Address: " << kCIDLib::FlushIt;
    TString strToAddr;
    if (!conIn.c4GetLine(strToAddr))
        return tCIDLib::EExitCodes::Normal;
    strToAddr.StripWhitespace();

    conOut << L" Topic: " << kCIDLib::FlushIt;
    TString strTopic;
    if (!conIn.c4GetLine(strTopic))
        return tCIDLib::EExitCodes::Normal;

    // Get the messages which is multi-line
    conOut  << L"  Enter Msg (End with a '.' on a line by itself):\n"
            << kCIDLib::FlushIt;
    TString strMsg;
    while (kCIDLib::True)
    {
        TString strCurLn;
        conIn.c4GetLine(strCurLn);
        if (strCurLn == L".")
            break;

        strMsg.Append(strCurLn);
        strMsg.Append(L"\n");
    }

    //
    //  Create an SMTP client object and set up the connection info. We
    //  get our host name and pass it as the source domain.
    //
    TSMTPClient smtpClient;
    smtpClient.SetConnInfo
    (
        facCIDSock().strIPHostName()
        , strServer
        , ippnSMTP
        , bSecure
        , strUserName
        , strPassword
    );

    //
    //  If a username/password was provided, then set that and the
    //  authentication type.
    //
    if (!strUserName.bIsEmpty())
        smtpClient.eAuthType(tCIDNet::EMailAuthTypes::SMTP);

    //
    //  We pass in the values we collected. For this common scenario it will
    //  just do an emplace of the values into the queue as an e-mail message
    //  object.
    //
    //  Set a 10 second per message timeout. Of course in our case we
    //  just have one msg.
    //
    smtpClient.AddMsgToQueue(strFromAddr, strToAddr, strTopic, strMsg);
    smtpClient.SendMsgs(10000);

    // The queue should be empty now
    if (smtpClient.c4MsgsInQueue())
        conOut << L"Messages were left in the queue" << kCIDLib::EndLn;

    return tCIDLib::EExitCodes::Normal;
}
