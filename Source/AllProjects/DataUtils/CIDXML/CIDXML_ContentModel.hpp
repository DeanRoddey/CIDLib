//
// FILE NAME: CIDXML_ContentModel.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/22/1999
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
//  This is the header file for the CIDXML_ContentModel.Cpp file, which
//  implements the TXMLContentModel class. This class is an abstraction from
//  which a few different specific content model types can be derived.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TXMLValidator;


// ---------------------------------------------------------------------------
//  CLASS: TXMLContentModel
// PREFIX: xcm
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLContentModel : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TXMLContentModel(const TXMLContentModel&) = delete;

        ~TXMLContentModel() {}


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TXMLContentModel& operator=(const TXMLContentModel&) = delete;


        // -------------------------------------------------------------------
        //  Public, pure virtual methods
        // -------------------------------------------------------------------
        virtual tCIDXML::EValidRes eValidate
        (
            const   tCIDLib::TCard4* const  pc4ChildIds
            , const tCIDLib::TCard4         c4ChildCount
            ,       tCIDLib::TCard4&        c4FailedAt
        )   const = 0;

        virtual tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
            , const TXMLValidator&          xvalSrc
        )   const = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TXMLContentModel() {}


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXMLContentModel,TObject)
};

#pragma CIDLIB_POPPACK
