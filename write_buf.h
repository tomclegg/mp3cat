#ifndef __WRITE_BUF__
#define __WRITE_BUF__

void write_buf (const char* buffer,
                int wanttowrite,
                const char* const outfile,
                const char* const outfile_prefix,
                const int bytes_per_file);

#endif
