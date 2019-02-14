/*
    visual d64

    This utility creates a picture from a cbm d64 disk drive image. This picture shows the phylical track/sector structure, the block data and whether the block is used or free. It can be used to visualize and understand how d64 images are organized and how the cbm dos works.

    https://github.com/exrom/visuald64
    
    Copyright (c) 2019, Stefan Waigand
    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

        Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
        Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. 

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "diskimage.h"

#include "bmp.h"
#define BYTES_PER_PIXEL         3

/* block has 256 bytes (pixels) which should be rendered into a rectangle */
#define BLOCK_SIZE              256
#define BLOCK_WIDTH_PX          16
#define BLOCK_HEIGHT_PX         (BLOCK_SIZE/BLOCK_WIDTH_PX)


void paint_block(unsigned char *bmp_raw_data, unsigned char *blockdata, TrackSector ts, TrackSector max_ts, int blockisfree)
{
    int i;      // counter data in block
    int data;   // data value 0..255
    int x;      // pixel column address (0 == top left)
    int y;      // pixel row address (0 == top left)
    int r;
    int g;
    int b;

    for (i=0; i<BLOCK_SIZE; i++)
    {
        x = (i % BLOCK_WIDTH_PX) +  (ts.sector * BLOCK_WIDTH_PX);
        y = (i / BLOCK_WIDTH_PX) + ((ts.track-1) * BLOCK_WIDTH_PX);

        data = blockdata[i];

        r = ((data>>0) & 7)*0x24>>blockisfree;
        g = ((data>>3) & 7)*0x24>>blockisfree;
        b = ((data>>6) & 3)*0x55>>blockisfree;

        bmp_raw_data[BYTES_PER_PIXEL * (x+(((max_ts.track) * BLOCK_HEIGHT_PX - y - 1) * max_ts.sector * BLOCK_WIDTH_PX)) + 0] = b;   // b
        bmp_raw_data[BYTES_PER_PIXEL * (x+(((max_ts.track) * BLOCK_HEIGHT_PX - y - 1) * max_ts.sector * BLOCK_WIDTH_PX)) + 1] = g;   // g
        bmp_raw_data[BYTES_PER_PIXEL * (x+(((max_ts.track) * BLOCK_HEIGHT_PX - y - 1) * max_ts.sector * BLOCK_WIDTH_PX)) + 2] = r;   // r
    }
}

int main(int argc, char **argv)
{
    DiskImage       *di;
    TrackSector     ts;
    TrackSector     max_ts;
    unsigned char   *block;
    char            **errortext;
    BMPImage        bmpimage;
    FILE            *fp_bmp;
    unsigned char   *bmp_raw_data;
    int             raw_data_size;

    printf("visual d64 V0.1\n");
    if (argc != 2)
    {
        printf("usage:\n   %s image.d64\n",argv[0]);
        return 1;
    }

    char *filename=argv[1];

    /* Load image into ram */
    if ((di = di_load_image(filename)) == NULL) {
        puts("di_load_image failed");
        return(1);
    }

    max_ts.track = di_tracks(di->type);
    max_ts.sector = di_sectors_per_track(di->type, 0);      // sector 0 has maximum number of tracks

    printf("image %s has %d tracks %d sectors (max)\n", filename, max_ts.track, max_ts.sector);

    raw_data_size=max_ts.track * max_ts.sector * BLOCK_SIZE * BYTES_PER_PIXEL;
    bmp_raw_data = malloc(raw_data_size);

    memset(bmp_raw_data, 100, raw_data_size);


//    /* Print BAM */
//    puts("TRK  FREE  MAP");
//    for (ts.track = 1; ts.track <= di_tracks(di->type); ++ts.track) {
//        printf("%3d: %2d/%d ", ts.track, di_track_blocks_free(di, ts.track), di_sectors_per_track(di->type, ts.track));
//        for (ts.sector = 0; ts.sector < di_sectors_per_track(di->type, ts.track); ++ts.sector) {
//            printf("%d", di_is_ts_free(di, ts));
//
//            block =  get_ts_addr(di, ts);
//        }
//        puts("");
//    }
//    puts("");
//
//    /* Print number of blocks free */
//    printf("%d blocks free\n", di->blocksfree);

    for (ts.track = 1; ts.track <= di_tracks(di->type); ++ts.track) {
        for (ts.sector = 0; ts.sector < di_sectors_per_track(di->type, ts.track); ++ts.sector) {
            block =  get_ts_addr(di, ts);
            paint_block(bmp_raw_data, block, ts, max_ts, di_is_ts_free(di, ts));
        }
    }


    /* Release d64 image */
    di_free_image(di);


    bmpimage.header.type                = MAGIC_VALUE;                      // Magic identifier: 0x4d42
    bmpimage.header.size                = raw_data_size+BMP_HEADER_SIZE;    // File size in bytes
    bmpimage.header.reserved1           = 0;                                // Not used
    bmpimage.header.reserved2           = 0;                                // Not used
    bmpimage.header.offset              = BMP_HEADER_SIZE;                  // Offset to image data in bytes from begi
    bmpimage.header.dib_header_size     = DIB_HEADER_SIZE;                  // DIB Header size in bytes
    bmpimage.header.width_px            = max_ts.sector * BLOCK_WIDTH_PX;   // Width of the image
    bmpimage.header.height_px           = max_ts.track * BLOCK_HEIGHT_PX;   // Height of image
    bmpimage.header.num_planes          = NUM_PLANE;                        // Number of color planes
    bmpimage.header.bits_per_pixel      = BITS_PER_PIXEL;                   // Bits per pixel
    bmpimage.header.compression         = COMPRESSION;                      // Compression type
    bmpimage.header.image_size_bytes    = raw_data_size+BMP_HEADER_SIZE;    // Image size in bytes
    bmpimage.header.x_resolution_ppm    = 0;                                // Pixels per meter
    bmpimage.header.y_resolution_ppm    = 0;                                // Pixels per meter
    bmpimage.header.num_colors          = NUM_COLORS;                       // Number of colors
    bmpimage.header.important_colors    = IMPORTANT_COLORS;                 // Important colors

    /* write picture */
    strcpy(filename+strlen(filename)-3, "bmp");

    if(!(fp_bmp = fopen(filename, "w+b" )))
    {
      printf("cannot open out file %s\r\n",filename);
      return(2);
    }


    bmpimage.data=bmp_raw_data;


    errortext=NULL;

    if (!write_bmp(fp_bmp, &bmpimage, errortext))
    {
        printf("write_bmp failed: %s\n",*errortext);
        return(1);
    }

    printf("image was written to %s\n",filename);

    free(bmp_raw_data);
    fclose(fp_bmp);

    return 0;
}
