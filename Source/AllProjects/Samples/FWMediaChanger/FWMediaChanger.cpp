//
// FILE NAME: FWMediaChangers.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/21/2008
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
//  This is the main module for a sample program that demonstrates how to
//  interact with firewire based media changers. This program will not work
//  unless you have Windows Media Center (a later version if on XP, or the
//  Vista based version), since the underlying low level support drivers are
//  not in any other version of Windows.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes. This program is so simple that we don't even have a header of
//  our own. So just include CIDLib, which is all we need.
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
    ,   tCIDLib::TVoid*     pData
);


// ---------------------------------------------------------------------------
//  Do the magic main module code
// ---------------------------------------------------------------------------
CIDLib_MainModule(TThread(L"FWMChangersMainThread", eMainThreadFunc))



// ---------------------------------------------------------------------------
//  Local data
//
//  conIn
//  conOut
//      We need input and output consoles in order to display info the user
//      and get info from him.
// ---------------------------------------------------------------------------
static TInConsole   conIn;
static TOutConsole  conOut;



// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------

//
//  This is the the thread function for the main thread. It is started by
//  the CIDLib_MainModule() macro above.
//
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    try
    {
        conOut << L"CIDLib Firewire Media Changer Sample\n" << kCIDLib::EndLn;

        //
        //  Iterate any attached changers and display them so that the user can
        //  select one. These records are pretty large so don't make the list
        //  huge. There aren't going to be THAT many such devices connected.
        //
        TVector<TMediaChgInfo> colChangers;
        const tCIDLib::TCard4 c4Count = TMediaChanger::c4EnumChangers(colChangers);

        if (c4Count)
        {
            // We got some, so dump them out
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                TMediaChgInfo& mciCur = colChangers[c4Index];
                conOut << (c4Index + 1) << L". ProdId="
                       << mciCur.m_strProductId << L"\n    Path="
                       << mciCur.m_strDevPath << L"\n    Number="
                       << mciCur.m_c4ChangerNum
                       << kCIDLib::EndLn;
            }

            // Let he user select a changer
            tCIDLib::TCard4 c4SelInd = 0;
            // <TBD>

            //
            //  Go back and find it again by the number and compare the
            //  returned data to make sure that the lookup by number works
            //  and gets the same changer.
            //
            TMediaChgInfo& mciSrc = colChangers[0];
            TMediaChgInfo mciFind;
            if (TMediaChanger::bFindChangerNum(mciSrc.m_c4ChangerNum, mciFind))
            {
                if (mciSrc.m_strProductId != mciFind.m_strProductId)
                {
                    conOut  << L" - Inconsistent product id on lookup"
                            << kCIDLib::EndLn;
                }

                if (mciSrc.m_strDevPath != mciFind.m_strDevPath)
                {
                    conOut  << L" - Inconsistent device path on lookup"
                            << kCIDLib::EndLn;
                }

                if (mciSrc.m_c4ChangerNum != mciFind.m_c4ChangerNum)
                {
                    conOut  << L" - Inconsistent changer number on lookup"
                            << kCIDLib::EndLn;
                }

                // Open that changer
                TMediaChanger mchTest(mciSrc.m_c4ChangerNum);
                mchTest.Open();

#if 0
                conOut << L"Occupied slots\n----------------------\n";
                for (tCIDLib::TCard4 c4SlotInd = 0; c4SlotInd < 200; c4SlotInd++)
                {
                    if (mchTest.bIsSlotOccupied(c4SlotInd))
                        conOut << L"    Slot " << c4SlotInd << kCIDLib::EndLn;
                }
                conOut << kCIDLib::EndLn;
#endif

                mchTest.LoadSlot(0);

                mchTest.Close();
            }
        }
         else
        {
            conOut  << L" - No media changers were found on this system\n"
                    << kCIDLib::EndLn;
        }
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
                <<  L"Error: " << errToCatch.strErrText() << kCIDLib::NewLn << kCIDLib::EndLn;
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
                << kerrToCatch.errcId() << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::FatalError;
    }

    // Catch a general exception
    catch(...)
    {
        conOut  << L"A general exception occured during processing"
                << kCIDLib::NewLn << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::SystemException;
    }

    return tCIDLib::EExitCodes::Normal;
}
