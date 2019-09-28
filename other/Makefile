LDFLAGS	?= -L/usr/local/Cellar/mbedtls/2.13.0/lib -lmbedtls -lmbedx509 -lmbedcrypto -lmbedtls -lmbedx509 -lmbedcrypto
CFLAGS	?= -W -Wall -I../src
PROG		?= mDashDevice
EXTRA		?=
#CFLAGS += -fsanitize=address

linux:
	$(CC) posix.c $(EXTRA) $(CFLAGS) $(LDFLAGS) ../src/linux-x64/libmDash.a -o $(PROG)

mac:
	$(CC) posix.c $(EXTRA) $(CFLAGS) $(LDFLAGS) ../src/macos/libmDash.a -o $(PROG)

