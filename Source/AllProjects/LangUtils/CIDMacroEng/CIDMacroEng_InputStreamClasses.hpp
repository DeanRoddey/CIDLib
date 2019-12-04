//
// FILE NAME: CIDMacroEng_InputStreamClasses.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/10/2003
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
//  This is the header file for the CIDMacroEng_InputStreamClasses.cpp file,
//  which implements a base MEng.System.Runtime.TextInputStream macro class.
//  From this class are derived classes for various types of input streams.
//
//  We don't need a value class for all of the derivatives. Instead we just
//  do one and set the appropriate class id on each one. So the factory method
//  of each info class just returns the same value class, but with different
//  info set on it. Pretty much all the derived input text stream classes have
//  to do is provide that factory method to get the right kind of C++ input
//  stream set on the value object.
//
// CAVEATS/GOTCHAS:
//
//  1)  The base class' methods check that the stream has been set, else they
//      throw. The derived classes don't do this unless they have some method
//      that requires it and usually they don't. The drived classes are mainly
//      involved in setting it set, and cleaning it up.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TMEngTextInStreamInfo;

// ---------------------------------------------------------------------------
//  CLASS: TMEngTextInStreamVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class TMEngTextInStreamVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngTextInStreamVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2ClassId
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngTextInStreamVal(const TMEngTextInStreamVal&) = delete;

        ~TMEngTextInStreamVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngTextInStreamVal& operator=(const TMEngTextInStreamVal&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsSet() const;

        const TString& strEncoding() const;

        const TString& strEncoding
        (
            const   TString&                strToSet
        );

        const TString& strExtra() const;

        const TString& strExtra
        (
            const   TString&                strToSet
        );

        TTextInStream& strmSrc
        (
            const   TCIDMacroEngine&        meOwner
        );

        const TTextInStream& strmSrc
        (
            const   TCIDMacroEngine&        meOwner
        )   const;

        tCIDLib::TVoid SetStream
        (
                    TTextInStream* const    pstrmToAdopt
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtuao methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckStreamSet
        (
            const   TCIDMacroEngine&        meOwner
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //  m_pstrmSrc
        //      The derived class gives us the target stream that they want
        //      to be used. This way, we can handle all of the formatting
        //      work at this level. It's not set in our ctor, it's set from
        //      some derived macro class method that creates it.
        //
        //  m_strEncoding
        //      The name of the encoding to use on this stream.
        //
        //  m_strExtra
        //      All of the derivatives use this same value class, but some of
        //      them need a little extra storage for information specific to
        //      them.
        // -------------------------------------------------------------------
        TTextInStream*  m_pstrmSrc;
        TString         m_strEncoding;
        TString         m_strExtra;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngTextInStreamVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngTextInStreamInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngTextInStreamInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngTextInStreamInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngTextInStreamInfo(const TMEngTextInStreamInfo&) = delete;

        ~TMEngTextInStreamInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngTextInStreamInfo& operator=(const TMEngTextInStreamInfo&) = delete;


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
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4Line
            , const TString&                strText
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatching, which we handle at the class level
        //      since this is a fully 'external' class, we store the ids of
        //      our methods as we register them.
        //
        //  m_c2TypeId_XXX
        //      The type ids for any of our nested types we create.
        //
        //  m_pmeciErrors
        //      A pointer to the error type, so that we have it around for
        //      error throwing.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_EndOfStream;
        tCIDLib::TCard2 m_c2MethId_GetLine;
        tCIDLib::TCard2 m_c2MethId_Reset;
        tCIDLib::TCard2 m_c2MethId_SetRepChar;
        tCIDLib::TCard2 m_c2MethId_SetErrAction;
        tCIDLib::TCard2 m_c2TypeId_Errors;
        tCIDLib::TCard2 m_c2TypeId_InErrActs;
        tCIDLib::TCard4 m_c4ErrId_Internalize;
        tCIDLib::TCard4 m_c4ErrId_Reset;
        tCIDLib::TCard4 m_c4ErrId_Configure;
        TMEngEnumInfo*  m_pmeciErrors;
        TMEngEnumInfo*  m_pmeciInErrActs;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngTextInStreamInfo,TMEngClassInfo)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngConInStreamInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngConInStreamInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngConInStreamInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngConInStreamInfo(const TMEngConInStreamInfo&) = delete;

        ~TMEngConInStreamInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngConInStreamInfo& operator=(const TMEngConInStreamInfo&) = delete;


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
        //      To speed up dispatching, which we handle at the class level
        //      since this is a fully 'external' class, we store the ids of
        //      our methods as we register them.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_DefCtor;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngConInStreamInfo,TMEngClassInfo)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngFileInStreamInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngFileInStreamInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngFileInStreamInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngFileInStreamInfo(const TMEngFileInStreamInfo&) = delete;

        ~TMEngFileInStreamInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngFileInStreamInfo& operator=(const TMEngFileInStreamInfo&) = delete;


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
        //      To speed up dispatching, which we handle at the class level
        //      since this is a fully 'external' class, we store the ids of
        //      our methods as we register them. We provide methods to format
        //      all of the fundamental types.
        //
        //  m_strDotDot
        //      All file paths must be fully qualified, and cannot have any
        //      ".." entries, so we search for them in any file paths.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_Close;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_EncCtor;
        tCIDLib::TCard2 m_c2MethId_GetFileName;
        tCIDLib::TCard2 m_c2MethId_Open;
        TString         m_strDotDot;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngFileInStreamInfo,TMEngClassInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngStringInStreamInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngStringInStreamInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngStringInStreamInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngStringInStreamInfo(const TMEngStringInStreamInfo&) = delete;

        ~TMEngStringInStreamInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngStringInStreamInfo& operator=(const TMEngStringInStreamInfo&) = delete;


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
        //      To speed up dispatching, which we handle at the class level
        //      since this is a fully 'external' class, we store the ids of
        //      our methods as we register them.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_SetText;
        tCIDLib::TCard2 m_c2MethId_StrCtor;
        tCIDLib::TCard2 m_c2MethId_SyncCtor;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngStringInStreamInfo,TMEngClassInfo)
};


#pragma CIDLIB_POPPACK


