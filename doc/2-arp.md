# ARP: Address Resolution Protocol
ARP provide a mapping between the two different forms of addresses: 32-bit IP addresses and whatever type of address the data link uses.

The network interface has a hardware address(the 48-bit value). Frame exchanged at the hardware level must be addressed to the corect **interface**. Knowing the host's IP is not enough, the dest's hardware addr must be known to send it data. And the ARP is to provide a dynamic mapping between them.

ARP request is an Ethernet frame.
```
32-bit Internet address  
    |        /|\  
    |arp      |rarp  
    |         |  
   \|/        |  
48-bit Ethernet address  
```
ARP provides a **dynamic mapping** from IP address to the corresponding hardware address, which happens automatically.

**Mention the example:**
## Example
Type the command `ftp bsdi`:

- The **application** FTP, calls fun `gethostbyname(3)` to **convert the hostname(bsdi) to its 32-bit ip addr**, in which the function is called a **resolver** in the DNS.
- The FTP client asks its TCP to establish a connection with that IP addr.
- TCP sends a **connection request segment** to the remost host by **sending an IP datagram to its address**.
  - If the destination host is on a locally attached network(Ethernet, tocken ring, etc.), the IP datagram can be sent **directly** to that host.
  - If the destination host is on a remote network, the **IP routing function determines the Internet address of a locally attached next-hop router to send the IP datagram to**.

And here comes the ARP:
- Assuming an Ethernet, the sending host must **convert the 32-bit IP address into a 48-bit Ethernet address**. **A translation is required from the logical Internet address to its corresponding physical hardware addr**. And ARP is **intended for broadcast networks where many host or routers are connected to a single network**.
- ARP broadcast an ARP request, which contains the IP address of the destination host and is the request "if you are the owner of this IP address, please respond to me with your hardware address."
- The dest host's ARP layer receives this broadcast, recognizes that the sender is asking for its hardware address, and replies with a **ARP reply**, which contains IP addr and the corresponding hardware addr.
- The ARP reply is received and the IP datagram that forced the ARP request-reply to be exchanged can now be sent.
- The IP datagram is finally sent to the dest host.

## ARP Packet Format
```c
struct etharp_hdr{
  uint16_t hwtype;
  uint16_t proto;
  uint8_t hwlen;
  uint8_t protolen;
  uint16_t opcode;
  struct eth_addr shwaddr;
  struct ip4_addr2 sipaddr;
  struct eth_addr dhwaddr;
  struct ip4_addr2 dipaddr;
};
```
As it is mentioned, the ARP is a ethernet frame, and here is the ARP packet in a ethernet frame.
- The first two fields with 6 bytes each are the src and dest Ethernet addr. And if the dest is all one bits, it means broadcast addr.
- Then the 2-byte Ethernet frame type, and 0x8086 for an ARP request.
- The hardware and protocol are used to describe the request ARP sent as hardware address(1 for ethernet) and protocol(0x0800 for IP) address in this case.
- Then the two 1-byte fields, hard size and prot size, to specify the sizes in bytes of hardware addr and the protocol addr. And 6 and 4 respectly in the example.
- op specifies whether the operation is an ARP request(a value of 1), ARP reply(2), RARP request(3), or RARP reply(4).
- The next four filds are the sender's hardware address, the sender's protocol address, the target's hardware address, and the target's protocol address

An ARP request fills all the fields **except the target hardware address**. When a system receive an ARP request directed to it, **it fills in its hardware address, swaps the two sender addresses with the two target addresses, sets the op field to 2 and send the reply**.

## Proxy ARP
Proxy ARP lets a router answer ARP requests on one of its networks for a host on another of it network.  
This fools the sender of ARP request into thinking that the router is the destination host.  
Also this hides two physical networks from each other, with a router between the two.

## Implementation
Thus the implementation could be devided into two parts: ARP request and ARP reply.

Mainwhile, we need to maintain to ARP cache a mechanism to update ARP cache is needed, but we omit the most states other than exist or not for now, as no timer is implemented currently.

### Algorithm
```
Receive and parse the ethernet and arp header, find it an ARP request.  
If it is an ARP for us(compare the dst's ip addr and our):
  If the src's ip addr is in the ARP cache:
    (Check the state of this ARP entry)
    Update it(Refresh the timer and the hardware addr, reset the state)
  Else
    Create a new entry, and set the fields
  EndIf

  Send the reply with our hwaddr filled in
Else
  Also check and update the ARP cache
EndIf
```

### Test
Temperarily, there is no frame padding in the program v0.2.

In one terminate, I execute my program. And execute arping in another, I got:
```
root@ubuntu:/home/yxg/Documents/tcp-ip# ./tcpip 
My hardware addr:        0a:0b:0c:0d:0e:0f
My ip addr:              10.0.0.4
=====
Ethernet frame:
ffffffffffff9a25
0853a4ac08060001
0800060400019a25
0853a4acc0a8ca83
ffffffffffff0a00
0004000000000000
=====
ethernet_input:
dest:           ff:ff:ff:ff:ff:ff
src:            9a:25:8:53:a4:ac
type:           0806
Incoming ARP.
hwtype:         1
proto:          0800
hwlen:          6
protolen:       4
opcode:         1
src's hwaddr:   9a:25:08:53:a4:ac
src's ipaddr:   192.168.202.131
dest's hwaddr:  ff:ff:ff:ff:ff:ff
dest's ipaddr:  10.0.0.4
Updating arp table...
New arp entry index: 0
ARP for us, sending a response...
=====
Ethernet frame:
0a0b0c0d0e0f9a25
0853a4ac08060001
0800060400019a25
0853a4acc0a8ca83
0a0b0c0d0e0f0a00
0004000000000000
=====
ethernet_input:
dest:           0a:0b:0c:0d:0e:0f
src:            9a:25:08:53:a4:ac
type:           0806
Incoming ARP.
hwtype:         1
proto:          0800
hwlen:          6
protolen:       4
opcode:         1
src's hwaddr:   9a:25:08:53:a4:ac
src's ipaddr:   192.168.202.131
dest's hwaddr:  0a:0b:0c:0d:0e:0f
dest's ipaddr:  10.0.0.4
Updating arp table...
Found arp entry index: 0
ARP for us, sending a response...
```
```
yxg@ubuntu:~$ arping -I tap0 10.0.0.4
ARPING 10.0.0.4 from 192.168.202.131 tap0
Unicast reply from 10.0.0.4 [0A:0B:0C:0D:0E:0F]  1.537ms
Unicast reply from 10.0.0.4 [0A:0B:0C:0D:0E:0F]  1.161ms
```
```
root@ubuntu:/home/yxg# tcpdump -i tap0
tcpdump: verbose output suppressed, use -v or -vv for full protocol decode
listening on tap0, link-type EN10MB (Ethernet), capture size 262144 bytes
20:45:07.070662 ARP, Request who-has 10.0.0.4 (0a:0b:0c:0d:0e:0f (oui Unknown)) tell 192.168.202.131, length 28
20:45:07.071012 ARP, Reply 10.0.0.4 is-at 0a:0b:0c:0d:0e:0f (oui Unknown), length 28
```
I slightly modified the output to make the result shorter and better to read.(I will change the printf format in the codes)

First, in the tcpip, we can see the ARP request is parsed correctly.  
And as I mannually set the program's IP address as 10.0.0.4, it regards the received ARP request for it.  
When the first time it received the request, the sender's IP addr is not in the ARP cache, and the program add it to a new entry.  
In the ethernet header, the dst hardware address is ff:ff:ff:ff:ff:ff, identified that the ARP request is broadcasted.
The second time it received the ARP request, it finds an entry with the IP address in the cache. Then it simply update the hardware address with the new one.  
And now, the dst hardware address in the ethernet header is the value I set, because the host also updated its ARP cache after it received the ARP reply.  
And it send it unicast than broadcast.

Second, the `arping` program showed that it had received the unicast ARP reply and the hardware address we set randomly, which means our ARP reply is right.  

Finally, the `tcpdump` result also verified my ARP functioning correctly. The "oui Unknown" after the hardware address is because that the hardware address I set is not in a legal format.

### TODO
- A timer and mechanism to throw away old entry
- Automatic periodly ARP request sending
- Sending ARP request if a frame is going to be sent to a ip addr without hwaddr
