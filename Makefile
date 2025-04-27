AR = $(GCC_PREFIX)ar
CC = $(GCC_PREFIX)gcc
CFLAGS = -fPIC -Iinclude -Isrc -Isrc/main -Isrc/glapi -Isrc/shader -Isrc/shader/grammar -Isrc/shader/slang -DTHREADS
LDFLAGS =
LIBS =
DYLIB =

TARGET = unix

ifeq ($(TARGET),windows)
CFLAGS += -DWIN32_THREADS
LDFLAGS += -Wl,--out-implib,libOSMesa.lib
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
	$(AR) rcs $@ $(OBJS)

libOSMesa$(DYLIB): $(OBJS)
	$(CC) $(LDFLAGS) -shared -o $@ $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -f `find src -name "*.o"` *.so *.dll *.a *.lib
