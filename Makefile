CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0 webkit2gtk-4.0`
LIBS = `pkg-config --libs gtk+-3.0 webkit2gtk-4.0` -lcmark -lgio-2.0

SRCS = main.c
OBJS = $(SRCS:.c=.o)
	TARGET = markdown_viewer

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

