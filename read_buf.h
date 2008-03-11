/* $Id: read_buf.h,v 1.1.1.1 2003/05/04 22:02:23 tom Exp $ */

#ifndef __READ_BUF__
#define __READ_BUF__

int read_buf (char* buffer,
	      int wanttoread,
	      const char* const Infile,
	      const int tail_bytes);

#endif
