//
// FILE NAME: CIDXML_ValidatorCore.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/30/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDXML_ValidatorCore.Cpp file, which
//  implements the TXMLValidator class. This class defines the abstract
//  API that all validators must implement.
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

class TXMLEntityMgr;
class TXMLParserCore;


// ---------------------------------------------------------------------------
//  CLASS: TXMLValidator
// PREFIX: xval
// ---------------------------------------------------------------------------
class CIDXMLEXP TXMLValidator : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        ~TXMLValidator();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bCanParseDTD() const = 0;

        virtual tCIDLib::TBoolean bIsValidRoot
        (
            const   TXMLElemDecl&           xdeclToTest
        ) = 0;

        virtual tCIDLib::TBoolean bValidateAttr
        (
            const   TXMLAttrDef&            xadToTest
            , const TString&                strValue
        ) = 0;

        virtual tCIDLib::TBoolean bValidateContent
        (
            const   TXMLElemDecl&           xdeclParent
            , const tCIDLib::TCard4* const  pc4Children
            , const tCIDLib::TCard4         c4ChildCount
        ) = 0;

        virtual TXMLElemDecl* pxdeclFindElemById
        (
            const   tCIDLib::TCard4         c4Id
        ) = 0;

        virtual const TXMLElemDecl* pxdeclFindElemById
        (
            const   tCIDLib::TCard4         c4Id
        )   const = 0;

        virtual TXMLElemDecl* pxdeclFindElemByName
        (
            const   TString&                strQName
            , const tCIDXML::EPoolOpts      eOpt
            ,       tCIDLib::TBoolean&      bAdded
        ) = 0;

        virtual const TXMLElemDecl* pxdeclFindElemByName
        (
            const   TString&                strQName
            , const tCIDXML::EPoolOpts      eOpt
            ,       tCIDLib::TBoolean&      bAdded
        )   const = 0;

        virtual TXMLEntityDecl* pxdeclFindEntityByName
        (
            const   TString&                strName
        ) = 0;

        virtual tCIDLib::TVoid ParseDefExtSS
        (
            const   TString&                strRootElem
        ) = 0;

        virtual tCIDLib::TVoid ParseDTD
        (
            const   tCIDLib::TBoolean       bIgnoreDTD
        ) = 0;

        virtual tCIDLib::TVoid ParseDTDDecl() = 0;

        virtual tCIDLib::TVoid Reset() = 0;



    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TXMLValidator
        (
                    TXMLParserCore* const   xprsOwner
        );


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        TXMLBufMgr& xbmOwner();
        TXMLEntityMgr& xemOwner();
        TXMLParserCore& xprsOwner();


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TXMLValidator(const TXMLValidator&);
        tCIDLib::TVoid operator=(const TXMLValidator&);


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pxbmBuffers
        //      This is the buffer manager of the owner parser. We get a copy
        //      out for convenience.
        //
        //  m_pxemOwner
        //      This is the entity manager that we get out of the parser. Its
        //      gotten out of the owner parser for convenience.
        //
        //  m_pxprsOwner
        //      This is the core parser that owns us. We have to do a lot
        //      of things via it. We also get some convenience pointers to
        //      some of its members. Its provide during ctor.
        // -------------------------------------------------------------------
        TXMLBufMgr*         m_pxbmOwner;
        TXMLEntityMgr*      m_pxemOwner;
        TXMLParserCore*     m_pxprsOwner;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TXMLValidator,TObject)
};

#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  TXMLValidator: Protected, non-virtual methods
// ---------------------------------------------------------------------------
inline TXMLBufMgr& TXMLValidator::xbmOwner()
{
    return *m_pxbmOwner;
}

inline TXMLEntityMgr& TXMLValidator::xemOwner()
{
    return *m_pxemOwner;
}

inline TXMLParserCore& TXMLValidator::xprsOwner()
{
    return *m_pxprsOwner;
}

