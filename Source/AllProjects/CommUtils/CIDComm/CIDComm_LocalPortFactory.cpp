//
// FILE NAME: CIDComm_LocalPortFactory.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/12/2005
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the simple port factory derivative that supports
//  local ports.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDComm_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TLocalComPortFactory,TComPortFactory)


// ---------------------------------------------------------------------------
//  CLASS: TLocalComPortFactory
// PREFIX: pfact
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TLocalComPortFactory: Constructors and Destructor
// ---------------------------------------------------------------------------

// Tell our parent these ports are configuraable
TLocalComPortFactory::TLocalComPortFactory() :

    TComPortFactory
    (
        TFacCIDComm::strLocalFactoryId
        , TFacCIDComm::strLocalPortPrefix
        , kCIDLib::True
    )
{
}

TLocalComPortFactory::~TLocalComPortFactory()
{
}


// ---------------------------------------------------------------------------
//  TLocalComPortFactory: Public, virtual methods
// ---------------------------------------------------------------------------

//
//  We put all the ports we support into the passed collection. We don't
//  clear it first since the facility class is building up a list from all
//  the installed factories.
//
tCIDLib::TBoolean
TLocalComPortFactory::bQueryPorts(       tCIDLib::TStrCollect& colToFill
                                 , const tCIDLib::TBoolean     bAvailOnly) const
{
    // <TBD> Is there an efficient way to support the bAvailOnly flag?

    //
    //  Ask the kernel comm support for any ports in the first 64 possible
    //  com slots.
    //
    const tCIDLib::TCard4 c4ToCheck = 128;
    tCIDLib::TBoolean abFlags[c4ToCheck];
    const tCIDLib::TCard4 c4Count = TKrnlCommPort::c4EnumPorts(abFlags, c4ToCheck);

    TString strName;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ToCheck; c4Index++)
    {
        if (abFlags[c4Index])
        {
            strName = TFacCIDComm::strLocalPortPrefix;
            strName.AppendFormatted(c4Index + 1);
            colToFill.objAdd(strName);
        }
    }

    // Return true if we got any
    return (c4Count != 0);
}


// We can accept any config since we are doing real local ports
tCIDLib::TBoolean
TLocalComPortFactory::bTestCfg(const TCommPortCfg&, TString&) const
{
    return kCIDLib::True;
}


// Parse the path and see if looks synatically correct
tCIDLib::TBoolean
TLocalComPortFactory::bValidatePath(const TString& strPath) const
{
    tCIDLib::TCard4 c4Dummy;
    return bValidate(strPath, c4Dummy);
}


TCommPortBase* TLocalComPortFactory::pcommMakeNew(const TString& strPath)
{
    //
    //  Get the local comm port number out of the path and use that to create
    //  a new local comm port.
    //
    tCIDLib::TCard4 c4PortNum;
    if (!bValidate(strPath, c4PortNum))
    {
        facCIDComm().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcFac_BadPortName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , strPath
        );
    }
    return new TCommPort(strPath, c4PortNum);
}


// ---------------------------------------------------------------------------
//  TLocalComPortFactory: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  The name will be like /Local/COMXX, so we need to get that number
//  out and set that on the underlying local com port class.
//
tCIDLib::TBoolean
TLocalComPortFactory::bValidate(const   TString&            strPath
                                ,       tCIDLib::TCard4&    c4PortNum) const
{
    // If it doesn't start with our prefix, then not ours
    if (!strPath.bStartsWithI(TFacCIDComm::strLocalPortPrefix))
        return kCIDLib::False;

    tCIDLib::TBoolean bOk;
    c4PortNum = TRawStr::c4AsBinary
    (
        strPath.pszBufferAt(TFacCIDComm::strLocalPortPrefix.c4Length())
        , bOk
        , tCIDLib::ERadices::Dec
    );
    return bOk;
}


