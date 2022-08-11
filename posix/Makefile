PROG    ?= mDashDeviceSimulator
CFLAGS  ?= -W -Wall -Wextra -g -I../src -pthread
LFLAGS  ?= -L/usr/lib -L/usr/local/opt/mbedtls/lib -I/opt/homebrew/opt/mbedtls/include -I/usrc/local -I/usr/local/opt -L/opt/homebrew/opt/mbedtls/lib -lmbedtls -lmbedx509 -lmbedcrypto -lmbedtls -lmbedx509 -lmbedcrypto
SOURCES ?= main.c ../src/mDash.c ../src/mongoose.c 

all: $(PROG)

$(PROG): $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) $(LFLAGS) $(EXTRA) -o $(PROG)

run: $(PROG)
	$(RUN) ./$(PROG) $(ARGS)

clean:
	rm -rf $(PROG) _CL* *.o *.dSYM *.gcov *.gcno *.gcda *.obj *.exe *.ilk *.pdb
