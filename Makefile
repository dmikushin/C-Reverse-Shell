APP := c-reverse-shell.exe

all: $(APP)

$(APP): reverse.c
	x86_64-w64-mingw32-gcc $< -o $@ -lwsock32 -lwininet


clean:
	rm server $(APP)
