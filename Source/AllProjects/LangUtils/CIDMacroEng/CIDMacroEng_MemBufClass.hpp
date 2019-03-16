//
// FILE NAME: CIDMacroEng_MemBufClass.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/18/2003
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
//  This is the header file for the CIDMacroEng_MemBufClass.cpp file, which
//  implements some derivatives of the class info and class value classes that
//  are needed to create a new macro class. Each new class requires two
//  derivatives, one that describes the class (and can create new instance
//  data object for that class) and another that provides the value storage
//  for instances of that class.
//
//  We provide these derivatives here for the MEng.System.RunTime.MemBuf class,
//  which provides access to a heap based memory buffer. It is given an
//  allocation size upon creation, and is fully allocated to that size. It
//  supports putting and getting the fundamental data types at arbitrary
//  offsets, to aid in building up or interpreting predefined memory
//  structures in memory, which is often required for interfacing to outside
//  systems.
//
//  The buffer is initially created with a trivial buffer size when the val
//  object is created, because the actual size won't be known until the macro
//  level contructor is called. It will then be reallocated.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TMEngMemBufVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngMemBufVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngMemBufVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngMemBufVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
            ,       THeapBuf* const         pmbufToUse
        );

        TMEngMemBufVal(const TMEngMemBufVal&) = delete;

        ~TMEngMemBufVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngMemBufVal& operator=(const TMEngMemBufVal&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDbgFormat
        (
                    TTextOutStream&         strmTarget
            , const TMEngClassInfo&         meciThis
            , const tCIDMacroEng::EDbgFmts     eFormat
            , const tCIDLib::ERadices       eRadix
            , const TCIDMacroEngine&        meOwner
        )   const override;

        tCIDLib::TVoid CopyFrom
        (
            const   TMEngClassVal&          mecvToCopy
            ,       TCIDMacroEngine&        meOwner
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const THeapBuf& mbufValue() const;

        THeapBuf& mbufValue();


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_eAdopt
        //      Indicates whether we adopted the passed buffer. We want to
        //      be able to use passed in buffers to push on the stack and
        //      get results back to the C++ world, that's why we use a pointer
        //      and allow adoption or just usage of passed buffers.
        //
        //  m_pmbufValue
        //      This is the underlyng CIDLib memory buffer that provides the
        //      storage for this class.
        // -------------------------------------------------------------------
        tCIDLib::EAdoptOpts m_eAdopt;
        THeapBuf*           m_pmbufValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngMemBufVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngMemBufInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngMemBufInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngMemBufInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngMemBufInfo(const TMEngMemBufInfo&) = delete;

        ~TMEngMemBufInfo();


        // -------------------------------------------------------------------
        //  Public oeprators
        // -------------------------------------------------------------------
        TMEngMemBufInfo& operator=(const TMEngMemBufInfo&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Init
        (
                    TCIDMacroEngine&        meOwner
        )   override;

        TMEngClassVal* pmecvMakeStorage
        (
            const   TString&                strName
            ,       TCIDMacroEngine&        meOwner
            , const tCIDMacroEng::EConstTypes  eConst
        )   const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid IndexCheck
        (
                    TCIDMacroEngine&        meOwner
            , const TMEngMemBufVal&         mecvCheck
            , const tCIDLib::TCard4         c4Count
            , const tCIDLib::TCard4         c4Index
            , const tCIDLib::TBoolean       bMaxSize
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bInvokeMethod
        (
                    TCIDMacroEngine&        meOwner
            , const TMEngMethodInfo&        methiTarget
            ,       TMEngClassVal&          mecvInstance
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckIndex
        (
                    TCIDMacroEngine&        meOwner
            , const TMemBuf&                mbufCheck
            , const tCIDLib::TCard4         c4Count
            , const tCIDLib::TCard4         c4Index
            , const tCIDLib::TBoolean       bMaxSize = kCIDLib::True
        );

        tCIDLib::TCard4 c4ExtractASCII
        (
                    TCIDMacroEngine&        meOwner
            , const TMemBuf&                mbufSrc
            , const tCIDLib::TCard4         c4At
            , const tCIDLib::TCard4         c4Count
            , const tCIDLib::ERadices       eRadix
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const MFormattable&           fmtblToken1
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const MFormattable&           fmtblToken1
            , const MFormattable&           fmtblToken2
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const MFormattable&           fmtblToken1
            , const MFormattable&           fmtblToken2
            , const MFormattable&           fmtblToken3
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2EnumId_XXX
        //      The ids of the nested types that we create.
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init. We don't have a default ctor, because
        //      the buffer size must be provided.
        //
        //  m_c4Err_XXX
        //      We store the error enum value ids for later use when we
        //      throw errors.
        //
        //  m_pmeciErrors
        //      When we register our error enum, we store a pointer to the
        //      class info object for use during error throwing. We don't
        //      own it.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2EnumId_Errors;
        tCIDLib::TCard2 m_c2MethId_CalcSum;
        tCIDLib::TCard2 m_c2MethId_CheckIndRange;
        tCIDLib::TCard2 m_c2MethId_CompBytes;
        tCIDLib::TCard2 m_c2MethId_CopyIn;
        tCIDLib::TCard2 m_c2MethId_CopyInAt;
        tCIDLib::TCard2 m_c2MethId_CopyOut;
        tCIDLib::TCard2 m_c2MethId_ExportString;
        tCIDLib::TCard2 m_c2MethId_ExportStringAt;
        tCIDLib::TCard2 m_c2MethId_ExportVarString;
        tCIDLib::TCard2 m_c2MethId_ExtractDecDigAt;
        tCIDLib::TCard2 m_c2MethId_ExtractDecValAt;
        tCIDLib::TCard2 m_c2MethId_ExtractHexDigAt;
        tCIDLib::TCard2 m_c2MethId_ExtractHexValAt;
        tCIDLib::TCard2 m_c2MethId_FindByte;
        tCIDLib::TCard2 m_c2MethId_GetCard1At;
        tCIDLib::TCard2 m_c2MethId_GetCard2At;
        tCIDLib::TCard2 m_c2MethId_GetCard4At;
        tCIDLib::TCard2 m_c2MethId_GetFloat4At;
        tCIDLib::TCard2 m_c2MethId_GetFloat8At;
        tCIDLib::TCard2 m_c2MethId_GetInt1At;
        tCIDLib::TCard2 m_c2MethId_GetInt2At;
        tCIDLib::TCard2 m_c2MethId_GetInt4At;
        tCIDLib::TCard2 m_c2MethId_GetAlloc;
        tCIDLib::TCard2 m_c2MethId_GetMaxSize;
        tCIDLib::TCard2 m_c2MethId_ImportString;
        tCIDLib::TCard2 m_c2MethId_ImportStringAt;
        tCIDLib::TCard2 m_c2MethId_InsertASCIIHexPair;
        tCIDLib::TCard2 m_c2MethId_MakeSpace;
        tCIDLib::TCard2 m_c2MethId_MoveToStart;
        tCIDLib::TCard2 m_c2MethId_PutCard1At;
        tCIDLib::TCard2 m_c2MethId_PutCard2At;
        tCIDLib::TCard2 m_c2MethId_PutCard4At;
        tCIDLib::TCard2 m_c2MethId_PutFloat4At;
        tCIDLib::TCard2 m_c2MethId_PutFloat8At;
        tCIDLib::TCard2 m_c2MethId_PutInt1At;
        tCIDLib::TCard2 m_c2MethId_PutInt2At;
        tCIDLib::TCard2 m_c2MethId_PutInt4At;
        tCIDLib::TCard2 m_c2MethId_Reallocate;
        tCIDLib::TCard2 m_c2MethId_RemoveSpace;
        tCIDLib::TCard2 m_c2MethId_SetAll;
        tCIDLib::TCard2 m_c2MethId_SzCtor;
        tCIDLib::TCard4 m_c4Err_BadChar;
        tCIDLib::TCard4 m_c4Err_BadDigitRange;
        tCIDLib::TCard4 m_c4Err_BadIndex;
        tCIDLib::TCard4 m_c4Err_BadInitSizes;
        tCIDLib::TCard4 m_c4Err_BadReallocSize;
        tCIDLib::TCard4 m_c4Err_BadValue;
        tCIDLib::TCard4 m_c4Err_ByteRange;
        tCIDLib::TCard4 m_c4Err_NotDecDig;
        tCIDLib::TCard4 m_c4Err_NotHexDig;
        tCIDLib::TCard4 m_c4Err_Overflow;
        tCIDLib::TCard4 m_c4Err_BlockMove;
        tCIDLib::TCard4 m_c4Err_SelfTarget;
        TMEngEnumInfo*  m_pmeciErrors;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngMemBufInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK


