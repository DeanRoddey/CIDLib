//
// FILE NAME: CIDMacroEng_StdClass.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/24/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_StdClass.cpp file, which
//  implements derivates of the class info and class value classes that are
//  used by the compiler for compiled macros from user code. These classes
//  are all the same except for the classpath of the class. The members of
//  such a class are all dynamically controlled and added as method, var,
//  import, and so forth objects. The invocation is just a passthrough to
//  the targeted method object.
//
//  The value class is just a dummy, since we don't need any actual C++
//  level data members, but we must have an instance data class. At some
//  point, it might be used to keep some housekeeping, instrumentation, or
//  debugging info. The macro level member objects are handled by the common
//  base value class. Any classes which only need macro level members objects
//  can use this class for their value object, which is all macro level
//  defined classes, but also any wrapped C++ classes which store their
//  data as standard macro level value objects (and they all should if they
//  can.)
//
//  Copying to and from involves copying the contents of each defined
//  variable for that class. We initially set the class as copyable, but
//  if any non-coypable member is added, it becomes non-copyable.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TMEngStdClassInfo;


// ---------------------------------------------------------------------------
//  CLASS: TMEngStdClassVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngStdClassVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngStdClassVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2ClassId
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngStdClassVal(const TMEngStdClassVal&) = delete;

        ~TMEngStdClassVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngStdClassVal& operator=(const TMEngStdClassVal&) = delete;


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


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends. We want our info class to be our friend so
        //  that, when he creates a new instance, he can load us up with
        //  value objects for our members.
        // -------------------------------------------------------------------
        friend class TMEngStdClassInfo;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngStdClassVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngStdClassInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngStdClassInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngStdClassInfo
        (
            const   TString&                strName
            , const TString&                strBasePath
            ,       TCIDMacroEngine&        meOwner
            , const TString&                strParentClassPath
        );

        TMEngStdClassInfo(const TMEngStdClassInfo&) = delete;

        ~TMEngStdClassInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngStdClassInfo& operator=(const TMEngStdClassInfo&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
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
        RTTIDefs(TMEngStdClassInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK


