program=joy
OPT=-g
LIBS=
CC?=gcc

OBJ=joy.o libenjoy/test/libenjoy.o
ifneq ($(BUILD_WINDOWS),true)
  OBJ += libenjoy/test/libenjoy_linux.o
else
  OBJ += libenjoy/test/libenjoy_win32.o
  LIBS += -lwinmm
endif

.PHONY: build
.PHONY: install
.PHONY: uninstall
.PHONY: clean
.PHONY: distrib

build: ${program}

clean:
	rm -f *.o ${program}

${program}: ${OBJ}
	$(CC) ${OBJ} -o ${program} ${OPT} ${LIBS}

test.o: test.c
	$(CC) test.c -c ${OPT}

libenjoy.o: libenjoy/src/libenjoy.c
	$(CC) libenjoy/src/libenjoy.c -c ${OPT}

ifneq ($(BUILD_WINDOWS),true)
libenjoy_linux.o: libenjoy/src/libenjoy_linux.c
	$(CC) libenjoy/src/libenjoy_linux.c -c ${OPT}
else
libenjoy_win32.o: libenjoy/src/libenjoy_win32.c
	$(CC) libenjoy/src/libenjoy_win32.c -c ${OPT}
endif
