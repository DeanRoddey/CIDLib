//
// FILE NAME: CIDMacroEng_OutputStreamClasses.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/07/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_OutputStreamClasses.cpp file,
//  which implements a base MEng.System.Runtime.TextOutputStream macro class.
//  From this class are derived classes for various types of output streams.
//
//  We provide methods to format all of the fundamental types. They are all
//  final because the drived stream classes create a stream of their desired
//  type and gives us the pointer, so the virtual aspect is handled via that
//  point to the C++ streams.
//
//  Because of this aspect of the stream classes, we don't need a value
//  class for all of the derivatives. Instead we just do one and set the
//  appropriate class id on each one. So the factory method of each info
//  class just returns the same value class, but with different info set
//  on it. Pretty much all the derived output text stream classes have to
//  do is provide that factory method to get the right kind of output stream
//  set on the value object.
//
//  This also means that we don't have to worry about the usual issue of
//  derived classes, since the value object passed down to parent methods
//  is the same C++ class in all cases. The derived classes are final so they
//  don't have any worries about that either.
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

class TMEngTextOutStreamInfo;

// ---------------------------------------------------------------------------
//  CLASS: TMEngTextOutStreamVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngTextOutStreamVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngTextOutStreamVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2ClassId
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngTextOutStreamVal(const TMEngTextOutStreamVal&) = delete;

        ~TMEngTextOutStreamVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngTextOutStreamVal& operator=(const TMEngTextOutStreamVal&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsSet() const;

        tCIDLib::TVoid Flush();

        const TString& strEncoding() const;

        const TString& strEncoding
        (
            const   TString&                strToSet
        );

        TTextOutStream& strmTarget
        (
                    TCIDMacroEngine&        meOwner
        );

        const TTextOutStream& strmTarget
        (
                    TCIDMacroEngine&        meOwner
        )   const;

        tCIDLib::TVoid SetStream
        (
                    TTextOutStream* const   pstrmToUse
            , const tCIDLib::EAdoptOpts     eAdopt
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eAdopt
        //      Indicates whether we own the stream. Normally we do, but in
        //      in the case of one derived class, the output console, it is
        //      actually owned by the engine, because it represents a configurable
        //      target stream (according to where/how the engine is being
        //      used.)
        //
        //  m_pstrmTarget
        //      The derived class gives us the target stream that they want
        //      to be used. This way, we can handle all of the formatting
        //      work at this level.
        //
        //  m_strEncoding
        //      The name of the encoding to use on this stream. Some derivatives
        //      won't use this, so only expose a method to get/set it on those
        //      that honor it. Some require a specific encoding.
        // -------------------------------------------------------------------
        tCIDLib::EAdoptOpts m_eAdopt;
        TTextOutStream*     m_pstrmTarget;
        TString             m_strEncoding;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngTextOutStreamVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngTextOutStreamInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngTextOutStreamInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngTextOutStreamInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngTextOutStreamInfo(const TMEngTextOutStreamInfo&) = delete;

        ~TMEngTextOutStreamInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngTextOutStreamInfo& operator=(const TMEngTextOutStreamInfo&) = delete;


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
            , const tCIDLib::TCard4         c4ToThrow
            ,       TError&                 errCaught
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2JustifyEnumId
        //      The class id of our nested justification enumeration type
        //      that we create when we are initialized.
        //
        //  m_c2RadixEnumId
        //      We use the radix enum (from BaseInfo), so we just go ahead
        //      and keep it around so we don't have to look it up each time
        //      a radix specific format is done.
        //
        //  m_c2MethId_XXX
        //      To speed up dispatching, which we handle at the class level
        //      since this is a fully 'external' class, we store the ids of
        //      our methods as we register them. We provide methods to format
        //      all of the fundamental types.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2EnumId_Errors;
        tCIDLib::TCard2 m_c2EnumId_Justify;
        tCIDLib::TCard2 m_c2EnumId_Radix;
        tCIDLib::TCard2 m_c2MethId_DNewLine;
        tCIDLib::TCard2 m_c2MethId_Flush;
        tCIDLib::TCard2 m_c2MethId_FmtBool;
        tCIDLib::TCard2 m_c2MethId_FmtCard1;
        tCIDLib::TCard2 m_c2MethId_FmtCard1R;
        tCIDLib::TCard2 m_c2MethId_FmtCard2;
        tCIDLib::TCard2 m_c2MethId_FmtCard2R;
        tCIDLib::TCard2 m_c2MethId_FmtCard4;
        tCIDLib::TCard2 m_c2MethId_FmtCard4R;
        tCIDLib::TCard2 m_c2MethId_FmtCard8;
        tCIDLib::TCard2 m_c2MethId_FmtCard8R;
        tCIDLib::TCard2 m_c2MethId_FmtChar;
        tCIDLib::TCard2 m_c2MethId_FmtCurLine;
        tCIDLib::TCard2 m_c2MethId_FmtFloat4;
        tCIDLib::TCard2 m_c2MethId_FmtFloat8;
        tCIDLib::TCard2 m_c2MethId_FmtFmtable;
        tCIDLib::TCard2 m_c2MethId_FmtInt1;
        tCIDLib::TCard2 m_c2MethId_FmtInt1R;
        tCIDLib::TCard2 m_c2MethId_FmtInt2;
        tCIDLib::TCard2 m_c2MethId_FmtInt2R;
        tCIDLib::TCard2 m_c2MethId_FmtInt4;
        tCIDLib::TCard2 m_c2MethId_FmtInt4R;
        tCIDLib::TCard2 m_c2MethId_FmtString;
        tCIDLib::TCard2 m_c2MethId_NewLine;
        tCIDLib::TCard2 m_c2MethId_Reset;
        tCIDLib::TCard2 m_c2MethId_SeekToEnd;
        tCIDLib::TCard2 m_c2MethId_SetJust;
        tCIDLib::TCard2 m_c2MethId_SetWidth;
        tCIDLib::TCard4 m_c4ErrId_FmtErr;
        TMEngEnumInfo*  m_pmeciErrs;
        TMEngEnumInfo*  m_pmeciJustify;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngTextOutStreamInfo,TMEngClassInfo)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngConOutStreamInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngConOutStreamInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngConOutStreamInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngConOutStreamInfo(const TMEngConOutStreamInfo&) = delete;

        ~TMEngConOutStreamInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngConOutStreamInfo& operator=(const TMEngConOutStreamInfo&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Init
        (
                    TCIDMacroEngine&        meOwner
        )   ;

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
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_DefCtor;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngConOutStreamInfo,TMEngClassInfo)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngFileOutStreamInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngFileOutStreamInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngFileOutStreamInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngFileOutStreamInfo(const TMEngFileOutStreamInfo&) = delete;

        ~TMEngFileOutStreamInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngFileOutStreamInfo& operator=(const TMEngFileOutStreamInfo&) = delete;


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
        //  m_c2EnumId_CreateAct
        //      The class id of the create action enumerated type from the
        //      BaseInfo class. This is used in method parameters so we look
        //      it up during init and keep it around for use.
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
        tCIDLib::TCard2 m_c2EnumId_CreateAct;
        tCIDLib::TCard2 m_c2MethId_Close;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_EncodingCtor;
        tCIDLib::TCard2 m_c2MethId_Open;
        TString         m_strDotDot;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngFileOutStreamInfo,TMEngClassInfo)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngStringOutStreamInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngStringOutStreamInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngStringOutStreamInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngStringOutStreamInfo(const TMEngStringOutStreamInfo&) = delete;

        ~TMEngStringOutStreamInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngStringOutStreamInfo& operator=(const TMEngStringOutStreamInfo&) = delete;


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
            , const tCIDLib::TCard4         c4ToThrow
            , const TString&                strText
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatching, which we handle at the class level
        //      since this is a fully 'external' class, we store the ids of
        //      our methods as we register them. We provide methods to format
        //      all of the fundamental types.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2EnumId_Errors;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_GetText;
        tCIDLib::TCard2 m_c2MethId_SizeCtor;
        tCIDLib::TCard2 m_c2MethId_XcodeTo;
        tCIDLib::TCard4 m_c4ErrId_Xcode;
        TMEngEnumInfo*  m_pmeciErrs;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngStringOutStreamInfo,TMEngClassInfo)
};


#pragma CIDLIB_POPPACK


