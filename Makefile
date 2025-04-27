AR = $(GCC_PREFIX)ar
CC = $(GCC_PREFIX)gcc
CFLAGS = -fPIC -Iinclude -Isrc -Isrc/main -Isrc/glapi -Isrc/shader -Isrc/shader/grammar -Isrc/shader/slang -DTHREADS $(EXTRA_CFLAGS)
LDFLAGS =
LIBS =
DYLIB =

TARGET = unix

ifneq (,$(filter $(CC),owcc))
CFLAGS += -std=c99
AR = wlib
endif

ifeq ($(TARGET),windows)
CFLAGS += -DWIN32_THREADS
ifeq (,$(filter $(CC),owcc))
LDFLAGS += -Wl,--out-implib,libOSMesa.lib
else
CFLAGS += -I$(WATCOM)/h -I$(WATCOM)/h/nt
endif
DYLIB = .dll
else ifeq ($(TARGET),unix)
CFLAGS += -DPTHREADS
LIBS += -lpthread -lm
DYLIB = .so
endif

.PHONY: all clean
.SUFFIXES: .c .o

all: libOSMesa.a libOSMesa$(DYLIB)

include objects.mk

libOSMesa.a: $(OBJS)
ifeq ($(AR),wlib)
	$(AR) -fo -b -n -io -o=$@ $@ +`echo $(OBJS) | sed 's/ / +/g'`
else
	$(AR) rcs $@ $(OBJS)
endif

libOSMesa$(DYLIB): $(OBJS)
	$(CC) $(LDFLAGS) -shared -o $@ $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -f `find src -name "*.o"` *.so *.dll *.a *.lib
