EXE=procs
OBJS=fork.o exec.o main.o parse.o
EXE2=sigs
OBJS2=sigs.o
EXE3=files
OBJS3=files.o

# set the default executable target to build

default: $(EXE) $(EXE2) $(EXE3)

# compiler/linker settings

CC=gcc
CFLAGS=-g -O0 -Wall --std=c99 -pedantic -Wextra -m32
# OS X linker option to turn off ASLR
#LDFLAGS=-g -O0 -m32 -Wl,-no_pie
LDFLAGS=-g -O0 -m32

# build targets

#build:
#	mkdir build

#BUILD=$(addprefix build/,main.o $(MODS))

$(EXE): $(OBJS)
	@rm -f $(EXE)
	$(CC) $(LDFLAGS) -o $(EXE) $^ $(LIBS)

$(EXE2): $(OBJS2)
	@rm -f $(EXE2)
	$(CC) $(LDFLAGS) -o $(EXE2) $^ $(LIBS)

$(EXE3): $(OBJS3)
	@rm -f $(EXE3)
	$(CC) $(LDFLAGS) -o $(EXE3) $^ $(LIBS)

#build/%.o: src/%.c
%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(EXE) $(OBJS) $(EXE2) $(OBJS2) $(EXE3) $(OBJS3)
	rm -rf build

.PHONY: default clean

