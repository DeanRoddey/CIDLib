//
// FILE NAME: UPnP1.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/11/2011
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
//  This is the main module of the program. This program demonstrates the
//  UPnP support of CIDLib. This one is very simple in terms of general
//  CIDLib program style. It just starts a local function up as the entry
//  point. No facility class.
//
//  As of a firmware release in later 2018, they once again broke the Windows
//  UPnP implementation, this time for the device properties service. So we
//  have all of that removed via a conditional (but still easy to re-try by
//  commenting the conditional out.)
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "CIDUPnP.hpp"

#if 0
#define USE_DEVPROPS = 1
#endif


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
    ,   tCIDLib::TVoid*     pData
);


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
static TInConsole       conIn(kCIDLib::True, 32);
static TOutConsole      conOut;



// ---------------------------------------------------------------------------
//  Do the magic main module code
// ---------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"UPnP1MainThread", eMainThreadFunc))


//
//  Checks that the number of tokens we got is the number of cmd parameters
//  expected plus 1 (the command is one of the tokens.)
//
//  c4TokCnt can't be less than 1, because we wouldn't have gotten called
//  if at least a command hadn't been gotten.
//
static tCIDLib::TBoolean
bCheckParmCnt(const tCIDLib::TCard4 c4TokCnt, const tCIDLib::TCard4 c4ParmsExp)
{
    if (c4TokCnt != c4ParmsExp + 1)
    {
        conOut  << L"Expected " << c4ParmsExp << L" parameters but only got "
                << (c4TokCnt - 1) << kCIDLib::EndLn;
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// Display a list of devices as returned by sync or async find
static tCIDLib::TVoid
ShowDevList(const tCIDLib::TCh* const pszTitle, tCIDLib::TKValsList& colDevs)
{
    conOut << L"\n------------------------------------------------\n"
           << pszTitle
           << L"\n------------------------------------------------\n";

    tCIDLib::TKValsList::TCursor cursList(&colDevs);
    if (!cursList.bIsValid())
    {
        conOut << L"[none found]\n";
        return;
    }

    tCIDLib::TCard4 c4Index = 1;
    for (; cursList; ++cursList)
    {
        const TKeyValues& kvalsCur = *cursList;

        conOut << c4Index << L".    " << kvalsCur.strVal1()
               << L"\n       UID: " << kvalsCur.strKey()
               << L"\n     Model: " << kvalsCur.strVal2()
               << L"\n      Type: " << kvalsCur.strVal3()
               << kCIDLib::DNewLn;

        c4Index++;
    }
    conOut << kCIDLib::EndLn;
}


static tCIDLib::TVoid
ShowSvcList(const tCIDLib::TCh* const pszTitle, const tCIDLib::TKVPList colServices)
{
    conOut << L"\n------------------------------------------------\n"
           << pszTitle
           << L"\n------------------------------------------------\n";

    tCIDLib::TKVPList::TCursor cursList(&colServices);
    if (!cursList.bIsValid())
    {
        conOut << L"[none found]\n";
        return;
    }

    for (; cursList; ++cursList)
    {
        const TKeyValuePair& kvalCur = *cursList;
        conOut << L"    " << kvalCur.strValue()
               << L"\n      UID: " << kvalCur.strKey()
               << kCIDLib::DNewLn;
    }
    conOut << kCIDLib::EndLn;
}


// Display a list of favorites we get from browsing the content directory
static tCIDLib::TVoid
ShowFavorites(  const   tCIDLib::TCard4         c4StartInd
                , const tCIDLib::TCard4         c4Total
                , const tCIDLib::TKValsList&    colLists)
{
    const tCIDLib::TCard4 c4Count = colLists.c4ElemCount();

    conOut << L"\n------------------------------------------------\n"
              L"Favorites "
           << (c4StartInd + 1)
           << L" through " << (c4StartInd + c4Count)
           << L" of " << c4Total
           << L")\n------------------------------------------------\n";

    TString strMeta;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TKeyValues& kvalsCur = colLists[c4Index];

        // Clip the metadata
        strMeta = kvalsCur.strVal2();
        if (strMeta.c4Length() > 72)
        {
            strMeta.CapAt(72);
            strMeta.Append(L"...");
        }

        conOut  << L"     Name = " << kvalsCur.strKey()
                << L"\n       Proto = " << kvalsCur.strVal1()
                << L"\n       Meta = " << strMeta
                << kCIDLib::NewEndLn;
    }
}


// Display a list of track items we get from browsing the content directory
static tCIDLib::TVoid
ShowTrackItems( const   tCIDLib::TCh* const         pszTitle
                , const tCIDLib::TCard4             c4StartInd
                , const tCIDLib::TCard4             c4Total
                , const TVector<TUPnPTrackItem>&    colTracks)
{
    const tCIDLib::TCard4 c4Count = colTracks.c4ElemCount();

    conOut << L"\n------------------------------------------------\n"
           << pszTitle << L"  (Tracks "
           << (c4StartInd + 1)
           << L" through " << (c4StartInd + c4Count)
           << L" of " << c4Total
           << L")\n------------------------------------------------\n";

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TUPnPTrackItem& upnptiCur = colTracks[c4Index];

        conOut  << L"    Title   = " << upnptiCur.strTitle()
                << L"\n      Album = " << upnptiCur.strAlbum()
                << L"\n     Artist = " << upnptiCur.strArtist()
                << L"\n    Track # = " << upnptiCur.c4TrackNum()
                << kCIDLib::NewEndLn;
    }
}


// Display a list of playlists we get from browsing the content directory
static tCIDLib::TVoid
ShowPlayLists(  const   tCIDLib::TCard4     c4StartInd
                , const tCIDLib::TCard4     c4Total
                , const tCIDLib::TKVPList&  colLists)
{
    const tCIDLib::TCard4 c4Count = colLists.c4ElemCount();

    conOut << L"\n------------------------------------------------\n"
              L"Playlists "
           << (c4StartInd + 1)
           << L" through " << (c4StartInd + c4Count)
           << L" of " << c4Total
           << L")\n------------------------------------------------\n";

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TKeyValuePair& kvalCur = colLists[c4Index];

        conOut  << L"     Name = " << kvalCur.strValue()
                << L"\n       Id = " << kvalCur.strKey()
                << kCIDLib::NewEndLn;
    }
}


// Look up a device in the passed list by type and set it up
tCIDLib::TBoolean bFindDevByType(const  tCIDLib::TKValsList&    colDevs
                                ,       TUPnPDevice&            upnpdTar
                                , const TString&                strType)
{
    tCIDLib::TKValsList::TCursor cursList(&colDevs);

    while (cursList)
    {
        if (cursList.objRCur().strVal2() == strType)
            break;
        ++cursList;
    }

    if (!cursList)
        return kCIDLib::False;
    return facCIDUPnP().bSetDeviceFromUID(cursList.objRCur().strKey(), upnpdTar);
}


// Look up a service in the passed list by type and set it up
tCIDLib::TBoolean bFindSvcByType(const  tCIDLib::TKVPList&  colServices
                                ,       TUPnPDevice&        upnpdPar
                                ,       TUPnPService&       upnpsTar
                                , const TString&            strType)
{
    tCIDLib::TKVPList::TCursor cursList(&colServices);
    while (cursList)
    {
        if (cursList->strValue() == strType)
            break;
        ++cursList;
    }

    if (!cursList)
        return kCIDLib::False;

    upnpdPar.SetServiceFromID(cursList.objRCur().strKey(), upnpsTar);
    return kCIDLib::True;
}


//
// Get a selection from 1 to the indicated max index
//
static tCIDLib::TBoolean
bGetSelection(  const   tCIDLib::TCh* const pszPrompt
                , const tCIDLib::TCard4     c4MaxIndex
                ,       tCIDLib::TCard4&    c4SelInd)
{
    TString strInp;

    // Ask them to select a codec
    tCIDLib::TBoolean bRet = kCIDLib::False;
    while(!bRet)
    {
        conOut << pszPrompt << L": " << kCIDLib::FlushIt;
        conIn >> strInp;
        conOut << kCIDLib::EndLn;

        if (strInp.bCompareI(L"exit"))
            return kCIDLib::False;

        // Should be a number, so check it, and it must be a valid one
        bRet = strInp.bToCard4(c4SelInd, tCIDLib::ERadices::Dec);
        if (bRet && (c4SelInd > 0) && (c4SelInd <= c4MaxIndex))
            break;

        conOut << L"Invalid, try again: " << kCIDLib::FlushIt;
    }

    conOut << kCIDLib::EndLn;

    // If we got a number make it a zero based index
    if (bRet)
        c4SelInd--;
    return bRet;
}


// Show our available commands
static tCIDLib::TVoid ShowCmds()
{
    conOut  << L"\n\nAvailable Commands:\n--------------------------------\n"
               L"AddPLToQ plid\n"
               L"BecomeStandaloneGrp\n"
               L"BrowseFavorites startind count\n"
               L"BrowseQueue startind count\n"
               L"BrowsePlaylist plid startind count\n"
               L"BrowsePlaylists startind count\n"
               L"BrowseTracks startind count\n"
               L"ChangePlayer\n"
               L"ClearQ\n"
               L"DeletePL plid\n"
               L"Exit\n"
               L"GetAutoplayInfo\n"
               L"GetAVStatus\n"
               L"GetAVTransURI\n"
               L"GetContUpdateIds\n"
               L"GetXfadeMode\n"
               L"GetGrpCoord\n"
               L"GetLEDState\n"
               L"GetLineLevels\n"
               L"GetMute\n"
               L"GetPosInfo\n"
               L"GetVolume\n"
               L"GetZoneInfo\n"
               L"JoinGrp [coordid]\n"
               L"Mute [on | off]\n"
               L"Next\n"
               L"Pause\n"
               L"Play\n"
               L"Previous\n"
               L"SetQToPL plnum (1, 2, 3, etc...)\n"
               L"SaveQAsPL [plname]\n"
               L"SetLEDState [off | on]\n"
               L"SetTransURI uri\n"
               L"SetXfadeMode [off | on]\n"
               L"ShowCmds\n"
               L"ShowCurZP\n"
               L"ShowMRServices\n"
               L"ShowMSServices\n"
               L"ShowZPServices\n"
               L"StartAutoplay plnum volume\n"
               L"Stop\n"

            << kCIDLib::EndLn;
}


//
//  Given the unique id of a zone player, set up all the devices and services
//  appropriately.
//
static tCIDLib::TBoolean
bInitObjects(const  TString&                strZPId
            ,       TUPnPDevice&            upnpdZP
            ,       TUPnPAudInService&      upnpsAudioIn
            ,       TUPnPDevPropsService&   upnpsDevProps
            ,       TUPnPGrpMgmtService&    upnpsGrpMgmt

            ,       TUPnPDevice&            upnpdMR
            ,       TUPnPAVTransService&    upnpsAVTrans
            ,       TUPnPRendCtrlService&   upnpsRendCtrl

            ,       TUPnPDevice&            upnpdMS
            ,       TUPnPContDirService&    upnpsContDir)
{
    // Set up a zone player object
    facCIDUPnP().SetDeviceFromUID(strZPId, upnpdZP);

    // Query the services the zone player guy has
    tCIDLib::TKVPList colZPServices;
    upnpdZP.QueryServices(colZPServices);

    // Set up an audio input service
    if (!bFindSvcByType(colZPServices, upnpdZP, upnpsAudioIn, kCIDUPnP::strSvcType_AudioIn))
    {
        conOut << L"\nZone Player had no audio in service\n" << kCIDLib::EndLn;
        return kCIDLib::False;
    }

    //
    //  Set up a device properities service.
    //
    //  THERE's a problem as of a the latest Sonos firmware, and this one breaks
    //  the WInodws UPnP implementation
    //
    #if defined(USE_DEVPROPS)
    if (!bFindSvcByType(colZPServices, upnpdZP, upnpsDevProps, kCIDUPnP::strSvcType_DeviceProperties))
    {
        conOut << L"\nZone Player had no device properities service\n" << kCIDLib::EndLn;
        return kCIDLib::False;
    }
    #endif

    // Set up a group management service, and enable eventing on it
    if (!bFindSvcByType(colZPServices, upnpdZP, upnpsGrpMgmt, kCIDUPnP::strSvcType_GroupManagement))
    {
        conOut << L"\nZone Player had no group management service\n" << kCIDLib::EndLn;
        return kCIDLib::False;
    }
    upnpdZP.SetServiceFromID
    (
        L"urn:upnp-org:serviceId:GroupManagement"
        , upnpsGrpMgmt
    );
    upnpsGrpMgmt.EnableEvents();

    // Now look up the ZP's child devices and find the media renderer one
    tCIDLib::TKValsList colChildDevs;
    upnpdZP.QueryChildDevices(colChildDevs, TString::strEmpty());

    if (!bFindDevByType(colChildDevs, upnpdMR, kCIDUPnP::strDevType_MediaRenderer))
    {
        conOut << L"\nNo media renderer child device was found\n" << kCIDLib::EndLn;
        return kCIDLib::False;
    }

    // Get a list of the media renderer's services
    tCIDLib::TKVPList colMRServices;
    upnpdMR.QueryServices(colMRServices);

    // Set up an AV transport service and enable eventing
    if (!bFindSvcByType(colMRServices, upnpdMR, upnpsAVTrans, kCIDUPnP::strSvcType_AVTransport))
    {
        conOut << L"\nZone Player had no AV transport service\n" << kCIDLib::EndLn;
        return kCIDLib::False;
    }
    upnpsAVTrans.EnableEvents();

    // Set up a renderer control service. Enable eventing
    if (!bFindSvcByType(colMRServices, upnpdMR, upnpsRendCtrl, kCIDUPnP::strSvcType_RendControl))
    {
        conOut << L"\nZone Player had no AV transport service\n" << kCIDLib::EndLn;
        return kCIDLib::False;
    }
    upnpsRendCtrl.EnableEvents();

    // Get the media server child device
    if (!bFindDevByType(colChildDevs, upnpdMS, kCIDUPnP::strDevType_MediaServer))
    {
        conOut << L"\nNo media server child device was found\n" << kCIDLib::EndLn;
        return kCIDLib::False;
    }

    // Get a list of his services
    tCIDLib::TKVPList colMSServices;
    upnpdMS.QueryServices(colMSServices);

    // Set up an content directory service
    if (!bFindSvcByType(colMSServices, upnpdMS, upnpsContDir, kCIDUPnP::strSvcType_ContentDirectory))
    {
        conOut << L"\nZone Player had no content directory service\n" << kCIDLib::EndLn;
        return kCIDLib::False;
    }
    upnpsContDir.EnableEvents();

    return kCIDLib::True;
}


//
//  Selects a player and set ups the passed services for that player. We return
//
static tCIDLib::TBoolean
bSelectZP(  TUPnPAsyncFinder&       upnpafZPs
            , TUPnPDevice&          upnpdZP
            , TUPnPAudInService&    upnpsAudioIn
            , TUPnPDevPropsService& upnpsDevProps
            , TUPnPGrpMgmtService&  upnpsGrpMgmt

            , TUPnPDevice&          upnpdMR
            , TUPnPAVTransService&  upnpsAVTrans
            , TUPnPRendCtrlService& upnpsRendCtrl

            , TUPnPDevice&          upnpdMS
            , TUPnPContDirService&  upnpsContDir)
{
    // Get the list of available zone players from the finder
    tCIDLib::TKValsList colZPlayers;
    tCIDLib::TCard4 c4SerialNum = 0;
    upnpafZPs.QueryDevList(colZPlayers, c4SerialNum);

    // Show the list of zone players to the user
    ShowDevList(L"Zone Players", colZPlayers);

    // Let him select one
    tCIDLib::TCard4 c4SelInd;
    if (!bGetSelection(L"Select a Player", colZPlayers.c4ElemCount(), c4SelInd))
        return kCIDLib::False;

    return bInitObjects
    (
        colZPlayers[c4SelInd].strKey()

        , upnpdZP
        , upnpsAudioIn
        , upnpsDevProps
        , upnpsGrpMgmt

        , upnpdMR
        , upnpsAVTrans
        , upnpsRendCtrl

        , upnpdMS
        , upnpsContDir
    );
}



//
//  This is the the thread function for the main thread. It is started by
//  the CIDLib_MainModule() macro above.
//
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    const TString strZPSearch(L"FindZonePlayers");
    try
    {
        tCIDLib::TBoolean   bRes;
        tCIDLib::TInt4      i4ContDirUID;

        //
        //  Go ahead and start a background search for all avialable
        //  zone players, even if we get a specific on on the command
        //  line. We still need to support selecting a new one later.
        //
        TUPnPAsyncFinder upnpafRoots;
        upnpafRoots.StartSearch(kCIDUPnP::strDevType_ZonePlayer, strZPSearch);

        // And this is all the devices and services we set up
        TUPnPDevice             upnpdZP;
        TUPnPAudInService       upnpsAudioIn;
        TUPnPDevPropsService    upnpsDevProps;
        TUPnPGrpMgmtService     upnpsGrpMgmt;

        TUPnPDevice             upnpdMR;
        TUPnPAVTransService     upnpsAVTrans;
        TUPnPRendCtrlService    upnpsRendCtrl;

        TUPnPDevice             upnpdMS;
        TUPnPContDirService     upnpsContDir;

        // See if we got one on the command line or need to choose one
        if (TSysInfo::c4CmdLineParmCount() > 0)
        {
            TString strZPUID = TSysInfo::strCmdLineParmAt(0);
            bRes = bInitObjects
            (
                strZPUID

                , upnpdZP
                , upnpsAudioIn
                , upnpsDevProps
                , upnpsGrpMgmt

                , upnpdMR
                , upnpsAVTrans
                , upnpsRendCtrl

                , upnpdMS
                , upnpsContDir
            );
        }
         else
        {
            // They need to select one, so wait for search to complete
            conOut << L"Searching for available zone players" << kCIDLib::EndLn;
            if (!upnpafRoots.bWaitListComplete(strZPSearch, 30000))
            {
                conOut << L"\nList completion never occurred" << kCIDLib::EndLn;
                return tCIDLib::EExitCodes::RuntimeError;
            }

            bRes = bSelectZP
            (
                upnpafRoots

                , upnpdZP
                , upnpsAudioIn
                , upnpsDevProps
                , upnpsGrpMgmt

                , upnpdMR
                , upnpsAVTrans
                , upnpsRendCtrl

                , upnpdMS
                , upnpsContDir
            );

            if (!bRes)
                return tCIDLib::EExitCodes::Normal;
        }

        // Now take commands until asked to exit
        TString astrCmd[8];
        tCIDLib::TCard4 c4InpVals;
        TString strInput;
        while (kCIDLib::True)
        {
            conOut << L"Enter a Command: " << kCIDLib::FlushIt;
            conIn >> strInput;
            conOut << kCIDLib::EndLn;

            // Tokenize the input on whitspace
            {
                strInput.StripWhitespace();
                c4InpVals = 0;
                TStringTokenizer stokCmd(&strInput, kCIDLib::szWhitespace);
                while(stokCmd.bGetNextToken(astrCmd[c4InpVals]))
                {
                    astrCmd[c4InpVals].StripWhitespace();
                    c4InpVals++;
                    if (c4InpVals == 8)
                        break;
                }
            }

            // If no input vals, then try again
            if (!c4InpVals)
                continue;

            // The first one is always the command
            const TString& strCmd = astrCmd[0];


            //
            //  And now process the command
            //
            if (strCmd.bCompareI(L"AddPLToQ"))
            {
                if (bCheckParmCnt(c4InpVals, 1))
                {
                    tCIDLib::TCard4 c4QdAt, c4NewCnt;
                    TString strURI(L"file:///jffs/settings/savedqueues.rsq#");
                    strURI.Append(astrCmd[1]);
                    upnpsAVTrans.AddURIToQueue(strURI, TString::strEmpty(), c4QdAt, c4NewCnt);

                    conOut << L"    Queued At = " << c4QdAt
                           << L"\n    New Count = " << c4NewCnt
                           << kCIDLib::NewEndLn;
                }
            }
             else if (strCmd.bCompareI(L"BecomeStandaloneGrp"))
            {
                upnpsAVTrans.BecomeStandaloneGrp();
            }
              else if (strCmd.bCompareI(L"BrowseFavorites"))
            {
                if (bCheckParmCnt(c4InpVals, 2))
                {
                    tCIDLib::TKValsList colFavs;
                    tCIDLib::TCard4 c4Total;
                    const tCIDLib::TCard4 c4StartInd = astrCmd[1].c4Val();

                    TString strIDL;
                    tCIDLib::TCard4 c4Ret = upnpsContDir.c4BrowseFavorites
                    (
                        c4StartInd
                        , astrCmd[2].c4Val()
                        , c4Total
                        , colFavs
                        , i4ContDirUID
                        , kCIDLib::False
                    );

                    // Display the list if any
                    if (c4Ret)
                    {
                        ShowFavorites(c4StartInd, c4Total, colFavs);
                    }
                     else
                    {
                        conOut << L"No favorites were returned" << kCIDLib::NewEndLn;
                    }
                }
            }
             else if (strCmd.bCompareI(L"BrowseQueue"))
            {
                if (bCheckParmCnt(c4InpVals, 2))
                {
                    TVector<TUPnPTrackItem> colTracks;
                    tCIDLib::TCard4 c4Total;
                    const tCIDLib::TCard4 c4StartInd = astrCmd[1].c4Val();

                    tCIDLib::TCard4 c4Ret = upnpsContDir.c4BrowseTracks
                    (
                        L"Q:0"
                        , c4StartInd
                        , astrCmd[2].c4Val()
                        , c4Total
                        , colTracks
                        , i4ContDirUID
                        , kCIDLib::False
                    );

                    // Display the list if any
                    if (c4Ret)
                        ShowTrackItems(L"Q Tracks", c4StartInd, c4Total, colTracks);
                    else
                        conOut << L"No tracks were returned" << kCIDLib::NewEndLn;
                }
            }
              else if (strCmd.bCompareI(L"BrowsePlaylist"))
            {
                if (bCheckParmCnt(c4InpVals, 3))
                {
                    TVector<TUPnPTrackItem> colTracks;
                    tCIDLib::TCard4 c4Total;
                    const tCIDLib::TCard4 c4StartInd = astrCmd[2].c4Val();

                    tCIDLib::TCard4 c4Ret = upnpsContDir.c4BrowseTracks
                    (
                        TStrCat(L"SQ:", astrCmd[1])
                        , c4StartInd
                        , astrCmd[3].c4Val()
                        , c4Total
                        , colTracks
                        , i4ContDirUID
                        , kCIDLib::False
                    );

                    // Display the list if any
                    if (c4Ret)
                        ShowTrackItems(L"PL Tracks", c4StartInd, c4Total, colTracks);
                    else
                        conOut << L"No playlists were returned" << kCIDLib::NewEndLn;
                }
            }
             else if (strCmd.bCompareI(L"BrowsePlaylists"))
            {
                if (bCheckParmCnt(c4InpVals, 2))
                {
                    tCIDLib::TKVPList colPLists;
                    tCIDLib::TCard4 c4Total;
                    const tCIDLib::TCard4 c4StartInd = astrCmd[1].c4Val();

                    tCIDLib::TCard4 c4Ret = upnpsContDir.c4BrowseLists
                    (
                        L"SQ:"
                        , c4StartInd
                        , astrCmd[2].c4Val()
                        , c4Total
                        , colPLists
                        , i4ContDirUID
                        , kCIDLib::False
                    );

                    // Display the list if any
                    if (c4Ret)
                        ShowPlayLists(c4StartInd, c4Total, colPLists);
                    else
                        conOut << L"No playlists were returned" << kCIDLib::NewEndLn;
                }
            }
             else if (strCmd.bCompareI(L"BrowseTracks"))
            {
                if (!bCheckParmCnt(c4InpVals, 2))
                {
                    TVector<TUPnPTrackItem> colTracks;
                    tCIDLib::TCard4 c4Total;
                    const tCIDLib::TCard4 c4StartInd = astrCmd[1].c4Val();

                    tCIDLib::TCard4 c4Ret = upnpsContDir.c4BrowseTracks
                    (
                        L"A:TRACKS"
                        , c4StartInd
                        , astrCmd[2].c4Val()
                        , c4Total
                        , colTracks
                        , i4ContDirUID
                        , kCIDLib::False
                    );

                    // Display the list if any
                    if (c4Ret)
                        ShowTrackItems(L"Repo Tracks", c4StartInd, c4Total, colTracks);
                    else
                        conOut << L"No tracks were returned" << kCIDLib::NewEndLn;
                }
            }
             else if (strCmd.bCompareI(L"ChangePlayer"))
            {
                i4ContDirUID = 0;

                upnpsAudioIn.Release();
                upnpsAVTrans.Release();
                upnpsContDir.Release();
                #if defined(USE_DEVPROPS)
                upnpsDevProps.Release();
                #endif
                upnpsGrpMgmt.Release();

                upnpdMR.Release();
                upnpdZP.Release();

                bRes = bSelectZP
                (
                    upnpafRoots

                    , upnpdZP
                    , upnpsAudioIn
                    , upnpsDevProps
                    , upnpsGrpMgmt

                    , upnpdMR
                    , upnpsAVTrans
                    , upnpsRendCtrl

                    , upnpdMS
                    , upnpsContDir
                );

                if (bRes)
                {
                    conOut  << L"Now using player '" << upnpdZP.strName()
                            << kCIDLib::NewLn << kCIDLib::EndLn;
                }
                 else
                {
                    conOut  << L"No new device selection was made. If this is "
                               L"due to a failure, then subsequent operations "
                               L"may not work until a successful device selection "
                               L"is made\n" << kCIDLib::EndLn;
                }
            }
             else if (strCmd.bCompareI(L"ClearQ"))
            {
                upnpsAVTrans.ClearQueue();
            }
             else if (strCmd.bCompareI(L"DeletePL"))
            {
                if (bCheckParmCnt(c4InpVals, 1))
                    upnpsContDir.DeleteObject(TStrCat(L"SQ:", astrCmd[1]));
            }
             else if (strCmd.bCompareI(L"Exit"))
            {
                break;
            }
             else if (strCmd.bCompareI(L"GetAutoPlayInfo"))
            {
                #if defined(USE_DEVPROPS)
                tCIDLib::TBoolean   bLinkedZones;
                tCIDLib::TCard4     c4Volume;
                TString             strRoomID;
                upnpsDevProps.QueryAutoPlay(bLinkedZones, strRoomID, c4Volume);

                conOut << L"     Linked = " << bLinkedZones
                       << L"\n     Volume = " << c4Volume
                       << L"\n     RoomID = " << strRoomID
                       << kCIDLib::NewEndLn;
                #endif
            }
             else if (strCmd.bCompareI(L"GetAVTransURI"))
            {
                TString strURI;
                upnpsAVTrans.QueryAVTransportURI(strURI);
                conOut  << L"   AV Transport URI = " << strURI
                        << kCIDLib::NewEndLn;
            }
             else if (strCmd.bCompareI(L"GetAVStatus"))
            {
                tCIDLib::TCard4         c4Speed;
                tCIDUPnP::ETransStates  eState;
                TString                 strStatus;
                upnpsAVTrans.QueryTransportInfo(eState, strStatus, c4Speed);

                conOut  << L"   AVStatus = " << strStatus
                        << L"\n      State = " << eState
                        << L"\n      Speed = " << c4Speed
                        << kCIDLib::NewEndLn;
            }
             else if (strCmd.bCompareI(L"GetContUpdateIds"))
            {
                TString strVal;
                upnpsContDir.QueryContUpdateID(strVal);
                conOut << L"   New Changes: " << strVal << kCIDLib::NewEndLn;
            }
             else if (strCmd.bCompareI(L"GetGrpCoord"))
            {
                TString strCoordID;
                TString strGroupID;

                upnpsGrpMgmt.QueryGrpCoordinator(strCoordID, strGroupID);
                conOut << L"  Grp Coord = " << strCoordID
                       << L"\n     Grp Id = " << strGroupID;

                conOut << L"\n   Is Local = ";
                if (upnpsGrpMgmt.bCoordIsLocal())
                     conOut << L"Yes";
                else
                     conOut << L"No";
                conOut << kCIDLib::NewEndLn;
            }
             else if (strCmd.bCompareI(L"GetLEDState"))
            {
                #if defined(USE_DEVPROPS)
                conOut << L"  LED State = ";
                if (upnpsDevProps.bLEDState())
                    conOut << L"On";
                else
                    conOut << L"Off";
                conOut << kCIDLib::NewEndLn;
                #endif
            }
             else if (strCmd.bCompareI(L"GetLineLevels"))
            {
                tCIDLib::TCard4 c4LLvl, c4RLvl;
                upnpsAudioIn.QueryLineLevels(c4LLvl, c4RLvl);
                conOut  << L"    Left Lvl = " << c4LLvl
                        << L", Right Lvl = " << c4RLvl
                        << kCIDLib::NewEndLn;
            }
             else if (strCmd.bCompareI(L"GetMute"))
            {
                conOut  << L"   Mute Is: ";
                if (upnpsRendCtrl.bQueryMute())
                    conOut << L"On";
                else
                    conOut << L"Off";
                conOut << kCIDLib::NewEndLn;
            }
             else if (strCmd.bCompareI(L"GetPosInfo"))
            {
                tCIDLib::TEncodedTime   enctDuration;
                tCIDLib::TEncodedTime   enctCurTime;
                tCIDLib::TCard4         c4TrackNum;
                tCIDLib::TCard4         c4PerComplete;
                TString                 strTrackURI;
                TUPnPTrackItem          upnptiMeta;

                upnpsAVTrans.QueryPositionInfo
                (
                    strTrackURI
                    , enctDuration
                    , c4TrackNum
                    , enctCurTime
                    , c4PerComplete
                    , &upnptiMeta
                );

                TTime tmCur(enctCurTime);
                tmCur.strDefaultFormat(TTime::strMediaTime());
                TTime tmDur(enctDuration);
                tmDur.strDefaultFormat(TTime::strMediaTime());

                conOut << L"  Track URI = " << strTrackURI
                       << L"\n    Cur Time = " << tmCur
                       << L"\n    Duration = " << tmDur
                       << L"\n     Percent = " << c4PerComplete
                       << L"\n   Track Num = " << c4TrackNum
                       << L"\n      Artist = " << upnptiMeta.strArtist()
                       << L"\n       Album = " << upnptiMeta.strAlbum()
                       << L"\n  Track Name = " << upnptiMeta.strTitle()

                       << kCIDLib::NewEndLn;
            }
             else if (strCmd.bCompareI(L"GetVolume"))
            {
                conOut  << L"   Volume Is: " << upnpsRendCtrl.c4QueryVolume()
                        << kCIDLib::NewEndLn;
            }
             else if (strCmd.bCompareI(L"GetZoneAttrs"))
            {
                #if defined(USE_DEVPROPS)
                TString strZoneName;
                TString strCurIcon;
                upnpsDevProps.QueryZoneAttrs(strZoneName,  strCurIcon);

                conOut << L"   Zone Name = " << strZoneName
                       << L"\n    Cur Icon = " << strCurIcon
                       << kCIDLib::NewEndLn;
                #endif
            }
             else if (strCmd.bCompareI(L"GetZoneInfo"))
            {
                #if defined(USE_DEVPROPS)
                TString strSerialNum;
                TString strIPAddress;
                TString strMacAddress;
                TString strSoftwareVer;

                upnpsDevProps.QueryZoneInfo
                (
                    strSerialNum
                    ,  strIPAddress
                    ,  strMacAddress
                    ,  strSoftwareVer
                );

                conOut << L"  SerialNum = " << strSerialNum
                       << L"\n      IPAddr = " << strIPAddress
                       << L"\n    Mac Addr = " << strMacAddress
                       << L"\n    Soft Ver = " << strSoftwareVer
                       << kCIDLib::NewEndLn;
                #endif
            }
             else if (strCmd.bCompareI(L"GetXfadeMode"))
            {
                conOut << L"    Crossfade is ";
                if (upnpsAVTrans.bCrossfadeMode())
                    conOut << L"On";
                else
                    conOut << L"Off";
                conOut << kCIDLib::NewEndLn;
            }
             else if (strCmd.bCompareI(L"JoinGrp"))
            {
                if (bCheckParmCnt(c4InpVals, 1))
                {
                    upnpsAVTrans.SetAVTransportURI
                    (
                        TStrCat(L"x-rincon:", astrCmd[1]), TString::strEmpty()
                    );
                }
            }
             else if (strCmd.bCompareI(L"Mute"))
            {
                if (bCheckParmCnt(c4InpVals, 1))
                    upnpsRendCtrl.SetMute(astrCmd[1] == L"on");
            }
             else if (strCmd.bCompareI(L"Next"))
            {
                upnpsAVTrans.Next();
            }
             else if (strCmd.bCompareI(L"Pause"))
            {
                upnpsAVTrans.Pause();
            }
             else if (strCmd.bCompareI(L"Play"))
            {
                upnpsAVTrans.Play();
            }
             else if (strCmd.bCompareI(L"Previous"))
            {
                upnpsAVTrans.Previous();
            }
             else if (strCmd.bCompareI(L"SaveQAsPL"))
            {
                if (bCheckParmCnt(c4InpVals, 1))
                {
                    TString strAssignedID;
                    upnpsAVTrans.SaveQAsPlaylist(astrCmd[1], strAssignedID);

                    conOut  << L" New list ID is " << strAssignedID
                            << kCIDLib::NewEndLn;
                }
            }
             else if (strCmd.bCompareI(L"SetLEDState"))
            {
                #if defined(USE_DEVPROPS)
                if (bCheckParmCnt(c4InpVals, 1))
                {
                    if (astrCmd[1].bCompareI(L"on"))
                        upnpsDevProps.bLEDState(kCIDLib::True);
                    else
                        upnpsDevProps.bLEDState(kCIDLib::False);
                }
                #endif
            }
             else if (strCmd.bCompareI(L"SetQToPL"))
            {
                if (bCheckParmCnt(c4InpVals, 1))
                {
                    upnpsAVTrans.ClearQueue();

                    tCIDLib::TCard4 c4QdAt, c4NewCnt;
                    upnpsAVTrans.AddURIToQueue
                    (
                        TStrCat(L"file:///jffs/settings/savedqueues.rsq#", astrCmd[1])
                        , TString::strEmpty()
                        , c4QdAt
                        , c4NewCnt
                    );

                    conOut << L"    Queued At = " << c4QdAt
                           << L"\n    New Count = " << c4NewCnt
                           << kCIDLib::NewEndLn;
                }
            }
             else if (strCmd.bCompareI(L"SetTransURI"))
            {
                if (bCheckParmCnt(c4InpVals, 1))
                {
                    upnpsAVTrans.SetAVTransportURI
                    (
                        astrCmd[1]
                        , L"Bubba Jones"
                        , L"Bubba Goes to Washington"
                        , L"Bubba's Sex Scandal"
                        , 1
                        , 2001
                    );
                }
            }
             else if (strCmd.bCompareI(L"SetXfadeMode"))
            {
                if (bCheckParmCnt(c4InpVals, 1))
                    upnpsAVTrans.bCrossfadeMode(astrCmd[1] == L"on");
            }
             else if (strCmd.bCompareI(L"ShowChildDevices"))
            {
                tCIDLib::TKValsList colChildDevs;
                upnpdZP.QueryChildDevices(colChildDevs, TString::strEmpty());
                ShowDevList(L"Child Devices", colChildDevs);
            }
             else if (strCmd.bCompareI(L"ShowCmds") || strCmd.bCompareI(L"?"))
            {
                ShowCmds();
            }
             else if (strCmd.bCompareI(L"ShowCurZP"))
            {
                #if defined(USE_DEVPROPS)
                TString strZoneName;
                TString strCurIcon;
                upnpsDevProps.QueryZoneAttrs(strZoneName, strCurIcon);

                conOut  << L"    Zone Name = " << strZoneName
                        << L"\n       ZP UID = " << upnpdZP.strUID()
                        << kCIDLib::NewEndLn;
                #endif
            }
             else if (strCmd.bCompareI(L"ShowZPServices"))
            {
                tCIDLib::TKVPList colZPServices;
                upnpdZP.QueryServices(colZPServices);
                ShowSvcList(L"Zone Player Services", colZPServices);
            }
             else if (strCmd.bCompareI(L"ShowMRServices"))
            {
                tCIDLib::TKVPList colMRServices;
                upnpdMR.QueryServices(colMRServices);
                ShowSvcList(L"Media Renderer Services", colMRServices);
            }
             else if (strCmd.bCompareI(L"ShowMSServices"))
            {
                tCIDLib::TKVPList colMSServices;
                upnpdMS.QueryServices(colMSServices);
                ShowSvcList(L"Media Server Services", colMSServices);
            }
             else if (strCmd.bCompareI(L"StartAutoplay"))
            {
                if (bCheckParmCnt(c4InpVals, 2))
                {
                    upnpsAVTrans.StartAutoplay
                    (
                        TStrCat(L"file:///jffs/settings/savedqueues.rsq#", astrCmd[1])
                        , kCIDLib::True
                        , kCIDLib::False
                        , astrCmd[2].c4Val()
                    );
                }
            }
             else if (strCmd.bCompareI(L"Stop"))
            {
                upnpsAVTrans.Stop();
            }
             else
            {
                conOut << L"I don't know that command\n" << kCIDLib::EndLn;
            }
        }

        upnpsAudioIn.Release();
        upnpsAVTrans.Release();
        upnpsContDir.Release();
        #if defined(USE_DEVPROPS)
        upnpsDevProps.Release();
        #endif
        upnpsGrpMgmt.Release();

        upnpdMR.Release();
        upnpdZP.Release();

        conOut << kCIDLib::EndLn;
    }

    // Catch any CIDLib runtime errors
    catch(TError& errToCatch)
    {
        // Log it if not already logged.
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        conOut  <<  L"A CIDLib runtime error occured during processing.\n"
                <<  L"Error: " << errToCatch.strErrText() << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::RuntimeError;
    }

    //
    //  Kernel errors should never propogate out of CIDLib, but I test
    //  for them in my demo programs so I can catch them if they do
    //  and fix them.
    //
    catch(const TKrnlError& kerrToCatch)
    {
        conOut  << L"A kernel error occured during processing.\nError="
                << kerrToCatch.errcId() << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        conOut  << L"A general exception occured during processing"
                << kCIDLib::NewEndLn;
        return tCIDLib::EExitCodes::SystemException;
    }

    return tCIDLib::EExitCodes::Normal;
}

