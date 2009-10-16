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
PACKAGE=	luaprocname
LIBVERSION=	4
VERSION=	$(LUAVERSION).$(LIBVERSION)

SRCS=		procname.c test.lua
EXTRADIST=	Makefile README ChangeLog

WARN=		-pedantic -Wall
INCS=		-I$(LUAINC)
CPPFLAGS=	-fPIC $(INCS) $(WARN)

MYNAME=		procname
MYLIB= 		$(MYNAME)

OBJS=		$(MYLIB).o

T= 		$(MYLIB).so

OS=$(shell uname)
ifeq ($(OS),Darwin)
  LDFLAGS_SHARED=-bundle -undefined dynamic_lookup
  LIBS=
else
  LDFLAGS_SHARED=-shared
  LIBS=
endif

# targets
phony += all
all:	$T

phony += test
test:	all
	$(LUA) test.lua

$T:	$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(LDFLAGS_SHARED) $(OBJS) $(LIBS)

$(OBJS):

phony += clean
clean:
	rm -f $(OBJS) $T core core.* a.out

phony += install
install: $T
	$(INSTALL) -D $T $(DESTDIR)/$(LUALIB)/$T
	
phony += show-funcs
show-funcs:
	@echo "$(MYNAME) library:"
	@fgrep '/**' l$(MYLIB).c | cut -f2 -d/ | tr -d '*' | sort

.PHONY: $(phony)
# eof
