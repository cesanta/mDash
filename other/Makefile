LDFLAGS ?= -L/usr/local/opt/mbedtls/lib -lmbedtls -lmbedx509 -lmbedcrypto -lmbedtls -lmbedx509 -lmbedcrypto
LIBPATH ?= ../src
CFLAGS  ?= -W -Wall -I../src -I$(LIBPATH)
PROG    ?= mDashDevice
EXTRA   ?=
#CFLAGS += -fsanitize=address

linux:
	$(CC) posix.c $(EXTRA) $(CFLAGS) $(LDFLAGS) $(LIBPATH)/linux-x64/libmDash.a -o $(PROG)

mac:
	$(CC) posix.c $(EXTRA) $(CFLAGS) $(LDFLAGS) $(LIBPATH)/macos/libmDash.a -o $(PROG)

