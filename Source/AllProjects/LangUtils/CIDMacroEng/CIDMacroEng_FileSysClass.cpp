//
// FILE NAME: CIDMacroEng_FileSysClass.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/25/2005
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the two class derivatives required to create a new
//  macro engine class that provides file system support.
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
#include    "CIDMacroEng_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TMEngDirIterInfo,TMacroEngClass)
RTTIDecls(TMEngDirIterVal,TMEngClassVal)
RTTIDecls(TMEngFileSysInfo,TMacroEngClass)
RTTIDecls(TMEngFileSysVal,TMEngClassVal)



// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_FileSysClass
{
    // The path and name for the directory iteration class
    const TString   strDirIterClassPath(L"MEng.System.Runtime.DirIter");
    const TString   strDirIterName(L"DirIter");

    // The path and name for the file system class
    const TString   strFileSysClassPath(L"MEng.System.Runtime.FileSystem");
    const TString   strFileSysName(L"FileSystem");
}





// ---------------------------------------------------------------------------
//  CLASS: TMEngDirIterVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngDirIterVal: Constuctors and Destructor
// ---------------------------------------------------------------------------
TMEngDirIterVal::TMEngDirIterVal(const  TString&                strName
                                , const tCIDLib::TCard2         c2Id
                                , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
{
}

TMEngDirIterVal::~TMEngDirIterVal()
{
}


// ---------------------------------------------------------------------------
//  TMEngDirIterVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngDirIterVal::bDbgFormat(        TTextOutStream&
                            , const TMEngClassInfo&
                            , const tCIDMacroEng::EDbgFmts
                            , const tCIDLib::ERadices
                            , const TCIDMacroEngine&) const
{
    // We have no displayable info
    return kCIDLib::False;
}


// -------------------------------------------------------------------
//  TMEngDirIterVal: Public, non-virtual methods
// -------------------------------------------------------------------
const TDirIter& TMEngDirIterVal::diterValue() const
{
    return m_diterValue;
}

TDirIter& TMEngDirIterVal::diterValue()
{
    return m_diterValue;
}


const TFindBuf& TMEngDirIterVal::fndbTmp() const
{
    return m_fndbTmp;
}

TFindBuf& TMEngDirIterVal::fndbTmp()
{
    return m_fndbTmp;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngDirIterInfo
// PREFIX: meunit
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngDirIterInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngDirIterInfo::strPath()
{
    return CIDMacroEng_FileSysClass::strDirIterClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngDirIterInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngDirIterInfo::TMEngDirIterInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_FileSysClass::strDirIterName
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2EnumId_Errors(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_FindFirst(kMacroEng::c2BadId)
    , m_c2MethId_FindNext(kMacroEng::c2BadId)
    , m_c4ErrFindFailed(kMacroEng::c2BadId)
    , m_pmeciErrors(0)
{
}

TMEngDirIterInfo::~TMEngDirIterInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngDirIterInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngDirIterInfo::Init(TCIDMacroEngine& meOwner)
{
    // An enum to represent the exceptions that this class throws
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"DirIterErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 1
        );
        m_c4ErrFindFailed = m_pmeciErrors->c4AddEnumItem(L"FindFailed", TString::strEmpty());
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2EnumId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    // The default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.DirIter"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Do a find first
    {
        TMEngMethodInfo methiNew
        (
            L"FindFirst"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"BasePath", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"WCToIter", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"FilesOnly", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddInParm(L"NormalOnly", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddOutParm(L"PathFound", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"FileSize", tCIDMacroEng::EIntrinsics::Card8);
        methiNew.c2AddOutParm(L"IsFile", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_FindFirst = c2AddMethodInfo(methiNew);
    }

    // Do a find next
    {
        TMEngMethodInfo methiNew
        (
            L"FindNext"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddOutParm(L"PathFound", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"FileSize", tCIDMacroEng::EIntrinsics::Card8);
        methiNew.c2AddOutParm(L"IsFile", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_FindNext = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngDirIterInfo::pmecvMakeStorage( const   TString&               strName
                                    ,       TCIDMacroEngine&
                                    , const tCIDMacroEng::EConstTypes eConst) const
{
    return new TMEngDirIterVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngDirIterInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngDirIterInfo::bInvokeMethod(        TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    TMEngDirIterVal& mecvActual = static_cast<TMEngDirIterVal&>(mecvInstance);
    TDirIter& diterThis = mecvActual.diterValue();
    TFindBuf& fndbTmp = mecvActual.fndbTmp();

    if (methiTarget.c2Id() == m_c2MethId_DefCtor)
    {
        // Just reset the object
        mecvActual.diterValue().Reset();
    }
     else if (methiTarget.c2Id() == m_c2MethId_FindFirst)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        try
        {
            // Expand out the CML level path to a real path
            meOwner.ExpandFilePath(meOwner.strStackValAt(c4FirstInd), m_pathExpand1);

            // Set up the search flags
            const tCIDLib::TBoolean bFilesOnly = meOwner.bStackValAt(c4FirstInd + 2);
            const tCIDLib::TBoolean bNormalOnly = meOwner.bStackValAt(c4FirstInd + 3);
            tCIDLib::EDirSearchFlags eFlags;
            if (bFilesOnly)
            {
                if (bNormalOnly)
                    eFlags = tCIDLib::EDirSearchFlags::NormalFiles;
                else
                    eFlags = tCIDLib::EDirSearchFlags::AllFiles;
            }
             else
            {
                if (bNormalOnly)
                    eFlags = tCIDLib::EDirSearchFlags::AllNormal;
                else
                    eFlags = tCIDLib::EDirSearchFlags::All;
            }

            const TString& strWC = meOwner.strStackValAt(c4FirstInd + 1);
            if (diterThis.bFindFirst(m_pathExpand1, strWC, fndbTmp, eFlags))
            {
                meOwner.ContractFilePath(fndbTmp.pathFileName(), m_pathExpand2);

                // Return the info
                TMEngStringVal& mecvPath = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 4);
                TMEngCard8Val& mecvSize = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd + 5);
                TMEngBooleanVal& mecvIsFile = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd + 6);

                mecvPath.strValue(m_pathExpand2);
                mecvSize.c8Value(fndbTmp.c8Size());
                mecvIsFile.bValue(fndbTmp.bIsFile());

                mecvRet.bValue(kCIDLib::True);
            }
             else
            {
                mecvRet.bValue(kCIDLib::False);
            }
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrFindFailed, errToCatch);
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_FindNext)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        try
        {
            if (diterThis.bFindNext(fndbTmp))
            {
                meOwner.ContractFilePath(fndbTmp.pathFileName(), m_pathExpand1);

                // Return the info
                TMEngStringVal& mecvPath = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);
                TMEngCard8Val& mecvSize = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd + 1);
                TMEngBooleanVal& mecvIsFile = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd + 2);

                mecvPath.strValue(m_pathExpand1);
                mecvSize.c8Value(fndbTmp.c8Size());
                mecvIsFile.bValue(fndbTmp.bIsFile());

                mecvRet.bValue(kCIDLib::True);
            }
             else
            {
                mecvRet.bValue(kCIDLib::False);
            }
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrFindFailed, errToCatch);
        }
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngDirIterInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngDirIterInfo::ThrowAnErr(       TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow
                            ,       TError&             errCaught)
{
    if (facCIDMacroEng().bShouldLog(errCaught))
    {
        errCaught.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errCaught);
    }

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , errCaught.strErrText()
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngFileSysVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngFileSysVal: Constuctors and Destructor
// ---------------------------------------------------------------------------
TMEngFileSysVal::TMEngFileSysVal(const  TString&                strName
                                , const tCIDLib::TCard2         c2Id
                                , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
{
}

TMEngFileSysVal::~TMEngFileSysVal()
{
}


// ---------------------------------------------------------------------------
//  TMEngFileSysVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngFileSysVal::bDbgFormat(        TTextOutStream&
                            , const TMEngClassInfo&
                            , const tCIDMacroEng::EDbgFmts
                            , const tCIDLib::ERadices
                            , const TCIDMacroEngine&) const
{
    // We have no displayable info
    return kCIDLib::False;
}





// ---------------------------------------------------------------------------
//  CLASS: TMEngFileSysInfo
// PREFIX: meunit
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngFileSysInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngFileSysInfo::strPath()
{
    return CIDMacroEng_FileSysClass::strFileSysClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngFileSysInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngFileSysInfo::TMEngFileSysInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_FileSysClass::strFileSysName
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2EnumId_Errors(kMacroEng::c2BadId)
    , m_c2MethId_CopyFile(kMacroEng::c2BadId)
    , m_c2MethId_DelFile(kMacroEng::c2BadId)
    , m_c2MethId_DirExists(kMacroEng::c2BadId)
    , m_c2MethId_FileExists(kMacroEng::c2BadId)
    , m_c2MethId_FindDirs(kMacroEng::c2BadId)
    , m_c2MethId_FindFiles(kMacroEng::c2BadId)
    , m_c2MethId_HasWildCards(kMacroEng::c2BadId)
    , m_c2MethId_MakePath(kMacroEng::c2BadId)
    , m_c2MethId_MakeSubDir(kMacroEng::c2BadId)
    , m_c2TypeId_PathList(kMacroEng::c2BadId)
    , m_c4ErrCopyFailed(kMacroEng::c2BadId)
    , m_c4ErrCreateFailed(kCIDLib::c4MaxCard)
    , m_c4ErrDelFailed(kCIDLib::c4MaxCard)
    , m_c4ErrGenErr(kCIDLib::c4MaxCard)
    , m_c4ErrOpenFailed(kCIDLib::c4MaxCard)
    , m_c4ErrPathNotFound(kCIDLib::c4MaxCard)
    , m_c4ErrPathNotFQ(kCIDLib::c4MaxCard)
    , m_c4ErrSearchFailed(kCIDLib::c4MaxCard)
    , m_pmeciErrors(0)
{
}

TMEngFileSysInfo::~TMEngFileSysInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngFileSysInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngFileSysInfo::Init(TCIDMacroEngine& meOwner)
{
    // An enum to represent the exceptions that this class throws
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"FileSysErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 8
        );
        m_c4ErrCopyFailed = m_pmeciErrors->c4AddEnumItem(L"CopyFailed", TString::strEmpty());
        m_c4ErrCreateFailed = m_pmeciErrors->c4AddEnumItem(L"CreateFailed", TString::strEmpty());
        m_c4ErrDelFailed = m_pmeciErrors->c4AddEnumItem(L"DelFailed", TString::strEmpty());
        m_c4ErrGenErr = m_pmeciErrors->c4AddEnumItem(L"FileSysErr", TString::strEmpty());
        m_c4ErrOpenFailed = m_pmeciErrors->c4AddEnumItem(L"OpenFailed", L"Could not open file '%(1)'");
        m_c4ErrPathNotFound = m_pmeciErrors->c4AddEnumItem(L"PathNotFound", L"Path '%(1)' was not found");
        m_c4ErrPathNotFQ = m_pmeciErrors->c4AddEnumItem(L"PathNotFQ", L"The passed path must be fully qualified");
        m_c4ErrSearchFailed = m_pmeciErrors->c4AddEnumItem(L"SearchFailed", TString::strEmpty());
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2EnumId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    //
    //  Create a nested class which is a vector of string objects. We
    //  need this as a parameter to some directory search methods.
    //
    {
        TMEngVectorInfo* pmeciNew = new TMEngVectorInfo
        (
            meOwner
            , L"PathList"
            , CIDMacroEng_FileSysClass::strFileSysClassPath
            , TMEngVectorInfo::strPath()
            , tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::String)
        );
        TJanitor<TMEngVectorInfo> janNewClass(pmeciNew);
        pmeciNew->BaseClassInit(meOwner);
        bAddNestedType(pmeciNew->strClassPath());
        m_c2TypeId_PathList = meOwner.c2AddClass(janNewClass.pobjOrphan());
    }

    // Look up some other types we need to use at runtime
//    m_pmeciCreateActs = meOwner.pmeciFindAs<TMEngEnumInfo>
//    (
//        TMEngBaseInfoInfo::strCreateActsPath()
//        , kCIDLib::True
//    );


    // Copy a file
    {
        TMEngMethodInfo methiNew
        (
            L"CopyFile"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"SourcePath", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"TarPath", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_CopyFile = c2AddMethodInfo(methiNew);
    }

    // Delete a file
    {
        TMEngMethodInfo methiNew
        (
            L"DelFile"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"FilePath", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_DelFile = c2AddMethodInfo(methiNew);
    }

    // Check to see if a directory exists
    {
        TMEngMethodInfo methiNew
        (
            L"DirExists"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"DirPath", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"NormalOnly", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_DirExists = c2AddMethodInfo(methiNew);
    }

    // Check to see if a file exists
    {
        TMEngMethodInfo methiNew
        (
            L"FileExists"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"FilePath", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"NormalOnly", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_FileExists = c2AddMethodInfo(methiNew);
    }

    // Find all dirs matching a wildcard in a given subdirectory
    {
        TMEngMethodInfo methiNew
        (
            L"FindDirs"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ParentDir", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"WildCard", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"NormalOnly", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddOutParm(L"ToFill", m_c2TypeId_PathList);
        m_c2MethId_FindDirs = c2AddMethodInfo(methiNew);
    }

    // Find all files matching a wildcard in a given subdirectory
    {
        TMEngMethodInfo methiNew
        (
            L"FindFiles"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ParentDir", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"WildCard", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"NormalOnly", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddOutParm(L"ToFill", m_c2TypeId_PathList);
        m_c2MethId_FindFiles = c2AddMethodInfo(methiNew);
    }

    // The default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.FileSys"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // See if a path has wild cards
    {
        TMEngMethodInfo methiNew
        (
            L"HasWildcards"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"PathToCheck", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_HasWildCards = c2AddMethodInfo(methiNew);
    }

    // Create a path if not already created
    {
        TMEngMethodInfo methiNew
        (
            L"MakePath"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToCreate", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_MakePath = c2AddMethodInfo(methiNew);
    }

    // Make a path if not already created
    {
        TMEngMethodInfo methiNew
        (
            L"MakeSubDir"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ParentPath", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"NewChild", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_MakeSubDir = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngFileSysInfo::pmecvMakeStorage(const  TString&                strName
                                ,       TCIDMacroEngine&
                                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngFileSysVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngFileSysInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngFileSysInfo::ThrowPathNotFQ(       TCIDMacroEngine&    meOwner
                                , const TString&            strThrowingClass)
{
    ThrowAnErr(meOwner, m_c4ErrPathNotFQ, strThrowingClass);
}


tCIDLib::TVoid
TMEngFileSysInfo::ThrowOpenFailed(          TCIDMacroEngine&    meOwner
                                    , const TString&            strThrowingClass
                                    , const TString&            strPath)
{
    TString strMsg(m_pmeciErrors->strTextValue(m_c4ErrOpenFailed));
    strMsg.eReplaceToken(strPath, L'1');
    ThrowAnErr(meOwner, m_c4ErrOpenFailed, strMsg, strThrowingClass);
}



// ---------------------------------------------------------------------------
//  TMEngFileSysInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngFileSysInfo::bInvokeMethod(        TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      )
{
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);

    if (methiTarget.c2Id() == m_c2MethId_CopyFile)
    {
        try
        {
            meOwner.ExpandFilePath(meOwner.strStackValAt(c4FirstInd), m_pathExpand1);
            meOwner.ExpandFilePath(meOwner.strStackValAt(c4FirstInd + 1), m_pathExpand2);
            TFileSys::CopyFile(m_pathExpand1, m_pathExpand2);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrCopyFailed, errToCatch);
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_DefCtor)
    {
        // Nothing to do in this case
    }
     else if (methiTarget.c2Id() == m_c2MethId_DelFile)
    {
        try
        {
            meOwner.ExpandFilePath(meOwner.strStackValAt(c4FirstInd), m_pathExpand1);
            TFileSys::DeleteFile(m_pathExpand1);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrDelFailed, errToCatch);
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_DirExists)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        try
        {
            meOwner.ExpandFilePath(meOwner.strStackValAt(c4FirstInd), m_pathExpand1);
            mecvRet.bValue
            (
                TFileSys::bExists
                (
                    m_pathExpand1
                    , meOwner.bStackValAt(c4FirstInd + 1) ? tCIDLib::EDirSearchFlags::NormalDirs
                                                          : tCIDLib::EDirSearchFlags::AllDirs
                )
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrSearchFailed, errToCatch);
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_FileExists)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        try
        {
            meOwner.ExpandFilePath(meOwner.strStackValAt(c4FirstInd), m_pathExpand1);
            mecvRet.bValue
            (
                TFileSys::bExists
                (
                    m_pathExpand1
                    , meOwner.bStackValAt(c4FirstInd + 1) ? tCIDLib::EDirSearchFlags::NormalFiles
                                                          : tCIDLib::EDirSearchFlags::AllFiles
                )
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrSearchFailed, errToCatch);
        }
    }
     else if ((methiTarget.c2Id() == m_c2MethId_FindDirs)
          ||  (methiTarget.c2Id() == m_c2MethId_FindFiles))
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        TMEngVectorVal& mecvToFill = meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 3);
        tCIDLib::TBoolean bGotSome = kCIDLib::False;
        try
        {
            meOwner.ExpandFilePath(meOwner.strStackValAt(c4FirstInd), m_pathExpand1);

            //
            //  The collection we get holds CML level objects, so we can't just
            //  pass it to the c4SearchDir() method. So we do a search loop
            //  ourself, with a directory interator.
            //
            tCIDLib::EDirSearchFlags eFlags;
            if (methiTarget.c2Id() == m_c2MethId_FindDirs)
            {
                if (meOwner.bStackValAt(c4FirstInd + 2))
                    eFlags = tCIDLib::EDirSearchFlags::NormalDirs;
                else
                    eFlags = tCIDLib::EDirSearchFlags::AllDirs;
            }
             else
            {
                if (meOwner.bStackValAt(c4FirstInd + 2))
                    eFlags = tCIDLib::EDirSearchFlags::NormalFiles;
                else
                    eFlags = tCIDLib::EDirSearchFlags::AllFiles;
            }

            mecvToFill.RemoveAll();
            TDirIter diterFind;
            TFindBuf fndbToFill;
            const TString& strWC = meOwner.strStackValAt(c4FirstInd + 1);
            if (diterFind.bFindFirst(m_pathExpand1, strWC, fndbToFill, eFlags))
            {
                bGotSome = kCIDLib::True;

                do
                {
                    // Get the CML level path out
                    meOwner.ContractFilePath
                    (
                        fndbToFill.pathFileName(), m_pathExpand2
                    );

                    mecvToFill.AddObject
                    (
                        new TMEngStringVal
                        (
                            TString::strEmpty()
                            , tCIDMacroEng::EConstTypes::NonConst
                            , m_pathExpand2
                        )
                    );
                }   while (diterFind.bFindNext(fndbToFill));
            }
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrSearchFailed, errToCatch);
        }
        mecvRet.bValue(bGotSome);
    }
     else if (methiTarget.c2Id() == m_c2MethId_HasWildCards)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        try
        {
            // In this case, we don't need to expand it
            mecvRet.bValue(TFileSys::bHasWildCards(meOwner.strStackValAt(c4FirstInd)));
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrGenErr, errToCatch);
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_MakePath)
    {
        try
        {
            meOwner.ExpandFilePath(meOwner.strStackValAt(c4FirstInd), m_pathExpand1);
            TFileSys::MakePath(m_pathExpand1);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrCreateFailed, errToCatch);
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_MakeSubDir)
    {
        try
        {
            // Expand the root path part
            meOwner.ExpandFilePath(meOwner.strStackValAt(c4FirstInd), m_pathExpand1);
            TFileSys::MakeSubDirectory(m_pathExpand1, meOwner.strStackValAt(c4FirstInd + 1));
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrCreateFailed, errToCatch);
        }
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngFileSysInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngFileSysInfo::ThrowAnErr(       TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow
                            , const TString&            strThrowingClass)
{
    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strThrowingClass
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , m_pmeciErrors->strTextValue(c4ToThrow)
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

tCIDLib::TVoid
TMEngFileSysInfo::ThrowAnErr(       TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow
                            , const TString&            strMsg
                            , const TString&            strThrowingClass)
{
    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strThrowingClass
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , strMsg
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

tCIDLib::TVoid
TMEngFileSysInfo::ThrowAnErr(       TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow
                            ,       TError&             errCaught)
{
    // If verbose log mode, we'll log the C++ level exception
    if (!errCaught.bLogged() && facCIDMacroEng().bLogWarnings())
    {
        errCaught.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errCaught);
    }

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , errCaught.strErrText()
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}


