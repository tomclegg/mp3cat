/* $Id: write_buf.h,v 1.1.1.1 2003/05/04 22:02:23 tom Exp $ */

#ifndef __WRITE_BUF__
#define __WRITE_BUF__

int write_buf (const char* buffer,
	       int wanttowrite,
	       const char* const outfile,
	       const char* const outfile_prefix,
	       const int bytes_per_file);

#endif
