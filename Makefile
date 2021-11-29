CC=gcc
CFLAGS=-std=gnu99 -Wall -Werror -g
LIBS=-lcunit -lpthread
INCLUDE_HEADERS_DIRECTORY=-Iheaders
TEST_DIR=./tests
SRC_DIR=./src
SRC_FILES=$(wildcard $(SRC_DIR)/*.c)

kmeans: main.c $(SRC_FILES:.c=.o) 
	$(CC) $(INCLUDE_HEADERS_DIRECTORY) $(CFLAGS) -o $@ $^ $(LIBS)  
# the command above will run the following command: gcc -Wall -Werror -g -o kmeans src/distance.o other_object_filespresent_above.o ... -lcunit -lpthread

%.o: %.c                  # If for example you want to compute example.c this will create an object file called example.o in the same directory as example.c. Don't forget to clean it in your "make clean"
	@$(CC) $(INCLUDE_HEADERS_DIRECTORY) $(CFLAGS) -o $@ -c $<

# The command for compiling and running tests
$(TEST_DIR)/%: $(TEST_DIR)/%.o $(SRC_DIR)/%.c $(wildcard $(SRC_DIR)/*.c)
	@$(CC) $(INCLUDE_HEADERS_DIRECTORY) $(CFLAGS) -o $@ $^ $(LIBS)
	@echo "------------------------- Running Test : $@ -------------------------- "
	@./$@ 

alltests : $(TEST_DIR)/filehandler $(TEST_DIR)/argumentsparser $(TEST_DIR)/distance $(TEST_DIR)/threadshandler

test_output_files: ./tests/output.o
	gcc -o test_output_files ./tests/output.o -lcunit -lpthread

./tests/output.o : ./tests/test_outputFiles.c
	@$(CC) $(INCLUDE_HEADERS_DIRECTORY) $(CFLAGS) -o ./tests/output.o -c ./tests/test_outputFiles.c  $(LIBS)

#Tests, compiles tests
tests: kmeans alltests clean

#Clean, all the object files 
clean: 
	@rm -f *.o
	@rm -f src/*.o
	@rm -f tests/*.o
	@rm -f kmeans
	@rm -f massif.out.*
	@find ./tests -type f ! -name "*.c" -exec rm {} \;
	@find ./src -type f ! -name "*.c" -exec rm {} \;
	@echo "Done cleaning."
compile_and_clean : kmeans clean_objects

clean_objects : 
	@rm -f *.o
	@rm -f src/*.o
	@rm -f tests/*.o
	@rm -f massif.out.*
	@find ./tests -type f ! -name "*.c" -exec rm {} \;
	@find ./src -type f ! -name "*.c" -exec rm {} \;
	@echo "Done cleaning."

valgrind_kmeans : compile_and_clean
	@echo "-------------------------- Running with valgrind to CHECK MEMORY LEAKS --------------------------"  
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --leak-resolution=high -s ./kmeans -k 4 -p 7 -n 3 -d euclidean -f output_files/test.csv input_binary/example.bin
	@echo "-------------------------- Running with helgrind to CHECK THREADS ERRORS --------------------------"  
	valgrind --tool=helgrind -s ./kmeans -k 2 -p 3 -n 20 -f output_files/test.csv input_binary/example.bin

valgrind: valgrind_kmeans
	
# a .PHONY target forces make to execute the command even if the target already exists
.PHONY: compile_and_clean 

timeExecute: compile_and_clean
	@for file in 'input_binary/spreadPoints.bin' 'input_binary/centeredPoints.bin'; \
	do \
		for k in 1 3 6; \
		do \
			for p in 6 7 8; \
			do \
				for n in 1 2 8 100; do echo time ./kmeans -q -k $$k -p $$p -n $$n -f test.csv $$file; time ./kmeans -q -k $$k -p $$p -n $$n -f test.csv $$file; done \
			done \
		done \
	done \

# Runs cppcheck on all source files
all_cppcheck: 
	@for src in $(SRC_FILES); do cppcheck $$src; done
	@cppcheck main.c

check: all_cppcheck valgrind timeExecute
