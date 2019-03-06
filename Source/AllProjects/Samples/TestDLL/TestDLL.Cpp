//
// FILE NAME: CIDDAE_WMAEncoder.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/21/2006
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements WMA encoder derivative of the base encoder class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Force inclusion of system libraries we need
// ---------------------------------------------------------------------------
#pragma     comment(lib, "ole32.lib")
#pragma     comment(lib, "Wmvcore.lib")


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ---------------------------------------------------------------------------
//  We need some windows stuff and some of our platform helpers
// ---------------------------------------------------------------------------
#include    <windows.h>
#include    <wmsdk.h>
#include    <vfw.h>



// ---------------------------------------------------------------------------
//  Local types and data
// ---------------------------------------------------------------------------
namespace CIDDAE_WMAEncoder
{
    // -----------------------------------------------------------------------
    //  We use an internal structure to hold the WMA specific info, so that
    //  we don't expose the Windows headers to the world.
    // -----------------------------------------------------------------------
    struct TWMAInfo
    {
        WM_MEDIA_TYPE   MediaType;
        WAVEFORMATEX    WaveFmt;
        IWMWriter*      pWriter;
        IWMProfile*     pProfile;
        QWORD           c8CurTime;
    };
}


// ---------------------------------------------------------------------------
//  Macro for convenient release of COM objects, and a simple COM object
//  janitor to allow us to create them in a scope and insure they are cleaned
//  up.
// ---------------------------------------------------------------------------
#define SafeCOMFree(p) \
if (p) \
{ \
   p->Release(); \
   p = 0; \
}

template <class T> class TCOMJanitor
{
    public :
        TCOMJanitor(T* pPtr) :
            m_pPtr(pPtr)
        {
        }

        ~TCOMJanitor()
        {
            if (m_pPtr)
                m_pPtr->Release();
        }

        T* pobjRelease()
        {
            T* pRet = m_pPtr;
            m_pPtr = 0;
            return pRet;
        }

        T* m_pPtr;
};


tCIDLib::EExitCodes eTestThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    // Allocate and initialize our internal data structure
    CIDDAE_WMAEncoder::TWMAInfo WMAInfo = {0};

    //
    //  Pre-fill in the wave format sine we always use the same source format
    //  which is CD style PCM data.
    //
    WMAInfo.WaveFmt.wFormatTag = WAVE_FORMAT_PCM;
    WMAInfo.WaveFmt.nChannels = 2;
    WMAInfo.WaveFmt.nSamplesPerSec = 44100;
    WMAInfo.WaveFmt.nAvgBytesPerSec = 176400;
    WMAInfo.WaveFmt.nBlockAlign = 4;
    WMAInfo.WaveFmt.wBitsPerSample = 16;
    WMAInfo.WaveFmt.cbSize = 0;

    // And the same for the media type structure
    WMAInfo.MediaType.majortype            = WMMEDIATYPE_Audio;
    WMAInfo.MediaType.subtype              = WMMEDIASUBTYPE_PCM;
    WMAInfo.MediaType.bFixedSizeSamples    = TRUE;
    WMAInfo.MediaType.bTemporalCompression = FALSE;
    WMAInfo.MediaType.lSampleSize          = 4;
    WMAInfo.MediaType.formattype           = WMFORMAT_WaveFormatEx;
    WMAInfo.MediaType.pUnk                 = NULL;
    WMAInfo.MediaType.cbFormat             = sizeof(WAVEFORMATEX);
    WMAInfo.MediaType.pbFormat             = (BYTE*)&WMAInfo.WaveFmt;

    // Create our writer object
    HRESULT hRes = ::WMCreateWriter(NULL, &WMAInfo.pWriter);
    if (FAILED(hRes))
    {
        return tCIDLib::EExitCodes::Normal;
    }

    // Create a profile manager
    IWMProfileManager* pProfMgr = 0;
    hRes = ::WMCreateProfileManager(&pProfMgr);
    if (FAILED(hRes))
    {
        return tCIDLib::EExitCodes::Normal;
    }
    TCOMJanitor<IWMProfileManager> janProfMgr(pProfMgr);

    unsigned char aProfBuf[4096];
    HANDLE hFile = CreateFile(L"H:\\Test.prx",
                            GENERIC_READ,
                            FILE_SHARE_READ,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL );
    memset(aProfBuf, 0, 4096);
    DWORD dwBytesRead;
    ReadFile(hFile, aProfBuf, 4096, &dwBytesRead, NULL);

    hRes = pProfMgr->LoadProfileByData
    (
        (const WCHAR*)aProfBuf
        , &WMAInfo.pProfile
    );
    if (FAILED(hRes))
    {
        return tCIDLib::EExitCodes::Normal;
    }

    // Set the profile on the writer
    hRes = WMAInfo.pWriter->SetProfile(WMAInfo.pProfile);
    if (FAILED(hRes))
    {
        return tCIDLib::EExitCodes::Normal;
    }

    //
    //  Get the current input properties of the input. Just use input
    //  zero on the assumption that we only ever have one.
    //
    IWMInputMediaProps* pInpProps = 0;
    hRes = WMAInfo.pWriter->GetInputProps(0, &pInpProps);
    if (FAILED(hRes))
    {
        return tCIDLib::EExitCodes::Normal;
    }
    TCOMJanitor<IWMInputMediaProps> janInpProps(pInpProps);

    // Set the prefab media type info on it that we set up earlier
    hRes = pInpProps->SetMediaType(&WMAInfo.MediaType);
    if (FAILED(hRes))
    {
        return tCIDLib::EExitCodes::Normal;
    }

    // And now set the input properties back on the writer
    hRes = WMAInfo.pWriter->SetInputProps(0, pInpProps);
    if (FAILED(hRes))
    {
        return tCIDLib::EExitCodes::Normal;
    }

    // Tell the writer the output file name
    WMAInfo.pWriter->SetOutputFilename(L"H:\\Test.WMA");

    // Reset any remaining per-session stuff
    WMAInfo.c8CurTime = 0;

    return tCIDLib::EExitCodes::Normal;
}


//
//  Handles any startup work we need to do. We do the work to set up the
//  writer object and the profile and get it ready to start accepting
//  samples.
//
CID_DLLEXPORT tCIDLib::EExitCodes eTestDLL()
{
    TThread thrTest(L"TestDLLThread", &eTestThreadFunc);
    thrTest.Start();
    return thrTest.eWaitForDeath(10000000);
}


