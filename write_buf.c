/* $Id: write_buf.c,v 1.1.1.1 2003/05/04 22:02:23 tom Exp $ */

#include "read_buf.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifndef O_NOFOLLOW
#define O_NOFOLLOW 0
#endif

static int Outfile_is_open=0;
static time_t Last_timestamp=0;
static int Outfd=STDOUT_FILENO;

int write_buf (const char* buffer, int wanttowrite,
	       const char* const outfile,
	       const char* const outfile_prefix,
	       const int bytes_per_file)
{
  int syswrite_bytes, newfd;
  struct stat statbuf;
  time_t current_time;
  char* newfilename;

  if (outfile && !Outfile_is_open) {
    while (0 > (newfd = open(outfile, O_CREAT|O_WRONLY|O_NOFOLLOW, 0x1ff))) {
      fprintf(stderr, "Couldn't open %s (%s), trying again\n", outfile, strerror(errno));
      sleep (1);
    }
    lseek (newfd, 0, SEEK_END);
    if (Outfd != STDOUT_FILENO)
      close (Outfd);
    Outfd = newfd;
  }
  if (outfile) {
    fstat (Outfd, &statbuf);
    if (statbuf.st_size >= bytes_per_file
	&& time(&current_time) > Last_timestamp) {
      Last_timestamp = current_time;
      while (!(newfilename = malloc (strlen(outfile_prefix)+21))) {
	fprintf(stderr, "Couldn't allocate %lu bytes, trying again\n", strlen(outfile_prefix)+21);
	sleep (1);
      }
      strcpy (newfilename, outfile_prefix);
      if (newfilename [strlen (newfilename) - 1] != '/')
	strcat (newfilename, "/");
      sprintf (&newfilename[strlen(newfilename)], "t%d.mp3", (int)current_time);
      rename (outfile, newfilename);
      free (newfilename);
      while (0 > (newfd = open (outfile, O_CREAT|O_WRONLY|O_NOFOLLOW, 0x1ff))) {
	fprintf(stderr, "Couldn't open %s (%s), trying again\n", outfile, strerror(errno));
	sleep (1);
      }
      close (Outfd);
      lseek (newfd, 0, SEEK_END);
      if (Outfd != STDOUT_FILENO)
	close (Outfd);
      Outfd = newfd;
    }
  }
  while (wanttowrite > 0) {
    syswrite_bytes = write (Outfd, buffer, wanttowrite);
    if (syswrite_bytes > 0) {
      wanttowrite -= syswrite_bytes;
      buffer += syswrite_bytes;
    }
    else if (!outfile && syswrite_bytes < 0 && errno != EAGAIN) {
      if (errno == EPIPE)
	exit (0);
      fprintf(stderr, "Write failed (%s), giving up.\n", strerror(errno));
      exit (1);
    }
    if (wanttowrite > 0) { /* disk full? */
      char* errorstring = strerror(errno);
      fprintf(stderr,
	      "write() only wrote %d of %d bytes (%s), trying again\n",
	      syswrite_bytes > 0 ? syswrite_bytes : 0,
	      wanttowrite,
	      errorstring ? errorstring : "unknown error");
      sleep (1);
    }
  }
}

#ifdef COMPILER_CAN_READ_PSEUDOCODE

write (wanttowrite_buf, wanttowrite_bytes) {
  int syswrite_bytes;
  if (writing_dir && !file_open_for_writing) {
    while (!open("<outdir>/current.mp3",
                 O_APPEND|O_CREAT|O_WRONLY|O_NOFOLLOW)) {
      sleep (1);
    }
  }
  if (writing_dir
      && Current_size >= Bytes_per_file
      && timestamp() > last_file_timestamp) {
    last_file_timestamp = timestamp();
    close();
    rename("current.mp3", "<last_file_timestamp>.mp3");
    Current_size = 0;
    open("current.mp3", "w");
  }
  while (wanttowrite_bytes > 0) {
    syswrite_bytes = syswrite ()) < wanttowrite_bytes;
    if(syswrite_bytes > 0) {
      wanttowrite_bytes -= syswrite_bytes;
      wanttowrite_buf += syswrite_bytes;
      Current_size += syswrite_bytes;
    }
    if (wanttowrite_bytes > 0) { // disk full
      sleep (1);
    }
  }
}

#endif
