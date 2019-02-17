# flags
CC		= gcc
STD		= -std=c99
FLAGS	= -g -Wall


# elf
EXE		= lightftpd


# Source
SOURCE	= src/ftpd.c


# make
$(EXE): $(SOURCE)
	$(CC) $(STD) $(FLAGS) $(SOURCE) -o $(EXE)

