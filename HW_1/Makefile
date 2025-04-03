# Compiler and flags
CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lpthread

# Source files
ask1_src = ask-1/montecarlo.c ask-1/my_rand.c
ask2_src = ask-2/ask2.c
ask3_src = ask-3/ask3.c
ask3_re_src = ask-3/ask3-re.c
ask4_src = ask-4/ask4.c ask-4/rw_lock.c ask-4/my_rand.c

# Object files
ask1_obj = ask-1/montecarlo.o ask-1/my_rand.o
ask2_obj = ask-2/ask2.o
ask3_obj = ask-3/ask3.o
ask3_re_obj = ask-3/ask3-re.o
ask4_obj = ask-4/ask4.o  ask-4/my_rand.o ask-4/rw_lock.o

# Executables
ask1_exe = ask-1/ask1
ask2_exe = ask-2/ask2
ask3_exe = ask-3/ask3
ask3_re_exe = ask-3/ask3_re
ask4_exe = ask-4/ask4

all: $(ask1_exe) $(ask2_exe) $(ask3_exe) $(ask3_re_exe) $(ask4_exe) 

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

ask1 $(ask1_exe): $(ask1_obj)
	$(CC) $(CFLAGS)  $? -o $(ask1_exe) $(LDFLAGS)

ask2 $(ask2_exe): $(ask2_obj)
	$(CC) $(CFLAGS)  $? -o $(ask2_exe) $(LDFLAGS)

ask3 $(ask3_exe): $(ask3_obj)
	$(CC) $(CFLAGS)  $? -o $(ask3_exe) $(LDFLAGS)

ask3_re $(ask3_re_exe): $(ask3_re_obj)
	$(CC) $(CFLAGS)  $? -o $(ask3_re_exe) $(LDFLAGS)

ask4 $(ask4_exe): $(ask4_obj) 
	$(CC) $(CFLAGS)  $? -o $(ask4_exe) $(LDFLAGS)

ask4_rd : $(ask4_obj) 
	$(CC) $(CFLAGS) -c ask-4/rw_lock.c -o ask-4/rw_lock.o 
	$(CC) $(CFLAGS)  $? -o $(ask4_exe) $(LDFLAGS)

ask4_wr : $(ask4_obj) 
	$(CC) $(CFLAGS) -c -DWPRIO ask-4/rw_lock.c -o ask-4/rw_lock.o  
	$(CC) $(CFLAGS)  $? -o $(ask4_exe) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(ask1_obj) $(ask2_obj) $(ask3_obj) $(ask3_re_obj) $(ask4_obj) $(ask1_exe) $(ask2_exe) $(ask3_exe) $(ask3_re_exe) $(ask4_exe)
