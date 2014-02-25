/* $Id: frame.c,v 1.1.1.1 2003/05/04 22:02:23 tom Exp $ */

#include "frame.h"
#include <stdio.h>

const static int
v1l1_bitrate[] = {-1,32,64,96,128,160,192,224,256,288,320,352,384,416,448,-1};
const static int
v1l2_bitrate[] = {-1,32,48,56,64,80,96,112,128,160,192,224,256,320,384,-1};
const static int
v1l3_bitrate[] = {-1,32,40,48,56,64,80,96,112,128,160,192,224,256,320,-1};
const static int
v2l1_bitrate[] = {-1,32,48,56,64,80,96,112,128,144,160,176,192,224,256,-1};
const static int
v2l2or3_bitrate[] = {-1,8,16,24,32,40,48,56,64,80,96,112,128,144,160,-1};

const static int samplerate_table[4][3] =
{ {11025, 12000, 8000}, /* version 2.5 */
  {-1, -1, -1}, /* reserved version */
  {22050, 24000, 16000}, /* version 2 */
  {44100, 48000, 32000} /* version 1 */
};

int frame_length (const char* const buf, const int bufsize, int verbose, int *save_version, int *save_layer)
     /* returns expected length of this frame based on header,
	or 0 if header is incomprehensible */
{
  int
    version,
    layer,
    bitrate, bitrate_index,
    samplerate, samplerate_index,
    padding,
    length;

  if (bufsize < 4)
    return 0;

  /* make sure first 11 bits are on */
  if (buf[0] != '\377')
    return 0;
  if ((buf[1] & '\340') != '\340')
    return 0;

  /* discover version: 0=2.5, 1=reserved, 2=2, 3=1  */
  #define VERSION_1 3
  #define VERSION_2 2
  #define VERSION_25 0
  version = (buf[1] >> 3) & 3;
  if (version == 1)
    return 0;

  /* discover layer: 0=reserved, 1=III, 2=II, 3=I */
  #define LAYER_I 3
  #define LAYER_II 2
  #define LAYER_III 1
  layer = (buf[1] >> 1) & 3;
  if (layer == 0)
    return 0;

  /* ignore protection bit: 0=protected, 1=not */

  /* discover bitrate */
  bitrate_index = (buf[2] >> 4) & 15;
  bitrate = 0;
  switch (version) {
  case VERSION_25:
  case VERSION_2:
    switch (layer) {
    case LAYER_III:
    case LAYER_II:
      bitrate = v2l2or3_bitrate[bitrate_index];
      break;
    case LAYER_I:
      bitrate = v2l1_bitrate[bitrate_index];
      break;
    default: /* should have already returned above! */
      return 0;
    }
    break;
  case VERSION_1:
    switch (layer) {
    case LAYER_III:
      bitrate = v1l3_bitrate[bitrate_index];
      break;
    case LAYER_II:
      bitrate = v1l2_bitrate[bitrate_index];
      break;
    case LAYER_I:
      bitrate = v1l1_bitrate[bitrate_index];
      break;
    default: /* should have already returned above! */
      return 0;
    }
    break;
  default: /* should have already returned above! */
    return 0;
  }
  /* some bitrates (0000, 1111) are invalid/undefined */
  if (bitrate <= 0)
    return 0;
  bitrate = bitrate * 1000;

  /* discover sample rate: 00, 01, 10, 11=reserved */
  samplerate_index = (buf[2] >> 2) & 3;
  if (samplerate_index == 3)
    return 0;
  samplerate = samplerate_table[version][samplerate_index];

  /* discover padding flag: 0=not padded, 1=padded */
  padding = (buf[2] >> 1) & 1;

  /* compute frame length: */
  if (layer == LAYER_I) {
    length = (12 * bitrate / samplerate + padding) * 4;
  } else {
    /* layer II and III */
    if (version == VERSION_1 || layer != LAYER_III)
      length = 144 * bitrate / samplerate + padding;
    else
      /* Judging by mpglib/common.c in lame source code -- and
         contrary to popular documentation -- MPEG-2 layer III and
         MPEG-2.5 layer III frames are half the size of other layer II
         and III frames: */
      length = 72 * bitrate / samplerate + padding;
  }

  if (verbose)
    fprintf (stderr, "version=%d layer=%d bitrate=%d samplerate=%d (%d) padding=%d length=%d\n",
	     version, layer, bitrate, samplerate, samplerate_index, padding, length);

  *save_version = version;
  *save_layer = layer;
  return length;
}
