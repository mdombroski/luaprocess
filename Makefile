# makefile for procname library for Lua

# change these to reflect your Lua installation
PREFIX=		/usr/local
LUAVERSION=	5.1
LUAINC= 	$(PREFIX)/include
LUALIB= 	$(PREFIX)/lib/lua/$(LUAVERSION)
LUABIN= 	$(PREFIX)/bin

# other executables
LUA=		lua
INSTALL=	install

# no need to change anything below here
WARN=		-pedantic -Wall
INCS=		-I$(LUAINC)
CPPFLAGS=	-fPIC $(INCS) $(WARN)

MYNAME=		procname
MYLIB= 		$(MYNAME)
OBJS=		$(MYLIB).o
TARGET= 	$(MYLIB).so

OS=$(shell uname)
ifeq ($(OS),Darwin)
  LDFLAGS_SHARED=-bundle -undefined dynamic_lookup
else
  LDFLAGS_SHARED=-shared
endif


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(LDFLAGS_SHARED) $(OBJS)

$(OBJS):

clean:
	rm -f $(OBJS) $(TARGET)

install: $(TARGET)
	$(INSTALL) -D $(TARGET) $(DESTDIR)/$(LUALIB)/$(TARGET)

test: all
	$(LUA) test.lua

.PHONY: all clean install test

