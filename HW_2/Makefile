# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -fopenmp

# Source files
ask1_src = ask1/game_of_life.c
ask2_src = ask2/back_sub.c

# Executables
ask1_exe = ask1/game_of_life
ask2_exe = ask2/back_sub

all: $(ask1_exe) $(ask2_exe) 

ask1: $(ask1_exe)
	$(CC) $(CFLAGS) $(ask1_src) -o $(ask1_exe)

ask2: $(ask2_exe)
	$(CC) $(CFLAGS)  $(ask2_src) -o $(ask2_exe)

out1: $(ask1_exe)
	$(CC) $(CFLAGS) -DOUT $(ask1_src) -o $(ask1_exe)

out2: $(ask2_exe)
	$(CC) $(CFLAGS) -DOUT  $(ask2_src) -o $(ask2_exe)


.PHONY: clean
clean:
	rm -f $(ask1_exe) $(ask2_exe) 
