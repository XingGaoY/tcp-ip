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
Here is the IP header format:
- 4-bit `version`, in IPv4 it is 4 of course.
- 4-bit `header length` is the number of 32-bit words in the header, and since it is a 4-bit field, 
it linits the header to 15 32-bit words. The normal value is 5.
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
