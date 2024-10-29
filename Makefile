CC := gcc
CFLAGS := -Wall -Wextra -Werror
TARGET := Aflelou
OBJS := aflelou.o Pretreatment/pretreatment.o Rotate/rotate.o

SDL_LIBS := -lSDL2 -lSDL2_image -lm

GTK_CFLAGS := $(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS := $(shell pkg-config --libs gtk+-3.0)

%.o: %.c
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -o $(TARGET) $(OBJS) $(GTK_LIBS) $(SDL_LIBS)
	./Aflelou

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS)