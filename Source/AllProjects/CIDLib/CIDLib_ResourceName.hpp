//
// FILE NAME: CIDLib_ResourceName.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/01/1997
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
//  This is the header for the CIDLib_ResourceName.Cpp file. This file
//  implements the TResourceName class. TResourceName is an abstraction for
//  the naming of shareable resources, such as events, semaphores, mutexes,
//  and memory. Since these things have different naming conventions on
//  each platform, CIDLib uses this abstraction to make such names portable.
//
//  A resource name consists of 3 parts. There is a company name part,
//  a subsystem name part, and a resource name part. This creates a name
//  hiearchy that is easily supported, but which still prevents any serious
//  possibility of name conflicts.
//
//  This class uses a kernel object, of class TKrnlRscName, which does all
//  of the actual work and contains all of the platform specific smarts.
//
//  This class will create a process unique name if you give it a process
//  handle.
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
//  CLASS: TResourceName
// PREFIX: rsn
// ---------------------------------------------------------------------------
class CIDLIBEXP TResourceName :

    public TObject, public MFormattable, public MDuplicable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TResourceName();

        TResourceName
        (
            const   TKrnlRscName&           krsnToUse
        );

        TResourceName
        (
            const   TString&                strCompany
            , const TString&                strSubsystem
            , const TString&                strResource
            , const tCIDLib::TProcessId     pidOfName = kCIDLib::pidInvalid
        );

        TResourceName
        (
            const   TResourceName&          rsnToCopy
        );

        ~TResourceName();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TBoolean operator==
        (
            const   TResourceName&          rsnToCompare
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TResourceName&          rsnToCompare
        )   const;

        TResourceName& operator=
        (
            const   TResourceName&          rsnToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsValid() const;

        tCIDLib::TVoid QueryNameParts
        (
                    TString&                strCompany
            ,       TString&                strSubsystem
            ,       TString&                strResource
        )   const;

        tCIDLib::TProcessId pidOfName() const;

        tCIDLib::TVoid SetName
        (
            const   TString&                strCompany
            , const TString&                strSubsystem
            , const TString&                strResource
            , const tCIDLib::TProcessId     pidOfName = kCIDLib::pidInvalid
        );

        const TString& strFullName
        (
            const   tCIDLib::ENamedRscTypes eType
        )   const;


    protected   :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strDest
        )   const override;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        ) override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eTypeCache
        //      This is the last type of resource we stored the full name
        //      for. If subsequent requests are of the same type, then
        //      we use the name already there.
        //
        //  m_krsnThis
        //      This the kernel resource name object that really does all
        //      of the work.
        //
        //  m_strFullName
        //      This is used to hold the full name of the resource. Its just
        //      to hold it, since the kernel resource name object creates
        //      it upon request. So it is filled in each time its needed.
        //      However, we cache the last type of resource we were asked
        //      to fill it for and don't bother if its the same as the last.
        // -------------------------------------------------------------------
        mutable tCIDLib::ENamedRscTypes m_eTypeCache;
        TKrnlRscName                    m_krsnThis;
        mutable TString                 m_strFullName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TResourceName,TObject)
        DefPolyDup(TResourceName)
        NulObject(TResourceName)
};

#pragma CIDLIB_POPPACK

