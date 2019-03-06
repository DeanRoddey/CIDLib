/*
 * jdatadst.cpp
 *
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains compression data destination routines for the case of
 * emitting JPEG data to a file (or any stdio stream).
 *
 *
 * CIDJPEG: This file has been modified to work solely in terms of our CIDLib
 * binary streaming system, which already abstracts data sources and targets.
 */

/* this is not a core library module, so it doesn't define JPEG_INTERNALS */
#include "CIDLib.hpp"

extern "C"
{
    #include "jinclude.h"
    #include "jpeglib.h"
    #include "jerror.h"
}


/* Expanded data destination object for stdio output */

typedef struct
{
  struct jpeg_destination_mgr pub; /* public fields */

  TBinOutStream*    pstrmTar;
  tCIDLib::TCard1*  pc1Buf;
} my_destination_mgr;

typedef my_destination_mgr * my_dest_ptr;
#define OUTPUT_BUF_SIZE  4096


/*
 * Initialize destination --- called by jpeg_start_compress
 * before any data is actually written.
 */

METHODDEF(void)
init_destination (j_compress_ptr cinfo)
{
  my_dest_ptr dest = (my_dest_ptr) cinfo->dest;

  /* Allocate the output buffer --- it will be released when done with image */
  dest->pc1Buf = (tCIDLib::TCard1*)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE, OUTPUT_BUF_SIZE);

  dest->pub.next_output_byte = dest->pc1Buf;
  dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
}


/*
 * Empty the output buffer --- called whenever buffer fills up.
 *
 * In typical applications, this should write the entire output buffer
 * (ignoring the current state of next_output_byte & free_in_buffer),
 * reset the pointer & count to the start of the buffer, and return TRUE
 * indicating that the buffer has been dumped.
 *
 * In applications that need to be able to suspend compression due to output
 * overrun, a FALSE return indicates that the buffer cannot be emptied now.
 * In this situation, the compressor will return to its caller (possibly with
 * an indication that it has not accepted all the supplied scanlines).  The
 * application should resume compression after it has made more room in the
 * output buffer.  Note that there are substantial restrictions on the use of
 * suspension --- see the documentation.
 *
 * When suspending, the compressor will back up to a convenient restart point
 * (typically the start of the current MCU). next_output_byte & free_in_buffer
 * indicate where the restart point will be if the current call returns FALSE.
 * Data beyond this point will be regenerated after resumption, so do not
 * write it out when emptying the buffer externally.
 */

METHODDEF(boolean)
empty_output_buffer (j_compress_ptr cinfo)
{
    my_dest_ptr dest = (my_dest_ptr) cinfo->dest;

    try
    {
        dest->pstrmTar->c4WriteRawBuffer(dest->pc1Buf, OUTPUT_BUF_SIZE);
    }

    catch(...)
    {
        ERREXIT(cinfo, JERR_FILE_WRITE);
    }

    dest->pub.next_output_byte = dest->pc1Buf;
    dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
    return TRUE;
}


/*
 * Terminate destination --- called by jpeg_finish_compress
 * after all data has been written.  Usually needs to flush buffer.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */

METHODDEF(void)
term_destination (j_compress_ptr cinfo)
{
    my_dest_ptr dest = (my_dest_ptr) cinfo->dest;
    size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;

    /* Write any data remaining in the buffer */
    if (datacount > 0)
    {
        try
        {
            dest->pstrmTar->c4WriteRawBuffer(dest->pc1Buf, datacount);
        }

        catch(...)
        {
            ERREXIT(cinfo, JERR_FILE_WRITE);
        }
    }
}


/*
 * Prepare for output to a binary stream.
 */
GLOBAL(void)
jpeg_stdio_dest (j_compress_ptr cinfo, TBinOutStream* pstrmOut)
{
    my_dest_ptr dest;

    /* The destination object is made permanent so that multiple JPEG images
     * can be written to the same file without re-executing jpeg_stdio_dest.
     * This makes it dangerous to use this manager and a different destination
     * manager serially with the same JPEG object, because their private object
     * sizes may be different.  Caveat programmer.
     */

    /* first time for this JPEG object? */
    if (cinfo->dest == NULL)
    {
        cinfo->dest = (struct jpeg_destination_mgr *)
        (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
      			  SIZEOF(my_destination_mgr));
    }

    dest = (my_dest_ptr) cinfo->dest;
    dest->pub.init_destination = init_destination;
    dest->pub.empty_output_buffer = empty_output_buffer;
    dest->pub.term_destination = term_destination;
    dest->pstrmTar = pstrmOut;
}

