# tcp-ip
A simple tcp/ip stack implemented with TUN/TAP device in linux user mode.

The notes and my implementation of tcp/ip stack is maintained here.

# Implemented Step by Step
I refered to saminiir's step, and mark them with tags 
## v0.1
[Set up TAP device and parse eth-hdr](doc/1-TAP-ethhdr.md)

## v0.2
[Implement ARP, handles ARP req and send reply](doc/2-arp.md) 

## Reference
- TCP/IP Illustrated vol.1
- https://github.com/saminiir/level-ip
- lwip
