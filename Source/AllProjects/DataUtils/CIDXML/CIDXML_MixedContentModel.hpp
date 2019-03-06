//
// FILE NAME: CIDXML_MixedContentModel.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/23/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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

        ~TXMLMixedCM();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDXML::EValidRes eValidate
        (
            const   tCIDLib::TCard4* const  pc4MixedIds
            , const tCIDLib::TCard4         c4MixedCount
            ,       tCIDLib::TCard4&        c4FailedAt
        )   const;

        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
            , const TXMLValidator&          xvalPool
        )   const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TXMLMixedCM();


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TXMLMixedCM(const TXMLMixedCM&);
        tCIDLib::TVoid operator=(const TXMLMixedCM&);


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


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXMLMixedCM,TXMLContentModel)
};

#pragma CIDLIB_POPPACK


