//
// FILE NAME: CIDKernel_ResourceName.hpp
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
//  This is the header for the CIDKernel_ResourceName.Cpp file, which
//  implements the TKrnlRscName class. This class abstracts the names of
//  named resources, so that user code can create portable resource names.
//  A resource name, as defined by CIDLib, consists of 3 separate parts.
//
//  There is a company name, a subsystem name, and a resource name. These
//  form a hierarchy that prevents name clashes. They are stored separatedly
//  inside the TKrnlRscName object, and the full (platform specific) name
//  is created upon demand (via a call to BuildFullName().)
//
//  A process id can be optionally provided to create process unique names.
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
//   CLASS: TKrnlRscName
//  PREFIX: krscn
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlRscName
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlRscName() = default;

        TKrnlRscName
        (
            const   tCIDLib::TCh* const     pszCompany
            , const tCIDLib::TCh* const     pszSubsytsem
            , const tCIDLib::TCh* const     pszResource
            , const tCIDLib::TProcessId     pidToFormat = kCIDLib::pidInvalid
        );

        TKrnlRscName
        (
            const   TKrnlRscName&           krscnSrc
        );

        TKrnlRscName
        (
                    TKrnlRscName&&          krscnSrc
        );

        ~TKrnlRscName();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlRscName& operator=
        (
            const   TKrnlRscName&           krscnSrc
        );

        TKrnlRscName& operator=
        (
                    TKrnlRscName&&          krscnSrc
        );

        tCIDLib::TBoolean operator==
        (
            const   TKrnlRscName&           krscnSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TKrnlRscName&           krscnSrc
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bBuildFullName
        (
                    tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4MaxChars
            , const tCIDLib::ENamedRscTypes eType
        )   const;

        tCIDLib::TBoolean bIsValid() const;

        tCIDLib::TProcessId pidOfName() const;

        const tCIDLib::TCh* pszCompanyName() const;

        const tCIDLib::TCh* pszSubsystemName() const;

        const tCIDLib::TCh* pszResourceName() const;

        tCIDLib::TVoid SetName
        (
            const   tCIDLib::TCh* const     pszCompany
            , const tCIDLib::TCh* const     pszSubsytsem
            , const tCIDLib::TCh* const     pszResource
            , const tCIDLib::TProcessId     pidToFormat = kCIDLib::pidInvalid
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FaultInStrs() const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pidOfName
        //      This is the optional process id to format into the string. It defaults
        //      to kCIDLib::pidInvalid, which means it won't be used.
        //
        //  m_pszCompany
        //  m_pszSubsystem
        //  m_pszResource
        //      These are the three name parts. To make move semantics efficient, these
        //      can be null, and will be faulted in (to an empty string) if accessed.
        //      They ahve to be mutable because of that.
        // -------------------------------------------------------------------
        tCIDLib::TProcessId     m_pidOfName = kCIDLib::pidInvalid;
        mutable tCIDLib::TCh*   m_pszCompany = nullptr;
        mutable tCIDLib::TCh*   m_pszSubsystem = nullptr;
        mutable tCIDLib::TCh*   m_pszResource = nullptr;
};

#pragma CIDLIB_POPPACK


