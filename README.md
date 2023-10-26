Summary
=======

mp3cat copies a byte stream, skipping everything except mp3 frames with valid headers.

    $ mp3cat [options] - - < infile.mp3 > outfile.mp3
    $ mp3cat [options] - outdir < infile.mp3
    $ mp3cat [options] indir - > outfile.mp3
    $ mp3cat [options] indir outdir

Installation
============

    mkdir -p /usr/local/src
    cd /usr/local/src
    git clone https://github.com/tomclegg/mp3cat
    cd mp3cat
    make install

Filtering mp3 files
===================

mp3cat can read a stream from standard input and write to standard output:

    $ mp3cat - - < infile.mp3 > outfile.mp3
    $ cat infiles/*.mp3 | mp3cat - - > outfile.mp3

mp3cat only outputs MP3 frames with valid headers, even if there is extra garbage in its input stream.

Writing to an mp3dir
====================

Instead of standard output, mp3cat can store MP3 data across multiple files in a directory (an "mp3dir"). An mp3dir contains a number of files called tTIMESTAMP.mp3 (where TIMESTAMP is a UNIX timestamp) and one file called current.mp3. mp3cat appends MP3 frames to current.mp3. When current.mp3 exceeds a size threshold (default 1000000 bytes) while writing an mp3 frame, mp3cat renames it to tTIMESTAMP.mp3 (where TIMESTAMP is the current time) and starts a new current.mp3.

    $ mkdir /var/log/mp3files
    $ mp3cat - /var/log/mp3files < some-large-file.mp3
    $ ls /var/log/mp3files
    current.mp3
    t1033516598.mp3
    t1033516599.mp3

Control the size of mp3 segments with the --size parameter:

    $ mp3cat --size 14745600 - /var/log/mp3files < some-large-file.mp3

You can make an audio logger by reading from your sound card and writing to an mp3dir. Scripts to set this up are included. If svscan is running in /service, you can set up a logging service like this:

    # set up logging and cleaning services:
    cd /var/service
    pw useradd mp3log -d /nonexistent -s /usr/bin/true
    pw useradd mp3logl -d /nonexistent -s /usr/bin/true
    mp3log-conf mp3log mp3logl /var/service/mp3log /var/log/mp3log /dev/dsp 32

    # start the services
    chown root:mp3log /dev/dsp
    chmod 660 /dev/dsp
    ln -s /var/service/mp3log /service/
    ln -s /var/service/mp3log-clean /service/

Exceptions
==========

If tTIMESTAMP.mp3 already exists when the size threshold is reached, mp3cat keeps writing to current.mp3. Therefore, output files will be larger than expected if the size threshold is reached in less than one second.

If a write fails (perhaps due to a full disk), mp3cat waits a second and tries again.

Reading from an mp3dir
======================

Instead of standard input, mp3cat can also read MP3 data from an mp3dir. It reads from the end of current.mp3 just as tail -f would: when it reaches the end of current.mp3, it waits for more data to appear. While waiting for more data, if mp3cat detects that current.mp3 has been renamed and a new current.mp3 has been created, it closes the old file, opens the new one, and starts reading from the start of the new file.

You can turn your audio logger into an MP3 radio station by reading from the logger's mp3dir and writing to a network client. Here is a guide to using mp3cat to make an MP3 logger and MP3 streaming server.

Options
=======

The following options are accepted before indir and outdir arguments.

<dl>
<dt>--noclean</dt>
<dd>Output entire input data, even if it contains stuff other than mp3 frames.</dd>
<dt>--size bytes</dt>
<dd>Set target size for mp3 files (mp3dir output only; default 1000000)</dd>
<dt>--tail bytes</dt>
<dd>How far from the end of current.mp3 to start reading (mp3dir input only; default 16384)</dd>
<dt>-v</dt>
<dd>Print more status information to stderr</dd>
</dl>

Feedback
========

I'm happy to hear whether mp3cat works for you, and how you're using it.

License
=======

GPLv2. See COPYING file.
