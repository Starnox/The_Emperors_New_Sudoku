CC  = gcc
CFLAGS = -Wall -Wextra -g
INCLUDES = 

LFLAGS = 

LIBS = -lm

SRCS = tema3.c cJSON.c 

OBJS = $(SRCS:.c=.o)

# Executable file
MAIN = tema3

build: $(MAIN)
	@echo  Your homework has been compiled successfully ^_^

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file)
# (see the gnu make manual section about automatic variables)

run:
	./$(MAIN)

.PHONY: clean

clean:
	rm -f *.o $(MAIN)

