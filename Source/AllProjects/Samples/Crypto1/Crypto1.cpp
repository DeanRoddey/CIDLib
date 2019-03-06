//
// FILE NAME: Crypto1.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/27/1998
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
//  This is the main module for the first of the cryptography oriented demo
//  programs. This one implements a simple file encrypter/decrypter using
//  the CIDLib supported block encryption algorithms. The user provides a
//  key phrase, and input file and and output file and a flag that says
//  whether to encrypt or decrypt the input file.
//
//  We take a key as a command line parameter. The default if we provided
//  that as text as a key is that it will be converted to UTF-8, with no
//  trailing terminator. That will then be used as the actual key. UTF-8 is
//  not byte order dependent.
//
// CAVEATS/GOTCHAS:
//
//  1)  This one does not make any attempt at internationalization. It just
//      has hard coded text.
//
//  2)  Since the encryption mechanism can only encrypt in multiples of its
//      block size (i.e. the number of bytes in its basic encryption block),
//      we do two things. First we make sure that our buffer that we read
//      into and write out of are an equal multiple of the block size. And
//      we write the original file size to the encrypted file and use that
//      to truncate the decrypted file back to its original size (to get
//      rid of the padding that occurs if the original file contents are
//      not an even multiple size of the encryption block size.
//
//      If we did not do the first, then we would get bytes of zeros into
//      the decrypted file at the end of each decrypted block. If we did
//      not do the latter, we could get some extra zero bytes at the end
//      of the decrypted file.
//
//  3)  The output file is overwritten, so be careful.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes. This program is so simple that we don't even have a header of
//  our own. So just include CIDCrypto, which is all we need. It brings in
//  CIDLib.Hpp.
// ---------------------------------------------------------------------------
#include    "CIDCrypto.hpp"


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
        , tCIDLib::TVoid*   pData
);

static tCIDLib::TVoid ShowUsage();



// ---------------------------------------------------------------------------
//  Local, static data
//
//  conOut
//      This is a console object which we use in this program for our standard
//      output. Its a specialized text stream class.
// ---------------------------------------------------------------------------
static  TOutConsole conOut;



// ---------------------------------------------------------------------------
//  Do the magic main module code
// ---------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"Crypto1MainThread", eMainThreadFunc))



// ---------------------------------------------------------------------------
//  Local methods
// ---------------------------------------------------------------------------

//
//  This is the the thread function for the main thread.
//
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    //
    //  Display a little blurb to say what we are and our version.
    //
    conOut    << kCIDLib::NewLn << L"Crypto1.Exe\nCompiled: "
                << TString(__DATE__) << kCIDLib::NewLn << kCIDLib::EndLn;

    //
    //  Ok, now we do our real work. Lets set up a try statement so that
    //  we can catch any errors that might occur and display a message.
    //
    tCIDLib::TBoolean   bEncrypt;
    tCIDLib::TBoolean   bOutputFileCreated = kCIDLib::False;
    TBinFileOutStream   strmOutput;
    tCIDLib::EExitCodes eExitCode = tCIDLib::EExitCodes::Normal;
    TString             strPhase;
    try
    {
        // Create a local string for temp command line work
        TString strTmp(512UL);

        //
        //  See if there are enough parameters. There must be 4, the encrypt/decrypt
        //  flag, the key, the input file, and the output file.
        //
        strPhase = L"testing command line parms";
        if (TSysInfo::c4CmdLineParmCount() != 4)
        {
            ShowUsage();
            return tCIDLib::EExitCodes::BadParameters;
        }

        //
        //  Get the first parameter and make sure its either /Encrypt or
        //  /Decrypt, though the case does not matter so do it case
        //  insensitively.
        //
        strTmp = TSysInfo::strCmdLineParmAt(0);
        if (strTmp.bCompareI(L"/Encrypt"))
        {
            bEncrypt = kCIDLib::True;
        }
         else if (strTmp.bCompareI(L"/Decrypt"))
        {
            bEncrypt = kCIDLib::False;
        }
         else
        {
            ShowUsage();
            return tCIDLib::EExitCodes::BadParameters;
        }

        //
        //  Get the next parameter which is the key that is to be used
        //  for the operation. There is not much that we can check about it
        //  so just use it to create a crypto key object.
        //
        strTmp = TSysInfo::strCmdLineParmAt(1);
        TCryptoKey ckeyWork(strTmp);

        //
        //  Get the input file and try to open it for read only access. We
        //  use a binary file stream for our reading.
        //
        strPhase = L"opening input file";
        strTmp = TSysInfo::strCmdLineParmAt(2);
        TBinFileInStream strmInput;
        strmInput.Open
        (
            strTmp
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::None
            , tCIDLib::EFileFlags::SequentialScan
        );

        //
        //  And now do the output file, telling it to create a new file or
        //  truncate an existing one. We also use a binary file stream
        //  here for our writing.
        //
        strPhase = L"creating output file";
        strTmp = TSysInfo::strCmdLineParmAt(3);
        strmOutput.Open
        (
            strTmp
            , tCIDLib::ECreateActs::CreateAlways
            , tCIDLib::EFilePerms::AllAccess
            , tCIDLib::EFileFlags::SequentialScan
        );

        //
        //  It worked so indicate that the output file is created. This
        //  lets us know below that we need to delete it if the operation
        //  fails.
        //
        bOutputFileCreated = kCIDLib::True;

        //
        //  Create a Blowfish encrypter object to do our work. Pass it
        //  it a crypto key object built from the input key string.
        //
        TBlowfishEncrypter crypWork(ckeyWork);

        //
        //  Since we have to encrypt to an even multiple of the block size
        //  of the encryption algorithm, we need to write out the original
        //  size of the file so that we can truncate the file back to that
        //  on decryption.
        //
        //  In order to provide a sanity check, we then NOT the file size
        //  and write it out again. On read we check these two in order to
        //  know that we have not gotten some random file.
        //
        tCIDLib::TCard8 c8OriginalSize = strmInput.c8CurSize();

        if (bEncrypt)
        {
            strmOutput  << c8OriginalSize << ~c8OriginalSize << kCIDLib::FlushIt;
        }
         else
        {
            strmInput >> c8OriginalSize;

            tCIDLib::TCard8 c8Test;
            strmInput >> c8Test;

            //
            //  If the not of the first value does not equal the second
            //  value, then its probably not a file that we encrypted.
            //
            if (c8Test != ~c8OriginalSize)
            {
                conOut << L"This file does not appear to have been encrypted "
                          L"by Crypto1. Giving up..." << kCIDLib::EndLn;
                return tCIDLib::EExitCodes::RuntimeError;
            }
        }

        //
        //  Alrighty then. We are ready to do the real work. We just need
        //  read in blocks of the input file, do the operation on them,
        //  then write them to the output file.
        //
        //  We happen to know that Blowfish creates an equal number of
        //  bytes for output as are put in, so our output buffer can be
        //  the same size as the input buffer.
        //
        const tCIDLib::TCard4 c4BufSize = crypWork.c4BlockSize() * 1024;
        THeapBuf mbufInput(c4BufSize);
        THeapBuf mbufOutput(c4BufSize);

        strPhase = L"converting file";
        while (1)
        {
            //
            //  Read up to the input buffer size from the file. If we get
            //  zero bytes, then we are done. Tell it not to throw an
            //  exception if we don't read all the requested bytes, because
            //  that is likely to happen on the last buffer read in.
            //
            const tCIDLib::TCard4 c4ActualRead = strmInput.c4ReadBuffer
            (
                mbufInput, c4BufSize, tCIDLib::EAllData::OkIfNotAll
            );

            //
            //  If we read in zero bytes, then are done so break out
            //  of the loop.
            //
            if (!c4ActualRead)
                break;

            //
            //  Convert the input buffer to the output buffer according to
            //  the requested operation. Indcate that we want to do the
            //  number of bytes that we actually read and we get back the
            //  bytes converted (which will be a different value if the
            //  bytes being converted is not an equal multiple of the block
            //  size, which can happen on the last block.)
            //
            tCIDLib::TCard4 c4Converted;
            if (bEncrypt)
                c4Converted = crypWork.c4Encrypt(mbufInput, mbufOutput, c4ActualRead);
            else
                c4Converted = crypWork.c4Decrypt(mbufInput, mbufOutput, c4ActualRead);

            //
            //  And write it out to the output file. In this case we do
            //  let it throw an exception if not all bytes are written.
            //
            strmOutput.c4WriteBuffer(mbufOutput, c4Converted);
        }

        // Flush out any trailing cached data
        strmOutput.Flush();

        //
        //  If we are decrypting, then we need to truncate back to the
        //  original file size.
        //
        if (!bEncrypt)
            strmOutput.TruncateAt(c8OriginalSize);
    }

    catch(TError& errToCatch)
    {
        // If this hasn't been logged already, then log it
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        conOut << L"An error occured while "
                 << strPhase << L"\n"
                 << L"Error: " << errToCatch.strErrText()
                 << kCIDLib::NewLn << kCIDLib::EndLn;
        eExitCode = tCIDLib::EExitCodes::RuntimeError;
    }

    catch(const TKrnlError& kerrToCatch)
    {
        conOut << L"A kernel error occured while "
                 << strPhase << L"\n"
                 << L"Error/Host Error: " << kerrToCatch.errcId()
                 << L"/" << kerrToCatch.errcHostId() << L"\n"
                 << L"Please report this error!! Kernel errors should not ever be seen by you\n"
                 << kCIDLib::EndLn;
        eExitCode = tCIDLib::EExitCodes::RuntimeError;
    }

    catch(...)
    {
        conOut << L"A system error occured while "
                 << strPhase << kCIDLib::NewLn << kCIDLib::EndLn;
        eExitCode = tCIDLib::EExitCodes::FatalError;
    }

    // Close the input and output files
    strmOutput.Close();

    //
    //  If we failed and the output file got created, then we need
    //  to remove it. If it went well, then announce it.
    //
    if (eExitCode != tCIDLib::EExitCodes::Normal)
    {
        if (bOutputFileCreated && TFileSys::bExists(strmOutput.strFileName()))
            TFileSys::DeleteFile(strmOutput.strFileName());
    }
     else
    {
        conOut << L"The operation succeeded\n" << kCIDLib::EndLn;
    }
    return eExitCode;
}


//
//  This is called if any of the parameters are not right. It will display
//  the correct parameter usage.
//
static tCIDLib::TVoid ShowUsage()
{
    conOut << L"Invalid parameters! Correct usage:\n"
             << L"  Crypto1 /flag key infile outfile\n"
             << L"  /flag   - /Decrypt or /Encrypt\n"
             << L"  key     - The key to use. In quotes if spaces are used\n"
             << L"  infile  - The input file to en(de)crypt\n"
             << L"  outfile - The output file that holds the results\n"
             << kCIDLib::EndLn;
}

