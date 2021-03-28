//
// FILE NAME: CIDXML_FileEntitySrc.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/20/1999
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
//  This file implements the TFileEntitySrc class.
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
#include    "CIDXML_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFileEntitySrc,TXMLEntitySrc)



// ---------------------------------------------------------------------------
//  CLASS: TFileEntitySrc
// PREFIX: xsrc
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFileEntitySrc: Constructors and Destructor
// ---------------------------------------------------------------------------
TFileEntitySrc::TFileEntitySrc(const TString& strSysId) :

    TXMLEntitySrc(strSysId)
{
}

TFileEntitySrc::TFileEntitySrc( const   TString&    strParentId
                                , const TString&    strSysId) :
    TXMLEntitySrc()
{
    //
    //  Weave any relative parts of the system id into the parent id that
    //  was passed. Then set it on our parent class as the system id.
    //
    TPathStr pathSysId(strSysId);
    pathSysId.AddToBasePath(strParentId);
    strSystemId(pathSysId);
}

TFileEntitySrc::TFileEntitySrc( const   TString&    strParentId
                                , const TString&    strSysId
                                , const TString&    strPubId) :
    TXMLEntitySrc()
{
    //
    //  Weave any relative parts of the system id into the parent id that
    //  was passed. Then set it on our parent class as the system id. Since
    //  we called the default parent ctor, we have to set the public id also.
    //
    TPathStr pathSysId(strSysId);
    pathSysId.AddToBasePath(strParentId);
    strSystemId(pathSysId);
    strPublicId(strPubId);
}

TFileEntitySrc::~TFileEntitySrc()
{
}


// ---------------------------------------------------------------------------
//  TFileEntitySrc: Public, inherited methods
// ---------------------------------------------------------------------------
TBinInStream* TFileEntitySrc::pstrmMakeNew() const
{
    // Just create a binary file input stream. The file has to exist
    return new TBinFileInStream
    (
        strSystemId()
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::None
        , tCIDLib::EFileFlags::SequentialScan
    );
}

