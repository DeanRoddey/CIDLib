//
// FILE NAME: TestNet_JSON.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/31/2012
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
//  This file does some test of the JSON parser
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Include underlying headers
// ---------------------------------------------------------------------------
#include    "TestNet.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTest_JSON1,TTestFWTest)
RTTIDecls(TTest_JSON2,TTestFWTest)
RTTIDecls(TTest_JSON3,TTestFWTest)
RTTIDecls(TTest_JSON4,TTestFWTest)
RTTIDecls(TTest_JSON5,TTestFWTest)



// ---------------------------------------------------------------------------
//  CLASS: TTest_JSON1
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_JSON1: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_JSON1::TTest_JSON1() :

    TTestFWTest
    (
        L"JSON 1", L"Basic JSON parser test 1", 3
    )
{
}

TTest_JSON1::~TTest_JSON1()
{
}


// ---------------------------------------------------------------------------
//  TTest_JSON1: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_JSON1::eRunTest(  TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;


    // A very basic embedded file test
    try
    {
        const TString strTestData
        (
            L"{\n"
            L"   \"Items\" : [\n"
            L"       { \"Value\" : \"0\" }\n"
            L"   ]\n"
            L"}\n"
        );

        TJSONParser jprsTest;
        TTextStringInStream strmSrc(&strTestData);

        TJSONValue* pjprsnRootVal = jprsTest.pjprsnParse(strmSrc);
        TJanitor<TJSONValue> janRoot(pjprsnRootVal);

        // Make sure the root is an object and cast
        if (pjprsnRootVal->eType() != tCIDNet::EJSONVTypes::Object)
        {
            strmOut << L"Root is not an object" << kCIDLib::DNewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }
        TJSONObject* pjprsnRoot = static_cast<TJSONObject*>(pjprsnRootVal);

        // Do a manual test to see if the 0th value is an array
        if (pjprsnRoot->eTypeAt(0) != tCIDNet::EJSONVTypes::Array)
        {
            strmOut << L"Expected array element under root" << kCIDLib::DNewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }
         else if (pjprsnRoot->strNameAt(0) != L"Items")
        {
            strmOut << L"Expected array to be named Items" << kCIDLib::DNewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }
         else
        {
            const TJSONArray& jprsnItems = *pjprsnRoot->pjprsnArrayAt(0);
            if (jprsnItems.c4ValCount() == 1)
            {
                // These 0th element should be an object
                const TJSONObject& jprsnElem1 = *jprsnItems.pjprsnObjAt(0);

                // It's name should be empty since it's an array element
                if (jprsnElem1.strName() != TString::strEmpty())
                {
                    strmOut << L"Array members should have no name'" << kCIDLib::DNewLn;
                    eRes = tTestFWLib::ETestRes::Failed;
                }
                 else if (jprsnElem1.strNameAt(0) != L"Value")
                {
                    // The 0th value in the object should be the Value value
                    strmOut << L"Expected value named 'Value'" << kCIDLib::DNewLn;
                    eRes = tTestFWLib::ETestRes::Failed;
                }
                 else
                {
                    // The Value value should be a simple value of string type
                    if (jprsnElem1.eTypeAt(0) != tCIDNet::EJSONVTypes::String)
                    {
                        strmOut << L"Expected a string value" << kCIDLib::DNewLn;
                        eRes = tTestFWLib::ETestRes::Failed;
                    }
                     else if (jprsnElem1.strValueAt(0) != L"0")
                    {
                        strmOut << L"Expected the value \"0\"" << kCIDLib::DNewLn;
                        eRes = tTestFWLib::ETestRes::Failed;
                    }
                }
            }
             else
            {
                strmOut << L"Expected 1 array element" << kCIDLib::DNewLn;
                eRes = tTestFWLib::ETestRes::Failed;
            }
        }
    }

    catch(const TError& errToCatch)
    {
        TModule::LogEventObj(errToCatch);
        strmOut << L"Exception occurred." << kCIDLib::DNewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }

    return eRes;
}




// ---------------------------------------------------------------------------
//  CLASS: TTest_JSON2
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_JSON2: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_JSON2::TTest_JSON2() :

    TTestFWTest
    (
        L"JSON 2", L"Basic JSON parser test 2", 3
    )
{
}

TTest_JSON2::~TTest_JSON2()
{
}


// ---------------------------------------------------------------------------
//  TTest_JSON2: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_JSON2::eRunTest(  TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;

    const TString strTestData
    (
        L"{\n"
        L"   \"jsonrpc\" :\"2.0\"\n"
        L"   ,\"method\":\"Application.GetProperties\"\n"
        L"   ,\"params\":\n"
        L"     {\n"
        L"       \"properties\" :\n"
        L"       [\"volume\",\"muted\",\"name\",\"version\"]\n"
        L"     }\n"
        L"}\n"
    );



    // Some more complex tests than in Basic 1
    try
    {
        TJSONParser jprsTest;
        TTextStringInStream strmSrc(&strTestData);

        TJSONValue* pjprsnRootVal = jprsTest.pjprsnParse(strmSrc);
        TJanitor<TJSONValue> janRoot(pjprsnRootVal);

        // Make sure the root is an object and cast
        if (pjprsnRootVal->eType() != tCIDNet::EJSONVTypes::Object)
        {
            strmOut << L"Root is not an object" << kCIDLib::DNewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }
        TJSONObject* pjprsnRoot = static_cast<TJSONObject*>(pjprsnRootVal);


        // Do various access tests
        if ((pjprsnRoot->jprsnValueAt(0).eType() != tCIDNet::EJSONVTypes::String)
        ||  (pjprsnRoot->jprsnValueAt(0).strName() != L"jsonrpc"))
        {
            strmOut << L"Expected a string named jsonrpc" << kCIDLib::DNewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }
         else if ((pjprsnRoot->jprsnValueAt(1).eType() != tCIDNet::EJSONVTypes::String)
              ||  (pjprsnRoot->jprsnValueAt(1).strName() != L"method"))
        {
            strmOut << L"Expected a string named method" << kCIDLib::DNewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }
         else if ((pjprsnRoot->jprsnValueAt(2).eType() != tCIDNet::EJSONVTypes::Object)
              ||  (pjprsnRoot->jprsnValueAt(2).strName() != L"params"))
        {
            strmOut << L"Expected an object named params" << kCIDLib::DNewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }
         else
        {
            // Get the array from the params object and check the values
            const TJSONObject& jprsnParams = *pjprsnRoot->pjprsnObjAt(2);
            const TJSONArray& jprsnProps = *jprsnParams.pjprsnArrayAt(0);

            if (jprsnProps.strName() != L"properties")
            {
                strmOut << L"Expected an array named peroperties" << kCIDLib::DNewLn;
                eRes = tTestFWLib::ETestRes::Failed;
            }
             else if (jprsnProps.c4ValCount() == 4)
            {
                if ((jprsnProps.jprsnValueAt(0).strValue() != L"volume")
                ||  (jprsnProps.jprsnValueAt(1).strValue() != L"muted")
                ||  (jprsnProps.strValueAt(2) != L"name")
                ||  (jprsnProps.strValueAt(3) != L"version"))
                {
                    strmOut << L"properties array values were wrong" << kCIDLib::DNewLn;
                    eRes = tTestFWLib::ETestRes::Failed;
                }
            }
             else
            {
                strmOut << L"Expected 4 properties array element" << kCIDLib::DNewLn;
                eRes = tTestFWLib::ETestRes::Failed;
            }
        }

        // Test some special ones for accessing values of specific types
        const TJSONSimpleVal& jprsnMethod = *pjprsnRoot->pjprsnFindSVal(L"method");
        if (jprsnMethod.strValue() != L"Application.GetProperties")
        {
            strmOut << L"Find simple val returned wrong value" << kCIDLib::DNewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }

        const TJSONObject& jprsnParams = *pjprsnRoot->pjprsnFindObj(L"params");
        if (jprsnParams.c4ValCount() != 1)
        {
            strmOut << L"Find object returned wrong node" << kCIDLib::DNewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }
    }

    catch(const TError& errToCatch)
    {
        TModule::LogEventObj(errToCatch);
        strmOut << L"Exception occurred." << kCIDLib::DNewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }

    //
    //  And test the lambda callback scheme. We collection the paths and values
    //  we get back and compare them to what we should see. They should be in
    //  their natural order so we can just run down the check list we create.
    //
    try
    {
        const tCIDLib::TCard4 c4ValCnt = 6;
        const tCIDLib::TCh* apszPaths[c4ValCnt] =
        {
            L"/jsonrpc"
            , L"/method"
            , L"/params/properties/[1]"
            , L"/params/properties/[2]"
            , L"/params/properties/[3]"
            , L"/params/properties/[4]"
        };

        const tCIDLib::TCh* apszVals[c4ValCnt] =
        {
            L"2.0"
            , L"Application.GetProperties"
            , L"volume"
            , L"muted"
            , L"name"
            , L"version"
        };

        tCIDLib::TKVPList colReported;
        TJSONParser jprsTest;
        TTextStringInStream strmSrc(&strTestData);

        jprsTest.bParse
        (
            strmSrc
            , [&colReported](const  TString&                strPath
                            , const TString&                strVal
                            , const tCIDNet::EJSONVTypes    eType)
            {
                colReported.objAdd(TKeyValuePair(strPath, strVal));;
                return kCIDLib::True;
            }
        );

        if (colReported.c4ElemCount() != 6)
        {
            strmOut << L"Callback parse didn't create 6 values." << kCIDLib::DNewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }
         else
        {
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ValCnt; c4Index++)
            {
                const TKeyValuePair& kvalCur = colReported[c4Index];
                if (kvalCur.strKey() != apszPaths[c4Index])
                {
                    strmOut << L"Expected callback path '" << apszPaths[c4Index]
                            << L"' but got '" << kvalCur.strKey() << L"'"
                            << kCIDLib::DNewLn;
                    eRes = tTestFWLib::ETestRes::Failed;
                    break;
                }

                if (kvalCur.strKey() != apszPaths[c4Index])
                {
                    strmOut << L"Expected callback value '" << apszVals[c4Index]
                            << L"' but got '" << kvalCur.strValue() << L"'"
                            << kCIDLib::DNewLn;
                    eRes = tTestFWLib::ETestRes::Failed;
                    break;
                }
            }
        }
    }

    catch(const TError& errToCatch)
    {
        TModule::LogEventObj(errToCatch);
        strmOut << L"Exception occurred." << kCIDLib::DNewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }
    return eRes;
}




// ---------------------------------------------------------------------------
//  CLASS: TTest_JSON3
// PREFIX: tfwt
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_JSON3: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_JSON3::TTest_JSON3() :

    TTestFWTest
    (
        L"JSON 3", L"Basic JSON file parser tests 3", 3
    )
{
}

TTest_JSON3::~TTest_JSON3()
{
}


// ---------------------------------------------------------------------------
//  TTest_JSON3: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_JSON3::eRunTest(  TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;


    try
    {
        // Get the source directory from the environment
        TPathStr pathFiles = TProcEnvironment::strFind(L"CID_SRCTREE");

        // And add the path to our files
        pathFiles.AddLevel(L"Source");
        pathFiles.AddLevel(L"AllProjects");
        pathFiles.AddLevel(L"Tests2");
        pathFiles.AddLevel(L"TestNet");
        pathFiles.AddLevel(L"Files");
        pathFiles.AddLevel(L"JSON");


        // A list of files to process
        const tCIDLib::TCh* apszFiles[] =
        {
            L"file1.json"
            , L"file2.json"
            , L"file3.json"
            , L"file4.json"
        };
        const tCIDLib::TCard4 c4FlCnt = tCIDLib::c4ArrayElems(apszFiles);

        // Loop through them all and try to parse each one
        TJSONParser jprsTest;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4FlCnt; c4Index++)
        {
            try
            {
                TPathStr pathSrc(pathFiles);
                pathSrc.AddLevel(apszFiles[c4Index]);

                TTextFileInStream strmSrc
                (
                    pathSrc
                    , tCIDLib::ECreateActs::OpenIfExists
                    , tCIDLib::EFilePerms::Default
                    , tCIDLib::EFileFlags::SequentialScan
                    , tCIDLib::EAccessModes::Read
                );

                TJSONValue* pjprsnRoot = jprsTest.pjprsnParse(strmSrc);
                TJanitor<TJSONValue> janRoot(pjprsnRoot);

            }

            catch(const TError& errToCatch)
            {
                TModule::LogEventObj(errToCatch);
                strmOut << L"Exception occurred on file "
                        << apszFiles[c4Index]
                        << kCIDLib::DNewLn;
            }
        }
    }

    catch(const TError& errToCatch)
    {
        TModule::LogEventObj(errToCatch);
        strmOut << L"Exception occurred." << kCIDLib::DNewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }
    return eRes;
}




// ---------------------------------------------------------------------------
//  CLASS: TTest_JSON4
// PREFIX: tfwt
//
//  This one is just an abusive escape character test, to make sure that a
//  bunch of escapes doesn't confuse the parser.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_JSON4: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_JSON4::TTest_JSON4() :

    TTestFWTest
    (
        L"JSON 4", L"JSON escape chars test", 3
    )
{
}

TTest_JSON4::~TTest_JSON4()
{
}


// ---------------------------------------------------------------------------
//  TTest_JSON4: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_JSON4::eRunTest(  TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;


    try
    {
        const TString strTestData
        (
            L"{\n"
            L"  \"Value1\" : \"\\\\\\b\\f\\n\\r\\t\"\n"
            L"  , \"Value2\" : \"K:\\\\\"\n"
            L"}\n"
        );

        TJSONParser jprsTest;
        TTextStringInStream strmSrc(&strTestData);

        TJSONValue* pjprsnRoot = jprsTest.pjprsnParse(strmSrc);
        TJanitor<TJSONValue> janRoot(pjprsnRoot);
    }

    catch(const TError& errToCatch)
    {
        TModule::LogEventObj(errToCatch);
        strmOut << L"Exception occurred." << kCIDLib::DNewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }
    return eRes;
}



// ---------------------------------------------------------------------------
//  CLASS: TTest_JSON5
// PREFIX: tfwt
//
//  This tests the formatting of JSON objects.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTest_JSON5: Constructor and Destructor
// ---------------------------------------------------------------------------
TTest_JSON5::TTest_JSON5() :

    TTestFWTest
    (
        L"JSON 5", L"JSON formatting", 4
    )
{
}

TTest_JSON5::~TTest_JSON5()
{
}


// ---------------------------------------------------------------------------
//  TTest_JSON5: Public, inherited methods
// ---------------------------------------------------------------------------
tTestFWLib::ETestRes
TTest_JSON5::eRunTest(  TTextStringOutStream&   strmOut
                        , tCIDLib::TBoolean&    bWarning)
{
    tTestFWLib::ETestRes eRes = tTestFWLib::ETestRes::Success;


    try
    {
        //
        //  Use 3 spaces for indent, since that's what we tell the formatting
        //  method to use.
        const TString strTestData
        (
            L"{\n"
            L"   \"Key1\" : \"Value1\",\n"
            L"   \"Key2\" : \"Value2\",\n"
            L"   \"Array\" :\n"
            L"   [\n"
            L"      {\n"
            L"         \"A1\" : \"AValue1\"\n"
            L"      }\n"
            L"   ]\n"
            L"}\n"
        );

        // Parse the message
        TJSONParser jprsTest;
        TTextStringInStream strmSrc(&strTestData);
        TJSONValue* pjprsnRoot = jprsTest.pjprsnParse(strmSrc);
        TJanitor<TJSONValue> janRoot(pjprsnRoot);

        // Now format it back out and compare
        TTextStringOutStream strmFmt(1024UL);
        TJSONParser::FormatJSON(*pjprsnRoot, strmFmt, 3);

        // Now compare the formatted to the original
        if (strmFmt.strData() != strTestData)
        {
            strmOut << L"Formatted text != to original" << kCIDLib::DNewLn;
            eRes = tTestFWLib::ETestRes::Failed;
        }
    }

    catch(const TError& errToCatch)
    {
        TModule::LogEventObj(errToCatch);
        strmOut << L"Exception occurred in formatting test." << kCIDLib::DNewLn;
        eRes = tTestFWLib::ETestRes::Failed;
    }
    return eRes;
}


