//
// FILE NAME: CIDResEd_BaseResData.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/26/2010
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDResEd_BaseResData.cpp file, which implements
//  some simple classes that are used by various other resource data defining
//  classes.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//   CLASS: TTextSym
//  PREFIX: tsym
//
//  We need a simple class to represent a text symbol, which has to include
//  the symbolic name and the type.
// ----------------------------------------------------------------------------
class TTextSym : public TObject, public MStreamable
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTextSym();

        TTextSym
        (
            const   TTextSym&               tsymToCopy
        );

        TTextSym
        (
            const   TString&                strSym
            , const tCIDResEd::EMsgTypes    eType
        );

        ~TTextSym();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTextSym& operator=
        (
            const   TTextSym&               tsymToAssign
        );

        tCIDLib::TBoolean operator==
        (
            const   TTextSym&               tsymToComp
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TTextSym&               tsymToComp
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsValid() const;

        tCIDResEd::EMsgTypes eType() const;

        tCIDLib::TVoid Reset();

        const TString& strRealSym() const;

        const TString& strSymName() const;

        tCIDLib::TVoid Set
        (
            const   TString&                strSym
            , const tCIDResEd::EMsgTypes    eType
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        );

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private class members
        //
        //  m_eType
        //      The type of message, i.e. message text, error text, or common.
        //
        //  m_strRealSym
        //      The real symbolic name, which may be different from what is
        //      parsed from (and written back to) the resource file. This is
        //      what is used when one of these is used to look up the text.
        //
        //  m_strSym
        //      The symbolic name as read from/written to the resource file.
        // -------------------------------------------------------------------
        tCIDResEd::EMsgTypes    m_eType;
        TString                 m_strRealSym;
        TString                 m_strSym;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TTextSym,TObject)
};


