PREFIX?=/usr/local
all: mp3cat
mp3cat: *.c *.h
	cc -o mp3cat *.c
install: install_bin install_scripts
install_bin: mp3cat
	mkdir -p $(PREFIX)/bin
	install -o root -g 0 mp3cat $(PREFIX)/bin/mp3cat
install_scripts:
	mkdir -p $(PREFIX)/bin
	install -o root -g 0 mp3log $(PREFIX)/bin/mp3log
	install -o root -g 0 mp3log-conf $(PREFIX)/bin/mp3log-conf
	install -o root -g 0 mp3dirclean $(PREFIX)/bin/mp3dirclean
	install -o root -g 0 mp3http $(PREFIX)/bin/mp3http
	install -o root -g 0 mp3stream-conf $(PREFIX)/bin/mp3stream-conf
