/* uncompr.c -- decompress a memory buffer
 * Copyright (C) 1995-2003, 2010 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* @(#) $Id$ */

#define ZLIB_INTERNAL
#include "zlibFactory.hpp"
#include "zlib.h"
#include <stdio.h>
/* ===========================================================================
     Decompresses the source buffer into the destination buffer.  sourceLen is
   the byte length of the source buffer. Upon entry, destLen is the total
   size of the destination buffer, which must be large enough to hold the
   entire uncompressed data. (The size of the uncompressed data must have
   been saved previously by the compressor and transmitted to the decompressor
   by some mechanism outside the scope of this compression library.)
   Upon exit, destLen is the actual size of the compressed buffer.

     uncompress returns Z_OK if success, Z_MEM_ERROR if there was not
   enough memory, Z_BUF_ERROR if there was not enough room in the output
   buffer, or Z_DATA_ERROR if the input data was corrupted.
*/
int ZEXPORT uncompress(Bytef* dest, uLongf* destLen, const Bytef* source, uLong sourceLen) {
    z_stream stream;
    int err;
    stream.next_in = (Bytef*)source;
    stream.avail_in = (uInt)sourceLen;
    /* Check for source > 64K on 16-bit machine: */
    if ((uLong)stream.avail_in != sourceLen) return Z_BUF_ERROR;

    stream.next_out = dest;
    stream.avail_out = (uInt)*destLen;
    if ((uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;

    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;

    err = inflateInit(&stream);
    if (err != Z_OK) return err;

#ifdef XILINX_CODE
    // Create factory class
    auto sObj = zlibFactory::getInstance();
    auto cflow = true;
    sObj->xilinxPreChecks();
    if (!(sObj->getXmode())) {
        cflow = false;
    }
    char* noaccel = getenv("XILINX_NO_ACCEL");
    if (noaccel && (std::stoi(noaccel) == 0)){ 
        cflow = false;
    }    

    if (cflow) {
        // Driver class
        zlibDriver* driver = sObj->getDriverInstance(&stream, XILINX_INFLATE);
        if (driver == NULL) {
            cflow = false;
        }

        uint64_t debytes = driver->xilinxHwUncompress(&stream, 0);
        *destLen = debytes;
        sObj->releaseDriverObj(&stream);
        err = inflateEnd(&stream);
        if (debytes == 0) {
            return Z_BUF_ERROR;
        } else {
            return Z_OK;
        }
    }
#endif

    err = inflate(&stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        inflateEnd(&stream);
        if (err == Z_NEED_DICT || (err == Z_BUF_ERROR && stream.avail_in == 0)) return Z_DATA_ERROR;
        return err;
    }
    *destLen = stream.total_out;

    err = inflateEnd(&stream);
    return err;
}
