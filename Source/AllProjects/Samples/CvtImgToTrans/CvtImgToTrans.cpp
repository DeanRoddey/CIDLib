//
// FILE NAME: CvtImgToTrans.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/06/2006
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
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
//  This is a small demo program which was written to convert some PNG images
//  (DirecTV logos) that were generated against white backgrounds, to alpha
//  based transparent versions.
//
//  So we read each PNG file in the provided directory, and we scan them and
//  find the white pixels and make them transparent. We also have to find the
//  transition pixels, which in the originals are anti-aliased, so the
//  transition from background white to foreground color is preceeded and
//  followed by a couple pixels that are blended into the white.
//
//  So we find these transition pixels, and put them back to the actual
//  color, and use the level of antialisaing to set the alpha channel on
//  those pixels. This creates a nice, smooth alpha-based anti-aliasing.
//
//  This isn't a particularly clean or clear one. It was done ad hoc to deal
//  with an immediate issue. But, I threw it into the samples since it does
//  demonstrate some useful stuff.
//
//  It shows the use of the bitmap and pixel array classes in particular. It
//  does some low level accessing of the pixel data as well, which you have to
//  be careful about, but it's often important in image process performance.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// -----------------------------------------------------------------------------
//  Includes
// -----------------------------------------------------------------------------
#include    "CIDLib.hpp"
#include    "CIDPNG.hpp"



// -----------------------------------------------------------------------------
//  Include magic main module code to start the main thread
// -----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);
CIDLib_MainModule(TThread(L"MainThread", eMainThreadFunc))


// -----------------------------------------------------------------------------
//  Local data
// -----------------------------------------------------------------------------
TOutConsole     conOut;
TInConsole      conIn;
TPathStr        pathSrc;
TPathStr        pathTar;

enum class EPixTypes
{
    Lead1
    , Lead2
    , Interior
    , Trail2
    , Trail1
};


// -----------------------------------------------------------------------------
//  Local helper methods
// -----------------------------------------------------------------------------

tCIDLib::TVoid ProcessPixel(const   TPixelArray::TRGBTriple*&   prgbSrc
                            ,       TPixelArray::TRGBQuad*&     prgbTar
                            , const EPixTypes                   eType)
{
    prgbTar->c1Red = prgbSrc->c1Red;
    prgbTar->c1Green = prgbSrc->c1Green;
    prgbTar->c1Blue = prgbSrc->c1Blue;

    if ((eType == EPixTypes::Lead1) || (eType == EPixTypes::Trail1))
        prgbTar->c1Alpha = 92;
    else if ((eType == EPixTypes::Lead2) || (eType == EPixTypes::Trail2))
        prgbTar->c1Alpha = 64;
    else
        prgbTar->c1Alpha = 0xFF;

    prgbSrc++;
    prgbTar++;
}


//
//  This method is called for each image. We do the processing here. We just
//  scan each line from right to left, top to bottom.
//
tCIDLib::TVoid ProcessImg(const TPNGImage& imgCur, TPixelArray& pixaNew)
{
    const tCIDLib::TCard4 c4Width = imgCur.c4Width();
    const tCIDLib::TCard4 c4Height = imgCur.c4Height();

    // For convenience, get a ref to the pixel array data
    const TPixelArray& pixaImg = imgCur.pixaBits();

    // Loop through all the rows
    for (tCIDLib::TCard4 c4Row = 0; c4Row < c4Height; c4Row++)
    {
        //
        //  These are all 24 bit RGB images, so we can do the efficient thing
        //  and directly access the buffer row by row.
        //
        const TPixelArray::TRGBTriple* prgbSrc
        (
            reinterpret_cast<const TPixelArray::TRGBTriple*>(pixaImg.pc1RowPtr(c4Row))
        );
        const TPixelArray::TRGBTriple* prgbStop = prgbSrc + c4Width;

        //
        //  And the output is the RGB quad type, so we can output directly to
        //  it as well.
        //
        TPixelArray::TRGBQuad* prgbTar
        (
            reinterpret_cast<TPixelArray::TRGBQuad*>(pixaNew.pc1RowPtr(c4Row))
        );

        // Loop through this row's pixels
        const TPixelArray::TRGBTriple* prgbEnd;
        tCIDLib::TCard4 c4Col = 0;
        while (c4Col < c4Width)
        {
            //
            //  If it's a pure background pixel, just copy it over as is
            //  and set the alpha fully transparent.
            //
            const tCIDLib::TBoolean bIsWhite
            (
                (prgbSrc->c1Red & prgbSrc->c1Green & prgbSrc->c1Blue) == 0xFF
            );


            if (bIsWhite)
            {
                prgbTar->c1Red = prgbSrc->c1Red;
                prgbTar->c1Green = prgbSrc->c1Green;
                prgbTar->c1Blue = prgbSrc->c1Blue;
                prgbTar->c1Alpha = 0;
                // Move both pointers up
                prgbSrc++;
                prgbTar++;
                c4Col++;
            }
             else
            {
                prgbTar->c1Red = prgbSrc->c1Red;
                prgbTar->c1Green = prgbSrc->c1Green;
                prgbTar->c1Blue = prgbSrc->c1Blue;

                //
                //  It's not a background pixel, so search forward until we
                //  find another background pixel. This tells us the width
                //  of this section of foreground pixels, which tells us how
                //  many pixels of aliasing we have to deal with on each
                //  side.
                //
                prgbEnd = prgbSrc + 1;
                while (prgbEnd < prgbStop)
                {
                    if ((prgbEnd->c1Red & prgbEnd->c1Green & prgbEnd->c1Blue) == 0xFF)
                    {
                        // Move back to the last non-background pixel
                        prgbEnd--;
                        break;
                    }
                    prgbEnd++;
                }

                // Calc the width of this foreground section
                const tCIDLib::TCard4 c4FWidth = (prgbEnd - prgbSrc) + 1;

                //
                //  If it's over 3, then we have strongly aliased pixels on
                //  each end.
                //
                tCIDLib::TCard4 c4Index = 0;
                ProcessPixel(prgbSrc, prgbTar, EPixTypes::Lead1);
                c4Index++;
                tCIDLib::TCard4 c4IntEnd;
                if (c4FWidth > 3)
                {
                    ProcessPixel(prgbSrc, prgbTar, EPixTypes::Lead2);
                    c4Index++;
                    c4IntEnd = c4FWidth - 2;
                }
                 else if (c4FWidth > 2)
                {
                    c4IntEnd = c4FWidth - 1;
                }
                 else
                {
                    c4IntEnd = c4Index;
                }
                while (c4Index < c4IntEnd)
                {
                    ProcessPixel(prgbSrc, prgbTar, EPixTypes::Interior);
                    c4Index++;
                }
                if (c4FWidth > 3)
                    ProcessPixel(prgbSrc, prgbTar, EPixTypes::Trail2);
                if (c4FWidth > 1)
                    ProcessPixel(prgbSrc, prgbTar, EPixTypes::Trail1);

                // Now move us up to the end pixel
                c4Col += c4FWidth - 1;
            }
        }
    }
}



// -----------------------------------------------------------------------------
//  This is the main program thread
// -----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid* const)
{
    // Let our caller go
    thrThis.Sync();

    // Get the source directory
    conOut << L"Source logo path: " << kCIDLib::FlushIt;
    conIn >> pathSrc;

    // Get the target directory
    conOut << L"Target output path: " << kCIDLib::FlushIt;
    conIn >> pathTar;

    // See if it exists. If not, then exit
    if (!TFileSys::bIsDirectory(pathSrc))
    {
        conOut << L"\nThe source path was not valid" << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::BadParameters;
    }

    // If the output path doesn't exist, then create it
    if (!TFileSys::bExists(pathTar))
        TFileSys::MakePath(pathTar);

    // See if any PNG files exist there
    TDirIter diterFiles;
    TFindBuf fndbCur;
    if (!diterFiles.bFindFirst(pathSrc, L"*.PNG", fndbCur))
    {
        conOut << L"No source PNG files were found" << kCIDLib::EndLn;
        return tCIDLib::EExitCodes::Normal;
    }

    // There are, so process them all
    TPNGImage   imgCur;
    TPathStr    pathOutFile;
    TString     strNameExt;
    do
    {
        // Create a binary file input stream over the current file
        {
            TBinFileInStream strmPNG
            (
                fndbCur.pathFileName()
                , tCIDLib::ECreateActs::OpenIfExists
                , tCIDLib::EFilePerms::Default
                , tCIDLib::EFileFlags::SequentialScan
                , tCIDLib::EAccessModes::Excl_Read
            );

            // Stream in the PNG image data
            strmPNG >> imgCur;

            // Verify that it's a 24 bit RGB image
            if ((imgCur.ePixFmt() != tCIDImage::EPixFmts::TrueClr)
            ||  (imgCur.eBitDepth() != tCIDImage::EBitDepths::Eight))
            {
                conOut  << L"Image '" << fndbCur.pathFileName()
                        << L"' was not 24 bit RGB. Skipping it"
                        << kCIDLib::EndLn;
                continue;
            }

            // Indicate what file we are doing
            conOut  << L"Image: " << fndbCur.pathFileName()
                    << kCIDLib::EndLn;

            //
            //  Create a new pixel array of the same size, but 32 bit
            //  so that we can create the new version.
            //
            TPixelArray pixaNew
            (
                tCIDImage::EPixFmts::TrueAlpha
                , tCIDImage::EBitDepths::Eight
                , tCIDImage::ERowOrders::TopDown
                , imgCur.szImage()
            );

            // Ok, we can process this image now
            ProcessImg(imgCur, pixaNew);

            //
            //  And write it back out with the same name, in the target
            //  directory.
            //
            pathOutFile = pathTar;
            fndbCur.pathFileName().bQueryNameExt(strNameExt);
            pathOutFile.AddLevel(strNameExt);

            // Create the new image to write out
            TPNGImage imgNew(pixaNew);
            TBinFileOutStream strmOut
            (
                pathOutFile
                , tCIDLib::ECreateActs::CreateAlways
                , tCIDLib::EFilePerms::Default
                , tCIDLib::EFileFlags::SequentialScan
                , tCIDLib::EAccessModes::Excl_Write
            );
            strmOut << imgNew << kCIDLib::FlushIt;
        }

    }   while (diterFiles.bFindNext(fndbCur));

    return tCIDLib::EExitCodes::Normal;
}

