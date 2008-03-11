all: mp3cat
mp3cat: *.c *.h
	cc -o mp3cat *.c
install:
	install -o root -g 0 mp3cat /usr/local/bin/mp3cat
	install -o root -g 0 mp3log /usr/local/bin/mp3log
	install -o root -g 0 mp3log-conf /usr/local/bin/mp3log-conf
	install -o root -g 0 mp3dirclean /usr/local/bin/mp3dirclean
	install -o root -g 0 mp3http /usr/local/bin/mp3http
	install -o root -g 0 mp3stream-conf /usr/local/bin/mp3stream-conf
