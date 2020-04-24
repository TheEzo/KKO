# Makefile
# KKO
# Tomas Willaschek
# xwilla00

CC=g++
SRC=src/main.cpp src/DataFile.cpp src/Tree.cpp
FLAGS=-Wall -Wextra -pedantic
BIN=huff_codec

default:
	${CC} ${SRC} ${FLAGS} -o ${BIN}

clean:
	rm -f kko_xwilla00.zip

pack:
	zip -r kko_xwilla00.zip src/ Makefile willa00.pdf

