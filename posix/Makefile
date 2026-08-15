PROG    ?= mDashDeviceSimulator
CFLAGS  ?= -W -Wall -Wextra -g -I../src -pthread -DMG_TLS=MG_TLS_BUILTIN
LFLAGS  ?=
SOURCES ?= main.c ../src/mDash.c ../src/mongoose.c

all: $(PROG)

$(PROG): $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) $(LFLAGS) $(EXTRA) -o $(PROG)

run: $(PROG)
	$(RUN) ./$(PROG) -pass "$(MDASH_DEVICE_KEY)"

clean:
	rm -rf $(PROG) _CL* *.o *.dSYM *.gcov *.gcno *.gcda *.obj *.exe *.ilk *.pdb
