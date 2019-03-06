//
// FILE NAME: CIDLib_UniqueName.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/07/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDLib_UniqueName.Cpp file. This file
//  implements the TUniqueName class, which is used to generate
//  sequentially numbered names.
//
//  This class uses the replacement token system. The text of the object
//  must contain a replacement parameter for the character '1' (i.e. %(1))
//  which will be replaced by a sequentially incremented number. The token
//  can be any legal token replacement so the format of the number can be
//  controlled.
//
// CAVEATS/GOTCHAS:
//
//  1)  We don't have a Cpp file, so our RTTI macros are done in CIDLib.Cpp
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TUniqueName
//  PREFIX: unam
// ---------------------------------------------------------------------------
class CIDLIBEXP TUniqueName : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TUniqueName();

        TUniqueName
        (
            const   TString&                strText
            , const tCIDLib::TCard4         c4InitialValue = 0
        );

        TUniqueName
        (
            const   tCIDLib::TCh* const     pszText
            , const tCIDLib::TCard4         c4InitialValue = 0
        );

        TUniqueName(const TUniqueName&) = delete;

        ~TUniqueName();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUniqueName& operator=(const TUniqueName&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4SetSequenceNum
        (
            const   tCIDLib::TCard4         c4NewNum
        );

        tCIDLib::TVoid QueryNewName
        (
                    TString&                strToFill
        );

        TString strQueryNewName();

        const TString& strText() const;

        const TString& strText
        (
            const   TString&                strNew
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ValidateText
        (
            const   TString&                strToValidate
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Number
        //      This is the counter that is used to generate the unique
        //      numbers for the names.
        //
        //  m_crsLock
        //      A critical section that we use to lock whil generating a
        //      new name.
        //
        //  m_strText
        //      The text that serves as the template for each name. It must
        //      have the token %(1) in it somewhere.
        // -------------------------------------------------------------------
        volatile tCIDLib::TCard4    m_c4Number;
        TCriticalSection            m_crsLock;
        TString                     m_strText;


        // -------------------------------------------------------------------
        //  Do any needed macros
        // -------------------------------------------------------------------
        RTTIDefs(TUniqueName,TObject)
};

#pragma CIDLIB_POPPACK


