PROG := sish

SRCDIR := src

CMD := ls ${SRCDIR}/*.c
SRCS := ${CMD:sh}
INCLUDE := include

BIN = bin

CFLAGS := -ansi -g3 -Wall -Werror -Wextra -Wformat=2 -Wjump-misses-init 
CFLAGS += -Wlogical-op -Wpedantic -Wshadow
CFLAGS += -I include

all: ${PROG}

depend:
	mkdep -- ${CFLAGS} *.c

${PROG}: ${SRCS}
	@mkdir -p ${BIN}
	${CC} ${CFLAGS} -o ${BIN}/${PROG} ${SRCS}

clean:
	rm -r ${BIN}
