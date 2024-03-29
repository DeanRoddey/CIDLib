//
// FILE NAME: CIDLib_Audio.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/04/1993
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
//  TAudio is a simple sound generating namespace that is used by CIDLib when
//  it needs to do any sound generation.
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
#include    "CIDLib_.hpp"



// ---------------------------------------------------------------------------
//  TAudio functions
// ---------------------------------------------------------------------------
tCIDLib::TVoid TAudio::AudioCue(const tCIDLib::EAudioCues eCue)
{
    if (!TKrnlAudio::bAudioCue(eCue))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcAudio_DoCue
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


// A fault tolerant WAV player. PlayWAVFile below works in terms of this
tCIDLib::TBoolean
TAudio::bPlayWAVFile(const  TString&            strFileToPlay
                    , const tCIDLib::EWaitModes eWait)
{
    return TKrnlAudio::bPlayWAVFile(strFileToPlay.pszBuffer(), eWait);
}


tCIDLib::TBoolean
TAudio::bQuerySystemVolume(         tCIDLib::TCard4&    c4ToFill
                            , const tCIDLib::TBoolean   bThrowIfNot)
{
    if (!TKrnlAudio::bQuerySystemVolume(c4ToFill))
    {
        if (bThrowIfNot)
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcAudio_QuerySystemVol
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TAudio::bSetSystemVolume(const  tCIDLib::TCard4     c4Percent
                        , const tCIDLib::TBoolean   bThrowIfNot)
{
    if (!TKrnlAudio::bSetSystemVolume(c4Percent))
    {
        if (bThrowIfNot)
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcAudio_SetSystemVol
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// Play a simple local system speaker beep, if it has one
tCIDLib::TVoid
TAudio::Beep(   const   tCIDLib::TCard4 c4Frequency
                , const tCIDLib::TCard4 c4Duration)
{
    TKrnlAudio::Beep(c4Frequency, c4Duration);
}


tCIDLib::TCard4
TAudio::c4EnumAudioDevices(tCIDLib::TKVPCollect& colToFill)
{
    tCIDLib::TCh achToFill[4096];

    achToFill[0] = kCIDLib::chNull;
    if (!TKrnlAudio::bEnumAudioDevices(achToFill, 4095))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcAudio_EnumDevices
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // It worked, set up a stream on the string and let's parse it
    TString strInfo(achToFill);
    TTextStringInStream strmSrc(tCIDLib::ForceMove(strInfo));
    colToFill.RemoveAll();

    TString strName;
    TString strID;
    while (!strmSrc.bEndOfStream())
    {
        strmSrc >> strName >> strID;
        colToFill.objAdd(TKeyValuePair(strName, strID));
    }
    return colToFill.c4ElemCount();
}


tCIDLib::TVoid TAudio::MuteSystemAudio(const tCIDLib::TBoolean bToSet)
{
    if (!TKrnlAudio::bMuteSystemAudio(bToSet))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcAudio_MuteSystem
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


tCIDLib::TVoid
TAudio::PlayWAVFile(const   TString&            strFileToPlay
                    , const tCIDLib::EWaitModes eWait)
{
    // Call the fault tolerant one above and throw if he fails
    if (!bPlayWAVFile(strFileToPlay, eWait))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcAudio_PlayWav
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strFileToPlay
        );
    }
}


tCIDLib::TVoid TAudio::SetWAVForCue(const   tCIDLib::EAudioCues eCue
                                    , const TString&            strFileForCue)
{
    //
    //  If the string is a null string, then we want to empty this cue's
    //  file entry out, so pass in a zero. Else pass in the file name.
    //
    tCIDLib::TBoolean bRes = kCIDLib::False;
    if (TString::bIsNullObject(strFileForCue))
        bRes = TKrnlAudio::bSetWAVForCue(eCue, nullptr);
    else
        bRes = TKrnlAudio::bSetWAVForCue(eCue, strFileForCue.pszBuffer());

    if (!bRes)
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(tCIDLib::c4EnumOrd(eCue))
            , TString(L"tCIDLib::EAudioCues")
        );
    }
}


TString TAudio::strDevInfo()
{
    return TString(TKrnlAudio::pszDevInfo());
}

