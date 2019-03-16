//
// FILE NAME: CIDIDL_CodeGen.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/09/2000
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
//  This file implements the high level code generation portion of the IDL
//  compiler. It calls private methods in other files to generate the
//  individual files.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDIDL.hpp"



// ---------------------------------------------------------------------------
//  TFacCIDIDL: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This method is called to generate the client proxy class header. It parses
//  through the XML tree and calls all the installed code generators, as
//  required, to spit out the code.
//
tCIDLib::TVoid
TFacCIDIDL::GenClient(          TCodeGenerator&         cgenTarget
                        , const TXMLTreeElement* const  pxtnodeConsts
                        , const TXMLTreeElement* const  pxtnodeTypes
                        , const TXMLTreeElement* const  pxtnodeMethods)
{
    // Tell the generator that we are starting the client
    cgenTarget.BeginClientIntf
    (
        m_strClientBase
        , m_strClientOutputDir
        , m_strClientFacName
        , m_strClientOptName
        , m_strCExportKeyword
        , m_bPrivHeader
    );

    //
    //  Tell it to start constants. We always have one, which is the interface
    //  id, and might have others.
    //
    cgenTarget.BeginConstants();
    cgenTarget.GenConstant
    (
        L"strInterfaceId"
        , L"TString"
        , m_strInterfaceId
        , TString::strEmpty()
    );

    if (pxtnodeConsts)
        GenConstants(cgenTarget, *pxtnodeConsts);

    if (pxtnodeTypes)
        GenTypes(cgenTarget, *pxtnodeTypes);

    // And that methods are beginning
    cgenTarget.BeginMethods();

    // We need a parameter list to pass in parms
    tCIDIDL::TParmList colParms;

    // Now iterate the methods and write them out one by one
    TCGenTypeInfo tinfoRet;
    const tCIDLib::TCard4 c4MethodCount = pxtnodeMethods->c4ChildCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4MethodCount; c4Index++)
    {
        // Get the current child element
        const TXMLTreeElement& xtnodeMethod
                            = pxtnodeMethods->xtnodeChildAtAsElement(c4Index);
        AssertElemName2(xtnodeMethod, L"CIDIDL:Method", L"CIDIDL:PollMethod");

        const TString& strMethodName = xtnodeMethod.xtattrNamed(L"CIDIDL:Name").strValue();

        //
        //  Get the return type info, which is in the 0th child, and set his type info
        //  on the type info object.
        //
        tinfoRet.Set(xtnodeMethod.xtnodeChildAtAsElement(0).xtnodeChildAtAsElement(0));

        // And the parameters, if any, which start in the 1st child
        colParms.RemoveAll();
        const tCIDLib::TCard4 c4ParmCount = xtnodeMethod.c4ChildCount();
        for (tCIDLib::TCard4 c4Index = 1; c4Index < c4ParmCount; c4Index++)
        {
            colParms.objAdd
            (
                TCGenMethodParm
                (
                    strMethodName
                    , xtnodeMethod.xtnodeChildAtAsElement(c4Index)
                )
            );
        }

        cgenTarget.GenMethod
        (
            strMethodName
            , tinfoRet
            , colParms
            , xtnodeMethod.strQName() == L"CIDIDL:PollMethod"
            , xtnodeMethod.xtattrNamed(L"CIDIDL:TimeOut").c4ValueAs()
            , xtnodeMethod.xtattrNamed(L"CIDIDL:InBaseClass").strValue() == L"Yes"
            , c4Index
        );
    }

    // Tell the generate that methods are ending, then the interface
    cgenTarget.EndMethods();
    cgenTarget.EndClientIntf();
}


//
//  If the command line parms indicated that we should parse a CIDIDL file
//  and generate code, then this method is called. We try to parse the input
//  file, and if that works, we try to generate the code based on the cmd
//  line parms that tell us what types of code to generate and the data in
//  the input CIDIDL file.
//
tCIDLib::TVoid TFacCIDIDL::GenCode()
{
    //
    //  The input XML is now in tree form in the tree parser member. So lets
    //  get the root element out, which is where we start our travels.
    //
    const TXMLTreeElement& xtnodeRoot = m_xtprsInput.xtdocThis().xtnodeRoot();
    AssertElemName(xtnodeRoot, L"CIDIDL:Interface")

    // Create a Cpp code generator and do the code gen on it
    {
        TCppGenerator cgenCpp;

        // Tell this guy to prep for output
        cgenCpp.BeginCodeGen();

        // And now loop through our children and process them
        const tCIDLib::TCard4 c4Count = xtnodeRoot.c4ChildCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            const TXMLTreeElement& xtnodeCur
            (
                xtnodeRoot.xtnodeChildAtAsElement(c4Index)
            );

            if (xtnodeCur.strQName() == L"CIDIDL:Globals")
            {
                if (m_bGenGlobals)
                {
                    GenGlobals
                    (
                        cgenCpp
                        , xtnodeCur
                        , m_strGlobalsOutputDir
                        , m_strGlobalsFacName
                        , m_strGlobalsName
                        , m_strGExportKeyword
                    );
                }
            }
             else if (xtnodeCur.strQName() == L"CIDIDL:ClassIntf")
            {
                GenInterface(cgenCpp, xtnodeCur);
            }
             else
            {
                // Don't know what this is so complain
                facCIDIDL.GenErr
                (
                    CID_FILE
                    , CID_LINE
                    , kIDLErrs::errcInp_ExpectedElem
                    , TString(L"CIDIDL:Globals | CIDIDL:ClassIntf")
                    , xtnodeCur.strQName()
                );
            }
        }

        // Tell this guy to wrap up now
        cgenCpp.EndCodeGen();
    }
}


//
//  This guy is called if globals (constants, types, etc... not related to the
//  class and generated into their own header) are being generated and a
//  constants block is seen. We get the node that holds all the constants
//  elements, and the target code generator.
//
tCIDLib::TVoid
TFacCIDIDL::GenConstants(       TCodeGenerator&     cgenTarget
                        , const TXMLTreeElement&    xtnodeConsts)
{
    cgenTarget.BeginConstants();

    // Each child is a constant value element, which we just spit out
    const tCIDLib::TCard4 c4Count = xtnodeConsts.c4ChildCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TXMLTreeElement& xtnodeCur = xtnodeConsts.xtnodeChildAtAsElement(c4Index);
        const TString& strType = xtnodeCur.xtattrNamed(L"CIDIDL:Type").strValue();
        const TString& strValue = xtnodeCur.xtattrNamed(L"CIDIDL:Value").strValue();
        const TString& strName = xtnodeCur.xtattrNamed(L"CIDIDL:Name").strValue();
        const TString* pstrDocs = &TString::strEmpty();

        // If there's a child node, it's the docs so get a pointer
        if (xtnodeCur.c4ChildCount())
        {
            const TXMLTreeElement& xtnodeDocs = xtnodeCur.xtnodeChildAtAsElement(0);
            pstrDocs = &xtnodeDocs.xtnodeChildAtAsText(0).strText();
        }
        cgenTarget.GenConstant(strName, strType, strValue, *pstrDocs);
    }

    // Tell the generator that constants are done
    cgenTarget.EndConstants();
}


tCIDLib::TVoid
TFacCIDIDL::GenEnum(        TCodeGenerator&     cgenTarget
                    , const TXMLTreeElement&    xtnodeEnum)
{
    //
    //  See if we have a documentation node first, and if so get the pointer
    //  to the text and bump the index so that we start at child 1 below.
    //
    const TString* pstrDocs = &TString::strEmpty();
    tCIDLib::TCard4 c4Index = 0;
    if (xtnodeEnum.c4ChildCount())
    {
        const TXMLTreeElement& xtnodeDoc= xtnodeEnum.xtnodeChildAtAsElement(c4Index);
        if (xtnodeDoc.strQName() == L"CIDIDL:DocText")
        {
            pstrDocs = &xtnodeDoc.xtnodeChildAtAsText(0).strText();
            c4Index++;
        }
    }

    // Create an enum info object and let it parse the info in
    TCGenEnumInfo einfoGen(xtnodeEnum);

    // And let the generator generate the enum
    cgenTarget.GenEnum(*pstrDocs, einfoGen);
}


//
//  If we are generating code, and the CIDIDL file has a Globals section,
//  we go through that info and output a global header file (i.e. stuff not
//  put into either the client proxy or server stub files.)
//
tCIDLib::TVoid
TFacCIDIDL::GenGlobals(         TCodeGenerator&     cgenTarget
                        , const TXMLTreeElement&    xtnodeGlobals
                        , const TString&            strOutDir
                        , const TString&            strFacName
                        , const TString&            strNameExt
                        , const TString&            strExpKeyword)
{
    // Tell the generator that it's starting globals
    cgenTarget.BeginGlobals
    (
        strOutDir
        , strFacName
        , strNameExt
        , strExpKeyword
        , m_bPrivHeader
    );

    const tCIDLib::TCard4 c4Count = xtnodeGlobals.c4ChildCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TXMLTreeElement& xtnodeCur = xtnodeGlobals.xtnodeChildAtAsElement(c4Index);

        if (xtnodeCur.strQName() == L"CIDIDL:Constants")
        {
            GenConstants(cgenTarget, xtnodeCur);
        }
         else if (xtnodeCur.strQName() == L"CIDIDL:Types")
        {
            GenTypes(cgenTarget, xtnodeCur);
        }
         else
        {
            // Don't know what this is so complain
            GenErr
            (
                CID_FILE
                , CID_LINE
                , kIDLErrs::errcInp_ExpectedElem
                , TString(L"CIDIDL:Constants | CIDIDL:Types")
                , xtnodeCur.strQName()
            );
        }
    }

    // Tell the generator that globals are done
    cgenTarget.EndGlobals();
}


tCIDLib::TVoid
TFacCIDIDL::GenInterface(       TCodeGenerator&     cgenTarget
                        , const TXMLTreeElement&    xtnodeIntf)
{
    // Tell the generator to prep for interface output
    cgenTarget.BeginIntf
    (
        xtnodeIntf.xtattrNamed(L"CIDIDL:Name").strValue()
        , xtnodeIntf.xtattrNamed(L"CIDIDL:InterfaceId").strValue()
    );

    // Store the base classes and interface id
    m_strClientBase = xtnodeIntf.xtattrNamed(L"CIDIDL:ClientBase").strValue();
    m_strServerBase = xtnodeIntf.xtattrNamed(L"CIDIDL:ServerBase").strValue();
    m_strInterfaceId = xtnodeIntf.xtattrNamed(L"CIDIDL:InterfaceId").strValue();

    //
    //  Loop through the child nodes and store away those that we find. We
    //  can have one for constants, one for constructors and one for methods.
    //
    const TXMLTreeElement* pxtnodeConsts = nullptr;
    const TXMLTreeElement* pxtnodeCtor = nullptr;
    const TXMLTreeElement* pxtnodeTypes = nullptr;
    const TXMLTreeElement* pxtnodeMethods = nullptr;

    const tCIDLib::TCard4 c4Count = xtnodeIntf.c4ChildCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TXMLTreeElement& xtnodeCur = xtnodeIntf.xtnodeChildAtAsElement(c4Index);

        if (xtnodeCur.strQName() == L"CIDIDL:Constants")
            pxtnodeConsts = &xtnodeCur;
        else if (xtnodeCur.strQName() == L"CIDIDL:Ctor")
            pxtnodeCtor = &xtnodeCur;
        else if (xtnodeCur.strQName() == L"CIDIDL:Methods")
            pxtnodeMethods = &xtnodeCur;
        else if (xtnodeCur.strQName() == L"CIDIDL:Types")
            pxtnodeTypes = &xtnodeCur;
        else
        {
            // Don't know what this is
            facCIDIDL.GenErr
            (
                CID_FILE
                , CID_LINE
                , kIDLErrs::errcInp_ExpectedElem
                , TString(L"CIDIDL:Constants | CIDIDL:Ctors | CIDIDL:Methods")
                , xtnodeCur.strQName()
            );
        }
    }

    //
    //  If there was a client output command line param, then we should output
    //  the client stuff.
    //
    if (m_bGenClient)
        GenClient(cgenTarget, pxtnodeConsts, pxtnodeTypes, pxtnodeMethods);

    if (m_bGenServer)
        GenServer(cgenTarget, pxtnodeConsts, pxtnodeTypes, pxtnodeCtor, pxtnodeMethods);

    // Tell the generator that interface output is done
    cgenTarget.EndIntf();
}


//
//  This method is called to generate the server stub class. It parses
//  through the XML tree and calls all the installed code generators, as
//  required, to spit out the code.
//
tCIDLib::TVoid
TFacCIDIDL::GenServer(          TCodeGenerator&         cgenTarget
                        , const TXMLTreeElement* const  pxtnodeConsts
                        , const TXMLTreeElement* const  pxtnodeTypes
                        , const TXMLTreeElement* const  pxtnodeCtor
                        , const TXMLTreeElement* const  pxtnodeMethods)
{
    // Build up a list of ctor parms
    tCIDIDL::TParmList colCtorParms;
    if (pxtnodeCtor)
    {
        const tCIDLib::TCard4 c4ParmCount = pxtnodeCtor->c4ChildCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ParmCount; c4Index++)
        {
            const TXMLTreeElement& xtnodeCur = pxtnodeCtor->xtnodeChildAtAsElement(c4Index);
            colCtorParms.objAdd(TCGenMethodParm(L"Ctor", xtnodeCur));
        }
    }

    // Tell the generator that we are starting the client
    cgenTarget.BeginServerIntf
    (
        m_strServerBase
        , m_strServerOutputDir
        , m_strServerFacName
        , m_strServerOptName
        , m_strSExportKeyword
        , colCtorParms
        , m_bPrivHeader
    );

    //
    //  Tell it to start constants. We always have one, which is the interface
    //  id, and might have others.
    //
    cgenTarget.BeginConstants();
    cgenTarget.GenConstant
    (
        L"strInterfaceId"
        , L"TString"
        , m_strInterfaceId
        , TString::strEmpty()
    );

    if (pxtnodeConsts)
        GenConstants(cgenTarget, *pxtnodeConsts);

    if (pxtnodeTypes)
        GenTypes(cgenTarget, *pxtnodeTypes);

    // And that methods are beginning
    cgenTarget.BeginMethods();

    // We need a parameter list to pass in parms
    tCIDIDL::TParmList colParms;

    // Now iterate the methods and write them out one by one
    TCGenTypeInfo tinfoRet;
    const tCIDLib::TCard4 c4MethodCount = pxtnodeMethods->c4ChildCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4MethodCount; c4Index++)
    {
        // Get the current child element
        const TXMLTreeElement& xtnodeMethod
                            = pxtnodeMethods->xtnodeChildAtAsElement(c4Index);
        AssertElemName2(xtnodeMethod, L"CIDIDL:Method", L"CIDIDL:PollMethod");
        const TString& strMethodName = xtnodeMethod.xtattrNamed(L"CIDIDL:Name").strValue();

        // Get the return info, which is in the 0th child of the return type
        tinfoRet.Set(xtnodeMethod.xtnodeChildAtAsElement(0).xtnodeChildAtAsElement(0));

        // And the parms, if any, which start in the 1st child
        colParms.RemoveAll();
        const tCIDLib::TCard4 c4ParmCount = xtnodeMethod.c4ChildCount();
        for (tCIDLib::TCard4 c4Index = 1; c4Index < c4ParmCount; c4Index++)
        {
            TCGenMethodParm& mparmCur = colParms.objAdd
            (
                TCGenMethodParm
                (
                    strMethodName
                    , xtnodeMethod.xtnodeChildAtAsElement(c4Index)
                )
            );

            // There are some illegal options
            if ((mparmCur.tinfoThis().strType() == L"CIDIDL:TMemBuf")
            &&  (mparmCur.eDir() != tCIDLib::EParmDirs::In))
            {
                // TMemBuf is an astract type so only valid on input only parameters
                facCIDIDL.GenErr
                (
                    CID_FILE
                    , CID_LINE
                    , kIDLErrs::errcInp_InOnly
                    , mparmCur.tinfoThis().strType()
                );
            }
        }

        cgenTarget.GenMethod
        (
            strMethodName
            , tinfoRet
            , colParms
            , xtnodeMethod.strQName() == L"CIDIDL:PollMethod"
            , xtnodeMethod.xtattrNamed(L"CIDIDL:TimeOut").c4ValueAs()
            , xtnodeMethod.xtattrNamed(L"CIDIDL:InBaseClass").strValue() == L"Yes"
            , c4Index
        );
    }

    // Tell the generate that methods are ending, then the interface
    cgenTarget.EndMethods();
    cgenTarget.EndServerIntf();
}


//
//  This is called when we see a structure in the types section.
//
tCIDLib::TVoid
TFacCIDIDL::GenStruct(TCodeGenerator& cgenTarget, const TXMLTreeElement& xtnodeStruct)
{
}


//
//  This is called when we are doing globals generation and see a types block.
//  It will iterate through the types and pass the data to the provided code
//  geneator.
//
tCIDLib::TVoid
TFacCIDIDL::GenTypes(        TCodeGenerator&    cgenTarget
                    , const TXMLTreeElement&    xtnodeTypes)
{
    // Tell the generator to get ready for global types
    cgenTarget.BeginTypes();

    // Each child is a type of some sort
    const tCIDLib::TCard4 c4Count = xtnodeTypes.c4ChildCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TXMLTreeElement& xtnodeCur = xtnodeTypes.xtnodeChildAtAsElement(c4Index);

        if (xtnodeCur.strQName() == L"CIDIDL:Enum")
        {
            GenEnum(cgenTarget, xtnodeCur);
        }
         else if (xtnodeCur.strQName() == L"CIDIDL:Struct")
        {
            GenStruct(cgenTarget, xtnodeCur);
        }
         else
        {
            // Don't know what this is so complain
            facCIDIDL.GenErr
            (
                CID_FILE
                , CID_LINE
                , kIDLErrs::errcInp_ExpectedElem
                , TString(L"CIDIDL:Enums")
                , xtnodeCur.strQName()
            );
        }
    }

    // And tell the generator that global types are done
    cgenTarget.EndTypes();
}


