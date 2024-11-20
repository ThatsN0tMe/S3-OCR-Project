CC := gcc
CFLAGS := -Wall -Wextra
TARGET := Afflelou
OBJS := afflelou.o Pretreatment/pretreatment.o Rotate/rotate.o Interface/Interface.o Detection/LineDetection.o Detection/DrawLine.o Detection/Detection.o

SDL_LIBS := -lSDL2 -lSDL2_image -lm

GTK_CFLAGS := $(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS := $(shell pkg-config --libs gtk+-3.0)

%.o: %.c
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -o $(TARGET) $(OBJS) $(GTK_LIBS) $(SDL_LIBS)
	./Afflelou

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS)