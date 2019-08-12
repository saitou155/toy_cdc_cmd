# for Windows (mingw)
ifeq ($(OS),Windows_NT)
ifeq ($(MSYSTEM),MINGW32)
CFLAGS	= -g -Wall 
LIBS	= 
# Reference compiled libraries in local directory (32bit)
CFLAGS	+= -I./libserialport/win32/include/
LIBS	+= -L./libserialport/win32/lib/ -lserialport -lsetupapi
else
CFLAGS	= -g -Wall 
LIBS	= 
# Reference compiled libraries in local directory (64bit)
CFLAGS	+= -I./libserialport/x64/include/
LIBS	+= -L./libserialport/x64/lib/ -lserialport -lsetupapi
endif
endif

# for Linux
ifeq ($(shell uname),Linux)
CFLAGS	= -g -Wall 
LIBS	= 
# sudo apt-get install libserialport-dev
CFLAGS	+= $(shell pkg-config --cflags libserialport)
LIBS	+= $(shell pkg-config --libs libserialport)
endif

# for MacOSX
ifeq ($(shell uname),Darwin)
CFLAGS	= -g -Wall 
LIBS	= 
# brew install libserialport
CFLAGS	+= -I$(shell brew --prefix libserialport)/include
LIBS	+= -L$(shell brew --prefix libserialport)/lib -lserialport
endif

toy_cdc_cmd:

toy_cdc_cmd.o:	toy_cdc_cmd.c
	$(CC) $(CFLAGS) -c toy_cdc_cmd.c -o toy_cdc_cmd.o

toy_cdc_cmd:	toy_cdc_cmd.o
	$(CC) $(CFLAGS) toy_cdc_cmd.o -o toy_cdc_cmd $(LIBS)

clean:
	$(RM) toy_cdc_cmd
	$(RM) toy_cdc_cmd.o
