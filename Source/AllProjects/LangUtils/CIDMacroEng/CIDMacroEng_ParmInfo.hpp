//
// FILE NAME: CIDMacroEng_ParmInfo.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/14/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_ParmInfo.cpp file, which
//  implements the TMEngParmInfo class. This class is what is stored in
//  a method object to describe the parameters to that method, if any.
//  It contains the name of the parameter (for name resolution purposes),
//  the class if of the data type, and the in/out designation for the
//  parameter.
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
//  CLASS: TMEngParmInfo
// PREFIX: mepi
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngParmInfo : public TMEngNamedItem
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngParmInfo();

        TMEngParmInfo
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2ClassId
            , const tCIDMacroEng::EParmDirs    eDir
        );

        TMEngParmInfo
        (
            const   TMEngParmInfo&          mepiToCopy
        );

        ~TMEngParmInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TBoolean operator==
        (
            const   TMEngParmInfo&          mepiToCompare
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TMEngParmInfo&          mepiToCompare
        )   const;

        TMEngParmInfo& operator=
        (
            const   TMEngParmInfo&          mepiToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsLegalOverride
        (
            const   TMEngParmInfo&          mepiToCheck
        );

        tCIDLib::TCard2 c2ClassId() const;

        tCIDMacroEng::EParmDirs eDir() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2ClassId
        //      The class id of the class type of this parameter.
        //
        //  m_eDir
        //      The direction indicator for the parameter.
        // -------------------------------------------------------------------
        tCIDLib::TCard2         m_c2ClassId;
        tCIDMacroEng::EParmDirs    m_eDir;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngParmInfo,TMEngNamedItem)
};

#pragma CIDLIB_POPPACK



