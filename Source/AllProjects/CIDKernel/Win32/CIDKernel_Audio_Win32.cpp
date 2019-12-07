//
// FILE NAME: CIDKernel_Audio.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/21/1998
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
//  This file implements the platfrom dependent parts of the TKrnlAudio
//  class.
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
#include    "CIDKernel_.hpp"
#include    "CIDKernel_InternalHelpers_.hpp"

#pragma     warning(push)
#include    <CodeAnalysis\Warnings.h>
#pragma     warning(disable : ALL_CODE_ANALYSIS_WARNINGS 26812)
#include    <Mmdeviceapi.h>
#include    <Mmsystem.h>
#include    <Endpointvolume.h>
#include    <dsound.h>
#pragma     warning(pop)


// ---------------------------------------------------------------------------
//  Force some platform libraries to be included
// ---------------------------------------------------------------------------
#pragma comment(lib, "Dsound.lib")



// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDKernel_Audio
{
    namespace
    {
        // -----------------------------------------------------------------------
        //  Local data
        //
        //  amapTable
        //      A table to map multimedia errors into our errors. It uses the
        //      standard error mapping record defined by CIDKernel.
        //
        //      <TBD> When is this to be used? Its not used now!
        //
        //  c4MajVer
        //  c4MinVer
        //      The maj/min version of the driver that is controlling this device.
        //
        //  i4DevId
        //      The id of the chosen audio device, if there is one. If not, then
        //      bAudioSupport is set to false and this value is -1.
        // -----------------------------------------------------------------------
        const tCIDKernel_::TErrMap   amapTable[] =
        {
                { MMSYSERR_ERROR        , kKrnlErrs::errcGen_GenFailure }
            ,   { MMSYSERR_BADDEVICEID  , kKrnlErrs::errcGen_InvalidDeviceId }
            ,   { MMSYSERR_NOTENABLED   , kKrnlErrs::errcGen_NotReady }
            ,   { MMSYSERR_ALLOCATED    , kKrnlErrs::errcGen_AlreadyAllocated }
            ,   { MMSYSERR_INVALHANDLE  , kKrnlErrs::errcData_InvalidHandle }
            ,   { MMSYSERR_NODRIVER     , kKrnlErrs::errcDev_NoDriverLoaded }
            ,   { MMSYSERR_NOMEM        , kKrnlErrs::errcMem_AllocError }
            ,   { MMSYSERR_NOTSUPPORTED , kKrnlErrs::errcGen_NotSupported }
            ,   { MMSYSERR_INVALFLAG    , kKrnlErrs::errcData_InvalidFlag }
            ,   { MMSYSERR_INVALPARAM   , kKrnlErrs::errcData_InvalidParameter }
            ,   { MMSYSERR_HANDLEBUSY   , kKrnlErrs::errcGen_Busy }
            ,   { MMSYSERR_INVALIDALIAS , kKrnlErrs::errcData_InvalidAlias }
            ,   { MMSYSERR_KEYNOTFOUND  , kKrnlErrs::errcGen_NotFound }
            ,   { MMSYSERR_READERROR    , kKrnlErrs::errcGen_ReadFault }
            ,   { MMSYSERR_WRITEERROR   , kKrnlErrs::errcGen_WriteFault }
            ,   { MMSYSERR_DELETEERROR  , kKrnlErrs::errcGen_DeleteError }
            ,   { MMSYSERR_VALNOTFOUND  , kKrnlErrs::errcGen_NotFound }
            ,   { MMSYSERR_NODRIVERCB   , kKrnlErrs::errcGen_NotSupported }
        };
        tCIDLib::TCard4         c4Formats;
        const tCIDLib::TCard4   c4XlatEntries = tCIDLib::c4ArrayElems(amapTable);
    }
}



// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------

// <TBD> Not used yet!
#if 0
static tCIDLib::TErrCode errcMapError(const tCIDLib::TOSErrCode errcToXlat)
{
    for (tCIDLib::TCard4 c4Index = 0;
                         c4Index < CIDKernel_Audio::c4XlatEntries; c4Index++)
    {
        if (CIDKernel_Audio::amapTable[c4Index].errcHostErr == errcToXlat)
            return CIDKernel_Audio::amapTable[c4Index].errcCIDErr;
    }
    return kKrnlErrs::errcHostErr;
}
#endif


// ---------------------------------------------------------------------------
//  A local callback method that is used in the enumeration of audio devices.
//  We define a small structure to pass in the info, which is the string to
//  format the info to and the size of that string.
// ---------------------------------------------------------------------------
struct TEnumAudDevInfo
{
    tCIDLib::TCh*   pchToFill;
    tCIDLib::TCard4 c4MaxChars;
};

INT_PTR CALLBACK
bEnumAudioDevs(GUID* pGUID, TCHAR* pszDesc, TCHAR* pszDevName, VOID* pContext)
{
    TEnumAudDevInfo* pInfo = static_cast<TEnumAudDevInfo*>(pContext);

    // Ignore the default defice
    if (!pGUID)
        return TRUE;

    // Format the GUID to a string
    constexpr tCIDLib::TCard4 c4BufSz = 127;
    tCIDLib::TCh achBuf[c4BufSz + 1];
    if (!::StringFromGUID2(*pGUID, achBuf, c4BufSz))
        return FALSE;

    if (pInfo->pchToFill[0] != kCIDLib::chNull)
        TRawStr::CatStr(pInfo->pchToFill, L"\n", pInfo->c4MaxChars);

    // Put out two lines with the name and GUID
    TRawStr::CatStr(pInfo->pchToFill, pszDesc, pInfo->c4MaxChars);
    TRawStr::CatStr(pInfo->pchToFill, L"\n", pInfo->c4MaxChars);
    TRawStr::CatStr(pInfo->pchToFill, achBuf, pInfo->c4MaxChars);

    return TRUE;
}


//
//  A helper for getting the default audio end point volume COM object, which we use for
//  getting and setting the system wide audio volume and mute.
//
static HRESULT hresGetDefAudioEndPointVol(IAudioEndpointVolume*& pToSet)
{
    const CLSID CLSID_MMDevEnum = __uuidof(MMDeviceEnumerator);
    const IID IID_IMMDevEnum = __uuidof(IMMDeviceEnumerator);
    HRESULT hRes;

    // Create a MM device enumerator which we need to get the default end point
    IMMDeviceEnumerator* pEnumerator = NULL;
    hRes = ::CoCreateInstance
    (
        CLSID_MMDevEnum, NULL, CLSCTX_ALL, IID_IMMDevEnum, (void**)&pEnumerator
    );

    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcAudio_GetDefAudioOut, hRes);
        return hRes;
    }
    TCOMJanitor<IMMDeviceEnumerator> janEnum(&pEnumerator);

    // Get the default audio end point
    IMMDevice* pMMDev = NULL;
    hRes = pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pMMDev);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcAudio_GetDefAudioOut, hRes);
        return hRes;
    }
    TCOMJanitor<IMMDevice> janMMDev(&pMMDev);

    // Use that (sigh...) to activate an end point volume object
    hRes = pMMDev->Activate
    (
        __uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (void**)&pToSet
    );
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcAudio_GetDefAudioOut, hRes);
        return hRes;
    }
    return S_OK;
}



// ---------------------------------------------------------------------------
//  TFacCIDKernel: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TCIDKrnlModule::bInitTermAudio(const tCIDLib::EInitTerm eState)
{
    if (eState == tCIDLib::EInitTerm::Initialize)
    {
    }
     else if (eState == tCIDLib::EInitTerm::Terminate)
    {
    }
    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  TKrnlAudio: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TKrnlAudio::Beep(const   tCIDLib::TCard4 c4Frequency
                                , const tCIDLib::TCard4 c4Duration)
{
    ::Beep(c4Frequency, c4Duration);
}


//
//  Mutes or unmutes the output on the default system audio output.
//
tCIDLib::TBoolean TKrnlAudio::bMuteSystemAudio(const tCIDLib::TBoolean bMute)
{
    // Call the local helper to get the volume for the default audio end point
    IAudioEndpointVolume* pAudioEPV = NULL;
    HRESULT hRes = hresGetDefAudioEndPointVol(pAudioEPV);
    if (FAILED(hRes))
        return kCIDLib::False;

    // We got it, so make sure it gets cleaned up
    TCOMJanitor<IAudioEndpointVolume> janEPV(&pAudioEPV);


    BOOL SysMute(bMute ? TRUE : FALSE);
    hRes = pAudioEPV->SetMute(SysMute, NULL);
    if (hRes != S_OK)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcAudio_SetSysMute, hRes);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  Plays a WAV file using local default WAV playback functionality, so it
//  goes the default system audio output for audio playback.
//
tCIDLib::TBoolean
TKrnlAudio::bPlayWAVFile(const  tCIDLib::TCh* const pszFileToPlay
                        , const tCIDLib::EWaitModes eWait)
{
    // Set the up basic flags
    tCIDLib::TCard4 c4Flags = SND_FILENAME | SND_NODEFAULT;

    // If they want to wait, then add in that flag
    if (eWait == tCIDLib::EWaitModes::Wait)
        c4Flags |= SND_SYNC;
    else
        c4Flags |= SND_ASYNC;

    if (!::PlaySound(pszFileToPlay, 0, c4Flags))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  Enumerates the available audio devices. It returns a string that is
//  formatted into sets of lines, like:
//
//      name1\n
//      GUID1\n
//      ....
//      nameN\n
//      GUIDN
//
//  The CIDLib layer that wraps this will use a stream to parse the info
//  out.
//
tCIDLib::TBoolean
TKrnlAudio::bEnumAudioDevices(          tCIDLib::TCh* const pszToFill
                                , const tCIDLib::TCard4     c4BufLen)
{
    //
    //  Set up our little info structure. Make sure that the output string
    //  is initially empty.
    //
    pszToFill[0] = kCIDLib::chNull;
    TEnumAudDevInfo EnumInfo;
    EnumInfo.pchToFill = pszToFill;
    EnumInfo.c4MaxChars = c4BufLen;

    // Kick off the enumeration
    HRESULT hRes = ::DirectSoundEnumerate
    (
        (LPDSENUMCALLBACK)bEnumAudioDevs, (void*)&EnumInfo
    );

    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcAudio_EnumDevs, hRes);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  Queries the system volume for the default audio output, the one used to play audio
//  by default and such.
//
tCIDLib::TBoolean TKrnlAudio::bQuerySystemVolume(tCIDLib::TCard4& c4ToFill)
{
    // Call the local helper to get the volume for the default audio end point
    IAudioEndpointVolume* pAudioEPV = NULL;
    HRESULT hRes = hresGetDefAudioEndPointVol(pAudioEPV);
    if (FAILED(hRes))
        return kCIDLib::False;

    // We got it, so make sure it gets cleaned up
    TCOMJanitor<IAudioEndpointVolume> janEPV(&pAudioEPV);

    // Get the scalar volume 0 to 1.0, and scale to our cardinal percent
    float fPerVol;
    hRes = pAudioEPV->GetMasterVolumeLevelScalar(&fPerVol);
    if (hRes != S_OK)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcAudio_QuerySysVolume, hRes);
        return kCIDLib::False;
    }

    // Scale and round to closest to avoid cutoff errors
    fPerVol *= 100.0;
    TMathLib::Round(fPerVol);

    // Then we can put it into our caller's parm and make sure no rounding up issue
    c4ToFill = tCIDLib::TCard4(fPerVol);
    if (c4ToFill > 100)
        c4ToFill = 100;
    return kCIDLib::True;
}


//
//  Sets the system volume for the default audio output, the one used to play audio
//  by default and such.
//
tCIDLib::TBoolean
TKrnlAudio::bSetSystemVolume(const tCIDLib::TCard4 c4VolPercent)
{
    // Call the local helper to get the volume for the default audio end point
    IAudioEndpointVolume* pAudioEPV = NULL;
    HRESULT hRes = hresGetDefAudioEndPointVol(pAudioEPV);
    if (FAILED(hRes))
        return kCIDLib::False;

    // We got it, so make sure it gets cleaned up
    TCOMJanitor<IAudioEndpointVolume> janEPV(&pAudioEPV);

    // Scale our percentage to his floating point scalar format
    float fPerVol = float(c4VolPercent / 100.0);
    if (fPerVol > 1.0)
        fPerVol = 1.0;

    hRes = pAudioEPV->SetMasterVolumeLevelScalar(fPerVol, NULL);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcAudio_SetSysVolume, hRes);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}

