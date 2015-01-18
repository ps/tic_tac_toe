COMPILER = gcc
CCFLAGS = -g

all: TTAI

TTAI: TTAI.o
	$(COMPILER) -o TTAI TTAI.o

TTAI.o: TTAI.c 
	$(COMPILER) $(CCFLAGS) -c TTAI.c

#removes all the object files created in the process
clean:
	rm -rf TTAI *.o
