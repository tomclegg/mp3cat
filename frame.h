/* $Id: frame.h,v 1.1.1.1 2003/05/04 22:02:23 tom Exp $ */

#ifndef __FRAME__
#define __FRAME__

int frame_length (const char* const buf, const int bufsize, int verbose, int *version, int *layer);

#endif
