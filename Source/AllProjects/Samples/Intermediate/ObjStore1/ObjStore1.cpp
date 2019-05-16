//
// FILE NAME: ObjStore1.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/16/2019
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
//  This is the main module for a sample program that demonstrates basic usage
//  of the CIDObjStore facility. CIDObjStore implements a simple database type
//  engine. I guess these days it would fall into the NoSQL category, being a
//  key/object pair based scheme.
//
//  It's definitely not for aggregating all of Google's search engine results,
//  it's for things like storing program configuration or system configuration
//  data basically. In CQC, the master server uses it to store user configuration
//  information, and the GUI clients use it to store last window position, last
//  opened directory, etc... type data.
//
//  So not for high volume work, but very useful for work-a-day storage of keyed
//  data.
//
//  The keys are a hierarchical path scheme, so things like:
//
//      /BubbasGames/Options/Current
//      /BubbasGames/Options/Previous
//      /LastPositions/UITab
//
//  That sort of thing. CIDObjStore is an embeddable engine, so any application can
//  make use of it. If the application is using its own private object store then
//  it doesn't have to worry about the keys at all in terms of possible clashes,
//  since only its keys will be in the store.
//
//  If the engine is embedded in a server, and providing storage for multiple
//  clients, then of course this becomes an issue. This can just be an obligation
//  on the clients, and it's not hard if they just use key paths that start with
//  some unique identifier. Or they can pass the server relative paths and the server
//  can partition them as desired by prefixing their keys with some client specific
//  value itself.
//
//  Any class that is streamable (implements MStreamable) can be stored in the object
//  store. The engine uses a little endian binary stream to flatten/resurrect
//  objects, so that provides for a canonical format. In a server based engine scenario
//  this would allow for sharing objects across platforms. You can read/write data
//  blobs, but it's on you to insure portability if that's important.
//
//  Objects are versioned and any update will bump the version. When you read an
//  object you get the version. You must also pass in the last version you got, and
//  it will just return nothing if you already have the latest version.
//
//  All operations on the database are synchronized. It's possible, if you do an
//  operation that forces an expansion of the database, that it might take a bit
//  longer than normal, and other operations on other threads will be held up
//  while this goes on.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes. We just need the object store which brings in CIDLib as well which
//  is all we need.
// ---------------------------------------------------------------------------
#include    "CIDObjStore.hpp"


// ---------------------------------------------------------------------------
//  Do the magic main module code
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"ObjStore1MainThread", eMainThreadFunc))



// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------

//
//  This is the the thread function for the main thread. It is started by
//  CIDLib_MainModule() above.
//
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    TTextOutStream& strmOut = TSysInfo::strmOut();
    try
    {
        //
        //  To avoid contaminating the source directories, we'll create our object
        //  store in the build output directory. Note the platform independent way
        //  of building paths.
        //
        const TString strDBName(L"ObjStore1Sample");
        const TString& strResDir = TProcEnvironment::strFind(L"CID_RESDIR");

        //
        //  Normally we'd just open an existing one if it was there. But for our
        //  test we want to be able to do a set of steps and not have to worry
        //  about any previously existing content. So I'm going to build up the
        //  path to the file and delete any previous one.
        //
        {
            TPathStr pathOld = strResDir;
            pathOld.AddLevel(strDBName);
            pathOld.AppendExt(L"CIDObjStore");
            if (TFileSys::bExists(pathOld))
                TFileSys::DeleteFile(pathOld);
        }

        // Now initialize and it should return true (created a new one)
        TCIDObjStore oseTest;
        if (!oseTest.bInitialize(strResDir, strDBName))
        {
            strmOut << L"Should have created a new object store but didn't"
                    << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::RuntimeError;
        }

        // Our test key we'll use
        const TString strKey_LastFrameArea(L"/LastPos/Main");

        //
        //  Save our last main window position. It's an area which is very, very
        //  unlikely to change size over time, so we don't ask it to reserve any
        //  extra space for this value. If it did change, the object might have to
        //  be moved within the store or the store expanded in a worst case.
        //
        //  The key should not exist yet, but just for demonstration purposes, we
        //  can check.
        //
        if (oseTest.bKeyExists(strKey_LastFrameArea))
        {
            strmOut << L"Main window position key should not exist but it does"
                    << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::RuntimeError;
        }

        //
        //  We know it doesn't exist so we can just unconditionally store it. If it
        //  existed, this would throw.
        //
        TArea areaMain(10, 10, 512, 480);
        oseTest.AddObject(strKey_LastFrameArea, areaMain);

        //
        //  Let's read it back. First we'll do the simple bReadObject(). This will
        //  return true if the object exists and we don't have the most recent version
        //  so we pass 0 (meaning we don't have a previous version.)
        //
        tCIDLib::TCard4 c4Version = 0;
        TArea areaCheck;
        if (!oseTest.bReadObject(strKey_LastFrameArea, c4Version, areaCheck))
        {
            strmOut << L"Area just written could not be read back" << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::RuntimeError;
        }

        //
        //  If we now do it again with the version we just got, we shouldn't get
        //  any new data.
        //
        if (oseTest.bReadObject(strKey_LastFrameArea, c4Version, areaCheck))
        {
            strmOut << L"Already had latest version but still got data"
                    << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::RuntimeError;
        }

        //
        //  The other option is eReadObject() which gives us full information.
        //  If we call it, still wtih the same version, we will be told it exists
        //  but there's no new data. This guy also has a trailing boolean option
        //  to throw if not found, but we won't use that here.
        //
        tCIDLib::ELoadRes eRes = oseTest.eReadObject
        (
            strKey_LastFrameArea, c4Version, areaCheck, kCIDLib::False
        );
        if (eRes != tCIDLib::ELoadRes::NoNewData)
        {
            strmOut << L"Should not have gotten new data but we did" << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::RuntimeError;
        }

        // If we zero the version back out, we should get new data
        c4Version = 0;
        eRes = oseTest.eReadObject
        (
            strKey_LastFrameArea, c4Version, areaCheck, kCIDLib::False
        );
        if (eRes != tCIDLib::ELoadRes::NewData)
        {
            strmOut << L"Should have gotten new data but we didn't" << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::RuntimeError;
        }

        //
        //  Normally, we don't know for sure if a key is there or not, so we use
        //  and add or update call. If returns true if it created it, else false if
        //  it just updated an existing key so that you can react to that if need be.
        //  The version in this case is output only and gives us back the version
        //  that this operation created.
        //
        areaMain.AdjustOrg(14, 24);
        if (oseTest.bAddOrUpdate(strKey_LastFrameArea, c4Version, areaMain))
        {
            strmOut << L"Object should have beenupdated, not created" << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::RuntimeError;
        }

        // Let's delete a key and then check for it to make sure it's gone
        oseTest.DeleteObject(strKey_LastFrameArea);
        if (oseTest.bKeyExists(strKey_LastFrameArea))
        {
            strmOut << L"Key still exists after deletion" << kCIDLib::EndLn;
            return tCIDLib::EExitCodes::RuntimeError;
        }
    }

    catch(const TError& errToCatch)
    {
        strmOut << L"An exception occurred:\n";
        TStreamIndentJan janIndent(&strmOut, 8);
        strmOut << errToCatch << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    return tCIDLib::EExitCodes::Normal;
}
