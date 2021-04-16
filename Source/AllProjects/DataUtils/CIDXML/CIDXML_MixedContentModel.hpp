//
// FILE NAME: CIDXML_MixedContentModel.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/23/1999
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
//  This is the header file for the CIDXML_MixedContentModel.Cpp file, which
//  implements the TXMLMixedCM class. This class is derived from the base
//  content model class, TXMLContentModel, and provides an implementation
//  for elements with the "Mixedren" style content model.
//
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
//  CLASS: TXMLMixedCM
// PREFIX: xcm
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLMixedCM : public TXMLContentModel
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TXMLMixedCM
        (
            const   tCIDLib::TCard4* const  pc4ChildIds
            , const tCIDLib::TCard4         c4Count
        );

        TXMLMixedCM(const TXMLMixedCM&) = delete;
        TXMLMixedCM(TXMLMixedCM&&) = delete;

        ~TXMLMixedCM();


        // -------------------------------------------------------------------
        //  Public constructors
        // -------------------------------------------------------------------
        TXMLMixedCM& operator=(const TXMLMixedCM&) = delete;
        TXMLMixedCM& operator=(TXMLMixedCM&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDXML::EValidRes eValidate
        (
            const   tCIDLib::TCard4* const  pc4MixedIds
            , const tCIDLib::TCard4         c4MixedCount
            ,       tCIDLib::TCard4&        c4FailedAt
        )   const final;

        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
            , const TXMLValidator&          xvalPool
        )   const final;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TXMLMixedCM();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Count
        //      This is the count of elements in the child id list.
        //
        //  m_pc4ChildIds
        //      This is the list of child ids that are valid for this content
        //      model. PCDATA is implied and not actually present.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4Count;
        tCIDLib::TCard4*    m_pc4ChildIds;
};

#pragma CIDLIB_POPPACK


