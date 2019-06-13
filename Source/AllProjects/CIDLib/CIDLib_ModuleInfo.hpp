//
// FILE NAME: CIDLib_ModuleInfo.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/23/2002
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
//  This is the header for the CIDLib_ModuleInfo.cpp files, which implements
//  the TCIDModuleInfo class. This is a very simple class that can hold the
//  information that indentifies a loadable module (DLL/ShareObj/SharedLib)
//  in terms of the information that CIDLib uses to identify and load them.
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
//  CLASS: TCIDModuleInfo
// PREFIX: modi
// ---------------------------------------------------------------------------
class CIDLIBEXP TCIDModuleInfo :

    public TObject, public MFormattable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDModuleInfo();

        TCIDModuleInfo
        (
            const   TString&                strBaseName
            , const tCIDLib::EModTypes      eModType
            , const tCIDLib::TCard4         c4MajVersion
            , const tCIDLib::TCard4         c4MinVersion
            , const tCIDLib::EModFlags      eFlags
        );

        TCIDModuleInfo
        (
            const   TString&                strBaseName
            , const tCIDLib::EModTypes      eModType
            , const tCIDLib::EModFlags      eFlags
        );

        TCIDModuleInfo
        (
            const   TCIDModuleInfo&         modiToCopy
        );

        ~TCIDModuleInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDModuleInfo& operator=
        (
            const   TCIDModuleInfo&         modiToAssign
        );

        tCIDLib::TBoolean operator==
        (
            const   TCIDModuleInfo&         modiToCompare
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TCIDModuleInfo&         modiToCompare
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4MajVersion() const;

        tCIDLib::TCard4 c4MinVersion() const;

        tCIDLib::EModFlags eFlags() const;

        tCIDLib::EModTypes eModType() const;

        const TString& strBaseName() const;

        tCIDLib::TVoid Set
        (
            const   TString&                strBaseName
            , const tCIDLib::EModTypes      eModType
            , const tCIDLib::TCard4         c4MajVersion
            , const tCIDLib::TCard4         c4MinVersion
            , const tCIDLib::EModFlags      eFlags
        );

        tCIDLib::TVoid Set
        (
            const   TString&                strBaseName
            , const tCIDLib::EModTypes      eModType
            , const tCIDLib::EModFlags      eFlags
        );


    protected   :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
        )   const;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        );

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const;


    private             :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4MajVersion
        //  m_c4MinVersion
        //      The major/minor version of the module, assuming its versioned.
        //      If not, then these will be ignored.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4MajVersion;
        tCIDLib::TCard4         m_c4MinVersion;
        tCIDLib::EModFlags      m_eFlags;
        tCIDLib::EModTypes      m_eModType;
        TString                 m_strBaseName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDModuleInfo,TObject)
};

#pragma CIDLIB_POPPACK


