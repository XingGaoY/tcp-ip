CC := $gcc
CFLAGS := -Wall -g	\
	-I src/include

OBJS = src/netif/dev_setup.o	\
       src/netif/tun.o          \
       src/tun_dev.o

tcp-ip: $(OBJS)
	$(CC) -o tcpip $(OBJS)
	
$(OBJS):%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	find . -name *.o | xargs rm -f
	rm tcpip
