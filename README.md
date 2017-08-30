# tcp-ip
A simple tcp/ip stack implemented with TUN/TAP device in linux user mode.

The notes and my implementation of tcp/ip stack is maintained here.

# Assignments
:new:: working on  
- [x] : implemented but is simplified and lack some feature
- [ ] : planning to do
### General
:new: skbuff
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
- [ ] UDP
  - :new: UDP Header Processing
  - :new: `sk`
  - :new: UDP API for App layer
### Socket
- :new: Socket API

# Implemented Step by Step
I refered to saminiir's step, and mark them with tags 
## v0.1
[Set up TAP device and parse eth-hdr](doc/1-TAP-ethhdr.md)

## v0.2
[Implement ARP, handles ARP req and send reply](doc/2-arp.md) 

## v0.3
[Add IP procedure to receive and send datagram, parse IP-hdr, handles ICMP req and send ICMP reply](doc/3-iphdr-icmp.md)

## Reference
- TCP/IP Illustrated vol.1
- https://github.com/saminiir/level-ip
- lwip
- linux net, [source code](https://github.com/lkl/linux/tree/master/net)
