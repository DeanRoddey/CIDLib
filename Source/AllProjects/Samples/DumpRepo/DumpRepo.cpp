//
// FILE NAME: DumpRepo.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/14/2006
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
//  This is the main module for the program. This program takes the path to
//  a CIDObjStore repository file and dumps out information about the
//  contents.
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
#include  "CIDRegX.hpp"

#include  "CIDObjStore.hpp"



// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
    ,   tCIDLib::TVoid*     pData
);



// ---------------------------------------------------------------------------
//  Do the magic main module code
// ---------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"DumpRepo", eMainThreadFunc))


// ---------------------------------------------------------------------------
//  Global data
// ---------------------------------------------------------------------------
TTextInStream& strmIn = TSysInfo::strmIn();
TTextOutStream& strmOut = TSysInfo::strmOut();



// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------

//
//  Creates a new repository and for everything that matches the passed
//  regular expression, it will copy over to the new one. Everything else
//  is dropped.
//
static tCIDLib::TVoid CopyToNew(        TCIDObjStore&   oseDump
                                , const TString&        strRegX
                                , const TString&        strOrgPath
                                , const TString&        strOrgName)
{
    tCIDLib::TCard4     c4DataSz;
    tCIDLib::TCard4     c4Version;
    THeapBuf            mbufData;
    tCIDLib::TStrList   colKeys;

    // Set up the regular expression
    TRegEx regxCopy;
    regxCopy.SetExpression(strRegX);

    // Create a new repository that we can move everything over to
    TString strNewName(strOrgName);
    strNewName.Append(L"_2");
    TCIDObjStore oseNew;
    if (!oseNew.bInitialize(strOrgPath, strNewName))
    {
        // It should have created a new repository
        strmOut << L"ERROR: New repository was not created!\n" << kCIDLib::EndLn;
        return;
    }

    // Query all the keyes and loop through them
    TString strKey;
    TString strTmp;
    oseDump.QueryAllKeys(colKeys);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < colKeys.c4ElemCount(); c4Index++)
    {
        //
        //  Get the current key and see if it matches the regular expression.
        //  If so, copy to the new one.
        //
        strKey = colKeys[c4Index];
        if (regxCopy.bFullyMatches(strKey, kCIDLib::False))
        {
            // Read in the raw data for this key
            c4Version = 0;
            tCIDLib::ELoadRes eRes = oseDump.eReadObjectDirect
            (
                strKey, c4Version, mbufData, c4DataSz, kCIDLib::True
            );

            // Shouldn't happen, but just in case...
            if (eRes != tCIDLib::ELoadRes::NewData)
            {
                strmOut << L"No data for repo key '"
                        << strKey << L"'." << kCIDLib::EndLn;
                continue;
            }

            // Add it to the new repo
            oseNew.AddObjectDirect(strKey, mbufData, c4DataSz, c4DataSz);
        }
    }
}


// Called for the /Delete=xxx type command, to delete a single thing
static tCIDLib::TVoid
DeleteKey(TCIDObjStore& oseDump, const TString& strToRemove)
{
    strmOut << L"Delete item(s) '" << strToRemove << L"'? (Y/N): "
            << kCIDLib::FlushIt;

    TString strReply;
    strmIn >> strReply;
    strmOut << kCIDLib::EndLn;

    strReply.ToUpper();
    if (strReply != L"Y")
    {
        strmOut << L"No action being taken" << kCIDLib::EndLn;
        return;

    }

    // He wants to do it, so let's create the reg ex object
    TRegEx regxDel;
    regxDel.SetExpression(strToRemove);

    // Now we iterate all the keys and remove any that match
    TString strKey;
    TString strTmp;
    tCIDLib::TStrList colKeys;
    tCIDLib::TStrList colRemoved;
    oseDump.QueryAllKeys(colKeys);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < colKeys.c4ElemCount(); c4Index++)
    {
        strKey = colKeys[c4Index];
        if (regxDel.bFullyMatches(strKey, kCIDLib::False))
        {
            if (oseDump.bDeleteObjectIfExists(strKey))
                colRemoved.objAdd(strKey);
        }
    }

    strmOut << L"\nRemoved these keys\n";
    for (tCIDLib::TCard4 c4Index = 0; c4Index < colRemoved.c4ElemCount(); c4Index++)
        strmOut << L"   " << colRemoved[c4Index] << kCIDLib::NewLn;

    strmOut << kCIDLib::EndLn;
}



// Called for the /DeleteScope=xxx type command, to delete a scope
static tCIDLib::TVoid DeleteScope(TCIDObjStore& oseDump, const TString& strToRemove)
{
    strmOut << L"Delete path '" << strToRemove << L"'? (Y/N): "
            << kCIDLib::FlushIt;

    TString strReply;
    strmIn >> strReply;
    strmOut << kCIDLib::EndLn;

    strReply.ToUpper();
    if (strReply == L"Y")
        oseDump.DeleteScope(strToRemove);
    else
        strmOut << L"No action being taken";
    strmOut << kCIDLib::EndLn;
}


//
//  Removes spaces from any CQC driver moniker names, which was accidentally
//  allowed initially.
//
static tCIDLib::TVoid RemoveSpaces(         TCIDObjStore&   oseDump
                                    , const TString&        strOrgPath
                                    , const TString&        strOrgName)
{
    tCIDLib::TCard4     c4DataSz;
    tCIDLib::TCard4     c4Version;
    THeapBuf            mbufData;
    tCIDLib::TStrList   colKeys;

    // Create a new repository that we can move everything over to
    TString strNewName(strOrgName);
    strNewName.Append(L"_2");
    TCIDObjStore oseNew;
    if (!oseNew.bInitialize(strOrgPath, strNewName))
    {
        // It should have created a new repository
        strmOut << L"ERROR: New repository was not created!\n" << kCIDLib::EndLn;
        return;
    }

    TString strKey;
    TString strTmp;
    oseDump.QueryAllKeys(colKeys);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < colKeys.c4ElemCount(); c4Index++)
    {
        strKey = colKeys[c4Index];

        // Read in the raw data for this key
        c4Version = 0;
        tCIDLib::ELoadRes eRes = oseDump.eReadObjectDirect
        (
            strKey, c4Version, mbufData, c4DataSz, kCIDLib::True
        );

        // Shouldn't happen, but just in case...
        if (eRes != tCIDLib::ELoadRes::NewData)
        {
            strmOut << L"No data for repo key '"
                    << strKey << L"'." << kCIDLib::EndLn;
            continue;
        }

        static const tCIDLib::TCh* apszBasePaths[] =
        {
            L"/CQC/IRCfg/Blaster/"
            , L"/CQC/IRCfg/Receiver/"
            , L"/CQC/CQCServer/Drivers/CfgStrs/"
            , L"/Drivers/VarDriver/"
        };
        const tCIDLib::TCard4 c4Count = tCIDLib::c4ArrayElems(apszBasePaths);

        tCIDLib::TCard4 c4Index2 = 0;
        for (; c4Index2 < c4Count; c4Index2++)
        {
            if (strKey.bStartsWith(apszBasePaths[c4Index2]))
                break;
        }

        if (c4Index2 < c4Count)
        {
            const tCIDLib::TCard4 c4BaseLen = TRawStr::c4StrLen(apszBasePaths[c4Index2]);

            // Build up the new path
            strKey.CopyOutSubStr(strTmp, c4BaseLen);
            strKey.CapAt(c4BaseLen);
            strTmp.bReplaceChar(L' ', L'_');
            strKey.Append(strTmp);
        }

        // Add it to the new repo
        oseNew.AddObjectDirect(strKey, mbufData, c4DataSz, c4DataSz);
    }

    // Close the new repository and old one
    oseDump.Close();
    oseNew.Close();

    // And re-open the old repository on the new file
    if (oseDump.bInitialize(strOrgPath, strNewName))
    {
        // It shouldn't have created a new repository
        strmOut << L"ERROR: Newly created repository was created!\n" << kCIDLib::EndLn;
        return;
    }
}


static tCIDLib::TVoid Rename(TCIDObjStore& oseDump)
{
    // They want to rename a path
    TString strOld = TSysInfo::strCmdLineParmAt(1);
    TString strNew = TSysInfo::strCmdLineParmAt(2);
    TPathStr pathRepo;

    strmOut << L"Do you want rename:\n  Old:" << strOld
            << L"\n   New:" << strNew
            << L"\n? (Y/N): "
            << kCIDLib::FlushIt;

    TString strReply;
    strmIn >> strReply;
    strmOut << kCIDLib::EndLn;

    strReply.ToUpper();
    if (strReply == L"Y")
    {
        tCIDLib::TCard4     c4DataSz;
        tCIDLib::TCard4     c4Version;
        THeapBuf            mbufData;

        oseDump.eReadObjectDirect
        (
            strOld, c4Version, mbufData, c4DataSz, kCIDLib::True
        );
        oseDump.bDeleteObjectIfExists(strOld);
        oseDump.AddObjectDirect
        (
            strNew, mbufData, c4DataSz, tCIDLib::TCard4(c4DataSz * 1.10)
        );
    }
}



// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------

//
//  This is the the thread function for the main thread. It is started by
//  the CIDLib_MainModule() macro above.
//
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    try
    {
        //
        //  We have to have at least 2 params (the command and the repo.) After that we
        //  can have optional options.
        //
        const tCIDLib::TCard4 c4ArgCnt = TSysInfo::c4CmdLineParmCount();
        if (c4ArgCnt < 2)
        {
            strmOut << L"Usage:\n"
                       L"   DumpRepo [/Dump /Delete= | /DeleteScope= | /RemoveSpaces| /Rename | /Restore] repopath [/CaseSensitive]\n\n"
                       L"       /Delete=xxx         where xxx can be a regular expression or literal value\n"
                       L"       /DeleteScope=xxx    where xxx can be a regular expression or literal value\n"
                       L"\n"
                       L"   DumpRepo /Copy=[regex] orgrepo newrepo\n"
                       L"       Creates a new repo and copies all keys that match the regular\n"
                       L"       expression to the new one\n"
                    << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::BadParameters;
        }

        // Get the command string out.
        TString strCmd = TSysInfo::strCmdLineParmAt(0);

        // Get the repo path out and prep it
        TPathStr pathRepo = TSysInfo::strCmdLineParmAt(1);
        if (!TFileSys::bExists(pathRepo, tCIDLib::EDirSearchFlags::NormalFiles))
        {
            strmOut << L"Input repository not found\n" << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::BadParameters;
        }

        // Split the path/name parts out
        TPathStr pathName;
        if (!pathRepo.bExtractNameExt(pathName))
        {
            strmOut << L"Invalid repository path\n" << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::BadParameters;
        }

        // Toss the extension from the name
        pathName.bRemoveExt();

        // If more parameters, get those
        tCIDObjStore::EFlags eFlags = tCIDObjStore::EFlags::None;
        TString strOpt;
        for (tCIDLib::TCard4 c4Index = 2; c4Index < c4ArgCnt; c4Index++)
        {
            strOpt = TSysInfo::strCmdLineParmAt(c4Index);
            if (strOpt.bCompareI(L"/CaseSensitive"))
            {
                eFlags = tCIDObjStore::EFlags::CaseSensitive;
            }
             else
            {
                strmOut << L"Unknown option parameter: " << strOpt << kCIDLib::EndLn;
                return  tCIDLib::EExitCodes::BadParameters;
            }
        }

        // Looks to be there, so open the repository
        TCIDObjStore oseDump(eFlags);
        if (oseDump.bInitialize(pathRepo, pathName, strCmd.bCompareI(L"/Recover")))
        {
            // It shouldn't have created a new repository
            strmOut << L"ERROR: New repository was created!\n" << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::RuntimeError;
        }

        if (strCmd.bStartsWithI(L"/Copy="))
        {
            //
            //  This will create a new repository file, and copy over any
            //  entries that match the passed regular expression.
            //
            strCmd.Cut(0, 6);
            CopyToNew(oseDump, strCmd, pathRepo, pathName);
        }
         else if (strCmd.bStartsWithI(L"/Delete="))
        {
            strCmd.Cut(0, 8);
            DeleteKey(oseDump, strCmd);
        }
         else if (strCmd.bStartsWithI(L"/DeleteScope="))
        {
            strCmd.Cut(0, 13);
            DeleteScope(oseDump, strCmd);
        }
         else if (strCmd.bCompareI(L"/Dump"))
        {
            // We'll do this below, since we always dump it
        }
         else if (strCmd.bCompareI(L"/Recover"))
        {
            // Nothign to do here, we'll dump after the validation
        }
         else if (strCmd.bCompareI(L"/RemoveSpaces"))
        {
            RemoveSpaces(oseDump, pathRepo, pathName);
        }
         else if (strCmd.bCompareI(L"/Rename"))
        {
            Rename(oseDump);
        }
         else if (!strCmd.bIsEmpty())
        {
            strmOut << L"Unknown command: '" << strCmd << L"', ignoring it"
                    << kCIDLib::EndLn;
        }

        // If recovering, ask for a backup, else do a validation
        if (strCmd.bCompareI(L"/Recover"))
        {
            oseDump.BackupStore();
        }
         else
        {
            oseDump.ValidateStore(strmOut);
        }

        // Close the repository
        oseDump.Close();
    }

    // Catch any CIDLib runtime errors
    catch(const TError& errToCatch)
    {
        strmOut << L"A CIDLib runtime error occured.\nError: "
                << errToCatch << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but test for
    //  them here just in case.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        strmOut << L"A kernel error occured.\nError: "
                << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        strmOut << L"A general exception occured" << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }
    return tCIDLib::EExitCodes::Normal;
}



