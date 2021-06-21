# SRES: SSH via a Reversed Socket connection

Reach a NAT/Firewalled machine from the Internet by initiating a socket connection to another machine in the Internet, and tunneling SSH session back through sockets. The idea is the same as in the well-known [Reverse-Shell](https://github.com/swisskyrepo/PayloadsAllTheThings/blob/master/Methodology%20and%20Resources/Reverse%20Shell%20Cheatsheet.md) tricks, but tailored to the SSH connection.

## Building

Prerequisite: MinGW or MSYS2 compiler toolchain.

```
git clone https://github.com/dev-frog/C-Reverse-Shell.git
cd C-Reverse-Shell
mkdir build
cd build
cmake ..
make
```  

## Testing

On the NAT/Firewalled machine:

```
./sres-host 192.168.1.3 4444
```

On the Internet machine:

```
./sres-remote 4444
ssh localhost -p 4445
```

