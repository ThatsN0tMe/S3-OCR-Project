CC := gcc
CFLAGS := -Wall -Wextra
TARGET := Afflelou
OBJS := afflelou.o Functions.o Solver/solver.o Pretreatment/pretreatment.o Pretreatment/image_history.o Rotate/rotate.o Rotate/autoRotate.o Interface/Interface.o Detection/Cutting.o Detection/LetterDetection.o Detection/WordDetection.o Detection/LineDetection.o Detection/GridDetection.o Detection/DrawShapes.o Detection/Detection.o

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