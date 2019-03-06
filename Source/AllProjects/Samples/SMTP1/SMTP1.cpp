//
// FILE NAME: SMTP1.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/27/2000
//
// COPYRIGHT: $_CIDLib_CopyRight_$
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
//  This is the main (only) module of the Sock3 demo program. This program demonstrates
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
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes. This program is so simple that we don't even have a header of
//  our own. So just include CIDNet, which will bring in all we need.
// ----------------------------------------------------------------------------
#include    "CIDNet.hpp"



// ----------------------------------------------------------------------------
//  Forward references
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
    TThread&            thrThis
    , tCIDLib::TVoid*   pData
);



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
//  Do the magic main module code
//
//  This tells CIDLib what the main thread of the program is. This is the
//  only thread object that is run automatically. All others are started
//  manually when required or desired.
// ----------------------------------------------------------------------------
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

    //
    //  Since this is a demo and partly a testing program, we'd like to
    //  catch all exceptions cleanly and report them. So put the whole thing
    //  in a try.
    //
    try
    {
        // Output a little program blurb
        conOut  << L"\nSock3.Exe\n"
                << L"CIDLib Socket Demo #3\n" << kCIDLib::EndLn;

        //
        //  The caller must provide us with the server name, so we have to at
        //  least have one parameter. They can optionally give us a port to
        //  use.
        //
        tCIDLib::TBoolean   bSecure = kCIDLib::False;
        tCIDLib::TIPPortNum ippnSMTP = kCIDNet::ippnSMTP;
        TString             strPassword;
        TString             strServer;
        TString             strUserName;

        TSysInfo::TCmdLineCursor cursParms = TSysInfo::cursCmdLineParms();
        for (; cursParms; ++cursParms)
        {
            TString strParm = *cursParms;

            if (strParm.bCompareI(L"/Secure"))
            {
                bSecure = kCIDLib::True;
            }
             else if (strParm.bStartsWithI(L"/Port="))
            {
                strParm.Cut(0, 6);
                try
                {
                    ippnSMTP = strParm.c4Val();
                }

                catch(...)
                {
                    conOut << L"The parameter value is invalid\n\n";
                    ShowUsage();
                    return tCIDLib::EExitCodes::BadParameters;
                }
            }
             else if (strParm.bStartsWithI(L"/Name="))
            {
                strParm.Cut(0, 6);
                strUserName = strParm;
            }
             else if (strParm.bStartsWithI(L"/Password="))
            {
                strParm.Cut(0, 10);
                strPassword = strParm;
            }
             else if (strParm.bStartsWithI(L"/Server="))
            {
                strParm.Cut(0, 8);
                strServer = strParm;
            }
             else
            {
                ShowUsage();
                return tCIDLib::EExitCodes::BadParameters;
            }
        }

        // Make sure we have what we need
        if (strServer.bIsEmpty()
        ||  (!strUserName.bIsEmpty() && strPassword.bIsEmpty()))
        {
            ShowUsage();
            return tCIDLib::EExitCodes::BadParameters;
        }

        //
        //  Create an email message object that we can fill in and send. To
        //  send messages, you add TEmailMsg objects to the output queue of
        //  a TSMTPClient object, then send them.
        //
        TEmailMsg emsgToSend;

        //
        //  Get the from field. This is what you'd think, it shows up as the
        //  person that the message is from. We loop until they enter a non
        //  empty value.
        //
        TString strInput;
        while (kCIDLib::True)
        {
            conOut << L"Enter 'From' Address: " << kCIDLib::FlushIt;
            if (conIn.c4GetLine(strInput))
            {
                strInput.StripWhitespace();
                if (!strInput.bIsEmpty())
                    break;
            }
        }
        emsgToSend.strFrom(strInput);

        //
        //  Ok, now get the to list. This can be more than one address, so
        //  we tell them to enter an empty string when done.
        //
        conOut << L"Enter 'To' Addresses (empty line to finish): "
               << kCIDLib::FlushIt;
        while (kCIDLib::True)
        {
            if (conIn.c4GetLine(strInput))
            {
                strInput.StripWhitespace();
                if (strInput.bIsEmpty())
                    break;

                emsgToSend.AddToAddr(strInput);
            }
             else
            {
                break;
            }
            conOut << L"  To: " << kCIDLib::FlushIt;
        }

        conOut << L" Topic: " << kCIDLib::FlushIt;
        conIn.c4GetLine(strInput);
        emsgToSend.strTopic(strInput);

        conOut  << L"  Enter Msg (End with a '.' on a line by itself):\n"
                << kCIDLib::FlushIt;
        TString strMsg;
        while (kCIDLib::True)
        {
            conIn.c4GetLine(strInput);
            if (strInput == L".")
                break;

            strMsg.Append(strInput);
            strMsg.Append(L"\n");
        }
        emsgToSend.strMsg(strMsg);

        //
        //  Create an SMTP client object. By default its going to get our
        //  own workstation name, and pass that as the source domain. We have
        //  no need to override this.
        //
        TSMTPClient smtpClient;

        // Set up the connection info
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


//emsgToSend.AddFileAttachment
//(
//    L"image/png", L"C:\\Tmp\\NewDigVU.png"
//);

        //
        //  Add some copies of the message to the client to be sent.
        //
//        smtpClient.AddMsgToQueue(new TEmailMsg(emsgToSend));
        smtpClient.AddMsgToQueue(new TEmailMsg(emsgToSend));

        // And finally we can try to send it. Set a 10 second per message timeout
        smtpClient.SendMsgs(10000);

        // The queue should be empty now
        if (smtpClient.c4MsgsInQueue())
            conOut << L"Messages were left in the queue" << kCIDLib::EndLn;
    }

    // Catch any CIDLib runtime errors
    catch(TError& errToCatch)
    {
        // If this hasn't been logged already, then log it
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        conOut  << L"A CIDLib runtime error occured during processing.\n  Error: "
                << errToCatch.strErrText() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but I test
    //  for them in my test programs so I can catch them if they do
    //  and fix them.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        conOut  << L"A kernel error occured during processing.\nError="
                << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        conOut  << L"A general exception occured during processing"
                << kCIDLib::DNewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }

    return tCIDLib::EExitCodes::Normal;
}
