# UDP User Datagram Protocol
UDP is a simple, datagram-oriented, transport layer protocol. It is connectless.  
UDP provides no reliability: no guarantee the datagrams ever reach their destination.  
When the datagram exceeds the netif's MTU, the IP datagram is fragmented.

## UDP Header
```c
struct udp_hdr{
  uint16_t src;
  uint16_t dest;
  uint16_t len;
  uint16_t chksum;
};
```
- 16-bit source and destination port number, to identify the source and destination 
process, both TCP and UDP use dest port number to demultiplex incoming IP datagram.
- 16-bit length of UDP head and UDP data, which is redundant with data length in IP 
header.
- 16-bit optional checksum, 0 indicates the sender hasn't calculated it. When it is 
calculated, a pseudo header is attached to its head for computation. The purpose is 
to let UDP double-check that data has arrived at the correct destination. When a 
checksum error is met, the datagram is discarded silently.

# sk_buff
The sk_buff is the structure that carrying data frame and some information added during 
it transfered up or down layers in linux. Similar with pbuf in lwIP and mbuf in FreeBSD.

## Linux network data structures

In linux the network layers are managed in such interfaces:
- socket: holds the protocol(PF_INET, PF_UNIX, etc), the communication type(SOCK_STREAM, 
SOCK_DGRAM, etc.) of the process. And the socket corresponds to sock, the control block. 
In linux, socket is bound with file system, and I ignored it in my implementation.
- sock: allocated after `bind()`, contains the specific transport layer protocol and basic 
info for transport layer. Also, it is registered to UDP or TCP to hold the header of sk_buff 
list of recv or xmit. sk_buff are multiplexed and put into correct list of the sock.
- sk_buff: generated after data received from net interface or before transmitted.
```
    process0   process1			    process2   process3
       |          |			       |          |
      \|/        \|/			      \|/        \|/
    socket     socket			    socket     socket
       |          |			       |          |
       |          |			       |          |
      \|/        \|/			      \|/        \|/
 UDP  sock----->sock			 TCP  sock----->sock
       |			      	       |
       |				      ...
      \|/
 recv_queue_head--->sk_buff--->sk_buff--...
```

## sk_buff structure
This is the sk_buff I simplified, it is enough for UDP now, maybe I need to add something for 
TCP:
```
struct sk_buff{
  struct sk_buff *next;
  struct sk_buff *prev;

  struct sk_buff_head *list;

  struct sock *sk;

  void *transport_header;
  void *network_header;
  void *mac_header;

  unsigned int len;		// The data part len in SKB. Changed when SKB transmitted from layer to layer. 

  void *head, *data, *tail, *end;
};

```
The first three pointers are used in sock list to link sk_buff. sk_buff is sorted by there source port number, 
and insert to the recv list held by correct sock.  
`sock sk` is used to find the sock it belongs to, as I mentioned, sk_buff belongs to one specific sock.  
And all the values left are for data tranfer from different layers:  
- the three header pointers point to the headers of the data frame, as the data goes down different layers, 
header will be parsed, and the pointer will record the position where the header starts for later use.
- len record the length of data remains(not parsed yet), changed when the sk_buff xmit up or down different layers.
- the last four pointer record the data space: the start, the start of data(to be transmitted to next layer), 
the end of data and the end of data space. The sk_buff will be allocated with a data space larger than the data 
received from net interface, and data will be copied into this space. Then the pointer `data` will act as a stack: 
moving down the data space pointing to the start position of data to be transmitted to next layer and decrease `len`. 
Similarly, when transmit the data, the sk_buff goes up layers and headers are added at the front of `data`. `data` will 
moving up and increase `len`.
```
head,data--->|-------|	     mac_header,head--->|-------|
             |       |                          |eth hdr|
	     | data  |                  data--->|-------|
	     |       |                          | data  |
     tail--->|-------|                  tail--->|-------|
      end--->|-------|                   end--->|-------|

        len                               len -= sizeof(eth hdr)
```

## sk
sk is the control block of a network transportation, holding the common information of several sk_buff's.
```c
struct sock{
  struct hlist_node sk_node;
  unsigned short sk_type;
  struct proto *sk_prot;

  pthread_spinlock_t rcv_lock;
  pthread_spinlock_t xmit_lock;
  struct sk_buff_head *sk_receive_queue;
  struct sk_buff_head *sk_xmit_queue;
};
```
And different protocols have their own sock: `udp_sock` or `tcp_sock`, with their optional data after the sock.  
In order to keep the function same and avoid reloading, we use `inet_sk()` and `udp_sk()` to get the optional info.
In udp, the optional structure holds the ip and port number of sender and receiver.

# UDP implementation
App should create a socket and bind it to a port before recv or xmit data.

After the IP layer find the datagram is UDP, it transfer the datagram to UDP layer.  
The UDP parse the header and look up the sock list to find the best fit entry with port number and ip addr, of course, 
the best one exactly matches. Then, UDP "pulls" the UDP header from the "data stack" in the sk_buff, and links the sk_buff 
to the recv queue of the sock.  
Here, it is the app's responsibility to manage the recv queue by calling recv().  

Transmit a datagram is similar: get the information from sock to add the UDP header and IP header. And as it is the first 
packet, the ARP cache is empty, and the Thread 2 finds an empty ARP entry, sets its state to "pending", broadcast an ARP request 
for the hardware address of the destination IP and block. Thread 1 will receive the ARP reply and update the ARP cache, it will 
wake up thread 2 when it change the state from pending to stable. Then Thread 2 continues to transmit packets.

## Test
From this version, I changed my tcp-ip to multi-thread: one to listen to the network interface, process it and attach it to 
the recv queue. Another to run the user APP to invoke socket API and doing some transmition. And all the information will be 
printed to ./log.

The app test code is in src/app, which is written in strictly POSICS API with irrelevant domains and protocols removed.
```c
    #include "app.h"
    #include <stdio.h>
    #include <string.h>
    #include <memory.h>

    #define dst_ip 0x83caa8c0 // 192.168.202.131
    #define dst_port 1800

    void udp_server(){
      struct __sockaddr addr;
      char buf[100];  

      printf("UDP server...\n");
      addr.port = 1800;
      int sockfd = raw_socket(_SOCK_DGRAM);
      raw_bind(sockfd, &addr);
      printf("UDP server binded...\n");
      while(1){
        memset(buf, 0, 100);
        int len = raw_recv(sockfd, buf, 100);
        for(int i=0; i<len; i++){
          if(i%8 == 0) printf("\n");
          printf("%c", buf[i]);
        }
      }
    }
```

As we can see, create a socket with `raw_socket()` first, `raw_bind()` it to a specific port and `raw_recv()` data.

Using `./tcpip -u -s` to execute UDP server and `./tcpip -u` for a UDP client. The server will print the data received and the 
client will transmit the data in stdin to the remote server. Also it's up to you to implement test program with the API I give 
in "src/include/app.h"

Of course, I used the sock program from http://www.icir.org/christian/sock.html to act like a "correct" server or client to 
generate packet I want. The usage is similar with the sock program in *TCP/IP Illustrate*.

### Client
Here is the client test, type in tcp-ip and the sock program will print the information it received:
```bash
root@ubuntu:/home/yxg/Documents/tcp-ip# ./tcpip -u
UDP client...
UDP client binded...
a
b
c
hello world!
^c
```
```bash
yxg@ubuntu:~/Documents/tcp-ip$ sock -u -s 1800
a
b
c
hello world!
^C
```
```bash
root@ubuntu:/home/yxg/Documents/lwip# tcpdump -i tap0 -v
tcpdump: listening on tap0, link-type EN10MB (Ethernet), capture size 262144 bytes
18:37:33.659183 ARP, Ethernet (len 6), IPv4 (len 4), Request who-has 192.168.202.131 (Broadcast) tell 10.0.0.4, length 46
18:37:33.659235 ARP, Ethernet (len 6), IPv4 (len 4), Reply 192.168.202.131 is-at 76:c0:6e:fa:77:5a (oui Unknown), length 28
18:37:33.660675 IP (tos 0x0, ttl 32, id 0, offset 0, flags [DF], proto UDP (17), length 30)
    10.0.0.4.1800 > 192.168.202.131.1800: UDP, length 2
18:37:35.690873 IP (tos 0x0, ttl 32, id 256, offset 0, flags [DF], proto UDP (17), length 30)
    10.0.0.4.1800 > 192.168.202.131.1800: UDP, length 2
18:37:38.650754 IP (tos 0x0, ttl 32, id 512, offset 0, flags [DF], proto UDP (17), length 30)
    10.0.0.4.1800 > 192.168.202.131.1800: UDP, length 2
18:37:45.594506 IP (tos 0x0, ttl 32, id 768, offset 0, flags [DF], proto UDP (17), length 41)
    10.0.0.4.1800 > 192.168.202.131.1800: UDP, length 13
tcpdump: pcap_loop: The interface went down
```
### Server
```bash
root@ubuntu:/home/yxg/Documents/tcp-ip# ./tcpip -u -s
UDP server...
UDP server binded...
Received an UDP packet...
a
Received an UDP packet...
b
Received an UDP packet...
c
Received an UDP packet...
hello world!
^C
```
```bash
yxg@ubuntu:~/Documents/tcp-ip$ sock -u 10.0.0.4 1800
a
b
c
hello world!
^C
```
```bash
root@ubuntu:/home/yxg/Documents/lwip# tcpdump -i tap0 -v
tcpdump: listening on tap0, link-type EN10MB (Ethernet), capture size 262144 bytes
18:52:37.474744 ARP, Ethernet (len 6), IPv4 (len 4), Request who-has 10.0.0.4 tell 192.168.202.131, length 28
18:52:37.476103 ARP, Ethernet (len 6), IPv4 (len 4), Reply 10.0.0.4 is-at 0a:0b:0c:0d:0e:0f (oui Unknown), length 46
18:52:37.476113 IP (tos 0x0, ttl 64, id 36018, offset 0, flags [DF], proto UDP (17), length 30)
    192.168.202.131.52116 > 10.0.0.4.1800: UDP, length 2
18:52:38.969569 IP (tos 0x0, ttl 64, id 36366, offset 0, flags [DF], proto UDP (17), length 30)
    192.168.202.131.52116 > 10.0.0.4.1800: UDP, length 2
18:52:40.361457 IP (tos 0x0, ttl 64, id 36495, offset 0, flags [DF], proto UDP (17), length 30)
    192.168.202.131.52116 > 10.0.0.4.1800: UDP, length 2
18:52:46.249781 IP (tos 0x0, ttl 64, id 37205, offset 0, flags [DF], proto UDP (17), length 41)
    192.168.202.131.52116 > 10.0.0.4.1800: UDP, length 13
tcpdump: pcap_loop: The interface went down
```
