CC = g++
CFLAGS = -O1 -fpermissive
LDFLAGS = -lraylib
TARGET = graphing
TARGETDIR = dst
TARGETPATH = $(TARGETDIR)/$(TARGET)
SRC = src/*.cpp
INCLUDE = -Iinclude/

all:
	$(CC) $(SRC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -o $(TARGETPATH)
clean:
	rm $(TARGETPATH)
run:
	$(TARGETPATH)
