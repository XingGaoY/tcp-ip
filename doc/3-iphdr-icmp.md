# IP Internet Protocol
IP is the protocol in the network layer handling the movement of packets around the network.

IP provedes an **unreliable and connectless** datagram delivery service.
- Unreliable: **no guarantee that an IP datagram successfully gets to its destination**. But IP
provide its best effort. When something goes wrong, such as a router temporarily running out 
of buffers, IP simply throw away the datagram and try to send an ICMP message back to its source.
- Connectless: IP does **not maintain any state information about successive datagrams**. 
Each datagram is handled independently from all other datagrams, which also means IP datagrams 
can get delivered **out of order**.

## IP header
```c
struct ip_hdr{
  uint8_t _v_hl;
  uint8_t _tos;
  uint16_t _len;
  uint16_t _id;
  uint16_t _offset;
  uint8_t _ttl;
  uint8_t _proto;
  uint16_t _chksum;
  struct ip4_addr src;
  struct ip4_addr dest;
  char data[];
};
```
Here is the IP header format:
- 4-bit `version`, in IPv4 it is 4 of course.
- 4-bit `header length` is the number of 32-bit words in the header, and since it is a 4-bit field, 
it limits the header to 15 32-bit words. The normal value is 5. We can use it to find the start of 
data.
- 8-bit `type-of-service`(TOS) is composed of a 3-bit precedence field(ignored today), 4 TOS bits 
and an unused bit that must be 0. The 4 TOS bits are: minimize delay, maximize throughput, maximize 
reliability and minimize monetory cost. And only one bit of them can be turned on. If all set 0, it 
is a normal service. The TOS feature is not supported by most TCP/IP implementations today.
- 16-bit `total length` is the total length of the IP datagram in bytes, and limits the IP datagram 
to 65535 bytes. It changes when fragmented. As ethernet padding frame to minimum length and an IP 
datagram can be smaller, this field is need to find the end of the datagram.
- 16-bit `identification` identifies each datagram sent by a host, normally increments by one each 
time a datagram is sent.
- 3-bit flags and 13-bit fragment offset will be explained in IP fragmentation.
- 8-bit `time-to-live`(TTL) sets **the upper limit on the number of routers through which a datagram 
can pass**, limits the life time of the datagram. It is initialized by the sender to some value(often 
32 or 64 and decrement by one every router that handles the datagram. The datagram is thrown away when 
TTL reachs 0, and a ICMP is sent to the source.
- 8-bit `protocol` identifies the protocol the data demultiplexed by IP: TCP, UCP, etc.
- 16-bit `header checksum` is calculated over the IP header only, the one's complement sum of the header. 
Since the receiver's checksum contains the checksum stored by the header, the receiver's checksum is all 
one bits if nothing in the header was modified. If not, the datagram is discarded silently.
- Also source and destination IP addr are contained.

## IP procedure
When the network interface parses an ethernet frame, and finds it a IP datagram, it sends the datagram to 
the IP layer. Then IP parses the IP header, and check some of the value(length, checksum, etc).  
Then IP determains which upper layer to send the datagram with the protocol contained in the IP header.  
After that, the upper layer might want to send reply and all it has is the destination's IP address, it will 
send them to the IP layer. IP layer will fill in the IP header, check the IP routing table to find the gate, 
and pass it to ethernet ARP. ARP will check the IP address, if it is in local network, ARP will search the 
ARP cache to find the hardware address and send it through the network interface.

# ICMP Internet Control Message Protocol
ICMP is considered part of the IP layer, which is transmitted within IP datagrams, **to communicates 
error messages and other conditions that require attention**.  
ICMP message is divided into two kinds: a query or an error message. An ICMP error message is never 
generated in response to an ICMP error message.

## ICMP format
- an 8-bit type and 8-bit code field to specify the ICMP type and condition
- 16-bit checksum of the entire ICMP message, calculated with the same algorithm with IP. **Notice it is 
the entire message, ICMP is padded to meet the lower bound of IP datagram, and the padding bit is covered 
in the checksum**

## Ping Program
The Ping program sends an ICMP echo request message to a host, expecting an ICMP echo reply to be return.
And here is the ICMP echo request and reply message format:
```c
struct icmp_echo_hdr{
  uint8_t type;
  uint8_t code;
  uint16_t chksum;
  uint16_t id;
  uint16_t seqno;
};
```
All the field is same with general ICMP message, except the `identifier` to identify the process ID of sending 
process, and the `seqno` to mark the sequence of ICMP request, it is initialed with 0 and is incremented every 
time a new echo request is sent.

## Test
Execute the program and `ping` in another terminal, we get the following result:
```
root@ubuntu:/home/yxg/Documents/tcp-ip# ./tcpip
My hardware addr:	a:b:c:d:e:f
My ip addr:		10.0.0.4
=====
Ethernet frame:

0a0b0c0d0e0f4ee5
2de0391f08004500
0054beec40004001
e68cc0a8ca830a00
00040800d8ef7762
00017701a4590000
0000ca7e03000000
0000101112131415
161718191a1b1c1d
1e1f202122232425
262728292a2b2c2d
2e2f303132333435
3637000000000000
0000000000000000
0000000000000000
0000000000000000
=====
Received an IP datagram:
version:				4
header length:				5
TOS:					00
total length:				0054
identification:				ecbe
offset:					0040
TTL:					40
proto:					01
header checksum:			8ce6
src's ip: 				192.168.202.131
dst's ip:				10.0.0.4
checksum:				00
Received an icmp echo
type:					08
code:					00
checksum:				efd8
identifier:				6277
seqnum:					0100
checksum calculated:			0000
Found the dst hwaddr in ARP cache:	4e:e5:2d:e0:39:1f
```
```
root@ubuntu:/home/yxg/Documents/tcp-ip# tcpdump -i tap0
tcpdump: verbose output suppressed, use -v or -vv for full protocol decode
listening on tap0, link-type EN10MB (Ethernet), capture size 262144 bytes
19:41:43.229146 ARP, Request who-has 10.0.0.4 tell 192.168.202.131, length 28
19:41:43.231262 ARP, Reply 10.0.0.4 is-at 0a:0b:0c:0d:0e:0f (oui Unknown), length 28
19:41:43.231288 IP 192.168.202.131 > 10.0.0.4: ICMP echo request, id 30562, seq 1, length 64
19:41:43.233604 IP 10.0.0.4 > 192.168.202.131: ICMP echo reply, id 30562, seq 1, length 64
19:41:44.231682 IP 192.168.202.131 > 10.0.0.4: ICMP echo request, id 30562, seq 2, length 64
19:41:44.232199 IP 10.0.0.4 > 192.168.202.131: ICMP echo reply, id 30562, seq 2, length 64
```
```
yxg@ubuntu:~/Documents/lwip$ ping 10.0.0.4
PING 10.0.0.4 (10.0.0.4) 56(84) bytes of data.
64 bytes from 10.0.0.4: icmp_seq=1 ttl=63 time=4.53 ms
64 bytes from 10.0.0.4: icmp_seq=2 ttl=63 time=0.561 ms
64 bytes from 10.0.0.4: icmp_seq=3 ttl=63 time=0.510 ms
64 bytes from 10.0.0.4: icmp_seq=4 ttl=63 time=1.38 ms
64 bytes from 10.0.0.4: icmp_seq=5 ttl=63 time=0.651 ms
^C
--- 10.0.0.4 ping statistics ---
5 packets transmitted, 5 received, 0% packet loss, time 4003ms
rtt min/avg/max/mdev = 0.510/1.528/4.538/1.537 ms
```

First, the result of paring the IP and ICMP message is showed.

And lets check the process, as there is no 10.0.0.4 entry in ARP cache, the host send an ARP 
request first to get the hardware address of it as the last chapter. Then it sends the ICMP 
echo request, when the server find the message is for them, it sends an ICMP echo back.

All the IP header is maintained except the exchange of src and dst address in the program and 
the new checksum of course.
And the type of ICMP is changed from 0x08 to 0x00. As we need to keep the sequence number and id, 
all the IP datagram is send to the ICMP procedure to keep the implementation is.

# TODO
- some check of IP header and ICMP message
- IP routing mechanism
- ICMP types left to be handled and ICMP query
