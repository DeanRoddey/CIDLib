//
// FILE NAME: CIDSock_IPHostInfo.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/05/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TIPHostInfo class, which contains a block of
//  info that can be queried about a TCP/IP host machine.
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
#include    "CIDSock_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TIPHostInfo,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TIPHostInfo
//  PREFIX: iphi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TIPHostInfo: Constructors and Destructor
// ---------------------------------------------------------------------------

// This will return all source addresses
TIPHostInfo::TIPHostInfo() :

    m_strHostName(L"")
{
    // Call a helper to avoid replicating code
    CommonSetup(kCIDLib::True);
}

// This will return all non-loopbacks, and optionally loopbacks
TIPHostInfo::TIPHostInfo(const  TString&            strHostName
                        , const tCIDLib::TBoolean   bIncludeLoopbacks) :

    m_strHostName(strHostName)
{
    // Call a helper to avoid replicating code
    CommonSetup(bIncludeLoopbacks);
}

TIPHostInfo::~TIPHostInfo()
{
}


// ---------------------------------------------------------------------------
//  TIPHostInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TIPHostInfo::c4IPAddrCount() const
{
    // The address/alias lists must be the same size so either will do
    return m_colAddrs.c4ElemCount();
}


const TIPAddress& TIPHostInfo::ipaAt(const tCIDLib::TCard4 c4Index) const
{
    // Make sure that there are any at all
    if (m_colAddrs.bIsEmpty())
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcTCP_NoIPAddrs
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    // Check the index
    if (c4Index >= m_colAddrs.c4ElemCount())
    {
        facCIDSock().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcTCP_IPAddrIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Index)
            , TCardinal(m_colAddrs.c4ElemCount())
        );
    }
    return m_colAddrs[c4Index];
}


const TString& TIPHostInfo::strHostName() const
{
    return m_strHostName;
}



// ---------------------------------------------------------------------------
//  TIPHostInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Avoids replicating the same init code in multiple ctors
tCIDLib::TVoid TIPHostInfo::CommonSetup(const tCIDLib::TBoolean bIncludeLoopbacks)
{
    //
    //  Query the addresses for this host.
    //
    TKrnlLList<TKrnlIPAddr> llstAddrs;
    const tCIDLib::TBoolean bRes = TKrnlIP::bQueryHostAddrs
    (
        m_strHostName.pszBuffer()
        , llstAddrs
        , tCIDSock::EAddrInfoFlags::None
        , bIncludeLoopbacks
        , kCIDLib::False
    );

    if (!bRes)
    {
        facCIDSock().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kSockErrs::errcTCP_QueryHostInfo
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Flush out lists and copy over the info
    m_colAddrs.RemoveAll();

    if (llstAddrs.bResetCursor())
    {
        TKrnlIPAddr* pkipaCur;
        while (llstAddrs.bNext(pkipaCur))
            m_colAddrs.objAdd(TIPAddress(*pkipaCur));
    }
}

