/* $Id: read_buf.c,v 1.1.1.1 2003/05/04 22:02:23 tom Exp $ */

#include "read_buf.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

static int Infile_is_open=0;
static int Infd=STDIN_FILENO;

int read_buf (char* buffer,
	      int wanttoread,
	      const char* const infile,
	      const int tail_bytes)
{
  int bytesread = 0, hasrolled, newfd, sysread_bytes;
  struct stat statbuf, fstatbuf;

  if (!Infile_is_open && infile) {
    while (0 > (newfd = open (infile, O_RDONLY))) {
      fprintf (stderr, "Couldn't open %s (%s), trying again\n",
	       infile,
	       strerror (errno));
      sleep (1);
    }
    lseek (newfd, -tail_bytes, SEEK_END);
    if (Infd != STDIN_FILENO)
      close (Infd);
    Infd = newfd;
    Infile_is_open = 1;
  }

  while (wanttoread > 0) {
    hasrolled =
      0 != infile
      &&
      0 == stat (infile, &statbuf)
      &&
      0 == fstat (Infd, &fstatbuf)
      &&
      statbuf.st_ino != fstatbuf.st_ino;
    sysread_bytes = read (Infd, buffer, wanttoread);
    if (infile == 0)
      return (sysread_bytes > 0 ? sysread_bytes : 0);
    if (sysread_bytes > 0) {
      buffer += sysread_bytes;
      wanttoread -= sysread_bytes;
      bytesread += sysread_bytes;
    }
    if (wanttoread > 0) {
      if (hasrolled) {
	if (Infd != STDIN_FILENO)
	  close (Infd);
	Infd = open (infile, O_RDONLY);
      }
      usleep (100);
    }
  }
  return bytesread;
}

#ifdef COMPILER_CAN_READ_PSEUDOCODE

read () {
  while (reading_dir && eof ()) {
    has_rolled
      = reading_dir && (current file's inode != current.mp3's inode);
    if (has_rolled && eof ()) {
      close();
      open("<indir>/current.mp3");
    } else {
      usleep(100);
    }
  }
  sysread ();
  return bytes_read;
}

#endif
