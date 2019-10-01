LDFLAGS	?= -L/usr/local/opt/mbedtls/lib -lmbedtls -lmbedx509 -lmbedcrypto -lmbedtls -lmbedx509 -lmbedcrypto
CFLAGS	?= -W -Wall -I../src
PROG		?= mDashDevice
EXTRA		?=
#CFLAGS += -fsanitize=address

linux:
	$(CC) posix.c $(EXTRA) $(CFLAGS) $(LDFLAGS) ../src/linux-x64/libmDash.a -o $(PROG)

mac:
	$(CC) posix.c $(EXTRA) $(CFLAGS) $(LDFLAGS) ../src/macos/libmDash.a -o $(PROG)

