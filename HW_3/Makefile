# Compiler and flags
CC = mpicc

# Source files
ask1_src = ask1/gol.c
ask2_src = ask2/mat_vec_mul.c

# Executables
ask1_exe = ask1/gol
ask2_exe = ask2/mat_vec

all: $(ask1_exe) $(ask2_exe) 

$(ask1_exe):
	$(CC) $(ask1_src) -o $(ask1_exe)

$(ask2_exe):
	$(CC)  $(ask2_src) -o $(ask2_exe)

out1:
	$(CC) -DOUT $(ask1_src) -o $(ask1_exe)

out2:
	$(CC) -DOUT  $(ask2_src) -o $(ask2_exe)


.PHONY: clean
clean:
	rm -f $(ask1_exe) $(ask2_exe) 
