CC = g++
CFLAGS = -std=c++17 -O1 -fpermissive -fno-strict-aliasing -Wl,--no-as-needed
LDFLAGS = -ldl -pthread -lraylib
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
