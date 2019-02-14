# Visual d64

This utility creates a picture from a cbm d64 disk drive image. This picture shows the phylical track/sector structure, the block data and whether the block is used or free. It can be used to visualize and understand how d64 images are organized and how the cbm dos works.

A typical d64 disk image has 683 blocks organized in 35 tracks and up to 21 sectors. Every sector is represented in the image as 16x16 colored pixels. This makes the picture 35x16=560 pixel high and 21x16=336 pixel wide. 

The color of one pixel represents the block data byte. Data 0x00 will give a black pixel, 0xFF a white one. Other data values will not give different gray values but colors (looks much nicer :-) ).

A block containing 00 01 02 03  04 05 06 07 ... F8 F9 FA FB  FC FD FE FF will result in this picture

![block with ascending data bytes](/img/block_increment.png)

A block which is marked as free in the block availibility map (BAM) will have 50% reduced brightness.

# Picture gallery

A just formatted disk on the vc1541 floppy drive. One can see the bam on track18/00 and the first empty directory block at track18/01. All other data is set to 0x01.

![empty vc1541 formatted disk](/img/empty_vc1541.d64.png)

An image conatining one file (written downwards starting at track 17). You can see that sectors are not written contiguously but interleaved.

![one file image](/img/one_file.png)

A d64 containing three files. Have a look at track 18/01, can you see the three directory entries?. First file is compressed data from track 17-10. Track 10 contains 6 used blocks and block 10/20 is partly written. Second an third file on track 19+20.

![three file image](/img/three_files.png)

Double sided d71 image. Side 1 track 22+23 and side 2 track 22 contain non-zero unused blocks. Likely because files were deleted from the image. This decreases the compressibility of a d64 into a zip file.

![double side d71 image](/img/double_sided_d71.png)


## usage

visuald64 mydiskimage.d64 --> mydiskimage.bmp is written.

## licensing

* diskimage.c is released under a slightly modified BSD license. https://paradroid.automac.se/diskimage/
* bmp lib taken from https://engineering.purdue.edu/ece264/17au/hw/HW15 © Copyright 2017 Purdue University
* visuald64.c released under the same slightly modified BSD license as diskimage.c. https://github.com/exrom/visuald64


## improvement ideas

* command line options
* selectable coloring to distinguish between petscii alphanumeric chars and assembly code
* create png
* include visuald64 in emulators and show which sector is currently written to. Highlight current file's blocks. Live watch block fill
* d64 picture art :)
