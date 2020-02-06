//
// FILE NAME: CIDJPEG_Image.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/13/2005
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
//  This file implements the TJPEGImage class.
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
#include    "CIDJPEG_.hpp"

// ---------------------------------------------------------------------------
//  The JPEG group library headers, which are only needed in here. We hide
//  all this stuff inside our standard image interface.
//
//  We need to supress some deprecation warnings here
// ---------------------------------------------------------------------------
#include    <stdio.h>
#include    <setjmp.h>
#include    "jpeglib.h"
#include    "jerror.h"

//
//  We bring these into here, so that we don't have to make their header
//  aware of our binary stream class. We are the only one who calls these.
//
void jpeg_stdio_dest JPP((j_compress_ptr cinfo, TBinOutStream* poutstrm));
void jpeg_stdio_src JPP((j_decompress_ptr cinfo, TBinInStream* pinstrm));

//
//  Our error manager extension, to support a long jump error exit. Since
//  our code is C++, we can't throw a C++ exception back out throw the C
//  code that calls our callbacks.
//
extern "C"
{
    struct MyErrorMgr
    {
        jpeg_error_mgr  pub;
        jmp_buf         setjmp_buffer;
        char            msg_buf[JMSG_LENGTH_MAX];
    };

    METHODDEF(void) my_error_exit(j_common_ptr cinfo)
    {
        // cinfo->err really points to a my_error_mgr struct, so coerce pointer
        MyErrorMgr* pMyErr = (MyErrorMgr*)cinfo->err;

        // Format the text and store it in our error string member
        (*cinfo->err->format_message)(cinfo, pMyErr->msg_buf);

        // Return control to the setjmp point
        longjmp(pMyErr->setjmp_buffer, 1);
    }
}


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TJPEGImage,TCIDImage)



// ---------------------------------------------------------------------------
//  CLASS: TJPEGImage
// PREFIX: img
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TJPEGImage: Constructors and Destructor
// ---------------------------------------------------------------------------
TJPEGImage::TJPEGImage() :

    TCIDImage(tCIDImage::EImgTypes::JPEG)
    , m_bFastDecode(kCIDLib::False)
    , m_bOptimalEncoding(kCIDLib::False)
    , m_c4CompQuality(75)
    , m_eOutSample(tCIDJPEG::EOutSamples::F4_2_2)
    , m_pc1DecContext(nullptr)
    , m_pc1EncContext(nullptr)
{
}

TJPEGImage::TJPEGImage(const TPixelArray& pixaBits) :

    TCIDImage(tCIDImage::EImgTypes::JPEG, pixaBits, 0, 0)
    , m_bFastDecode(kCIDLib::False)
    , m_bOptimalEncoding(kCIDLib::False)
    , m_c4CompQuality(75)
    , m_eOutSample(tCIDJPEG::EOutSamples::F4_2_2)
    , m_pc1DecContext(nullptr)
    , m_pc1EncContext(nullptr)
{
    // We need our data to be top down, so if it's not, flip it
    ForceRowOrder(tCIDImage::ERowOrders::TopDown);
}

TJPEGImage::TJPEGImage( const   TPixelArray&    pixaBits
                        , const TClrPalette&    palToSet
                        , const tCIDLib::TCard4 c4ClrsImportant
                        , const tCIDLib::TCard4 c4ClrsUsed) :

    TCIDImage
    (
        tCIDImage::EImgTypes::JPEG
        , pixaBits
        , palToSet
        , c4ClrsImportant
        , c4ClrsUsed
    )
    , m_bFastDecode(kCIDLib::False)
    , m_bOptimalEncoding(kCIDLib::False)
    , m_c4CompQuality(75)
    , m_eOutSample(tCIDJPEG::EOutSamples::F4_2_2)
    , m_pc1DecContext(nullptr)
    , m_pc1EncContext(nullptr)
{
    // We need our data to be top down, so if it's not, flip it
    ForceRowOrder(tCIDImage::ERowOrders::TopDown);
}

TJPEGImage::TJPEGImage(const TCIDImage& imgSrc) :

    TCIDImage(tCIDImage::EImgTypes::JPEG, imgSrc)
    , m_bFastDecode(kCIDLib::False)
    , m_bOptimalEncoding(kCIDLib::False)
    , m_c4CompQuality(75)
    , m_eOutSample(tCIDJPEG::EOutSamples::F4_2_2)
    , m_pc1DecContext(nullptr)
    , m_pc1EncContext(nullptr)
{
    // We need our data to be top down, so if it's not, flip it
    ForceRowOrder(tCIDImage::ERowOrders::TopDown);
}

TJPEGImage::TJPEGImage(const TJPEGImage& imgSrc) :

    TCIDImage(tCIDImage::EImgTypes::JPEG, imgSrc)
    , m_bFastDecode(imgSrc.m_bFastDecode)
    , m_bOptimalEncoding(imgSrc.m_bOptimalEncoding)
    , m_c4CompQuality(imgSrc.m_c4CompQuality)
    , m_eOutSample(imgSrc.m_eOutSample)
    , m_pc1DecContext(nullptr)
    , m_pc1EncContext(nullptr)
{
    // Note that we zero the enc/dec contexts. We'll fault our own in if needed
}

TJPEGImage::TJPEGImage(TJPEGImage&& imgSrc) :

    TJPEGImage()
{
    *this = tCIDLib::ForceMove(imgSrc);
}


TJPEGImage::~TJPEGImage()
{
    // Clean up context objects
    if (m_pc1DecContext)
    {
        // And clean up the data
        jpeg_decompress_struct* pcinfo = (jpeg_decompress_struct*)m_pc1DecContext;
        jpeg_destroy_decompress(pcinfo);
        delete [] m_pc1DecContext;
        m_pc1DecContext = nullptr;
    }

    if (m_pc1EncContext)
    {
        // And clean up the data
        jpeg_compress_struct* pcinfo = (jpeg_compress_struct*)m_pc1EncContext;
        jpeg_destroy_compress(pcinfo);
        delete [] m_pc1EncContext;
        m_pc1EncContext = nullptr;
    }
}


// ---------------------------------------------------------------------------
//  TJPEGImage: Public operators
// ---------------------------------------------------------------------------
TJPEGImage& TJPEGImage::operator=(const TJPEGImage& imgSrc)
{
    if (this != &imgSrc)
    {
        TParent::operator=(imgSrc);

        // Copy our stuff that aren't just comp/decomp time stuff
        m_bFastDecode       = imgSrc.m_bFastDecode;
        m_bOptimalEncoding  = imgSrc.m_bOptimalEncoding;
        m_c4CompQuality     = imgSrc.m_c4CompQuality;
        m_eOutSample        = imgSrc.m_eOutSample;
    }
    return *this;
}

TJPEGImage& TJPEGImage::operator=(TJPEGImage&& imgSrc)
{
    if (this != &imgSrc)
    {
        TParent::operator=(tCIDLib::ForceMove(imgSrc));

        tCIDLib::Swap(m_bFastDecode, imgSrc.m_bFastDecode);
        tCIDLib::Swap(m_bOptimalEncoding, imgSrc.m_bOptimalEncoding);
        tCIDLib::Swap(m_c4CompQuality, imgSrc.m_c4CompQuality);
        tCIDLib::Swap(m_eOutSample, imgSrc.m_eOutSample);
        tCIDLib::Swap(m_pc1DecContext, imgSrc.m_pc1DecContext);
        tCIDLib::Swap(m_pc1EncContext, imgSrc.m_pc1EncContext);
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TJPEGImage: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TJPEGImage::bFastDecode() const
{
    return m_bFastDecode;
}

tCIDLib::TBoolean TJPEGImage::bFastDecode(const tCIDLib::TBoolean bToSet)
{
    m_bFastDecode = bToSet;
    return m_bFastDecode;
}


tCIDLib::TBoolean TJPEGImage::bOptimalEncoding() const
{
    return m_bOptimalEncoding;
}

tCIDLib::TBoolean TJPEGImage::bOptimalEncoding(const tCIDLib::TBoolean bToSet)
{
    m_bOptimalEncoding = bToSet;
    return m_bOptimalEncoding;
}


tCIDLib::TCard4 TJPEGImage::c4CompQuality() const
{
    return m_c4CompQuality;
}

tCIDLib::TCard4 TJPEGImage::c4CompQuality(const tCIDLib::TCard4 c4ToSet)
{
    m_c4CompQuality = c4ToSet;
    return m_c4CompQuality;
}


tCIDJPEG::EOutSamples TJPEGImage::eOutSample() const
{
    return m_eOutSample;
}

tCIDJPEG::EOutSamples TJPEGImage::eOutSample(const tCIDJPEG::EOutSamples eToSet)
{
    m_eOutSample = eToSet;
    return m_eOutSample;
}


// ---------------------------------------------------------------------------
//  TJPEGImage: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TJPEGImage::StreamFrom(TBinInStream& strmToReadFrom)
{
    jpeg_decompress_struct* pcinfo = nullptr;
    try
    {
        //
        //  Create the structures we need. If we've not created the
        //  decompression context yet, then do that.
        //
        if (!m_pc1DecContext)
        {
            m_pc1DecContext = new tCIDLib::TCard1[sizeof(jpeg_decompress_struct)];
            jpeg_create_decompress((jpeg_decompress_struct*)m_pc1DecContext);
        }
        pcinfo = (jpeg_decompress_struct*)m_pc1DecContext;

        // Set up our error structure. We override the default handler
        MyErrorMgr jerr;
        jerr.msg_buf[0] = 0;
        pcinfo->err = jpeg_std_error((jpeg_error_mgr*)&jerr);
        jerr.pub.error_exit = my_error_exit;

        //
        //  We use a jump error recovery, because we can't throw a C++
        //  exception back through the C code that calls us back.
        //
        if (setjmp(jerr.setjmp_buffer))
        {
            //
            //  If we get here, then there was an error and the message text
            //  is in the buffer. So we'll throw an error based on that.
            //
            facCIDJPEG().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kJPEGErrs::errcFile_ReadFailed
                , TString(jerr.msg_buf)
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
            );
        }

        jpeg_stdio_src(pcinfo, &strmToReadFrom);

        // Read the header data
        jpeg_read_header(pcinfo, TRUE);

        // Start the decompression operation
        jpeg_start_decompress(pcinfo);

        //
        //  If our fast decode flag is set, then we'll turn off some stuff
        //  that is used to create higher quality images.
        //
        if (m_bFastDecode)
        {
            pcinfo->do_fancy_upsampling = 0;
            pcinfo->do_block_smoothing = 0;
            pcinfo->dither_mode = JDITHER_NONE;
        }

        //
        //  At this point, we can call our parent class to set up the image
        //  attributes. This will set up the pixel array and we can then
        //  just have the JPEG guy write straight into our buffer.
        //
        TSize szImg(pcinfo->image_width, pcinfo->image_height);
        tCIDImage::EBitDepths   eDepth;
        tCIDImage::EPixFmts     eFmt;
        if (pcinfo->output_components == 1)
        {
            // It's gray scale
            eFmt = tCIDImage::EPixFmts::GrayScale;
            eDepth = tCIDImage::EBitDepths::Eight;
        }
         else if (pcinfo->num_components == 3)
        {
            // It's a true color image
            eFmt = tCIDImage::EPixFmts::TrueClr;
            eDepth = tCIDImage::EBitDepths::Eight;
        }
         else
        {
            // No other formats are supported by JPEG, so freak out
            facCIDJPEG().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kJPEGErrs::errcFile_UnsupportedFmt
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
            );
        }
        Set(eFmt, eDepth, tCIDImage::ERowOrders::TopDown, szImg, 0, 0);

        // Get the pixel array out so that we can load up scan lines into it
        TPixelArray& pixaTar = pixaNCBits();

        // And read all the scan lines in
        while (pcinfo->output_scanline < pcinfo->output_height)
        {
            // We let it load directly into our pixel array
            tCIDLib::TCard1* pc1Cur = pixaTar.pc1RowPtr(pcinfo->output_scanline);
            jpeg_read_scanlines(pcinfo, &pc1Cur, 1);
        }

        // Clean up the decompression operations
        jpeg_finish_decompress(pcinfo);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        // Force a full cleanup, so we have to recreate next time, and rethrow
        if (pcinfo)
        {
            jpeg_destroy_decompress(pcinfo);
            delete [] m_pc1DecContext;
            m_pc1DecContext = 0;
        }
        throw;
    }
}


tCIDLib::TVoid TJPEGImage::StreamTo(TBinOutStream& strmToWriteTo) const
{
    //
    //  Until we get up to a certain point, the exceptoin handler should
    //  not try to call finish.
    //
    tCIDLib::TBoolean bNeedFinish = kCIDLib::False;

    jpeg_compress_struct* pcinfo = nullptr;
    try
    {
        //
        //  Create the structures we need. If we've not created the
        //  compression context yet, then do that.
        //
        if (!m_pc1EncContext)
        {
            m_pc1EncContext = new tCIDLib::TCard1[sizeof(jpeg_compress_struct)];
            jpeg_create_compress((jpeg_compress_struct*)m_pc1EncContext);
        }
        pcinfo = (jpeg_compress_struct*)m_pc1EncContext;

        // Set up our error structure. We override the default handler
        MyErrorMgr jerr;
        jerr.msg_buf[0] = 0;
        pcinfo->err = jpeg_std_error((jpeg_error_mgr*)&jerr);
        jerr.pub.error_exit = my_error_exit;

        //
        //  We use a jump error recovery, because we can't throw a C++
        //  exception back through the C code that calls us back.
        //
        if (setjmp(jerr.setjmp_buffer))
        {
            //
            //  If we get here, then there was an error and the message text
            //  is in the buffer. So we'll throw an error based on that.
            //
            facCIDJPEG().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kJPEGErrs::errcFile_WriteFailed
                , TString(jerr.msg_buf)
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
            );
        }

        jpeg_stdio_dest(pcinfo, &strmToWriteTo);

        //
        //  Get the pixel array so we can set up the image parms and have
        //  access to the data.
        //
        const TPixelArray& pixaSrc = pixaBits();
        pcinfo->image_width = pixaSrc.c4Width();
        pcinfo->image_height = pixaSrc.c4Height();

        //
        //  Based on the depth and format, set the input data description.
        //  This guy doesn't support a lot of options here, so basically
        //  either it's gray scale/8 or trueclr/8. Anything else and we have
        //  to massage the data for each scan line to feed it into the
        //  compressor.
        //
        const tCIDImage::EPixFmts eFmt = pixaSrc.ePixFmt();
        const tCIDImage::EBitDepths eDepth = pixaSrc.eBitDepth();

        //
        //  Based on the format, set up the input format info that it will
        //  expect from us.
        //
        if (eFmt == tCIDImage::EPixFmts::GrayScale)
        {
            // We'll convert to 8 bit gray scale
            pcinfo->input_components = 1;
            pcinfo->in_color_space = JCS_GRAYSCALE;
        }
         else
        {
            // Else it's RGB or palette which we will convert to RGB
            pcinfo->input_components = 3;
            pcinfo->in_color_space = JCS_RGB;
        }

        //
        //  Set defaults based on format stuff above, then tweak according to
        //  our current settings.
        //
        jpeg_set_defaults(pcinfo);
        jpeg_set_quality(pcinfo, m_c4CompQuality, TRUE);
        pcinfo->input_gamma = f8Gamma();

        if (m_bOptimalEncoding)
            pcinfo->optimize_coding = 1;
        else
            pcinfo->optimize_coding = 0;

        // Ignore out sampling if not 3 component format
        if (pcinfo->input_components == 3)
        {
            switch(m_eOutSample)
            {
                case tCIDJPEG::EOutSamples::F4_2_1 :
                    pcinfo->comp_info[0].v_samp_factor = 1;
                    break;

                case tCIDJPEG::EOutSamples::F4_2_2 :
                    pcinfo->comp_info[0].h_samp_factor = 1;
                    break;

                case tCIDJPEG::EOutSamples::F4_4_4 :
                    pcinfo->comp_info[0].h_samp_factor = 1;
                    pcinfo->comp_info[0].v_samp_factor = 1;
                    break;

                case tCIDJPEG::EOutSamples::F4_2_0 :
                default :
                    // This is the dafault
                    break;
            };
        }

        // And start the compression process
        jpeg_start_compress(pcinfo, TRUE);

        // Indicate that we should do a finish in the exception handler now
        bNeedFinish = kCIDLib::True;

        // Worst case line buffer is true color
        tCIDLib::TCard1* pc1Buf = new tCIDLib::TCard1[pcinfo->image_width * 3];
        TArrayJanitor<tCIDLib::TCard1> janBuf(pc1Buf);

        if ((eFmt == tCIDImage::EPixFmts::TrueClr)
        ||  ((eFmt == tCIDImage::EPixFmts::GrayScale)
        &&   (eDepth == tCIDImage::EBitDepths::Eight)))
        {
            //
            //  If it's true color or 8 bit gray scale (no alpha in either
            //  case), we can pass our scan lines straight in.
            //
            while (pcinfo->next_scanline < pcinfo->image_height)
            {
                const tCIDLib::TCard1* pc1Cur = pixaSrc.pc1RowPtr(pcinfo->next_scanline);
                jpeg_write_scanlines
                (
                    pcinfo
                    , (tCIDLib::TCard1**)&pc1Cur
                    , 1
                );
            }
        }
         else if (eFmt == tCIDImage::EPixFmts::TrueAlpha)
        {
            //
            //  It's true color plus alpha, so we have to throw away the 4th
            //  byte of each sample.
            //
            while (pcinfo->next_scanline < pcinfo->image_height)
            {
                const tCIDLib::TCard1* pc1Cur = pixaSrc.pc1RowPtr(pcinfo->next_scanline);
                tCIDLib::TCard1* pc1Tar = pc1Buf;

                for (tCIDLib::TCard4 c4Col = 0; c4Col < pcinfo->image_width; c4Col++)
                {
                    // Copy over the 3 color components
                    *pc1Tar++ = *pc1Cur++;
                    *pc1Tar++ = *pc1Cur++;
                    *pc1Tar++ = *pc1Cur++;

                    // Skip the alpha byte
                    pc1Cur++;
                }
                jpeg_write_scanlines(pcinfo, (tCIDLib::TCard1**)&pc1Buf, 1);
            }

        }
         else if (eFmt == tCIDImage::EPixFmts::GrayAlpha)
        {
            // It's got to be 16 bit or 8 byte gray scale with alpha
            while (pcinfo->next_scanline < pcinfo->image_height)
            {
                const tCIDLib::TCard1* pc1Cur = pixaSrc.pc1RowPtr(pcinfo->next_scanline);
                tCIDLib::TCard1* pc1Tar = pc1Buf;

                if (eDepth == tCIDImage::EBitDepths::Eight)
                {
                    // We take the graph byte and skip the alpha byte
                    for (tCIDLib::TCard4 c4Col = 0; c4Col < pcinfo->image_width; c4Col++)
                    {
                        *pc1Tar++ = *pc1Cur++;
                        pc1Cur++;
                    }
                }
                 else
                {
                    //
                    //  We take the high gray scale byte and skip the low
                    //  gray scale and two alphas.
                    //
                    *pc1Tar++ = *pc1Cur;
                    pc1Cur += 4;
                }
                jpeg_write_scanlines(pcinfo, (tCIDLib::TCard1**)&pc1Buf, 1);
            }
        }
         else if (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Palette)
              ||  (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Color)
              &&   (eDepth == tCIDImage::EBitDepths::Five)))
        {
            //
            //  It palette based or it's 5 bit RGB, so we'll convert to 8 bit
            //  RGB.
            //
            const TClrPalette& palSrc = palClrs();
            TRGBClr rgbCur;
            TPoint pntAt;
            while (pcinfo->next_scanline < pcinfo->image_height)
            {
                tCIDLib::TCard1* pc1Tar = pc1Buf;
                pntAt.i4Y(pcinfo->next_scanline);
                tCIDLib::TCard4 c4ByteInd = 0;
                for (tCIDLib::TCard4 c4Col = 0; c4Col < pcinfo->image_width; c4Col++)
                {
                    pntAt.i4X(c4Col);
                    pixaSrc.QueryRGBAt(pntAt, palSrc, rgbCur);

                    *pc1Tar++ = rgbCur.c1Blue();
                    *pc1Tar++ = rgbCur.c1Green();
                    *pc1Tar++ = rgbCur.c1Red();
                }

                // And write out the converted line
                jpeg_write_scanlines(pcinfo, (tCIDLib::TCard1**)&pc1Buf, 1);
            }
        }
         else
        {
            //
            //  It's sub-8 bit or 16 bit gray, so it's a fairly straightforward
            //  conversion that we can do pretty efficiently. We'll convert
            //  to 8 bit gray scale by just shifting the values up
            //  appopriately.
            //
            while (pcinfo->next_scanline < pcinfo->image_height)
            {
                tCIDLib::TCard1* pc1Tar = pc1Buf;
                for (tCIDLib::TCard4 c4Col = 0; c4Col < pcinfo->image_width; c4Col++)
                {
                    // Get the value out for this pixel
                    const tCIDLib::TCard4 c4Pix = pixaSrc.c4At
                    (
                        c4Col
                        , pcinfo->next_scanline
                    );

                    switch(eDepth)
                    {
                        case tCIDImage::EBitDepths::One :
                        {
                            if (c4Pix)
                                *pc1Tar++ = 0xFF;
                            else
                                *pc1Tar++ = 0;
                            break;
                        }

                        case tCIDImage::EBitDepths::Two :
                        {
                            *pc1Tar++ = tCIDLib::TCard1(c4Pix << 6);
                            break;
                        }

                        case tCIDImage::EBitDepths::Four :
                        {
                            *pc1Tar++ = tCIDLib::TCard1(c4Pix << 4);
                            break;
                        }

                        case tCIDImage::EBitDepths::Sixteen :
                        {
                            // Take the high byte and skip the low
                            *pc1Tar++ = tCIDLib::TCard1(c4Pix >> 8);
                            break;
                        }
                    };
                }

                // And write out the converted line
                jpeg_write_scanlines(pcinfo, (tCIDLib::TCard1**)&pc1Buf, 1);
            }
        }

        // Finish off the compression operation
        jpeg_finish_compress(pcinfo);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        // Make sure we clean up the per-use stuff
        if (pcinfo && bNeedFinish)
            jpeg_finish_compress(pcinfo);
        throw;
    }
}


