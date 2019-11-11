LDFLAGS ?= -L/usr/lib -L/usr/local/opt/mbedtls/lib -lmbedtls -lmbedx509 -lmbedcrypto -lmbedtls -lmbedx509 -lmbedcrypto
LIBPATH ?= ../src
CFLAGS  ?= -W -Wall -I../src -I$(LIBPATH)
PROG    ?= mDashDevice
EXTRA   ?=
#CFLAGS += -fsanitize=address

linux:
	$(CC) -o $(PROG) posix.c $(EXTRA) $(CFLAGS) $(LIBPATH)/linux-x64/libmDash.a $(LDFLAGS)

mac:
	$(CC) -o $(PROG) posix.c $(EXTRA) $(CFLAGS) $(LIBPATH)/macos/libmDash.a $(LDFLAGS)

