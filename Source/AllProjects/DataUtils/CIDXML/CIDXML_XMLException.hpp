//
// FILE NAME: CIDXML_ParserException.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/01/1999
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the header file for the CIDXML_ParserException.Cpp file. This
//  file implements the TXMLException class.
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
//  CLASS: TXMLException
// PREFIX: xexc
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLException : public TObject, public MFormattable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TXMLException
        (
            const   tCIDLib::TErrCode       errcThrow
            , const TString&                strMessage
            , const TString&                strEntityName
            , const tCIDLib::TCard4         c4Line
            , const tCIDLib::TCard4         c4Column
        );

        TXMLException
        (
            const   TXMLException&          xexcToCopy
        );

        ~TXMLException();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TXMLException& operator=
        (
            const   TXMLException&          xexcToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Column() const;

        tCIDLib::TCard4 c4Line() const;

        const TString& strEntityName() const;

        const TString& strMessage() const;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Column
        //  m_c4Line
        //      The column and line with the entity being parsed when the
        //      error occured.
        //
        //  m_errcThrown
        //      This is the error code that is being thrown.
        //
        //  m_strEntityName
        //      This is the name of the entity that was being parsed when
        //      the error occured.
        //
        //  m_strMessage
        //      This is the error text for the error being thrown. It will
        //      be loaded by the client code and provided in the ctor.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4Column;
        tCIDLib::TCard4     m_c4Line;
        tCIDLib::TErrCode   m_errcThrown;
        TString             m_strEntityName;
        TString             m_strMessage;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXMLException,TObject)
};

#pragma CIDLIB_POPPACK

