//
// FILE NAME: CIDMacroEng_BinFileClass.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/25/2005
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
//  This file implements the two class derivatives required to create a new
//  macro engine class that provides binary file support.
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
RTTIDecls(TMEngBinFileInfo,TMacroEngClass)
RTTIDecls(TMEngBinFileVal,TMEngClassVal)



// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_BinFileClass
{
    namespace
    {
        const TString   strName(L"BinFile");
        const TString   strClassPath(L"MEng.System.Runtime.BinFile");
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngBinFileVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngBinFileVal: Constuctors and Destructor
// ---------------------------------------------------------------------------
TMEngBinFileVal::TMEngBinFileVal(const  TString&                strName
                                , const tCIDLib::TCard2         c2Id
                                , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
{
}

TMEngBinFileVal::~TMEngBinFileVal()
{
}


// ---------------------------------------------------------------------------
//  TMEngBinFileVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngBinFileVal::bDbgFormat(        TTextOutStream&     strmTarget
                            , const TMEngClassInfo&
                            , const tCIDMacroEng::EDbgFmts eFormat
                            , const tCIDLib::ERadices
                            , const TCIDMacroEngine&    ) const
{
    if (eFormat == tCIDMacroEng::EDbgFmts::Long)
    {
        strmTarget << L"Open = "  << m_bflValue.bIsOpen() << kCIDLib::NewLn;
        if (m_bflValue.bIsOpen())
            strmTarget  << L"Name = " << m_bflValue.strName() << kCIDLib::NewLn;
    }
     else
    {
        strmTarget << m_bflValue.strName();
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngBinFileVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
TBinaryFile& TMEngBinFileVal::bflValue()
{
    return m_bflValue;
}


tCIDLib::TVoid TMEngBinFileVal::Close()
{
    return m_bflValue.Close();
}


tCIDLib::TVoid
TMEngBinFileVal::Open(  const   TString&                strFileName
                        , const tCIDLib::EAccessModes   eAccess
                        , const tCIDLib::ECreateActs eAction
                        , const tCIDLib::EFilePerms     ePerms
                        , const tCIDLib::EFileFlags     eFlags)
{
    // Close it first in case it was open
    m_bflValue.Close();

    // Set the new file name and open it with new settings
    m_bflValue.strName(strFileName);
    m_bflValue.Open(eAccess, eAction, ePerms, eFlags);
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngBinFileInfo
// PREFIX: meunit
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngBinFileInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngBinFileInfo::strPath()
{
    return CIDMacroEng_BinFileClass::strClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngBinFileInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngBinFileInfo::TMEngBinFileInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_BinFileClass::strName
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2EnumId_Errors(kCIDMacroEng::c2BadId)
    , m_c2MethId_Close(kCIDMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kCIDMacroEng::c2BadId)
    , m_c2MethId_Flush(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetCurPos(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetCurSize(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetIsOpen(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetLastAcc(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetLastMod(kCIDMacroEng::c2BadId)
    , m_c2MethId_OffsetCurPos(kCIDMacroEng::c2BadId)
    , m_c2MethId_Open(kCIDMacroEng::c2BadId)
    , m_c2MethId_ReadBuffer(kCIDMacroEng::c2BadId)
    , m_c2MethId_SetCurPos(kCIDMacroEng::c2BadId)
    , m_c2MethId_TruncateAt(kCIDMacroEng::c2BadId)
    , m_c2MethId_WriteBuffer(kCIDMacroEng::c2BadId)
    , m_c4ErrCloseFailed(kCIDLib::c4MaxCard)
    , m_c4ErrGenErr(kCIDLib::c4MaxCard)
    , m_c4ErrOpenFailed(kCIDLib::c4MaxCard)
    , m_c4ErrNotOpen(kCIDLib::c4MaxCard)
    , m_c4ErrReadFailed(kCIDLib::c4MaxCard)
    , m_c4ErrWriteFailed(kCIDLib::c4MaxCard)
    , m_pmeciErrors(0)
    , m_pmeciCreateActs(0)
{
    // Add imports for any non-intrinics we use internally
    bAddClassImport(TMEngBaseInfoInfo::strPath());
}

TMEngBinFileInfo::~TMEngBinFileInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngBinFileInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngBinFileInfo::Init(TCIDMacroEngine& meOwner)
{
    // An enum to represent the exceptions that this class throws
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"BinFileErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 8
        );
        m_c4ErrCloseFailed = m_pmeciErrors->c4AddEnumItem(L"CloseFailed", TString::strEmpty());
        m_c4ErrGenErr = m_pmeciErrors->c4AddEnumItem(L"FileErr", TString::strEmpty());
        m_c4ErrOpenFailed = m_pmeciErrors->c4AddEnumItem(L"OpenFailed", TString::strEmpty());
        m_c4ErrNotOpen = m_pmeciErrors->c4AddEnumItem(L"NotOpen", facCIDMacroEng().strMsg(kMEngErrs::errcRT_NotOpen));
        m_c4ErrReadFailed = m_pmeciErrors->c4AddEnumItem(L"ReadFailed", TString::strEmpty());
        m_c4ErrWriteFailed = m_pmeciErrors->c4AddEnumItem(L"WriteFailed", TString::strEmpty());
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2EnumId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    // Add some literals for some flags that the Open method uses
    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kFlag_WriteThrough"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val
              (
                L"kFlag_WriteThrough"
                , tCIDMacroEng::EConstTypes::Const
                , tCIDLib::c4EnumOrd(tCIDLib::EFileFlags::WriteThrough)
              )
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kFlag_RandomAccess"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val
              (
                L"kFlag_RandomAccess"
                , tCIDMacroEng::EConstTypes::Const
                , tCIDLib::c4EnumOrd(tCIDLib::EFileFlags::RandomAccess)
              )
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kFlag_SequentialScan"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val
              (
                L"kFlag_SequentialScan"
                , tCIDMacroEng::EConstTypes::Const
                , tCIDLib::c4EnumOrd(tCIDLib::EFileFlags::SequentialScan)
              )
        )
    );

    // Add some literals for the access mode flags
    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kAccess_Read"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val
              (
                L"kFAccess_Read"
                , tCIDMacroEng::EConstTypes::Const
                , tCIDLib::c4EnumOrd(tCIDLib::EAccessModes::Read)
              )
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kAccess_Write"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val
              (
                L"kFAccess_Write"
                , tCIDMacroEng::EConstTypes::Const
                , tCIDLib::c4EnumOrd(tCIDLib::EAccessModes::Write)
              )
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kAccess_DenyRead"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val
              (
                L"kFAccess_DenyRead"
                , tCIDMacroEng::EConstTypes::Const
                , tCIDLib::c4EnumOrd(tCIDLib::EAccessModes::DenyRead)
              )
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kAccess_DenyWrite"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val
              (
                L"kFAccess_DenyWrite"
                , tCIDMacroEng::EConstTypes::Const
                , tCIDLib::c4EnumOrd(tCIDLib::EAccessModes::DenyRead)
              )
        )
    );


    // Look up some other types we need to use at runtime
    m_pmeciCreateActs = meOwner.pmeciFindAs<TMEngEnumInfo>
    (
        TMEngBaseInfoInfo::strCreateActsPath()
        , kCIDLib::True
    );

    // Close the file object if open
    {
        TMEngMethodInfo methiNew
        (
            L"Close"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Close = c2AddMethodInfo(methiNew);
    }

    // The default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.BinFile"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Flush the file buffer
    {
        TMEngMethodInfo methiNew
        (
            L"Flush"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Flush = c2AddMethodInfo(methiNew);
    }

    // Get the current file position and file size
    {
        TMEngMethodInfo methiNew
        (
            L"GetCurPos"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetCurPos = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetCurSize"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetCurSize = c2AddMethodInfo(methiNew);
    }

    // Get whether the file is open
    {
        TMEngMethodInfo methiNew
        (
            L"GetIsOpen"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetIsOpen = c2AddMethodInfo(methiNew);
    }

    // Get the last access or last modify times
    {
        TMEngMethodInfo methiNew
        (
            L"GetLastAccess"
            , tCIDMacroEng::EIntrinsics::Time
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetLastAcc = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetLastMod"
            , tCIDMacroEng::EIntrinsics::Time
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetLastMod = c2AddMethodInfo(methiNew);
    }

    // Offset the current file position
    {
        TMEngMethodInfo methiNew
        (
            L"OffsetCurPos"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"OfsBy", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_OffsetCurPos = c2AddMethodInfo(methiNew);
    }

    // Open a file
    {
        TMEngMethodInfo methiNew
        (
            L"Open"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Path", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"CreateAct", m_pmeciCreateActs->c2Id());
        methiNew.c2AddInParm(L"AccFlags", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Flags", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_Open = c2AddMethodInfo(methiNew);
    }

    // Read a buffer
    {
        TMEngMethodInfo methiNew
        (
            L"ReadBuffer"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::MemBuf);
        m_c2MethId_ReadBuffer = c2AddMethodInfo(methiNew);
    }

    // Change the current read/write position
    {
        TMEngMethodInfo methiNew
        (
            L"SetCurPos"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"NewPos", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_SetCurPos = c2AddMethodInfo(methiNew);
    }

    // Truncate the file at a given position (leaves the pos there)
    {
        TMEngMethodInfo methiNew
        (
            L"TruncateAt"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"At", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_TruncateAt = c2AddMethodInfo(methiNew);
    }

    // Write a buffer
    {
        TMEngMethodInfo methiNew
        (
            L"WriteBuffer"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"ToWrite", tCIDMacroEng::EIntrinsics::MemBuf);
        m_c2MethId_WriteBuffer = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngBinFileInfo::pmecvMakeStorage(const  TString&                strName
                                ,       TCIDMacroEngine&
                                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngBinFileVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngBinFileInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngBinFileInfo::bInvokeMethod(        TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    //
    //  In our case, we have all external 'methods', so we just handle them
    //  all either right here, or call a private to do it.
    //
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    TMEngBinFileVal& mecvActual = static_cast<TMEngBinFileVal&>(mecvInstance);
    TBinaryFile& bflThis = mecvActual.bflValue();

    if (methiTarget.c2Id() == m_c2MethId_Close)
    {
        try
        {
            bflThis.Close();
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrCloseFailed, errToCatch.strErrText());
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_DefCtor)
    {
        // Nothing to do in this case
    }
     else if (methiTarget.c2Id() == m_c2MethId_Flush)
    {
        CheckIsOpen(meOwner, bflThis);
        try
        {
            bflThis.Flush();
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrGenErr, errToCatch.strErrText());
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_GetCurPos)
    {
        CheckIsOpen(meOwner, bflThis);
        try
        {
            TMEngCard8Val& mecvRet
                        = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1);
            mecvRet.c8Value(bflThis.c8CurPos());
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrGenErr, errToCatch.strErrText());
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_GetCurSize)
    {
        CheckIsOpen(meOwner, bflThis);
        try
        {
            TMEngCard8Val& mecvRet
                        = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1);
            mecvRet.c8Value(bflThis.c8CurSize());
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrGenErr, errToCatch.strErrText());
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_GetIsOpen)
    {
        TMEngBooleanVal& mecvRet
                        = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(bflThis.bIsOpen());
    }
     else if (methiTarget.c2Id() == m_c2MethId_GetLastAcc)
    {
        TMEngTimeVal& mecvRet
                        = meOwner.mecvStackAtAs<TMEngTimeVal>(c4FirstInd - 1);
        mecvRet.tmValue(bflThis.tmLastAccess());
    }
     else if (methiTarget.c2Id() == m_c2MethId_GetLastMod)
    {
        TMEngTimeVal& mecvRet
                        = meOwner.mecvStackAtAs<TMEngTimeVal>(c4FirstInd - 1);
        mecvRet.tmValue(bflThis.tmLastWrite());
    }
     else if (methiTarget.c2Id() == m_c2MethId_OffsetCurPos)
    {
        CheckIsOpen(meOwner, bflThis);
        try
        {
            TMEngCard8Val& mecvRet
                        = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1);
            mecvRet.c8Value(bflThis.c8OffsetFilePos(meOwner.i4StackValAt(c4FirstInd)));
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrGenErr, errToCatch.strErrText());
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_Open)
    {
        // Expand to a real local path, and make sure it has the right slashes
        TPathStr pathToOpen;
        meOwner.ExpandFilePath(meOwner.strStackValAt(c4FirstInd), pathToOpen);

        try
        {
            mecvActual.Open
            (
                pathToOpen
                , tCIDLib::EAccessModes(meOwner.c4StackValAt(c4FirstInd + 2))
                , tCIDLib::ECreateActs(meOwner.c4StackValAt(c4FirstInd + 1))
                , tCIDLib::EFilePerms::Default
                , tCIDLib::EFileFlags(meOwner.c4StackValAt(c4FirstInd + 3))
            );
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrOpenFailed, errToCatch.strErrText());
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_ReadBuffer)
    {
        CheckIsOpen(meOwner, bflThis);
        try
        {
            TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
            TMEngMemBufVal& mecvBuf = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd);
            mecvRet.c4Value
            (
                bflThis.c4ReadBuffer
                (
                    mecvBuf.mbufValue()
                    , meOwner.c4StackValAt(c4FirstInd + 1)
                )
            );
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrReadFailed, errToCatch.strErrText());
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_SetCurPos)
    {
        CheckIsOpen(meOwner, bflThis);
    }
     else if (methiTarget.c2Id() == m_c2MethId_TruncateAt)
    {
        CheckIsOpen(meOwner, bflThis);
        try
        {
            bflThis.TruncateAt(meOwner.c8StackValAt(c4FirstInd));
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrGenErr, errToCatch.strErrText());
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_WriteBuffer)
    {
        CheckIsOpen(meOwner, bflThis);
        try
        {
            TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
            const TMEngMemBufVal& mecvBuf = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd + 1);
            mecvRet.c4Value
            (
                bflThis.c4WriteBuffer
                (
                    mecvBuf.mbufValue()
                    , meOwner.c4StackValAt(c4FirstInd)
                )
            );
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrWriteFailed, errToCatch.strErrText());
        }
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngBinFileInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngBinFileInfo::CheckIsOpen(          TCIDMacroEngine&    meOwner
                                , const TBinaryFile&        bflToCheck)
{
    if (!bflToCheck.bIsOpen())
        ThrowAnErr(meOwner, m_c4ErrNotOpen);
}


tCIDLib::TVoid
TMEngBinFileInfo::ThrowAnErr(       TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow)
{
    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , m_pmeciErrors->strTextValue(c4ToThrow)
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

tCIDLib::TVoid
TMEngBinFileInfo::ThrowAnErr(       TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow
                            , const TString&            strText)
{
    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , strText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

