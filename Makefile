PREFIX?=/usr/local
INSTALL_USER?=$(shell id -u)
INSTALL_GROUP?=$(shell id -g)
INSTALL_COMMAND?=install -o $(INSTALL_USER) -g $(INSTALL_GROUP) -m 0555
all: mp3cat
mp3cat: *.c *.h
	cc -Wall -o mp3cat *.c
install: install_bin install_scripts
install_bin: mp3cat
	mkdir -p $(PREFIX)/bin
	$(INSTALL_COMMAND) mp3cat $(PREFIX)/bin/mp3cat
install_scripts:
	mkdir -p $(PREFIX)/bin
	$(INSTALL_COMMAND) mp3log $(PREFIX)/bin/mp3log
	$(INSTALL_COMMAND) mp3log-conf $(PREFIX)/bin/mp3log-conf
	$(INSTALL_COMMAND) mp3dirclean $(PREFIX)/bin/mp3dirclean
	$(INSTALL_COMMAND) mp3http $(PREFIX)/bin/mp3http
	$(INSTALL_COMMAND) mp3stream-conf $(PREFIX)/bin/mp3stream-conf
