//
// FILE NAME: CIDKernel_StackDump_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 01/18/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file provides the Linux versions of the stack dump functions that
//  are called to get a dump of the call sequence to a place where an error
//  has occured. The methods here are actually statics of TKrnlThread, but
//  they are done here because they are really standalone and very platform
//  specific.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"


namespace
{
    // ---------------------------------------------------------------------------
    //  Local static data
    //
    //  __fdDump
    //      This is the handle of the file being dumped to. Its set and then
    //      used by all of the APIs. The dump operations are protected so we
    //      don't have to worry about thread's stepping on each other's toes.
    //
    //  __mtx
    //      This is the critical section structure that we used to insure that
    //      only one thread enters the dump function at a time. Its initilized
    //      during DLL init.
    //
    //  __pszTargetDir
    //      The directory into which dump files are placed. This is settable via
    //      the CIDLIB_ERRDUMPDIR environment variable. If not set, then they
    //      will go into the current directory.
    // ---------------------------------------------------------------------------
    tCIDLib::TSInt       __fdDump;
    pthread_mutex_t      __mtx;
    const tCIDLib::TSCh* __pszTargetDir = 0;

    // ---------------------------------------------------------------------------
    //  Local functions
    // ---------------------------------------------------------------------------
    tCIDLib::TVoid __WriteStr(const tCIDLib::TSCh* const pszStr)
    {
        ::write(__fdDump, pszStr, ::strlen(pszStr));
    }

    tCIDLib::TVoid __WriteLine(const tCIDLib::TSCh* const pszStr)
    {
        ::write(__fdDump, pszStr, ::strlen(pszStr));
        tCIDLib::TSCh schNewLine = '\n';
        ::write(__fdDump, &schNewLine, 1);
    }

    tCIDLib::TVoid __FmtCard4(tCIDLib::TCard4         c4Val
                              , tCIDLib::TSCh* const  pszBuf
                              , tCIDLib::ERadices     eRadix)
    {
        static tCIDLib::TSCh chChars[] =
        {
              '0', '1', '2', '3', '4', '5', '6', '7',
              '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
        };

        tCIDLib::TBoolean   bLeading0 = kCIDLib::True;
        tCIDLib::TCard4     c4Divisor, c4Tmp;
        tCIDLib::TInt4      i4Ind, i4StrInd;

        //
        //  Put a 0 in the first byte just in case the value is zero.
        //  Otherwise, the code below would leave an empty string.
        //
        pszBuf[0] = '\000';

        if (eRadix == tCIDLib::ERadices::Hex)
        {
            c4Divisor   = 0x10000000;
            i4Ind       = 7;
        }
        else
        {
            //
            //  Divisor starts at the billions since largest number is
            //  around 4 billion.
            //
            c4Divisor   = 1000000000;
            i4Ind       = 9;
        }

        i4StrInd = 0;
        for (; i4Ind >= 0; i4Ind--)
        {
            c4Tmp = c4Val / c4Divisor;

            // If non-zero make sure that leading zero is cleared
            if (c4Tmp)
                bLeading0 = kCIDLib::False;

            if (!bLeading0 || (eRadix == tCIDLib::ERadices::Hex))
            {
                pszBuf[i4StrInd] = chChars[c4Tmp];
                i4StrInd++;
            }

            // Bump down the value by the modulus
            c4Val -= (c4Tmp * c4Divisor);

            if (eRadix == tCIDLib::ERadices::Hex)
                c4Divisor >>= 4;
             else
                c4Divisor /= 10;
        }

        if (i4StrInd)
            pszBuf[i4StrInd] = 0;
    }

    tCIDLib::TVoid __CreateFile()
    {
        //
        //  Build the name of the file that we are going to output to. Its the
        //  target directory, plus the process name and the process id. We
        //  build the name itself into a separate directory because, if we cannot
        //  create the file in the target dir, we create in the current directory.
        //
        tCIDLib::TSCh szFullName[kCIDLib::c4MaxPathLen + 1];

        szFullName[0] = 0;
        tCIDLib::TSCh* pszNameEnd = szFullName;
        if (__pszTargetDir)
        {
            ::strcpy(szFullName, __pszTargetDir);
            tCIDLib::TCard4 c4Len = ::strlen(szFullName);
            if (szFullName[c4Len - 1] != '\\')
            {
                ::strcat(szFullName, "\\");
                c4Len++;
            }
            pszNameEnd = szFullName + c4Len;
        }

        tCIDLib::TSCh szTmpBuf[kCIDLib::c4MaxPathLen + 1];
        TRawStr::pszConvert(TKrnlSysInfo::pszProcessName(), szTmpBuf, kCIDLib::c4MaxPathLen);
        ::sprintf(pszNameEnd, "%s_%i", szTmpBuf, ::getpid());
        TRawStr::pszConvert(kCIDLib::pszErrInfExt, szTmpBuf, kCIDLib::c4MaxPathLen);
        ::strcat(szFullName, szTmpBuf);

        //
        //  Ok, we are doing pretty well so far, so lets open or create
        //  the file and seek to the end in order to append beyond any
        //  current content.
        //
        tCIDLib::TCard4 c4Mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
        tCIDLib::TSInt iFlags = O_CREAT | O_APPEND | O_WRONLY;
        __fdDump = ::open(szFullName, iFlags, c4Mode);

        if (__fdDump == -1)
        {
            // If its not because the path is bad, then throw the error
            if (errno == EISDIR || errno == EACCES || errno == ENAMETOOLONG ||
                errno == ENOENT || errno == ENOTDIR || errno == EROFS)
            {
                // Just open the file in the current directory
                __fdDump = ::open(szTmpBuf, iFlags, c4Mode);
            }
        }
    }
}


// ---------------------------------------------------------------------------
//  Intra-facility functions
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDKrnlModule::__bInitTermStackDump(const tCIDLib::EInitTerm eInitTerm)
{
    if (eInitTerm == tCIDLib::EInitTerm::Initialize)
    {
        ::pthread_mutex_init(&__mtx, 0);

        //
        //  See if there is an environment variable for the target output
        //  directory for dump files.
        //
        tCIDLib::TSCh szTmpBuf[1024];
        TRawStr::pszConvert(kCIDKernel_::pszErrDumpDir, szTmpBuf, 1023);
        tCIDLib::TSCh* pszTmp = ::getenv(szTmpBuf);
        if (pszTmp)
        {
            __pszTargetDir = new tCIDLib::TSCh[::strlen(pszTmp) + 1];
            ::strcpy(const_cast<tCIDLib::TSCh*>(__pszTargetDir), pszTmp);
        }
    }
    else
    {
        ::pthread_mutex_destroy(&__mtx);
    }

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TKrnlThread: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TKrnlThread::DumpException( const   TKrnlThread&            kthrCaller
                            , const tCIDLib::TCh* const     pszThreadName
                            , const tCIDLib::TVoid* const   pExceptData)
{
    tCIDLib::TSCh szTmp[512];

    // Create the file that we dump to
    __CreateFile();
    if (__fdDump == -1)
        return;

    // Write the header for an exception
    __WriteLine("--------------------------------------");
    __WriteLine("Signal in Process: ");

    // Log out the common info that we do for all dumps
    __WriteStr("   Process: ");
    TRawStr::pszConvert(TKrnlSysInfo::pszProcessName(), szTmp, 511);
    __WriteLine(szTmp);
    __WriteStr("    Thread: ");
    TRawStr::pszConvert(pszThreadName, szTmp, 511);
    __WriteLine(szTmp);

    tCIDLib::TZStr64 szSigNum;

    szSigNum[0] = kCIDLib::chNull;
    TRawStr::bFormatVal(tCIDLib::TInt4(pExceptData), szSigNum, 64);

    TRawStr::pszConvert(szSigNum, szTmp, 511);
    __WriteStr(szTmp);
    __WriteStr(": ");
    __WriteLine(sys_siglist[tCIDLib::TInt4(pExceptData)]);

    ::close(__fdDump);
    __fdDump = -1;
}


tCIDLib::TVoid
TKrnlThread::DumpRuntimeError(  const   TKrnlThread&        kthrCaller
                                , const tCIDLib::TCh* const pszThreadName
                                , const tCIDLib::TCh* const pszFacility
                                , const tCIDLib::TCh* const pszError
                                , const tCIDLib::TCh* const pszAuxText
                                , const tCIDLib::TErrCode   errcId
                                , const tCIDLib::TErrCode   errcKrnlId
                                , const tCIDLib::TOSErrCode errcHostId
                                , const tCIDLib::TCh* const pszFile
                                , const tCIDLib::TCard4     c4LineNumber)
{
    tCIDLib::TSCh szTmp[512];

    // Insure only on thread tries to dump at once
    ::pthread_mutex_lock(&__mtx);

    // Create the file and store the handle locally for use
    __CreateFile();

    if (__fdDump == -1)
    {
        ::pthread_mutex_unlock(&__mtx);
        return;
    }

    // Write out the runtime error header
    __WriteLine("--------------------------------------");
    __WriteLine("Runtime Error: ");

    // Log the common stuff that's logged for all dumps
    __WriteStr("   Process: ");
    TRawStr::pszConvert(TKrnlSysInfo::pszProcessName(), szTmp, 511);
    __WriteLine(szTmp);
    __WriteStr("    Thread: ");
    TRawStr::pszConvert(pszThreadName, szTmp, 511);
    __WriteLine(szTmp);

    __WriteLine("\r\nError Information:");
    __WriteStr("        Facility: ");
    TRawStr::pszConvert(pszFacility, szTmp, 511);
    __WriteLine(szTmp);
    __WriteStr("           Error: ");
    TRawStr::pszConvert(pszError, szTmp, 511);
    __WriteLine(szTmp);

    if (pszAuxText)
    {
        __WriteStr("  Aux Error Text: ");
        TRawStr::pszConvert(pszAuxText, szTmp, 511);
        __WriteLine(szTmp);
    }

    __WriteStr("        Error Id: ");
    __FmtCard4(errcId, szTmp, tCIDLib::ERadices::Dec);
    __WriteLine(szTmp);
    __WriteStr(" Kernel Error Id: ");
    __FmtCard4(errcKrnlId, szTmp, tCIDLib::ERadices::Dec);
    __WriteLine(szTmp);
    __WriteStr("   Host Error Id: ");
    __FmtCard4(errcHostId, szTmp, tCIDLib::ERadices::Dec);
    __WriteLine(szTmp);
    __WriteStr("            File: ");
    TRawStr::pszConvert(pszFile, szTmp, 511);
    __WriteLine(szTmp);
    __WriteStr(".");
    __FmtCard4(c4LineNumber, szTmp, tCIDLib::ERadices::Dec);
    __WriteLine(szTmp);

    ::close(__fdDump);
    __fdDump = -1;

    ::pthread_mutex_unlock(&__mtx);
}
