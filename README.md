# SRES: SSH via a Reversed Socket connection

Reach a NAT/Firewalled machine from the Internet by initiating a socket connection to another machine in the Internet, and tunneling SSH session back through sockets. The idea is the same as in the well-known [Reverse-Shell](https://github.com/swisskyrepo/PayloadsAllTheThings/blob/master/Methodology%20and%20Resources/Reverse%20Shell%20Cheatsheet.md) tricks, but tailored to the SSH connection.

## Building

Prerequisite on Windows: MinGW or MSYS2 compiler toolchain.

```
git clone https://github.com/dmikushin/sres.git
cd sres
mkdir build
cd build
cmake ..
make
```  

## Testing

On the NAT/Firewalled machine (127.0.0.1 can be a different host in the firewalled intranet):

```
./sres-host 192.168.1.3 4444 127.0.0.1 22
```

On the Internet machine:

```
./sres-remote 4444
ssh localhost -p 4445
```

## Equivalent solution with two SSH tunnels

Requires SSH to a remote machine in the Internet:

```
ssh -L 4444:localhost:22 127.0.0.1 -N
ssh -R 4445:localhost:4444 username@192.168.1.3 -N
```

