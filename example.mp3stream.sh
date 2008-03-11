#!/bin/sh

# accept an http request (ignoring the details of the request)
# and send an mp3 stream in response.
#
# this is installed in /var/service/mp3stream/mp3stream.sh

read a
if echo "$a" | grep -q ' HTTP/1.1'
then
  read a
  while echo -n "$a" | egrep -q :
  do
    read a
  done
fi
echo "HTTP/1.1 200 OK
Content-type: audio/mpeg

"
exec mp3cat --tail 65536 mp3dir -
