# tcp-ip
A simple tcp/ip stack implemented with TUN/TAP device in linux user mode.

The notes and my implementation of tcp/ip stack is maintained here.

# Implemented Step by Step
I refered to saminiir's step, and mark them with tags 
## v0.1
[Set up TAP device and parse eth-hdr](doc/1-TAP-ethhdr.md)

## v0.2
[Implement ARP, handles ARP req and send reply](doc/2-arp.md) 

## v0.3
[Add IP procedure to receive and send datagram, parse IP-hdr, handles ICMP req and send ICMP reply](doc/3-iphdr-icmp.md)

## v0.4

[UDP and sock](doc/4-1-UDP-sock.md)  
[socket and socket API(1)](doc/4-2-socket-API-1.md)

# Assignments
:new:: working on  
- [x] : implemented but is simplified and lack some feature
- [ ] : planning to do
### General
- [x] skbuff
### Net Device
- [x] TAP/TUN
### Link layer
- [x] Ethernet
  - [x] Ethernet Header Processing(rcv & xmit)
- [x] ARP
  - [x] ARP Header Processing(rcv & xmit)
  - [x] ARP cache(no timer, only a static table)
### Network layer
- [x] IP
  - [x] IP Header Processing(rcv & xmit)
- [ ] ICMP
  - [x] ICMP header Processing(rcv & xmit)
  - [x] ICMP request and reply(able to answer `ping` program)
### Transport layer
- [x] UDP
  - [x] UDP Header Processing
  - [x] UDP recv and xmit
  - [x] UDP API for App layer
### Socket
- :new: Socket API
  - [x] `socket()`
  - [x] `bind()`
  - [x] `recv()`
  - [x] `send()`

## Reference
- TCP/IP Illustrated vol.1
- https://github.com/saminiir/level-ip
- lwip
- linux net, [source code](https://github.com/lkl/linux/tree/master/net)
- sock program for test, http://www.icir.org/christian/sock.html
