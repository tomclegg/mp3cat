/* $Id: mp3cat.c,v 1.1.1.1 2003/05/04 22:02:23 tom Exp $ */

/* mp3cat
 *
 * Copyright 2002 Tom Clegg
 * tom@tomclegg.net -- 306 Carbonate St, Nelson BC V1L 4P2 Canada
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#define BUFMAX 8192 /* at least 4 times size of largest expected MP3 frame */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "frame.h"
#include "read_buf.h"
#include "write_buf.h"

int main(int argc, char * const * argv)
{
  int bytesread;
  char buffer[BUFMAX];
  int bufend=0;
  int bufbegin=0;
  int framesize, nextframesize;
  int nbytes=0;
  int nframes=0;
  int timetostop=0;
  int layer, version, nextlayer, nextversion;
  char *Infile = 0, *Outfile = 0;
  const char * Outfile_prefix = 0;
  int Bytes_per_file=1000000;
  int Tail_bytes=BUFMAX*2;
  int Verbose=0;
  int Clean=1;
  int arg;

  for (arg=1; arg<argc && argv[arg][0] == '-' && argv[arg][1] != '\0'; arg++) {
    if (argv[arg][1] == 'v') {
      for (Verbose=1; argv[arg][Verbose] == 'v'; Verbose++);
      if (argv[arg][Verbose] != '\0')
	fprintf (stderr, "Ignoring garbage after 'v' characters in arg '%s'\n",
		 argv[arg]);
    }
    else if (!strcmp (argv[arg], "--clean"))
      Clean=1;
    else if (!strcmp (argv[arg], "--noclean"))
      Clean=0;
    else if (arg+1<argc && !strcmp (argv[arg], "--size")) {
      int param;
      if (sscanf (argv[arg+1], "%d", &param))
	Bytes_per_file = param;
      else
	fprintf (stderr, "Couldn't read --size parameter '%s'; ignoring.\n",
		 argv[arg+1]);
      arg++;
    }
    else if (arg+1<argc && !strcmp (argv[arg], "--tail")) {
      int param;
      if (sscanf (argv[arg+1], "%d", &param))
	Tail_bytes = param;
      else
	fprintf (stderr, "Couldn't read --tail parameter '%s'; ignoring.\n",
		 argv[arg+1]);
      arg++;
    }
    else
      fprintf (stderr, "Invalid argument '%s'; ignoring.\n", argv[arg]);
  }
  if (argc - arg != 2) {
    fprintf (stderr,
"Usage: %s [options] { indir | - } { outdir | - }\n"
"\n"
"Options:\n"
"  --noclean     Output entire input data, even if it contains stuff\n"
"                other than mp3 frames.\n"
"  --size bytes  Set target size for mp3 files (mp3dir output only;\n"
"                default 1000000).\n"
"  --tail bytes  How far from the end of current.mp3 to start reading\n"
"                (mp3dir input only; default 16384).\n"
"  -v            Print more status information to stderr.\n"
"\n"
"Examples:\n"
"  %s - - < dirty.mp3 > clean.mp3\n"
"  {generate MP3 stream} | %s --size 99000 - /tmp/mp3dir\n"
"  %s -v --tail 8000 /tmp/mp3dir - | {send to network client}\n"
"\n"
"WWW: http://tomclegg.net/mp3cat\n"
"\n",
	     argv[0], argv[0], argv[0], argv[0]);
    exit (1);
  }

  if (!strcmp (argv[arg], "-"))
    Infile = 0;
  else {
    if (!(Infile = malloc (strlen (argv[arg]) + 21))) {
      fprintf (stderr, "Couldn't allocate %d bytes; giving up.", strlen (argv[arg]) + 21);
      exit (1);
    }
    strcpy (Infile, argv[arg]);
    if (Infile [strlen (Infile) - 1] != '/')
      strcat (Infile, "/");
    strcat (Infile, "current.mp3");
  }

  ++arg;

  if (!strcmp (argv[arg], "-"))
    Outfile = 0;
  else {
    Outfile_prefix = argv[arg];
    if (!(Outfile = malloc (strlen (argv[arg]) + 21))) {
      fprintf (stderr, "Couldn't allocate %d bytes; giving up.", strlen (argv[arg]) + 21);
      exit (1);
    }
    strcpy (Outfile, argv[arg]);
    if (Outfile [strlen (Outfile)] != '/')
      strcat (Outfile, "/");
    strcat (Outfile, "current.mp3");
  }

  #define FLUSHFRAMES if (nframes) { if (Verbose >= 1) fprintf(stderr, "%d frames\n", nframes); nframes=0; }
  #define FLUSHBYTES if (nbytes) { if (Verbose >= 1) fprintf(stderr, "%d junkbytes\n", nbytes); nbytes=0; }

  do {
    /* assert: bufend >= bufbegin */

    /* shift buffer if bufbegin has passed BUFMAX/4 */
    if (bufbegin > BUFMAX/4) {
      bufend -= bufbegin;
      memmove (buffer, &buffer[bufbegin], bufend);
      bufbegin=0;
    }

    /* replenish buffer if bufend is under BUFMAX/2 */
    if (bufend <= BUFMAX/2 && !timetostop) {
      bytesread = read_buf (&buffer[bufend], BUFMAX-bufend,
			    Infile, Tail_bytes);
      if (bytesread > 0)
	bufend += bytesread;
      else if (!Infile)
	timetostop = 1;
    }

    framesize = frame_length (&buffer[bufbegin], bufend-bufbegin, 0, &version, &layer);
    if (framesize > 0 &&
	(nextframesize = frame_length (&buffer[bufbegin+framesize], bufend-bufbegin-framesize, 0, &nextversion, &nextlayer)) &&
	nextversion == version && nextlayer == layer) {

      /* valid MP3 frame */

      ++nframes;
      FLUSHBYTES;

      if (0)
	frame_length (&buffer[bufbegin], bufend-bufbegin, 1, &version, &layer);
      write_buf (&buffer[bufbegin], framesize, Outfile, Outfile_prefix, Bytes_per_file);
      bufbegin += framesize;
    }
    else {

      /* not a valid MP3 frame */

      ++nbytes;
      FLUSHFRAMES;

      if (Verbose >= 2 && framesize > 0) {
	fprintf (stderr, "Found: ");
	framesize = frame_length (&buffer[bufbegin], bufend-bufbegin, 1, &version, &layer);
	fprintf (stderr, "...but the next frame looks wrong");
	if (nextframesize)
	  fprintf (stderr, " (version %d, layer %d)", nextversion, nextlayer);
	fprintf (stderr, "\n");
      }
      bufbegin ++;
    }

  } while (bufbegin < bufend);

  FLUSHBYTES;
  FLUSHFRAMES;

  exit(0);
}
