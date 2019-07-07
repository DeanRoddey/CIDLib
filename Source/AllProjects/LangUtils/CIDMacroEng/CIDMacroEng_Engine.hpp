//
// FILE NAME: CIDMacroEng_Engine.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2003
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
//  This is the header file for the CIDMacroEng_Engine.cpp file. This file
//  implements the macro engine, which is the core class of the macro system.
//  It provides the central repository of information for a macro session
//  which includes all of the registered classes, the call stack, session
//  settings, the temp pool, and pointers to the callback handlers that the
//  client installs.
//
//  We also define a small class, TMEngCallStackJan that is used to insure
//  that, when we 'fake' calls directly to macro methods from C++ code, that
//  the call stack gets back to the original stack top, no matter what.
//
// CAVEATS/GOTCHAS:
//
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TMEngExceptVal;


// ---------------------------------------------------------------------------
//  CLASS: TCIDMacroEngine
// PREFIX: me
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TCIDMacroEngine : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public class types
        // -------------------------------------------------------------------
        using TParmList = tCIDMacroEng::TClassValList;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDMacroEngine();

        TCIDMacroEngine(const TCIDMacroEngine&) = delete;

        ~TCIDMacroEngine();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDMacroEngine& operator=(const TCIDMacroEngine&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddBoolParm
        (
                    TParmList&              colOut
            , const TString&                strName
            , const tCIDLib::TBoolean       bConst
        );

        tCIDLib::TVoid AddStringParm
        (
                    TParmList&              colOut
            , const TString&                strName
            , const tCIDLib::TBoolean       bConst
        );

        tCIDLib::TBoolean bAreEquivCols
        (
            const   tCIDLib::TCard2         c2ClassId1
            , const tCIDLib::TCard2         c2ClassId2
            , const tCIDLib::TBoolean       bSameElemType
        )   const;

        tCIDLib::TBoolean bClassIsLoaded(const TString& strClassPath) const
        {
            return m_colClasses.bKeyExists(strClassPath);
        }

        tCIDLib::TBoolean bDebugMode() const
        {
            return m_bDebugMode;
        }

        tCIDLib::TBoolean bDebugMode
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TBoolean bInIDE() const;

        tCIDLib::TBoolean bInIDE
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TBoolean bFormatNextCallFrame
        (
                    TTextOutStream&         strmTarget
            ,       tCIDLib::TCard4&        c4PrevInd
        );

        tCIDLib::TBoolean bGetLastExceptInfo
        (
                    TString&                strClass
            ,       TString&                strText
            ,       tCIDLib::TCard4&        c4Line
        );

        tCIDLib::TBoolean bInvokeDefCtor
        (
                    TMEngClassVal&          mecvTarget
            , const TCIDUserCtx* const      pcuctxRights
        );

        tCIDLib::TBoolean bIsCollectionClass
        (
            const   tCIDLib::TCard2         c2ToCheck
        )   const;

        tCIDLib::TBoolean bIsDerivedFrom
        (
            const   tCIDLib::TCard2         c2IdToTest
            , const tCIDLib::TCard2         c2BaseClassId
        )   const;

        tCIDLib::TBoolean bIsDerivedFrom
        (
            const   tCIDLib::TCard2         c2IdToTest
            , const tCIDMacroEng::EIntrinsics  eBaseType
        )   const;

        tCIDLib::TBoolean bIsIndexableClass(const tCIDLib::TCard2 c2IdToCheck) const
        {
            return ((c2IdToCheck == m_c2ArrayId) || (c2IdToCheck == m_c2VectorId));
        }

        tCIDLib::TBoolean bIsIntrinsicClass(const tCIDLib::TCard2 c2IdToCheck) const
        {
            return (c2IdToCheck < tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Count));
        }

        tCIDLib::TBoolean bIsLiteralClass
        (
            const   tCIDLib::TCard2         c2ToCheck
        )   const;

        tCIDLib::TBoolean bIsNumericClass(const tCIDLib::TCard2 c2IdToCheck) const
        {
            return (c2IdToCheck >= tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::FirstNum))
                   && (c2IdToCheck <= tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::LastNum));
        }

        tCIDLib::TBoolean bStackValAt
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TBoolean       bCheckType = kCIDLib::False
        )   const;

        tCIDLib::TBoolean bValidation() const
        {
            return m_bValidation;
        }

        tCIDLib::TBoolean bValidation
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TCard1 c1StackValAt
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TBoolean       bCheckType = kCIDLib::False
        )   const;

        tCIDLib::TCard2 c2AddClass
        (
                    TMEngClassInfo* const   pmeciToAdopt
        );

        tCIDLib::TCard2 c2AddClassDefer
        (
                    TMEngClassInfo* const   pmeciToAdopt
        );

        tCIDLib::TCard2 c2EnumValAt
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TCard2         c2ExpectedId
            , const tCIDLib::TBoolean       bCheckType = kCIDLib::False
        )   const;

        tCIDLib::TCard2 c2FindClassId
        (
            const   TString&                strClassPath
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::True
        )   const;

        tCIDLib::TCard2 c2FindEntryPoint
        (
                    TMEngClassInfo&         meciTarget
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::True
        );

        tCIDLib::TCard2 c2StackValAt
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TBoolean       bCheckType = kCIDLib::False
        )   const;

        tCIDLib::TCard4 c4ClassCount() const;

        tCIDLib::TCard4 c4CreateParmList
        (
            const   TMEngMethodInfo&        methiTarget
            ,       TParmList&              colParms
        );

        tCIDLib::TCard4 c4CurLine() const
        {
            return m_c4CurLine;
        }

        tCIDLib::TCard4 c4CurLine(const tCIDLib::TCard4 c4ToSet)
        {
            m_c4CurLine = c4ToSet;
            return m_c4CurLine;
        }

        tCIDLib::TCard4 c4FindNextTry
        (
                    tCIDLib::TCard4&        c4CatchIP
        )   const;

        tCIDLib::TCard4 c4FirstParmInd
        (
            const   TMEngMethodInfo&        methiCalled
        );

        tCIDLib::TCard4 c4NextUniqueId();

        tCIDLib::TCard4 c4StackTop() const
        {
            return m_c4CallStackTop;
        }

        tCIDLib::TCard4 c4StackValAt
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TBoolean       bCheckType = kCIDLib::False
        )   const;

        tCIDLib::TCard8 c8StackValAt
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TBoolean       bCheckType = kCIDLib::False
        )   const;

        tCIDLib::TCh chStackValAt
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TBoolean       bCheckType = kCIDLib::False
        )   const;

        const tCIDLib::TStrList& colStackValAt
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TBoolean       bCheckType = kCIDLib::False
        )   const;

        const TCIDUserCtx& cuctxRights() const;

        tCIDLib::TVoid CheckIDEReq();

        tCIDLib::TVoid CheckSameClasses
        (
            const   TMEngClassVal&          mecv1
            , const TMEngClassVal&          mecv2
        )   const;

        tCIDLib::TVoid CheckStackTop
        (
            const   tCIDLib::TCard2         c2ExpectedClassId
            , const tCIDMacroEng::EConstTypes  eConst
        )   const;

        tCIDLib::TVoid Cleanup
        (
            const   tCIDLib::TBoolean       bRemoveBuiltins
        );

        tCIDLib::TVoid ContractFilePath
        (
            const   TString&                strFullPath
            ,       TPathStr&               pathToFill
        );

        tCIDMacroEng::EExceptReps eExceptReport() const;

        tCIDMacroEng::EExceptReps eExceptReport
        (
            const   tCIDMacroEng::EExceptReps  eToSet

        );

        tCIDMacroEng::EClassMatches eResolveClassName
        (
            const   TString&                strName
            ,       TRefCollection<TMEngClassInfo>& colMatches
        );

        tCIDMacroEng::ENumTypes eXlatNumType
        (
            const   tCIDLib::TCard2         c2NumClassId
        );

        tCIDLib::TVoid ExpandFilePath
        (
            const   TString&                strMacroFile
            ,       TPathStr&               pathToFill
        );

        tCIDLib::TVoid Exception
        (
            const   TError&                 errThrown
        );

        tCIDLib::TFloat4 f4StackValAt
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TBoolean       bCheckType = kCIDLib::False
        )   const;

        tCIDLib::TFloat8 f8StackValAt
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TBoolean       bCheckType = kCIDLib::False
        )   const;

        tCIDLib::TVoid FlipStackTop();

        tCIDLib::TInt4 i4Run
        (
                    TMEngClassVal&          mecvTarget
            ,       TParmList&              colParms
            , const TCIDUserCtx* const      pcuctxRights
        );

        tCIDLib::TInt4 i4Run
        (
                    TMEngClassVal&          mecvTarget
            , const TString&                strParmVals
            , const TCIDUserCtx* const      pcuctxRights
        );

        tCIDLib::TInt1 i1StackValAt
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TBoolean       bCheckType = kCIDLib::False
        )   const;

        tCIDLib::TInt2 i2StackValAt
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TBoolean       bCheckType = kCIDLib::False
        )   const;

        tCIDLib::TInt4 i4StackValAt
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TBoolean       bCheckType = kCIDLib::False
        )   const;

        tCIDLib::TVoid LogLastExcept();

        TMEngClassInfo& meciFind
        (
            const   TString&                strClassPath
        );

        const TMEngClassInfo& meciFind
        (
            const   TString&                strClassPath
        )   const;

        TMEngClassInfo& meciFind
        (
            const   tCIDLib::TCard2         c2ClassId
        );

        const TMEngClassInfo& meciFind
        (
            const   tCIDLib::TCard2         c2ClassId
        )   const;

        TMEngClassInfo& meciFind
        (
            const   tCIDMacroEng::EIntrinsics  eClassType
        );

        const TMEngClassInfo& meciFind
        (
            const   tCIDMacroEng::EIntrinsics  eClassType
        )   const;

        template <typename T>
        T& meciFindAs(const tCIDLib::TCard2 c2ClassId)
        {
            return static_cast<T&>(meciFind(c2ClassId));
        }

        template <typename T>
        T& meciFindAs(const tCIDMacroEng::EIntrinsics eClassType)
        {
            return static_cast<T&>(meciFind(eClassType));
        }

        TMEngClassInfo& meciPushMethodCall
        (
            const   tCIDLib::TCard2         c2ClassId
            , const tCIDLib::TCard2         c2MethodId
        );

        TMEngClassInfo& meciPushMethodCall
        (
            const   tCIDLib::TCard2         c2ClassId
            , const tCIDLib::TCard2         c2MethodId
            , const TMEngClassInfo* const   pmeciCaller
            , const TMEngOpMethodImpl* const pmethCaller
            , const TMEngMethodInfo* const  pmethiCaller
            ,       TMEngClassVal* const    pmecvCaller
            , const tCIDLib::TCard4         c4CallLine
            , const tCIDLib::TCard4         c4CallIP
        );

        TMEngCallStackItem& mecsiAt
        (
            const   tCIDLib::TCard4         c4Index
        );

        const TMEngCallStackItem& mecsiAt
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        TMEngCallStackItem& mecsiTop();

        TMEngMethodInfo& methiFind
        (
            const   tCIDLib::TCard2         c2ClassId
            , const tCIDLib::TCard2         c2MethodId
        );

        const TMEngExceptVal& mecvExceptionThrown() const;

        TMEngExceptVal& mecvExceptionThrown();

        const TMEngClassVal& mecvStackAt
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        TMEngClassVal& mecvStackAt
        (
            const   tCIDLib::TCard4         c4Index
        );

        template <typename T>
        const T& mecvStackAtAs(const tCIDLib::TCard4 c4Index) const
        {
            return static_cast<const T&>(mecvStackAt(c4Index));
        }

        template <typename T> T& mecvStackAtAs(const tCIDLib::TCard4 c4Index)
        {
            return static_cast<T&>(mecvStackAt(c4Index));
        }

        const TMEngClassVal& mecvStackAtTop() const;

        TMEngClassVal& mecvStackAtTop();

        template <typename T> const T& mecvStackAtTopAs() const
        {
            return static_cast<const T&>(mecvStackAtTop());
        }

        template <typename T> T& mecvStackAtTopAs()
        {
            return static_cast<T&>(mecvStackAtTop());
        }

        tCIDLib::TVoid MultiPop
        (
            const   tCIDLib::TCard4         c4Count
        );

        const TCIDUserCtx* pcuctxRights() const;

        TMEngClassInfo* pmeciFind
        (
            const   TString&                strClassPath
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        );

        const TMEngClassInfo* pmeciFind
        (
            const   TString&                strClassPath
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        )   const;

        template <typename T>
        T* pmeciFindAs( const   TString&    strClassPath
                        , const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False)
        {
            return static_cast<T*>(pmeciFind(strClassPath, bThrowIfNot));
        }

        const TMEngClassInfo* pmeciLastMacroCaller
        (
                    tCIDLib::TCard4&        c4Line
        );

        TMEngClassInfo* pmeciLoadExternalClass
        (
            const   TString&                strClassPathToLoad
        );

        TMEngClassVal* pmecvNewFromPath
        (
            const   TString&                strName
            , const TString&                strClassPath
            , const tCIDMacroEng::EConstTypes  eConst
        );

        MMEngDebugIntf* pmedbgToUse() const
        {
            return m_pmedbgToUse;
        }

        TMEngCallStackItem* pmecsiMostRecentCall
        (
                    tCIDLib::TCard4&        c4ToFill
        );

        TTextOutStream* pstrmConsole();

        tCIDLib::TVoid PopBackTo
        (
            const   tCIDLib::TCard4         c4To
        );

        tCIDLib::TVoid PopTop();

        tCIDLib::TVoid PushBoolean
        (
            const   tCIDLib::TBoolean       bToPush
            , const tCIDMacroEng::EConstTypes  eConst
        );

        tCIDLib::TVoid PushCard1
        (
            const   tCIDLib::TCard1         c1ToPush
            , const tCIDMacroEng::EConstTypes  eConst
        );

        tCIDLib::TVoid PushCard2
        (
            const   tCIDLib::TCard2         c2ToPush
            , const tCIDMacroEng::EConstTypes  eConst
        );

        tCIDLib::TVoid PushCard4
        (
            const   tCIDLib::TCard4         c4ToPush
            , const tCIDMacroEng::EConstTypes  eConst
        );

        tCIDLib::TVoid PushCard8
        (
            const   tCIDLib::TCard8         c8ToPush
            , const tCIDMacroEng::EConstTypes  eConst
        );

        tCIDLib::TVoid PushChar
        (
            const   tCIDLib::TCh            chToPush
            , const tCIDMacroEng::EConstTypes  eConst
        );

        tCIDLib::TVoid PushEnum
        (
            const   tCIDLib::TCard2         c2ClassId
            , const tCIDLib::TCard2         c2OrdValue
            , const tCIDMacroEng::EConstTypes  eConst
        );

        tCIDLib::TVoid PushException();

        tCIDLib::TVoid PushFloat4
        (
            const   tCIDLib::TFloat4        f4ToPush
            , const tCIDMacroEng::EConstTypes  eConst
        );

        tCIDLib::TVoid PushFloat8
        (
            const   tCIDLib::TFloat8        f8ToPush
            , const tCIDMacroEng::EConstTypes  eConst
        );

        tCIDLib::TVoid PushInt1
        (
            const   tCIDLib::TInt1          i1ToPush
            , const tCIDMacroEng::EConstTypes  eConst
        );

        tCIDLib::TVoid PushInt2
        (
            const   tCIDLib::TInt2          i2ToPush
            , const tCIDMacroEng::EConstTypes  eConst
        );

        tCIDLib::TVoid PushInt4
        (
            const   tCIDLib::TInt4          i4ToPush
            , const tCIDMacroEng::EConstTypes  eConst
        );

        tCIDLib::TVoid PushPoolValue
        (
            const   TString&                strTypeName
            , const tCIDMacroEng::EConstTypes  eConst
        );

        tCIDLib::TVoid PushPoolValue
        (
            const   tCIDLib::TCard2         c2TypeId
            , const tCIDMacroEng::EConstTypes  eConst
        );

        tCIDLib::TVoid PushPoolValue
        (
            const   tCIDMacroEng::EIntrinsics  eType
            , const tCIDMacroEng::EConstTypes  eConst
        );

        tCIDLib::TVoid PushPoolValue
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2TypeId
            , const tCIDMacroEng::EConstTypes  eConst
        );

        tCIDLib::TVoid PushPoolValue
        (
            const   TString&                strName
            , const tCIDMacroEng::EIntrinsics  eType
            , const tCIDMacroEng::EConstTypes  eConst
        );

        tCIDLib::TVoid PushString
        (
            const   TString&                strToPush
            , const tCIDMacroEng::EConstTypes  eConst
        );

        tCIDLib::TVoid PushStringList
        (
            const   tCIDLib::TStrList&      colToPush
            , const tCIDMacroEng::EConstTypes  eConst
        );

        tCIDLib::TVoid PushTime
        (
            const   TTime&                  tmToPush
            , const tCIDMacroEng::EConstTypes  eConst
        );

        tCIDLib::TVoid PushTry
        (
            const   tCIDLib::TCard4         c4ToPush
        );

        tCIDLib::TVoid PushValue
        (
                    TMEngClassVal* const    pmecvPushed
            , const tCIDMacroEng::EStackItems  eType
            , const tCIDLib::TBoolean       bRepush = kCIDLib::False
        );

        tCIDLib::TVoid RepushAt
        (
            const   tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid QueryScopeClasses
        (
            const   TString&                strScope
            ,       tCIDLib::TStrCollect&   colToFill
        )   const;

        tCIDLib::TVoid Reset();

        const TString& strClassPathForId
        (
            const   tCIDLib::TCard2         c2Id
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        )   const;

        const TString& strClassPathForId
        (
            const   tCIDMacroEng::EIntrinsics  eClassType
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        )   const;

        const TString& strSpecialDynRef() const;

        const TString& strSpecialDynRef
        (
            const   TString&                strToSet
        );

        const TString& strStackValAt
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TBoolean       bCheckType = kCIDLib::False
        )   const;

        tCIDLib::TVoid ReportLastExcept();

        tCIDLib::TVoid SetConsole
        (
                    TTextOutStream* const   pstrmToUse
        );

        tCIDLib::TVoid SetDbgHandler
        (
                    MMEngDebugIntf* const   pmedbgToUse
        );

        tCIDLib::TVoid SetErrHandler
        (
                    MMEngErrHandler* const  pmeehToUse
        );

        tCIDLib::TVoid SetException
        (
            const   tCIDLib::TCard2         c2ErrClassId
            , const TString&                strSrcClassPath
            , const tCIDLib::TCard4         c4ErrorNum
            , const TString&                strErrorName
            , const TString&                strErrorText
            , const tCIDLib::TCard4         c4LineNum
        );

        tCIDLib::TVoid SetFileResolver
        (
                    MMEngFileResolver* const pmefrToUse
        );

        tCIDLib::TVoid Sleep
        (
            const   tCIDLib::TCard4         c4Millis
        );

        const TTime& tmStackValAt
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TBoolean       bCheckType = kCIDLib::False
        )   const;

        tCIDLib::TVoid UnknownException();

        tCIDLib::TVoid ValidateCallFrame
        (
                    TMEngClassVal&          mecvInstance
            , const tCIDLib::TCard2         c2MethodId
        )   const;


    protected :
        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid AdjustStart
        (
                    TMEngClassVal&          mecvTarget
            ,       TParmList&              colParms
        );


    private :
        // -------------------------------------------------------------------
        //  Private data types
        // -------------------------------------------------------------------
        using TClassList    = TRefKeyedHashSet<TMEngClassInfo,TString,TStringKeyOps>;
        using TClassIdList  = TRefVector<TMEngClassInfo>;
        using TCntValList   = TCntPtr<tCIDMacroEng::TClassValList>;
        using TTempPool     = TVector<TCntValList>;
        using TCallStack    = TRefVector<TMEngCallStackItem>;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckClassId
        (
            const   tCIDLib::TCard2         c2Id
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid CheckNotEmpty
        (
            const   tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid CheckStackExpand();

        tCIDLib::TVoid CheckStackIndex
        (
            const   tCIDLib::TCard4         c4Line
            , const tCIDLib::TCard4         c4IndToCheck
        )   const;

        tCIDLib::TVoid CheckItemType
        (
            const   tCIDLib::TCard2         c2TypeId
            , const tCIDLib::TCard2         c2ExpectedId
            , const tCIDLib::TCard4         c4Line
            , const tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TVoid CheckItemType
        (
            const   tCIDLib::TCard2         c2TypeId
            , const tCIDMacroEng::EIntrinsics  eType
            , const tCIDLib::TCard4         c4Line
            , const tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TVoid CheckStackUnderflow
        (
            const   tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid CheckVarIndices
        (
            const   tCIDLib::TCard4         c4Line
            , const tCIDLib::TCard2         c2ClassId
            , const tCIDLib::TCard2         c2VarId
        );

        tCIDLib::TVoid FormatAFrame
        (
                    TTextOutStream&         strmTarget
            , const tCIDLib::TCard4         c4CallInd
        );

        tCIDLib::TVoid InitTempPool();

        TMEngClassVal* pmecvGet
        (
            const   tCIDLib::TCard2         c2ClassId
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngClassVal* pmecvGet
        (
            const   tCIDMacroEng::EIntrinsics  eType
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngClassVal* pmecvGet
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2ClassId
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngClassVal* pmecvGet
        (
            const   TString&                strName
            , const tCIDMacroEng::EIntrinsics  eType
            , const tCIDMacroEng::EConstTypes  eConst
        );

        tCIDLib::TVoid RegisterBuiltInClasses();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bDebugMode
        //      The IDE sets this on, else it is off by default. It is passed
        //      to each parser source, and tells them whether conditional
        //      debug blocks should be ignored or not.
        //
        //  bInIDE
        //      The IDE will set this flag, which lets us do some things differently
        //      than we would when running normally.
        //
        //  m_bValidation
        //      This can be set to make the engine do stack checks. Normally they
        //      aren't done because of the overhead.
        //
        //  m_c2NextClassId
        //      This is used to stamp classes that are added to this instance of the
        //      engine. Initially it's set to zero. As long as we keep the built in
        //      classes around for subsequent invocations, the Reset() method will set
        //      it to the next available id after the built in ones, so per-invocation
        //      ids start from there.
        //
        //  m_c2ArrayId
        //  m_c2VectorId
        //      For later checks, we store the class ids of the collection classes
        //      when we register them. Note that the array id is the last of the pre-
        //      setup classes that are always there. Any others only get loaded if the
        //      parsed CML uses them. So, the Cleanup() method, if told not to remove
        //      intrinsics, will remove any classes with a class id higher tahn the
        //      array class id. That will leave all of the fundamental classes in place
        //      so that we don't have to constantly reload them.
        //
        //  m_c4CallStackTop
        //      This is the top of stack index for the call stack. The vector
        //      used will expand as required, but we don't want to actually
        //      remove items from the vector, since that would require
        //      new/delete calls for every push/pop. So we let it expand as
        //      required, and use our own top of stack index. It is really
        //      always set to the count of items, i.e. the index where the
        //      *next* item will be placed. So if zero, the stack is empty.
        //
        //  m_c4CurLine
        //      While running a macro, this is kept up to date with the current
        //      line within the current class. Line number opcodes provide
        //      this info and the method impl class calls us to keep us
        //      informed about this, so that other code can get to it.
        //
        //  m_c4NextUniqueId
        //      Various classes may have a need to assign some unique numeric
        //      id to object instances. We provide a method to get ids that
        //      guaranteed unique within the engine instance, and use this
        //      counter to provide them. Since any given macro engine instance
        //      is single threaded, we need no sync to do this.
        //
        //      Note that we DO NOT reset this when the engine is reset, for further
        //      safety. We just let it keep counting up.
        //
        //  m_colClasses
        //      This is a collection of classes which have been registered with this
        //      engine instance. It will always include the built in classes, and will
        //      have at least one main class that implements the user's macro code, and
        //      that class will typically import other classes. This one is non-adopting,
        //      and the by=id one below actually owns them.
        //
        //  m_colClassesById
        //      We have to have fast lookup by name for name resolution during the
        //      parse, but we need fast lookup by id during runtime, since the resolved
        //      ids are used. So, we have a separate vector for that. Since we assign
        //      ids as ascending indices, by just putting them on the vector as they
        //      arrive, we create a by id lookup. This one owns the objects, not the by
        //      name one, for performance reasons, see the Cleanup method.
        //
        //  m_colCallStack
        //      This is the call stack. Return value storage, parameters, and
        //      procedure call information is pushed onto the stack and popped
        //      back off as calls are made.
        //
        //  m_colTempPool
        //      This is our temp value pool. We can put value objects here, keyed under
        //      class id, and reuse them.
        //
        //  m_eExceptReport
        //      Controls how the engine reports macro level exceptions to the error
        //      interface. By default it only reports exceptions if they are not handled.
        //
        //  m_pcuctxRights
        //      A pointer to a user security/rights context that can be set by users
        //      of the engine. This will in turn be available to any of the user
        //      classes that might need it when their methods are invoked.
        //
        //  m_pmedbgToUse
        //      This is an optional debugger interface that lets a debugger tap into
        //      the engine. It will usually be zero. We just make this directly
        //      available via a getter, so that the method impl class can get to it.
        //      He makes most of the calls.
        //
        //  m_pmeehToUse
        //      A pointer to the error handler to use. We adopt it. We pass exception
        //      information to this guy if any unhandled exceptions occur.
        //
        //  m_pmefrToUse
        //      A pointer to the file system resolver we are to use. This guy handles
        //      converting macro file paths into file system file paths.
        //
        //  m_pmecvThrown
        //      When an exception is thrown, we need a place to store it so that it
        //      can propogate possibly all the way back out of a nested call sequence.
        //      There is never more than one being thrown at once per engine, so we
        //      store it here. It is faulted in upon first use, and then just updated
        //      in subsequent throw operations.
        //
        //  m_pstrmConsole
        //      The outside world sets a text stream that should be used when the macro
        //      creates an OutputConsole class. We don't own this stream, we just ref it.
        //
        //  m_strSpecialDynRef
        //      We support a special dynamic class reference that can be set by the
        //      containing application.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean           m_bInIDE;
        tCIDLib::TBoolean           m_bDebugMode;
        tCIDLib::TBoolean           m_bValidation;
        tCIDLib::TCard2             m_c2ArrayId;
        tCIDLib::TCard2             m_c2NextClassId;
        tCIDLib::TCard2             m_c2VectorId;
        tCIDLib::TCard4             m_c4CallStackTop;
        tCIDLib::TCard4             m_c4CurLine;
        tCIDLib::TCard4             m_c4NextUniqueId;
        TCallStack                  m_colCallStack;
        TClassList                  m_colClasses;
        TClassIdList                m_colClassesById;
        TTempPool                   m_colTempPool;
        tCIDMacroEng::EExceptReps   m_eExceptReport;
        const TCIDUserCtx*          m_pcuctxRights;
        MMEngDebugIntf*             m_pmedbgToUse;
        MMEngErrHandler*            m_pmeehToUse;
        MMEngFileResolver*          m_pmefrToUse;
        TMEngExceptVal*             m_pmecvThrown;
        TTextOutStream*             m_pstrmConsole;
        TString                     m_strSpecialDynRef;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDMacroEngine,TObject)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngCallStackJan
// PREFIX: jan
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngCallStackJan
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngCallStackJan() = delete;

        TMEngCallStackJan
        (
                    TCIDMacroEngine* const  pmeToRestore
        );

        TMEngCallStackJan(const TMEngCallStackJan&) = delete;

        ~TMEngCallStackJan();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngCallStackJan& operator=(const TMEngCallStackJan&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4OrgTop
        //      The original call stack top that we will restore to
        //
        //  m_pmeToRestore
        //      The pointer to the engine to restore the call stack top to
        //      when we destruct.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4OrgTop;
        TCIDMacroEngine*    m_pmeToRestore;
};

#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  TCIDMacroEngine: Public, non-virtual methods
// ---------------------------------------------------------------------------



