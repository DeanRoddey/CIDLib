#include "CIDImgFact.hpp"

tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"ImgDiffMainThread", eMainThreadFunc))
TOutConsole  conOut;

tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    thrThis.Sync();

    tCIDImage::EImgTypes eType;
    tCIDImage::TUImgPtr img1
    (
        facCIDImgFact().pimgLoadImgFile(TSysInfo::strCmdLineParmAt(0), eType)
    );
    tCIDImage::TUImgPtr img2
    (
        facCIDImgFact().pimgLoadImgFile(TSysInfo::strCmdLineParmAt(1), eType)
    );
    if (img1->szImage() == img2->szImage())
    {
        // Just treat them like raw bytes
        const tCIDLib::TCard1* pc1Buf1 = img1->pixaBits().pc1Buffer();
        const tCIDLib::TCard1* pc1Buf2 = img2->pixaBits().pc1Buffer();
        const tCIDLib::TCard4 c4ByteCnt = img1->c4ImageSz();

        tCIDLib::TFloat8 f8DiffAccum = 0;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ByteCnt; c4Index++)
            f8DiffAccum += tCIDLib::AbsDiff(pc1Buf1[c4Index], pc1Buf2[c4Index]);
        const tCIDLib::TFloat8 f8MaxDiff(0xFF * img1->szImage().c4SquareUnits());
        conOut << L"Ratio " << (f8DiffAccum / f8MaxDiff) << kCIDLib::NewEndLn;
    }
     else
    {
        conOut << L"Images are not the same size" << kCIDLib::NewEndLn;
    }

    return tCIDLib::EExitCodes::Normal;
}