//
// FILE NAME: CIDMacroEng_MiscClasses.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2003
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_MiscClasses.cpp file, which
//  implements some derivatives of the class info and class value classes that
//  are needed to create a new macro class. Each new class requires two
//  derivatives, one that describes the class (and can create new instance
//  data object for that class) and another that provides the value storage
//  for instances of that class.
//
//  We provide these derivatives here for a few misc types that didn't fit
//  into the other files.
//
//  Note that we define a TMEngObjectInfo macro class. This is a special
//  class which has no value representation, and which is the explicit base
//  class from which all other macro classes are derived. It defines no methods
//  at this time, but might eventually.
//
//  Also important is the TMEngFormattableInfo class, which defines the
//  Formattable macro class. All classes derived from this class must
//  implement the FormatTo() method, and format their output to a passed
//  text output stream. Most of the 'data oriented' classes will derive
//  from this class, since they tend to want to be formattable. Format is
//  non-final and required, so if they derive from the formattable class, they
//  must override it.
//
//  And we define the exception class here also. We use a single exception
//  class, not a hierarchy, so it's final.
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
//  CLASS: TMEngExceptVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngExceptVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngExceptVal() = delete;

        TMEngExceptVal
        (
            const   TString&                strName
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngExceptVal(const TMEngExceptVal&) = delete;

        ~TMEngExceptVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngExceptVal& operator=(const TMEngExceptVal&) = delete;


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


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckError
        (
            const   tCIDLib::TCard2         c2ErrClassId
            , const tCIDLib::TCard4         c4ErrToCheck
        )   const;

        tCIDLib::TBoolean bCheckGreater
        (
            const   tCIDLib::TCard2         c2ErrClassId
            , const tCIDLib::TCard4         c4ErrToCheck
        )   const;

        tCIDLib::TCard4 c4ErrorNum() const;

        tCIDLib::TCard2 c2ErrClassId() const;

        tCIDLib::TCard4 c4LineNum() const;

        tCIDLib::TVoid Reset();

        const TString& strErrorName() const;

        const TString& strErrorText() const;

        const TString& strSrcClassPath() const;

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCard2         c2ErrClassId
            , const TString&                strSrcClassPath
            , const tCIDLib::TCard4         c4ErrorNum
            , const TString&                strErrorName
            , const TString&                strErrorText
            , const tCIDLib::TCard4         c4LineNum
        );


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_c2ErrClassId
        //      The id of the enum class that was thrown. This lets us be
        //      type conscious with just a single exception class.
        //
        //  m_c4ErrorNum
        //      The error code being thrown. It's meaning is dependent upon
        //      the class it comes from. It is the ordinal value of the enum
        //      that was thrown.
        //
        //  m_c4LineNum
        //      The line number within the source class where the throw was
        //      done.
        //
        //  m_strErrorName
        //  m_strErrorText
        //      The error name and text. Errors are provided as enums, and
        //      each enum value has an associated name and text description,
        //      which are stored here for later display purposes.
        //
        //  m_strSrcClassPath
        //      The classpath of the class that threw the error. This is just
        //      for debug formatting.
        // -------------------------------------------------------------------
        tCIDLib::TCard2     m_c2ErrClassId;
        tCIDLib::TCard4     m_c4ErrorNum;
        tCIDLib::TCard4     m_c4LineNum;
        TString             m_strErrorName;
        TString             m_strErrorText;
        TString             m_strSrcClassPath;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngExceptVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngExceptInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngExceptInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngExceptInfo() = delete;

        TMEngExceptInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngExceptInfo(const TMEngExceptInfo&) = delete;

        ~TMEngExceptInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngExceptInfo& operator=(const TMEngExceptInfo&) = delete;



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
        //  Private data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_Check;
        tCIDLib::TCard2 m_c2MethId_CheckGreater;
        tCIDLib::TCard2 m_c2MethId_GetClass;
        tCIDLib::TCard2 m_c2MethId_GetErrorNum;
        tCIDLib::TCard2 m_c2MethId_GetErrorText;
        tCIDLib::TCard2 m_c2MethId_GetLine;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngExceptInfo,TMEngClassInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngFormattableVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngFormattableVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngFormattableVal
        (
            const   TString&                strName
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngFormattableVal(const TMEngFormattableVal&) = delete;

        ~TMEngFormattableVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngFormattableVal& operator=(const TMEngFormattableVal&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CopyFrom
        (
            const   TMEngClassVal&          mecvToCopy
            ,       TCIDMacroEngine&        meOwner
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngFormattableVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngFormattableInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngFormattableInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TCard2 c2FormatToId();

        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngFormattableInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngFormattableInfo(const TMEngFormattableInfo&) = delete;

        ~TMEngFormattableInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngFormattableInfo& operator=(const TMEngFormattableInfo&) = delete;


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
        //  Private data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_DefCtor;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngFormattableInfo,TMEngClassInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngObjectInfo
// PREFIX: meti
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngObjectInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngObjectInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngObjectInfo(const TMEngObjectInfo&) = delete;

        ~TMEngObjectInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngObjectInfo& operator=(const TMEngObjectInfo&) = delete;


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
        //  Private data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_DefCtor;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngObjectInfo,TMEngClassInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngVoidVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngVoidVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        TMEngVoidVal
        (
            const   TString&                strName
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngVoidVal(const TMEngVoidVal&) = delete;

        ~TMEngVoidVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngVoidVal& operator=(const TMEngVoidVal&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngVoidVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngVoidInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngVoidInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngVoidInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngVoidInfo(const TMEngVoidInfo&) = delete;

        ~TMEngVoidInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngVoidInfo& operator=(const TMEngVoidInfo&) = delete;


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
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngVoidInfo,TMEngClassInfo)
};


#pragma CIDLIB_POPPACK


