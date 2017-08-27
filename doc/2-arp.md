# ARP: Address Resolution Protocol
ARP provide a mapping between the two different forms of addresses: 32-bit IP addresses and whatever type of address the data link uses.

The network interface has a hardware address(the 48-bit value). Frame exchanged at the hardware level must be addressed to the corect **interface**. Knowing the host's IP is not enough, the dest's hardware addr must be known to send it data. And the ARP is to provide a dynamic mapping between them.

ARP request is an Ethernet frame.

32-bit Internet address  
    |        /|\  
    |arp      |rarp  
    |         |  
   \|/        |  
48-bit Ethernet address  

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
As it is mentioned, the ARP is a ethernet frame, and here is the ARP packet in a ethernet frame.
- The first two fields with 6 bytes each are the src and dest Ethernet addr. And if the dest is all one bits, it means broadcast addr.
- Then the 2-byte Ethernet frame type, and 0x8086 for an ARP request.
- The hardware and protocol are used to describe the request ARP sent as hardware address(1 for ethernet) and protocol(0x0800 for IP) address in this case.
- Then the two 1-byte fields, hard size and prot size, to specify the sizes in bytes of hardware addr and the protocol addr. And 6 and 4 respectly in the example.
- op specifies whether the operation is an ARP request(a value of 1), ARP reply(2), RARP request(3), or RARP reply(4).
- The next four filds are the sender's hardware address, the sender's protocol address, the target's hardware address, and the target's protocol address

An ARP request fills all the fields **except the target hardware address**. When a system receive an ARP request directed to it, **it fills in its hardware address, swaps the two sender addresses with the two target addresses, sets the op field to 2 and send the reply**.

## Implementation
Thus the implementation could be devided into two parts: ARP request and ARP reply, and maintain an ARP cache.

Mainwhile, a mechanism to update ARP cache is needed, but we omit it for now.

Before that, we need to parse the ARP header in the format mentioned before.
