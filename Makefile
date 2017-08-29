CC := $gcc
CFLAGS := -Wall -Werror -g -pthread	\
	-I src/include

CUR_DIR := $(shell pwd)
OBJ_DIR := $(CUR_DIR)/obj

vpath %.o $(OBJ_DIR)

DIRS := src/netif	\
        src/core

SRC := $(foreach n, $(DIRS), $(wildcard $(n)/*.c))


OBJS := $(patsubst src/%.c, obj/%.o, $(SRC))

tcp-ip: $(OBJS)
	$(CC) $(CFLAGS) -o tcpip $(OBJS)
	
$(OBJS):obj/%.o:src/%.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	find . -name *.o | xargs rm -f
	rm tcpip
